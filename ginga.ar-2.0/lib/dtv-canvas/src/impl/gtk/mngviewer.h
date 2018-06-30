#pragma once

#include <gtk/gtk.h>
#include "../../mngviewer.h"
#include "../../canvas.h"
#include <libmng.h>

typedef struct _GtkWidget GtkWidget;

namespace canvas {

class System;

namespace gtk {

class MngViewer;

struct _MngData
{
	canvas::Surface *surface;
	MngViewer *viewer;
	/* private */
	GTimer * timer;
	guint timeout_ID;
	guint width;
	guint height;
	mng_handle MNG_handle;
	guchar *MNG_drawing_buffer;
	guchar *MNG_alpha_buffer;
	guchar *mng_food;
	guint bytes_to_eat;
	guint bytes_eaten;
	guint first;
	guint last;
	canvas::MngViewer::StopPlayerCallback stop_player;
	System *sys;
	int *frame_number;
};

typedef struct _MngData MngData;

class MngViewer : public canvas::MngViewer {
public:
	MngViewer( canvas::System *sys, Surface *surface );
	virtual ~MngViewer( void );

	virtual bool load( const std::string &file, const std::string &fitMode );
	virtual void stop();
	virtual void draw();

	//	Aux
	void onFinished();

	void onExpose( GdkEventExpose *event );
	virtual void dispatchKey( util::key::type key, bool isUp );

	void createSurface ( int width, int height );
	canvas::Surface *getSurface() const { return _surface; }

	std::string &getFitMode() { return _fitMode; }
	bool getScalingSizes( canvas::Rect &t /*target*/, canvas::Rect &s /*source*/ );

protected:
	virtual void onResized();
	gboolean loadMngFromMemory ( MngData *mng_view, guchar *data_to_eat, guint data_size );
	gboolean initLibMng( MngData *mng_view );

	//	Aux
	void stopAnim();

private:
	System *_sys;
	MngData *_mngView;
	canvas::Surface *_surface;
	std::string _fitMode;
	int _frameNumber;
};

}
}

