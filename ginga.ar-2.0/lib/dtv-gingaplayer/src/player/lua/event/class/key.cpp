#include "key.h"
#include "../../lua.h"
#include "../../../../input/manager.h"
#include <canvas/system.h>

#include <pthread.h>
#include <ginga_api.h>

namespace player {
namespace event {
namespace key {

//	Event key types
enum type {
	unknown = 0,
	press,
	release
};

static type getKeyEventType( const char *name ) {
	if (!strcmp( name, "press" )){
		return press;
	}else if (!strcmp( name, "release" )){
		return release;
	}else{
		return unknown;
	}
}

int postEvent( lua_State *st, bool isOut, int eventPos ) {
	//	Get event type
	std::string value = lua::getField( st, eventPos, "type" );
	if (value.empty()) {
		return luaL_error( st, "[event::key] Invalid type field in event" );
	}
	type type = getKeyEventType( value.c_str() );

	if (type != unknown) {
		//	Get key field
		value = lua::getField( st, eventPos, "key" );
		if (value.empty()) {
			return luaL_error( st, "[event::key] Invalid key field in event" );
		}
		util::key::type key = util::key::getKey( value.c_str() );

		if (key != util::key::null){
			//	Get event module from stack
			Module *module = Module::get( st );
			if (!module) {
				return luaL_error( st, "[player::event::key] Invalid event module" );		
			}

			if (isOut) {
				module->dispatchKey( key, type == release );
			}
			else {				
				dispatchKey( module, key, type == release, true );
			}

		} else {
			return luaL_error( st, "[player::event::key], Invalid key: key=%s", value.c_str() );
		}

	} else {
		return luaL_error( st, "[player::event::key] Invalid type: type=%s", value.c_str() );
	}

	lua_pushboolean( st, 1 );
	return 1;
}

//LGESP
static pthread_mutex_t key_lock;
#define LOCK_DISPATCHKEY() pthread_mutex_lock(&key_lock)
#define UNLOCK_DISPATCHKEY() pthread_mutex_unlock(&key_lock)
#define INIT_DISPATCHKEY() pthread_mutex_init(&key_lock,NULL)

void init_keyModule(){
    assert(INIT_DISPATCHKEY()==0);
}
//LGESP END

void dispatchKey( Module *module, util::key::type key, bool isUp, bool isInternal ) {
	//LGESP
 	//LOCK_DISPATCHKEY(); 
	isInternal = false; //LGESP NEGINGA-199 

	EventImpl event;
	event["class"] = "key";
	event["type"] = isUp ? "release" : "press";
	event["key"] = util::key::getKeyName( key );
	module->dispatchIn( &event, isInternal );

	//LGESP
	//UNLOCK_DISPATCHKEY();

}

}
}
}
