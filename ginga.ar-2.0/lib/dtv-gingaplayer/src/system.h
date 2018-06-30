#pragma once

#include "input/input.h"
#include <canvas/types.h>
#include <canvas/size.h>
#include <util/id/ident.h>
#include <string>
#include <vector>
#include <util/log.h>
#include "player/settings.h"

namespace connector {
	class EditingCommandData;
}

namespace canvas {
	class System;
}

namespace player {

namespace input {
	class Manager;
}

class Device;

class System {
public:
	System();
	virtual ~System();

	//	Start/stop system
	bool canRun() const;
	void run();
	void exit();

	//	Device handling
	void addDevice( Device *dev );
	void destroy( Device *dev );
	void destroy();
	Device *getDevice( const std::string &device, int devNumber ) const;

	//	Enqueue task into player thread
	void enqueue( const boost::function<void (void)> &func );

	//	Input
	bool dispatchKey( ::util::key::type key, bool isUp );
	bool addInputListener( const input::ListenerId &id, const input::InputCallback &callback, const ::util::key::Keys &keys );
	bool delInputListener( const input::ListenerId &id );

	//	Editing command
	typedef boost::function<void (connector::EditingCommandData *event)> OnEditingCommand;
	void onEditingCommand( const OnEditingCommand &callback );
	void dispatchEditingCommand( connector::EditingCommandData *event );

	//	Timers
	util::id::Ident registerTimer( ::util::DWORD ms, const canvas::EventCallback &callback );
	void unregisterTimer( util::id::Ident &id );
	
	//	IO events;
	util::id::Ident addIO( util::DESCRIPTOR fd, const canvas::EventCallback &callback );
	void stopIO( util::id::Ident &id );

	//	Socket events
	util::id::Ident addSocket( util::SOCKET sock, const canvas::EventCallback &callback );
	void stopSocket( util::id::Ident &id );

	typedef boost::function<void (::util::key::type key, bool isUp )> InputCallback;

	inline void setPem(void* pem) { _pem = pem; }
	inline void *getPem() { return _pem; }

	inline void setSettings( player::settings::Settings *settings ) { _settings = settings; }
	inline player::settings::Settings *settings() { return _settings; }

	//Properties
	//Properties
	typedef int (*GetPropertyCallback)(const std::string &key,std::string *value);
	void setOnGetProperty(GetPropertyCallback cb) { getPropertyCallback = cb; }
	void onGetProperty(const std::string &key,std::string *value);
	typedef int (*GetNativePropertyCallback)( char * key, char * value );
	void setOnGetNativeProperty(GetNativePropertyCallback cb) { getNativePropertyCallback = cb; }
	void onGetNativeProperty( char * key, char * value );
	GetNativePropertyCallback getNativePropertyCallback;
	typedef int (*SetPropertyCallback)(const std::string &key,const std::string &value);
	void setOnSetProperty(SetPropertyCallback cb) { setPropertyCallback = cb; }
	void onSetProperty(const std::string &key,const std::string &value);

	typedef int (*KeySetUpdateCallback)(const std::string &name,const std::string &value);
	void setOnKeySetUpdate(KeySetUpdateCallback cb) { keySetUpdateCallback = cb; }
	void onKeySetUpdate( const std::string &name, const std::string &value );

	void changeVideo( int x1, int y1, int width, int height );
	void setVisible( bool visible );

protected:
	canvas::System *canvasSystem() const;
	void destroyDevice( Device *dev );
	void onReserveKeys( const ::util::key::Keys &keys );
	void onRunTask( canvas::Canvas *canvas, const boost::function<void (void)> &task );	

private:
	GetPropertyCallback getPropertyCallback;
	SetPropertyCallback setPropertyCallback;
	KeySetUpdateCallback keySetUpdateCallback;
	
	std::vector<Device *> _devices;
	input::Manager *_input;
	OnEditingCommand _onEditingCommand;
	void *_pem;
	player::settings::Settings *_settings;
};

}
