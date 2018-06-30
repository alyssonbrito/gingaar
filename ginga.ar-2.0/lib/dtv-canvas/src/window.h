#pragma once

#include "types.h"
#include "rect.h"
#include <vector>
#include <string>

namespace canvas {

class VideoOverlay;

class Window {
public:
	Window();
	virtual ~Window();
	
	//	Initialization
	virtual bool initialize( Surface *surface = NULL );
	virtual void finalize();

	//	Getters
	const std::string &title() const;
	const Size &size() const;

	//	Video overlays
	VideoOverlay *createVideoOverlay();
	void destoyVideoOverlay( VideoOverlayPtr &ptr );

	//	Layer methods
	virtual Surface *lockLayer( Canvas *canvas )=0;
	virtual void renderLayer( Surface *surface, const std::vector<Rect> &dirtyRegions )=0;
	virtual void unlockLayer( Surface *surface );

	//	Video methods
	virtual int getFormat( char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines );
	virtual void cleanup();
	virtual void *allocFrame( void **pixels );
	virtual void freeFrame( void *frame );
	virtual void renderFrame( void *frame );

	//	Instance creation
	static Window *create();

	virtual Surface *getSurface() { return NULL; }
	virtual void setSys(canvas::System* sys);
	canvas::System* getSys() const;
	virtual bool sendKey( ::util::key::type key, bool isUp );

	virtual void resize() {}

	virtual void changeVideo( int x1, int y1, int width, int height );
	virtual void setVisible( bool visible );

protected:
	//	Mode methods
	void setMode();
	virtual void setFullscreenMode();
	virtual void setNormalMode();
	virtual void setSize( int width, int height );

	//	Overlays
	const std::vector<VideoOverlayPtr> &overlays() const;
	virtual VideoOverlay *createOverlayInstance() const;
	virtual void setSizeImpl( const Size &size );
	virtual void setPointImpl( int x, int y );
	virtual void setVisibleImpl( bool visible );

    Surface *_surface;

private:
	std::string _title;
	Size _size;
	bool _isFullScreen;
	std::vector<VideoOverlayPtr> _overlays;
	Rect _videoBounds;
	canvas::System* _sys;
};
	
}
