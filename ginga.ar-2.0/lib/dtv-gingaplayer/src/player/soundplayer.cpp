#include "soundplayer.h"
#include "soundproperties.h"
#include "../device.h"
#include <canvas/mediaplayer.h>
#include <util/mcr.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/numeric/conversion/cast.hpp>


namespace player {

SoundPlayer::SoundPlayer( Device *dev ) : Player( dev )
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);

	_media = dev->createMediaPlayer();
	_sound = new SoundProperties( _media );

	_media->onStop( boost::bind(&SoundPlayer::stop,this) );

	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	typeSet.insert( "SoundPlayer" );
}

SoundPlayer::~SoundPlayer()
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	delete _sound;
}

bool SoundPlayer::startPlay()
{
	startPlay(0);
}

bool SoundPlayer::startPlay(double time)
{
	bool ret = false;

	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d] URL = [%s]", __FUNCTION__, __LINE__, url().c_str());
	if (_media->initialize( url() )) {
		int mediaType = getMediaType();

		device()->loadMedia( url(), &_handleId, mediaType);
		if (time > 0)
		{
			int timeInt = boost::numeric_cast<int>(time) * 1000;
			
			device()->setMediaTime(_handleId, timeInt);
		}
		_media->play();
		device()->playMedia( _handleId );
		_sound->updateValues();

		LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
		ret = true;
	}
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d] Handler ID = %d[0x%x], Result = [%s]", __FUNCTION__, __LINE__, _handleId, _handleId, ret);
	return ret;
}

void SoundPlayer::stopPlay()
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	_media->stop();
	device()->stopMedia( _handleId );
	device()->closeMedia( _handleId );
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
}

void SoundPlayer::pausePlay( bool needPause )
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	_media->pause( needPause );

	if ( needPause ) {
		device()->freezeMedia( _handleId );
	} else {
		device()->resumeMedia( _handleId );
	}
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
}

void SoundPlayer::setSoundLevel( int value )
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	device()->setSoundLevel(value, _handleId);
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
}

int SoundPlayer::getSoundLevel()
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	int result = device()->getSoundLevel(_handleId);
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d] Sound Level = [%d]", __FUNCTION__, __LINE__, result);
	return result;
}

void SoundPlayer::setBalanceLevel( int value )
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	device()->setBalanceLevel(value, _handleId);
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
}

int SoundPlayer::getBalanceLevel()
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	int result = device()->getBalanceLevel(_handleId);
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d] Balance Level = [%d]", __FUNCTION__, __LINE__, result);
	return result;
}

void SoundPlayer::setTrebleLevel( int value )
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	device()->setTrebleLevel(value, _handleId);
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
}

int SoundPlayer::getTrebleLevel()
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	int result = device()->getTrebleLevel(_handleId);
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d] Treble Level = [%d]", __FUNCTION__, __LINE__, result);
	return result;
}

void SoundPlayer::setBassLevel( int value )
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	device()->setBassLevel(value, _handleId);
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
}

int SoundPlayer::getBassLevel()
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	int result = device()->getBalanceLevel(_handleId);
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d] Balance Level = [%d]", __FUNCTION__, __LINE__, result);
	return result;
}

bool SoundPlayer::supportSchemma( schema::type sch ) const
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	return sch == schema::file || sch == schema::rtp || sch == schema::rtsp;
}

void SoundPlayer::registerProperties()
{
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
	Player::registerProperties();
	_sound->registerProperties(this);
	LDEBUG("SoundPlayer", "[SoundPlayer::%s::%d]", __FUNCTION__, __LINE__);
}

}
