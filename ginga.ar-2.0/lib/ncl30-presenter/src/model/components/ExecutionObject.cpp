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
#include "../../../include/model/ExecutionObject.h"
#include "../../../include/model/CompositeExecutionObject.h"
#include <util/log.h>
#include <util/keydefs.h>
#include <ncl30/util/functions.h>
#include <boost/lexical_cast.hpp>

using namespace util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
ExecutionObject::ExecutionObject( string id, Node* node, bool handling ) {
	APRINT_NCLP("==>");fflush(stderr);
	initializeExecutionObject( id, node, NULL, handling );
	//APRINT_NCLP("id [%s] node_p[%p] handling[%d]",id.c_str(),node,handling);
	APRINT_NCLP("<==");fflush(stderr);
}

ExecutionObject::ExecutionObject( string id, Node* node, GenericDescriptor* descriptor, bool handling ) {
	APRINT_NCLP("==>");fflush(stderr);
	initializeExecutionObject( id, node, new CascadingDescriptor( descriptor ), handling );
	//APRINT_NCLP(" id [%s] node_p[%p] descriptor_p[%p] handling[%d]",id.c_str(),node,descriptor,handling);
	APRINT_NCLP("<==");fflush(stderr);
}

ExecutionObject::ExecutionObject( string id, Node* node, CascadingDescriptor* descriptor, bool handling ) {
	APRINT_NCLP("==>");fflush(stderr);
	initializeExecutionObject( id, node, descriptor, handling );
	APRINT_NCLP("id [%s] node[%s] descriptor_p[%p] handling[%d]",
		id.c_str(),node->getId().c_str(),descriptor,handling);
	APRINT_NCLP("<==");fflush(stderr);
}


ExecutionObject::~ExecutionObject() {
	APRINT_NCLP("<==> id [%s] ",getId().c_str());
	map<string, FormatterEvent*>::iterator i;
	map<Node*, Node*>::iterator j;

	Node* parentNode;
	CompositeExecutionObject* parentObject;

	deleting = true;

	dataObject = NULL;
	wholeContent = NULL;
	mainEvent = NULL;

	if (transMan != NULL) {
		delete transMan;
		transMan = NULL;
	}

	if (events != NULL) {
		/* deleted by FormatterConverter
		i = events->begin();
		while (i != events->end()) {
			FormatterEvent *event = i->second;
			if (event != NULL) {
				delete event;
				event = NULL;
			}
			++i;
		}*/

		events->clear();
		delete events;
		events = NULL;
	}

	if (presEvents != NULL) {
		presEvents->clear();
		delete presEvents;
		presEvents = NULL;
	}

	if (selectionEvents != NULL) {
		selectionEvents->clear();
		delete selectionEvents;
		selectionEvents = NULL;
	}

	if (otherEvents != NULL) {
		otherEvents->clear();
		delete otherEvents;
		otherEvents = NULL;
	}

	if (nodeParentTable != NULL) {
		j = nodeParentTable->begin();
		while (j != nodeParentTable->end()) {
			parentNode = j->second;
			if (parentTable->count( parentNode ) != 0) {
				parentObject = (CompositeExecutionObject*) ((*parentTable)[parentNode]);

				parentObject->removeExecutionObject( this );
			}
			++j;
		}

		nodeParentTable->clear();
		delete nodeParentTable;
		nodeParentTable = NULL;
	}

	if (parentTable != NULL) {
		parentTable->clear();
		delete parentTable;
		parentTable = NULL;
	}

	if (descriptor != NULL) {
		delete descriptor;
		descriptor = NULL;
	}
}

void ExecutionObject::initializeExecutionObject( string id, Node* node, CascadingDescriptor* descriptor, bool handling ) {

	if (descriptor!=NULL){
		APRINT_NCLP("descriptor[%s] ",descriptor->getId().c_str());
	}
	APRINT_NCLP("==> id [%s] node[%s] ",id.c_str(),node->getId().c_str());
	typeSet.insert( "ExecutionObject" );

	this->deleting = false;
	this->id = id;
	this->dataObject = node;
	this->wholeContent = NULL;
	this->startTime = ncl_util::infinity();
	this->durationTime = 0;
	this->descriptor = NULL;

	this->nodeParentTable = new map<Node*, Node*> ;
	this->parentTable = new map<Node*, void*> ;
	this->isItCompiled = false;

	this->events = new map<string, FormatterEvent*> ;
	this->presEvents = new vector<PresentationEvent*> ;
	this->selectionEvents = new set<SelectionEvent*> ;
	this->otherEvents = new vector<FormatterEvent*> ;

	this->pauseCount = 0;
	this->mainEvent = NULL;
	this->descriptor = descriptor;
	this->isHandler = false;
	this->isHandling = handling;

	this->transMan = new EventTransitionManager();

	this->transitionInVisible = false;
	APRINT_NCLP("<==");
}

bool ExecutionObject::instanceOf( string s ) {
	return (typeSet.find( s ) != typeSet.end());
}

int ExecutionObject::compareToUsingId( ExecutionObject* object ) {
	return id.compare( object->getId() );
}

Node* ExecutionObject::getDataObject() {
	return dataObject;
}

CascadingDescriptor* ExecutionObject::getDescriptor() {
	return descriptor;
}

string ExecutionObject::getId() {
	return id;
}

void* ExecutionObject::getParentObject() {
	return getParentObject( dataObject );
}

void* ExecutionObject::getParentObject( Node* node ) {
	Node* parentNode;
	void* parentObj = NULL;

	if (nodeParentTable->count( node ) != 0) {
		parentNode = (*nodeParentTable)[node];
		if (parentTable->count( parentNode ) != 0) {
			parentObj = (*parentTable)[parentNode];
		}
	}
	return parentObj;
}

void ExecutionObject::addParentObject( void* parentObject, Node* parentNode ) {

	addParentObject( dataObject, parentObject, parentNode );
}

void ExecutionObject::addParentObject( Node* node, void* parentObject, Node* parentNode ) {

	(*nodeParentTable)[node] = parentNode;
	(*parentTable)[parentNode] = parentObject;
}

void ExecutionObject::setDescriptor( CascadingDescriptor* cascadingDescriptor ) {

	this->descriptor = cascadingDescriptor;
}

void ExecutionObject::setDescriptor( GenericDescriptor* descriptor ) {
	CascadingDescriptor* cascade;
	cascade = new CascadingDescriptor( descriptor );
	this->descriptor = cascade;
}

string ExecutionObject::toString() {
	return id;
}

bool ExecutionObject::addEvent( FormatterEvent* event ) {
	APRINT_NCLP("==>");
	if (events->count( event->getId() ) != 0) {
		APRINT_NCLP("<==");
		return false;
	}

	(*events)[event->getId()] = event;
	if (event->instanceOf( "PresentationEvent" )) {
		APRINT_NCLP("---");
		addPresentationEvent( (PresentationEvent*) event );

	} else if (event->instanceOf( "SelectionEvent" )) {
		APRINT_NCLP("---");
		selectionEvents->insert( ((SelectionEvent*) event) );

	} else {
		APRINT_NCLP("---");
		otherEvents->push_back( event );
	}

	APRINT_NCLP("<==");
	return true;
}

void ExecutionObject::addPresentationEvent( PresentationEvent* event ) {
	PresentationEvent* auxEvent;
	double begin, auxBegin;
	int posBeg, posEnd, posMid;

	if ((event->getAnchor())->instanceOf( "LambdaAnchor" )) {
		presEvents->insert( presEvents->begin(), event );
		wholeContent = (PresentationEvent*) event;

	} else {
		begin = event->getBegin();

		// undefined events are not inserted into transition table
		if (PresentationEvent::isUndefinedInstant( begin )) {
			return;
		}

		if (event->getAnchor()->instanceOf( "RelativeTimeIntervalAnchor" )) {
			posBeg = 0;
			posEnd = presEvents->size() - 1;
			while (posBeg <= posEnd) {
				posMid = (posBeg + posEnd) / 2;
				auxEvent = (PresentationEvent*) ((*presEvents)[posMid]);
				auxBegin = auxEvent->getBegin();
				if (begin < auxBegin) {
					posEnd = posMid - 1;

				} else if (begin > auxBegin) {
					posBeg = posMid + 1;

				} else {
					posBeg = posMid + 1;
					break;
				}
			}

			presEvents->insert( (presEvents->begin() + posBeg), event );
		}
	}

	transMan->addPresentationEvent( event );
}

int ExecutionObject::compareTo( ExecutionObject* object ) {
	int ret;

	ret = compareToUsingStartTime( object );
	if (ret == 0)
		return compareToUsingId( object );
	else
		return ret;
}

int ExecutionObject::compareToUsingStartTime( ExecutionObject* object ) {
	double thisTime, otherTime;

	thisTime = startTime;
	otherTime = (object->getExpectedStartTime());

	if (thisTime < otherTime) {
		return -1;

	} else if (thisTime > otherTime) {
		return 1;
	}

	return 0;
}

bool ExecutionObject::containsEvent( FormatterEvent* event ) {
	return (events->count( event->getId() ) != 0);
}

FormatterEvent* ExecutionObject::getEventFromAnchorId( string anchorId ) {
	map<string, FormatterEvent*>::iterator i;
	FormatterEvent* event;
	Anchor* anchor;
	string anchorName = " ";

	if (events != NULL) {
		i = events->begin();
		LDEBUG("ExecutionObject", "getEvent: searching '%s' for '%s'  with following events:", anchorId.c_str(), id.c_str());

		while (i != events->end()) {
			event = i->second;
			if (event != NULL) {
				if (event->instanceOf( "AnchorEvent" )) {
					anchor = ((AnchorEvent*) event)->getAnchor();
					if (anchor != NULL) {
						if (anchor->instanceOf( "IntervalAnchor" )) {
							anchorName = anchor->getId();

						} else if (anchor->instanceOf( "LabeledAnchor" )) {
							anchorName = ((LabeledAnchor*) anchor)->getLabel();

						} else if (anchor->instanceOf( "LamdaAnchor" )) {
							anchorName = "";
						}
						LDEBUG("ExecutionObject", "\t%s", anchorName.c_str());
						if (anchorName == anchorId && !event->instanceOf( "SelectionEvent" )) {

							return event;
						}
					}

				} else if (event->instanceOf( "AttributionEvent" )) {
					anchor = ((AttributionEvent*) event)->getAnchor();
					if (anchor != NULL) {
						if (((PropertyAnchor*) anchor)->getPropertyName() == anchorId) {

							return event;
						}
					}
				}
			}
			++i;
		}
	}
	return NULL;
}

FormatterEvent* ExecutionObject::getEvent( string id ) {
	FormatterEvent* ev;
	if (events != NULL && events->count( id ) != 0) {
		ev = (*events)[id];
		return ev;
	}
	return NULL;
}

vector<FormatterEvent*>* ExecutionObject::getEvents() {
	vector<FormatterEvent*>* eventsVector = NULL;
	map<string, FormatterEvent*>::iterator i;

	if (events == NULL || events->empty()) {
		return NULL;
	}

	eventsVector = new vector<FormatterEvent*> ;
	for (i = events->begin(); i != events->end(); ++i) {
		eventsVector->push_back( i->second );
	}

	return eventsVector;
}

set<AnchorEvent*>* ExecutionObject::getSampleEvents() {
	set<AnchorEvent*>* eventsSet;
	map<string, FormatterEvent*>::iterator i;
	FormatterEvent* event;
	ContentAnchor* anchor;

	if (events == NULL || events->empty()) {
		return NULL;
	}

	eventsSet = new set<AnchorEvent*> ;
	i = events->begin();
	while (i != events->end()) {
		event = i->second;
		if (event->instanceOf( "AnchorEvent" )) {
			anchor = ((AnchorEvent*) event)->getAnchor();
			if (anchor->instanceOf( "SampleIntervalAnchor" )) {
				eventsSet->insert( (AnchorEvent*) event );
			}
		}
		++i;
	}

	return eventsSet;
}

double ExecutionObject::getExpectedStartTime() {
	return startTime;
}

double ExecutionObject::getExpectedDurationTime() {
	return durationTime;
}
PresentationEvent* ExecutionObject::getWholeContentPresentationEvent() {
	return wholeContent;
}

void ExecutionObject::setStartTime( double t ) {
	startTime = t;
}

void ExecutionObject::setDurationTime( double t ) {
	durationTime = t;
}
void ExecutionObject::updateEventDurations() {
	vector<PresentationEvent*>::iterator i;

	i = presEvents->begin();
	while (i != presEvents->end()) {
		updateEventDuration( *i );
		++i;
	}
}

void ExecutionObject::updateEventDuration( PresentationEvent* event ) {
	double duration;

	if (!containsEvent( (FormatterEvent*) event )) {
		return;
	}

	duration = ncl_util::NaN();

	std::string dur = descriptor->getExplicitDuration();
	if (descriptor != NULL) {
		if (descriptor->instanceOf( "CascadingDescriptor" )) {
			if (dur != "nill" && event == wholeContent) {
				dur.erase(std::remove_if(dur.begin(), dur.end(), ncl_util::is_alpha_or_space()), dur.end());
				try{
					duration = boost::lexical_cast<double>( dur ) * 1000;
				}catch(boost::bad_lexical_cast &e){
					LWARN("ExecutionObject","Conversion error[%s]",dur.c_str());
				}
			} else if (event->getDuration() > 0) {
				duration = event->getDuration();
			} else {
				duration = 0;
			}
		}

	} else {
		if (event->getDuration() > 0) {
			duration = event->getDuration();
		} else {
			duration = 0;
		}
	}

	if (duration < 0) {
		event->setDuration( ncl_util::NaN() );
	} else {
		event->setDuration( duration );
	}
}

bool ExecutionObject::removeEvent( FormatterEvent* event ) {
	vector<PresentationEvent*>::iterator i;
	set<SelectionEvent*>::iterator j;
	vector<FormatterEvent*>::iterator k;

	if (!containsEvent( event )) {
		return false;
	}

	if (event->instanceOf( "PresentationEvent" )) {
		for (i = presEvents->begin(); i != presEvents->end(); ++i) {
			if (*i == (PresentationEvent*) event) {
				presEvents->erase( i );
				break;
			}
		}
		transMan->removeEventTransition( (PresentationEvent*) event );

	} else if (event->instanceOf( "SelectionEvent" )) {
		j = selectionEvents->find( ((SelectionEvent*) event) );
		if (j != selectionEvents->end()) {
			selectionEvents->erase( j );
		}

	} else {
		for (k = otherEvents->begin(); k != otherEvents->end(); ++i) {

			if (*k == event) {
				otherEvents->erase( k );
				break;
			}
		}
	}

	if (events->count( event->getId() ) != 0) {
		events->erase( events->find( event->getId() ) );
	}

	return true;
}

bool ExecutionObject::isCompiled() {
	return isItCompiled;
}

void ExecutionObject::setCompiled( bool status ) {
	isItCompiled = status;
}

bool ExecutionObject::isTransitionInVisible() {
	return transitionInVisible;
}

void ExecutionObject::setTransitionInVisible( bool visible ) {
	transitionInVisible = visible;
}

void ExecutionObject::removeNode( Node* node ) {
	Node* parentNode;

	if (node != dataObject) {
		if (nodeParentTable->count( node ) != 0) {
			parentNode = (*nodeParentTable)[node];

			if (parentTable->count( parentNode ) != 0) {
				parentTable->erase( parentTable->find( parentNode ) );
			}

			if (nodeParentTable->count( node ) != 0) {
				nodeParentTable->erase( nodeParentTable->find( node ) );
			}
		}
	}
}

vector<Node*>* ExecutionObject::getNodes() {
	APRINT_NCLP("<==>");
	vector<Node*>* nodes;
	map<Node*, Node*>::iterator i;

	if (nodeParentTable->empty()) {
		return NULL;
	}

	nodes = new vector<Node*> ;
	for (i = nodeParentTable->begin(); i != nodeParentTable->end(); ++i) {
		nodes->push_back( i->first );
	}

	if (nodeParentTable->count( dataObject ) == 0) {
		nodes->push_back( dataObject );
	}

	return nodes;
}

NodeNesting* ExecutionObject::getNodePerspective() {
	return getNodePerspective( dataObject );
}

NodeNesting* ExecutionObject::getNodePerspective( Node* node ) {
	APRINT_NCLP("node[%s]",node->getId().c_str());
	Node* parentNode;
	NodeNesting* perspective;
	CompositeExecutionObject* parentObject;

	if (nodeParentTable->count( node ) == 0) {
		if (dataObject == node) {
			perspective = new NodeNesting();

		} else {
			return NULL;
		}

	} else {
		parentNode = (*nodeParentTable)[node];
		if (parentTable->count( parentNode ) != 0) {

			parentObject = (CompositeExecutionObject*) ((*parentTable)[parentNode]);

			perspective = parentObject->getNodePerspective( parentNode );

		} else {
			return NULL;
		}
	}
	perspective->insertAnchorNode( node );
	return perspective;
}

vector<ExecutionObject*>* ExecutionObject::getObjectPerspective() {
	return getObjectPerspective( dataObject );
}

vector<ExecutionObject*>* ExecutionObject::getObjectPerspective( Node* node ) {

	Node* parentNode;
	vector<ExecutionObject*>* perspective = NULL;
	CompositeExecutionObject* parentObject;

	if (nodeParentTable->count( node ) == 0) {
		if (dataObject == node) {
			perspective = new vector<ExecutionObject*> ;

		} else {
			return NULL;
		}

	} else {
		parentNode = (*nodeParentTable)[node];
		if (parentTable->count( parentNode ) != 0) {

			parentObject = (CompositeExecutionObject*) ((*parentTable)[parentNode]);

			perspective = parentObject->getObjectPerspective( parentNode );

		} else {
			return NULL;
		}
	}

	perspective->push_back( this );
	return perspective;
}

vector<Node*>* ExecutionObject::getParentNodes() {
	APRINT_NCLP("==>");
	vector<Node*>* parents;
	map<Node*, Node*>::iterator i;

	if (nodeParentTable->empty()) {
		APRINT_NCLP("<== NULL");
		return NULL;
	}

	parents = new vector<Node*> ;
	for (i = nodeParentTable->begin(); i != nodeParentTable->end(); ++i) {
		parents->push_back( i->second );
	}

	return parents;
}

FormatterEvent* ExecutionObject::getMainEvent() {
	return mainEvent;
}

bool ExecutionObject::prepare( FormatterEvent* event, double offsetTime ) {
	CompositeExecutionObject* parentObject;
	int size;
	map<Node*, void*>::iterator i;
	double startTime = 0;
	ContentAnchor* contentAnchor;
	FormatterEvent* auxEvent;
	AttributionEvent* attributeEvent;
	PropertyAnchor* attributeAnchor;
	int j;
	string value;

	LDEBUG("ExecutionObject", "prepare(%s)", id.c_str());
	if (event == NULL || !containsEvent( event ) || event->getCurrentState() != EventUtil::ST_SLEEPING) {

		LDEBUG("ExecutionObject", "prepare(%s) ret false", id.c_str());
		return false;
	}

	if (mainEvent != NULL && mainEvent->getCurrentState() != EventUtil::ST_SLEEPING) {

		return false;
	}

	LDEBUG("ExecutionObject", "prepare(%s) locked", id.c_str());
	mainEvent = event;
	if (mainEvent->instanceOf( "AnchorEvent" )) {
		contentAnchor = ((AnchorEvent*) mainEvent)->getAnchor();
		if (contentAnchor != NULL && contentAnchor->instanceOf( "LabeledAnchor" )) {

			i = parentTable->begin();
			while (i != parentTable->end()) {
				parentObject = (CompositeExecutionObject*) (i->second);
				// register parent as a mainEvent listener
				mainEvent->addEventListener( parentObject );
				++i;
			}
			return true;
		}
	}

	if (mainEvent->instanceOf( "PresentationEvent" )) {
		startTime = ((PresentationEvent*) mainEvent)->getBegin() + offsetTime;
		setStartTime(startTime);

		if (startTime > ((PresentationEvent*) mainEvent)->getEnd()) {
			return false;
		}
		double duration = ((PresentationEvent*) mainEvent)->getDuration();
		
		setDurationTime(duration);
	}
	i = parentTable->begin();
	while (i != parentTable->end()) {
		parentObject = (CompositeExecutionObject*) (i->second);
		// register parent as a mainEvent listener
		mainEvent->addEventListener( parentObject );
		++i;
	}

	prepareTransitionEvents( ContentAnchor::CAT_TIME, startTime );

	if (otherEvents != NULL) {
		size = otherEvents->size();
		for (j = 0; j < size; j++) {
			auxEvent = (*otherEvents)[j];
			if (auxEvent->instanceOf( "AttributionEvent" )) {
				attributeEvent = (AttributionEvent*) auxEvent;
				attributeAnchor = attributeEvent->getAnchor();
				value = attributeAnchor->getPropertyValue();
				if (value != "") {
					attributeEvent->setValue( value );
				}
			}
		}
	}

	this->offsetTime = startTime;
	return true;
}


bool ExecutionObject::canStart() {
	if (mainEvent == NULL && wholeContent == NULL) {
		return false;
	}
	if (mainEvent != NULL && mainEvent->getCurrentState() != EventUtil::ST_SLEEPING) {

		return false;
	}
	return true;
}

void ExecutionObject::start() {
	if (mainEvent == NULL) {
		prepare( wholeContent, 0.0 );
	}

	if (mainEvent->instanceOf( "AnchorEvent" )) {
		ContentAnchor* contentAnchor = ((AnchorEvent*) mainEvent)->getAnchor();
		if (contentAnchor != NULL && contentAnchor->instanceOf( "LabeledAnchor" )) {
			mainEvent->start();
		}
		//LGESP-820
		else if (contentAnchor != NULL && ( contentAnchor->instanceOf("IntervalAnchor") ||  contentAnchor->instanceOf( "SampleIntervalAnchor") )) {
			mainEvent->start();
		}
	}

	transMan->start( offsetTime );
}

bool ExecutionObject::updateTransitionTable( double value, short int transType ) {
	return transMan->updateTransitionTable( value, mainEvent, transType );
}

void ExecutionObject::resetTransitionEvents( short int transType ) {
	transMan->resetTimeIndexByType( transType );
}

void ExecutionObject::prepareTransitionEvents( short int transType, double startTime ) {
	transMan->prepare( mainEvent == wholeContent, startTime, transType );
}

set<double>* ExecutionObject::getTransitionsValues( short int transType ) {
	return transMan->getTransitionsValues( transType );
}

EventTransition* ExecutionObject::getNextTransition() {
	if (mainEvent == NULL || mainEvent->getCurrentState() == EventUtil::ST_SLEEPING || !mainEvent->instanceOf( "PresentationEvent" )) {

		return NULL;
	}

	return transMan->getNextTransition( mainEvent );
}

bool ExecutionObject::stop() {
	ContentAnchor* contentAnchor;
	double endTime;

	LDEBUG("ExecutionObject", "stop(%s)", id.c_str());

	if (mainEvent == NULL) {

		LDEBUG("ExecutionObject", "stop for '%s' returns false because mainEvent is NULL", id.c_str());
		return false;
	}

	if (mainEvent->getCurrentState() == EventUtil::ST_SLEEPING) {

		LDEBUG("ExecutionObject", "stop for '%s' returns false because mainEvent is SLEEPING", id.c_str());
		return false;
	}

	if (mainEvent->instanceOf( "PresentationEvent" )) {
		endTime = ((PresentationEvent*) mainEvent)->getEnd();
		transMan->stop( endTime );

	} else if (mainEvent->instanceOf( "AnchorEvent" )) {
		contentAnchor = ((AnchorEvent*) mainEvent)->getAnchor();
		if (contentAnchor != NULL && contentAnchor->instanceOf( "LabeledAnchor" )) {
			LDEBUG("ExecutionObject", "stop for '%s' call mainEvent '%s' stop", id.c_str(), mainEvent->getId().c_str());
			mainEvent->stop();
		}
	}

	transMan->resetTimeIndex();
	pauseCount = 0;
	return true;
}

bool ExecutionObject::abort() {
	ContentAnchor* contentAnchor;
	short objectState;
	double endTime;

	LDEBUG("ExecutionObject", "abort(%s)", id.c_str());
	if (mainEvent == NULL) {
		return false;
	}

	objectState = mainEvent->getCurrentState();

	if (objectState == EventUtil::ST_SLEEPING) {
		return false;
	}

	if (mainEvent->instanceOf( "PresentationEvent" )) {
		endTime = ((PresentationEvent*) mainEvent)->getEnd();
		transMan->abort( endTime );

	} else if (mainEvent->instanceOf( "AnchorEvent" )) {
		contentAnchor = ((AnchorEvent*) mainEvent)->getAnchor();
		if (contentAnchor != NULL && contentAnchor->instanceOf( "LabeledAnchor" )) {

			mainEvent->abort();
		}
	}

	transMan->resetTimeIndex();
	pauseCount = 0;
	return true;
}

bool ExecutionObject::pause() {
	FormatterEvent* event;
	vector<FormatterEvent*>* evs;
	vector<FormatterEvent*>::iterator i;

	LDEBUG("ExecutionObject", "pause(%s)", id.c_str());
	if (mainEvent == NULL || mainEvent->getCurrentState() == EventUtil::ST_SLEEPING) {

		return false;
	}

	evs = getEvents();
	if (evs != NULL) {
		if (pauseCount == 0) {
			i = evs->begin();
			while (i != evs->end()) {
				event = *i;
				if (event->getCurrentState() == EventUtil::ST_OCCURRING) {
					event->pause();
				}
				++i;
			}
		}
		delete evs;
		evs = NULL;
	}

	pauseCount++;
	return true;
}

bool ExecutionObject::resume() {
	FormatterEvent* event;
	vector<FormatterEvent*>* evs;
	vector<FormatterEvent*>::iterator i;

	LDEBUG("ExecutionObject", "resume(%s)", id.c_str());

	if (pauseCount == 0) {
		return false;

	} else {
		pauseCount--;
		if (pauseCount > 0) {
			return false;
		}
	}

	evs = getEvents();
	if (evs != NULL) {
		if (pauseCount == 0) {
			i = evs->begin();
			while (i != evs->end()) {
				event = *i;
				if (event->getCurrentState() == EventUtil::ST_PAUSED) {
					event->resume();
				}
				++i;
			}
		}
		delete evs;
		evs = NULL;
	}

	return true;
}

bool ExecutionObject::setProperty( const std::string &name, const std::string &value ) {
	APRINT_NCLP("<==> name[%s] value[%s]",name.c_str(),value.c_str());
	FormatterRegion* region = descriptor->getFormatterRegion();
	if (region->getLayoutRegion()->setProperty( name, value )) {
		return region->updateRegionBounds();
	}
	return false;
}

bool ExecutionObject::getParentsVisibility( void ) {
	APRINT_PROPERTY("==>");
	//check if the parent context is visible
	bool areAllParentsVisible = true;
	vector<Node*>* parentNodes;
	vector<Node*>::iterator it;
	parentNodes  = getParentNodes();
	for (it = parentNodes->begin(); it != parentNodes->end(); ++it) {
		Node* parent = (Node*) *it;
		if (parent->instanceOf( "ContextNode" ) || parent->instanceOf("DocumentNode")){
			bptni::PropertyAnchor * prop = (bptni::PropertyAnchor *) parent->getAnchor("visible");
			if (prop!=NULL){
				APRINT_NCLP("property[%s:%s]",prop->getPropertyName().c_str(),prop->getPropertyValue().c_str());
				if (prop->getPropertyName()=="visible" && prop->getPropertyValue()=="false"){
					areAllParentsVisible = false;
				}
			}
		}
	}//for
	APRINT_PROPERTY("<== return visibleParents[%d]",areAllParentsVisible);
	return areAllParentsVisible;
}

bool ExecutionObject::setPropertyValue( AttributionEvent* event, string value ) {
	APRINT_NCLP("<==>");
	return setProperty( (event->getAnchor())->getPropertyName(), value );
}

bool ExecutionObject::unprepare() {
	if (mainEvent == NULL || mainEvent->getCurrentState() != EventUtil::ST_SLEEPING) {
		LDEBUG("ExecutionObject", "unprepare(%s) unlocked ret false", id.c_str());
		return false;
	}

	map<Node*, void*>::iterator i;
	CompositeExecutionObject* parentObject;

	i = parentTable->begin();
	while (i != parentTable->end()) {
		parentObject = (CompositeExecutionObject*) (i->second);
		// register parent as a mainEvent listener
		mainEvent->delEventListener( parentObject );
		++i;
	}

	mainEvent = NULL;
	LDEBUG("ExecutionObject", "unprepare(%s) unlocked", id.c_str());
	return true;
}

void ExecutionObject::setHandling( bool isHandling ) {
	this->isHandling = isHandling;
}

void ExecutionObject::setHandler( bool isHandler ) {
	this->isHandler = isHandler;
}

void ExecutionObject::selectionEvent( int accessCode, double currentTime ) {
	int selCode;
	SelectionEvent* selectionEvent;
	IntervalAnchor* intervalAnchor;
	FormatterEvent* expectedEvent;
	Anchor* expectedAnchor;
	string anchorId = "";
	set<SelectionEvent*>* selectedEvents;
	set<SelectionEvent*>::iterator i;

	if ((!isHandling && !isHandler) || mainEvent == NULL || mainEvent->getCurrentState() != EventUtil::ST_OCCURRING) {

		return;
	}

	//simulating some keys of keyboard
	if (accessCode == util::key::f1) {
		accessCode = util::key::red;

	} else if (accessCode == util::key::f2) {
		accessCode = util::key::green;

	} else if (accessCode == util::key::f3) {
		accessCode = util::key::yellow;

	} else if (accessCode == util::key::f4) {
		accessCode = util::key::blue;

	} else if (accessCode == util::key::f5) {
		accessCode = util::key::menu;

	} else if (accessCode == util::key::f6) {
		accessCode = util::key::info;

	} else if (accessCode == util::key::f7) {
		accessCode = util::key::epg;

	} else if (accessCode == util::key::plus_sign) {
		accessCode = util::key::volume_up;

	} else if (accessCode == util::key::minus_sign) {
		accessCode = util::key::volume_down;

	} else if (accessCode == util::key::page_up) {
		accessCode = util::key::channel_up;

	} else if (accessCode == util::key::page_down) {
		accessCode = util::key::channel_down;

	} else if (accessCode == util::key::backspace) {
		accessCode = util::key::back;

	} else if (accessCode == util::key::escape) {
		accessCode = util::key::exit;
	}

	selectedEvents = new set<SelectionEvent*> ;
	i = selectionEvents->begin();
	while (i != selectionEvents->end()) {
		selectionEvent = (SelectionEvent*) (*i);
		selCode = selectionEvent->getSelectionCode();
		if (selCode == accessCode) {
			if (selectionEvent->getAnchor()->instanceOf( "LambdaAnchor" )) {

				selectedEvents->insert( selectionEvent );

			} else if (selectionEvent->getAnchor()->instanceOf( "IntervalAnchor" )) {

				intervalAnchor = (IntervalAnchor*) (selectionEvent->getAnchor());

				if (intervalAnchor->getBegin() <= currentTime && currentTime <= intervalAnchor->getEnd()) {

					selectedEvents->insert( selectionEvent );
				}

			} else {
				expectedAnchor = selectionEvent->getAnchor();
				if (expectedAnchor->instanceOf( "LabeledAnchor" )) {
					anchorId = ((LabeledAnchor*) expectedAnchor)->getLabel();

				} else {
					anchorId = expectedAnchor->getId();
				}

				expectedEvent = getEventFromAnchorId( anchorId );
				if (expectedEvent != NULL) {
					LDEBUG("ExecutionObject", "select(%s) analyzing event '%s' ", id.c_str(), expectedEvent->getId().c_str());
					if (expectedEvent->getCurrentState() == EventUtil::ST_OCCURRING) {

						selectedEvents->insert( selectionEvent );

					}
				} else {
					LWARN("ExecutionObject", "select(%s) can't find event for anchorid = '%s'", id.c_str(),anchorId.c_str());
				}
			}
		}
		++i;
	}

	i = selectedEvents->begin();
	while (i != selectedEvents->end()) {
		selectionEvent = (*i);

		LDEBUG("ExecutionObject", "select(%s) starting event '%s'", id.c_str(),selectionEvent->getId().c_str());

		if (!selectionEvent->start()) {
			LWARN("ExecutionObject", "select cant start '%s'", selectionEvent->getId().c_str());

		}
		++i;
	}

	delete selectedEvents;
	selectedEvents = NULL;
}

::util::key::Keys ExecutionObject::getInputEvents() {
	set<SelectionEvent*>::iterator i;
	::util::key::Keys keys;
	SelectionEvent* ev;
	util::key::type keyCode;

	//	keys = new set<int>;
	i = selectionEvents->begin();
	while (i != selectionEvents->end()) {
		ev = (*i);
		keyCode = ev->getSelectionCode();
		keys.insert( keyCode );
		if (keyCode == util::key::red) {
			keys.insert( util::key::f1 );

		} else if (keyCode == util::key::green) {
			keys.insert( util::key::f2 );

		} else if (keyCode == util::key::yellow) {
			keys.insert( util::key::f3 );

		} else if (keyCode == util::key::blue) {
			keys.insert( util::key::f4 );

		} else if (keyCode == util::key::menu) {
			keys.insert( util::key::f5 );

		} else if (keyCode == util::key::info) {
			keys.insert( util::key::f6 );

		} else if (keyCode == util::key::epg) {
			keys.insert( util::key::f7 );

		} else if (keyCode == util::key::volume_up) {
			keys.insert( util::key::plus_sign );

		} else if (keyCode == util::key::volume_down) {
			keys.insert( util::key::minus_sign );

		} else if (keyCode == util::key::channel_up) {
			keys.insert( util::key::page_up );

		} else if (keyCode == util::key::channel_down) {
			keys.insert( util::key::page_down );

		} else if (keyCode == util::key::back) {
			keys.insert( util::key::backspace );

		} else if (keyCode == util::key::exit) {
			keys.insert( util::key::escape );
		}
		++i;
	}

	return keys;
}

}
}
}
}
}
}
}
