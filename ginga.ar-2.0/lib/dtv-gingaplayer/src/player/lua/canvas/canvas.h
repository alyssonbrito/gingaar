#pragma once

#include <string>
#include <vector>

typedef struct lua_State lua_State;

namespace canvas {
	class Surface;
}

namespace player {

class LuaPlayer;

namespace mcanvas {

class SurfaceWrapper;

class Module {
public:
	Module( LuaPlayer *player, lua_State *lua );
	virtual ~Module();

	static const int GC_MILLI_SECONDS = 200;

	void start( canvas::Surface *surface );
	void stop();

	int createSurface( lua_State *L, canvas::Surface *surface, bool isPrimary=false );
	std::string getFilePath( const std::string &filepath );
	static Module *get( lua_State *st );
	std::string getOriginPath() const;

	bool removeWrapper( SurfaceWrapper *wrapper );

private:
	LuaPlayer *_player;
	lua_State *_lua;
	canvas::Surface *_surface;
	std::vector<SurfaceWrapper *> _surfaces;
	std::string _originPath;
};

}
}
