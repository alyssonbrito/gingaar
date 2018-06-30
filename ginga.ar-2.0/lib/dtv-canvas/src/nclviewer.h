#pragma once

#include <string>
#include <boost/function.hpp>
#include "util/keydefs.h"

namespace canvas {

class Point;
class Size;
class System;
class Surface;

class NclViewer {
public:
	explicit NclViewer( Surface *surface );
	virtual ~NclViewer( void );

	virtual void *prepare()=0;
	virtual void stop()=0;
	virtual void draw()=0;

	//	Widget
	virtual void resize( const Size &size );
	virtual void move( const Point &point );

	//	Events
	typedef boost::function<void (void)> Callback;
	void setCallbackLoadFinished( const Callback &callback );

	//	Instance creation
	static NclViewer *create( System *sys, Surface *s );

	virtual void dispatchKey( util::key::type key, bool isUp ) = 0;

	Surface *surface();
	void setDevice(void *dev) { _dev = dev; }
	void *getDevice() const { return _dev; }

	typedef boost::function<canvas::Surface* (void)> SurfaceCallback;
	void setSurfaceCallback( const SurfaceCallback &callback );
	canvas::Surface *getSurface();


protected:
	virtual void onResized();
	void runCallbackLoadFinished();

private:
	Surface *_surface;
	Callback _callbackFinished;
	SurfaceCallback _surfaceCallback;
	void *_dev;
	NclViewer() {}
};

}

