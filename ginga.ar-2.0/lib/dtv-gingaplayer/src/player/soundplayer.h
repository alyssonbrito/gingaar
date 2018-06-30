#pragma once

#include "../player.h"

namespace canvas {
	class MediaPlayer;
}

namespace player {

class SoundProperties;

class SoundPlayer : public player::Player {
public:
	SoundPlayer( Device *dev );
	virtual ~SoundPlayer();

	SoundProperties *_sound;
	void setSoundLevel(int value);
	int getSoundLevel();

	void setBalanceLevel(int value);
	int getBalanceLevel();

	void setTrebleLevel(int value);
	int getTrebleLevel();

	void setBassLevel(int value);
	int getBassLevel();

protected:
	virtual bool startPlay();
	virtual bool startPlay(double time);
	virtual void stopPlay();
	virtual void pausePlay( bool pause );
	virtual bool supportSchemma( schema::type sch ) const;
	virtual void registerProperties();

private:
	canvas::MediaPlayer *_media;
	int _handleId;
};

}
