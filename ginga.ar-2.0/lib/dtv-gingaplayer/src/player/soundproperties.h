#pragma once

#include "../player.h"
#include "soundplayer.h"

namespace canvas {
	class MediaPlayer;
}

namespace player {

class SoundProperties {
public:
	explicit SoundProperties( canvas::MediaPlayer *media );
	virtual ~SoundProperties();

	virtual void registerProperties( Player *player );

	void updateValues();
	void setSoundPropertiesLevel(double level);

protected:
	//	Properties
	void applySoundLevel();
	void applyBalanceLevel();
	void applyTrebleLevel();
	void applyBassLevel();

private:
	float _soundLevel;
	float _balanceLevel;
	float _trebleLevel;
	float _bassLevel;

	canvas::MediaPlayer *_media;
	SoundPlayer *_player;
};

}
