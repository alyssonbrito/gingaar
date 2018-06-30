#include "color.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "system.h"
#include <util/log.h>

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cerrno>
#include <sstream>

namespace canvas {

namespace color {

typedef struct {
	const char *name;
	int r;
	int g;
	int b;
	int a;
} ColorType;

#define DO_ENUM_COLOR( name, r, g, b, a ) { #name, r, g, b, a },
static const ColorType colors[] = {
	COLOR_LIST(DO_ENUM_COLOR)
	{ NULL, 0, 0, 0, 0 }
};
#undef DO_ENUM_COLOR

bool get( const char *textColor, Color &color ) {
	int i=0;
	while (colors[i].name && strcmp(colors[i].name,textColor) != 0) {
		i++;
	}
	if (i < LAST_COLOR) {
		color = Color( colors[i].r, colors[i].g, colors[i].b, colors[i].a );
		return true;
	}

	//	LG Electronics: #4407
	//	Check if color is defined in '#00004A' format
	//	Example is Bela content
	if (textColor[0] == '#') {
		char *endptr;
		errno=0;
		long val = strtol(textColor+1, &endptr, 16); // Base is 16

		//	Check for various possible errors
		if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
			LWARN("Color", "RGB color is out of range.\n");
			return false; // Out of range
		}

		if (endptr == textColor + 1) {
			LWARN("Color", "No digits were found in RGB color.\n");
			return false; // No digits
		}
		color = Color((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
		return true;
	}
	return false;
}

}

Color::Color( void )
{
	r = 0;
	g = 0;
	b = 0;
	alpha = 0;
}

Color::Color( int paramR, int paramG, int paramB, int paramAlpha )
{
	r = paramR;
	g = paramG;
	b = paramB;
	alpha = paramAlpha;
}

Color::Color( int paramR, int paramG, int paramB )
{
	r = paramR;
	g = paramG;
	b = paramB;
	alpha = 255;
}

Color::Color( long rgb ) {
	r = ( rgb >> 16 ) & 0xFF;
	g = ( rgb >> 8 ) & 0xFF;
	b = ( rgb & 0xFF );
	alpha = 255;
}

Color::~Color( void )
{
}

Color Color::createColor (const char * color ) {
	if (color[0] == '#') {
		char *endptr;
		errno=0;
		long val = strtol( color + 1, & endptr, 16 ); // Base is 16

		//	Check for various possible errors
		if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
			LWARN("Color", "RGB color is out of range.\n");
			return Color ( 0, 0, 0 ); // Out of range
		}

		if (endptr == color + 1) {
			LWARN("Color", "No digits were found in RGB color.\n");
			return Color ( 0, 0, 0 ); // No digits
		}

		return Color((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
	}
	return Color ( 0, 0, 0 );
}

bool Color::operator == ( const Color & color ) const {
	return r == color.r && g == color.g && b == color.b;
}

bool Color::operator != ( const Color & color ) const {
	return !((*this) == color);
}

Color Color::operator + ( const Color & color ) {
	Color res ( r, g, b, alpha );
	res.r = res.r + color.r;
	res.g = res.g + color.g;
	res.b = res.b + color.b;
	return res;
}

Color Color::operator - ( const Color & color ) {
	Color res ( r, g, b, alpha );
	res.r = res.r - color.r;
	res.g = res.g - color.g;
	res.b = res.b - color.b;
	return res;
}

Color Color::operator + ( double factor ) {
	Color res ( r, g, b, alpha );
	res.r = res.r + factor;
	res.g = res.g + factor;
	res.b = res.b + factor;
	return res;
}

Color Color::operator * ( int factor ) {
	Color res ( r, g, b, alpha );
	res.r = res.r * factor;
	res.g = res.g * factor;
	res.b = res.b * factor;
	return res;
}

Color Color::operator / ( double factor ) {
	Color res ( r, g, b, alpha );
	res.r = ( factor != 0.0 ) ? res.r / factor : 0;
	res.g = ( factor != 0.0 ) ? res.g / factor : 0;
	res.b = ( factor != 0.0 ) ? res.b / factor : 0;
	return res;
}

std::string Color::toString ( ) {
	std::stringstream ss;
	ss << "Color(" << r << ", " << g << ", " << b << ", " << alpha << ")";
	return ss.str ( );
}

long Color::asNumber () {
	int _r = ( r < 0 ? 0 : ( r > 255 ? 255 : r ) );
	int _g = ( g < 0 ? 0 : ( g > 255 ? 255 : g ) );
	int _b = ( b < 0 ? 0 : ( b > 255 ? 255 : b ) );

	// where's alpha?
	return ( _r << 16 | _g << 8 | _b );
}

bool Color::equals( const Color &color, int threshold ) const {
	int error = abs(color.r - r) + abs( color.g - g ) + abs( color.b - b ) + abs( color.alpha - alpha );
	return error <= threshold;
}

}
