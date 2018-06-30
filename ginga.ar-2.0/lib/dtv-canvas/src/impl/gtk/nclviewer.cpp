#include "nclviewer.h"
#include "../../canvas.h"
#include "../../size.h"
#include "../../system.h"
#include <util/log.h>
#include <gtk/gtk.h>
#include <cairo/cairo.h>
#include <webkit/webkit.h>
#include <boost/bind.hpp>
#include <exception>
#include <stdexcept>
#include <assert.h>
#include <gdk/gdkkeysyms.h>
#include "../cairo/surface.h"
#include <util/cfg/configregistrator.h>
#include "../../window.h"

namespace canvas {
namespace gtk {

NclViewer::NclViewer( System *sys, Surface *surface )
	: canvas::NclViewer(surface), _sys(sys)
{
	_window = NULL;
}

NclViewer::~NclViewer( void )
{
	stopView();
}

void NclViewer::onResized() {
	const Size &s = this->surface()->getSize();
	gtk_widget_set_size_request ( _window, s.w, s.h );
}

void NclViewer::move( const Point & /* point */ ) {
}

void NclViewer::onFinished() {
}

void NclViewer::stopView() {
	if (_window) {
		LINFO("NclViewer", "Stop page");
		gtk_widget_destroy( _window );
		_window = NULL;
	}
}

#if GTK_MAJOR_VERSION < 3
void NclViewer::onExpose( GdkEventExpose *event )
{
	LDEBUG("gtk::NclViewer", "onExpose" );
	if (!this->getSurface()) {
		LWARN("gtk::NclViewer", "Surface error" );
		return;
	}

	// copy from Surface::markDirtySurface
	const Point &point = this->surface()->getLocation();
	const Size &size = this->surface()->getSize();

	Rect copy;
	copy.x = point.x + event->area.x;
	copy.y = point.y + event->area.y;
	copy.w = std::min( std::max(size.w - event->area.x, 0), event->area.width );
	copy.h = std::min( std::max(size.h - event->area.y, 0), event->area.height );

	canvas::Surface* srcSurface = this->getSurface();
	this->surface()->setCompositionMode( composition::source );
	this->surface()->blit(canvas::Point(0,0), srcSurface);
	this->surface()->markDirty(copy);
	this->surface()->flush();

}

static
gboolean damageEventCb( GtkWidget *widget, GdkEventExpose *event, gpointer user_data )
{
	NclViewer *ncl = (NclViewer *)user_data;

	ncl->onExpose(event);
	return false;
}

#else

void NclViewer::onDraw( cairo_t *cr )
{
	LDEBUG("gtk::NclViewer", "onDraw" );
	if (!this->getSurface()) {
		LWARN("gtk::NclViewer", "Surface error" );
		return;
	}

	GdkRectangle clipRect;
	if (!gdk_cairo_get_clip_rectangle(cr, &clipRect)) {
		return;
	}

	// copy from Surface::markDirtySurface
	const Point &point = this->surface()->getLocation();
	const Size &size = this->surface()->getSize();

	Rect copy;
	copy.x = point.x + clipRect.x;
	copy.y = point.y + clipRect.y;
	copy.w = std::min( std::max(size.w - clipRect.x, 0), clipRect.width );
	copy.h = std::min( std::max(size.h - clipRect.y, 0), clipRect.height );

	canvas::Surface* srcSurface = this->getSurface();
	this->surface()->setCompositionMode( composition::source );
	this->surface()->blit(canvas::Point(0,0), srcSurface);
	this->surface()->markDirty(copy);
	this->surface()->flush();

}

static
gboolean drawEventCb( GtkWidget* /* widget */, cairo_t* cr, gpointer user_data )
{
	NclViewer *ncl = (NclViewer *)user_data;

	ncl->onDraw(cr);
	return false;
}

#endif

void *NclViewer::prepare() {
	const Size &s = surface()->getSize();
	canvas::Point point = this->surface()->getLocation();
	canvas::Rect position(point.x, point.y, s.w, s.h);
	_window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	int window_width = util::cfg::getValue<int>( "gui.window.size.width" );
	int window_height = util::cfg::getValue<int>( "gui.window.size.height" );
	gtk_window_move( GTK_WINDOW(_window), window_width + 1, window_height + 1 );

	gtk_window_resize( GTK_WINDOW( _window ), s.w, s.h );
	gtk_widget_show( _window );
	gtk_widget_hide( _window );

#if GTK_MAJOR_VERSION < 3
	g_signal_connect_after( _window, "expose-event", G_CALLBACK(damageEventCb), this );
#else
	g_signal_connect_after( _window, "draw", G_CALLBACK(drawEventCb), this );
#endif

	return (void*)_window;
}

void NclViewer::stop() {
	stopView();
}

void NclViewer::draw() {
	LDEBUG("gtk::NclViewer", "Draw" );
	if (!this->getSurface()) {
		return;
	}

	canvas::Surface* srcSurface = this->getSurface();
	this->surface()->setCompositionMode( composition::source );
	this->surface()->blit(canvas::Point(0,0), srcSurface);
	this->surface()->markDirty();
	this->surface()->flush();
}

void NclViewer::dispatchKey(util::key::type /* key */, bool /* isUp */)
{
}

}
}
