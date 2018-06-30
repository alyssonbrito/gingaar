#pragma once

#include <canvas/size.h>
#include <canvas/rect.h>

namespace canvas {
	class Surface;
}

namespace player {
namespace mcanvas {

class SurfaceWrapper {
public:
	SurfaceWrapper( canvas::Surface *surface, bool isPrimary );
	virtual ~SurfaceWrapper();

	canvas::Surface *surface();

	void setFlip( bool horizontal, bool vertical );
	bool isFlippedH() const;
	bool isFlippedV() const;

	void setScaledSize( const canvas::Size &size );
	canvas::Size getScaledSize() const;

	void replaceSurface( canvas::Surface *newSurface );

	bool needScale() const;

	void rotation( int degrees );
	int rotation() const;

	bool isPrimary() const;

	//	Crop
	void setCrop( canvas::Rect& rect ) {
		_cropRect = rect;
		_useCrop = true;
	}

	canvas::Rect getCrop() const {
		return _cropRect;
	}

	bool isCropSet() const {
		return _useCrop;
	}

	void clearCrop() {
		_useCrop = false;
	}

private:
	canvas::Surface *_surface;
	bool _flipH;
	bool _flipV;
	canvas::Size _scaledSize;
	int _rotation;
	bool _isPrimary;
	canvas::Rect _cropRect;
	bool _useCrop;

	SurfaceWrapper() {}
};

typedef SurfaceWrapper * SurfaceWrapperPtr;

}
}
