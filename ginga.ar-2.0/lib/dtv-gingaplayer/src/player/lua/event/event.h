#pragma once

#include "types.h" 
#include <canvas/types.h>
#include <util/id/ident.h>
#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <list>

typedef struct lua_State lua_State;

namespace util {
namespace net {
	class Socket;
}
}

namespace player {

class System;
class LuaPlayer;

namespace event {

class Handler;
class DispatchOutInfo;

class Module {
public:
	Module( System *sys, LuaPlayer *player, lua_State *lua );
	virtual ~Module();

	//	Initialization
	void setCallback( const Callback &callback );
	void setInputEventCallback( const InputCallback &callback );	

	//	Lua player methods
	void dispatchKey( util::key::type key, bool isUp );
	void dispatchPresentation( evtAction::type action, const std::string &label );
	void dispatchAttribution( const std::string &name, evtAction::type action, const std::string &value );

	//	Dispatch methods
	typedef boost::shared_ptr<DispatchOutInfo> DispatchOutInfoPtr;

	void dispatchIn( EventImpl *table, bool isInternal );
	void dispatchInCallback( int ref, util::id::Ident &timerID );
	void dispatchOut(
		evtType::type type,
		evtAction::type action,
		const std::string &nodeId,
		const std::string &parameter,
		const std::string &value );
	void dispatchOutCallback( DispatchOutInfoPtr dispatchInfo, util::id::Ident &timerID );

	//	Lua methods (Internal use)
	int postEvent( lua_State *L, const char *evtClass, bool isOut, int eventPos );
	int addHandler( lua_State *L, int pos, int method, const std::string &className, const std::string &classFilter );
	void removeHandler( lua_State *L, int index );
	util::DWORD uptime() const;
	
	//	Timer
	typedef std::pair<util::id::Ident,int> TimerRef;
	typedef std::vector<TimerRef> TimerList;
	void registerTimer( lua_State *L, util::DWORD ms );
	void cancelTimer( int timerRef );
	void cancelTimerAsync( util::id::ID_TYPE id );
	void cancelTimerCallback( util::id::ID_TYPE id, util::id::Ident &internalTimerID );
	void cancelTimer( util::id::ID_TYPE id, bool needUnregister );
	
	//	Socket
	typedef boost::shared_ptr<util::net::Socket> SocketPtr;
	typedef std::map<util::id::Ident, SocketPtr> SocketList;
	util::id::Ident connect( const std::string &host, const char* port, util::DWORD timeout=0 );
	bool disconnect( util::id::ID_TYPE socketID );
	bool send( util::id::ID_TYPE socketID, const std::string &data, util::DWORD timeout=0 );

	//	Aux
	static Module *get( lua_State *st );

	//	SMS
	bool sendSMS( const std::string &to, const std::string &value, const std::string &id );
	void registerSMSHandler( const std::string &port );
	void unregisterSMSHandler( const std::string &port );

protected:
	typedef int (*PostEventMethod)( lua_State *st, bool isOut, int eventPos );	
	typedef std::map<std::string,PostEventMethod> Classes;
	typedef boost::shared_ptr<Handler> HandlerPtr;
	typedef std::vector<HandlerPtr> Handlers;

	//	Timers 
	void cancelTimerAux( TimerList::iterator &it, bool unregister );
	void onTimerExpired( util::id::Ident &timerID );

	//	Sockets
	void disconnect( SocketList::iterator &it );
	void onDataReceived( util::id::Ident &socketID );

private:
	System *_sys;	
	LuaPlayer *_player;
	lua_State *_lua;
	Callback _callback;
	InputCallback _inCallback;
	Classes _classes;
	Handlers _handlers;
	Handlers _handlersToRemove;

	SocketList _sockets;
	TimerList _timers;
	std::list<util::id::Ident> _internalTimers;
};

}	// namespace event
}	// namespace player

