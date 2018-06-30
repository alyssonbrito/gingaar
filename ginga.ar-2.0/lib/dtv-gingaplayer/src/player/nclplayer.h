#pragma once

#include "graphicplayer.h"
#include <util/keydefs.h>
#include "../system.h"
#include "../device.h"

namespace player {

class NclPlayer : public GraphicPlayer {
public:
	NclPlayer( Device *dev );
	virtual ~NclPlayer();

	virtual bool isApplication() const;

protected:
	//	Types
	typedef std::pair<util::key::type, bool> SelectionEventData;

	virtual bool startPlay();
	virtual void stopPlay();
	virtual void refresh();
	virtual bool supportRemote() const;

	//	Events
	virtual void onSizeChanged( const canvas::Size &size );
	virtual void dispatchKey( util::key::type key, bool isUp );

private:
	void *_pemem;
	System *_sys;
	Device *_dev;
};

}
