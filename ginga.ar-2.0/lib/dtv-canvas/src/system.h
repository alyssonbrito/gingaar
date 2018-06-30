#pragma once

#include "types.h"
#include "size.h"
#include <boost/thread/mutex.hpp>
#include <vector>
#include "surface.h"

namespace canvas {

class Canvas;
class Size;
class MediaPlayer;
class WebViewer;
class MngViewer;
class Window;
class Surface;
class Input;
class Player;

class System {
public:
	System();
	virtual ~System();

	//	Initialization
	bool initialize( Surface* surface = NULL );
	void finalize();

	//	Run/exit system loop
	virtual void run()=0;
	virtual void exit()=0;

	//	Dispatcher
	typedef boost::function<void (void)> DispatcherTask;
	void enqueue( const DispatcherTask &task );

	//	Notifications
	void setWindowDestroyedCallback( const NotifyCallback &callback );

	//	Input
	void addInput( Input *in );
	bool dispatchKey( util::key::type keyValue, bool isUp );
	bool setInputCallback( const KeyCallback &callback );

	//	Timers
	virtual util::id::Ident registerTimer( util::DWORD ms, const EventCallback &callback )=0;
	virtual void unregisterTimer( util::id::Ident &id )=0;

	//	IO events
	virtual util::id::Ident addIO( util::DESCRIPTOR fd, const EventCallback &callback )=0;
	virtual void stopIO( util::id::Ident &id )=0;

	//	Socket events
	virtual util::id::Ident addSocket( util::SOCKET sock, const EventCallback &callback )=0;
	virtual void stopSocket( util::id::Ident &id )=0;

	//	Web viewer
	virtual WebViewer *createWebViewer( Surface * );

	//	Mng viewer
	virtual MngViewer *createMngViewer( Surface * );

	//	Getters/Setters
	Window *window() const;
	Canvas *canvas() const;
	Player *player() const;

	//	Instance creation
	static System *create( const std::string &use="" );
	void changeVideo( int x1, int y1, int width, int height );
	void setVisible( bool visible );

protected:
	//	Types
	typedef std::vector<Input *> InputList;

	//	Initialization
	virtual bool init();
	virtual void fin();
	void cleanup();

	//	Canvas methods
	virtual Canvas *createCanvas() const;
	bool setupCanvas( const canvas::Size &size );

	//	Player methods
	virtual Player *createPlayer() const;
	bool setupPlayer();

	//	Window methods
	virtual Window *createWindow() const;
	bool setupWindow( Surface* surface );
	void windowDestroyed();

	//	Aux task
	virtual void postEvent( const DispatcherTask &task )=0;

private:
	Window *_win;
	Canvas *_canvas;
	Player *_player;
	InputList _inputs;
	KeyCallback _inputCallback;
	NotifyCallback _onWinDestroyedCallback;
};

}
