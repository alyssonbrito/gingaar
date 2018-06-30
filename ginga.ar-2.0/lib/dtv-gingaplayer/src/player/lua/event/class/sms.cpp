#include "sms.h"
#include "../event.h"
#include "../../lua.h"
#include <util/id/ident.h>
#include <boost/lexical_cast.hpp>

namespace player {
namespace event {
namespace sms {

int postEvent( lua_State * st, bool /*isOut*/, int eventPos ) {
	Module *module = Module::get( st );
	if (!module) {
		return luaL_error( st, "[event::sms] Invalid event module" );
	}

	std::string value = lua::getField( st, eventPos, "type" );
	if (value.empty()) {
		return luaL_error( st, "[event::sms] Invalid type" );
	} else if (value.compare("send") == 0) {
		std::string to = lua::getField( st, eventPos, "to" );
		if (to.empty()) {
			return luaL_error( st, "[event::sms] Invalid to" );
		}

		value = lua::getField( st, eventPos, "value" );
		std::string id = lua::getField( st, eventPos, "id" );

		EventImpl event;
		event["class"] = "sms";
		event["type"] =	"send";
		event["to"] = to;
		event["value"] = value;
		event["id"] = id;

		bool res = module->sendSMS( to, value, id );
		if (res) {
			event["sent"] = "true";
		} else {
			event["error"] = "Sending failed";
		}

		module->dispatchIn( &event, true );

	} else if (value.compare("register") == 0) {
		value = lua::getField( st, eventPos, "port" );
		if (value.empty()) {
			return luaL_error( st, "[event::sms] Invalid port" );
		}
		module->registerSMSHandler(value);

	} else if (value.compare("unregister") == 0) {
		value = lua::getField( st, eventPos, "port" );
		if (value.empty()) {
			return luaL_error( st, "[event::sms] Invalid port" );
		}
		module->unregisterSMSHandler( value );

	} else {
		return luaL_error( st, "[event::sms] Invalid type: type=%s", value.c_str() );
	}

	lua_pushboolean( st, 1 );
	return 1;
}

void dispatchSMSReceived( Module *module, const std::string &from, const std::string &port, const std::string &value ) {
	EventImpl event;
	event["class"] = "sms";
	event["type"] = "receive";
	event["from"] = from;
	event["port"] = port;
	event["value"] = value;
	module->dispatchIn(&event, false);
}

}
}
}
