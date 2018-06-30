#include "nclplayer.h"
#include "../device.h"
#include "../system.h"
#include <util/mcr.h>
#include <boost/bind.hpp>
#include "../property/forwardproperty.h"
#include <canvas/surface.h>
#include <canvas/system.h>
#include <canvas/window.h>
#include <boost/filesystem/operations.hpp>
#include "../../../ncl30-presenter/src/presentationenginemanager.h"
namespace bptgn = br::pucrio::telemidia::ginga::ncl;

namespace fs = boost::filesystem;

namespace player {

NclPlayer::NclPlayer( Device *dev )
	: GraphicPlayer( dev )
{
	_pemem = NULL;
	_sys = NULL;
	_dev = NULL;
}

NclPlayer::~NclPlayer()
{
}

bool NclPlayer::startPlay() {
	if (GraphicPlayer::startPlay()) {
			std::string url = body();
			if (fs::exists( url ) && fs::is_regular_file( url )) {
				fs::path workingDir(url);
			if (chdir(workingDir.remove_filename().string().c_str()) == 0) {
				_sys = new System();
				_dev = new Device( _sys );
				_dev->setEmbeddedSize(surface()->getSize());
				_dev->initialize(surface());
				_sys->addDevice( _dev );
				_pemem = ((bptgn::PresentationEngineManager*)(getDevice()->systemPlayer()->getPem()))->runEmbedded( url, _sys );
				return true;
	}
			LDEBUG("NclPlayer", "Cannot change working directory: %s", workingDir.string().c_str());
	}
}
	return false;
}

void NclPlayer::stopPlay() {
	if (_pemem) {
		((bptgn::PresentationEngineManager*)(getDevice()->systemPlayer()->getPem()))->stopEmbedded( _pemem );
		delete _sys;
		_sys = NULL;
		_pemem = NULL;
		_dev = NULL;
	}
	GraphicPlayer::stopPlay();
}

//	Events
void NclPlayer::onSizeChanged( const canvas::Size &size ) {
	_dev->system()->window()->resize();
}

void NclPlayer::refresh() {
	surface()->markDirty();
	surface()->flush();
}

bool NclPlayer::supportRemote() const {
	return false;
}

bool NclPlayer::isApplication() const {
	return true;
}

void NclPlayer::dispatchKey( util::key::type key, bool isUp ) {
	GraphicPlayer::dispatchKey( key, isUp );
}

}
