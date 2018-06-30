#include "nclviewer.h"
#include "surface.h"
#include <util/log.h>
#include <util/mcr.h>
#include "generated/config.h"
#include <assert.h>

namespace canvas {

NclViewer *NclViewer::create( System *sys, Surface * /* surface */ ) {
	UNUSED(sys);
	return (NclViewer*)NULL;
}

NclViewer::NclViewer( canvas::Surface *surface )
	: _surface( surface ), _dev(NULL)
{
	_surface->setCompositionMode( composition::source );
}

NclViewer::~NclViewer( void )
{
}

//	Widget
void NclViewer::resize( const Size & /*size*/ ) {
	onResized();
}

void NclViewer::move( const Point & /*point*/ ) {
	onResized();
}

void NclViewer::onResized() {
	const Size &size = _surface->getSize();
	LDEBUG("NclViewer", "Surface resized: width=%d, height=%d", size.w, size.h);
}

Surface *NclViewer::surface() {
	assert(_surface);
	return _surface;
}

void NclViewer::setCallbackLoadFinished( const Callback &callback ){
	_callbackFinished = callback;
}

void NclViewer::runCallbackLoadFinished(){
	if (_callbackFinished){
		_callbackFinished();
	}
}

void NclViewer::setSurfaceCallback( const SurfaceCallback &callback ) {
	_surfaceCallback = callback;
}

canvas::Surface *NclViewer::getSurface() {
	if (!_surfaceCallback.empty()) {
		return _surfaceCallback();
	}
	return NULL;
}

}

