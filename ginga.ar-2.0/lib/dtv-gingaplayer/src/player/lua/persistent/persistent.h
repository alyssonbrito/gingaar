#pragma once
#include <map>
#include <string>
#include <vector>

typedef struct lua_State lua_State;

namespace player {

class LuaPlayer;

namespace persistent {

class Module
{
public:
	Module( LuaPlayer *player, lua_State *lua );
	virtual ~Module();

	void stop();
	static Module *get( lua_State *st );

private:
	LuaPlayer *_player;
	lua_State *_lua;
};

}
}
