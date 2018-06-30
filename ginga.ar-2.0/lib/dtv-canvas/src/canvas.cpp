#include "canvas.h"
#include "surface.h"
#include "window.h"
#include "dummy/canvas.h"
#include "generated/config.h"
#if SYS_CANVAS_USE_CAIRO
#	include "impl/cairo/canvas.h"
#endif
#include <util/cfg/configregistrator.h>
#include <util/mcr.h>
#include <util/log.h>
#include <boost/foreach.hpp>
#include <boost/math/special_functions/round.hpp>
#include <limits.h>

namespace canvas {

REGISTER_INIT_CONFIG( gui_canvas ) {
	root().addNode( "canvas" )
#if SYS_CANVAS_USE_CAIRO
	.addValue( "use", "Canvas to instance", std::string("cairo") )
#else
	.addValue( "use", "Canvas to instance", std::string("dummy") )
#endif

	.addValue( "joinPercentage", "Extra area that can be used when joining dirty regions", 0.2 )

	.addNode( "showBlits" )
		.addValue( "enabled", "Delimit blitted regions", false )
		.addValue( "track", "Number of flush to trak (max=5)", 5 );

	root()("canvas").addNode( "size" )
		.addValue( "width", "Canvas width", 720 )
		.addValue( "height", "Canvas height", 576 );
}

Canvas *Canvas::create() {
	std::string use = util::cfg::getValue<std::string>("gui.canvas.use");

	LINFO("Canvas", "Using canvas: %s", use.c_str());

#if SYS_CANVAS_USE_CAIRO
	if (use == "cairo") {
		return new cairo::Canvas();
	}
#endif
	return new dummy::Canvas();
}

Canvas::Canvas()
{
	_win = NULL;
	_inTransaction = 0;
	_showBlittedRegions = false;
	_blittedRegionsSurface = NULL;
	_joinPercentage  = util::cfg::getValue<double>("gui.canvas.joinPercentage");
	_argb = false;
}

Canvas::~Canvas()
{
	if (_surfaces.size()) {
		LWARN("Canvas", "not all surfaces destroyed: size=%d", _surfaces.size() );
	}
	assert(!_win);
}

//	Initialization
bool Canvas::initialize( Window *win ) {
	assert(win);

	_dirtyRegions.push_back(Rect(Point(0,0),_size));

	//	Setup window
	_win = win;

	_showBlittedRegions = util::cfg::getValue<bool>("gui.canvas.showBlits.enabled");
	_nTrackedFlush = util::cfg::getValue<int>("gui.canvas.showBlits.track");
	if (_nTrackedFlush<1 || _nTrackedFlush>5) {
		throw std::runtime_error( "Invalid showBlits.track value. Must be between 1 and 5" );
	}
	if (_showBlittedRegions) {
		_blittedRegionsSurface = createSurface(size());
		_blittedRegionsSurface->setZIndex(INT_MAX);
		_blittedRegionsColors.push_back(Color(255,0,0));
		_blittedRegionsColors.push_back(Color(255,255,0));
		_blittedRegionsColors.push_back(Color(0,255,0));
		_blittedRegionsColors.push_back(Color(0,255,255));
		_blittedRegionsColors.push_back(Color(0,0,255));
	}

	return true;
}

void Canvas::finalize() {
	_win = NULL;
	if (_showBlittedRegions) {
		destroy(_blittedRegionsSurface);
	}
}

Surface *Canvas::createSurface( const Size &size ) {
	Rect rect(0,0,size.w,size.h);
	return createSurface( rect );
}

Surface *Canvas::createSurface( const Rect &rect ) {
	try {
		return addSurface( createSurfaceImpl( rect ) );
	} catch (...) {}
	return NULL;
}

Surface *Canvas::createSurfaceFromPath( const std::string &file ) {
	try {
		return addSurface( createSurfaceImpl( file ) );
	} catch (...) {}
	return NULL;
}

Surface *Canvas::createSurface( ImageData *img ) {
	try {
		return createSurfaceImpl( img );
	} catch (...) {}
	return NULL;
}

const Size &Canvas::size() const {
	return _size;
}

Surface *Canvas::addSurface( Surface *surface ) {
	LDEBUG("Canvas", "Add surface: surface=%p", surface );
	assert(surface);
	_surfaces.push_back( surface );
	return surface;
}

void Canvas::destroy( SurfacePtr &surface ) {
	LDEBUG("Canvas", "Destroy surface: surface=%p", surface );
	assert(surface);
	std::vector<Surface *>::iterator it=std::find(
		_surfaces.begin(), _surfaces.end(), surface );
	if (it != _surfaces.end()) {
		delete (*it);
		_surfaces.erase( it );
	}
	surface = NULL;
}

struct SortSurfaces {
	bool operator()( Surface *s1, Surface *s2 ) const {
		Surface::plan p1 = s1->getPlan();
		Surface::plan p2 = s2->getPlan();
		if (p1 != p2) {
			if (p1 == Surface::video) {
				return true;
			} else if (p2 == Surface::video) {
				return false;
			} else if (p1 == Surface::background) {
				return true;
			} else {
				return false;
			}
		}
		return s1->getZIndex() < s2->getZIndex();
	}
};

const std::vector<Rect> &Canvas::dirtyRegions() const {
	return _dirtyRegions;
}

Surface *Canvas::getSurfaceById( const std::string &id ) {
	LTRACE("Canvas", "getSurfaceById: [%s]", id.c_str());
	//	Invalidate surfaces
	BOOST_FOREACH( Surface *surface, _surfaces ) {
		std::string sId = surface->getBelonging();
		if (!sId.empty() && sId == id) {
			return surface;
		}
	}
	return NULL;
}

void Canvas::invalidateRegion( const Rect &rect ) {
	if (!rect.isEmpty()) {
		std::vector<Rect> toErase;
		std::vector<Rect> toAdd;

		BOOST_FOREACH( const Rect &dirtyRegion, _dirtyRegions ) {
			if (dirtyRegion.includes( rect )) {
				// If rect is already included in any dirty region return
				return;
			} else if ( rect.includes( dirtyRegion ) ) {
				// If rect includes a region, mark to remove it
				toErase.push_back( dirtyRegion );
			} else {
				Rect composedRect = rect.encompass(dirtyRegion);
				Rect intersection;
				rect.intersection(dirtyRegion, intersection);
				int extraJoinedArea = composedRect.area()-rect.area()-dirtyRegion.area()+intersection.area();
				int maxExtraJoinedArea = boost::math::iround(composedRect.area() * _joinPercentage);

				if (extraJoinedArea<=maxExtraJoinedArea) {
					// If the resulting join area of rect and dirtyRegion
					// is smaller enough, add the new composed rect
					toErase.push_back(dirtyRegion);
					toAdd.push_back(composedRect);
				} else if (intersection.area()>0) {
					// Substract dirtyRegion from rect and add resulting
					// rectangles that don't intersect with dirtyRegion
					if (rect.goesInto(dirtyRegion)) {
						toAdd.push_back(rect.cut(intersection));
					} else if (dirtyRegion.goesInto(rect)) {
						toErase.push_back(dirtyRegion);
						toAdd.push_back(dirtyRegion.cut(intersection));
						toAdd.push_back(rect);
					}
				}
			}
		}

		// Remove old regions
		BOOST_FOREACH( const Rect region, toErase ) {
			std::vector<Rect>::iterator it=std::find( _dirtyRegions.begin(), _dirtyRegions.end(), region );
			if (it != _dirtyRegions.end()) {
				_dirtyRegions.erase( it );
			}
		}

		if (toAdd.size()>0) {
			// If the original region has changed invalidate new regions
			BOOST_FOREACH( const Rect& r, toAdd ) {
				invalidateRegion(r);
			}
		} else {
			// else add the original region
			_dirtyRegions.push_back( rect );
		}
	}
}

void Canvas::flush(bool force /*= false*/) {
	if (!_inTransaction || force) {
		flushImpl();
	}
}

void Canvas::beginDraw() {
	_inTransaction++;
}

void Canvas::endDraw() {
	if (_inTransaction > 0) {
		_inTransaction--;
		flush();
	}
}

void Canvas::flushImpl() {
	if (_showBlittedRegions && !_dirtyRegions.empty()) {
		_previousDirtyRegions.push_back(std::vector<Rect>(_dirtyRegions.begin(), _dirtyRegions.end()));
		_blittedRegionsSurface->autoFlush(false);
		if (_previousDirtyRegions.size()>(unsigned)_nTrackedFlush) {
			_previousDirtyRegions.pop_front();
		}
		_blittedRegionsSurface->clear();
		int i = _previousDirtyRegions.size();
		BOOST_FOREACH( std::vector<Rect> tmp,  _previousDirtyRegions ) {
			_blittedRegionsSurface->setColor(_blittedRegionsColors[--i]);
			BOOST_FOREACH( const Rect &dirtyRegion, tmp ) {
				_blittedRegionsSurface->drawRect( dirtyRegion );
			}
		}
		_blittedRegionsSurface->autoFlush(true);
	}

	if (!_dirtyRegions.empty()) {
		win()->renderLayer( NULL, _dirtyRegions );

		//	Clean dirty regions
		_dirtyRegions.clear();
	}
}
	
bool Canvas::saveAsImage( const std::string &file ) {
	Surface *s = _win->getSurface();
	bool result = s->saveAsImage(file);
	destroy(s);
	return result;
}

bool Canvas::equalsImage( const std::string &file ) {
	Surface *s = _win->getSurface();
	bool result = s->equalsImage(file);
	destroy(s);
	return result;
}

Window *Canvas::win() const {
	return _win;
}

void Canvas::reset() {
	_surfaces.clear();
	flush();
}

void Canvas::setARGB( bool argb ) {
	_argb = argb;
}

void Canvas::getSurfacesForBlit(std::vector<Surface *>& toBlit, const Rect &rect) {
	BOOST_FOREACH( Surface *surface, _surfaces ) {
		if (surface->autoFlush() && surface->isVisible() && rect.intersects(surface->getBounds())) {
			toBlit.push_back( surface );
		}
	}

	//	Sort surfaces by zIndex
	std::stable_sort( toBlit.begin(), toBlit.end(), SortSurfaces() );
}

}
