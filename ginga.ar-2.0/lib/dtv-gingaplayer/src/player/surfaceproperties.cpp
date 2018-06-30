#include "surfaceproperties.h"
#include "../property/propertyimpl.h"
#include "../player.h"
#include <util/log.h>
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <canvas/surface.h>
#include <boost/bind.hpp>

namespace player {

static bool checkPlan( const std::string &plan ) {
	return !strcmp( "graphic", plan.c_str() ) || !strcmp( "video", plan.c_str() ) || !strcmp( "background", plan.c_str() );
}

SurfaceProperties::SurfaceProperties( canvas::Canvas *canvas )
	: _canvas(canvas), _bounds(0,0,-1,-1)
{
	_surface = NULL;
	_zIndex  = 0;
	_plan = "graphic";
}

SurfaceProperties::~SurfaceProperties( void )
{
	assert(!_surface);
}

void SurfaceProperties::onSizeChanged( const OnSizeChanged &callback ) {
	_onSizeChanged = callback;
}

void SurfaceProperties::onPositionChanged( const OnPositionChanged &callback ) {
	_onPositionChanged = callback;
}

void SurfaceProperties::onResizeChanged( const OnResizeChanged &callback) {
	_onResizeChanged = callback;
}

canvas::Canvas *SurfaceProperties::canvas() {
	return _canvas;
}

bool SurfaceProperties::createSurface() {
	canvas::Rect tmp(_bounds);
	calculateBounds(tmp);
	return createSurface( tmp );
}

void SurfaceProperties::destroy() {
	canvas()->destroy( _surface );
}

bool SurfaceProperties::createSurface( const canvas::Rect &rect ) {
	LDEBUG("SurfaceProperties", "Creating surface: pos=(%d,%d), size=(%d,%d)", rect.x, rect.y, rect.w, rect.h);
	
	_surface = canvas()->createSurface( rect );
	if (_surface) {
		_surface->autoFlush(true);
		return true;
	}
	return false;
}

canvas::Surface *SurfaceProperties::surface() const {
	return _surface;
}

void SurfaceProperties::registerProperties( Player *player ) {
	{	//	Add bounds
		PropertyImpl<canvas::Rect> *prop=new PropertyImpl<canvas::Rect>( true, _bounds );
		prop->setCheck( boost::bind(&check::bounds,canvas(),_1) );
		prop->setApply(	boost::bind(&SurfaceProperties::applyBounds,this,prop) );
		player->addProperty( property::type::bounds, prop );
	}
	{	//	Add zIndex
		PropertyImpl<int> *prop=new PropertyImpl<int>( false, _zIndex );
		prop->setCheck( boost::bind( std::greater_equal<int>(), _1, 0) );
		prop->setApply( boost::bind(&SurfaceProperties::applyZIndex,this) );
		player->addProperty( property::type::zIndex, prop );
	}
	{	//	Add Id
		PropertyImpl<std::string> *prop=new PropertyImpl<std::string>( true, _id );
		prop->setApply( boost::bind(&SurfaceProperties::applyId,this) );
		player->addProperty( property::type::id, prop );
	}
	{	//	Add plan
		PropertyImpl<std::string> *prop=new PropertyImpl<std::string>( false, _plan );
		prop->setCheck( boost::bind( &checkPlan, _1) );
		prop->setApply( boost::bind(&SurfaceProperties::applyPlan,this) );
		player->addProperty( property::type::plan, prop );
	}
}

void SurfaceProperties::applyId() {
	std::string id = _id;
	LDEBUG("SurfaceProperties", "apply Id, value=%s", id.c_str());
	surface()->setBelonging( id );
}

void SurfaceProperties::applyBounds(PropertyImpl<canvas::Rect> *prop) {
	bool needRefresh=false;
	
	//	Get new bounds
	canvas::Rect newBounds(_bounds);
	calculateBounds(newBounds);

	LDEBUG("SurfaceProperties", "apply bounds: (%d,%d,%d,%d)", newBounds.x, newBounds.y, newBounds.w, newBounds.h);

	{	//	Compare size
		const canvas::Size newSize( newBounds );
		const canvas::Size &curSize=surface()->getSize();

		if (newSize != curSize) {
			//	Create a new surface from current
			surface()->resize( newSize );
			needRefresh = true;

			//	Notify to dependents
			if (!_onSizeChanged.empty()) {
				_onSizeChanged( newSize );
			}
		}
	}

	{	//	Compare location
		const canvas::Point newPoint( newBounds );
		const canvas::Point &curPoint=surface()->getLocation();
		if (newPoint != curPoint) {
			surface()->setLocation( newPoint );

			//	Notify to dependents
			if (!_onPositionChanged.empty()) {
				_onPositionChanged( newPoint );
			}			
		}
	}

	if (!_onResizeChanged.empty()) {
		_onResizeChanged( newBounds );
	}			

	//	Refresh if necesary
	prop->setNeedResfresh(needRefresh);
}

void SurfaceProperties::calculateBounds( canvas::Rect &/*bounds*/ ) {
}

const canvas::Rect &SurfaceProperties::bounds() const {
	return _bounds;
}

void SurfaceProperties::applyZIndex() {
	LDEBUG("SurfaceProperties", "apply zIndex, value=%d", _zIndex);
	int tmp=_zIndex;
	getZIndex(tmp);
	surface()->setZIndex(tmp);
}

void SurfaceProperties::applyPlan() {
	LDEBUG("SurfaceProperties", "apply plan, value=%s", _plan.c_str());
	std::string tmp=_plan;
	surface()->setPlan(tmp);
}

void SurfaceProperties::getZIndex( int &zIndex ) {
	zIndex *=2;
}

int SurfaceProperties::zIndex() const {
	return _zIndex;
}

}
