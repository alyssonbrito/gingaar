#include "../lua.h"
#include "../../luaplayer.h"
#include "../../../system.h"
#include "class/key.h"
#include "class/ncl.h"
#include "class/user.h"
#include "class/tcp.h"
#include "class/si.h"
#include "class/edit.h"
#include "class/sms.h"
#include <canvas/system.h>
#include <util/net/socket.h>
#include <util/net/ipv4/sockaddr.h>
#include <util/net/resolver.h>
#include <util/buffer.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#define LUA_EVENT "lua_module_event"
#define LIMITTIMEOUT 10000
#define DEFAULTTIMEOUT 2500

namespace player {
namespace event {

struct FindTimerByID {
	FindTimerByID( util::id::ID_TYPE timerID ) : _timerID(timerID) {}

	bool operator()( const Module::TimerRef &timer ) const {
		return timer.first->getID() == _timerID;
	}

	util::id::ID_TYPE _timerID;

private:
	// Avoid VisualStudio warning C4512
	FindTimerByID &operator=( FindTimerByID & /*ft*/ ) { return *this; }
};

struct FindTimerByRef {
	FindTimerByRef( int id ) : _id(id) {}

	bool operator()( const Module::TimerRef &timer ) const {
		return timer.second == _id;
	}

	int _id;
};

struct FindSocketByID {
	FindSocketByID( util::id::ID_TYPE id ) : _id(id) {}

	bool operator()( const Module::SocketList::value_type &pair ) const {
		util::id::Ident id = pair.first;
		return util::id::isValid(id) ? (id->getID() == _id) : false;
	}

	util::id::ID_TYPE _id;

private:
	// Avoid VisualStudio warning C4512
	FindSocketByID &operator=( FindSocketByID & /*ft*/ ) { return *this; }
};

class Handler {
public:
	Handler( lua_State *lua, int ref, const std::string &className="any", const std::string &classFilter="" )
		: _lua(lua), _method( ref ), _className(className), _classFilter(classFilter) {}

	virtual ~Handler() {
		luaL_unref(_lua, LUA_REGISTRYINDEX, _method);
	}

	bool isEqual( lua_State *L, int index ) {
		//	Get stored function
		lua_rawgeti( L, LUA_REGISTRYINDEX, _method );
		//	Compare with parameter
		bool res = ( lua_equal( L, index, -1 ) == 1 );
		lua_pop( L, 1 );
		return res;
	}

	bool dispatch( EventImpl *table ) {
		bool result=false;
		const std::string &className = (*table)["class"];

		const std::string &eventType = (*table)["type"];
		if (eventType == "presentation") {
			std::string filterName = (*table)["label"];
			if (_classFilter != filterName) {
				return result;
			}
		}


		if (_className == "any" || _className == className) {
			lua_rawgeti(_lua, LUA_REGISTRYINDEX, _method);

			lua_newtable(_lua);
			BOOST_FOREACH( const EventImpl::value_type &value, *table ) {
				lua_pushstring( _lua, value.second.c_str() );
				lua_setfield( _lua, -2, value.first.c_str() );
			}

			if (lua_pcall(_lua, 1, 1, 0) != 0) {
				LWARN("lua::module::handler::dispatch", "Dispatch error : %s", lua_tostring(_lua, -1));
				lua_pop( _lua, 1 );
				return false;
			}

			result=lua_toboolean( _lua, -1 ) == 1;
			lua_pop( _lua, 1 );
		}
		return result;
	}

	bool dispatchEvent( int ref ) {
		bool result = false;
		lua_rawgeti( _lua, LUA_REGISTRYINDEX, ref );
		std::string className = lua::getField( _lua, -1, "class" );

		std::string typeName = lua::getField( _lua, -1, "type" );
		if (className == "si") {
			if (typeName == "services") {
				lua_getfield(_lua, LUA_GLOBALSINDEX, "si::event");
				if ( !lua_isnil( _lua, -1 ) ) {
					int dataRef = lua_tointeger( _lua, -1);
					lua_pop( _lua, 1);
					lua_rawgeti( _lua, LUA_REGISTRYINDEX, dataRef );
					lua_setfield ( _lua, -2, "data");
					luaL_unref(_lua, LUA_REGISTRYINDEX, dataRef);
				} else {
					lua_pop( _lua, 1);
				}
			}
		}

		if (typeName == "presentation") {
			std::string filterName = lua::getField( _lua, -1, "label" );
			if (_classFilter != filterName) {
				return result;
			}
		}

		if (_className == "any" || _className == className) {
			lua_rawgeti( _lua, LUA_REGISTRYINDEX, _method );
			lua_pushvalue( _lua, -2 );

			if (lua_pcall(_lua, 1, 1, 0) != 0) {
				LWARN("lua::module::handler::dispatchEvent", "Dispatch error : %s", lua_tostring(_lua, -1));
				lua_pop( _lua, 2 );
				return false;
			}

			result = (lua_toboolean( _lua, -1 ) == 1);
			lua_pop( _lua, 1 );
		}

		lua_pop( _lua, 1 );
		return result;
	}

private:
	lua_State *_lua;
	int _method;
	std::string _className;
	std::string _classFilter;
};

/*******************************************************************************
*	Event module
*******************************************************************************/
static int l_post( lua_State *L ) {
	//	(a) evt: event -> sent: boolean; err_msg: string
	//	(b) dst:string, evt: event -> sent: boolean; err_msg: string
	//	dst = [in | out]; default=out

	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "[player::event] Invalid event module" );
	}

	//	Check dst
	bool dstIsOut=true;
	int eventPos=1;
	if (lua_gettop(L) == 2) {
		const char *dst = luaL_checkstring(L, 1);
		if (!dst) {
			return luaL_error(L, "[player::event] Invalid dst");
		}

		if (!strcmp("out",dst)) {
			dstIsOut=true;
		}
		else if (!strcmp( "in", dst )) {
			dstIsOut=false;
		}
		else {
			return luaL_error(L, "[player::event] Invalid dst");
		}

		eventPos=2;
	}

	//	Get class event
	luaL_checktype(L, eventPos, LUA_TTABLE);
	std::string value = lua::getField( L, eventPos, "class" );
	if (value.empty()) {
		return luaL_error(L, "[player::event] Event class nil" );
	}

	return module->postEvent( L, value.c_str(), dstIsOut, eventPos );
}

static int l_cancel( lua_State *L ) {
	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return (int) luaL_error( L, "[player::event] Invalid event module" );
	}

	//	Get timerID
	util::id::ID_TYPE timerID = static_cast<util::id::ID_TYPE>( lua_tonumber(L,lua_upvalueindex(1)) );

	module->cancelTimerAsync( timerID );
	return 0;
}

static int l_timer( lua_State *L ) {
	// event.timer( time: number, f: function ) -> cancel: function

	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "[player::event] Invalid event module" );
	}

	//	Check ms value
	util::DWORD ms = (util::DWORD) luaL_checknumber( L, 1 );

	//	Check function and save in the registry
	luaL_checktype( L, 2, LUA_TFUNCTION );	//	(f)

	//	Register system timer
	module->registerTimer( L, ms );
	return 1;
}

static int l_uptime( lua_State *L ) {	//	TODO: Get uptime from player
	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "[player::event] Invalid event module" );
	}

	util::DWORD ms = module->uptime();
	lua_pushnumber(L, ms);
	return 1;
}

static int l_register( lua_State *L ) {
	//	event.register( [pos:number]; f:function; [class:string; [...:any]] )
	std::string className="any";
	std::string eventType;
	std::string classFilter;
	int index=1;

	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "[player::event] Invalid event module" );
	}

	//	Get pos
	int pos=-1;
	if (lua_type(L, index) == LUA_TNUMBER) {
		pos = (int) luaL_checknumber( L, index );
		if (pos <= 0) {
			return luaL_argerror(L, 1, NULL);
		}
		index++;
	}

	//	Get function
	luaL_checktype(L, index, LUA_TFUNCTION);
	int fncIndex=index;
	index++;

	//	Have class filter?
	if (lua_gettop(L) >= index)	{
		//	Get class name
		className=luaL_checkstring( L, index );
		index++;
	}

	if (lua_gettop(L) >= index)	{
		//	Get event type
		eventType=luaL_checkstring( L, index );
		index++;
	}

	if (lua_gettop(L) >= index)	{
		//	Get class filter
		classFilter=luaL_checkstring( L, index );
		index++;
	}

	//	Add event handler
	return module->addHandler( L, pos, fncIndex, className, classFilter );
}

static int l_unregister( lua_State *L ) {
	//	event.unregister( f:function )

	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "[player::event] Invalid event module" );
	}

	//	Check function
	luaL_checktype( L, 1, LUA_TFUNCTION );	//	(f)

	//	Remove event handler
	module->removeHandler( L, 1 );
	return 0;
}

static const struct luaL_Reg event_methods[] = {
	{ "post",       l_post       },
	{ "timer",      l_timer      },
	{ "uptime",     l_uptime     },
	{ "register",   l_register   },
	{ "unregister", l_unregister },
	{ NULL,         NULL         }
};


/*******************************************************************************
*	Class Module
*******************************************************************************/
Module::Module( System *sys, LuaPlayer *player, lua_State *lua )
	: _sys(sys), _player(player), _lua( lua )
{
	//	Store module into stack
	lua::storeObject( lua, this, LUA_EVENT );

	//	Register classes
	_classes["ncl"]  = &ncl::postEvent;
	_classes["key"]  = &key::postEvent;
	_classes["user"] = &user::postEvent;
	_classes["tcp"] = &tcp::postEvent;
	_classes["si"] = &si::postEvent;
	_classes["edit"] = &edit::postEvent;
	_classes["sms"] = &sms::postEvent;

	//LGESP
 	key::init_keyModule();

	//	Register event methods
	luaL_register( _lua, "event", event_methods );
}

Module::~Module()
{
	{	//	Cleanup timers
		TimerList::iterator it=_timers.begin();
		while (it != _timers.end()) {
			cancelTimer( (*it).first->getID(), true );
			it = _timers.begin();
		}
	}

	{	//	Cleanup sockets
		SocketList::iterator it=_sockets.begin();
		while (it != _sockets.end()) {
			disconnect( it );
			it=_sockets.begin();
		}
	}

	std::for_each(_internalTimers.begin(),
		      _internalTimers.end(),
		      boost::bind(&System::unregisterTimer, _sys, _1));
}

//	Initialization
void Module::setCallback( const Callback &callback ) {
	_callback = callback;
}

void Module::setInputEventCallback( const InputCallback &callback ) {
	_inCallback = callback;
}

//	LuaPlayer methods
void Module::dispatchKey( util::key::type key, bool isUp ) {
	key::dispatchKey( this, key, isUp );
}

void Module::dispatchPresentation( evtAction::type action, const std::string &label ) {
	ncl::dispatchPresentation( this, action, label );
}

void Module::dispatchAttribution( const std::string &name, evtAction::type action, const std::string &value ) {
	ncl::dispatchAttribution( this, name, action, value );
}

//	Dispatch methods
void Module::dispatchInCallback( int ref, util::id::Ident &timerID )
{
	Handlers::iterator it=_handlers.begin();
	while (it != _handlers.end()) {
		HandlerPtr handler = (*it);
		if (handler.get() && handler->dispatchEvent( ref )) {
			break;
		}
		it++;
	}
	_handlersToRemove.clear();

	// delete event
	luaL_unref(_lua, LUA_REGISTRYINDEX, ref);

	std::list<util::id::Ident>::iterator iter = std::find( _internalTimers.begin(), _internalTimers.end(), timerID );
	if (iter != _internalTimers.end()) {
		_internalTimers.erase( iter );
	}
}

void Module::dispatchIn( EventImpl *table, bool isInternal ) {
	//	LG Electronics: #4375
	//	According specification 'ABNTNBR1560602_2007' the next logic should be implemented for procedural player:
	//	A <property> element defined as a child of a <media> element representing a procedural code
	//	may be mapped to a code span (function, method, etc.) or to a code attribute. When it is
	//	mapped to a code span, a link action “set” applied to the property shall cause the code
	//	span execution, with the set values interpreted as input parameters.
	//	Here the value, wrapped by event is passed to procedural code.
	EventImpl::const_iterator it1=table->find( "type" );
	EventImpl::const_iterator it2=table->find( "class" );
	if (it1 != table->end() && it2 != table->end()) {
		if((*it1).second == std::string("attribution") && (*it2).second == std::string("ncl")) {
			it1 = table->find( "name" );
			it2 = table->find( "value" );
			if (it1 != table->end() && it2 != table->end()) {
				lua_getfield( _lua, LUA_GLOBALSINDEX, (*it1).second.c_str());
				if (lua_isfunction(_lua, -1 )) {
					lua_pushstring(_lua, (*it2).second.c_str());
					if (lua_pcall(_lua, 1, 1, 0) != 0) {
						LWARN("lua::module::dispatchIn", "Mapping error : %s", lua_tostring(_lua, -1));
						lua_pop( _lua, 1 );
					}
				} else {
					if (!lua_isnil(_lua, -1)) {
						lua_pushstring(_lua, (*it1).second.c_str());
						lua_pushstring(_lua, (*it2).second.c_str());
						lua_settable(_lua, LUA_GLOBALSINDEX);
					}
					lua_pop(_lua, 1);
				}
			}
		}
	}

	if (isInternal) {
		lua_newtable( _lua );
		BOOST_FOREACH( const EventImpl::value_type &value, *table ) {
			lua_pushstring( _lua, value.second.c_str() );
			lua_setfield( _lua, -2, value.first.c_str() );
		}

		// save event into registry
		int ref = luaL_ref( _lua, LUA_REGISTRYINDEX );

		util::id::Ident DispatchInTimerID = _sys->registerTimer( 1, boost::bind( &Module::dispatchInCallback, this, ref, _1 ) );
		_internalTimers.push_back( DispatchInTimerID );
	} else {
		Handlers::iterator it=_handlers.begin();

#if 1
        // we use the for loop to fix issue LGESP-716
        long unsigned size = _handlers.size();
        for (long unsigned i=0;i<size;i++){
            HandlerPtr handler = (*it);
            if (handler.get() && handler->dispatch( table )) {
                break;
            }
            it++;
            if(size>_handlers.size()){
                size = _handlers.size();
            }
        }
#endif
#if 0		
		//this was was causing a crash when a new handler was being added simultaneously
		while (it != _handlers.end()) {
			HandlerPtr handler = (*it);
			if (handler.get() && handler->dispatch( table )) {
				break;
			}
			it++;
		}
#endif
		_handlersToRemove.clear();
	}

	if (!_inCallback.empty()) {
		_inCallback( table );
	}
}

class DispatchOutInfo {
public:
	DispatchOutInfo(evtType::type type, evtAction::type action, const std::string &nodeId, const std::string &parameter, const std::string &value)
		: type(type), action(action), nodeId(nodeId), parameter(parameter), value(value) {}

	evtType::type type;
	evtAction::type action;
	const std::string nodeId;
	const std::string parameter;
	const std::string value;
};

void Module::dispatchOutCallback(DispatchOutInfoPtr dispatchInfo, util::id::Ident &timerID)
{
	if (!_callback.empty()) {
		_callback(dispatchInfo->type, dispatchInfo->action, dispatchInfo->nodeId, dispatchInfo->parameter, dispatchInfo->value);
	}

	std::list<util::id::Ident>::iterator it = std::find( _internalTimers.begin(), _internalTimers.end(), timerID );
	if (it != _internalTimers.end()) {
		_internalTimers.erase( it );
	}
}


void Module::dispatchOut(
	evtType::type type,
	evtAction::type action,
	const std::string &nodeId,
	const std::string &parameter,
	const std::string &value )
{
	if (!_callback.empty()) {
		DispatchOutInfoPtr dispatchInfo( new DispatchOutInfo( type, action, nodeId, parameter, value ) );
		util::id::Ident DispatchOutTimerID =  _sys->registerTimer( 1, boost::bind( &Module::dispatchOutCallback, this, dispatchInfo, _1 ) );
		_internalTimers.push_back( DispatchOutTimerID );
	}
}

//	Lua event methods
int Module::postEvent( lua_State *L, const char *evtClass, bool isOut, int eventPos ) {
	Classes::const_iterator it=_classes.find( evtClass );
	if (it != _classes.end()) {
		return ((*it).second)( L, isOut, eventPos );
	}
	else {
		return luaL_error( L, "[player::event] Class not implemented: class=%s", evtClass );
	}
}

int Module::addHandler( lua_State *L, int pos, int fncIndex, const std::string &className, const std::string &classFilter ) {
	//	Find handler
	Handlers::const_iterator it=_handlers.begin();
	while (it != _handlers.end()) {
		HandlerPtr handler=(*it);
		if (handler.get() && handler->isEqual( L, fncIndex )) {
			return luaL_error( L, "[player::event] Handler already registered" );
		}
		it++;
	}

	//	Push function into registry
	lua_pushvalue( L, fncIndex );
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);

	HandlerPtr handler( new Handler( _lua, ref, className, classFilter ) );
	if (pos < 0) {
		_handlers.push_back( handler );
	}
	else {
		if(_handlers.size() < static_cast<size_t>( pos )){
			_handlers.resize( pos, HandlerPtr() );
		}
		HandlerPtr removeHandler = _handlers[pos-1];
		if (removeHandler.get()) {
			_handlersToRemove.push_back(removeHandler);
		}

		_handlers[pos-1] = handler;
	}
	return 0;
}

void Module::removeHandler( lua_State *L, int index ) {
	Handlers::iterator it=_handlers.begin();
	while (it != _handlers.end()) {
		HandlerPtr handler = (*it);
		if (handler.get() &&handler->isEqual( L, index )) {
			_handlersToRemove.push_back(handler);
			_handlers.erase(it);
			break;
		}
		it++;
	}
}

util::DWORD Module::uptime() const {
	return _player->uptime();
}

void Module::registerTimer( lua_State *L, util::DWORD ms) {
	//	Register timer
	util::id::Ident timerID=_sys->registerTimer( ms, boost::bind( &Module::onTimerExpired, this, _1 ) );

	//	Push function into registry
	lua_pushvalue( L, 2 );
	int key = luaL_ref( L, LUA_REGISTRYINDEX );

	//	Push cancel function with timerID as upvalue(1)
	lua_pushnumber( L, timerID->getID() );
	lua_pushcclosure( L, l_cancel, 1 );

	_timers.push_back( std::make_pair(timerID,key) );
}

void Module::cancelTimerAsync( util::id::ID_TYPE id ) {
	util::id::Ident cancelLuaTimerID =
		_sys->registerTimer( 0, boost::bind( &Module::cancelTimerCallback, this, id, _1 ) );

	_internalTimers.push_back( cancelLuaTimerID );
}

void Module::cancelTimerCallback( util::id::ID_TYPE id, util::id::Ident &internalTimerID ) {
	cancelTimer(id, true);

	std::list<util::id::Ident>::iterator it = std::find( _internalTimers.begin(),
							     _internalTimers.end(),
							     internalTimerID );

	if ( it != _internalTimers.end() )
		_internalTimers.erase( it );
}

void Module::cancelTimer( int timerRef ) {
	//	Find timer in list
	TimerList::iterator it=std::find_if(
		_timers.begin(),
		_timers.end(),
		FindTimerByRef(timerRef)
	);
	if (it != _timers.end()) {
		cancelTimerAux( it, true );
	}
}

void Module::cancelTimer( util::id::ID_TYPE id, bool needUnregister ) {
	//	Find timer in list
	TimerList::iterator it=std::find_if(
		_timers.begin(),
		_timers.end(),
		FindTimerByID(id)
	);
	if (it != _timers.end()) {
		cancelTimerAux( it, needUnregister );
	}
}

void Module::cancelTimerAux( TimerList::iterator &it, bool needUnregister ) {
	//	Cancel function callback
	luaL_unref(_lua, LUA_REGISTRYINDEX, it->second );

	//	Cancel timer
	if (needUnregister) {
		_sys->unregisterTimer( it->first );
	}

	//	Remove from list
	_timers.erase( it );
}

void Module::onTimerExpired( util::id::Ident &timerID ) {
	//LDEBUG("lua::Event::Module", "On Timer expired: timerID=%p", timerID->getID() );
	bool result=false;
	TimerList::const_iterator it=std::find_if(
		_timers.begin(),
		_timers.end(),
		FindTimerByID(timerID->getID())
	);
	if  (it != _timers.end()) {
		//	Get function from registry into stack
		lua_rawgeti(_lua, LUA_REGISTRYINDEX, it->second);
		if (!lua_isnil( _lua, -1 )) {
			if (lua_pcall(_lua, 0, 0, 0) != 0) {
				LWARN("lua::module::onTimerExpired", "Timer handler error : %s", lua_tostring(_lua, -1));
				lua_pop( _lua, 1 );
			}
			cancelTimer( timerID->getID(), false );
			result=true;
		}
		else {
			lua_pop( _lua, 1 );
		}
	}

	if (!result) {
		LWARN( "lua::Event::Module", "Timer not found" );
	}
}

util::id::Ident Module::connect( const std::string &host, const char* port, util::DWORD timeout ) {
	util::id::Ident socketID;
	std::vector<util::net::SockAddr> addresses;
	if ( !util::net::resolve( host, port, addresses )) {
		LERROR("lua::Event::Module", "Cannot resolve socket address: host=%s, port=%d", host.c_str(), port );
		return socketID;
	}

	//	Adjust the timeout
	if (timeout == 0 || timeout > LIMITTIMEOUT) {
		timeout = DEFAULTTIMEOUT;
	}

	//	Create socket
	SocketPtr sock = boost::make_shared<util::net::Socket>();
	if (!sock->create( util::net::type::stream )) {
		LERROR("lua::Event::Module", "Cannot create socket: host=%s, port=%d", host.c_str(), port );
		return socketID;
	}

	BOOST_FOREACH( util::net::SockAddr sockAddr, addresses ) {
		LDEBUG("lua::Event::Module", "Trying connect: host=%s, sockAddr=%s", host.c_str(), sockAddr.asString().c_str() );

		sock->setRcvTimeout( timeout );

		sock->setSndTimeout( timeout );

		//	Try connect to remote host
		if (!sock->connect( sockAddr )) {
			LERROR("lua::Event::Module", "Cannot connect: host=%s, sockAddr=%s", host.c_str(), sockAddr.asString().c_str() );
			continue;
		}

		//	Set socket non blocking
		if (!sock->setNonBlocking( true )) {
			LERROR("lua::Event::Module", "Cannot set socket non blocking: host=%s, sockAddr=%s", host.c_str(), sockAddr.asString().c_str() );
			continue;
		}

		//	Add socket to dispatcher
		socketID = _sys->addSocket( sock->fd(), boost::bind(&Module::onDataReceived, this, _1));
		if (util::id::isValid(socketID)) {
			LDEBUG("lua::Event::Module", "Socket connected: socket=%d", sock->fd() );
			_sockets[socketID] = sock;
			break;
		}
	}

	return socketID;
}


bool Module::disconnect( util::id::ID_TYPE socketID ) {
	bool res = false;
	SocketList::iterator it=std::find_if(
		_sockets.begin(),
		_sockets.end(),
		FindSocketByID(socketID)
	);
	if (it != _sockets.end()) {
		disconnect( it );
		res = true;
	}
	return res;
}

void Module::disconnect( SocketList::iterator &it ) {
	LDEBUG( "lua::Event::Module", "disconnect socketID: %ld", it->first->getID() );

	//	Stop listen for data
	util::id::Ident id = it->first;
	_sys->stopSocket( id );

	//	Disconnect and cleanup
	SocketPtr sock = it->second;
	sock->close();

	_sockets.erase( it );
}

void Module::onDataReceived( util::id::Ident &socketID ) {
	LDEBUG("lua::Event::Module", "onDataReceived socketID: %li", socketID->getID());
	SocketList::iterator it=_sockets.find( socketID );
	if (it != _sockets.end()) {
		SocketPtr sock = it->second;
		util::Buffer buf( 1024 );
		util::SSIZE_T len = sock->recv( buf.buffer(), buf.capacity() );
		while (len > 0) {
			{	//	Notify data
				std::string tmp( buf.buffer(), len );
				tcp::onDataReceived( this, tmp, socketID->getID() );
			}

			len = sock->recv( buf.buffer(), buf.capacity() );
		}
		if (len == 0 && sock->isOpen()) { //	No data and peer has performed an orderly shutdown ... close socket!
			LWARN("lua::Event::Module", "Connection closed, disconnect socket" );
			disconnect( it );
		}
	}
}

bool Module::send( util::id::ID_TYPE socketID, const std::string &data, util::DWORD timeout ) {
	bool res = false;
	SocketList::iterator it=std::find_if(
		_sockets.begin(),
		_sockets.end(),
		FindSocketByID(socketID)
	);
	if (it != _sockets.end()) {
		LDEBUG( "lua::Event::Module", "Send data to connection: %ld", socketID );
		it->second->setSndTimeout( timeout );
		util::SSIZE_T bytes=it->second->send( data.c_str(), data.length() );
		res=bytes > 0 && static_cast<size_t>(bytes) == data.length();
		if (!res) {
			LWARN("lua::Event::Module", "Connection error, disconnect socket: bytes=%d", bytes );
			disconnect( it );
		}
	}
	else {
		LWARN("lua::Event::Module", "Socket not found" );
	}
	return res;
}

Module *Module::get( lua_State *st ) {
	return lua::getObject<Module>(st,LUA_EVENT);
}

bool Module::sendSMS( const std::string &/*to*/, const std::string &/*value*/, const std::string &/*id*/ ) {
	return false;
}

void Module::registerSMSHandler( const std::string &/*port*/ ) {
}

void Module::unregisterSMSHandler( const std::string &/*port*/ ) {
}

}
}

