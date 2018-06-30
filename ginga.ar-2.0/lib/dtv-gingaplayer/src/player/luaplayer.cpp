#include "luaplayer.h"
#include "settings.h"
#include "lua/lua.h"
#include "lua/event/event.h"
#include "lua/canvas/canvas.h"
#include "lua/settings/settings.h"
#include "lua/persistent/persistent.h"
#include "lua/ate/ate.h"
#include "../property/forwardproperty.h"
#include "../device.h"
#include "../system.h"
#include <canvas/system.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

namespace player {

namespace fs = boost::filesystem;

//LGESP dsmcc
//Fixing issue NEGINGA-136
std::string _absPath;
static int get_module(lua_State *lua) {
	const char *modname = luaL_checkstring(lua, 1);
	std::string p = _absPath + modname + ".lua";
	onFetchFilesFromCarousel(p);
	return 0;
}

static int _lua_dsmcc(lua_State *lua) {
	const char *filename = luaL_checkstring(lua, 1);
	std::string p = filename;//_absPath + modname + ".lua";
	onFetchFilesFromCarousel(p);
	return 0;
}

LuaPlayer::LuaPlayer( Device *dev )
  : GraphicPlayer( dev )
{
	_lua = NULL;
	_event = NULL;
	_canvas = NULL;
	_settings = NULL;
	_persistent = NULL;
	_ate = NULL;
}

#include <setjmp.h>
static jmp_buf _ginga_lua_panic_jmp;
static void _at_panic(void)
{
	fprintf(stderr,"[%s][%s][%d] PANIC   \n",__FILE__,__FUNCTION__,__LINE__);
	fprintf(stderr,"[%s][%s][%d] PANIC   \n",__FILE__,__FUNCTION__,__LINE__);
	fprintf(stderr,"[%s][%s][%d] PANIC   \n",__FILE__,__FUNCTION__,__LINE__);
	fprintf(stderr,"[%s][%s][%d] PANIC   \n",__FILE__,__FUNCTION__,__LINE__);
	fprintf(stderr,"[%s][%s][%d] PANIC   \n",__FILE__,__FUNCTION__,__LINE__);
	fprintf(stderr,"[%s][%s][%d] PANIC   \n",__FILE__,__FUNCTION__,__LINE__);
}
static int _ginga_lua_panic(lua_State * /* L */)
{
	longjmp(_ginga_lua_panic_jmp,1);
}

LuaPlayer::~LuaPlayer()
{
	assert(!_event);
	assert(!_canvas);
	assert(!_settings);
	assert(!_persistent);
	assert(!_ate);
}

bool LuaPlayer::isApplication() const {
	return true;
}

bool LuaPlayer::init() {
	assert( !_lua );
	
	//	Initialize graphic player
	if (!GraphicPlayer::init()) {
		return false;
	}
	
	//	Create lua state
	_lua = luaL_newstate();
	if (!_lua) {
		LWARN("LuaPlayer", "Cannot initialize lua engine");
		return false;
	}

#if 0
	/* LUA PANIC**/
	if ( ! setjmp(_ginga_lua_panic_jmp) )
		{
		fprintf(stderr,"[%s][%s][%d] PANIC	\n",__FILE__,__FUNCTION__,__LINE__);
		fprintf(stderr,"[%s][%s][%d] PANIC   \n",__FILE__,__FUNCTION__,__LINE__);
		fprintf(stderr,"[%s][%s][%d] PANIC	\n",__FILE__,__FUNCTION__,__LINE__);
		_at_panic();
		}
	lua_atpanic(_lua,&_ginga_lua_panic);
#endif

	if (!loadModules()) {
		LWARN("LuaPlayer", "Cannot load lua modules");
		return false;
	}

	return true;
}

void LuaPlayer::fin() {
	unloadModules();
	closeLua();
	GraphicPlayer::fin();
}

void LuaPlayer::loadModule( lua_CFunction fnc, const char *name ) {
	lua_pushcfunction(_lua, fnc);
	lua_pushstring(_lua, name);
	lua_call(_lua, 1, 0);
}

void LuaPlayer::loadModuleWithout( lua_CFunction fnc, const char *name, std::vector<std::string> &toExclude ) {
	loadModule(fnc, name);
	lua_getglobal(_lua, name);

	for (std::vector<std::string>::iterator it = toExclude.begin() ; it != toExclude.end(); ++it) {
		lua_pushnil(_lua);
		lua_setfield(_lua, -2, (*it).c_str());
	}

	lua_setglobal(_lua, name);
}

bool LuaPlayer::loadModules() {
	//	Open libs
	loadModule(luaopen_base, LUA_COLIBNAME);
	loadModule(luaopen_table, LUA_TABLIBNAME);
	loadModule(luaopen_string, LUA_STRLIBNAME);
	loadModule(luaopen_math, LUA_MATHLIBNAME);
	loadModule(luaopen_io, LUA_IOLIBNAME);

	std::vector<std::string> toExclude;
	toExclude.push_back(std::string("execute"));
	toExclude.push_back(std::string("exit"));
	toExclude.push_back(std::string("getenv"));
	toExclude.push_back(std::string("remove"));
	toExclude.push_back(std::string("rename"));
	toExclude.push_back(std::string("tmpname"));
	toExclude.push_back(std::string("setlocale"));
	toExclude.push_back(std::string("clock"));

	loadModuleWithout(luaopen_os, LUA_OSLIBNAME, toExclude);

	toExclude.clear();
	toExclude.push_back("loadlib");
	loadModuleWithout(luaopen_package, LUA_LOADLIBNAME, toExclude);

	//	Initialize event/canvas modules
	_event = new event::Module( device()->systemPlayer(), this, _lua );
	_canvas = new mcanvas::Module( this, _lua );
	_settings = new settings::Module( this, _lua );
	_ate = new ate::Module( this, _lua );

	//_file = new file::Module( this, _lua );

	//LGESP Fixing issue NEGINGA-136
	// TODO: Replace this by creating a dsmcc module
	// On dsmcc module call Device->OnFetchFile
	// TODO: use same hook as _lua_dsmcc ?
	lua_register(_lua,"get_module",get_module);
	luaL_dostring(_lua,"table.insert(package.loaders, 1, get_module)");

	return true;
}

void LuaPlayer::unloadModules() {
	//	Unload event module
	DEL(_event);
	DEL(_canvas);
	DEL(_settings);
	DEL(_persistent);
	DEL(_ate);
}

void LuaPlayer::closeLua() {
	if (_lua) {
		lua_close( _lua );
		_lua = NULL;
	}
}

void LuaPlayer::addPath( const std::string &modulePath ) {
	std::string _modulePath = modulePath;
	lua_getglobal( _lua, "package" );
	lua_getfield( _lua, -1, "path" ); // get field "path" from table at top of stack (-1)
	if (_packagePath.empty()) {
		_packagePath = lua_tostring( _lua, -1 ); // grab path string from top of stack
	}

#if 0
	std::string cur_path = _packagePath;
	std::string cur_path = "./?;./?.lua";

	{	// Add search: modulePath/?
		cur_path.append( ";" );
		fs::path path = _modulePath;
		path /= "?";
		cur_path.append( path.string().c_str() );
	}

	{	// Add search: modulePath/?.lua
		cur_path.append( ";" ); // do your path magic here
		fs::path path = _modulePath;
		path /= "?.lua";
		cur_path.append( path.string().c_str() );
	}
#endif

	if (_modulePath[_modulePath.length() - 1] != '/') {
		_modulePath += "/";
	}

	std::string cur_path = "?;";
	cur_path += _modulePath+"?;" + _modulePath + "?.lua;./?;./?.lua";
	lua_pop( _lua, 1 ); // get rid of the string on the stack we just pushed on line 5
	lua_pushstring( _lua, cur_path.c_str() ); // push the new one
	lua_setfield( _lua, -2, "path" ); // set the field "path" in table at -2 with value at top of stack

	// Set player root path
	// this is used by Lua to complete filepath
	lua_pushstring( _lua, _modulePath.c_str() );
	lua_setfield( _lua, LUA_REGISTRYINDEX, "GINGA_PLAYER_ROOT_PATH" ); // set the field "GINGA_PLAYER_ROOT_PATH" in registry with value at top of stack

	//fetch file callback function
	lua_pushcfunction( _lua, _lua_dsmcc );
	lua_setfield( _lua, LUA_REGISTRYINDEX, "GINGA_PLAYER_DSMCC" );

	if (_modulePath[_modulePath.length() - 1] != '/') {
	    _absPath = _modulePath + "/";
	}else{
	    _absPath = _modulePath ;
	}

	lua_pop( _lua, 2 ); // get rid of 2 values passed to LUA_REGISTRYINDEX
}

bool LuaPlayer::startPlay( void ) {
	if (GraphicPlayer::startPlay()) {
		assert(_lua);

		//	Calculate the path from lua file
		LINFO("LuaPlayer", "Start play, source=%s", body().c_str());

		onFetchFilesFromCarousel (body().c_str());

		//	Add path to load library
		addPath( rootPath() );
		
		//	Start canvas
		_canvas->start( surface() );
		return true;
	}
	return false;
}

void LuaPlayer::stopPlayer( event::evtAction::type evtAction ) {
	assert(_lua);
	LINFO("LuaPlayer", "Stop play");

	onPresentationEvent( evtAction );
	
	//	Stop canvas
	_canvas->stop();
	_persistent->stop();

	GraphicPlayer::stopPlay();
}

void LuaPlayer::stopPlay( void ) {
	stopPlayer( event::evtAction::stop );
}

void LuaPlayer::pausePlay( bool pause ) {
	if (pause) {
		onPresentationEvent( event::evtAction::pause );
	} else {
		onPresentationEvent( event::evtAction::resume );
	}
}

void LuaPlayer::abortPlay( void ) {
	stopPlayer( event::evtAction::abort );
}

void LuaPlayer::refresh() {
	//	Load file
	if (luaL_dofile(_lua, body().c_str() )) {
		int i=lua_gettop(_lua);
		std::string desc(lua_tostring(_lua, i));
		std::string path = desc.substr(0,desc.find(":"));
		desc.erase(0,desc.find(":")+1);
		std::string line = desc.substr(0,desc.find(":"));
		desc.erase(0,desc.find(":")+1);
		LERROR("LuaPlayer", "in lua script '%s' at line = %s, %s", path.c_str(), line.c_str(),desc.c_str());
	}
	onPresentationEvent( event::evtAction::start );

}

void LuaPlayer::registerProperties() {
	//	Setup graphic player
	GraphicPlayer::registerProperties();

	{	//	Property attributionEvent
		ForwardProperty<AttributionEventData> *prop = new ForwardProperty<AttributionEventData>(
			boost::bind(&LuaPlayer::sendAttributionEvent,this,_1), true );
		addProperty( property::type::attributionEvent, prop );
	}

	{	//	Property presentationEvent
		ForwardProperty<std::string> *prop = new ForwardProperty<std::string>(
			boost::bind(&LuaPlayer::onPresentationEvent,this,event::evtAction::start,_1), true );
		addProperty( property::type::presentationEvent, prop );
	}

}

void LuaPlayer::setEventCallback( const event::Callback &callback ) {
	_event->setCallback( callback );
}

void LuaPlayer::setInputEventCallback( const event::InputCallback &callback ) {
	_event->setInputEventCallback( callback );
}

void LuaPlayer::onPresentationEvent( player::event::evtAction::type action, const std::string &label/*=""*/ ) {
	_event->dispatchPresentation( action, label );
}

void LuaPlayer::sendAttributionEvent( const AttributionEventData &data ) {
	onAttributionEvent( data.first, data.second );
}

void LuaPlayer::onAttributionEvent( const std::string &name/*=""*/, const std::string &value/*=""*/ ) {
	_event->dispatchAttribution( name, event::evtAction::start, value );
}

void LuaPlayer::dispatchKey( util::key::type key, bool isUp ) {
	_event->dispatchKey( key, isUp );
	GraphicPlayer::dispatchKey( key, isUp );
}

}
