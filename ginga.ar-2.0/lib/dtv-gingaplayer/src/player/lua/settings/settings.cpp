#include "settings.h"
#include "../../settings.h"
#include "../lua.h"
#include "../../luaplayer.h"
#include <string>
#include <string.h>
#include <assert.h>
#include "../../../device.h"
#include "../../../system.h"
#include <util/functions.h>
#include <math.h>

#define LUA_SETTINGS  "lua_settings_module"
#define LUA_SETTINGS_META   "lua_settings_meta"
#define LUA_SETTINGS_GROUPS "lua_settings_groups"
#define FOCUS_BORDER_TRANSPARENCY "focusBorderTransparency"

namespace player {
namespace settings {

static bool _DeviceClassDefined = true;

void createObject( lua_State *st, int id ) {
	APRINT_LUA("<==>");
	int *ptr = (int*)lua_newuserdata(st, sizeof(int));
	*ptr = id;
	lua_getfield(st, LUA_REGISTRYINDEX, LUA_SETTINGS_META);
	lua_setmetatable(st, -2);
}

int indexHandler( lua_State *st ) {
	APRINT_LUA("==>");
	std::string value = "";
	Module *module = Module::get(st);
	int *id = (int*)lua_touserdata(st, 1);
	if (tokens::screenSize <= *id && *id <= tokens::info) {
		int i = lua_tointeger(st, 2);
		APRINT_LUA("i[%d]",i);
		module->settings()->getIndexProperty( *id, i-1, value );
	} else if (lua_isstring(st, 2)) {
		std::string name = lua_tostring(st, 2);
		int group = module->settings()->getPropertyOrGroup( *id, name, value, true );
		APRINT_LUA("name[%s] value[%s] group[%d]",name.c_str(),value.c_str(),group);
		if (group > tokens::unknown) {
			APRINT_LUA("---");
			if (!_DeviceClassDefined && (tokens::screenSize <= group) && (group <= tokens::audioType)) {
				module->settings()->getIndexProperty( *id, 0, value );
				APRINT_LUA("value[%s]",value.c_str());
			} else {
				APRINT_LUA("---");
				lua_getfield(st, LUA_REGISTRYINDEX, LUA_SETTINGS_GROUPS);
				lua_rawgeti(st, -1, group);
				if (!lua_isuserdata(st, -1)) {
					APRINT_LUA("---");
					lua_pop(st, 1);
					createObject(st, group);
					lua_pushvalue(st, -1);      // copy object
					lua_rawseti(st, -3, group); // store object in LUA_SETTINGS_GROUPS
				}
				APRINT_LUA("<== return[1]");
				return 1;
			}
		}else{
			APRINT_LUA("token not found");
		}
	}
	//LGESP - check if number
	try{
		double dnum = boost::lexical_cast<double>(value);
		lua_pushnumber(st,dnum);
		APRINT_LUA("pushing number[%f]",dnum);
	}catch(boost::bad_lexical_cast &e){
		lua_pushstring(st, value.c_str());
		APRINT_LUA("pushing string[%s]",value.c_str());
	}
	APRINT_LUA("<== return[1] valueasString[%s]",value.c_str());
	return 1;
}

int newindexHandler( lua_State *st ) {
	APRINT_LUA("==>");
	int *id = (int*)lua_touserdata(st, 1);
	if (*id != tokens::defaults) {
		APRINT_LUA("<== 0");
		return 0;
	}

	if (!lua_isstring(st, 2)) {
		APRINT_LUA("<== 0");
		return 0;
	}
	std::string name = lua_tostring(st, 2);

	const char *str = lua_tostring(st, 3);
	if (!str) {
		APRINT_LUA("<== 0");
		return 0;
	}

	std::string value = str;
	if (value.empty()) {
		APRINT_LUA("<== 0");
		return 0;
	}

	//LGESP - converting to number
	if (name==FOCUS_BORDER_TRANSPARENCY){
		APRINT_LUA("Transparency. Converting to number");
		double dnum=0;
		if (util::convert2Percentual(value,&dnum)==false){
			APRINT("<== EXCEPTION! Conversion error [%s]",value.c_str());
			return 0;
		}
		value = boost::lexical_cast<std::string>(dnum);
		APRINT_LUA("New value[%f][%s]",dnum,value.c_str());
	}

	Module *module = Module::get( st );
	module->settings()->setProperty( *id, name, value, false );

	APRINT_LUA("<== 0 name[%s] value[%s]",name.c_str(),value.c_str());
	return 0;
}

int concatHandler( lua_State *st ) {
	APRINT_LUA("==>");
	std::string res = "";
	Module *module = Module::get( st );

	int *id = (int*)lua_touserdata(st, 1);
	if (id) {
		module->settings()->getIndexProperty( *id, 0, res );
	} else {
		res = lua_tostring(st, 1);
	}

	std::string s_add = "";
	id = (int*)lua_touserdata(st, 2);
	if (id) {
		module->settings()->getIndexProperty( *id, 0, s_add );
	} else {
		s_add = lua_tostring(st, 2);
	}

	res += s_add;
	lua_pushstring(st, res.c_str());
	APRINT_LUA("<== res[%s]",res.c_str());
	return 1;
}

int tostringHandler( lua_State *st ) {
	std::string res = "";
	int *id = (int*)lua_touserdata(st, 1);
	Module *module = Module::get( st );
	module->settings()->getIndexProperty( *id, 0, res );
	lua_pushstring(st, res.c_str());
	APRINT_LUA("<==> res[%s]",res.c_str());
	return 1;
}

int callHandler( lua_State *st ) {
	int *id = (int*)lua_touserdata(st, 1);
	int index = 0;
	if (lua_gettop(st) == 2) {
		index = luaL_checkint(st, 2) - 1;
	}

	std::string res = "";
	Module *module = Module::get( st );
	module->settings()->getIndexProperty( *id, index, res );

	lua_pushstring(st, res.c_str());
	APRINT_LUA("<==> res[%s]",res.c_str());
	return 1;
}

int lenHandler( lua_State *st ) {
	APRINT_LUA("<==>");
	std::string res = "";
	int *id = (int*)lua_touserdata(st, 1);
	Module *module = Module::get( st );
	module->settings()->getIndexProperty( *id, 0, res );
	lua_pushinteger(st, res.length());
	return 1;
}

/*******************************************************************************
 *	Settings module
 *******************************************************************************/
Module::Module( LuaPlayer *player, lua_State *st ) :
		_player( player ),
		_lua( st )
{
	APRINT_LUA("<==>");
	_settings = _player->device()->systemPlayer()->settings();

	//	Store module into stack
	lua::storeObject( _lua, this, LUA_SETTINGS );

	lua_newtable(_lua); // settings metatable
	lua_pushcfunction(_lua, indexHandler);
	lua_setfield(_lua, -2, "__index");
	lua_pushcfunction(_lua, newindexHandler);
	lua_setfield(_lua, -2, "__newindex");
	lua_pushcfunction(_lua, concatHandler);
	lua_setfield(_lua, -2, "__concat");
	lua_pushcfunction(_lua, tostringHandler);
	lua_setfield(_lua, -2, "__tostring");
	lua_pushcfunction(_lua, callHandler);
	lua_setfield(_lua, -2, "__call");
	lua_pushcfunction(_lua, lenHandler);
	lua_setfield(_lua, -2, "__len");
	lua_setfield(_lua, LUA_REGISTRYINDEX, LUA_SETTINGS_META); // save settings metatable

	lua_createtable(_lua, tokens::LAST, 0); // settings.groups
	lua_setfield(_lua, LUA_REGISTRYINDEX, LUA_SETTINGS_GROUPS);

	createObject(_lua, tokens::unknown);
	lua_setfield(_lua, LUA_GLOBALSINDEX, "settings" );  // add settings to global context
}

Module::~Module() {
	APRINT_LUA("<==>");
}

Module *Module::get( lua_State *st ) {
	return lua::getObject<Module>( st, LUA_SETTINGS );
}

}
}
