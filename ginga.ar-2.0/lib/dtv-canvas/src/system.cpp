#include "system.h"
#include "canvas.h"
#include "window.h"
#include "input.h"
#include "player.h"
#include "webviewer.h"
#include "mngviewer.h"
#include "custom/system.h"

#include "generated/config.h"
#if SYS_SYSTEM_USE_SDL
#       include "impl/sdl/system.h"
#endif
#if SYS_SYSTEM_USE_GTK
#       include "impl/gtk/system.h"
#endif
#if SYS_SYSTEM_USE_X11
#       include "impl/x11/system.h"
#endif
#if SYS_SYSTEM_USE_DIRECTFB
#       include "impl/directfb/system.h"
#endif
#if SYS_SYSTEM_USE_QT
#       include "impl/qt/system.h"
#endif

#include <util/log.h>
#include <util/cfg/configregistrator.h>
#include <util/task.h>
#include <util/mcr.h>
#include <boost/bind.hpp>

namespace canvas {

REGISTER_INIT_CONFIG( gui ) {
	root().addNode( "gui" )
#if SYS_SYSTEM_USE_GTK
		.addValue( "use", "System to initialize", std::string("gtk") );
#elif SYS_SYSTEM_USE_SDL
		.addValue( "use", "System to initialize", std::string("sdl") );
#elif SYS_SYSTEM_USE_DIRECTFB
		.addValue( "use", "System to initialize", std::string("directfb") );
#elif SYS_SYSTEM_USE_X11
		.addValue( "use", "System to initialize", std::string("x11") );
#elif SYS_SYSTEM_USE_QT
		.addValue( "use", "System to initialize", std::string("qt") );
#elif SYS_SYSTEM_USE_CUSTOM
		.addValue( "use", "System to initialize", std::string("custom") );
#else
		.addValue( "use", "System to initialize", std::string("dummy") );
#endif
}

System *System::create( const std::string &useParam/*=""*/ ) {
	System *sys=NULL;
	std::string use = useParam.empty() ? util::cfg::getValue<std::string> ("gui.use") : useParam;

	LINFO("System", "Using system: use=%s", use.c_str() );

#if SYS_SYSTEM_USE_SDL
	if (use == "sdl") {
		sys = new sdl::System();
	}
#endif

#if SYS_SYSTEM_USE_GTK
	if (use == "gtk") {
		sys = new gtk::System();
	}
#endif

#if SYS_SYSTEM_USE_QT
	if (use == "qt") {
		sys = new qt::System();
	}
#endif

#if SYS_SYSTEM_USE_DIRECTFB
	if (use == "directfb") {
		sys = new directfb::System();
	}
#endif

#if SYS_SYSTEM_USE_X11
	if (use == "x11") {
		sys = new x11::System();
	}
#endif

	if (use == "custom") {
		sys = new custom::System();
	}

	if (sys) {
		Input::create(sys);
	}

	return sys;
}

System::System() {
	_win = NULL;
	_canvas = NULL;
	_player = NULL;
}

System::~System() {
	CLEAN_ALL( Input *, _inputs );
	assert(!_win);
	assert(!_canvas);
	assert(!_player);
}

void System::windowDestroyed() {
	LINFO("System", "Window closed")
	if (!_onWinDestroyedCallback.empty()) {
		_onWinDestroyedCallback();
	} else {
		exit();
	}
}

void System::setWindowDestroyedCallback( const NotifyCallback &callback ) {
	_onWinDestroyedCallback = callback;
}

//	Initialization
bool System::initialize( Surface* surface ) {
	LDEBUG("System", "Initialize");

	if (!surface && !init()) {
		return false;
	}

	if (_inputCallback.empty()) {
		LINFO("System", "input callback empty");
	}

	//	Create and initialize window
	if (!setupWindow(surface)) {
		cleanup();
		return false;
	}

	//	Get canvas size from config
	canvas::Size size;
	if (surface) {
		size = surface->getSize();
	} else {
		util::cfg::PropertyNode &sizeNode = util::cfg::get("gui.canvas.size");
		size.w = sizeNode.get<int>( "width" );
		size.h = sizeNode.get<int>( "height" );
	}

	//	Create and initialize canvas
	if (!setupCanvas( size )) {
		cleanup();
		return false;
	}

	//	Create and initialize player
	if (!setupPlayer()) {
		cleanup();
		return false;
	}

	//	Initialize inputs
	BOOST_FOREACH( Input *input, _inputs ) {
		if (!input->initialize()) {
			LWARN("System", "Cannot initialize input: name=%s\n", input->name().c_str() );
		}
	}

	return true;
}

void System::cleanup() {
	if (_canvas) {
		_canvas->finalize();
		DEL(_canvas);
	}

	if (_win) {
		_win->finalize();
		DEL(_win);
	}

	if (_player) {
		_player->finalize();
		DEL(_player);
	}
}

void System::finalize() {
	LDEBUG("Port", "Finalize");
	//	Finalize inputs
	BOOST_FOREACH( Input *input, _inputs ) {
		input->finalize();
	}
	cleanup();
	fin();
}

bool System::init() {
	return true;
}

void System::fin() {
}

//	Tasks
void System::enqueue( const DispatcherTask &task ) {
	postEvent( task );
}

//	Input
bool System::setInputCallback( const KeyCallback &callback ) {
	//	Setup input callback
	_inputCallback = callback;
	return true;
}

//	HTML viewer
WebViewer *System::createWebViewer( Surface *surface ) {
	return WebViewer::create( this, surface );
}

//	MNG viewer
MngViewer *System::createMngViewer( Surface *surface ) {
	return MngViewer::create( this, surface );
}

//	Canvas
bool System::setupCanvas( const canvas::Size &size ) {
	_canvas = createCanvas();
	if (!_canvas) {
		LWARN("System", "cannot create canvas");
		return false;
	}

	_canvas->setSize(size);

	if (!_canvas->initialize( _win )) {
		LWARN("System", "cannot initialize canvas");		
		DEL(_canvas);
		return false;
	}

	return true;
}

Canvas *System::createCanvas() const {
	return Canvas::create();
}

Canvas *System::canvas() const {
	assert(_canvas);
	return _canvas;
}

//	Window
Window *System::createWindow() const {
	return Window::create();
}

Window *System::window() const {
	assert(_win);
	return _win;
}

bool System::setupWindow( Surface* surface ) {
	//	Create and initialize window
	_win = createWindow();
	if (!_win) {
		LWARN("System", "cannot create display");
		return false;
	}

	_win->setSys(this);

	if (!_win->initialize(surface)) {
		LWARN("System", "cannot initialize display");		
		DEL(_win);
		return false;
	}

	return true;
}

//	Player
Player *System::createPlayer() const {
	return Player::createPlayer();	
}

bool System::setupPlayer() {
	_player = createPlayer();
	if (!_player) {
		LWARN("System", "cannot create player");
		return false;
	}

	if (!_player->initialize()) {
		LWARN("System", "cannot initialize player");		
		DEL(_player);
		return false;
	}

	return true;
}

Player *System::player() const {
	return _player;
}

//	Input
void System::addInput( Input *in ) {
	_inputs.push_back( in );
}

bool System::dispatchKey( util::key::type keyValue, bool isUp ) {
	bool res = false;
	std::string keyReservation;
	if (!_inputCallback.empty()) {
		res = _inputCallback( keyValue, isUp );
	}
	return res;
}

void System::changeVideo( int x1, int y1, int width, int height ) {
	_win->changeVideo( x1, y1, width, height );
}

void System::setVisible( bool visible ) {
	_win->setVisible( visible );
}


}
