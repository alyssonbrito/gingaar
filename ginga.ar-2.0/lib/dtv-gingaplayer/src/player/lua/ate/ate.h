#pragma once

typedef struct lua_State lua_State;


namespace player {

class System;
class LuaPlayer;

namespace ate {

class Module {
public:
	Module ( LuaPlayer *player, lua_State *lua );
	virtual ~Module();

	//	Aux
	static Module *get( lua_State *st );


private:
	System *_sys;
	LuaPlayer *_player;
	lua_State *_lua;

};
}	//	namespace event
}	//	namespace player

