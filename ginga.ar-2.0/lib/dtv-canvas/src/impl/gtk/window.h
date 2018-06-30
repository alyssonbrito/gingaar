#pragma once

#include <gtk/gtk.h>
#include "../../window.h"

typedef struct _GtkWidget GtkWidget;
typedef union  _GdkEvent GdkEvent;

namespace canvas {
namespace gtk {

class Window : public canvas::Window {
public:
	Window();
	virtual ~Window();

	//	Initialization
	virtual bool initialize( Surface *surface = NULL );
	virtual void finalize();

	//	Layer methods
	virtual Surface *lockLayer( Canvas *canvas );
	virtual void renderLayer( Surface *surface, const std::vector<Rect> &dirtyRegions );

	//	Aux	
	void draw( cairo_t* cr, const Rect &r );
	void updateWindow( GdkEvent *event );
	GtkWidget *widget() const;

	virtual void* getSurface() const { return _surface; }
	virtual bool sendKey( ::util::key::type key, bool isUp );
	virtual Surface *getSurface();

	virtual void resize();

protected:
	//	Overlays
	virtual canvas::VideoOverlay *createOverlayInstance() const;

	//	Mode methods
	void setFullscreenMode();
	void setNormalMode();
	virtual void setSizeImpl( const Size &size );
	virtual void setPointImpl( int x, int y );
	virtual void setVisibleImpl( bool visible );

private:
	GtkWidget *_window;
};
	
}
}
