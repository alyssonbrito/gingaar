#pragma once

#include <gtk/gtk.h>
#include "../../nclviewer.h"

typedef struct _GtkWidget GtkWidget;

namespace canvas {

class System;

namespace gtk {

class NclViewer : public canvas::NclViewer {
public:
	NclViewer( canvas::System *sys, Surface *surface );
	virtual ~NclViewer( void );

	virtual void *prepare();
	virtual void stop();
	virtual void draw();

	virtual void move( const Point &point );

	//	Aux
	void onFinished();

#if GTK_MAJOR_VERSION < 3
	void onExpose( GdkEventExpose *event );
#else
	void onDraw( cairo_t *cr );
#endif

	virtual void dispatchKey( util::key::type key, bool isUp );

protected:
	virtual void onResized();


	//	Aux
	void stopView();

private:
	System *_sys;
	GtkWidget *_window;
};

}
}

