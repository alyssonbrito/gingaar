#pragma once

#include "graphicplayer.h"
#include <util/keydefs.h>
#include <vector>
#include "lua/ate/ate.h"

typedef struct lua_State lua_State;
typedef int (*lua_CFunction) (lua_State *L);

namespace player {

namespace mcanvas {
	class Module;
}

namespace settings {
	class Module;
}

namespace persistent {
	class Module;
}

class LuaPlayer: public GraphicPlayer {
public:
	LuaPlayer( Device *dev );
	virtual ~LuaPlayer();

	virtual bool isApplication() const;

	//	Callback initialization
	virtual void setEventCallback( const event::Callback &callback );
	virtual void setInputEventCallback( const event::InputCallback &callback );

protected:
	//	Types
	typedef std::pair<std::string,std::string> AttributionEventData;
	typedef std::pair<util::key::type,bool> SelectionEventData;
	
	//	Virtual methods
	virtual bool init();
	virtual void fin();
	virtual bool startPlay( void );
	virtual void stopPlay( void );
	virtual void pausePlay( bool pause );
	virtual void abortPlay( void );
	virtual void refresh();
	virtual void registerProperties();
	virtual void dispatchKey( util::key::type key, bool isUp );

	//	Events
	void onPresentationEvent( event::evtAction::type action, const std::string &label="" );
	void sendAttributionEvent( const AttributionEventData &data );
	void onAttributionEvent( const std::string &name="", const std::string &value="" );
	void onKeyEvent( util::key::type key, bool isUp );
	
	//	Aux lua
	bool loadModules();
	void loadModule( lua_CFunction fnc, const char *name );
	void loadModuleWithout( lua_CFunction fnc, const char *name, std::vector<std::string> &toExclude );
	void unloadModules();
	void stopPlayer( event::evtAction::type evtAction );
	void closeLua();
	void addPath( const std::string &path );

private:
	lua_State *_lua;
	event::Module *_event;
	mcanvas::Module *_canvas;
	settings::Module *_settings;
	persistent::Module *_persistent;
	ate::Module *_ate;
	std::string _packagePath;
};

}
