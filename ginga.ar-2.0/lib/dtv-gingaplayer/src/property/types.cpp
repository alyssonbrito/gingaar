#include "types.h"
#include <canvas/color.h>
#include <boost/algorithm/string.hpp>

namespace player {
namespace property {

#define DO_ENUM_PROPERTY_STRING( p ) #p,
#define DO_ENUM_PROPERTY_STRING1( p, p1 ) p1,
static const char* properties[] = {
    "NULL",
    PROPERTY_LIST(DO_ENUM_PROPERTY_STRING,DO_ENUM_PROPERTY_STRING1)
    "LAST"
};
#undef DO_ENUM_PROPERTY_STRING
#undef DO_ENUM_PROPERTY_STRING1

type::Type getProperty( const char *propertyName ) {
	for (int i=0; i<type::LAST; i++) {
		if (!strcmp( properties[i], propertyName)) {
			return (type::Type)i;
		}
	}
	return type::null;
}

const char *getPropertyName( type::Type pValue ) {
	if (pValue > type::LAST) {
		pValue=type::null;
	}
	return properties[pValue];
}

}	//	namespace property
}	//	namespace player
