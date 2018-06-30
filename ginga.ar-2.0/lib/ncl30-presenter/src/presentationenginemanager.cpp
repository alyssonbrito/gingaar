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

#include "presentationenginemanager.h"
#include "../include/FormatterMediator.h"
#include "../include/adapters/FormatterPlayerAdapter.h"
#include "../include/privatebase/IPrivateBaseManager.h"
#include "generated/config.h"
#include <gingaplayer/system.h>
#include <gingaplayer/device.h>
#include <gingaplayer/player/settings.h>
#include <canvas/size.h>
#include <canvas/system.h>
#include <util/keydefs.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <stdio.h>

using std::string;
using std::vector;
using std::map;

using ::br::pucrio::telemidia::ginga::ncl::FormatterMediator;
using ::br::pucrio::telemidia::ginga::ncl::PrivateBaseManager;
using ::br::pucrio::telemidia::ginga::ncl::adapters::FormatterPlayerAdapter;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {

PresentationEngineManager::PresentationEngineManager( player::System *sys ) :
		_sys( sys ),
		_formatter( NULL ),
		_paused( false ),
		_stopped( false ),
		_settings( NULL ),
		_currentPrivateBaseId( "-1" ) {
	assert( _sys );
	sys->setPem( this );
}

PresentationEngineManager::~PresentationEngineManager() {
	delete _settings;
}

bool PresentationEngineManager::setDocument( const std::string &file ) {
	return startDocument( file );
}

void PresentationEngineManager::setBaseId( const std::string &id ) {
	_currentPrivateBaseId = id;
}

void* PresentationEngineManager::runEmbedded( const std::string &url, player::System *sys ) {
  APRINT_NCLP("<==>");
	LDEBUG( "PresentationEngineManager", "Start embedded: doc=%s", url.c_str() );
	PresentationEngineManager *pem = new PresentationEngineManager(sys);
	pem->startDocument( url, true );
	return pem;
}

void PresentationEngineManager::stopEmbedded( void* window ) {
  APRINT_NCLP("<==>");
	LDEBUG( "PresentationEngineManager", "Stop embedded" );
	PresentationEngineManager *pem = static_cast<PresentationEngineManager*>(window);
	pem->stopEmbeddedDocument();
	delete pem;
}

//	Compare
static boost::condition_variable _stopWakeup;
static boost::mutex _stopMutex;

bool PresentationEngineManager::startDocument( const std::string &file, bool embedded ) {
  APRINT_NCLP("<==>");
	LDEBUG( "Port", "Start document: doc=%s", file.c_str() );

	if (formatter()) {
		_sys->enqueue( boost::bind( &PresentationEngineManager::stopDocument, this ) );
		{ //	Wait for compare
			boost::unique_lock<boost::mutex> lock( _stopMutex );
			while (!_stopped) {
				_stopWakeup.wait( lock );
			}
			LDEBUG( "Port", "End document: doc=%s", file.c_str() );
			_stopped = false;
		}
	}

	//	Init settings module
	if (!_settings) {
		_settings = new player::settings::Settings();
	}
	_settings->init();
	_sys->setSettings( _settings );
	_sys->settings()->setOnGetNativeProperty(_sys->getNativePropertyCallback);

	initFormatter( file );
	if (!formatter()->parseDocument()) {
		LWARN( "Port", "parseDocument fail" );
		return false;
	}

	//	Play formatter
	_sys->enqueue( boost::bind( &FormatterMediator::play, formatter() ) );

	return true;
}

void PresentationEngineManager::stopEmbeddedDocument() {
	LDEBUG( "PresentationEngineManager", "Stop embedded document" );
	//	Stop formatter
	formatter()->stop();
	delFormatter();
	_stopped = true;
}
void PresentationEngineManager::qStopDocument() {
	LDEBUG( "Port", "Enqueue Stop document" );
	_sys->enqueue( boost::bind( &PresentationEngineManager::stopDocument, this ) );
}

void PresentationEngineManager::stopDocument() {
	LDEBUG( "Port", "Stop document" );
	_stopMutex.lock();

	//	Stop formatter
	LDEBUG( "Port", "Stop formatter" );
	formatter()->stop();
	delFormatter();

	//	Stop settings module
	LDEBUG( "Port", "Stop setting module" );

	_stopped = true;
	_stopMutex.unlock();

	LDEBUG( "Port", "Stop notify" );
	_stopWakeup.notify_one();
	LDEBUG( "Port", "Stop finished" );
	onStop();
}

bool PresentationEngineManager::run() {
  APRINT_NCLP("<==>");
	if (!_sys->canRun()) {
		LWARN( "Port", "no device configurated" );
		return false;
	}

	onStart();

#if 0
	{	//	Add listener for keys
		::util::key::Keys keys;

		keys.insert( ::util::key::greater_than_sign );
		keys.insert( ::util::key::less_than_sign );

		keys.insert( ::util::key::f10 );
		keys.insert( ::util::key::power );
		keys.insert( ::util::key::escape );
		keys.insert( ::util::key::stop );
		keys.insert( ::util::key::exit );

		keys.insert( ::util::key::f12 );
		keys.insert( ::util::key::pause );

		player::input::InputCallback fnc = boost::bind( &PresentationEngineManager::userEventReceived, this, _1, _2 );
		_sys->addInputListener( this, fnc, keys );
	}
#endif

	//	Run canvas loop
	_sys->run();

	stopDocument();
	onStop();

	//	Remove listeners
	_sys->delInputListener( this );

	return true;
}


#if 0
void PresentationEngineManager::userEventReceived( ::util::key::type key, bool isUp ) {
	if (!isUp) {
		return;
	}

	switch ( key ) {
		case ::util::key::f10:
		case ::util::key::power:
		case ::util::key::escape:
		case ::util::key::stop:
		case ::util::key::exit: {
			_sys->exit();
			break;
		}
		case ::util::key::f12:
		case ::util::key::pause: {
			if (_paused) {
				formatter()->pause();
			} else {
				formatter()->resume();
			}
			_paused = !_paused;
			break;
		}
		case ::util::key::greater_than_sign: {
			formatter()->forward();
			break;
		}
		case ::util::key::less_than_sign: {
			formatter()->backward();
			break;
		}
		default:
			break;
	};

}
#endif

void PresentationEngineManager::onKeyPress( const KeyPressCallback &callback ) {
	FormatterPlayerAdapter::onKeyPress( callback );
	FormatterFocusManager::onKeyPress( callback );
}

//	Enqueue task into presentation thread
void PresentationEngineManager::enqueue( const boost::function<void( void )> &func ) {
	_sys->enqueue( func );
}

player::Device *PresentationEngineManager::mainScreen() const {
	return _sys->getDevice( "systemScreen(0)", 0 );
}

player::System *PresentationEngineManager::sys() {
	return _sys;
}

FormatterMediator *PresentationEngineManager::formatter() {
	return _formatter;
}

const std::string &PresentationEngineManager::baseId() {
	return _currentPrivateBaseId;
}

void PresentationEngineManager::initFormatter( const std::string &file ) {
	_formatter = new FormatterMediator( _currentPrivateBaseId, file, _sys );
	_formatter->setPEM(this);
}

void PresentationEngineManager::delFormatter() {
	DEL( _formatter );
}

void PresentationEngineManager::setOnStopDocument(const notifyOnStopDoc &_callback){
	_onStopCallback = _callback;
}

void PresentationEngineManager::onStart() {
}

void PresentationEngineManager::onStop() {
	LDEBUG( "Port", "onStop in" );
	if (_onStopCallback != 0) {
		_onStopCallback(); //onStoppedApp
	}
	LDEBUG( "Port", "onStop out" )
}


bool PresentationEngineManager::nclControlDoc( int controlCode, std::string param )
{
	bool result = true;
	switch(controlCode) {
		case NCL_SET_DOC: {
			result = setDocument( param );
			break;
		}
		case NCL_START_DOC: {
			formatter()->play();
			break;
		}
		case NCL_PAUSE_DOC: {
			if (_paused) {
				formatter()->pause();
			} 
			_paused = !_paused;
			break;
		}
		case NCL_RESUME_DOC: {
			if (!_paused) {
				formatter()->resume();
			}
			_paused = !_paused;
			break;
		}
		case NCL_STOP_DOC: {
			formatter()->stop();
			break;
		}
		default:
			break;
	}
	return result;
}

bool PresentationEngineManager::nclEdit( int command, string param1, string param2, string param3 ) {

	bool result = false;

	string docId = formatter()->getdocId();
	switch(command) {
		case ADD_REGION: {
			LayoutRegion *res = formatter()->addRegion( docId, param1, param2, param3 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_REGION: {
			LayoutRegion *res = formatter()->removeRegion( docId, param1, param2 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_REGION_BASE: {
			RegionBase *res = formatter()->addRegionBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_REGION_BASE: {
			RegionBase *res = formatter()->removeRegionBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_RULE: {
			Rule *res = formatter()->addRule(docId, param1);
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_RULE: {
			Rule *res = formatter()->removeRule( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_RULE_BASE: {
			RuleBase *res = formatter()->addRuleBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_RULE_BASE: {
			RuleBase *res = formatter()->removeRuleBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_CONNECTOR: {
			Connector *res = formatter()->addConnector( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_CONNECTOR: {
			Connector *res = formatter()->removeConnector( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_CONNECTOR_BASE: {
			ConnectorBase *res = formatter()->addConnectorBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_CONNECTOR_BASE: {
			ConnectorBase *res = formatter()->removeConnectorBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_DESCRIPTOR: {
			GenericDescriptor *res = formatter()->addDescriptor( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_DESCRIPTOR: {
			GenericDescriptor *res = formatter()->removeDescriptor( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_DESCRIPTOR_SWITCH: {
			//DescriptorSwitch *res = formatter()->addDescriptorSwitch(docId, param1);
			GenericDescriptor *res = formatter()->addDescriptor( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_DESCRIPTOR_SWITCH: {
			//DescriptorSwitch *res = formatter()->removeDescriptorSwitch(docId, param1);
			GenericDescriptor *res = formatter()->removeDescriptor( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_DESCRIPTOR_BASE: {
			DescriptorBase *res = formatter()->addDescriptorBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_DESCRIPTOR_BASE: {
			DescriptorBase *res = formatter()->removeDescriptorBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_TRANSITION: {
			Transition *res = formatter()->addTransition( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_TRANSITION: {
			Transition *res = formatter()->removeTransition( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_TRANSITION_BASE: {
			TransitionBase *res = formatter()->addTransitionBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_TRANSITION_BASE: {
			TransitionBase *res = formatter()->removeTransitionBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_IMPORT_BASE: {
			Base *res = formatter()->addImportBase( docId, param1, param2 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_IMPORT_BASE: {
			Base *res = formatter()->removeImportBase( docId, param1, param2 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_IMPORT_DOC_BASE: {
			NclDocument *res = formatter()->addImportedDocumentBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_IMPORT_DOC_BASE: {
			NclDocument *res = formatter()->removeImportedDocumentBase( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_IMPORT_NCL: {
			NclDocument *res = formatter()->addImportNCL( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_IMPORT_NCL: {
			NclDocument *res = formatter()->removeImportNCL( docId, param1 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_NODE: {
			Node *res = formatter()->addNode( docId, param1, param2 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_NODE: {
			Node *res = formatter()->removeNode( docId, param1, param2 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_INTERFACE: {
			InterfacePoint *res = formatter()->addInterface( docId, param1, param2 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_INTERFACE: {
			InterfacePoint *res = formatter()->removeInterface( docId, param1, param2 );
			if(res) {
				result = true;
			}
			break;
		}
		case ADD_LINK: {
			Link *res = formatter()->addLink( docId, param1, param2 );
			if(res) {
				result = true;
			}
			break;
		}
		case REMOVE_LINK: {
			Link *res = formatter()->removeLink( docId, param1, param2 );
			if(res) {
				result = true;
			}
			break;
		}
		case SET_PROPERTY: {
//			bool res = formatter()->setPropertyValue( docId, );
//			if(res) {
//				result = true;
//			}
			break;
		}
		default:
			break;
	};
	return result;
}

string PresentationEngineManager::nclSettingsNodesGetValue( string name )
{
	return _settings->getProperty( name );
}

std::string PresentationEngineManager::nclGetPropertyValue( std::string name )
{
	return 0;
}

/*bool */ void PresentationEngineManager::nclSetVisible( int id, bool visible )
{
	if (_NCLPlayerSetVisibility.empty()) {
		return;// false;
	}

	//_sys->setVisible( visible );
	_NCLPlayerSetVisibility( id, visible );
	//return true;
}

void PresentationEngineManager::registerNCLPlayerSetVisibility( const cbNCLXletVisibility &callback ) {
	_NCLPlayerSetVisibility = callback;
}

bool PresentationEngineManager::nclSetBounds( int width, int height, int x1, int y1, int x2, int y2 )
{
	_sys->changeVideo( x1, y1, width, height );
	return true;
}

void PresentationEngineManager::RegisterNCLPlayerEvent( const notifyOfNCLPlayerEvent &callback ) {
	_NCLPlayerEvent = callback;
}

void PresentationEngineManager::notifyNCLPlayerEvent( short actionType, std::string propValue ) {
	if (_NCLPlayerEvent.empty()) {
		return;
	}
	switch(actionType) {
		case SimpleAction::ACT_START:
			_NCLPlayerEvent( NCL_EVENT_START, propValue );
			break;
		case SimpleAction::ACT_STOP:
			_NCLPlayerEvent( NCL_EVENT_STOP, propValue );
			break;
		case SimpleAction::ACT_PAUSE:
			_NCLPlayerEvent( NCL_EVENT_PAUSE, propValue );
			break;
		case SimpleAction::ACT_RESUME:
			_NCLPlayerEvent( NCL_EVENT_RESUME, propValue );
			break;
		case SimpleAction::ACT_ABORT:
			_NCLPlayerEvent( NCL_EVENT_ABORT, propValue );
			break;
		case SimpleAction::ACT_SET:
			_NCLPlayerEvent( NCL_EVENT_ATTRIBUTION_SET, propValue );
			break;
		default:
			break;
	}
}

void PresentationEngineManager::nclStartXlet( const std::string &initialClass, const std::string &classPath, int x, int y, unsigned int width, unsigned int height )
{
	if (_NCLStartXlet.empty()) {
		return;
	}
	_NCLStartXlet( initialClass, classPath, x, y, width, height );
}

void PresentationEngineManager::registerNCLStartXlet( const cbNCLStartXlet &callback )
{
        _NCLStartXlet = callback;
}

void PresentationEngineManager::nclControlXlet( long controlCode )
{
	if (_NCLControlXlet.empty()) {
		return;
	}
	_NCLControlXlet( controlCode );
}

void PresentationEngineManager::registerNCLControlXlet (const cbNCLControlXlet &callback )
{
        _NCLControlXlet = callback;
}

void PresentationEngineManager::nclNotifyXletAppState( unsigned char state )
{
	if (_XletAppState.empty()) {
		return;
	}
	_XletAppState( state );
}

void PresentationEngineManager::registerXletAppStateCallback( const notifyOfXletAppState &callback )
{
	_XletAppState = callback;
}

}
}
}
}
}
