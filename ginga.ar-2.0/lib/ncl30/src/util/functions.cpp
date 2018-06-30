#include "functions.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <iostream>
#include <util/log.h>

namespace ncl_util {

#define CHECK_RANGE(var,min,max)				\
	{ if (var < min) var = min; else if (var > max) var = max; }

double NaN() {
	return -1.0;
}

double infinity() {
	return std::numeric_limits<double>().infinity();
}

bool isNaN( double value ) {
	return value < 0;
}

bool isInfinity( double value ) {
	return value == std::numeric_limits<double>().infinity();
}

double getDoubleValue( const std::string &value, bool throwException ) {
	double pValue = 0;
	try {
		pValue = boost::lexical_cast<double>( boost::trim_copy( value ) );
	} catch (boost::bad_lexical_cast &) {
		LWARN("util:functions", "Conversion error by[%s]",value.c_str());
		if (throwException) {
			throw;
		}
	}
	return pValue;
}
int getPixelValue( const std::string &value, bool throwException ) {

	int pValue = 0;

	std::string actualValue = value;
	boost::trim( actualValue );
	boost::trim_right_if( actualValue, boost::is_any_of( "px" ) );

//	if (actualValue.empty()) {
//		actualValue = "0";
//	}

	try {
		pValue = boost::lexical_cast<int>( boost::trim_copy( actualValue ) );
	} catch (boost::bad_lexical_cast &) {
		LWARN("util:functions", "Conversion error by[%s]",value.c_str());
		if (throwException) {
			throw;
		}
	}

	return pValue;
}

float getPercentualValue( const std::string &value, bool throwException ) {

	float pValue = 0.0;

	std::string actualValue = value;
	boost::trim( actualValue );
	boost::trim_right_if( actualValue, boost::is_any_of( "%" ) );
	try {
		pValue = boost::lexical_cast<float>( actualValue ) / 100;
		if (!throwException) {
			CHECK_RANGE( pValue, 0, 1 );
		}
	} catch (boost::bad_lexical_cast &) {
		LWARN("util:functions", "Conversion error by[%s]",value.c_str());
		if (throwException) {
			throw;
		}
	}

	return pValue;
}

float getTimeValue( const std::string &value, bool throwException ) {

	float pValue = 0.0;

	std::string actualValue = value;
	//boost::trim( actualValue );
	//boost::trim_right_if( actualValue, boost::is_any_of( "s" ) );
	actualValue.erase(boost::remove_if(actualValue, boost::is_any_of( "s ()\t" ) ),actualValue.end());

	try {
		pValue = boost::lexical_cast<float>( actualValue ) ;
		return pValue;
	} catch (boost::bad_lexical_cast &) {
		LWARN("util:functions", "Time not in seconds[%s]",value.c_str());
		APRINT_NCL3("Time not in seconds[%s]",value.c_str());
	}//catch
	boost::trim_right_if( actualValue, boost::is_any_of( "m" ) );
	try {
		pValue = boost::lexical_cast<float>( actualValue ) ;
		return pValue*60;
	} catch (boost::bad_lexical_cast &) {
		LWARN("util:functions", "Time not in minutes[%s]",value.c_str());
		if (throwException) {
			throw;
		}
	}//catch

	return pValue;
}

bool splitString( const std::string &value, const std::string &separator,
								std::string *a, std::string *b){
	APRINT_NCL3("==> value[%s][%d] separator[%s][%d] ",value.c_str(),value.length(),separator.c_str(),separator.length());	
	try{
		std::size_t pos = value.find(separator);
		if (pos== std::string::npos){
			APRINT_NCL3("<== return[false]");
			return false;
		}
		APRINT_NCL3("pos[%u]",pos);
		if (pos==0) {
			*a = "";
		}
		else {
			*a = value.substr(0,pos);
		}
		APRINT_NCL3("a[%s]",a->c_str());
		*b = value.substr(pos+separator.length());
		APRINT_NCL3("b[%s]",b->c_str());
		return true;
	}catch (...){
		return false;
	}
}//spliString

bool getStringComponent( const std::string &value, const std::string &separator,int num,
							std::string *a){
	bool res = false;
	if (a==NULL){
		return false;
	}
	try{
		std::size_t init_pos=0;
		std::size_t end_pos;
		//if (num>0){
			for(int i=1;i<=num;i++){
				APRINT_NCL3("i[%d]",i);
				init_pos = value.find(separator,init_pos);
				init_pos+=separator.length();
			}
		//init_pos+=separator.length();
		//}
		if (init_pos== std::string::npos){
			return false;
		}
		end_pos=value.find(separator,init_pos);

		if (end_pos== std::string::npos){
			end_pos = value.length();
		}
		if (init_pos>=end_pos) {
			*a = "";
		} else {
			*a = value.substr(init_pos,end_pos-init_pos);
		}
		res = true;
	}catch (...){
		APRINT_NCL3("--- EXCEPTION");
		res =  false;
	}
	return res;
}//getStringComponent

bool splitString( const std::string &value, const std::string &separator,
							std::string *a, std::string *b,std::string *c, std::string *d){
	APRINT_NCL3("==> value[%s][%lu] separator[%s][%lu] ",value.c_str(),value.length(),separator.c_str(),separator.length());
	bool res = false;
	try{
		res  = getStringComponent(value,separator,0,a);
		res &= getStringComponent(value,separator,1,b);
		res &= getStringComponent(value,separator,2,c);
		res &= getStringComponent(value,separator,3,d);
	}catch (...){
		APRINT_NCL3("EXCEPTION");
			res = false;
	}
	APRINT_NCL3("<== return[%d] splitedString[%s][%s][%s][%s] ",res,a->c_str(),b->c_str(),c->c_str(),d->c_str());
	return res;
}//spliString

bool isPercentualValue( const std::string &value ) {
	std::string v = value;
	boost::trim( v );
	//	LG Electronics: #4308
	//	Hack to kick VivaMaisPeso content, where "%%" in width property
	//	is used instead of "%". Change occurrence of "%%" to "%".
	boost::replace_all(v, "%%", "%");
	return v.find( "%" ) == v.size() - 1;
}

bool isValidNumericValue( const std::string &value ) {
	bool result = true;
	try {
		if (isPercentualValue( value )) {
			float percentualValue = getPercentualValue( value, true );
			if ((percentualValue > 1) || (percentualValue < 0)) {
				LERROR("ncl30", "The value %s must be between 0 and 100", value.c_str());
				result = false;
			}
		} else {
			int pxValue = getPixelValue( value, true );
			if (pxValue < 0) {
				LERROR("ncl30", "The value %s is invalid", value.c_str());
				result = false;
			}
		}
	} catch (boost::bad_lexical_cast &) {
		LERROR("ncl30", "The value %s is invalid", value.c_str());
		result = false;
	}
	return result;
}

int hexToInt ( const std::string &value ) {
	int x;
	std::stringstream ss;

	ss << std::hex << value;
	ss >> x;

	return static_cast < int > ( x );
}

std::string intToHex ( const int &value ) {
	std::stringstream ss;
	ss << "#" << std::setfill ( '0' ) << std::setw ( 6 ) << std::hex << value;
	return ss.str();
}

int webRGBtoInt ( const std::string &value ) {
	int result = 0;

	try {
		if (value[0] == '#') {
			std::string temp = value.substr(1, value.length() - 1);
			result = hexToInt ( temp );
		}
		else {
			result = boost::lexical_cast<int>( value );
		}
	} catch (boost::bad_lexical_cast &) {
		result = 0;
	}

	return result;
}

std::string getRGBColor(const std::string &color){

	std::string rgb = "#FA8072";
	if (color=="white"){
		rgb = "#FFFFFF";
	}else if (color == "black"){
		rgb = "#000000";
	}else if (color == "red"){
		rgb = "#FF0000";
	}else if (color == "lime"){
		rgb = "#00FF00";
	}else if (color == "blue"){
		rgb = "#0000FF";
	}else if (color == "yellow"){
		rgb = "#FFFF00";
	}else if (color == "cyan"|| color == "aqua"){
		rgb = "#00FFFF";
	}else if (color == "magenta" || color == "fuchsia"){
		rgb = "#FF00FF";
	}else if (color == "silver"){
		rgb = "#C0C0C0";
	}else if (color == "gray"){
		rgb = "#808080";
	}else if (color == "maroon"){
		rgb = "#800000";
	}else if (color == "olive"){
		rgb = "#808000";
	}else if (color == "green"){
		rgb = "#008000";
	}else if (color == "purple"){
		rgb = "#800080";
	}else if (color == "teal"){
		rgb = "#008080";
	}else if (color == "navy"){
		rgb = "#008080";
	}
	return rgb;

}//getRGBColor

}//namespace
