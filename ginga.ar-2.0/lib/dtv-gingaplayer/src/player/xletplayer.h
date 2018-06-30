#pragma once

#include <util/keydefs.h>
#include <vector>
#include "graphicplayer.h"


namespace bptgn = br::pucrio::telemidia::ginga::ncl;

namespace player {

class XletPlayer: public GraphicPlayer {
public:
	XletPlayer( Device *dev );
	virtual ~XletPlayer();

	virtual bool isApplication() const;

	//	Callback initialization
	virtual void setEventCallback( const event::Callback &callback );

protected:
	//	Types
	typedef std::pair<std::string,std::string> AttributionEventData;
	typedef std::pair<util::key::type,bool> SelectionEventData;

	//	Virtual methods
	virtual bool init();
	virtual void fin();
	virtual bool startPlay();
	virtual void stopPlay();
	virtual void pausePlay( bool pause );
	virtual void abortPlay();
	virtual void registerProperties();

	//	Properties
	void applyVisible();

	//	Events
	void onPresentationEvent( event::evtAction::type action, const std::string &label="" );
	void sendAttributionEvent( const AttributionEventData &data );
	void onAttributionEvent( const std::string &name="", const std::string &value="" );

	void stopPlayer( event::evtAction::type evtAction );

	void AppStateCallback( unsigned char state );

private:
	bptgn::PresentationEngineManager* _pem;
	event::Callback _callback;
	bool _paused;
	std::string _x_mainclass;
	std::string _x_classpath;
	int _playerId;
	bool _visible;
};

}
