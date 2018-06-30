#include "graphicproperties.h"
#include "../property/propertyimpl.h"
#include "../player.h"
#include <util/log.h>
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <canvas/surface.h>
#include <boost/bind.hpp>

#include <execinfo.h>

namespace player {

template<typename T>
void printBackground (const T & value) {
	if (typeid(std::string) == typeid(value)){
		APRINT_PLAYER("<==> Color[%s]", value.c_str() );
		LDEBUG("GraphicProperties", "Background Color = [%s]", value.c_str() );
	}
}

GraphicProperties::GraphicProperties( canvas::Canvas *canvas )
	: SurfaceProperties( canvas )
{
	_opacity = 1.0f;
	_visible = true;
	_backgroundColor = "transparent";
	_chromakey = "nill";
}

GraphicProperties::~GraphicProperties( void )
{
}

void GraphicProperties::clear() {
	canvas::Surface *s = surface();

	//Get background color
	canvas::Color previousColor = s->getColor();
	canvas::Color backColor;
	canvas::color::get( _backgroundColor.c_str(), backColor );
	s->setColor(backColor);

	canvas::Rect rect = s->getBounds();
	rect.x = 0;
	rect.y = 0;

	s->setCompositionMode( canvas::composition::source );
	s->fillRect( rect );

	s->setCompositionMode( canvas::composition::source_over );
	s->setColor(previousColor);
}

void GraphicProperties::registerProperties( Player *player ) {
	SurfaceProperties::registerProperties( player );

	{	//	Add Opacity
		PropertyImpl<float> *prop=new PropertyImpl<float>( false, _opacity );
		prop->setCheck( boost::bind(&check::range<float>,_1,0.0f,1.0f) );
		prop->setApply( boost::bind(&GraphicProperties::applyOpacity,this) );
		player->addProperty( property::type::opacity, prop );
	}
	{	//	Add backgroundColor
		PropertyImpl<std::string> *prop=new PropertyImpl<std::string>( true, _backgroundColor );
		prop->setPrintMethod( boost::bind ( &printBackground<std::string>, _1) );
		prop->setCheck( boost::bind(&check::color,_1) );
		prop->setApply( boost::bind(&GraphicProperties::applyBackgroundColor,this) );
		player->addProperty( property::type::backgroundColor, prop );
	}
	{	//	Add visible
		PropertyImpl<bool> *prop=new PropertyImpl<bool>( false, _visible );
		prop->setApply( boost::bind(&GraphicProperties::applyVisible,this) );
		player->addProperty( property::type::visible, prop );
	}
	{	//	Add rgbChromakey
		PropertyImpl<std::string> *prop=new PropertyImpl<std::string>( true, _chromakey );
		prop->setPrintMethod( boost::bind ( &printBackground<std::string>, _1) );
		prop->setApply( boost::bind(&GraphicProperties::applyChromakey,this) );
		player->addProperty( property::type::rgbChromakey, prop );
	}
}

bool GraphicProperties::isVisible() const {
	return _visible;
}

std::string GraphicProperties::rgbChromakey() const {
	return _chromakey;
}

void GraphicProperties::applyOpacity() {
	LDEBUG("GraphicProperties", "apply opacity, value=%f", _opacity);
	surface()->setOpacity( (util::BYTE) (255 * _opacity) );
}

void GraphicProperties::applyVisible() {
	LDEBUG("GraphicProperties", "apply visible, value=%d", _visible );
	surface()->setVisible( _visible );
}

void GraphicProperties::applyBackgroundColor() {
	LDEBUG("GraphicProperties", "apply background color, value=%s", _backgroundColor.c_str() );
	canvas::Color c;
	canvas::color::get( _backgroundColor.c_str(), c );
	surface()->setBackgroundColor( c );
	surface()->clear();
}

void GraphicProperties::applyChromakey() {
  APRINT_PLAYER("<==>");
	LDEBUG("GraphicProperties", "apply chromake color, value=%s", _chromakey.c_str() );
  //surface()->transformWithChromakey( _chromakey );
	//surface()->clear();
}

void GraphicProperties::forceTransparentBgColor() {
	LDEBUG("GraphicProperties", "Forcing to set a transparent background." );
	_backgroundColor = "transparent";
	applyBackgroundColor();
}

}
