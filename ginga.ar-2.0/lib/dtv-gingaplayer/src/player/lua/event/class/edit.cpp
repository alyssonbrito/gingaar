#include "edit.h"
#include "../event.h"
#include "../../lua.h"
#include <canvas/system.h>

namespace player {
namespace event {
namespace edit {

int postEvent( lua_State *st, bool /*isOut*/, int eventPos ) {
	//	Get event module from stack
	Module *module = Module::get( st );
	if (!module) {
		return luaL_error( st, "[event::edit] Invalid event module" );
	}

	std::string value = lua::getField( st, eventPos, "command" );
	if (value.empty() || value.compare("setPropertyValue") != 0) {
		return luaL_error( st, "[event::edit] Invalid command field in event" );
	}

	std::string nodeId = lua::getField( st, eventPos, "nodeId" );
	if (nodeId.empty()) {
		return luaL_error( st, "[event::edit] Invalid nodeId field in event" );
	}

	std::string propertyId= lua::getField( st, eventPos, "propertyId" );
	if (propertyId.empty()) {
		return luaL_error( st, "[event::edit] Invalid propertyId field in event" );
	}

	value = lua::getField( st, eventPos, "value" );
	if (value.empty()) {
		return luaL_error( st, "[event::edit] Invalid value field in event" );
	}

	module->dispatchOut( evtType::edit, evtAction::start, nodeId, propertyId, value );

	lua_pushboolean( st, 1 );
	return 1;
}

}
}
}
