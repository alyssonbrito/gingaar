#include "mirrorplayer.h"
#include "../device.h"
#include <canvas/surface.h>
#include <canvas/canvas.h>
#include <util/mcr.h>
#include <boost/bind.hpp>

namespace player {

MirrorPlayer::MirrorPlayer( Device *dev )
	: GraphicPlayer( dev )
{
}

MirrorPlayer::~MirrorPlayer()
{
}

bool MirrorPlayer::startPlay() {
	bool result=false;
	if (GraphicPlayer::startPlay()) {
		//	Add mirror callback
		mirror = boost::bind(&MirrorPlayer::makeMirror, this, _1);
		getSurface()->addMirror( body(), std::make_pair((void*)this, mirror) );
		canvas::Surface* surface = getSurface()->canvas()->getSurfaceById( body() );
		if (surface) {
			surface->flush();
		}
		result = true;
	}
	return result;
}

void MirrorPlayer::stopPlay() {
	if (mirror) {
		getSurface()->removeMirror( body(), (void*)this );
	}
	GraphicPlayer::stopPlay();
}

//	Events
void MirrorPlayer::onSizeChanged( const canvas::Size &size ) {
	UNUSED(size);
	refresh();
}

void MirrorPlayer::onPositionChanged( const canvas::Point &point ) {
	UNUSED(point);
}

bool MirrorPlayer::makeMirror ( void *surface ) {
	canvas::Surface* destSurface = getSurface();
	if (surface) {
		canvas::Surface* srcSurface = static_cast<canvas::Surface*>(surface);
		canvas::Rect t = canvas::Rect(0, 0, destSurface->getSize().w,  destSurface->getSize().h);
		canvas::Rect s = canvas::Rect(0, 0, srcSurface->getSize().w, srcSurface->getSize().h);

		if (!destSurface->scale( t, srcSurface, s )) {
			LWARN("MnvViewer", "cannot scale image");
		}
	} else {
		getSurface()->clear();
	}
	destSurface->markDirty();
	destSurface->flush();

	return true;
}

void MirrorPlayer::refresh() {
	canvas::Surface* surface = getSurface()->canvas()->getSurfaceById( body() );
	if (surface) {
		surface->flush();
	}
}

void MirrorPlayer::apply( bool needRefresh ) {
	if (needRefresh) {
		refresh();
	}

	onPropertyChanged();
}

bool MirrorPlayer::isApplication() const {
	return false;
}

}
