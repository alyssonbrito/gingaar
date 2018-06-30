#include "ncl.h"
#include "../../lua.h"
#include <util/log.h>
#include <ginga_api.h>

namespace player {
namespace event {
namespace ncl {

int postEvent( lua_State *st, bool isOut, int eventPos ) {
	//	Get event module from stack
	Module *module = Module::get( st );
	if (!module) {
		return luaL_error( st, "[event::ncl] Invalid event module" );		
	}

	//	Get type of event
	std::string value = lua::getField( st, eventPos, "type" );
	if (value.empty()) {
		return luaL_error( st, "[event::ncl] Invalid type field in event" );
	}
	evtType::type type = getEventType( value.c_str() );
	if (type == evtType::unknown) {
		return luaL_error( st, "[event::ncl] Invalid type: type=%s\n", value.c_str() );
	}

	evtAction::type action;
	if (type != evtType::edit) {
		//	Get action
		value = lua::getField( st, eventPos, "action" );
		//	LG Electronics
		//	old spec (2007) support
		if (value.empty()) {
			value = lua::getField( st, eventPos, "transition" );
		}
		if (value.empty()) {
			return luaL_error( st, "[event::ncl] Invalid action field in event" );
		}
		action = getEventAction( value.c_str() );
		if (action == evtAction::unknown){
			return luaL_error( st, "[event::ncl] Invalid action: action=%s", value.c_str() );
		}
	}

	switch (type) {
		case evtType::presentation: {
			std::string label = lua::getField( st, eventPos, "label" );
			//	LG Electronics
			//	old spec (2007) support
			if (label.empty()) {
				label = lua::getField( st, eventPos, "area" );
			}

			if (isOut){
				module->dispatchOut( evtType::presentation, action, "", label, "" );
			}else{
				dispatchPresentation( module, action, label, true );
			}

			break;
		}
		case evtType::attribution: {
			std::string name = lua::getField( st, eventPos, "name" );
			//	LG Electronics
			//	old spec (2007) support
			if (name.empty()) {
				name = lua::getField( st, eventPos, "area" );
			}
			if (name.empty()) {
				return luaL_error( st, "[event::ncl] Invalid name field in event" );
			}

			value = lua::getField( st, eventPos, "value" );
			if (value.empty()) {
				LWARN("[event::ncl]", "Invalid value field in event name: %s", name.c_str());
			}

			if (isOut){
				module->dispatchOut( evtType::attribution, action, "", name, value );
			}else{
				dispatchAttribution( module, name, action, value, true );
			}

			break;
		}
		case evtType::selection: {
			value = lua::getField( st, eventPos, "label" );
		//	LG Electronics
		//	old spec (2007) support
			if (value.empty()) {
				value = lua::getField( st, eventPos, "area" );
			}
			if (value.empty()) {
				return luaL_error( st, "[event::ncl] Invalid label field in event" );
			}
			module->dispatchOut( evtType::selection, action, "", value, "" );
			break;
		}
		case evtType::edit: {
			value = lua::getField( st, eventPos, "command" );
			if (value.empty() || value.compare("setPropertyValue") != 0) {
				break;
			}
			std::string nodeId = lua::getField( st, eventPos, "nodeId" );
			if (nodeId.empty()) {
				break;
			}
			std::string propertyId= lua::getField( st, eventPos, "propertyId" );
			if (propertyId.empty()) {
				break;
			}
			value = lua::getField( st, eventPos, "value" );
			if (value.empty()) {
				break;
			}
			module->dispatchOut( evtType::edit, evtAction::start, nodeId, propertyId, value );
			break;
		}
		default:
			break;
	}

	lua_pushboolean( st, 1 );
	return 1;
}

void dispatchPresentation( Module *module, evtAction::type action, const std::string &label, bool isInternal ) {
	EventImpl event;
	event["class"] = "ncl";
	event["type"] = "presentation";
	event["label"] = label;
	event["action"] = getActionName( action );
	module->dispatchIn( &event, isInternal );
}

void dispatchAttribution( Module *module, const std::string &name, evtAction::type action, const std::string &value, bool isInternal ) {
	EventImpl event;
	event["class"] = "ncl";
	event["type"] = "attribution";
	event["name"] = name;
	event["action"] = getActionName( action );
	event["value"] = value;
	module->dispatchIn( &event, isInternal );
}	

}
}
}
