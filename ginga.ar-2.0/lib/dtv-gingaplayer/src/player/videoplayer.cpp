#include "videoplayer.h"
#include "soundproperties.h"
#include "../device.h"
#include <canvas/mediaplayer.h>
#include <canvas/player.h>
#include <canvas/window.h>
#include <canvas/surface.h>
#include <canvas/system.h>
#include <util/mcr.h>

namespace player {

VideoPlayer::VideoPlayer( Device *dev )
	: GraphicPlayer( dev )
{
	LDEBUG("VideoPlayer", "Initializing Video Player");
	_media = dev->createMediaPlayer();
	//TODO: Audio Properties
	//_sound = new SoundProperties( _media );

	_media->onStop( boost::bind(&VideoPlayer::stop,this) );
}

VideoPlayer::~VideoPlayer()
{
	LDEBUG("VideoPlayer", "Finalizing Video Player");
	//delete _sound;
	delete _media;
}

bool VideoPlayer::startPlay() {
	LDEBUG("VideoPlayer", "Starting Video Player");

	if (GraphicPlayer::startPlay()) {

		//	Set surface transparent
		surface()->flushCompositionMode( canvas::composition::source );

		canvas::Rect bounds = surface()->getBounds();
		LDEBUG("VideoPlayer", "Initializing the surface to Position (%d, %d) "
				              "and Screen Size [%d x %d] and URL [%s]",
				              bounds.x, bounds.y,
				              bounds.w, bounds.h,
				              url().c_str());

		//	Initialize media player
		if (!_media->initialize( bounds, url() )) {
			LDEBUG("VideoPlayer", "Media Player cannot be initialized");
			return false;
		}

		// Send event to Common Core to load media
		device()->loadMedia( url(), &_handleId, 0);
		LDEBUG("VideoPlayer", "Handler ID was set to [%d(0x%x)]",
				              _handleId, _handleId);

		if ( url().compare( "sbtvd-ts://video" ) == 0 ) {
			LDEBUG("VideoPlayer", "SBTVD-TS should have a transparent background.");
			GraphicPlayer::forceTransparentBgColor();
		}

		//	Play
		_media->play();

		//Temporary solution: We only play the media if the media is not the main video, 
		//as it is already playing, if you send event to play again
		// the main video will stop and play again during resize 
		if ( url().compare(0,11, "sbtvd-ts://" ) != 0 ) {
			// Send event to Common Core to play media
			device()->playMedia(_handleId);
		}

		LDEBUG("VideoPlayer", "Start to Video Player succeeded");
		return true;
	}

	LDEBUG("VideoPlayer", "Start to Video Player failed");
	return false;
}

void VideoPlayer::stopPlay() {
	LDEBUG("VideoPlayer", "Stopping Video Player. Handler ID: [%d(0x%x)]",
			              _handleId, _handleId);

	device()->stopMedia(_handleId);

	// * LGESP *
	// Somehow, after closing media, the video player cannot reload the handler
	// again. Maybe, the handler value is lost or the Common Core could not
	// reopen handler after it's closed. In this case, keep it opened.
	//device()->closeMedia(_handleId);

	_media->stop();
	_media->finalize();
	GraphicPlayer::stopPlay();
}

void VideoPlayer::registerProperties() {
	LDEBUG("VideoPlayer", "Registering Properties to Video Player");
	GraphicPlayer::registerProperties();
	//_sound->registerProperties(this);
}

void VideoPlayer::pausePlay( bool pause ) {
	LDEBUG("VideoPlayer", "Pausing Video Player: [%s]. Handler ID: [%d(0x%x)]",
			              (pause ? "paused" : "resumed"),
			              _handleId, _handleId);

	_media->pause( pause );
}

//	Events
/**
 * Resize
 */
void VideoPlayer::onSizeChanged( const canvas::Size &size ) {
	LDEBUG("VideoPlayer", "Size Changed to [%d x %d]. Handler ID: [%d(0x%x)]", size.w, size.h, _handleId, _handleId);
	_media->resize( size );
}

/**
 * Move video widget
 */
void VideoPlayer::onPositionChanged( const canvas::Point &point ) {
	LDEBUG("VideoPlayer", "Position Changed to [%d x %d]. Handler ID: [%d(0x%x)]",
			              point.x, point.y,
			              _handleId, _handleId);

	_media->move( point );
}

/**
 * Resize video widget
 */
void VideoPlayer::onResizeChanged( const canvas::Rect &newBounds ) {
	LDEBUG("VideoPlayer", "Resize Changed to Position: (%d, %d) and Size: "
			              "[%d x %d]. Handler ID: [%d(0x%x)]",
			              newBounds.x, newBounds.y,
			              newBounds.w, newBounds.h,
			              _handleId, _handleId);

	device()->resizeMedia(newBounds.x, newBounds.y,
			              newBounds.w, newBounds.h,
			              _handleId);
}

bool VideoPlayer::supportSchemma( schema::type sch ) const {
	LDEBUG("VideoPlayer", "Supported Schema: [%s]. Handler ID: [%d(0x%x)]",
			              player::schema::getName ( sch ),
			              _handleId, _handleId );
/**
 * If the platform does not support dual video decode, only returns schema::sbtvd
 */
#ifdef NO_DUAL_VIDEO_DECODER
       return sch == schema::sbtvd;
#else
       return sch == schema::file || sch == schema::rtp || sch == schema::rtsp || sch == schema::sbtvd || sch == schema::http;
#endif
}

}
