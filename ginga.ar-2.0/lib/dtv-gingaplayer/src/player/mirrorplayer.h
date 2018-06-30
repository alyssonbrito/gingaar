#pragma once

#include "graphicplayer.h"
#include <util/keydefs.h>

namespace canvas {
	class MirrorViewer;
}

namespace player {

class MirrorPlayer : public GraphicPlayer {
public:
	MirrorPlayer( Device *dev );
	virtual ~MirrorPlayer();

	virtual bool isApplication() const;

protected:
	virtual bool startPlay();
	virtual void stopPlay();
	virtual void refresh();
	virtual void apply( bool needRefresh );

	//	Events
	virtual void onSizeChanged( const canvas::Size &size );
	virtual void onPositionChanged( const canvas::Point &point );
	bool makeMirror ( void *surface );


private:
	boost::function<bool (canvas::Surface*)> mirror;
};

}
