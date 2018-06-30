#pragma once

#include <string>
#include <util/types.h>

#define COLOR_LIST(fnc)	          \
	fnc ( white,       0xFF, 0xFF, 0xFF, 0xFF ) \
	fnc ( aqua,        0x00, 0xFF, 0xFF, 0xFF ) \
	fnc ( lime,        0x00, 0xFF, 0x00, 0xFF ) \
	fnc ( yellow,      0xFF, 0xFF, 0x00, 0xFF ) \
	fnc ( red,         0xFF, 0x00, 0x00, 0xFF ) \
	fnc ( fuchsia,     0xFF, 0x00, 0xFF, 0xFF ) \
	fnc ( purple,      0x80, 0x00, 0x80, 0xFF ) \
	fnc ( maroon,      0x80, 0x00, 0x00, 0xFF ) \
	fnc ( blue,        0x00, 0x00, 0xFF, 0xFF ) \
	fnc ( navy,        0x00, 0x00, 0x80, 0xFF ) \
	fnc ( teal,        0x00, 0x80, 0x80, 0xFF ) \
	fnc ( green,       0x00, 0x80, 0x00, 0xFF ) \
	fnc ( olive,       0x80, 0x80, 0x00, 0xFF ) \
	fnc ( silver,      0xC0, 0xC0, 0xC0, 0xFF ) \
	fnc ( gray,        0x80, 0x80, 0x80, 0xFF ) \
	fnc ( black,       0x00, 0x00, 0x00, 0xFF ) \
	fnc ( transparent, 0x00, 0x00, 0x00, 0x00 ) \
	fnc ( magenta,     0x00, 0x00, 0x00, 0x00 )

namespace canvas {

class Color;

namespace color {

#define DO_ENUM_COLOR( name, r, g, b, a ) name,
enum type {
	COLOR_LIST(DO_ENUM_COLOR)
	LAST_COLOR
};
#undef DO_ENUM_COLOR

bool get( const char *textColor, Color &color );
}	//	end color

class Color {
public:
	Color( void );
	Color( int r, int g, int b, int alpha );
	Color( int r, int g, int b );
	Color( long rgb );
	~Color( void );

	static Color createColor (const char * color );

	bool operator==(const Color &color) const;
	bool operator!=(const Color &color) const;

	// Math Operation with two-operands
	Color operator + ( const Color & color );
	Color operator - ( const Color & color );

	Color operator + ( double factor );
	Color operator * ( int factor );
	Color operator / ( double factor );

	std::string toString ( );

	long asNumber ();

	bool equals( const Color &color, int threshold ) const;

	int r, g, b, alpha;
};
	
}
