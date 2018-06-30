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

#include "../../include/focus/FormatterFocusManager.h"
#include <gingaplayer/player/settings.h>
#include <gingaplayer/system.h>
#include "generated/config.h"
#include "../../include/emconverter/FormatterConverter.h"
#include <util/log.h>
#include <util/keydefs.h>

using namespace ::br::pucrio::telemidia::ginga::ncl::emconverter;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace focus {

namespace impl {
static FormatterFocusManager::KeyPressCallback _onKey;
}

FormatterFocusManager::FormatterFocusManager( PlayerAdapterManager* playerManager, emconverter::FormatterConverter* converter, player::System *system ) {
	currentFocus = NULL;
	isHandler = false;
	selectedObject = NULL;
	this->playerManager = playerManager;
	this->converter = converter;
	_sys = system;
	isStartComposition = false;
}

FormatterFocusManager::~FormatterFocusManager() {
	_sys->settings()->setProperty( "service.currentKeyMaster", "" );
	_sys->settings()->setProperty( "service.currentFocus", "" );
	unregister();
	selectedObject = NULL;
	playerManager = NULL;
	converter = NULL;
	_sys = NULL;
}

bool FormatterFocusManager::setKeyHandler( bool isHandler ) {
	if (this->isHandler == isHandler) {
		return false;
	}

	this->isHandler = isHandler;
	converter->setHandlingStatus( isHandler );

	return isHandler;
}

ExecutionObject* FormatterFocusManager::getObjectFromFocusIndex( const std::string &focusIndex ) {
	vector<ExecutionObject*>::iterator j;
	CascadingDescriptor* desc;

	j = _objects.begin();
	while (j != _objects.end()) {
		desc = (*j)->getDescriptor();
		if (desc->getFormatterRegion()->isVisible() || (*j)->isTransitionInVisible()) {
			if (focusIndex == desc->getFocusIndex()) {
				return *j;
			}
		}
		++j;
	}

	return NULL;
}

static int focusIndexCmp( const char* s1, const char* s2 )
{
	int l1 = strlen(s1);
	int l2 = strlen(s2);
	while (*s1 && *s1 < '1') {
		s1++;
		l1--;
	}

	while (*s2 && *s2 < '1') {
		s2++;
		l2--;
	}

	if (l1 != l2) {
		return (l1 - l2);
	}

	while (*s1 && *s1 == *s2) {
		s1++;
		s2++;
	}

	return (*s1 - *s2);
}

void FormatterFocusManager::setKeyMaster( const std::string &focusIndex ) {
	ExecutionObject* nextObject = NULL;
	CascadingDescriptor* nextDescriptor = NULL;

	if (focusIndex == "" && selectedObject != NULL) {
		selectedObject->setHandler( false );
		selectedObject->getDescriptor()->getFormatterRegion()->setSelection( false );
		exitSelection( selectedObject );
		selectedObject = NULL;
		return;
	}

	nextObject = getObjectFromFocusIndex( focusIndex );
	if (nextObject == NULL) {
		nextObject = converter->getObjectFromNodeId( focusIndex );

		if (nextObject == NULL) {
			LWARN("FormatterFocusManager", "setKeyMaster: can't set '%s' as master: object is not available", focusIndex.c_str());
			return;
		}
	}

	if (selectedObject == nextObject) {
		return;
	}

	nextDescriptor = nextObject->getDescriptor();
	if (nextDescriptor == NULL) {
		LWARN("FormatterFocusManager", "setKeyMaster: can't set '%s' as master: NULL descriptor", focusIndex.c_str());
		return;
	}

	if (nextDescriptor->getFormatterRegion() != NULL) {
		if (nextDescriptor->getFormatterRegion()->isVisible() && nextDescriptor->getFormatterRegion()->setSelection(true)) {
			//	recovering old selected object
			if (selectedObject != NULL) {
				selectedObject->setHandler( false );
				selectedObject->getDescriptor()->getFormatterRegion()->setSelection( false );
			}

			//	selecting new object
			selectedObject = nextObject;
			selectedObject->setHandler( true );

			enterSelection( selectedObject );
			nextObject->selectionEvent( ::util::key::null, playerManager->uptime( selectedObject ) );

		} else {
			LWARN("FormatterFocusManager", "setKeyMaster: can't select '%s'", focusIndex.c_str());
		}

	} else {
		LWARN("FormatterFocusManager", "setKeyMaster: can't select '%s' region == NULL", focusIndex.c_str());
	}
}

void FormatterFocusManager::setFocus( const std::string &focusIndex ) {
	ExecutionObject* nextObject = NULL;

	if ((currentFocus && focusIndex == currentFocus->getDescriptor()->getFocusIndex()) || focusIndex == "" || focusIndex == "nill") {
		LWARN("FormatterFocusManager", "setFocus: index '%s' is ignored", focusIndex.c_str());
		return;
	}

	nextObject = getObjectFromFocusIndex( focusIndex );
	if (nextObject == NULL) {
		LWARN("FormatterFocusManager", "setFocus: index '%s' is not visible", focusIndex.c_str());
		return;
	}

	if (currentFocus) {
		currentFocus->getDescriptor()->getFormatterRegion()->setFocus( false );
	}

	currentFocus = nextObject;
	_sys->settings()->setProperty( "service.currentFocus", focusIndex );
	nextObject->getDescriptor()->getFormatterRegion()->setFocus( true );
}

void FormatterFocusManager::showObject( ExecutionObject* object ) {
	CascadingDescriptor* descriptor;
	string focusIndex, objectId;
	string paramValue;
	vector<string>::iterator i;
	bool isId = false;

	if (object == NULL) {
		LWARN("FormatterFocusManager", "showObject: object is null");
		return;
	}

	descriptor = object->getDescriptor();
	if (descriptor == NULL || !descriptor->getFormatterRegion()) {
		LWARN("FormatterFocusManager", "showObject: descriptor or region is null");
		return;
	}

	_objects.push_back(object);

	focusIndex = descriptor->getFocusIndex();
	objectId = object->getDataObject()->getDataEntity()->getId();
	if (focusIndex == "" || focusIndex == "nill") {
		isId = true;
	}

	if (!descriptor->getFormatterRegion()->isVisible() && !object->isTransitionInVisible()) {
		return;
	}

	paramValue = _sys->settings()->getProperty( "service.currentKeyMaster" );
	if (paramValue != "" && (paramValue == objectId || paramValue == focusIndex)) {
		setKeyMaster(paramValue);
	}

	paramValue = _sys->settings()->getProperty( "service.currentFocus" );
	if (paramValue != "" && paramValue == focusIndex && !isId) {
		setFocus( focusIndex );
	}

	if (!currentFocus && !isStartComposition) {
		focusSmallest();
	}
}

void FormatterFocusManager::hideObject( ExecutionObject* object ) {
	if (object == NULL || object->getDescriptor() == NULL) {
		return;
	}

	if (selectedObject == object) {
		exitSelection( selectedObject );
		selectedObject = NULL;
	}

	std::vector<ExecutionObject*>::iterator it = std::find( _objects.begin(), _objects.end(), object);
	if (it != _objects.end()) {
		_objects.erase( it );
	}

	if (currentFocus == object) {
		currentFocus = NULL;
		focusSmallest();
	}
}

bool FormatterFocusManager::keyCodeOk( ExecutionObject* currentObject ) {
	selectedObject = currentObject;
	selectedObject->setHandler( true );
	string id = selectedObject->getDataObject()->getDataEntity()->getId();

	changeSettingState( "service.currentKeyMaster", "start", id );
	enterSelection( selectedObject );
	selectedObject->selectionEvent( ::util::key::null, playerManager->uptime( selectedObject ) );
	changeSettingState( "service.currentKeyMaster", "stop", id );

	return true;
}

bool FormatterFocusManager::keyCodeBack() {
	selectedObject->setHandler( false );
	selectedObject->getDescriptor()->getFormatterRegion()->setSelection( false );

	changeSettingState( "service.currentKeyMaster", "start", "" );
	exitSelection( selectedObject );
	selectedObject = NULL;
	changeSettingState( "service.currentKeyMaster", "stop", "" );

	std::string paramValue = _sys->settings()->getProperty( "service.currentFocus" );
	setFocus(paramValue);
	if (!currentFocus) {
		focusSmallest();
	}

	return false;
}

bool FormatterFocusManager::enterSelection( ExecutionObject* object ) {
	string keyMaster = (selectedObject->getDataObject()->getDataEntity()->getId());
	std::string focusIndex = object->getDescriptor()->getFocusIndex();
	std::string paramValue = _sys->settings()->getProperty( "service.currentKeyMaster" );
	if (paramValue.empty() || paramValue != focusIndex) {
		_sys->settings()->setProperty( "service.currentKeyMaster", keyMaster );
	}
	playerManager->setKeyHandler( object, true );
	return true;
}

void FormatterFocusManager::exitSelection( ExecutionObject* object ) {
	playerManager->setKeyHandler( object, false );
	_sys->settings()->setProperty( "service.currentKeyMaster", "" );
}

void FormatterFocusManager::registerNavigationKeys() {
	::util::key::Keys keys;

	keys.insert( ::util::key::cursor_down );
	keys.insert( ::util::key::cursor_left );
	keys.insert( ::util::key::cursor_right );
	keys.insert( ::util::key::cursor_up );

	keys.insert( ::util::key::enter );
	keys.insert( ::util::key::ok );

	keys.insert( ::util::key::backspace );
	keys.insert( ::util::key::back );

	player::input::InputCallback fnc = boost::bind( &br::pucrio::telemidia::ginga::ncl::focus::FormatterFocusManager::userEventReceived, this, _1, _2 );
	_sys->addInputListener( this, fnc, keys );
}

void FormatterFocusManager::unregister() {
	_sys->delInputListener( this );
}

void FormatterFocusManager::changeSettingState( const std::string &name, const std::string &act, const std::string &value ) {
	set<ExecutionObject*>* settingObjects;
	set<ExecutionObject*>::iterator i;
	vector<FormatterEvent*>* events;
	vector<FormatterEvent*>::iterator j;
	PropertyAnchor* property;
	string pName;

	settingObjects = converter->getSettingNodeObjects();

	LDEBUG("FormatterFocusManager", "changeSettingState: number of settings objects: '%i'", settingObjects->size());

	i = settingObjects->begin();
	while (i != settingObjects->end()) {
		events = (*i)->getEvents();
		if (events != NULL) {
			j = events->begin();
			while (j != events->end()) {
				if ((*j)->instanceOf( "AttributionEvent" )) {
					property = ((AttributionEvent*) (*j))->getAnchor();
					pName = property->getPropertyName();
					if (pName == name) {
						if (act == "start") {
							(*j)->start();

						} else if (act == "stop") {
							((AttributionEvent*) (*j))->setValue( value );
							(*j)->stop();
						}
					}
				}
				++j;
			}
			delete events;
		}
		++i;
	}
	delete settingObjects;
}

bool FormatterFocusManager::userEventReceived( ::util::key::type key, bool isUp ) {
	bool result = true;

	if (!isHandler) {
		if (!impl::_onKey.empty()) {
			impl::_onKey( key );
		}

		return true;
	}

	if (isUp) {
		if (!impl::_onKey.empty()) {
			impl::_onKey( key );
		}

		return false;
	}

	if (selectedObject) {
		if (key == ::util::key::backspace || key == ::util::key::back) {
			playerManager->setPlayerKeyHandler( selectedObject );
			result = keyCodeBack();
		}
	} else {
		if (!currentFocus) {
			LWARN("FormatterFocusManager", "userEventReceived: object == NULL");
			focusSmallest();
			return true;
		}

		CascadingDescriptor* currentDescriptor = currentFocus->getDescriptor();
		if (key == ::util::key::enter || key == ::util::key::ok) {
			if (currentDescriptor->getFormatterRegion()->setSelection( true )) {
				keyCodeOk( currentFocus );
				playerManager->setFormatterKeyHandler( selectedObject );
			}
			result = false;
		} else {
			string nextIndex = "";
			if (key == ::util::key::cursor_up) {
				nextIndex = currentDescriptor->getMoveUp();
			} else if (key == ::util::key::cursor_down) {
				nextIndex = currentDescriptor->getMoveDown();
			} else if (key == ::util::key::cursor_left) {
				nextIndex = currentDescriptor->getMoveLeft();
			} else if (key == ::util::key::cursor_right) {
				nextIndex = currentDescriptor->getMoveRight();
			}

			if (nextIndex != "" && nextIndex != "nill") {
				changeSettingState( "service.currentFocus", "start", nextIndex );
				setFocus( nextIndex );
				changeSettingState( "service.currentFocus", "stop", nextIndex );
			}
		}
	}

	if (!impl::_onKey.empty()) {
		impl::_onKey( key );
	}

	return result;
}

void FormatterFocusManager::onKeyPress( const FormatterFocusManager::KeyPressCallback &callback ) {
	impl::_onKey = callback;
}

void FormatterFocusManager::setStartComposition( bool isComposition ) {
	isStartComposition = isComposition;
	if (!isComposition && !currentFocus) {
		focusSmallest();
	}
}

void FormatterFocusManager::focusSmallest() {
	vector<ExecutionObject*>::iterator j;
	string ix, focusIndex;
	for (j = _objects.begin(); j != _objects.end(); ++j) {
		if ((*j)->getDescriptor()->getFormatterRegion()->isVisible() || (*j)->isTransitionInVisible()) {
			focusIndex = (*j)->getDescriptor()->getFocusIndex();
			if (!focusIndex.empty() && (ix.empty() || focusIndexCmp(focusIndex.c_str(), ix.c_str()) < 0)) {
				ix = focusIndex;
			}
		}
	}
	if (!ix.empty()) {
		setFocus(ix);
	}
}

}
}
}
}
}
}
