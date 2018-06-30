#pragma once
#include <map>
#include <string>
#include <vector>
#include "../../settings.h"

typedef struct lua_State lua_State;

namespace player {

class LuaPlayer;

namespace settings {

class Module
{
public:
	Module( LuaPlayer *player, lua_State *lua );
	virtual ~Module();

	static Module *get( lua_State *st );
	inline Settings *settings() { return _settings; }

private:
	LuaPlayer *_player;
	lua_State *_lua;
	Settings *_settings;
};

}
}
