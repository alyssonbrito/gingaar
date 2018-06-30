#include "system.h"
#include "device.h"
#include "input/manager.h"
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <util/mcr.h>
#include <util/cfg/cfg.h>

namespace player {

System::System()
{
	_input = new input::Manager();

	getPropertyCallback = NULL;
	setPropertyCallback = NULL;
	keySetUpdateCallback = NULL;

	_pem = NULL;
	_settings = NULL;
}

System::~System()
{
	LTRACE( "Port", "[System::%s::%d] Destructor called", __FUNCTION__, __LINE__ );
	delete _input;
	destroy();
}

bool System::canRun() const {
	return !_devices.empty();
}

void System::run() {
	//	Setup reserve keys
	_input->onGlobalKeysChange( boost::bind(&System::onReserveKeys,this,_1) );

	//	Setup devices
	BOOST_FOREACH( Device *dev, _devices ) {
		dev->start();
	}

	//	Run canvas
	canvasSystem()->run();

	LDEBUG( "Port", "System::run exited from canvas" );

	//	Stop devices
	BOOST_FOREACH( Device *dev, _devices ) {
		dev->stop();
	}

	LDEBUG( "Port", "System::run exited from canvas" );

}

void System::exit() {
	//	Stop canvas
	canvasSystem()->exit();
}

void System::addDevice( Device *dev ) {
	_devices.push_back( dev );
}

void System::destroy( Device *dev ) {
	LTRACE( "Port", "[System::%s::%d] Destroy called", __FUNCTION__, __LINE__ );
	std::vector<Device *>::iterator it=std::find(
		_devices.begin(), _devices.end(), dev );
	if (it != _devices.end()) {
		destroyDevice( dev );
		_devices.erase( it );
	}
}

void System::destroy() {
	LTRACE( "Port", "[System::%s::%d] Destroy called", __FUNCTION__, __LINE__ );
	BOOST_FOREACH( Device *dev, _devices ) {
		destroyDevice( dev );
	}
	_devices.clear();
}

void System::destroyDevice( Device *dev ) {
	LTRACE( "Port", "[System::%s::%d] Destroy called", __FUNCTION__, __LINE__ );
	dev->finalize();
	delete dev;
}

template<typename T>
static Device *findDevice( const std::vector<Device *> &devs, T cmp, int num ) {
	std::vector<Device *>::const_iterator it=devs.begin();
	int i = 0;
	while (it != devs.end()) {
		if (cmp((*it))) {
			if (num == i) {
				return (*it);
			}
			else {
				i++;
			}
		}
		it++;
	}
	return NULL;
}

Device *System::getDevice( const std::string &device, int devNumber ) const {
	//	SystemScreen
	size_t found = device.find( "systemScreen" );
	if (found != std::string::npos) {
		return findDevice( _devices, boost::bind(&Device::haveVideo,_1), devNumber );
	}

	//	SystemAudio
	found = device.find( "systemAudio" );
	if (found != std::string::npos) {
		return findDevice( _devices, boost::bind(&Device::haveAudio,_1), devNumber );
	}

	return NULL;
}

void System::onRunTask( canvas::Canvas *canvas, const boost::function<void (void)> &task ) {
	canvas->beginDraw();
	task();
	canvas->endDraw();
}

void System::enqueue( const boost::function<void (void)> &func ) {
	canvasSystem()->enqueue( boost::bind(&System::onRunTask,this,canvasSystem()->canvas(),func) );
}

bool System::dispatchKey( ::util::key::type key, bool isUp ) {
	LDEBUG( "Port", "System::dispatchKey IN key = %d isUp = %d", key, isUp );
	bool res = false;
	std::string keyReservation;

	LDEBUG( "Port", "System::dispatchKey IN key = %d isUp = %d", key, isUp );

	res = _input->dispatchKey ( key, isUp );

	keyReservation = util::key::getKeyReservation ( key );
	// Key is not from reserved group
	if ( keyReservation.empty ( ) ) {
		LWARN( "System", "Key is not from reserved group." );
		return res;
	}

	std::string keyCapture = util::cfg::getValue < std::string > ( "channel.keyCapture" );
	LWARN( "System", "Key Capture = [%s]", keyCapture.c_str ( ) );

	// Key Capture is empty, it's threated as application not reserves keys
	if ( keyCapture.empty ( ) ) {
		LWARN( "System", "Key is not from key capture." );
		return res;
	}

	// Check for not specification values, 'Fora da Norma'
	if ( keyCapture == "true" ) {
		res = true;
	}
	else if ( keyCapture == "false" ) {
		res = false;
	}
	else {
		// Check if key should be consumed
		size_t pos = keyCapture.find ( keyReservation );
		if ( pos != std::string::npos ) {
			res = true;
		}
		else if ( ( keyReservation == "colored_keys" ) || ( keyReservation == "selection_keys" )
				|| ( keyReservation == "cursor_keys" ) ) {
			keyReservation = "interactive_keys";
			size_t pos = keyCapture.find ( keyReservation );
			if ( pos != std::string::npos ) {
				res = true;
			}
		}
	}

	LDEBUG( "Port", "System::dispatchKey OUT key = %d isUp = %d res = %d", key, isUp, res );
	return res;
}

bool System::addInputListener( const input::ListenerId &id, const input::InputCallback &callback, const ::util::key::Keys &keys ) {
	return _input->addInputListener( id, callback, keys );
}

bool System::delInputListener( const input::ListenerId &id ) {
	return _input->delInputListener( id );
}

canvas::System *System::canvasSystem() const {
	assert( _devices[0] );
	return _devices[0]->system();
}

void System::onReserveKeys( const ::util::key::Keys &keys ) {
	BOOST_FOREACH( Device *dev, _devices ) {
		dev->onReserveKeys( keys );
	}
}

//	Editing command
void System::onEditingCommand( const OnEditingCommand &callback ) {
	_onEditingCommand = callback;
}

void System::dispatchEditingCommand( connector::EditingCommandData *event ) {
	if (!_onEditingCommand.empty()) {
		_onEditingCommand( event );
	}
}

//	Timers
util::id::Ident System::registerTimer( ::util::DWORD ms, const canvas::EventCallback &callback ) {
	return canvasSystem()->registerTimer( ms, callback );
}

void System::unregisterTimer( util::id::Ident &timerID ) {
	canvasSystem()->unregisterTimer( timerID );
}

util::id::Ident System::addIO( util::DESCRIPTOR fd, const canvas::EventCallback &callback ) {
	return canvasSystem()->addIO(fd, callback);
}

void System::stopIO( util::id::Ident &id ) {
	canvasSystem()->stopIO( id );
}

//	Socket events
util::id::Ident System::addSocket( util::SOCKET sock, const canvas::EventCallback &callback ) {
	return canvasSystem()->addSocket(sock, callback);	
}

void System::stopSocket( util::id::Ident &id ) {
	canvasSystem()->stopSocket( id );
}

void System::onGetProperty ( const std::string &key, std::string *value ) {

	LDEBUG( "System", "[System::%s::%d] ", __FUNCTION__, __LINE__ );
	if ( getPropertyCallback != 0 ) {
		LDEBUG( "System", "[System::%s::%d]", __FUNCTION__, __LINE__ );
		( *getPropertyCallback ) ( key, value );
	}
}
void System::onSetProperty ( const std::string &key, const std::string &value ) {

	LDEBUG( "System", "[System::%s::%d]", __FUNCTION__, __LINE__ );
	if ( setPropertyCallback != 0 ) {
		LDEBUG( "System", "[System::%s::%d]", __FUNCTION__, __LINE__ );
		( *setPropertyCallback ) ( key, value );
	}

}

void System::onGetNativeProperty ( char * key, char * value ) {

	LDEBUG( "System", "[System::%s::%d] ", __FUNCTION__, __LINE__ );
	if ( getNativePropertyCallback != 0 ) {
		LDEBUG( "System", "[System::%s::%d]", __FUNCTION__, __LINE__ );
		( *getNativePropertyCallback ) ( key, value );
	}
}
void System::onKeySetUpdate ( const std::string &name, const std::string &value ) {

	LDEBUG( "System", "onKeySetUpdate, name=%s, value='%s'", name.c_str ( ), value.c_str ( ) );
	if ( keySetUpdateCallback != 0 ) {
		( *keySetUpdateCallback ) ( name, value );
	}

}

void System::changeVideo( int x1, int y1, int width, int height ) {
	canvasSystem()->changeVideo( x1, y1, width, height );
}

void System::setVisible( bool visible ) {
	canvasSystem()->setVisible( visible );
}

}
