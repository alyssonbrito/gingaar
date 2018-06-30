#include "surface.h"
#include "rect.h"
#include "canvas.h"
#include "system.h"
#include <util/log.h>
#include <assert.h>
#include <boost/foreach.hpp>
#include <stdio.h>
namespace canvas {

Surface::Mirrors Surface::_mirrors;

void Surface::destroy( Surface *s ) {
	Canvas *canvas = s->canvas();
	canvas->destroy( s );
}

Surface::Surface( const Point &pos )
	: _pos(pos)
{
	_visible = true;
	_zIndex = 1;
	_plan = graphic;
	_dirty = false;
	_autoFlush = false;
	_wasSetColorCalled = false;
	_mode = composition::source_over;
	_flushMode = composition::source_over;
	_encoding = charset::ISO_8859_1;
	canvas::color::get( "transparent", _backgroundcolor );
}

Surface::Surface()
{
	_visible = true;
	_zIndex = 1;
	_dirty = false;
	_autoFlush = false;
	_wasSetColorCalled = false;
	_mode = composition::source_over;
	_flushMode = composition::source_over;
	_encoding = charset::ISO_8859_1;
	canvas::color::get( "transparent", _backgroundcolor );
}

Surface::~Surface()
{
	if (!_belonging.empty()) {
		runMirror( _belonging, NULL );
	}
}

void Surface::markDirtySurface( const Rect &rect ) {
    const Point &point=getLocation();
    const Size &size=getSize();
    Rect copy;
    copy.x = point.x + rect.x;
    copy.y = point.y + rect.y;
    copy.w = std::min(std::max(size.w-rect.x,0), rect.w);
    copy.h = std::min(std::max(size.h-rect.y,0), rect.h);
    markDirty( copy );
}

Surface *Surface::createSimilar( const Rect &rect ) {
	Surface *tmp = canvas()->createSurface( rect );
	if (tmp) {
		tmp->setVisible( _visible );
		tmp->setZIndex( _zIndex );
		tmp->autoFlush( _autoFlush );
		tmp->setFont( _font );
		tmp->setColor( _color );
    tmp->setWasSetColorCalled(getWasSetColorCalled());
		tmp->setBackgroundColor( _backgroundcolor );
		tmp->setCompositionMode( _mode );
		tmp->flushCompositionMode( _flushMode );
		tmp->setCharsetEncoding( _encoding );
		tmp->setTextAlign( _textAlign );
		tmp->markDirty();
	}
	return tmp;
}

Surface *Surface::transformWithChromakey( const std::string &chromakey ) {
	return transformWithChromakeyImpl( chromakey );
}

const Font &Surface::getFont() const {
	return _font;
}

void Surface::setDefaultFont() {
	setFont( Font() );
}

std::string Surface::installFont (const std::string &fontFile){
	return installFontFile(fontFile);
}

bool Surface::setFont( const Font &font ) {
	_font = font;
	return applyFont();
}

/****************************** Clip functions ********************************/

bool Surface::setClip( const Rect &rect ) {
    bool check=true;

    check &= ( rect.w>=0 && rect.h>=0 );

    if (check) {
        setClipImpl( rect );
    } else {
        LWARN("Surface", "setClip fail. Invalid bounds rect(x=%d y=%d w=%d h=%d)", rect.x, rect.y, rect.w, rect.h);
    }
    return check;
}

/****************************** Line functions ********************************/

bool Surface::drawLine( int x1, int y1, int x2, int y2 ) {
    int min_x = std::min(x1, x2);
    int min_y = std::min(y1, y2);
    int max_x = std::max(x1, x2);
    int max_y = std::max(y1, y2);

    Rect lineRect(min_x, min_y, max_x-min_x+1, max_y-min_y+1);

	drawLineImpl( x1, y1, x2, y2 );

	markDirtySurface( lineRect );

    return true;
}

/****************************** Rect functions ********************************/

bool Surface::drawRect( const Rect &rect ) {
    bool check=true;

    check &= (rect.w>=0 && rect.h>=0);

    if (check) {
        if (rect.w>0 && rect.h>0) {
            drawRectImpl(rect);
            markDirtySurface( rect );
        }
    } else {
        LWARN("Surface", "drawRect fail. Invalid bounds rect(x=%d y=%d w=%d h=%d)", rect.x, rect.y, rect.w, rect.h);
    }

    return check;
}

bool Surface::fillRect( const Rect &rect ) {
    bool check=true;

    check &= (rect.w>=0 && rect.h>=0);

    if (check) {
        if (rect.w>0 && rect.h>0) {
            fillRectImpl( rect );
            markDirtySurface( rect );
        }
    } else {
        LWARN("Surface", "fillRect fail. Invalid bounds rect(x=%d y=%d w=%d h=%d)", rect.x, rect.y, rect.w, rect.h);
    }

    return check;
}

/*************************** RoundRect functions ******************************/

bool Surface::drawRoundRect( const Rect &rect, int arcW, int arcH ) {
    bool check=true;

    check &= (rect.w>=0 && rect.h>=0);
    check &= (arcW > -1 && arcH > -1);

    if (check) {
        if (rect.w>0 && rect.h>0) {
            if (arcW==0 || arcH==0) {
                drawRectImpl( rect );
            } else {
                drawRoundRectImpl(rect, arcW, arcH);
            }
            markDirtySurface( rect );
        }else{
          LWARN("Surface", "rectangle w or h not >0");
        }
    } else {
        LWARN("Surface", "drawRoundRect fail. Invalid bounds rect(x=%d y=%d w=%d h=%d, arcW=%d, arcH=%d)", rect.x, rect.y, rect.w, rect.h, arcW, arcH);
    }

    return check;
}

bool Surface::fillRoundRect( const Rect &rect, int arcW, int arcH ) {
    bool check=true;

    check &= (rect.w>=0 && rect.h>=0);
    check &= (arcW > -1 && arcH > -1);

    if (check) {
        if (rect.w>0 && rect.h>0) {
            if (arcW==0 ||  arcH==0) {
                fillRectImpl( rect );
            } else {
                fillRoundRectImpl(rect, arcW, arcH);
            }
            markDirtySurface( rect );
        }else{
          LWARN("Surface", "rectangle w or h not >0");
        }
    } else {
	    LWARN("Surface", "fillRoundRect fail. Invalid bounds rect(x=%d y=%d w=%d h=%d, arcW=%d, arcH=%d)",
		    rect.x, rect.y, rect.w, rect.h, arcW, arcH);
    }

    return check;
}

/**************************** Polygon functions ******************************/

bool Surface::drawPolygon( const std::vector<Point>& vertices, bool closed/*=true*/ ) {
    bool check=true;

    check &= vertices.size()>2;

    canvas::Size size = getSize();
    int minX = size.w;
    int minY = size.h;
    int maxX = 0;
    int maxY = 0;
    for(size_t i=0; i<vertices.size() && check; ++i) {
        const Point& p = vertices[i];
	minX = std::min(minX, p.x);
	minY = std::min(minY, p.y);
	maxX = std::max(maxX, p.x);
	maxY = std::max(maxY, p.y);
    }

    Rect polygonRect(Rect(minX, minY, maxX - minX + 1, maxY - minY + 1));

    if (check) {
        drawPolygonImpl( vertices, closed );
        markDirtySurface( polygonRect );
    } else {
        LWARN("Surface", "drawPolygon fail. Invalid vertices");
    }
    return check;
}

bool Surface::fillPolygon( const std::vector<Point>& vertices ) {
    bool check=true;

    check &= vertices.size()>2;

    canvas::Size size = getSize();
    int minX = size.w;
    int minY = size.h;
    int maxX = 0;
    int maxY = 0;
    for(size_t i=0; i<vertices.size() && check; ++i) {
        const Point& p = vertices[i];
	minX = std::min(minX, p.x);
	minY = std::min(minY, p.y);
	maxX = std::max(maxX, p.x);
	maxY = std::max(maxY, p.y);
    }

    Rect polygonRect(minX, minY, maxX - minX + 1, maxY - minY + 1);

    if (check) {
        fillPolygonImpl( vertices );
        markDirtySurface( polygonRect );
    } else {
        LWARN("Surface", "fillPolygon fail. Invalid vertices");
    }
    return check;
}

/**************************** Ellipse functions ******************************/

bool Surface::drawEllipse( const Point &center, int rw, int rh, int angStart, int angStop ) {
    bool check=true;

    Rect ellipseRect(center.x - rw, center.y - rh, rw*2+1, rh*2+1);
    check &= (rw>=0 && rh>=0);

    if (check) {
        if (rw>0 && rh>0) {
            drawEllipseImpl(center, rw, rh, angStart, angStop);
            markDirtySurface(ellipseRect);
        }
    } else {
        LWARN("Surface", "drawEllipse fail. Invalid bounds center(x=%d y=%d) rw=%d rh=%d angStart=%d angStop=%d", center.x, center.y, rw, rh, angStart, angStop);
    }
    return check;
}

bool Surface::fillEllipse( const Point &center, int rw, int rh, int angStart, int angStop ) {
    bool check=true;

    Rect ellipseRect(center.x - rw, center.y - rh, rw*2+1, rh*2+1);
    check &= (rw>=0 && rh>=0);

    if (check) {
        if (rw>0 && rh>0) {
            fillEllipseImpl(center, rw, rh, angStart, angStop);
            markDirtySurface(ellipseRect);
        }
    } else {
        LWARN("Surface", "fillEllipse fail. Invalid bounds center(x=%d y=%d) rw=%d rh=%d angStart=%d angStop=%d", center.x, center.y, rw, rh, angStart, angStop);
    }
    return check;
}

/****************************** Text functions ********************************/

void Surface::replaceIndent( std::string &line ) {
	size_t current = 0;
	size_t idx = line.find('\t');
	while (idx != std::string::npos) {
		line.replace(idx, 1, 4, ' ');
		current = idx + 1;
		idx = line.find('\t', current);
	}
}

void Surface::findLines( const std::string &text, std::vector<std::string> &lines ) {
	std::string line;
	size_t current = 0;
	size_t idx = text.find('\n');
	while (idx!=std::string::npos) {
		if (idx==current) {
			line = "";
			current++;
		} else {
			line = text.substr(current, idx-current);
			replaceIndent(line);
			current = idx + 1;
		}
		lines.push_back(line);
		idx = text.find('\n', current);
	}
	line = text.substr(current);
	replaceIndent(line);
	lines.push_back(line);
}

bool Surface::drawText( const Point &pos, const std::string &text ) {
    bool check=true;
    Size size;
    int ascent = fontAscent();
    int descent = fontDescent();

    check &= measureText(text, size);
    Rect textRect(pos.x, pos.y-ascent+1, size.w, size.h);
    check &= (size.w>=0 && size.h>=0);

    if (check) {
        if (text.length()>0) {
            drawTextImpl(pos, text, ascent );
            markDirtySurface(textRect);
        }
    } else {
        LWARN("Surface", "drawText fail. Invalid bounds pos(x=%d y=%d) ascent=%d descent=%d)", pos.x, pos.y, ascent, descent);
    }
    return check;
}

bool Surface::drawText( const Rect &rect, const std::string &text, Alignment alignment/*=alignment::None*/, int spacing/*=0*/ ) {
    bool check=true;

    check &= pointInBounds(Point(rect.x, rect.y));
    check &= pointInBounds(Point(std::max(rect.x+rect.w-1,0), std::max(rect.y+rect.h-1,0)));
    //  The rest of the alignments are not yet supported.
    check &= (alignment==canvas::alignment::None);

    if (check) {
        if (text.length()>0) {
            drawTextImpl(rect, text, alignment, spacing);
            markDirtySurface( rect );
        }
    } else {
        LWARN("Surface", "drawText fail. Invalid bounds rect(x=%d y=%d, w=%d, h=%d)", rect.x, rect.y, rect.w, rect.h);
    }
    return check;
}

bool Surface::measureText( const std::string &text, Size &size ) {
    if (text.empty()) {
        size.h = 0;
        size.w = 0;
    } else {
        measureTextImpl(text,size);
    }
    return true;
}

/*********************** Pixel manipulation functions *************************/

bool Surface::getPixelColor( const Point &pos, Color &color ) {
	bool check=true;
	check &= pointInBounds( pos );
	if (check) {
		getPixelColorImpl(pos, color);
	} else {
		LWARN("Surface", "getPixelColor fail. Invalid bounds pos(x=%d y=%d)", pos.x, pos.y);
	}
	return check;
}

bool Surface::setPixelColor( const Point &pos, const Color &color ) {
    bool check=true;

    check &= pointInBounds( pos );
    if (check) {
        setPixelColorImpl(pos, color);
        canvas::Rect rect(pos.x, pos.y, 1, 1);
        markDirtySurface(rect);
    } else {
        LWARN("Surface", "setPixelColor fail. Invalid bounds pos(x=%d y=%d)", pos.x, pos.y);
    }
    return check;
}

void Surface::flush() {
	if (_autoFlush) {
		canvas()->flush();
	}
	if (!_belonging.empty()) {
		runMirror( _belonging, this );
	}
}

void Surface::autoFlush( bool inCanvasFlush ) {
	_autoFlush=inCanvasFlush;
	setAutoFlushImpl( inCanvasFlush );
	markDirty();
}

void Surface::setAutoFlushImpl( bool /*inCanvasFlush*/ ) {
}

bool Surface::autoFlush() const {
	return _autoFlush;
}

void Surface::flushCompositionMode( composition::mode mode ) {
	_flushMode = mode;
}

composition::mode Surface::flushCompositionMode() {
	return _flushMode;
}

void Surface::markDirty() {
	Rect dirty = getBounds();
	markDirty( dirty );
}

void Surface::markDirty( const Rect &rect ) {
	if (_autoFlush) {
		markDirtyImpl( rect );
		canvas()->invalidateRegion( rect );
		_dirty=true;
	}
}

void Surface::markDirtyImpl( const Rect &/*rect*/ ) {
}

void Surface::clearDirty() {
	_dirty = false;
}

void Surface::invalidateRegion( const Rect &rect ) {
	if (_autoFlush && !_dirty) {
		Rect bounds = getBounds();
		if (rect.intersects( bounds )) {
			_dirty=true;
		}
	}
}

bool Surface::getDirtyRegion( const Rect &dirtyRegion, Rect &blitRect ) {
	Rect bounds = getBounds();
	return dirtyRegion.intersection( bounds, blitRect );
}

bool Surface::isDirty() const {
	return _dirty;
}

void Surface::setZIndex( int zIndex ) {
	if (_zIndex != zIndex) {
		_zIndex = zIndex;
		setZIndexImpl( zIndex );
		markDirty();
	}
}

void Surface::setZIndexImpl( int /*zIndex*/ ) {
}

int Surface::getZIndex() const {
	return _zIndex;
}

void Surface::setPlan( const std::string &plan ) {
	Surface::plan p = unknown;
	if (plan == "graphic") {
		p = graphic;
	} else if (plan == "video") {
		p = video;
	} else if (plan == "background") {
		p = background;
	}
	if (p != unknown && _plan != p) {
		_plan = p;
		markDirty();
	}
}

Surface::plan Surface::getPlan() const {
	return _plan;
}

void Surface::setColor( const Color &color ) {
	_color = color;
	setColorImpl( _color );
}

bool Surface::getWasSetColorCalled() const{
  APRINT_GRAPHICS("<==> Return[%d]",_wasSetColorCalled);
  return _wasSetColorCalled;
}

void Surface::setWasSetColorCalled(bool was) {
  APRINT_GRAPHICS("<==> was[%d]",was);
  _wasSetColorCalled = was;
}

void Surface::setColorImpl( Color &/*color*/ ) {
}

const Color &Surface::getColor() const {
	return _color;
}

void Surface::setBackgroundColor( const Color &color ) {
	_backgroundcolor = color;
}

const Color &Surface::getBackgroundColor() const {
	return _backgroundcolor;
}

void Surface::setTextAlign( const std::string &align ) {
	_textAlign = align;
}

const std::string &Surface::getTextAlign() const {
	return _textAlign;
}

bool Surface::clear() {
	bool res;
	Size size = getSize();
	Rect bounds(0,0,size.w,size.h);
	res = clear( bounds );
	if (!_belonging.empty()) {
		runMirror( _belonging, NULL );
	}
	return res;
}

bool Surface::clear( const Rect &rect ) {
	bool check=true;

	check &= (rect.w>0 && rect.h>0);

	if (check) {
		composition::mode oldMode = getCompositionMode();
		setCompositionMode(composition::clear);
		fillRectImpl(rect);
		setCompositionMode(oldMode);
		markDirtySurface( rect );
	} else {
		LWARN("Surface", "clear fail. Invalid bounds rect(x=%d y=%d w=%d h=%d)", rect.x, rect.y, rect.w, rect.h);
	}
	return check;
}

void Surface::setVisible( bool visible ) {
	if (visible != _visible) {
		setVisibleImpl( visible );
		markDirty();
	}
	_visible = visible;
}

void Surface::setVisibleImpl( bool /*visible*/ ) {
}

bool Surface::isVisible() const {
	return _visible;
}

void Surface::setLocation( const Point &point ) {
	markDirty();
	_pos.x = point.x;
	_pos.y = point.y;
	setLocationImpl( point );
	markDirty();
}

void Surface::setLocationImpl( const Point &/*point*/ ) {
}

const Point &Surface::getLocation() const {
	return _pos;
}

Rect Surface::getBounds() const {
	Point pos = getLocation();
	Size size = getSize();
	Rect bounds(pos.x, pos.y, size.w, size.h);
	return bounds;
}

bool Surface::equalsImage( const std::string &file ) {
	Surface* image = canvas()->createSurfaceFromPath( file );

	if (!image) {
		return false;
	}
	Size s_size = getSize();
	Size i_size = image->getSize();

	if (s_size.w != i_size.w || s_size.h != i_size.h) {
		canvas()->destroy( image );
		return false;
	} else {
		for (int i = 0; i < s_size.w; ++i) {
			for (int j = 0; j < s_size.h; ++j) {
				if (!comparePixels(Point(i,j), image)) {
					canvas()->destroy( image );
					return false;
				}
			}
		}
	}
	canvas()->destroy( image );
	return true;
}

#define THRESHOLD 20
bool Surface::comparePixels( const Point &pos, Surface *image ) {
	util::DWORD p1, p2;
	bool check=true;

	p1 = getPixel(pos);
	p2 = image->getPixel(pos);
	if (p1!=p2) {
		canvas::Color c1, c2;
		getPixelColor(pos, c1);
		image->getPixelColor(pos, c2);
		if (!c1.equals(c2, THRESHOLD ) ) {
			LINFO("Surface", "Difference found in pixel (%d, %d): s_color=(%d,%d,%d,%d), i_color=(%d,%d,%d,%d).",
				pos.x, pos.y, c1.r, c1.g, c1.b, c1.alpha, c2.r, c2.g, c2.b, c2.alpha );
			check=false;
		}
	}
	return check;
}

bool Surface::pointInBounds( const Point &point ) const {
	Size size = getSize();
	bool check_x = point.x > -1 && point.x < size.w;
	bool check_y = point.y > -1 && point.y < size.h;
	return check_x && check_y;
}

bool Surface::blit( const Point &targetPoint, Surface *srcSurface ) {
	const Size &s=srcSurface->getSize();
	Rect srcRect(0,0,s.w,s.h);
	return blit( targetPoint, srcSurface, srcRect );
}

bool Surface::blit( const Point &targetPoint, Surface *srcSurface, const Rect &source ) {
	bool check=true;

	check &= (source.w>=0 && source.h>=0);

	if (check) {
		blitImpl(targetPoint, srcSurface, source);
		markDirtySurface( Rect( targetPoint.x, targetPoint.y, source.w, source.h ) );
	} else {
		const Rect &bounds=getBounds();
		LWARN("Surface", "blit fail. Invalid bounds: bounds=(%d,%d,%d,%d), targetPoint=(x=%d y=%d) source=(x=%d y=%d w=%d h=%d)",
			bounds.x, bounds.y, bounds.w, bounds.h,
			targetPoint.x, targetPoint.y,
			source.x, source.y, source.w, source.h);
	}
	return check;
}

bool Surface::scale( const Rect &targetRect, Surface *srcSurface, bool flipw /*=false*/, bool fliph /*=false*/ ) {
	const Size &s=srcSurface->getSize();
	Rect srcRect(0,0,s.w,s.h);
	return scale( targetRect, srcSurface, srcRect, flipw, fliph );
}

bool Surface::scale( const Rect &targetRect, Surface *srcSurface, const Rect &source, bool flipw /*=false*/, bool fliph /*=false*/ ) {
	bool check=true;

	check &= (source.w>0 && source.h>0);

	check &= (targetRect.w>0 && targetRect.h>0);

	if (check) {
		scaleImpl( targetRect, srcSurface, source, flipw, fliph );
		markDirtySurface( targetRect );
	} else {
		const Rect &bounds=getBounds();
		LWARN("Surface", "scale fail. Invalid bounds: bounds=(%d,%d,%d,%d), target=(%d,%d,%d,%d) source=(%d,%d,%d,%d)",
			bounds.x, bounds.y, bounds.w, bounds.h,
			targetRect.x, targetRect.y, targetRect.w, targetRect.h,
			source.x, source.y, source.w, source.h);
	}
	return check;
}

Surface *Surface::rotate( int degrees ) {
	Surface *newSurface = NULL;
	if ((degrees%90) != 0) {
		LWARN("Surface", "rotate fail. Degrees must be a multiple of 90 (degrees=%d)", degrees);
	} else {
		while (degrees<0) {
			degrees = degrees + 360;
		}
		newSurface = rotateImpl( degrees%360 );
		markDirtySurface( newSurface->getBounds() );
	}
	return newSurface;
}

bool Surface::resize( const Size &size, bool scaleContent/*=false*/ ) {
	bool check = size.w>0 && size.h>0;
	if (check) {
		markDirty();
		resizeImpl(size, scaleContent);
	} else {
		LWARN("Surface", "resize fail. New size must be greater than 0. newSize=(%d,%d)", size.w, size.h);
	}
	return check;
}

void Surface::setCompositionMode( composition::mode mode ) {
	switch (mode) {
		case composition::source_over:
		case composition::source:
		case composition::clear:
			_mode = mode;
			setCompositionModeImpl( mode );
			break;
		default:
			LWARN("Surface", "setCompositionMode fail. Composition mode not suported!");
			break;
	}
}

void Surface::setCharsetEncoding( charset::encoding encoding ) {
	switch (encoding) {
		case charset::ISO_8859_1:
			_encoding = encoding;
			setCharsetEncodingImpl( encoding );
			break;
		default:
			LWARN("Surface", "setCharsetEncoding fail. Charset encoding not suported!");
			break;
	}
}

charset::encoding Surface::getCharsetEncoding() const {
	return _encoding;
}

composition::mode Surface::getCompositionMode() {
	return _mode;
}

bool Surface::blitEx(const Point & /* targetPoint */, Surface * /* srcSurface */, const Rect & /* sourceRect */, bool, bool) {
	return true;
}

void Surface::MaskCreate(bool /* transitionIn */, void* /* transition */, int /* stepCount */, void* /* anim */) {
}

bool Surface::HasMask(void* /* anim */) {
	return false;
}

void Surface::MaskRemove(void* /* anim */) {
}

void Surface::MaskStepTransition(int /* remainingSteps */, void* /* anim */) {
}

const std::string &Surface::getBelonging() const {
	return _belonging;
}

void Surface::setBelonging( const std::string &belonging ) {
	_belonging = belonging;
}

void Surface::addMirror( const std::string &id, GraphicMirrorCallback mirror ) {
	if (_mirrors.count( id ) != 0) {
		(_mirrors)[id].push_back(mirror);
	} else {
		MirrorsCallbacks callbacks;
		callbacks.push_back(mirror);
		_mirrors.insert(std::make_pair(id, callbacks));
	}
}

void Surface::removeMirror( const std::string &id, void *obj ) {
	if (_mirrors.count( id ) != 0) {
		MirrorsCallbacks &callbacks = (_mirrors)[id];
		if (callbacks.size() > 1) {
			MirrorsCallbacks::iterator it;
			for (it = callbacks.begin(); it != callbacks.end(); it++) {
				GraphicMirrorCallback callback = *it;
				if (callback.first == obj) {
					callbacks.erase(it);
					break;
				}
			}
		} else if (callbacks.size() == 1) {
			GraphicMirrorCallback callback = *callbacks.begin();
			if (callback.first == obj) {
				_mirrors.erase( _mirrors.find( id ) );
			}
		}
	}
}


void Surface::runMirror( const std::string &id, canvas::Surface *surface ) {
	if (_mirrors.count( id ) != 0) {
		MirrorsCallbacks &callbacks = (_mirrors)[id];
		BOOST_FOREACH(GraphicMirrorCallback mirror, callbacks) {
			if (mirror.second) {
				mirror.second( surface );
			}
		}
	}
}

}
