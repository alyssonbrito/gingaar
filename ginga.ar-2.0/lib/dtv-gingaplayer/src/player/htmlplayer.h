#pragma once

#include "graphicplayer.h"
#include <util/keydefs.h>

namespace canvas {
	class WebViewer;
}

namespace player {

class HtmlPlayer : public GraphicPlayer {
public:
	HtmlPlayer( Device *dev );
	virtual ~HtmlPlayer();

	virtual bool isApplication() const;

protected:
	//	Types
	typedef std::pair<util::key::type, bool> SelectionEventData;

	virtual bool startPlay();
	virtual void stopPlay();
	virtual void refresh();
	virtual bool supportRemote() const;
	virtual void registerProperties();

	//	Events
	virtual void onSizeChanged( const canvas::Size &size );
	virtual void dispatchKey( util::key::type key, bool isUp );

	void applyStyle();

private:
	canvas::WebViewer *_html;
	std::string _style;
};

}
