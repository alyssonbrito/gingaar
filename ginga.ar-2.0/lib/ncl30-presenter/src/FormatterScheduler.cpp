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

#include "../include/emconverter/FormatterConverter.h"
#include "../include/FormatterScheduler.h"
#include "../include/FormatterMediator.h"
#include "presentationenginemanager.h"
#include <gingaplayer/player/settings.h>
#include <ncl30/animation/Animation.h>
#include <util/log.h>

using namespace ::br::pucrio::telemidia::ginga::ncl::emconverter;
using namespace ::br::pucrio::telemidia::ncl::animation;
namespace bptgn = br::pucrio::telemidia::ginga::ncl;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
FormatterScheduler::FormatterScheduler( PlayerAdapterManager* playerManager, RuleAdapter* ruleAdapter, void* compiler, FormatterMediator* formatterMediator, player::System *system ) {

	this->playerManager = playerManager;
	this->ruleAdapter = ruleAdapter;
	this->compiler = compiler;
	_formatterMediator = formatterMediator;
	this->focusManager = new FormatterFocusManager( this->playerManager, (FormatterConverter*) compiler, system );

	this->documentEvents = new vector<FormatterEvent*>;
	this->documentStatus = new map<FormatterEvent*, bool>;
	_sys = system;
}

FormatterScheduler::~FormatterScheduler() {
	playerManager = NULL;
	ruleAdapter = NULL;
	_formatterMediator = NULL;

	if (focusManager != NULL) {
		delete focusManager;
		focusManager = NULL;
	}

	compiler = NULL;

	if (documentEvents != NULL) {
		delete documentEvents;
		documentEvents = NULL;
	}

	if (documentStatus != NULL) {
		delete documentStatus;
		documentStatus = NULL;
	}
}

void FormatterScheduler::stopListening( void* event ) {
	vector<FormatterEvent*>::iterator i;

	if (documentEvents != NULL) {
		i = documentEvents->begin();
		while (i != documentEvents->end()) {
			if (*i == event) {
				documentEvents->erase( i );
				return;
			}
			++i;
		}
	}
}

FormatterFocusManager* FormatterScheduler::getFocusManager() {
	return focusManager;
}

bool FormatterScheduler::setKeyHandler( bool isHandler ) {
	return focusManager->setKeyHandler( isHandler );
}

bool FormatterScheduler::isDocumentRunning( FormatterEvent* event ) {
	ExecutionObject* executionObject;
	CompositeExecutionObject* parentObject;
	FormatterEvent* documentEvent;

	/*NodeEntity* dataObject;
	 NodeNesting* referPerspective;
	 set<ReferNode*>* sameInstances = NULL;
	 set<ReferNode*>::iterator i;*/

	executionObject = (ExecutionObject*) (event->getExecutionObject());
	/*dataObject = (NodeEntity*)(executionObject->getDataObject());

	 if (dataObject != NULL && dataObject->instanceOf("NodeEntity")) {
	 sameInstances = dataObject->getSameInstances();
	 if (sameInstances != NULL && !sameInstances->empty()) {
	 i = sameInstances->begin();
	 while (i != sameInstances->end()) {
	 referPerspective = new NodeNesting((*i)->getPerspective());
	 ++i;
	 }
	 }
	 }*/

	parentObject = (CompositeExecutionObject*) (executionObject->getParentObject());

	if (parentObject != NULL) {
		while (parentObject->getParentObject() != NULL) {
			executionObject = (ExecutionObject*) (parentObject);
			parentObject = (CompositeExecutionObject*) (parentObject->getParentObject());
		}

		documentEvent = executionObject->getWholeContentPresentationEvent();

	} else {
		documentEvent = event;
	}

	if (documentStatus->count( documentEvent ) != 0) {
		return (*documentStatus)[documentEvent];
	}

	return false;
}

void FormatterScheduler::setTimeBaseObject( ExecutionObject* object, string nodeId ) {

	ExecutionObject* documentObject;
	ExecutionObject* parentObject;

	Node* documentNode;
	Node* compositeNode;
	Node* timeBaseNode;
	NodeNesting* perspective;
	NodeNesting* compositePerspective;

	if (nodeId.find_last_of( '#' ) != std::string::npos) {
		return;
	}

	documentObject = object;
	parentObject = (ExecutionObject*) (documentObject->getParentObject());
	if (parentObject != NULL) {
		while (parentObject->getParentObject() != NULL) {
			documentObject = parentObject;
			if (documentObject->getDataObject()->instanceOf( "ReferNode" )) {
				break;
			}
			parentObject = (ExecutionObject*) (documentObject->getParentObject());
		}
	}

	if (documentObject == NULL || documentObject->getDataObject() == NULL) {
		return;
	}

	documentNode = documentObject->getDataObject();
	if (documentNode->instanceOf( "ReferNode" )) {
		compositeNode = (NodeEntity*) ((ReferNode*) documentNode)->getReferredEntity();
	} else {
		compositeNode = documentNode;
	}

	if (compositeNode == NULL || !(compositeNode->instanceOf( "CompositeNode" ))) {

		return;
	}

	timeBaseNode = ((CompositeNode*) compositeNode)->recursivelyGetNode( nodeId );

	if (timeBaseNode == NULL || !(timeBaseNode->instanceOf( "ContentNode" ))) {

		return;
	}

	perspective = new NodeNesting( timeBaseNode->getPerspective() );
	if (documentNode->instanceOf( "ReferNode" )) {
		perspective->removeHeadNode();
		compositePerspective = new NodeNesting( documentNode->getPerspective() );

		compositePerspective->append( perspective );
		perspective = compositePerspective;
	}
	/*
	 try {
	 timeBaseObject = ((FormatterConverter*) compiler)->getExecutionObject( perspective, NULL, ((FormatterConverter*) compiler)->getDepthLevel() );

	 if (timeBaseObject != NULL) {
	 timeBasePlayer = playerManager->getPlayer( timeBaseObject );
	 if (timeBasePlayer != NULL) {
	 //objectPlayer->setTimeBasePlayer(timeBasePlayer);
	 }
	 }

	 } catch (ObjectCreationForbiddenException* exc) {
	 return;
	 }*/
}

void FormatterScheduler::runAction( void* someAction ) {
  APRINT_NCLP("<==>");
	runAction( ((LinkSimpleAction*) someAction)->getEvent(), someAction );
}

void FormatterScheduler::runAction( FormatterEvent* event, void* someAction ) {
  APRINT_NCLP("<==>");

	LinkSimpleAction* action;
	action = (LinkSimpleAction*) someAction;

	ExecutionObject* executionObject;
	CascadingDescriptor* descriptor;
	NodeEntity* dataObject;
	short actionType;
	string attName;
	string attValue;

	if (event == NULL) {
		LWARN("FormatterScheduler", "Trying to run a NULL event");
		return;
	}

	executionObject = (ExecutionObject*) (event->getExecutionObject());

	LDEBUG("FormatterScheduler", "runAction: event '%s' for '%s'", event->getId().c_str(), executionObject->getId().c_str());

	if (isDocumentRunning( event ) && !executionObject->isCompiled()) {
		((FormatterConverter*) compiler)->compileExecutionObjectLinks( executionObject, ((FormatterConverter*) compiler)->getDepthLevel() );
	}

	dataObject = (NodeEntity*) (executionObject->getDataObject()->getDataEntity());

	if (dataObject->instanceOf( "NodeEntity" )) {
		set<ReferNode*>* gradSame;
		set<ReferNode*>::iterator i;
		gradSame = ((NodeEntity*) dataObject)->getGradSameInstances();
		if (gradSame != NULL) {
			((FormatterConverter*) compiler)->checkGradSameInstance( gradSame, executionObject );
			LDEBUG("FormatterScheduler", "runAction: refer='%s' perspective = '%s'", dataObject->getId().c_str(),executionObject->getNodePerspective()->getHeadNode()->getId().c_str());
		}
	}

	if (executionObject->instanceOf( "ExecutionObjectSwitch" )){
		if (event->instanceOf( "SwitchEvent" )) {
			runActionOverSwitch( (ExecutionObjectSwitch*) executionObject, (SwitchEvent*) event, action );
		}else{
			if (action->getType() == SimpleAction::ACT_STOP || action->getType() == SimpleAction::ACT_ABORT) {
				((ExecutionObjectSwitch*) executionObject)->select( NULL );
			}
		}
		//reverting the code because of the App  tic tac toe ("Jogo da Velha" TV Justiça)
		runActionOverComposition( (CompositeExecutionObject*) executionObject, action );

	} else if (executionObject->instanceOf( "CompositeExecutionObject" ) && (executionObject->getDescriptor() == NULL || executionObject->getDescriptor()->getPlayerName() == "")) {
		runActionOverComposition( (CompositeExecutionObject*) executionObject, action );

	} else if (event->instanceOf( "AttributionEvent" )) {
		runActionOverProperty( event, action );

	} else {
		actionType = action->getType();

		if (executionObject->instanceOf( "ApplicationExecutionObject" ) && !event->instanceOf( "AttributionEvent" )) {

			runActionOverApplicationObject( (ApplicationExecutionObject*) executionObject, event, action );

			LDEBUG("FormatterScheduler", "run action over ApplicationExecutionObject");
			return;
		}
		bool playerprepared = true;
		switch ( actionType ) {
			case SimpleAction::ACT_START:
				//TODO: if (isDocumentRunning(event)) {
				if (!playerManager->hasPrepared( executionObject )) {

					LDEBUG("FormatterScheduler", "runAction: event '%s' for '%s' START", event->getId().c_str(), executionObject->getId().c_str());

					ruleAdapter->adaptDescriptor( executionObject );
					descriptor = executionObject->getDescriptor();
					if (descriptor != NULL) {
						descriptor->setFormatterLayout();
					}

					//if (playerManager->prepare( executionObject, event )) {
					playerManager->prepare( executionObject, event );
					
					if (executionObject != NULL && executionObject->getDescriptor() != NULL) {

						// look for a reference time base player
						attValue = executionObject->getDescriptor()->getParameterValue( "x-timeBaseObject" );

						if (attValue != "") {
							setTimeBaseObject( executionObject, attValue );
						}
					}

					event->addEventListener( this );
					playerManager->start( executionObject );

					//} else {
					//	LWARN("FormatterScheduler", "could not prepare player");
					//	playerprepared = false;
					//}
				} else {
					playerManager->start( executionObject );
				}
				break;

			case SimpleAction::ACT_PAUSE:
				playerManager->pause( executionObject );
				break;
			case SimpleAction::ACT_RESUME:
				playerManager->resume( executionObject );
				break;
			case SimpleAction::ACT_ABORT:
				playerManager->abort( executionObject );
				break;
			case SimpleAction::ACT_STOP:
				playerManager->stop( executionObject );
				break;
		}
		if(playerprepared) {
			((bptgn::PresentationEngineManager*)(_sys->getPem()))->notifyNCLPlayerEvent( actionType, executionObject->getId() );
		}
	}
}

void FormatterScheduler::runActionOverProperty( FormatterEvent* event, LinkSimpleAction* action ) {
	APRINT_NCLP("==>");

	short actionType;
	string propName, propValue;

	NodeEntity* dataObject;
	ExecutionObject* executionObject;
	Animation* anim;

	executionObject = (ExecutionObject*) (event->getExecutionObject());
	dataObject = (NodeEntity*) (executionObject->getDataObject()->getDataEntity());

	if (dataObject->instanceOf( "ContentNode" ) && ((ContentNode*) dataObject)->isSettingNode() && action->instanceOf( "LinkAssignmentAction" )) {

		propName = ((AttributionEvent*) event)->getAnchor()->getPropertyName();

		propValue = ((LinkAssignmentAction*) action)->getValue();
		if (propValue != "" && propValue.substr( 0, 1 ) == "$") {
			try {
				propValue = solveImplicitRefAssessment( propValue, (LinkAssignmentAction*) action, (AttributionEvent*) event );
			} catch (const std::runtime_error &e) {
				LWARN("FormatterScheduler", "Catch exception: %s", e.what());
				APRINT_NCLP("<==");
				return;
			}
		}

		event->start();
		((AttributionEvent*) event)->setValue( propValue );

		((bptgn::PresentationEngineManager*)(_sys->getPem()))->notifyNCLPlayerEvent( actionType, executionObject->getId() );

		LDEBUG("FormatterScheduler", "set value, name=%s, value='%s'", propName.c_str(), propValue.c_str() );
		if (propName == "service.currentFocus") {
			focusManager->setFocus( propValue );
		} else if (propName == "service.currentKeyMaster") {
			focusManager->setKeyMaster( propValue );
		} else {
			_sys->settings()->setProperty( propName, propValue );
		}
		event->stop();

	} else {

		anim = ((LinkAssignmentAction*) action)->getAnimation();
		actionType = action->getType();

		switch (actionType) {
			case SimpleAction::ACT_SET:{
				LDEBUG("FormatterScheduler", "runActionOverProperty: over '%s' for '%s'", event->getId().c_str(), executionObject->getId().c_str());

				if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
					LDEBUG("FormatterScheduler", "runActionOverProperty: trying to set an event that is not sleeping: '%s'", event->getId().c_str());
					APRINT_NCLP("<== WARN");
					return;
				}

				propValue = ((LinkAssignmentAction*) action)->getValue();
				if (propValue != "" && propValue.substr( 0, 1 ) == "$") {
					propValue = solveImplicitRefAssessment( propValue, (LinkAssignmentAction*) action, (AttributionEvent*) event );
				}

				event->start();
				((AttributionEvent*) event)->setValue( propValue );
				APRINT_NCLP("----");
				if (anim != NULL) {
					APRINT_NCLP("--> startAnimation");
					if (!playerManager->startAnimation( executionObject, (AttributionEvent*) event, propValue, anim )) {
						APRINT_NCLP("<-->eventStop");
						event->stop();
					}
					APRINT_NCLP("<-- startAnimation");
				} else if (playerManager->hasPrepared( executionObject )) {
					playerManager->setProperty( executionObject, event, propValue );
					event->stop();
				} else {
					executionObject->setPropertyValue( (AttributionEvent*) event, propValue );
					event->stop();
				}
				APRINT_NCLP("---");
				((bptgn::PresentationEngineManager*)(_sys->getPem()))->notifyNCLPlayerEvent( actionType, executionObject->getId() );
			}break;

			default:{
				LDEBUG("FormatterScheduler", "runActionOverProperty: actionType='%hd'", actionType);
			}
		}
	}
  APRINT_NCLP("<==");
}

void FormatterScheduler::runActionOverApplicationObject( ApplicationExecutionObject* executionObject, FormatterEvent* event, LinkSimpleAction* action ) {

	CascadingDescriptor* descriptor;

	string attValue, attName;
	int actionType = action->getType();

	switch ( actionType ) {
		case SimpleAction::ACT_START:
			//TODO: if (isDocumentRunning(event)) {
			if (!playerManager->hasPrepared( executionObject )) {
				if (ruleAdapter->adaptDescriptor( executionObject )) {

					descriptor = executionObject->getDescriptor();
					if (descriptor != NULL) {
						descriptor->setFormatterLayout();
					}
				}

				playerManager->prepare( executionObject, event );

				if (executionObject->getDescriptor() != NULL) {
					// look for a reference time base player
					attValue = executionObject->getDescriptor()->getParameterValue( "x-timeBaseObject" );

					if (attValue != "") {
						setTimeBaseObject( executionObject, attValue );
					}
				}

			} else {
				playerManager->prepare( executionObject, event );
			}

			event->addEventListener( this );
			if (playerManager->setCurrentEvent( executionObject, event )) {
				playerManager->start( executionObject );
			}
			break;
		case SimpleAction::ACT_PAUSE:
			playerManager->setCurrentEvent( executionObject, event );
			playerManager->pause( executionObject );
			break;
		case SimpleAction::ACT_RESUME:
			playerManager->setCurrentEvent( executionObject, event );
			playerManager->resume( executionObject );
			break;
		case SimpleAction::ACT_ABORT:
			playerManager->setCurrentEvent( executionObject, event );
			playerManager->abort( executionObject );
			break;
		case SimpleAction::ACT_STOP:
			playerManager->setCurrentEvent( executionObject, event );
			playerManager->stop( executionObject );
			break;
	}
}

void FormatterScheduler::runActionOverComposition( CompositeExecutionObject* compositeObject, LinkSimpleAction* action ) {
  APRINT_NCLP("<==>");

	CompositeNode* compositeNode;
	Port* port;
	NodeNesting* compositionPerspective;
	NodeNesting* perspective;

	map<string, ExecutionObject*>* objects;
	map<string, ExecutionObject*>::iterator j;
	ExecutionObject* childObject;

	AttributionEvent* attrEvent;
	FormatterEvent* event;
	string propName;
	string propValue;

	vector<Node*>* nestedSeq;

	FormatterEvent* childEvent;
	int i, size;
	vector<FormatterEvent*>* events;

	LDEBUG("FormatterScheduler", "Run action %hd over COMPOSITION %s", action->getType(), compositeObject->getId().c_str());

	if (action->getType() == SimpleAction::ACT_SET) {
		event = action->getEvent();
		if (!event->instanceOf( "AttributionEvent" )) {
			LWARN("FormatterScheduler", "runActionOverComposition: event ins't of attribution type");
			return;
		}

		attrEvent = (AttributionEvent*) event;
		propName = attrEvent->getAnchor()->getPropertyName();
		propValue = ((LinkAssignmentAction*) action)->getValue();
		event = compositeObject->getEventFromAnchorId( propName );

		LDEBUG("FormatterScheduler", "Run SET action over COMPOSITION '%s' event '%s' value '%s'", compositeObject->getId().c_str(), propName.c_str(),propValue.c_str());

		if (event != NULL) {
			event->start();
			compositeObject->setPropertyValue( (AttributionEvent*) event, propValue );

			event->stop();

		} else {
			compositeObject->setPropertyValue( attrEvent, propValue );
			attrEvent->stop();
		}

		objects = compositeObject->getExecutionObjects();
		if (objects == NULL) {
			LDEBUG("FormatterScheduler", "runActionOverComposition: SET no childs found!");
			return;
		}

		j = objects->begin();
		while (j != objects->end()) {
			childObject = j->second;
			if (childObject->instanceOf( "CompositeExecutionObject" )) {
				runActionOverComposition( (CompositeExecutionObject*) childObject, action );
			} else {
				childEvent = childObject->getEventFromAnchorId( propName );
				if (childEvent != NULL) { //attribution with transition
					runAction( childEvent, action );

				} else { //force attribution
					playerManager->setProperty( childObject, attrEvent, propValue );
				}
			}
			++j;
		}

	} else if (action->getType() == SimpleAction::ACT_START) {
		compositeNode = (CompositeNode*) (compositeObject->getDataObject()->getDataEntity());

		if (compositeNode->instanceOf( "ReferNode" ))
		{
			bptn::Entity *referredNode;
			referredNode = ((bptnr::ReferNode*) compositeNode)->getReferredEntity();

			compositeNode = (SwitchNode*) (referredNode->getDataEntity());
		}	
		size = compositeNode->getNumPorts();
		compositionPerspective = compositeObject->getNodePerspective();
		events = new vector<FormatterEvent*>;
		for (i = 0; i < size; i++) {
			port = compositeNode->getPort( i );
			perspective = compositionPerspective->copy();
			if(port->getInterfacePoint() != NULL)
			{
				nestedSeq = port->getMapNodeNesting();
				perspective->append( nestedSeq );
				delete nestedSeq;
			}
			childObject = ((FormatterConverter*) compiler)->getExecutionObject( perspective, NULL, ((FormatterConverter*) compiler)->getDepthLevel() );

			if (childObject != NULL && port->getEndInterfacePoint() != NULL && port->getEndInterfacePoint()->instanceOf( "ContentAnchor" )) {

				childEvent = (PresentationEvent*) (((FormatterConverter*) compiler)->getEvent( childObject, port->getEndInterfacePoint(), EventUtil::EVT_PRESENTATION, "" ));

				if (childEvent != NULL) {
					events->push_back( childEvent );
				}
			}
		}
		focusManager->setStartComposition( true );
		size = events->size();
		for (i = 0; i < size; i++) {
			runAction( (*events)[i], action );
		}
		delete events;
		events = NULL;
		focusManager->setStartComposition( false );

	} else {
		events = new vector<FormatterEvent*>;

		objects = compositeObject->getExecutionObjects();
		if (objects != NULL) {
			j = objects->begin();
			while (j != objects->end()) {
				childObject = j->second;
				childEvent = childObject->getMainEvent();
				if (childEvent == NULL) {
					childEvent = childObject->getWholeContentPresentationEvent();
				}

				if (childEvent != NULL) {
					events->push_back( childEvent );
				}
				++j;
			}
			delete objects;
			objects = NULL;
		}

		size = events->size();
		for (i = 0; i < size; i++) {
			runAction( (*events)[i], action );
		}

		delete events;
		events = NULL;
	}
}

void FormatterScheduler::runActionOverSwitch( ExecutionObjectSwitch* switchObject, SwitchEvent* event, LinkSimpleAction* action ) {
  APRINT_NCLP("<==>");

	ExecutionObject* selectedObject;
	FormatterEvent* selectedEvent;

	selectedObject = switchObject->getSelectedObject();
	if (selectedObject == NULL) {
		selectedObject = ((FormatterConverter*) compiler)->processExecutionObjectSwitch( switchObject );

		if (selectedObject == NULL) {
			return;
		}
	}

	selectedEvent = event->getMappedEvent();
	if (selectedEvent != NULL) {
		runAction( selectedEvent, action );

	} else {
		runSwitchEvent( switchObject, event, selectedObject, action );
	}

	if (action->getType() == SimpleAction::ACT_STOP || action->getType() == SimpleAction::ACT_ABORT) {

		switchObject->select( NULL );
	}
}

void FormatterScheduler::runSwitchEvent( ExecutionObjectSwitch* switchObject, SwitchEvent* switchEvent, ExecutionObject* selectedObject, LinkSimpleAction* action ) {
  APRINT_NCLP("<==>");

	FormatterEvent* selectedEvent;
	SwitchPort* switchPort;
	vector<Port*>* mappings;
	vector<Port*>::iterator i;
	Port* mapping;
	NodeNesting* nodePerspective;
	vector<Node*>* nestedSeq;
	ExecutionObject* endPointObject;

	selectedEvent = NULL;
	switchPort = (SwitchPort*) (switchEvent->getInterfacePoint());
	mappings = switchPort->getPorts();
	if (mappings != NULL) {
		i = mappings->begin();
		while (i != mappings->end()) {
			mapping = *i;
			if (mapping->getNode() == selectedObject->getDataObject()) {
				nodePerspective = switchObject->getNodePerspective();
				nestedSeq = mapping->getMapNodeNesting();
				nodePerspective->append( nestedSeq );
				delete nestedSeq;
				endPointObject = ((FormatterConverter*) compiler)->getExecutionObject( nodePerspective, NULL, ((FormatterConverter*) compiler)->getDepthLevel() );

				if (endPointObject != NULL) {
					selectedEvent = ((FormatterConverter*) compiler)->getEvent( endPointObject, mapping->getEndInterfacePoint(), switchEvent->getEventType(), switchEvent->getKey() );
				}

				break;
			}
			++i;
		}
	}

	if (selectedEvent != NULL) {
		switchEvent->setMappedEvent( selectedEvent );
		runAction( selectedEvent, action );
	}
}

string FormatterScheduler::solveImplicitRefAssessment( string propValue, LinkAssignmentAction* action, AttributionEvent* event ) {
	string roleId = action->getValue();
	if (roleId != "") {
		roleId = roleId.substr( 1, roleId.length() );
	}

	FormatterEvent* refEvent = ((AttributionEvent*) event)->getImplicitRefAssessmentEvent( roleId );

	if (refEvent != NULL) {
		propValue = ((AttributionEvent*) refEvent)->getCurrentValue();

		//ExecutionObject* refObject = ((ExecutionObject*)(refEvent->getExecutionObject()));
		LDEBUG("FormatterScheduler", "solveImplicitRefAssessment: %s = %s", refEvent->getId().c_str(), propValue.c_str() );
		return propValue;
	} else {
		LWARN("FormatterScheduler", "solveImplicitRefAssessment: refEvent not found for '%s', LinkAssignmentAction value is '%s'", event->getId().c_str(), action->getValue().c_str() );
	}
	return "";
}

void FormatterScheduler::startEvent( FormatterEvent* event ) {
  APRINT_NCLP("<==>");
	LinkSimpleAction* fakeAction;

	fakeAction = new LinkSimpleAction( event, SimpleAction::ACT_START, _sys );
	runAction( event, fakeAction );
	delete fakeAction;
}

void FormatterScheduler::stopEvent( FormatterEvent* event ) {
  APRINT_NCLP("<==>");
	LinkSimpleAction* fakeAction;

	fakeAction = new LinkSimpleAction( event, SimpleAction::ACT_STOP, _sys );
	runAction( event, fakeAction );
	delete fakeAction;
}

void FormatterScheduler::pauseEvent( FormatterEvent* event ) {
  APRINT_NCLP("<==>");
	LinkSimpleAction* fakeAction;

	fakeAction = new LinkSimpleAction( event, SimpleAction::ACT_PAUSE, _sys );
	runAction( event, fakeAction );
	delete fakeAction;
}

void FormatterScheduler::resumeEvent( FormatterEvent* event ) {
  APRINT_NCLP("<==>");
	LinkSimpleAction* fakeAction;

	fakeAction = new LinkSimpleAction( event, SimpleAction::ACT_RESUME, _sys );
	runAction( event, fakeAction );
	delete fakeAction;
}

void FormatterScheduler::initializeDocumentSettings( Node* node ) {
	vector<Anchor*>* anchors;
	vector<Anchor*>::iterator i;
	vector<Node*>* nodes;
	vector<Node*>::iterator j;
	Anchor* anchor;

	if (node->instanceOf( "ContentNode" )) {
		if (((ContentNode*) node)->isSettingNode()) {
			NodeNesting* perspective = new NodeNesting( node->getPerspective() );
			FormatterConverter* converter = static_cast<FormatterConverter*>(compiler);
			ExecutionObject* obj = converter->getExecutionObject( perspective, NULL, converter->getDepthLevel() );
			if (obj) {
				converter->compileExecutionObjectLinks( obj, converter->getDepthLevel() );
			}
			anchors = ((ContentNode*) node)->getAnchors();
			if (anchors != NULL) {
				i = anchors->begin();
				while (i != anchors->end()) {
					anchor = (*i);
					if (anchor->instanceOf( "PropertyAnchor" )) {
						PropertyAnchor* attributeAnchor = (PropertyAnchor*) anchor;
						_sys->settings()->setProperty( attributeAnchor->getPropertyName(), attributeAnchor->getPropertyValue() );
						informPlatform( attributeAnchor->getPropertyName(), attributeAnchor->getPropertyValue() );
					}
					++i;
				}
			}
		}

	} else if (node->instanceOf( "CompositeNode" )) {
		nodes = ((CompositeNode*) node)->getNodes();
		if (nodes != NULL) {
			j = nodes->begin();
			while (j != nodes->end()) {
				initializeDocumentSettings( *j );
				++j;
			}
		}

	} else if (node->instanceOf( "ReferNode" )) {
		initializeDocumentSettings( (NodeEntity*) ((ReferNode*) node)->getDataEntity() );
	}
}

void FormatterScheduler::startDocument( FormatterEvent* documentEvent, vector<FormatterEvent*>* entryEvents ) {
  APRINT_NCLP("<==>");

	if (documentEvent == NULL || entryEvents == NULL) {
		LWARN("FormatterScheduler", "startDocument: documentEvent == NULL || entryEvents == NULL");
		return;
	}

	if (entryEvents->empty()) {
		LWARN("FormatterScheduler", "startDocument: entryEvents is empty");
		return;
	}

	vector<FormatterEvent*>::iterator it;
	for (it = documentEvents->begin(); it != documentEvents->end(); ++it) {
		if (*it == documentEvent) {
			return;
		}
	}

	documentEvent->addEventListener( this );
	documentEvents->push_back( documentEvent );
	(*documentStatus)[documentEvent] = true;

	initializeDocumentSettings( ((ExecutionObject*) (documentEvent->getExecutionObject()))->getDataObject() );

	focusManager->registerNavigationKeys();

	int i, size;
	FormatterEvent* event;

	focusManager->setStartComposition( true );
	size = entryEvents->size();
	for (i = 0; i < size; i++) {
		event = (*entryEvents)[i];
    APRINT_NCLP("runEvent [%s]",event->getId().c_str());
		startEvent( event );
	}
	focusManager->setStartComposition( false );
  APRINT_NCLP("<==");
}

void FormatterScheduler::removeDocument( FormatterEvent* documentEvent ) {
	ExecutionObject* obj;
	vector<FormatterEvent*>::iterator i;

	//TODO: do a better way to remove documents (see lockComposite)
	obj = (ExecutionObject*) (documentEvent->getExecutionObject());
	if (compiler != NULL) {
		((FormatterConverter*) compiler)->removeExecutionObject( obj );
	}

	if (documentEvents != NULL) {
		for (i = documentEvents->begin(); i != documentEvents->end(); ++i) {

			if (*i == documentEvent) {
				documentEvents->erase( i );
				break;
			}
		}
	}

	if (documentStatus != NULL && documentStatus->count( documentEvent ) != 0) {

		documentStatus->erase( documentStatus->find( documentEvent ) );
	}
}

void FormatterScheduler::stopDocument( FormatterEvent* documentEvent ) {
	if (documentStatus->count( documentEvent ) != 0) {
		ExecutionObject* executionObject;

		documentEvent->delEventListener( this );
		(*documentStatus)[documentEvent] = false;

		executionObject = (ExecutionObject*) (documentEvent->getExecutionObject());

		if (executionObject->instanceOf( "CompositeExecutionObject" )) {
			((CompositeExecutionObject*) executionObject)->setAllLinksAsUncompiled( true );
		}

		stopEvent( documentEvent );

		//Can't remove this document, I've been deleted...
		//removeDocument(documentEvent);
	}
}

void FormatterScheduler::pauseDocument( FormatterEvent* documentEvent ) {
	vector<FormatterEvent*>::iterator i;
	for (i = documentEvents->begin(); i != documentEvents->end(); ++i) {
		if (*i == documentEvent) {
			(*documentStatus)[documentEvent] = false;
			pauseEvent( documentEvent );
			break;
		}
	}
}

void FormatterScheduler::resumeDocument( FormatterEvent* documentEvent ) {
	bool contains;
	contains = false;
	vector<FormatterEvent*>::iterator i;
	for (i = documentEvents->begin(); i != documentEvents->end(); ++i) {
		if (*i == documentEvent) {
			contains = true;
			break;
		}
	}

	if (contains) {
		resumeEvent( documentEvent );
		(*documentStatus)[documentEvent] = true;
	}
}

void FormatterScheduler::stopAllDocuments() {
	int i, size;
	vector<FormatterEvent*>* auxDocEventList;
	FormatterEvent* documentEvent;

	if (!documentEvents->empty()) {
		auxDocEventList = new vector<FormatterEvent*>( *documentEvents );

		size = auxDocEventList->size();
		for (i = 0; i < size; i++) {
			documentEvent = (*auxDocEventList)[i];
			stopDocument( documentEvent );
		}

		auxDocEventList->clear();
		delete auxDocEventList;
		auxDocEventList = NULL;
	}
}

void FormatterScheduler::pauseAllDocuments() {
	int i, size;
	FormatterEvent* documentEvent;

	if (!documentEvents->empty()) {
		size = documentEvents->size();
		for (i = 0; i < size; i++) {
			documentEvent = (*documentEvents)[i];
			pauseDocument( documentEvent );
		}
	}
}

void FormatterScheduler::resumeAllDocuments() {
	int i, size;
	FormatterEvent* documentEvent;

	if (!documentEvents->empty()) {
		size = documentEvents->size();
		for (i = 0; i < size; i++) {
			documentEvent = (*documentEvents)[i];
			resumeDocument( documentEvent );
		}
	}
}

void FormatterScheduler::eventStateChanged( void* someEvent, short transition, short /*previousState*/) {

	LDEBUG("FormatterScheduler", "eventStateChanged: start, event: %p", someEvent);
	ExecutionObject* object;
	FormatterEvent* event;
	bool contains;

	event = (FormatterEvent*) someEvent;

	LDEBUG("FormatterScheduler", "eventStateChanged: '%s' transition '%hd'", event->getId().c_str(), transition);

	contains = false;
	vector<FormatterEvent*>::iterator it;
	for (it = documentEvents->begin(); it != documentEvents->end(); ++it) {
		if (*it == event) {
			contains = true;
			break;
		}
	}

	if (contains) {
		switch ( transition ) {
			case EventUtil::TR_STOPS:
			case EventUtil::TR_ABORTS:
				_formatterMediator->presentationCompleted( event );
				//removeDocument(event);
				break;
		}

	} else {
		switch ( transition ) {
			case EventUtil::TR_STARTS:
				//TODO: if (isDocumentRunning(event)) {
				object = (ExecutionObject*) (event->getExecutionObject());
				//TODO si es necesario: region((FormatterDevice*)layoutManager)->showObject(	object);
				focusManager->showObject( object );
				break;
			case EventUtil::TR_STOPS:
				if (((PresentationEvent*) event)->getRepetitions() == 0) {
					event->delEventListener( this );
					object = (ExecutionObject*) (event->getExecutionObject());
					focusManager->hideObject( object );
					//playerManager->removePlayer(object);
				}
				break;
			case EventUtil::TR_ABORTS:
				event->delEventListener( this );
				object = (ExecutionObject*) (event->getExecutionObject());
				focusManager->hideObject( object );
				//playerManager->removePlayer(object);
				break;
		}
	}
}

short FormatterScheduler::getPriorityType() {
	return IEventListener::PT_CORE;
}

void FormatterScheduler::informPlatform( const std::string &name, const std::string &value ) {
	LDEBUG("Port", "Check property before informing to the Platform => Name: '%s'; Value: '%s'", name.c_str(), value.c_str());

	if (name == "channel.keyCapture") {
		LDEBUG("Port", "Notifying Platform...");
		_sys->onKeySetUpdate(name, value);
	}

}

}
}
}
}
}
