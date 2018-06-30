#include "si.h"
#include "../event.h"
#include "../../lua.h"
#include <canvas/system.h>
#include <boost/foreach.hpp>
#include <util/log.h>

namespace player {
namespace event {
namespace si {

EventTypeService first;
SiStream firstStream;
std::vector<EventTypeService*> testingServices;


SiStream::SiStream()
	: _pid( 0 )
	, _componentTag( 0 )
	, _type( 0 )
	, _regionSpecType( 0 )
{
}

EventTypeService::EventTypeService()
	: _id( 0 )
	, _isAvailable( false )
	, _isPartialReception( false )
	, _parentalControlRating ( 0 )
	, _runningStatus( 0 )
	, _serviceType( 0 )
{
}

std::vector<EventTypeService*> getServices() {
	//	Inflate with testing parameters
	first.setId(64);
	first.setAvailable(true);
	first.setPartialReception(false);
	first.setParentControlRating(0);
	first.setRunningStatus(0);
	first.setServiceType(255) ;
	first.setProviderName(std::string(""));
	first.setServiceName(std::string(""));

	firstStream.setPid(768);
	firstStream.setComponentTag(0);
	firstStream.setType(27);
	firstStream.setRegionSpecType(0);
	firstStream.setRegionSpec(std::string(""));

	first.addStream(firstStream);


	testingServices.push_back(&first);

	return testingServices;
}

void fillService(lua_State *st) {

	int index = 1;
	std::vector<EventTypeService*> services = getServices(); // get testing services
	BOOST_FOREACH( EventTypeService* service, services) {

		lua_pushinteger(st, index++);

		//	Setting the main table
		lua_newtable( st );

		lua_pushinteger(st, service->id());
		lua_setfield(st, -2, "id");
		lua_pushboolean(st, service->isAvailable());
		lua_setfield(st, -2, "isAvailable");
		lua_pushboolean(st, service->isPartialReception());
		lua_setfield(st, -2, "isPartialReception");
		lua_pushinteger(st, service->parentalControlRating());
		lua_setfield(st, -2, "parentalControlRating");
		lua_pushinteger(st, service->runningStatus());
		lua_setfield(st, -2, "runningStatus");
		lua_pushinteger(st, service->serviceType());
		lua_setfield(st, -2, "serviceType");
		lua_pushstring(st, service->providerName().c_str());
		lua_setfield(st, -2, "providerName");
		lua_pushstring(st, service->serviceName().c_str());
		lua_setfield(st, -2, "serviceName");

		//	Setting the stream table
		lua_newtable( st );

		int j = 1;
		std::vector<SiStream*> streams = service->streams();
		BOOST_FOREACH( SiStream* stream, streams) {

			lua_pushinteger(st, j++);

			lua_newtable( st );

			lua_pushinteger(st, stream->pid());
			lua_setfield(st, -2, "pid");
			lua_pushinteger(st, stream->componentTag());
			lua_setfield(st, -2, "componentTag");
			lua_pushinteger(st, stream->type());
			lua_setfield(st, -2, "type");
			lua_pushinteger(st, stream->regionSpecType());
			lua_setfield(st, -2, "regionSpecType");
			lua_pushstring(st, stream->regionSpec().c_str());
			lua_setfield(st, -2, "regionSpec");

			lua_settable(st, -3);

		}

		lua_setfield(st, -2, "stream");



		//	Setting the table to global context
		lua_settable(st, -3);
	}
}

int postEvent( lua_State *st, bool /*isOut*/, int eventPos ) {
	//	Get event module from stack
	Module *module = Module::get( st );
	if (!module) {
		return luaL_error( st, "[event::ncl] Invalid event module" );
	}

	EventImpl table;
	lua_pushnil(st);  // First key
	while (lua_next(st, eventPos) != 0) {
		const char *key=lua_tostring(st,-2);
		const char *value=lua_tostring(st,-1);
		table[key] = value;

		//	Removes 'value'; keeps 'key' for next iteration
		lua_pop(st, 1);
	}


	std::string eventType;
	EventImpl::const_iterator it=table.find( "type" );
	if (it != table.end()) {
		eventType =  (*it).second;
	}

	if (eventType == std::string("services")) {
		LWARN("Event::si", "Services type si event support, testing data");
		//	First, set table
		lua_newtable( st );

		fillService(st);

		//	Free referred object if it exists
		lua_getfield( st, LUA_GLOBALSINDEX, "si::event");
		if ( !lua_isnil( st, -1 ) ) {
			int dataRef = lua_tointeger( st, -1);
			lua_pop( st, 1);
			luaL_unref(st, LUA_REGISTRYINDEX, dataRef);
		} else {
			lua_pop( st, 1);
		}

		// Save event into registry
		int ref = luaL_ref( st, LUA_REGISTRYINDEX );
		lua_pushinteger(st, ref);
		lua_setfield(st, LUA_GLOBALSINDEX, "si::event");
	} else if (eventType == std::string("mosaic")) {
		LWARN("Event::si", "Mosaic type si event support is not implemented ");
	} else if (eventType == std::string("epg")) {
		LWARN("Event::si", "Epg type event si support is not implemented ");
	} else if (eventType == std::string("time")) {
		LWARN("Event::si", "Time type event si support is not implemented ");
	} else {
		LWARN("Event::si", "Unknown si event type");
	}

	//	Dispatch table
	dispatchSi( module, &table );

	lua_pushboolean( st, 1 );
	return 1;
}

void dispatchSi( Module *module, EventImpl *table ) {
	module->dispatchIn( table, true );
}

}
}
}
