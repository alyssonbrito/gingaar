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

#include "../../include/adapters/ApplicationPlayerAdapter.h"
#include <gingaplayer/system.h>
#include <gingaplayer/input/input.h>
#include <gingaplayer/player.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <gingaplayer/player/settings.h>
#include "../../include/FormatterMediator.h"
#include <ncl30/util/functions.h>
#include <boost/lexical_cast.hpp>


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace application {

ApplicationPlayerAdapter::ApplicationPlayerAdapter( PlayerAdapterManager* manager, player::System *system ) :
	FormatterPlayerAdapter( manager, system ) {

	getTypeSet().insert( "ApplicationPlayerAdapter" );
	preparedEvents = new map<string, FormatterEvent*> ;
	currentEvent = NULL;
	editingCommandListener = NULL;
}

ApplicationPlayerAdapter::~ApplicationPlayerAdapter() {
	if (preparedEvents != NULL) {
		delete preparedEvents;
		preparedEvents = NULL;
	}
	currentEvent = NULL;
}

void ApplicationPlayerAdapter::setNclEditListener( INclEditListener* listener ) {

	this->editingCommandListener = listener;
}

bool ApplicationPlayerAdapter::hasPrepared() {
	return getObject() != NULL;
}

bool ApplicationPlayerAdapter::prepare( ExecutionObject* object, FormatterEvent* event ) {
	if (object == NULL) {
		return false;
	}

	if (this->getObject() != object) {
		preparedEvents->clear();
		setObject( object );
		createPlayer();
	}

	player::event::Callback fnc = boost::bind( &ApplicationPlayerAdapter::onEvent, this, _1, _2, _3, _4, _5 );
	getPlayer()->setEventCallback( fnc );

	if (event->instanceOf( "PresentationEvent" )) {
		CascadingDescriptor* descriptor = object->getDescriptor();
		if (descriptor != NULL) {
			std::string explicitDur = descriptor->getExplicitDuration();
			if (explicitDur != "nill") {
				explicitDur.erase(std::remove_if(explicitDur.begin(), explicitDur.end(), ncl_util::is_alpha_or_space()), explicitDur.end());
				double duration = 0;
				try{
					duration = boost::lexical_cast<double>( explicitDur ) * 1000;
				} catch (boost::bad_lexical_cast &) {
					APRINT_NCLP("Conversion error[%s]",explicitDur.c_str());
					LWARN("ApplicationPlayerAdapter", "Conversion error[%s]",explicitDur.c_str());
				}
				object->removeEvent( event );
				bptnt::Transition* transition = descriptor->getFormatterRegion()->getTransOut();
				double dur = 0;
				if (transition) {
					dur = std::min(transition->getDur(), duration);
				}
				static_cast<PresentationEvent*>(event)->setEnd( duration - dur );
				object->addEvent( event );
			}
		}
	}

	if (event->instanceOf("AnchorEvent") && static_cast<AnchorEvent*>(event)->getAnchor()->instanceOf("SampleIntervalAnchor")) {
		SampleIntervalAnchor* anchor = static_cast<SampleIntervalAnchor*>(static_cast<AnchorEvent*>(event)->getAnchor());
		std::pair<std::string, int> firstpair = std::make_pair( "_first", static_cast<int>(anchor->getBegin()));
		getPlayer()->setProperty( "attributionEvent", firstpair );
		int last = static_cast<int>(anchor->getEnd());
		std::pair<std::string, int> secondpair = std::make_pair( "_last", last);
		getPlayer()->setProperty( "attributionEvent", secondpair );
	}

	if (event->getCurrentState() == EventUtil::ST_SLEEPING) {
		this->getObject()->prepare( event, 0 );
		prepare( event );
		return true;
	}
	return false;
}

void ApplicationPlayerAdapter::prepare( FormatterEvent* event ) {
	CascadingDescriptor* descriptor;
	LayoutRegion* region;

	if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
		return;
	}

	descriptor = getObject()->getDescriptor();
	if (descriptor != NULL) {
		region = descriptor->getRegion();
		if (region != NULL) {
			//	player->setNotifyContentUpdate(region->getDeviceClass() == 1);
		}
	}

	/*if (event->instanceOf("AnchorEvent")) {
	 if ((((AnchorEvent*)event)->getAnchor())->instanceOf(
	 "LambdaAnchor")) {

	 duration = ((PresentationEvent*)event)->getDuration();

	 if (duration < IntervalAnchor::OBJECT_DURATION) {
	 player->setScope(
	 "",
	 IPlayer::TYPE_PRESENTATION,
	 0.0, duration / 1000);
	 }

	 } else if (((((AnchorEvent*)event)->getAnchor()))->
	 instanceOf("IntervalAnchor")) {

	 intervalAnchor = (IntervalAnchor*)(
	 ((AnchorEvent*)event)->getAnchor());

	 player->setScope(
	 ((AnchorEvent*)event)->getAnchor()->getId(),
	 IPlayer::TYPE_PRESENTATION,
	 (intervalAnchor->getBegin() / 1000),
	 (intervalAnchor->getEnd() / 1000));

	 } else if (((((AnchorEvent*)event)->getAnchor()))->
	 instanceOf("LabeledAnchor")) {

	 player->setScope(((LabeledAnchor*)((AnchorEvent*)event)->
	 getAnchor())->getLabel(), IPlayer::TYPE_PRESENTATION);
	 }
	 }*/

	(*preparedEvents)[event->getId()] = event;
}

bool ApplicationPlayerAdapter::stop() {
	FormatterPlayerAdapter::stop();

	map<string, FormatterEvent*>::iterator i = preparedEvents->begin();
	while (i != preparedEvents->end()) {
		FormatterEvent* event = i->second;
		if (event != NULL && event->instanceOf( "AnchorEvent" ) && ((AnchorEvent*) event)->getAnchor() != NULL && ((AnchorEvent*) event)->getAnchor()->instanceOf( "LambdaAnchor" )) {
			currentEvent = event;
			currentEvent->stop();
		}
		++i;
	}

	if (currentEvent == NULL) {
		return true;
	}

	if (currentEvent->getCurrentState() == EventUtil::ST_OCCURRING || currentEvent->getCurrentState() == EventUtil::ST_PAUSED) {
		currentEvent->stop();
	}

	if (preparedEvents->count( currentEvent->getId() ) != 0 && preparedEvents->size() == 1) {
		preparedEvents->clear();
	} else {
		i = preparedEvents->find( currentEvent->getId() );
		if (i != preparedEvents->end()) {
			preparedEvents->erase( i );
		}
	}
	return true;
}

void ApplicationPlayerAdapter::onEvent( player::event::evtType::type type, player::event::evtAction::type action, const std::string &nodeId, const std::string &parameter/*=""*/, const std::string &value/*=""*/) {
	if (type == player::event::evtType::edit) {
		ExecutionObject* obj = getObject();
		if (nodeId.compare(obj->getDataObject()->getId()) != 0) {
			if (!obj->getParentObject()) {
				return;
			}
			while (obj->getParentObject()) {
				obj = (ExecutionObject*)(obj->getParentObject());
			}
			if (nodeId.compare(obj->getDataObject()->getId()) != 0) {
				obj = ((CompositeExecutionObject*)obj)->getExecutionObjectRecursivelly( nodeId );
			}
			if (!obj) {
				return;
			}
		}

		bool use = false;
		FormatterEvent* event = obj->getEventFromAnchorId( parameter );
		if (event && event->instanceOf( "AttributionEvent" )) {
			use = true;
		}
		if (use) {
			event->start();
		}

		NodeEntity* dataObject = (NodeEntity*) (obj->getDataObject()->getDataEntity());
		if (dataObject->instanceOf("ContentNode") && ((ContentNode*)dataObject)->isSettingNode()) {
			if (parameter== "service.currentFocus") {
				getManager()->getFormatterMediator()->getFormatterScheduler()->getFocusManager()->setFocus( value );
			} else if (parameter == "service.currentKeyMaster") {
				getManager()->getFormatterMediator()->getFormatterScheduler()->getFocusManager()->setKeyMaster( value );
			} else if (parameter == "channel.keyCapture") {
				getSystem()->settings()->setProperty( parameter, value );
				getSystem()->onKeySetUpdate(parameter, value);
			} else {
				getSystem()->settings()->setProperty( parameter, value );
			}
		} else {
			if (getManager()->hasPrepared( obj )) {
				if (use) {
					getManager()->setProperty( obj, event, value );
				}
			} else {
				obj->setProperty( parameter, value );
			}
		}
		if (use) {
			event->stop();
		}
		return;
	}

	if (type == player::event::evtType::attribution && action == player::event::evtAction::start) {
		setProperty( parameter, value );
	}

	FormatterEvent* event = NULL;
	event = getObject()->getEventFromAnchorId( parameter );

	if (parameter.empty() || getObject()->getId() == parameter) {
		if (action == player::event::evtAction::stop) {
			getSystem()->enqueue(boost::bind( &PlayerAdapterManager::stop, getManager(), getObject(), false ) );
		}
		return;
	}

	if (event == NULL) {
		return;
	}

	if (!((type == player::event::evtType::attribution && event->instanceOf( "AttributionEvent" )) || (type == player::event::evtType::presentation && event->instanceOf( "PresentationEvent" )))) {
		return;
	}

	switch ( action ) {
		case player::event::evtAction::start: {
			event->start();
			break;
		}
		case player::event::evtAction::pause: {
			event->pause();
			break;
		}
		case player::event::evtAction::resume: {
			event->resume();
			break;
		}
		case player::event::evtAction::abort: {
			event->abort();
			break;
		}
		case player::event::evtAction::stop: {
			event->stop();
			break;
		}
		default: {
			break;
		}
	};
	/*			case IPlayer::PL_NOTIFY_NCLEDIT:
	 if (editingCommandListener != NULL) {
	 editingCommandListener->nclEdit(param);
	 }
	 */
}

bool ApplicationPlayerAdapter::setCurrentEvent( FormatterEvent* event ) {
	string interfaceId;

	if (event == NULL) {
		return true;
	}

	if (!event->instanceOf( "SelectionEvent" ) && event->instanceOf( "AnchorEvent" )) {

		interfaceId = ((AnchorEvent*) event)->getAnchor()->getId();

		if ((((AnchorEvent*) event)->getAnchor())->instanceOf( "LabeledAnchor" )) {

			interfaceId = ((LabeledAnchor*) ((AnchorEvent*) event)->getAnchor())->getLabel();

		} else if ((((AnchorEvent*) event)->getAnchor())->instanceOf( "LambdaAnchor" )) {

			interfaceId = "";
		}

		currentEvent = event;
		ApplicationExecutionObject* aeo = dynamic_cast<ApplicationExecutionObject*> ( getObject() );
		if (aeo != NULL) {
			aeo->setCurrentEvent( currentEvent );
		} else {
			LWARN("ApplicationPlayerAdapter", "%s %i : Esto no es un ApplicationExecutionObject*",__FILE__,__LINE__);
		}

		if (getPlayer()->isPlaying() && !interfaceId.empty()) {
			getPlayer()->setProperty( "presentationEvent", interfaceId );
			return false;
		}

	} else if (event->instanceOf( "AttributionEvent" )) {
		interfaceId = ((AttributionEvent*) event)->getAnchor()->getPropertyName();

		//TODO player->setScope(interfaceId, IPlayer::TYPE_ATTRIBUTION);

		currentEvent = event;
		((ApplicationExecutionObject*) getObject())->setCurrentEvent( currentEvent );

		//TODO player->setCurrentScope(interfaceId);

	}

	return true;
}

void ApplicationPlayerAdapter::setProperty( const std::string &propertyName, const std::string &propertyValue ) {
	FormatterPlayerAdapter::setProperty( propertyName, propertyValue );
	if(getPlayer()->isPlaying()) {
		std::pair<std::string, std::string> data = std::make_pair( propertyName, propertyValue );
		getPlayer()->setProperty( "attributionEvent", data );
	}
}

}
}
}
}
}
}
}
