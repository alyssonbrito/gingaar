#include "../lua.h"
#include "../../luaplayer.h"
#include "../../../system.h"

extern "C" {
#include "server.h"
}


#define LUA_ATE "lua_module_ate"
#define PASSED 0
#define FAILED 1

namespace player {
namespace ate {


/*******************************************************************************
*	Event module
*******************************************************************************/

static int l_failed( lua_State *L ) {
	//	get number of arguments
	int n = lua_gettop(L);
	int id = 0;
	const char * reason;

	if (n != 2) {
	//	push number 1 into ncl stack
	lua_pushnumber(L,1);

	//	inform that there is only one return value on the stack
	return 1;
	}

	//	get the id paramenter
	id = lua_tonumber(L,1);

	//	get the result string
	reason = lua_tostring(L,2);

	printf("\n\nEngine Log - ID: %d\n",id);
	printf("\n\nEngine Log - REASON: %s\n", reason);

	//	this will inform result to ATE
	start_server(FAILED, id, reason);

	//	return 0 to indicate success
	lua_pushnumber(L,0);

	return 1;
}

static int l_passed( lua_State *L ) {
	//	get number of arguments
	int n = lua_gettop(L);
	int id = 0;
	const char * reason;

	if (n != 2) {
		//	push number 1 into ncl stack
		lua_pushnumber(L,1);

		//	inform that there is only one return value on the stack
		return 1;
	}

	//	get the id paramenter
	id = lua_tonumber(L,1);

	//	get the result string
	reason = lua_tostring(L,2);

	printf("\n\nEngine Log - ID: %d\n",id);
	printf("\n\nEngine Log - REASON: %s\n", reason);

	//	this will inform result to ATE
	start_server(PASSED, id, reason);

	//	eturn 0 to indicate success
	lua_pushnumber(L,0);

	return 1;
}


static const struct luaL_Reg ate_methods[] = {
	{ "passed",       l_passed      },
	{ "failed",      l_failed      },
	{ NULL,         NULL         }
};


/*******************************************************************************
*	Class Module
*******************************************************************************/
Module::Module( LuaPlayer *player, lua_State *lua )
	: _player(player), _lua( lua )
{
	//	Store module into stack
	lua::storeObject( lua, this, LUA_ATE );

	//	Register event methods
	luaL_register( _lua, "ate", ate_methods );
}

Module::~Module()
{
}

Module *Module::get( lua_State *st ) {
	return lua::getObject<Module>(st,LUA_ATE);
}

}
}

