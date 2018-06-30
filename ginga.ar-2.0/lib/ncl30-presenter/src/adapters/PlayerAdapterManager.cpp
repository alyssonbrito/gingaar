/******************************************************************************
 Este arquivo eh parte da implementacao do ambiente declarativo do middleware
 Ginga (Ginga-NCL).

 Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

 Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
 os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
 Software Foundation.

 Este programa eh distribuido na expectativa de que seja util, porem, SEM
 NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
 ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
 GNU versao 2 para mais detalhes.

 Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
 com este programa; se nao, escreva para a Free Software Foundation, Inc., no
 endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

 Para maiores informacoes:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 ******************************************************************************
 This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

 Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.
 Copyright 2010 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata


 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
 details.

 You should have received a copy of the GNU General Public License version 2
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

 For further information contact:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 *******************************************************************************/

#include "../../include/adapters/PlayerAdapterManager.h"
#include "../../include/adapters/ApplicationPlayerAdapter.h"
#include "../../include/adapters/FormatterPlayerAdapter.h"
#include "../../include/adapters/INclEditListener.h"
#include "generated/config.h"
#include <ncl30/animation/Animation.h>
#include <ncl30/layout/LayoutRegion.h>
#include <gingaplayer/player.h>
#include <gingaplayer/device.h>
#include <gingaplayer/system.h>
#include <util/log.h>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include "../../include/FormatterMediator.h"
using std::string;

namespace fs = boost::filesystem;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {

PlayerAdapterManager::PlayerAdapterManager( const std::string &docPath, player::System *system, FormatterMediator *formater ) {
	_docPath = docPath;
	_sys = system;
	_formater = formater;
}

PlayerAdapterManager::~PlayerAdapterManager() {
	_players.clear();
}

bool PlayerAdapterManager::start( bptgnmc::ExecutionObject* object ) {
	PlayerPtr player;
	if (object != NULL) {
		Players::iterator i = _players.find( object->getId() );
		if (i == _players.end()) {
			player = initializePlayer( object );
		} else {
			player = (i->second);
		}
	}

	return player ? player->start() : false;
}

void PlayerAdapterManager::onStopTimer( bptgnmc::ExecutionObject* object, bool ms ) {
	//	Enqueue because timelinetimer belongs to the player
	_sys->enqueue( boost::bind( &PlayerAdapterManager::stop, this, object, false ) );
}
bool PlayerAdapterManager::stop( bptgnmc::ExecutionObject* object, bool proccessTransitions /*=true*/) {
	FormatterPlayerAdapter *player = getPlayer( object );
	if (player != NULL && player->stop( proccessTransitions )) {
		deletePlayer( object->getId() );
		return true;
	}
	return false;
}

bool PlayerAdapterManager::pause( bptgnmc::ExecutionObject* object ) {
	FormatterPlayerAdapter *player = getPlayer( object );
	if (player != NULL) {
		return player->pause();
	}
	return false;
}

bool PlayerAdapterManager::resume( bptgnmc::ExecutionObject* object ) {
	FormatterPlayerAdapter *player = getPlayer( object );
	if (player != NULL) {
		return player->resume();
	}
	return false;
}

bool PlayerAdapterManager::abort( bptgnmc::ExecutionObject* object ) {
	FormatterPlayerAdapter *player = getPlayer( object );
	if (player != NULL && player->abort()) {

		//LGESP-826
		player::Player *player2 = player->getPlayer();
		if (player2->instanceOf( "SoundPlayer" ))
		{
			player2->stop();
		}	
	
		return stop( object );
	};
	return false;
}

bool PlayerAdapterManager::hasPrepared( bptgnmc::ExecutionObject* object ) {
	FormatterPlayerAdapter *player = getPlayer( object );
	return player != NULL && player->hasPrepared();
}

bool PlayerAdapterManager::prepare( bptgnmc::ExecutionObject* object, bptgnme::FormatterEvent* event ) {
	FormatterPlayerAdapter *player = getPlayer( object, true );
	return player != NULL && player->prepare( object, event );
}

bool PlayerAdapterManager::setCurrentEvent( bptgnmc::ExecutionObject* object, bptgnme::FormatterEvent* event ) {
	FormatterPlayerAdapter *player = getPlayer( object );
	return player != NULL && player->setCurrentEvent( (bptgnme::AttributionEvent*) event );
}

void PlayerAdapterManager::setProperty( bptgnmc::ExecutionObject* object, bptgnme::FormatterEvent* event, const std::string &value ) {
	FormatterPlayerAdapter *player = getPlayer( object );
	if (player != NULL) {
		player->setPropertyValue( (bptgnme::AttributionEvent*) event, value );
	}
	else
	{
		std::string propName = ( ((bptgnme::AttributionEvent*) event)->getAnchor())->getPropertyName();

		if ( propName.compare("visible") == 0 && value.compare("true") == 0 )
		{
			vector<Node*>* parentNodes;
			vector<Node*>::iterator it;
			parentNodes  = object->getParentNodes();
			for (it = parentNodes->begin(); it != parentNodes->end(); ++it) {
				Node* parent = (Node*) *it;
				if (parent->instanceOf( "ContextNode" ) || parent->instanceOf("DocumentNode")){
					bptni::PropertyAnchor * prop = (bptni::PropertyAnchor *) parent->getAnchor("visible");
					if (prop!=NULL){
						if (prop->getPropertyName()=="visible" && prop->getPropertyValue()=="false"){
							prop->setPropertyValue("true");
						}
					}
				}
			}
		}
	}
}

bool PlayerAdapterManager::startAnimation( bptgnmc::ExecutionObject* object, bptgnme::AttributionEvent* event, const std::string &value, bptna::Animation* animation ) {
    bool ret;
  APRINT_NCLP("==>");
	FormatterPlayerAdapter *player = getPlayer( object );
	ret= player != NULL && player->startAnimation( event, value, animation );
  APRINT_NCLP("<==");
  return ret;
}

::util::DWORD PlayerAdapterManager::uptime( bptgnmc::ExecutionObject* object ) {
	FormatterPlayerAdapter *player = getPlayer( object );
	return player ? getPlayer( object )->uptime() : (::util::DWORD)0;
}

bool PlayerAdapterManager::setKeyHandler( bptgnmc::ExecutionObject* object, bool handler ) {
	FormatterPlayerAdapter *player = getPlayer( object );
	return player != NULL && player->setKeyHandler( handler );
}

void PlayerAdapterManager::onTransitionTimer( bptgnmc::ExecutionObject* object, double ms ) {
	//	Enqueue because timelinetimer belongs to the player
	_sys->enqueue( boost::bind( &PlayerAdapterManager::proccessTransition, this, object, ms ) );
}

void PlayerAdapterManager::updateGraphicPresentation() {
	for (Players::iterator it = _players.begin(); it != _players.end(); it++) {
		it->second->getPlayer()->updateImage();
	}
}

void PlayerAdapterManager::proccessTransition( bptgnmc::ExecutionObject* object, double ms ) {
	if (object->updateTransitionTable( ms, ContentAnchor::CAT_TIME )) {
		FormatterPlayerAdapter *player = getPlayer( object, true );
		if (player) {
			player->processTransitions();
		}
	} else {
		stop( object, true );
	}
}

bool PlayerAdapterManager::deletePlayer( const std::string &objectId ) {
	Players::iterator i = _players.find( objectId );
	if (i != _players.end()) {
		_players.erase( i );
		return true;
	}
	return false;
}

PlayerAdapterManager::PlayerPtr PlayerAdapterManager::initializePlayer( ExecutionObject* object ) {
	CascadingDescriptor* descriptor;
	NodeEntity* dataObject;
	PlayerPtr player( (FormatterPlayerAdapter*) NULL );

	LayoutRegion* region;

	if (!object) {
		LWARN( "PlayerAdapterManager", "init, invalid object" );
		return player;
	}

	dataObject = (NodeEntity*) (object->getDataObject()->getDataEntity());
	if (((ContentNode*) dataObject)->isSettingNode()) {
		LWARN( "PlayerAdapterManager", "init, not in setting mode" );
		return player;
	}

	//	Get device
	descriptor = object->getDescriptor();
	if (!descriptor) {
		LWARN( "PlayerAdapterManager", "init, object without descriptor" );
		return player;
	}

	FormatterRegion *fr = descriptor->getFormatterRegion();
	if (fr == NULL) {
		LWARN( "PlayerAdapterManager", "init, couldn't find FormatterRegion" );
		return player;
	}

	region = fr->getLayoutRegion();
	bptnl::Device &device = region->getDevice();
	player::Device *dev = _sys->getDevice( device.name, device.number );
	if (!dev) {
		LWARN( "PlayerAdapterManager", "init, device not found: %s", device.name.c_str() );
		return player;
	}

	//	Get url and mime type
	std::string mimeType;
	std::string url;
	if (dataObject->instanceOf( "ContentNode" )) {
		mimeType = ((ContentNode*) dataObject)->getNodeType();
		url = ((ContentNode*) dataObject)->getUrl();
	}

	//	Create player
	player::Player *devPlayer = dev->create( url, mimeType );
	if (devPlayer) {
		devPlayer->setProperty( "id", dataObject->getId() );
		if (devPlayer->isApplication()) {
			player = PlayerPtr( new application::ApplicationPlayerAdapter( this, _sys ) );
		} else {
			player = PlayerPtr( new FormatterPlayerAdapter( this, _sys ) );
		}
		player->setPlayer( devPlayer );
		_players[object->getId()] = player;
		devPlayer->setPEM(getFormatterMediator()->getPEM());
	} else {
		LWARN( "PlayerAdapterManager", "init, cannot create player for mime type: type=%s", mimeType.c_str() );
		return player;
	}

	return player;
}

FormatterPlayerAdapter* PlayerAdapterManager::getPlayer( ExecutionObject* object, bool init/*=false*/) {
	FormatterPlayerAdapter* player = NULL;
	if (object != NULL) {
		Players::iterator i = _players.find( object->getId() );
		if (i == _players.end()) {
			if (init) {
				PlayerPtr newPlayer( initializePlayer( object ) );
				player = newPlayer.get();
			}
		} else {
			player = (i->second.get());
		}
	}
	return player;
}

bool PlayerAdapterManager::getUrl( const std::string &fileName, std::string &url ) {
	fs::path filePath( fileName );
	if (!boost::find_first( fileName, "://" )) {
		fs::path docPath( _docPath );
		if (!filePath.is_absolute()) {
			filePath = docPath.parent_path() / filePath;
		}

		onFetchFilesFromCarousel(filePath.string());

		url = "file://" + filePath.string();
	} else {
	  char str_begin[10];
	  char str_end[1000];
	  sscanf(fileName.c_str(),"%s://%s",str_begin,str_end);

	  onFetchFilesFromCarousel(str_end);

	  url = fileName;
	}
	return exists( filePath );
}

FormatterMediator* PlayerAdapterManager::getFormatterMediator() {
	return _formater;
}

void PlayerAdapterManager::setPlayerKeyHandler( ExecutionObject *object ) {
	FormatterPlayerAdapter *player = getPlayer( object );
	if (player != NULL) {
		player->setPlayerKeyHandler();
	}
}

void PlayerAdapterManager::setFormatterKeyHandler( ExecutionObject *object ) {
	FormatterPlayerAdapter *player = getPlayer( object );
	if (player != NULL) {
		player->setFormatterKeyHandler();
	}
}

}
}
}
}
}
}

