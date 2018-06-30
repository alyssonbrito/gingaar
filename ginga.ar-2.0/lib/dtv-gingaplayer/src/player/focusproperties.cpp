#include "focusproperties.h"
#include "../property/propertyimpl.h"
#include "../player.h"
#include "graphicplayer.h"
#include "../device.h"
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <canvas/surface.h>
#include <util/log.h>
#include <util/cfg/cfg.h>
#include <boost/bind.hpp>
#include "../system.h"
#include "settings.h"

namespace player {

static bool checkSource( const std::string &file ) {
	if (file.empty()) {
		return true;
	}
	return check::fileExists( file );
}

static bool checkTransparencyRange( const float &value, const float &min, const float &max ) {
	return (value >= min && value <= max) || (value == std::numeric_limits<float>().max());
}

static bool checkColor( const std::string &color ) {
	canvas::Color tmp;
	if (color.empty()) {
		return true;
	}
	return canvas::color::get( color.c_str(), tmp );
}

FocusProperties::FocusProperties( canvas::Canvas *canvas, GraphicPlayer* player)
	: SurfaceProperties( canvas )
{
	_focus = false;
	_selected = false;
	_applied = false;	
	_width = std::numeric_limits<int>().max();
	_curWidth = std::numeric_limits<int>().max();
	_borderColor    = player->device()->systemPlayer()->settings()->getProperty( "default.focusBorderColor" );
	_selBorderColor = player->device()->systemPlayer()->settings()->getProperty( "default.selBorderColor" );
	_transparency   = player->device()->systemPlayer()->settings()->getFocusBorderTransparency();
	 if (checkColor( _borderColor ) && checkColor( _selBorderColor ) && checkTransparencyRange( _transparency, 0.0f, 1.0f )) {
		 _borderColor.clear();
		 _selBorderColor.clear();
		 _transparency = std::numeric_limits<float>().max();
	 }

	_player = player;
	_altPlayer = NULL;
	_hasAltSel = false;
}

FocusProperties::~FocusProperties()
{
	delAlt(false);
}

void FocusProperties::registerProperties( Player *player ) {
	SurfaceProperties::registerProperties( player );
	
	{	//	Add focus
		MultiPropertyImpl<bool> *prop=new MultiPropertyImpl<bool>( _applied, false, _focus );
		player->addProperty( property::type::focus, prop );
		prop->setApply( boost::bind(&FocusProperties::drawFocus,this) );	}

	{	//	Add selected
		MultiPropertyImpl<bool> *prop=new MultiPropertyImpl<bool>( _applied, false, _selected );
		prop->setApply( boost::bind(&FocusProperties::drawSelected,this) );
		player->addProperty( property::type::focusSelected, prop );
	}	

	{	//	Add focus border color
		MultiPropertyImpl<std::string> *prop=new MultiPropertyImpl<std::string>( _applied, false, _borderColor );
		prop->setCheck( boost::bind(&checkColor,_1) );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );		
		player->addProperty( property::type::focusBorderColor, prop );
	}

	{	//	Add selected border color
		MultiPropertyImpl<std::string> *prop=new MultiPropertyImpl<std::string>( _applied, false, _selBorderColor );
		prop->setCheck( boost::bind(&checkColor,_1) );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );
		player->addProperty( property::type::selBorderColor, prop );
	}	

	{	//	Add border width
		MultiPropertyImpl<int> *prop=new MultiPropertyImpl<int>( _applied, false, _width );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );		
		player->addProperty( property::type::focusBorderWidth, prop );
	}

	{	//	Add border transparency
		MultiPropertyImpl<float> *prop=new MultiPropertyImpl<float>( _applied, false, _transparency );
		prop->setCheck( boost::bind(&checkTransparencyRange,_1,0.0f,1.0f) );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );		
		player->addProperty( property::type::focusBorderTransparency, prop );
	}

	{	//	Focus source property
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, false, _focusSource );
		prop->setApply( boost::bind(&FocusProperties::drawFocus,this) );
		player->addProperty( property::type::focusSrc, prop );
	}

	{	//	Focus selected source property
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, false, _focusSelSource );
		prop->setApply( boost::bind(&FocusProperties::drawSelected,this) );
		player->addProperty( property::type::focusSelSrc, prop );		
	}

	{	//	Bounds property for Alt player
		PropertyImpl<canvas::Rect> *prop=new PropertyImpl<canvas::Rect>( false, _altBounds );
		prop->setApply( boost::bind(&FocusProperties::applyAltBounds,this) );
		player->addProperty( property::type::bounds, prop );
	}

	{	//	ZIndex property for Alt player
		PropertyImpl<int> *prop=new PropertyImpl<int>( false, _altZIndex );
		prop->setApply( boost::bind(&FocusProperties::applyAltZIndex,this) );
		player->addProperty( property::type::zIndex, prop );
	}
}

void FocusProperties::refresh() {
	LDEBUG("FocusProperties", "Apply focus properties: focus=%d, selected=%d", _focus, _selected);

	if (!surface()) {
		LWARN("FocusProperties", "Surface is not created");
		return;
	}

	std::string borderColor;
	if (_borderColor.empty()) {
		borderColor =  _player->device()->systemPlayer()->settings()->getProperty( "default.focusBorderColor" );
		if (!checkColor( borderColor )) {
			LWARN("FocusProperties", "default focusBorderColor is not correct (focusBorderColor=%s)", borderColor.c_str());
			return;
		}
	} else {
		borderColor = _borderColor;
	}

	std::string selBorderColor;
	if (_selBorderColor.empty()) {
		selBorderColor =  _player->device()->systemPlayer()->settings()->getProperty( "default.selBorderColor" );
		if (!checkColor( _selBorderColor )) {
			LWARN("FocusProperties", "default selBorderColor is not correct (selBorderColor=%s)", selBorderColor.c_str());
			return;
		}
	} else {
		selBorderColor = _selBorderColor;
	}

	surface()->setVisible( _focus );
	if (_focus) {
		surface()->clear();
		if (_selected) {
			drawBorder( selBorderColor );
		}
		else {
			drawBorder( borderColor );
		}
	}
}

void FocusProperties::delAlt( bool setVis ) {
	if (_altPlayer) {
		_altPlayer->finalize();
		delete _altPlayer;
		_altPlayer = NULL;
		_hasAltSel = false;
		if (setVis) {
			_player->surface()->setVisible(_player->isVisible());
		}
	}
}

bool FocusProperties::startAlt( const std::string &src ) {
	if (src.empty()) {
		return false;
	}
	delAlt();
	_altPlayer = static_cast<GraphicPlayer*>(_player->device()->create( src, "", true ));
	if (_altPlayer) {
		if (_altPlayer->initialize()) {
			const canvas::Rect rect = _player->surface()->getBounds();
			_altPlayer->setProperty("src", src);
			_altPlayer->setProperty("bounds", rect);
			_altPlayer->setProperty("zIndex", zIndex());
			if (_altPlayer->play()) {
				_player->surface()->setVisible(false);
				return true;
			}
			_altPlayer->finalize();
		}
		delete _altPlayer;
		_altPlayer = NULL;
	}
	return false;
}

void FocusProperties::drawFocus() {
	if (_hasAltSel)
		return;
	if (_focus) {
		startAlt(_focusSource);
	} else {
		delAlt();
 	}
	refresh();
}

void FocusProperties::drawSelected() {
	if (_selected) {
		_hasAltSel = startAlt( _focusSelSource );
	} else if (_hasAltSel) {
		delAlt();
		if (_focus) {
			startAlt( _focusSource );
		}
	}
	refresh();
}

void FocusProperties::drawBorder( const std::string &strColor ) {
	//	Set color
	canvas::Color color;
	canvas::color::get( strColor.c_str(), color );
	float transparency;
	if (_transparency == std::numeric_limits<float>().max()) {
		transparency = _player->device()->systemPlayer()->settings()->getFocusBorderTransparency();
	} else {
		transparency = _transparency;
	}
	color.alpha = (util::BYTE) (255-(255*transparency));
	surface()->setColor( color );

	int width;
	if (_width == std::numeric_limits<int>().max()) {
		width = _player->device()->systemPlayer()->settings()->getFocusBorderWidth();
	} else {
		width = _width;
	}

	if (width != _curWidth) {
		canvas::Rect newBounds(_altBounds);
		calculateBounds(newBounds);
		const canvas::Size newSize( newBounds );
		const canvas::Size &curSize=surface()->getSize();
		if (newSize != curSize) {
			//	Create a new surface from current
			surface()->resize( newSize );
			const canvas::Point newPoint( newBounds );
			const canvas::Point &curPoint=surface()->getLocation();
			if (newPoint != curPoint) {
				surface()->setLocation( newPoint );
			}
		}
	}

	if (width < 0) {
		//	Draw rectangles
		canvas::Size size = surface()->getSize();
		for (int i=0; i < abs(width); ++i) {
			canvas::Rect border;
			border.x = i;
			border.y = i;
			border.w = size.w-i*2;
			border.h = size.h-i*2;
			surface()->drawRect(border);
		}
	}
	else {
		const canvas::Rect &oBounds = bounds();
		const canvas::Rect &fBounds = surface()->getBounds();

		LDEBUG("FocusProperties", "drawBorder: original=(%d,%d,%d,%d), focus=(%d,%d,%d,%d), width=%d",
			oBounds.x, oBounds.y, oBounds.w, oBounds.h, fBounds.x, fBounds.y, fBounds.w, fBounds.h, width);

		int linesUp = 0;
		int linesDown = 0;
		{	//	Up
			int lines=oBounds.y - fBounds.y;
			if (lines) {
				canvas::Rect rect( 0, 0, fBounds.w, lines );
				surface()->fillRect( rect );
			}
		}

		{	//	Down
			linesUp=oBounds.y - fBounds.y;
			linesDown=(fBounds.h - oBounds.h) - linesUp;
			canvas::Rect rect( 0, fBounds.h-linesDown, fBounds.w, linesDown );
			surface()->fillRect( rect );			
		}		

		{	//	Left
			int lines=oBounds.x - fBounds.x;
			if (lines) {
				canvas::Rect rect( 0, 0 + linesUp , lines, fBounds.h - linesUp - linesDown);
				surface()->fillRect( rect );
			}
		}

		{	//	Right
			int linesLeft=oBounds.x - fBounds.x;
			int linesRight=(fBounds.w - oBounds.w) - linesLeft;
			if (linesRight) {
				canvas::Rect rect( fBounds.w-linesRight, 0 + linesUp, linesRight, fBounds.h - linesUp - linesDown);
				surface()->fillRect( rect );
			}
		}				
	}
}

void FocusProperties::composeSurface( const std::string &imageFile ) {
	canvas::Surface *image = canvas()->createSurfaceFromPath( imageFile );
	const canvas::Size &i_size = image->getSize();
	const canvas::Size &s_size = surface()->getSize();
	surface()->scale( canvas::Rect(0,0,s_size.w,s_size.h), image, canvas::Rect(0,0,i_size.w,i_size.h) );
	canvas()->destroy( image );
}

void FocusProperties::calculateBounds( canvas::Rect &bounds ) {
	int width;
	if (_width == std::numeric_limits<int>().max()) {
		width = _player->device()->systemPlayer()->settings()->getFocusBorderWidth();
	} else {
		width = _width;
	}
	_curWidth = width;
	if (width > 0) {
		const canvas::Size &max=canvas()->size();
		int x = std::max(0,bounds.x-width);
		int y = std::max(0,bounds.y-width);
		
		int linesLeft=bounds.x - x;
		bounds.w = bounds.w+width+linesLeft;
		if (bounds.w+x > max.w) {
			bounds.w = max.w-x;
		}

		int linesUp=bounds.y - y;
		bounds.h = bounds.h+width+linesUp;
		if (bounds.h+y > max.h) {
			bounds.h = max.h-y;
		}

		bounds.x = x;
		bounds.y = y;

		LDEBUG("FocusProperties", "calculateBounds: bounds=(%d,%d,%d,%d)", bounds.x, bounds.y, bounds.w, bounds.h);
	}	
}

void FocusProperties::getZIndex( int &zIndex ) {
	zIndex *= 2;
	zIndex++;
}

void FocusProperties::applyAltBounds() {
	if (_altPlayer) {
		_altPlayer->setProperty("bounds", _altBounds);
	}
}

void FocusProperties::applyAltZIndex() {
	if (_altPlayer) {
		_altPlayer->setProperty("zIndex", _altZIndex);
	}
}

}

