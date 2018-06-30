#pragma once

#include <string>
#include <boost/function.hpp>
#include "util/keydefs.h"

namespace canvas {

class Point;
class Size;
class System;
class Surface;

class WebViewer {
public:
	explicit WebViewer( Surface *surface );
	virtual ~WebViewer( void );

	virtual bool load( const std::string &uri )=0;
	virtual void stop()=0;
	virtual void draw()=0;

	//	Widget
	virtual void resize( const Size &size );

	//	Events
	typedef boost::function<void (void)> Callback;
	void setCallbackLoadFinished( const Callback &callback );

	//	Instance creation
	static WebViewer *create( System *sys, Surface *s );

	virtual void dispatchKey( util::key::type key, bool isUp ) = 0;

	void setStyle( const std::string &style );
	const std::string &getStyle() const;
	virtual bool applyStyle();

protected:
	virtual void onResized();
	Surface *surface();
	void runCallbackLoadFinished();

private:
	Surface *_surface;
	Callback _callbackFinished;
	std::string _style;
	WebViewer() {}
};

}

