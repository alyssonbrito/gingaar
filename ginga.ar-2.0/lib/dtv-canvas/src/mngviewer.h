#pragma once

#include <string>
#include <boost/function.hpp>
#include "util/keydefs.h"

namespace canvas {

class Point;
class Size;
class System;
class Surface;

class MngViewer {
public:
	explicit MngViewer( Surface *surface );
	virtual ~MngViewer( void );

	virtual bool load( const std::string &uri, const std::string &fitMode )=0;
	virtual void stop()=0;
	virtual void draw()=0;

	//	Widget
	virtual void resize( const Size &size );
	virtual void move( const Point &point );

	//	Events
	typedef boost::function<void (void)> Callback;
	void setCallbackLoadFinished( const Callback &callback );

	//	Instance creation
	static MngViewer *create( System *sys, Surface *s );

	typedef boost::function<void()> StopPlayerCallback;
	virtual void dispatchKey( util::key::type key, bool isUp ) = 0;
	void setFrameParameters( int first, int last, StopPlayerCallback callback );

protected:
	virtual void onResized();
	Surface *surface();
	void runCallbackLoadFinished();

protected:
	int _first;
	int _last;
	StopPlayerCallback _stopPlayer;

private:
	Surface *_surface;
	Callback _callbackFinished;
	MngViewer() {}
};

}

