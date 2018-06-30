#include "textplayer.h"
#include "../property/propertyimpl.h"
#include "../property/forwardproperty.h"
#include <util/log.h>
#include <canvas/rect.h>
#include <canvas/color.h>
#include <canvas/font.h>
#include <canvas/surface.h>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include "../input/input.h"
#include "../device.h"
#include "../system.h"

namespace player {

static bool checkStyle( const std::string &style ) {
	return !strcmp( "normal", style.c_str() ) || !strcmp( "italic", style.c_str() );
}

static bool checkVariant( const std::string &variant ) {
	return !strcmp( "normal", variant.c_str() ) || !strcmp( "small-caps", variant.c_str() );
}

static bool checkWeight( const std::string &weight ) {
	return !strcmp( "normal", weight.c_str() ) || !strcmp( "bold", weight.c_str() );
}

static bool checkColor( const std::string &color ) {
	canvas::Color tmp;
	return canvas::color::get( color.c_str(), tmp );
}

static bool checkAlign( const std::string &align ) {
	return !strcmp( "left", align.c_str() ) || !strcmp( "right", align.c_str() ) || !strcmp( "center", align.c_str() ) || !strcmp( "justify", align.c_str() );
}

static bool checkScroll( const std::string &scroll ) {
	return !strcmp( "none", scroll.c_str() ) || !strcmp( "vertical", scroll.c_str() ) || !strcmp( "horizontal", scroll.c_str() ) || !strcmp( "automatic", scroll.c_str() ) || !strcmp( "both", scroll.c_str() );
}

TextPlayer::TextPlayer( Device *dev )
	: GraphicPlayer( dev )
{
	_applied = false;
	_size = TextPlayer::DEFAULT_FONT_SIZE;
	_family = "Tiresias";
	_color = "white";
	_style = "normal";
	_variant = "normal";
	_weight = "normal";
	_scroll = "none";
	_scrollIndex = 0;
	_scrollLimit = 0;
	_scrollApplied = false;
	_beginText = "";
	_endText = "";
	_beginPosition = 0;
	_endPosition = 0;
	_align = "left";
}

TextPlayer::~TextPlayer() {
	if (_scrollApplied) {
		device()->systemPlayer()->delInputListener(this);
	}
}

void TextPlayer::refresh() {
	std::string text;
	std::vector<std::string> lines;
	std::string scrolledText;
	int w,h = 0;
	scrolledText = std::string("");

	if (readFile( text )) {
		if(hasVerticalScroll()) {
			//	set width with less pixels so that there will be space for scroll bar
			w = surface()->getBounds().w - 15;
			h = surface()->getBounds().h;
		} else {
			w = surface()->getBounds().w;
			h = surface()->getBounds().h;
		}

		surface()->clear();

		//	Trim the text if needed
		int position = 0;
		if (!_beginText.empty() && _beginPosition > 0) {
			position = _beginPosition - 1;
			if (position >= 0) {
				boost::iterator_range<std::string::iterator> r = boost::algorithm::find_nth( text,_beginText, position);
				if (!r.empty()) {
					text = std::string(r.begin(), text.end());
				} else {
					LWARN("TextPlayer", "Wrong text position");
				}
			}
		} else if (!_endText.empty() && _endPosition > 0) {
			int count = 0;
			size_t pos = text.find(_endText);
			while (pos != std::string::npos) {
				count++;
				pos = text.find(_endText, pos + 1);
			}
			position = count - _endPosition;
			if (position >= 0) {
				boost::iterator_range<std::string::iterator> r = boost::algorithm::find_nth( text,_endText, position);
				if (!r.empty()) {
					text = std::string(r.begin(), text.end());
				} else {
					LWARN("TextPlayer", "Wrong text position");
				}
			}
		}

		//	breaks the text on paragraphs
		surface()->findLines(text, lines);

		_scrollLimit = lines.size();

		for(size_t i = _scrollIndex; i < lines.size(); ++i) {
			scrolledText+= lines[i];
		}

		surface()->drawText( canvas::Rect(0,0,w,h), scrolledText );

		if(hasVerticalScroll()) {
			drawVerticalScrollBar();
		}

	}
}

void TextPlayer::drawVerticalScrollBar() {

	const canvas::Rect &bounds = surface()->getBounds();

	canvas::Color c;
	canvas::color::get( "gray", c );
	surface()->setColor( c );

	//	creates vertical scroll bar background
	canvas::Rect bar(bounds.w - 15,1,15,bounds.h-2);
	surface()->fillRect( bar );


	canvas::color::get( "white", c );
	surface()->setColor( c );

	//	draws upper square
	canvas::Rect upController(bounds.w - 14,2,13,10);
	surface()->fillRect( upController );

	//	draws down square
	canvas::Rect downController(bounds.w - 14,bounds.h-12,13,10);
	surface()->fillRect( downController );

	//	draws current bar position
	canvas::Rect barPosition(bounds.w - 14,(_scrollIndex*(bounds.h - 58)/_scrollLimit+13) ,13,30);
	surface()->fillRect( barPosition );

	surface()->flush();

	//	sets color back to original text color
	applyColor();
}


bool TextPlayer::readFile( std::string &text ) {
	std::ifstream ifs;
	std::string tmp;

	ifs.open( body().c_str() );
	if (!ifs.is_open()) {
		LERROR("TextPlayer", "failed to open file %s", body().c_str());
		return false;
	}
	while (!ifs.eof()) {
		getline( ifs, tmp );
		text += tmp + "\r\n";
	}
	ifs.close();
	return true;
}

void TextPlayer::registerProperties() {
	GraphicPlayer::registerProperties();
	
	{	//	Add fontSize
		MultiPropertyImpl<int> *prop = new MultiPropertyImpl<int>( _applied, true, _size );
		prop->setCheck( boost::bind( std::greater<int>(), _1, 0) );
		prop->setApply( boost::bind( &TextPlayer::applyFont, this) );
		addProperty( property::type::fontSize, prop );
	}
	{	//	Add fontFamily
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, true, _family );
		prop->setCheck( boost::bind( &check::notEmpty, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyFont,this) );
		addProperty( property::type::fontFamily, prop );
	}
	{	//	Add fontStyle
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, true, _style );
		prop->setCheck( boost::bind( &checkStyle, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyFont,this) );
		addProperty( property::type::fontStyle, prop );
	}
	{	//	Add fontVariant
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, true, _variant );
		prop->setCheck( boost::bind( &checkVariant, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyFont,this) );
		addProperty( property::type::fontVariant, prop );
	}
	{	//	Add fontWeight
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, true, _weight );
		prop->setCheck( boost::bind( &checkWeight, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyFont,this) );
		addProperty( property::type::fontWeight, prop );
	}
	{	//	Add fontColor
		PropertyImpl<std::string> *prop = new PropertyImpl<std::string>( true, _color );
		prop->setCheck( boost::bind( &checkColor, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyColor,this) );
		addProperty( property::type::fontColor, prop );
	}
	{	//	Add scroll
		PropertyImpl<std::string> *prop = new PropertyImpl<std::string>( true, _scroll );
		prop->setCheck( boost::bind( &checkScroll, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyScroll,this) );
		addProperty( property::type::scroll, prop );
	}
	{	//	Add textAlign
		PropertyImpl<std::string> *prop = new PropertyImpl<std::string>( true, _align );
		prop->setCheck( boost::bind( &checkAlign, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyAlign,this) );
		addProperty( property::type::textAlign, prop );
	}

	{	//	Property attributionEvent
		ForwardProperty<AttributionEventData> *prop = new ForwardProperty<AttributionEventData>(
			boost::bind(&TextPlayer::setAdditionalParameters,this,_1), false );
		addProperty( property::type::attributionEvent, prop );
	}

}

void TextPlayer::applyFont() {
	canvas::Font font;
	
	LINFO("TextPlayer", "Apply font: family=%s, style=%s, variant=%s, weight=%s, size=%d",
		_family.c_str(), _style.c_str(), _variant.c_str(), _weight.c_str(), _size);

	//	Apply size
	font.size( _size );

	//	Apply style
	font.italic( !strcmp( "italic", _style.c_str() ) );

	//	Apply variant
	font.smallCaps( !strcmp( "small-caps", _variant.c_str() ) );

	//	Apply weight
	font.bold( !strcmp( "bold", _weight.c_str() ) );
	
	{	//	Apply family
		std::vector<std::string> families;
		boost::erase_all(_family, " ");
		boost::split( families, _family, boost::is_any_of( "," ) );
		font.families(families);
		surface()->setFont(font);
	}
}

void TextPlayer::applyColor() {
	LINFO("TextPlayer", "Apply font color: color=%s", _color.c_str());
	canvas::Color c;
	canvas::color::get( _color.c_str(), c );
	surface()->setColor( c );

}

void TextPlayer::applyAlign() {
	LINFO("TextPlayer", "Apply text align: color=%s", _align.c_str());
	surface()->setTextAlign( _align );
}

void TextPlayer::setAdditionalParameters( const AttributionEventData &data ) {
	LINFO("TextPlayer", "Apply text begin parameters: parameter name =%s", data.first.c_str());
	if (data.first == "_beginText" ) {
		_beginText = data.second;
	} else if (data.first == "_beginPosition" ) {
		_beginPosition = atoi(data.second.c_str());
	} else if (data.first == "_endText" ) {
		_endText = data.second;
	} else if (data.first == "_endPosition" ) {
		_endPosition = atoi(data.second.c_str());
	}
}

void TextPlayer::applyScroll() {
	LINFO("TextPlayer", "Apply scroll: scroll=%s", _scroll.c_str());

	::util::key::Keys keys;

	//	vertical scroll bar
	if ( !strcmp( "both", _scroll.c_str() ) ||  !strcmp( "vertical", _scroll.c_str() ) ){
	  keys.insert( ::util::key::cursor_down );
	  keys.insert( ::util::key::cursor_up );
	}

	//	horizontal scroll bar
	if ( !strcmp( "both", _scroll.c_str() ) ||  !strcmp( "horizontal", _scroll.c_str() ) ){
	  keys.insert( ::util::key::cursor_left );
	  keys.insert( ::util::key::cursor_right );
	}

	player::input::InputCallback fnc = boost::bind( &TextPlayer::userEventReceived, this, _1, _2 );
	device()->systemPlayer()->addInputListener( this, fnc, keys );
	_scrollApplied = true;
}


void TextPlayer::userEventReceived( ::util::key::type key, bool isUp ) {
	//	release event
	if ( !isUp ) {
		return;
	}

	switch ( key ) {
		case ::util::key::cursor_up:{
			if (_scrollIndex>0) {
				_scrollIndex--;
			}
			refresh();
			break;
		}

		case ::util::key::cursor_down: {
			if(_scrollIndex < _scrollLimit) {
				_scrollIndex++;
			}
			refresh();
			break;
		}

		case ::util::key::cursor_left:{
			break;
		}

		case ::util::key::cursor_right: {
			break;
		}

		default:
			break;
		};
}

bool TextPlayer::hasHorizontalScroll(){
	return !strcmp( "both", _scroll.c_str() ) || !strcmp( "horizontal", _scroll.c_str() );
}

bool TextPlayer::hasVerticalScroll(){
	return !strcmp( "both", _scroll.c_str() ) || !strcmp( "vertical", _scroll.c_str() );
}
}

