#include "window.h"
#include "system.h"
#include "videooverlay.h"
#include "../cairo/canvas.h"
#include "../cairo/surface.h"
#include <util/log.h>
#include <util/mcr.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <gtk/gtk.h>
#include <util/cfg/configregistrator.h>

namespace canvas {
namespace gtk {

util::key::type getKeyCode( guint symbol );

#if GTK_MAJOR_VERSION >= 3
static gboolean on_window_draw( GtkWidget * /*da*/, cairo_t *cr, gpointer user_data ) {
	GdkRectangle clipRect;
	if (!gdk_cairo_get_clip_rectangle(cr, &clipRect)) {
		return FALSE;
	}
	Rect rect( clipRect.x, clipRect.y, clipRect.width, clipRect.height );
	Window *vo = (Window *)user_data;
	assert(vo);
	vo->draw( cr, rect );
	return TRUE;
}
#else
static gboolean on_window_expose_event( GtkWidget *da, GdkEventExpose *event, gpointer user_data ) {
	Rect rect( event->area.x, event->area.y, event->area.width, event->area.height );
	Window *vo = (Window *)user_data;
	assert(vo);
	GdkWindow *win=gtk_widget_get_window(da);
	cairo_t *cr = gdk_cairo_create(win);
	vo->draw( cr, rect );
	cairo_destroy(cr);
	return TRUE;
}
#endif

GdkEventKey *g_curKeyEvent = NULL;

static gboolean key_press_cb( GtkWidget * /*widget*/, GdkEventKey *kevent, gpointer user_data ) {

	bool ret = false;
	g_curKeyEvent = kevent;

	Window *vo = (Window *)user_data;
	if (vo->getSys()) {
		ret = static_cast<canvas::gtk::System*>(vo->getSys())->onKeyEvent( getKeyCode(kevent->keyval), kevent->type == GDK_KEY_RELEASE );
	}
	g_curKeyEvent = NULL;

	return ret;
}

static void on_window_destroyed( GtkWidget * /*widget*/, gpointer user_data ) {
	Window *vo = (Window *)user_data;
	if (vo->getSys()) {
		static_cast<canvas::gtk::System*>(vo->getSys())->onWindowDestroyed();
	}
}

static gboolean on_event( GtkWindow * /*window*/, GdkEvent *event, gpointer user_data ) {
	Window *vo = (Window *)user_data;
	assert(vo);
	vo->updateWindow( event );
	return FALSE;
}

Window::Window()
{
	_surface = NULL;
	_window = NULL;
}

Window::~Window()
{
}

//	Initialization
bool Window::initialize( Surface *surface ) {
	LDEBUG("gtk::Window", "Initialize");
	_surface = surface;

	//	Create main window
	_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	assert(_window);
	g_object_ref( _window );

	//	LG Electronics: #4357
	//	When GTK shows window first time it runs some initialization routine
	//	and window blinks even if window is transparent.
	//	So we change window size and show it for short time
	//	to force processing of GTK initialization here.
	bool dnb = util::cfg::getValue<bool>( "quirks.windowQuirkDoNotBlink" );
	//if (dnb && !surface) {
	if (dnb ) {
		APRINT_GRAPHICS("blink resize");
		gtk_window_resize( GTK_WINDOW( _window ), 1, 1 );
		gtk_widget_show( _window );
		gtk_widget_hide( _window );
	}

	//	Setup main window
	gtk_widget_set_events( GTK_WIDGET(_window),  GDK_STRUCTURE_MASK | GDK_FOCUS_CHANGE );	
	//gtk_widget_set_events( GTK_WIDGET(_window), GDK_ALL_EVENTS_MASK );
	if (!surface) {
		gtk_window_set_title( GTK_WINDOW(_window), title().c_str() );
		gtk_window_set_position(GTK_WINDOW(_window), GTK_WIN_POS_CENTER);
	}
	gtk_widget_set_app_paintable(_window, TRUE);
#ifdef GDK_WINDOWING_X11
	gtk_window_set_resizable(GTK_WINDOW(_window), FALSE);
	gtk_widget_set_double_buffered(_window, FALSE);
#endif

//	gtk_window_set_opacity(GTK_WINDOW(_window), 0.0f);

	if (surface) {
		Size s = surface->getSize();
		int window_width = util::cfg::getValue<int>( "gui.window.size.width" );
		int window_height = util::cfg::getValue<int>( "gui.window.size.height" );
		gtk_window_move( GTK_WINDOW(_window), window_width + 1,window_height + 1 );
		gtk_window_resize( GTK_WINDOW( _window ), s.w, s.h );
		setSize( s.w, s.h );
	} else {
 	//	Setup mode [FULLSCREEN | NORMAL]
		setMode();
	}

	{	//	Set window to work with RGBA
		GdkScreen *screen = gtk_widget_get_screen(_window);
#if GTK_MAJOR_VERSION < 3
		gtk_widget_set_colormap( _window, gdk_screen_get_rgba_colormap(screen) );
#else
		GdkVisual* visual = gdk_screen_get_rgba_visual(screen);
		if (!visual)
			visual = gdk_screen_get_system_visual(screen);

		gtk_widget_set_visual(_window, visual);
#endif
	}

	//	Setup window signals
#if GTK_MAJOR_VERSION >= 3
	g_signal_connect( G_OBJECT(_window), "draw", G_CALLBACK(on_window_draw), this);
#else
	g_signal_connect( G_OBJECT(_window), "expose_event", G_CALLBACK(on_window_expose_event), this);
#endif
	if (!surface) {
		g_signal_connect( G_OBJECT(_window), "key_press_event", G_CALLBACK(key_press_cb), this );
		g_signal_connect( G_OBJECT(_window), "key_release_event", G_CALLBACK(key_press_cb), this );
		g_signal_connect( G_OBJECT(_window), "destroy", G_CALLBACK (on_window_destroyed), this);
		g_signal_connect( G_OBJECT(_window), "configure-event", G_CALLBACK(on_event), this );
		g_signal_connect( G_OBJECT(_window), "unmap-event", G_CALLBACK(on_event), this );
		g_signal_connect( G_OBJECT(_window), "map-event", G_CALLBACK(on_event), this );
		g_signal_connect( G_OBJECT(_window), "focus-in-event", G_CALLBACK(on_event), this );
	}

	//	Show window
	gtk_widget_show_all(_window);

	if (surface) {
		((cairo::Surface *)surface)->setWebkitWindow(gtk_widget_get_window(_window));
	}

	return true;
}

void Window::setFullscreenMode() {
	assert(_window);
	gtk_window_fullscreen(GTK_WINDOW(_window));

	//	Set the new windows size (fullscreen size)
	GdkScreen *scr = gtk_window_get_screen(GTK_WINDOW(_window));
	int w = gdk_screen_get_width(scr);
	int h = gdk_screen_get_height(scr);
	setSize(w, h);
}

void Window::setNormalMode() {
	assert(_window);

	const Size &s = size();

	GdkGeometry hints;
	hints.min_width = s.w;
	hints.min_height = s.h;
	hints.width_inc = s.w;
	hints.height_inc = s.h;

	GdkWindowHints mask = (GdkWindowHints) (GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE );

	gtk_window_set_geometry_hints (GTK_WINDOW (_window),
			_window,
			&hints,
			mask);
}

void Window::finalize() {
	if (_surface) {
		static_cast<cairo::Surface*>(_surface)->setWebkitWindow( NULL );
		_surface = NULL;
	}

	//	Remove window
	gtk_widget_destroy( _window );
	g_object_unref( _window );
}

GtkWidget *Window::widget() const {
	return _window;
}

//	Overlays
canvas::VideoOverlay *Window::createOverlayInstance() const {
	return new VideoOverlay( (Window *)this );
}

//	Layer methods
Surface *Window::lockLayer( Canvas * /*canvas*/ ) {
	return NULL;
}

void Window::renderLayer( Surface * /*surface*/, const std::vector<Rect> &dirtyRegions ) {
	BOOST_FOREACH( const Rect &r, dirtyRegions ) {
		//	Mark exposed area
		gtk_widget_queue_draw_area(
			_window,
			r.x,
			r.y,
			r.w,
			r.h
		);
	}
	//  LG Electronics: #4311
	//  Dirty regions are not updated synchronously, we need to force expose event.
	//  Sultan is a content example
	gdk_window_process_all_updates();
}

void Window::draw( cairo_t* cr, const Rect &r ) {
	//LDEBUG("gtk::Window", "Draw: rect=(%d,%d,%d,%d)", r.x, r.y, r.w, r.h );

	//	set a clip region for the expose event
	cairo_rectangle( cr, r.x, r.y, r.w, r.h );
	cairo_clip_preserve( cr );

	//	Clear region
	if (_surface){
		//use the surface background.
		canvas::Color bc;
		bc = _surface->getBackgroundColor();
		APRINT_GRAPHICS("backgounColor[%d-%d:%d:%d]",bc.alpha, bc.r,bc.g,bc.b);
		cairo_set_source_rgba (cr, bc.r, bc.g, bc.b, bc.alpha);
		cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint (cr);


	}else{
		APRINT_GRAPHICS("normal cleaning");
		cairo_set_operator( cr, CAIRO_OPERATOR_CLEAR );
		cairo_set_source_rgba (cr, 0, 0, 0, 0.0);
		cairo_fill(cr);
	}
	APRINT_GRAPHICS("ON DRAW! Cleaned" );


	std::vector<Surface *> toBlit;
	getSys()->canvas()->getSurfacesForBlit(toBlit, r);

	Rect blitRect;
	BOOST_FOREACH( Surface *srfc, toBlit ) {
		cairo::Surface *surface = static_cast<cairo::Surface*>(srfc);
		r.intersection(surface->getBounds(), blitRect);
		Rect r = surface->getBounds();
		APRINT_GRAPHICS("surfaces [%d:%d:%d:%d]",r.x,r.y,r.w,r.h);

		//	Set composition mode from surface
		composition::mode mode = surface->flushCompositionMode();
		cairo_operator_t op;
		switch (mode) {
			case composition::source: op = CAIRO_OPERATOR_SOURCE; break;
			case composition::clear: op = CAIRO_OPERATOR_CLEAR; break;
			case composition::source_over:
			default:  op = CAIRO_OPERATOR_OVER; break;
		}
		cairo_set_operator( cr, op );

		//	Blit dirty region
		Point surfacePos = surface->getLocation();
		if (surface->getWebkitWindow()) {
			gdk_cairo_set_source_window( cr, surface->getWebkitWindow(), surfacePos.x, surfacePos.y );
		} else {
			cairo_set_source_surface ( cr, surface->getContent(), surfacePos.x, surfacePos.y );
		}
		cairo_rectangle( cr, blitRect.x, blitRect.y, blitRect.w, blitRect.h );
		cairo_clip(cr);
		cairo_t* src_maskcr = surface->GetMask();
		if (src_maskcr) {
			cairo_mask_surface(cr, surface->GetMaskSurf(), surfacePos.x, surfacePos.y);
			cairo_surface_t* _borderSurface = surface->GetBorderSurf();
			if (_borderSurface){
				cairo_set_source_surface ( cr, _borderSurface, surfacePos.x, surfacePos.y );
				std::string borderColor = surface->getBorderColor();
                        	if (borderColor.compare("blend")==0)
					cairo_paint_with_alpha( cr, 0.8 );
				else
					cairo_paint_with_alpha(cr, 1.0);
			}
		} else {
			cairo_paint_with_alpha(cr, surface->alpha());
		}
		cairo_reset_clip(cr);
	}
	cairo_reset_clip(cr);
	if (_surface) {
		_surface->markDirty();
		_surface->flush();
//		gtk_window_set_opacity(GTK_WINDOW(da), 1.0f);
	}
}

void Window::updateWindow( GdkEvent *event ) {
	LTRACE( "gtk::Window", "Update main window properties: type=%d\n", event->type );

	const std::vector<canvas::VideoOverlayPtr> &ovs = overlays();
	BOOST_FOREACH( canvas::VideoOverlay *tmp, ovs ) {
		VideoOverlay *vo = dynamic_cast<VideoOverlay *>(tmp);
		vo->onMainChanged( event );
	}
}

guint getGdkKeyCode(util::key::type symbol);
bool Window::sendKey( ::util::key::type key, bool isUp ) {
	if (!_window) {
		return false;
	}

	gboolean return_val = FALSE;
	GdkEventKey _gdkEvent;
	_gdkEvent.window = gtk_widget_get_window(_window);
	_gdkEvent.send_event = TRUE;
	_gdkEvent.state = 0;
	_gdkEvent.length = 0;
	_gdkEvent.string = NULL;
	_gdkEvent.group = 0;
	_gdkEvent.is_modifier = 0;
	_gdkEvent.time = GDK_CURRENT_TIME;
	_gdkEvent.keyval = getGdkKeyCode(key);
	_gdkEvent.hardware_keycode = key;

	if (isUp) {
		_gdkEvent.type = GDK_KEY_RELEASE;
		g_signal_emit_by_name( _window, "key-release-event", &_gdkEvent, &return_val );
	} else {
		_gdkEvent.type = GDK_KEY_PRESS;
		g_signal_emit_by_name( _window, "key-press-event", &_gdkEvent, &return_val );
	}
	return return_val;
}

Surface* Window::getSurface() {
	const Size &s = getSys()->canvas()->size();
	cairo::Surface* surface = static_cast<cairo::Surface*>(getSys()->canvas()->createSurface(Rect(0,0,s.w,s.h)));
	cairo_t* cr = surface->getContext();
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	gdk_cairo_set_source_window(cr, gtk_widget_get_window(_window), 0, 0);
	cairo_paint(cr);
	return surface;
}

void Window::resize() {
	if (_surface) {
		const Size &s = _surface->getSize();
		gtk_widget_set_size_request(_window, s.w, s.h);
	}
}

void Window::setSizeImpl( const Size &s ) {
	GdkGeometry hints;
	hints.min_width = s.w;
	hints.min_height = s.h;
	hints.max_width = s.w;
	hints.max_height = s.h;

	GdkWindowHints mask = (GdkWindowHints) (GDK_HINT_MAX_SIZE | GDK_HINT_MIN_SIZE);
	gtk_window_set_geometry_hints(GTK_WINDOW (_window), _window, &hints, mask);
}

void Window::setPointImpl( int x, int y ) {
	gtk_window_move(GTK_WINDOW (_window), x, y);
}

void Window::setVisibleImpl( bool visible ) {
	if(visible) {
		gtk_widget_show(_window);
	} else {
		gtk_widget_hide(_window);
	}
}

}
}
