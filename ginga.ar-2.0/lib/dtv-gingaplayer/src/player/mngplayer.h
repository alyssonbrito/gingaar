#pragma once

#include "graphicplayer.h"
#include <util/keydefs.h>

namespace canvas {
	class MngViewer;
}

namespace player {

class MngPlayer : public GraphicPlayer {
public:
	MngPlayer( Device *dev );
	virtual ~MngPlayer();

	virtual bool isApplication() const;

protected:
	//	Types
	typedef std::pair<util::key::type, bool> SelectionEventData;

	virtual bool startPlay();
	virtual void stopPlay();
	virtual void refresh();
	virtual bool supportRemote() const;
	virtual void apply( bool needRefresh );
	virtual void registerProperties();

	std::string fitMode();

	//	Events
	virtual void onSizeChanged( const canvas::Size &size );
	virtual void onPositionChanged( const canvas::Point &point );
	virtual void dispatchKey( util::key::type key, bool isUp );

	typedef std::pair<std::string,int> AttributionEventData;
	void setAdditionalParameters( const AttributionEventData &data );

private:
	canvas::MngViewer *_mng;
	int _first;
	int _last;
};

}
