#include "mngplayer.h"
#include "../device.h"
#include <canvas/mngviewer.h>
#include <util/mcr.h>
#include <boost/bind.hpp>
#include "../property/propertyimpl.h"
#include "../property/forwardproperty.h"

namespace player {

MngPlayer::MngPlayer( Device *dev )
	: GraphicPlayer( dev )
{
	_first = 0;
	_last = 0;
}

MngPlayer::~MngPlayer()
{
}

bool MngPlayer::startPlay() {
	bool result=false;
	if (GraphicPlayer::startPlay()) {
		//	Create web viewer
		_mng = device()->createMngViewer( surface() );
		if (_mng) {
			if (_first || _last) {
				_mng->setFrameParameters( _first, _last, boost::bind(&MngPlayer::stopPlay, this) );
			}
			//	Load url
			_mng->load( body(), fitMode() );
			result=true;
		}
	}
	return result;
}

void MngPlayer::stopPlay() {
	_mng->stop();
	GraphicPlayer::stopPlay();
	DEL(_mng);
}

//	Events
void MngPlayer::onSizeChanged( const canvas::Size &size ) {
	//	Redraw html
	_mng->resize( size );
}

void MngPlayer::onPositionChanged( const canvas::Point &point ) {
	_mng->move( point );
}

void MngPlayer::refresh() {
	_mng->draw();
}

void MngPlayer::apply( bool needRefresh ) {
	//	If need refresh the player, do work
	if (needRefresh) {
		refresh();
	}

	onPropertyChanged();
}

bool MngPlayer::supportRemote() const {
	return true;
}

bool MngPlayer::isApplication() const {
	return true;
}

std::string MngPlayer::fitMode() {
	std::string fitMode;
	fit::mode::type mode = getFitMode();
	switch (mode) {
		case player::fit::mode::fill: {
			fitMode = "fill";
			break;
		}
		case player::fit::mode::meet: {
			fitMode = "meet";
			break;
		}
		case player::fit::mode::meetBest: {
			fitMode = "meetBest";
			break;
		}
		case player::fit::mode::slice: {
			fitMode = "slice";
			break;
		}
		case player::fit::mode::hidden:
		default: {
			fitMode = "hidden";
			break;
		}
	}
	return fitMode;
}

void MngPlayer::dispatchKey( util::key::type key, bool isUp ) {
	_mng->dispatchKey( key, isUp );
	GraphicPlayer::dispatchKey( key, isUp );
}

void MngPlayer::registerProperties() {
	GraphicPlayer::registerProperties();
	{	//	Property attributionEvent
		ForwardProperty<AttributionEventData> *prop = new ForwardProperty<AttributionEventData>(
		boost::bind(&MngPlayer::setAdditionalParameters,this,_1), false );
		addProperty( property::type::attributionEvent, prop );
	}
}

void MngPlayer::setAdditionalParameters( const AttributionEventData &data ) {
	LINFO("MngPlayer", "Apply frame parameters: parameter name =%s", data.first.c_str());
	if (data.first == "_first" ) {
		_first = data.second;
	} else if (data.first == "_last" ) {
		_last = data.second;
	}
}

}
