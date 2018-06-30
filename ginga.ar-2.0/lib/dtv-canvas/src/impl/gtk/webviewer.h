#pragma once

#include <gtk/gtk.h>
#include "../../webviewer.h"

typedef struct _GtkWidget GtkWidget;

namespace canvas {

class System;

namespace gtk {

class WebViewer : public canvas::WebViewer {
public:
	WebViewer( canvas::System *sys, Surface *surface );
	virtual ~WebViewer( void );

	virtual bool load( const std::string &file );
	virtual void stop();
	virtual void draw();

	//	Aux
	void onFinished();
	void stopPage();
	bool startToStop;
	bool readytodelete;
	
#if GTK_MAJOR_VERSION < 3
	void onExpose( GdkEventExpose *event );
#else
	void onDraw( cairo_t* cr );
#endif
	virtual void dispatchKey( util::key::type key, bool isUp );
	virtual bool applyStyle();

protected:
	virtual void onResized();

	//	Aux
	

private:
	System *_sys;	
	GtkWidget *_window;
	GtkWidget *_view;
};

}
}

