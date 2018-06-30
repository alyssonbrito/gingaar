#pragma once

#include <iostream>
#include <iomanip>

#include <string>
#include <list>
#include "../../../../include/ginga_api.h"

namespace ncl_util {

float getPercentualValue( const std::string &value, bool throwException = false);
bool isPercentualValue( const std::string &value );

double getDoubleValue( const std::string &value, bool throwException = false);
int getPixelValue( const std::string &value, bool throwException = false);
float getTimeValue( const std::string &value, bool throwException = false);
bool getStringComponent( const std::string &value, const std::string &separator,int num,
		std::string *a);
bool splitString( const std::string &value, const std::string &separator,
		std::string *a, std::string *b);
bool splitString( const std::string &value, const std::string &separator,
		std::string *a, std::string *b,std::string *c, std::string*d);

double NaN();
double infinity();
bool isNaN( double value );
bool isInfinity( double value );
bool isValidNumericValue( const std::string &values);
int hexToInt ( const std::string &value );
std::string intToHex ( const int &value );
int webRGBtoInt ( const std::string &value );
std::string getRGBColor(const std::string &color);

struct is_alpha_or_space {
	bool operator()( char c ) {
		return std::isalpha(c) || std::isspace(c);
	}
};

}
