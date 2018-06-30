#include "htmlplayer.h"
#include "../device.h"
#include "../player.h"
#include <canvas/webviewer.h>
#include <util/mcr.h>
#include <boost/bind.hpp>
#include "../property/forwardproperty.h"
#include "../property/propertyimpl.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace player {

HtmlPlayer::HtmlPlayer( Device *dev )
	: GraphicPlayer( dev )
{
}

HtmlPlayer::~HtmlPlayer()
{
}

bool HtmlPlayer::startPlay() {
	bool result=false;
	if (GraphicPlayer::startPlay()) {
		//	Create web viewer
		_html = device()->createWebViewer( surface() );
		if (_html) {
			//	Load url
			_html->load( url() );
			result=true;
		}
	}
	return result;
}

void HtmlPlayer::stopPlay() {
	_html->stop();
	GraphicPlayer::stopPlay();
	DEL(_html);
}

//	Events
void HtmlPlayer::onSizeChanged( const canvas::Size &size ) {
	//	Redraw html
	_html->resize( size );
}

void HtmlPlayer::refresh() {
	_html->draw();
}

bool HtmlPlayer::supportRemote() const {
	return true;
}

void HtmlPlayer::registerProperties() {
	LDEBUG("HtmlPlayer", "registerProperties");
	//	Setup graphic player
	GraphicPlayer::registerProperties();

	{	//	Style
		PropertyImpl<std::string> *prop = new PropertyImpl<std::string>( true, _style );
		prop->setApply( boost::bind(&HtmlPlayer::applyStyle,this) );
		addProperty( property::type::style, prop );
	}
}

void HtmlPlayer::applyStyle() {
	LDEBUG("HtmlPlayer", "apply style, value=%s", _style.c_str());

	fs::path workingDir = fs::path(body()).remove_filename().string().c_str();
	workingDir /= _style;
		
	if (!fs::exists( workingDir )){
		
		LDEBUG("HtmlPlayer", "workingDir, value=%s", workingDir.c_str());
		onFetchFilesFromCarousel(workingDir.string());
	
	}

	if (fs::is_regular_file( workingDir )) {
		_html->setStyle( workingDir.string() );
	}	


}

void HtmlPlayer::dispatchKey( util::key::type key, bool isUp ) {
	_html->dispatchKey( key, isUp );
	GraphicPlayer::dispatchKey( key, isUp );
}

bool HtmlPlayer::isApplication() const {
	return true;
}

}
