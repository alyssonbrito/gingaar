#include "mngviewer.h"
#include "surface.h"
#include <util/log.h>
#include <util/mcr.h>
#include "generated/config.h"
#if SYS_HTML_USE_CEF
#include "impl/cef/MngViewer.h"
#endif
#include <assert.h>

namespace canvas {

MngViewer *MngViewer::create( System * sys, Surface * /* surface */ ) {
#if SYS_HTML_USE_CEF
#else
	UNUSED(sys);
#endif
	return (MngViewer*)NULL;
}

MngViewer::MngViewer( canvas::Surface *surface )
	: _surface( surface )
{
	_surface->setCompositionMode( composition::source );
	_first = 0;
	_last = 0;
}

MngViewer::~MngViewer( void )
{
}

//	Widget
void MngViewer::resize( const Size & /*size*/ ) {
	onResized();	//	TODO: maty
}

void MngViewer::move( const Point & /*point*/ ) {
	onResized();	//	TODO: maty
}

void MngViewer::onResized() {
	const Size &size = _surface->getSize();
	LDEBUG("MngViewer", "Surface resized: width=%d, height=%d", size.w, size.h);
}

Surface *MngViewer::surface() {
	assert(_surface);
	return _surface;
}

void MngViewer::setCallbackLoadFinished( const Callback &callback ){
	_callbackFinished = callback;
}

void MngViewer::runCallbackLoadFinished(){
	if (_callbackFinished){
		_callbackFinished();
	}
}

void MngViewer::setFrameParameters( int first, int last, StopPlayerCallback callback ) {
	_first = first;
	_last = last;
	_stopPlayer = callback;
}

}

