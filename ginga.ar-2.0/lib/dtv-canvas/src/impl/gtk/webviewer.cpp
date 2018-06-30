#include "webviewer.h"
#include "../../canvas.h"
#include "../../size.h"
#include "../../system.h"
#include <util/log.h>
#include <gtk/gtk.h>
#include <cairo/cairo.h>
#include <webkit/webkit.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <exception>
#include <stdexcept>
#include <assert.h>
#include <gdk/gdkkeysyms.h>
#include "../cairo/surface.h"
#include <util/cfg/configregistrator.h>
#include <boost/algorithm/string/replace.hpp>
#include "../../../../../include/ginga_api.h"


namespace canvas {
namespace gtk {

static void loadStatusCallback(WebKitWebView *webView, GParamSpec * /*spec*/, gpointer user_data)
{
	WebKitLoadStatus status = webkit_web_view_get_load_status(webView);
	WebViewer *web = (WebViewer *)user_data;

	if (status != WEBKIT_LOAD_FINISHED){
        	return;
    	}

	if (web) {
		web->applyStyle();
		web->onFinished();
	}
	web->readytodelete = true;

}

WebViewer::WebViewer( System *sys, Surface *surface )
	: canvas::WebViewer(surface), _sys(sys)
{
	startToStop=false;
	readytodelete=false;
	_view = NULL;
	_window = NULL;

}

static void loadResourceCallback(WebKitWebView* webView, WebKitWebFrame* webFrame, WebKitWebResource* resource, WebKitNetworkRequest* request, WebKitNetworkResponse* response)
{
       const gchar * uri = webkit_web_resource_get_uri(resource);
       if (uri!=NULL){
               const std::string url   = (std::string) uri;
               onFetchFilesFromCarousel(url);
       }
}


WebViewer::~WebViewer( void )
{
	stopPage();

}

void WebViewer::onResized() {
	const Size &s = this->surface()->getSize();
	gtk_widget_set_size_request ( _view, s.w, s.h );
}

void WebViewer::onFinished() {
    runCallbackLoadFinished();
}


#if GTK_MAJOR_VERSION < 3
void WebViewer::onExpose( GdkEventExpose *event )
{
	// copy from Surface::markDirtySurface
	const Point &point = this->surface()->getLocation();
	const Size &size = this->surface()->getSize();

	Rect copy;
	copy.x = point.x + event->area.x;
	copy.y = point.y + event->area.y;
	copy.w = std::min( std::max(size.w - event->area.x, 0), event->area.width );
	copy.h = std::min( std::max(size.h - event->area.y, 0), event->area.height );

	this->surface()->markDirty( copy );
	this->surface()->flush();
}

static
gboolean damageEventCb( GtkWidget *widget, GdkEventExpose  *event, gpointer user_data )
{
	WebViewer *web = (WebViewer *)user_data;

	web->onExpose(event);
	return false;
}

#else

void WebViewer::onDraw( cairo_t* cr )
{
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

	this->surface()->markDirty( copy );
	this->surface()->flush();
}

static
gboolean drawEventCb( GtkWidget* /* widget */, cairo_t* cr, gpointer user_data )
{
	WebViewer *web = (WebViewer *)user_data;

	web->onDraw(cr);
	return false;
}

#endif


void WebViewer::stopPage() {
	if (_view) {
		LINFO("WebViewer", "Stop page");

		if (startToStop==false){
			startToStop=true;	
			g_signal_handlers_disconnect_by_func(_view, reinterpret_cast<void*>(loadStatusCallback) , this);
        		g_signal_handlers_disconnect_by_func(_view,  reinterpret_cast<void*>(loadResourceCallback), this);
#if GTK_MAJOR_VERSION < 3
        		g_signal_handlers_disconnect_by_func(_view, reinterpret_cast<void*>(damageEventCb) , this);
#else
        		g_signal_handlers_disconnect_by_func(_view,  reinterpret_cast<void*>(drawEventCb) , this);
#endif
			webkit_web_view_stop_loading( WEBKIT_WEB_VIEW(_view) );
			

		}
		 /*this is workaround because of webkit crash when the engine destroys widget 
		just after starts the webview	*/
		if (readytodelete==true){ 
			readytodelete=false;		
    			(dynamic_cast<cairo::Surface *>(this->surface()))->setWebkitWindow( NULL );
 	   		//	Destroy widgets
			gtk_widget_destroy(_view);
			_view = NULL;
			gtk_widget_destroy(_window);
			_window = NULL;
		}			
	}
}


bool WebViewer::load( const std::string &file ) {
	const Size &s = surface()->getSize();

	LINFO("WebViewer", "Load page: url=%s, width=%d, height=%d",
		file.c_str(), s.w, s.h);
	
	stopPage();
	
	//	Create container
	_window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	if (!_window) {
		LERROR("WebViewer", "Cannot create gtk window!");
		return false;
	}

	int window_width = util::cfg::getValue<int>( "gui.window.size.width" );
	int window_height = util::cfg::getValue<int>( "gui.window.size.height" );

	gtk_window_move( GTK_WINDOW(_window), window_width + 1, window_height + 1 );

	// Remove resize grip
	gtk_window_set_resizable ( GTK_WINDOW(_window), FALSE );

	//	Create webkit
	_view = webkit_web_view_new ();
	if (!_view) {
		LERROR("WebViewer", "Cannot create gtk webkit webview!");
		return false;
	}

#if GTK_MAJOR_VERSION < 3
	g_signal_connect_after( _view, "expose-event", G_CALLBACK(damageEventCb), this );
#else
	g_signal_connect_after( _view, "draw", G_CALLBACK(drawEventCb), this );
#endif

	//	Setup webkit
	webkit_web_view_set_full_content_zoom( WEBKIT_WEB_VIEW (_view), TRUE );
	webkit_web_view_set_transparent( WEBKIT_WEB_VIEW (_view), TRUE );

	//	Load url
	webkit_web_view_load_uri (WEBKIT_WEB_VIEW (_view), file.c_str() );
	
	//	Add webkit to container
	gtk_widget_set_size_request ( _view, s.w, s.h );
	gtk_container_add (GTK_CONTAINER (_window), _view);
	gtk_widget_show_all (_window);

	//	Setup callbacks
	g_signal_connect(_view, "load-finished", G_CALLBACK(loadStatusCallback), this );
	g_signal_connect(_view, "document-load-finished", G_CALLBACK(loadStatusCallback), this );
	g_signal_connect(_view, "notify::load-status", G_CALLBACK(loadStatusCallback), this );
	g_signal_connect(_view, "destroy", G_CALLBACK(loadStatusCallback), this);
	g_signal_connect(_view, "close-web-view", G_CALLBACK(loadStatusCallback), this);

	// Webkit will call this callback just before load a resource, so we do fetch from Carousel
	g_signal_connect(_view,"resource-request-starting", G_CALLBACK(loadResourceCallback), this);

	(dynamic_cast<cairo::Surface *>(this->surface()))->setWebkitWindow( gtk_widget_get_window( _view ) );

	return true;
}

void WebViewer::stop() {
	stopPage();
}

bool WebViewer::applyStyle() {
	
	WebKitWebSettings* settings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW (_view));
        g_object_set(G_OBJECT(settings),"default-font-family", "TiresiasScreenfont", 
					"serif-font-family", "TiresiasScreenfont",
					"default-font-size", 12,
					"sans-serif-font-family", "TiresiasScreenfont", NULL);

        webkit_web_view_set_settings(WEBKIT_WEB_VIEW (_view), settings);

	std::string style = getStyle();
	if (!style.empty()) {
		std::string script = "var fileref=document.getElementById('NCLStyleSheetName'); \
			if (!fileref) { \
				fileref=document.createElement('link'); \
				fileref.setAttribute('rel', 'stylesheet'); \
				fileref.setAttribute('type', 'text/css'); \
				fileref.setAttribute('id', 'NCLStyleSheetName'); \
				fileref.setAttribute('href', 'filename'); \
				document.getElementsByTagName('head')[0].appendChild(fileref); \
			} else { \
				fileref.setAttribute('href', 'filename'); \
				document.getElementsByTagName('head')[0].replaceChild(fileref, fileref); \
			} \
			";
		boost::replace_all( script, "filename", style);
		webkit_web_view_execute_script(WEBKIT_WEB_VIEW (_view), script.c_str());
		return true;
	}
	return false;
}

void WebViewer::draw() {
	if(_view){
            this->surface()->markDirty();
            this->surface()->flush();
	}
}

extern GdkEventKey *g_curKeyEvent;
guint getGdkKeyCode( util::key::type symbol ) ;

void WebViewer::dispatchKey( util::key::type key, bool isUp )
{
	GdkEventKey k;
	GdkEventKey *eventk = NULL;
	if (!key || !_view){
		return;
	}

	if (g_curKeyEvent == NULL) {
		k.keyval = getGdkKeyCode( key );
		k.type = GDK_KEY_RELEASE;
		k.window = gtk_widget_get_window(_window);
		k.send_event = TRUE;
		k.state = 0;
		k.length = 0;
		k.string = NULL;
		k.group = 0;
		k.is_modifier = 0;
		k.time = GDK_CURRENT_TIME;
		k.hardware_keycode = key;
		eventk = &k;
	} else {
		eventk = g_curKeyEvent;
	}

	gboolean return_val = FALSE;
	if (isUp){
		g_signal_emit_by_name( _view, "key-release-event", eventk, &return_val );
	} else {
		g_signal_emit_by_name( _view, "key-press-event", eventk, &return_val );
	}
}

}
}
