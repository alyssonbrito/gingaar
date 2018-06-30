#include "lua.h"
#include "../luaplayer.h"

namespace player {
namespace lua {

void storeObject( lua_State *st, void *obj, const char *index ) {
	lua_pushlightuserdata( st, obj );
	lua_setfield( st, LUA_REGISTRYINDEX, index );
}

std::string getField( lua_State *st, int pos, const char *szIndex ) {
	std::string field;
	lua_getfield( st, pos, szIndex );
	if ( !lua_isnil( st, -1 ) && lua_isstring( st, -1 ) ) {
		size_t len = lua_strlen( st, -1 );
		field.assign( luaL_checkstring( st, -1 ), len );
	}
	lua_pop( st, 1 );

	return field;
}


}
}

