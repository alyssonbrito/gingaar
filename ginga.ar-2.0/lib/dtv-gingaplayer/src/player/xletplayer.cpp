#include <stdio.h>
#include "xletplayer.h"
#include "settings.h"
#include "../property/forwardproperty.h"
#include "../device.h"
#include "../system.h"
#include <canvas/system.h>
#include <canvas/surface.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/bind.hpp>
#include "../property/propertyimpl.h"
#include "../../../ncl30-presenter/src/presentationenginemanager.h"

namespace player {

XletPlayer::XletPlayer( Device *dev ) : GraphicPlayer( dev ), _paused(false), _playerId(-1), _visible(true)
{
	_x_mainclass = "";
	_x_classpath = "";
	_pem = ( bptgn::PresentationEngineManager * ) ( getDevice()->systemPlayer()->getPem() );
}

XletPlayer::~XletPlayer() {
}

bool XletPlayer::isApplication() const
{
	return true;
}

//	Callback initialization
void XletPlayer::setEventCallback( const event::Callback &callback )
{
	_callback = callback;
}

bool XletPlayer::init()
{
	//	Initialize graphic player
	if (!GraphicPlayer::init()) {
		return false;
	}
	return true;
}

void XletPlayer::fin()
{
	GraphicPlayer::fin();
}

bool XletPlayer::startPlay()
{
	if (GraphicPlayer::startPlay()) {
		// NÃO, ISSO NÃO É UM BOM APPROACH!!!!!
		std::string temp = _pem->path + "/" + _x_classpath + "/" + _x_mainclass + ".class";
		onFetchFilesFromCarousel (temp.c_str());

		canvas::Rect rec = getSurface()->getBounds();
		LWARN("XletPlayer", "Position: ( %d, %d ); Size: ( %d x %d )", rec.x, rec.y, rec.w, rec.h);
		_pem->nclStartXlet( _x_mainclass, _x_classpath, rec.x, rec.y, rec.w, rec.h );
		_pem->registerXletAppStateCallback(boost::bind(&XletPlayer::AppStateCallback, this, _1));
		return true;
	}
	return false;
}

void XletPlayer::stopPlay()
{
	_pem->registerXletAppStateCallback( NULL );
	stopPlayer( event::evtAction::stop );
}

void XletPlayer::pausePlay( bool pause )
{
	if (pause) {
		onPresentationEvent( event::evtAction::pause );
	} else {
		onPresentationEvent( event::evtAction::resume );
	}
}

void XletPlayer::abortPlay()
{
	stopPlayer( event::evtAction::abort );
}

void XletPlayer::registerProperties()
{
	//	Setup graphic player
	GraphicPlayer::registerProperties();

	{	//	Property attributionEvent
		ForwardProperty<AttributionEventData> *prop = new ForwardProperty<AttributionEventData>(
			boost::bind(&XletPlayer::sendAttributionEvent,this,_1), true );
		addProperty( property::type::attributionEvent, prop );
	}

	{	//	Property presentationEvent
		ForwardProperty<std::string> *prop = new ForwardProperty<std::string>(
			boost::bind(&XletPlayer::onPresentationEvent,this,event::evtAction::start,_1), true );
		addProperty( property::type::presentationEvent, prop );
	}

	{
		PropertyImpl<std::string> *prop = new PropertyImpl<std::string>( true, _x_mainclass );
		addProperty( property::type::x_mainclass, prop );
	}

	{
		PropertyImpl<std::string> *prop = new PropertyImpl<std::string>( true, _x_classpath );
		addProperty( property::type::x_classpath, prop );
	}

	{	// erase check
		PropertyImpl<std::string> *prop = (PropertyImpl<std::string>*)getProperty( property::type::src );
		prop->setCheck( boost::bind(&check::always<std::string>,_1) );
	}

	{	//	Add visible
		PropertyImpl<bool> *prop=new PropertyImpl<bool>( false, _visible );
		prop->setApply( boost::bind(&XletPlayer::applyVisible,this) );
		addProperty( property::type::visible, prop );
	}
}

void XletPlayer::onPresentationEvent( event::evtAction::type action, const std::string &label/*=""*/ )
{
	long control_code = 0;

	switch (action) {
		case event::evtAction::start: {
			control_code = APP_SIGNAL_START;
			break;
		}
		case event::evtAction::pause: {
			control_code = APP_BRIDGE_PAUSE;
			break;
		}
		case event::evtAction::resume: {
			control_code = APP_BRIDGE_RESUME;
			break;
		}
		case event::evtAction::stop:
		case event::evtAction::abort: {
			control_code = APP_SIGNAL_KILL;
			break;
		}
		default:
			return;
	}

	_pem->nclControlXlet( control_code );
}

void XletPlayer::sendAttributionEvent( const AttributionEventData &data )
{
	onAttributionEvent( data.first, data.second );
}

void XletPlayer::onAttributionEvent( const std::string &name/*=""*/, const std::string &value/*=""*/ )
{
	LWARN("XletPlayer", "Attribute Event: (Name = %s, Value = %s)", name.c_str(), value.c_str());
	//	TODO property mapping
}

void XletPlayer::stopPlayer( event::evtAction::type evtAction )
{
	onPresentationEvent( evtAction );
	GraphicPlayer::stopPlay();
}

void XletPlayer::AppStateCallback( unsigned char state )
{
	if (_callback.empty()) {
		return;
	}

	event::evtAction::type action;
	switch (state) {
		case APP_NOT_LOADED:
		case APP_DESTROYED:
			action = event::evtAction::stop;
			break;
		default:
			return;
	};

	_callback(event::evtType::presentation, action, "", "", ""); //handle only stop
}


void XletPlayer::applyVisible() {
	LWARN("XletPlayer", "Xlet Visibility: [%d]", ( _visible == true ? 1 : 0 ));
	_pem->nclSetVisible( _playerId, ( _visible == true ? 1 : 0 ) );
}
}
