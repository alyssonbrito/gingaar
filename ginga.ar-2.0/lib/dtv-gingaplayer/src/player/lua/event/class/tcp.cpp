#include "tcp.h"
#include "../event.h"
#include "../../lua.h"
#include <util/id/ident.h>
#include <boost/lexical_cast.hpp>

namespace player {
namespace event {
namespace tcp {

int postEvent( lua_State * st, bool /*isOut*/, int eventPos ) {
	Module *module = Module::get( st );
	if (!module) {
		return luaL_error( st, "[event::tcp] Invalid event module" );
	}

	std::string value = lua::getField( st, eventPos, "type" );
	if (value.empty()) {
		return luaL_error( st, "[event::tcp] Invalid type" );
	}
	else if (value.compare("connect") == 0) {
		std::string host = lua::getField( st, eventPos, "host" );
		if (host.empty()) {
			return luaL_error( st, "[event::tcp] Invalid host" );
		}

		std::string port = lua::getField( st, eventPos, "port" );
		if (port.empty()) {
			return luaL_error( st, "[event::tcp] Invalid port" );
		}

		EventImpl event;
		event["class"] = "tcp";
		event["type"] =	"connect";
		event["host"] = host;
		event["port"] = port;

		util::id::Ident id;
		
		value = lua::getField( st, eventPos, "timeout" );
		if (!value.empty()) {
			id = module->connect( host, port.c_str(), boost::lexical_cast<util::DWORD>(value.c_str()) );
		} else {
			id = module->connect( host, port.c_str());
		}
		
		if (util::id::isValid(id)) {
			event["connection"] = boost::lexical_cast<std::string>(id->getID());
		} else {
			event["error"] = "Connection failed";
		}

		module->dispatchIn( &event, true );

	} else if (value.compare("disconnect") == 0) {
		value = lua::getField( st, eventPos, "connection" );
	
		if (value.empty()) {
			return luaL_error( st, "[event::tcp] Invalid connection" );
		}
		
		bool res = module->disconnect(boost::lexical_cast<util::id::ID_TYPE>(value.c_str()));
		if (!res) {
			return luaL_error( st, "[event::tcp] Invalid connection" );
		}
	}
	else if (value.compare("data") == 0) {
		bool result;
		
		//	Get connection
		value = lua::getField( st, eventPos, "connection" );
		if (value.empty()) {
			return luaL_error( st, "[event::tcp] Invalid connection" );
		}
		util::id::ID_TYPE conn=boost::lexical_cast<util::id::ID_TYPE>(value.c_str());

		//	Get value
		value = lua::getField( st, eventPos, "value" );
		if (value.empty()) {
			return luaL_error( st, "[event::tcp] Invalid value" );
		}
		std::string data=value;

		//	Get timeout
		value = lua::getField( st, eventPos, "timeout" );
		if (!value.empty()) {
			result = module->send( conn, data, boost::lexical_cast<util::DWORD>(value.c_str()) );
		} else {
			result = module->send( conn, data );
		}

		if (!result) {
			return luaL_error( st, "[event::tcp] Error, cannot send data: value=%s", data.c_str() );
		}
	}
	else {
		return luaL_error( st, "[event::tcp] Invalid type: type=%s", value.c_str() );
	}

	lua_pushboolean( st, 1 );
	return 1;
}

void onDataReceived( Module *module, const std::string &val, int socketID) {
	EventImpl event;
	event["class"] = "tcp";
	event["type"] =	"data";
	event["connection"] = boost::lexical_cast<std::string>(socketID);
	event["value"] = val;
	module->dispatchIn( &event, false );
}

}
}
}
