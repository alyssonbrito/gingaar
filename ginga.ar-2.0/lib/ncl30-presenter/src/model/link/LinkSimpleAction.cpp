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

#include "../../../include/model/LinkSimpleAction.h"
#include "../../../include/model/LinkAssignmentAction.h"
#include <gingaplayer/system.h>
#include <util/log.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
LinkSimpleAction::LinkSimpleAction( FormatterEvent* event, short type, player::System *system ) :
		LinkAction(system) {

	this->event = event;
	this->actionType = type;
	listener = NULL;
	typeSet.insert( "LinkSimpleAction" );
}

LinkSimpleAction::~LinkSimpleAction() {
	listener = NULL;
	event = NULL;
}

FormatterEvent* LinkSimpleAction::getEvent() {
	return event;
}

short LinkSimpleAction::getType() {
	return actionType;
}

void LinkSimpleAction::setActionListener( LinkActionListener* listener ) {

	this->listener = listener;
}

vector<FormatterEvent*>* LinkSimpleAction::getEvents() {
	if (event == NULL
	)
		return NULL;

	vector<FormatterEvent*>* events;
	events = new vector<FormatterEvent*>;

	events->push_back( event );
	return events;
}

vector<LinkAction*>* LinkSimpleAction::getImplicitRefRoleActions() {
	vector<LinkAction*>* actions;
	string value;

	actions = new vector<LinkAction*>;

	if (this->instanceOf( "LinkAssignmentAction" )) {
		value = ((LinkAssignmentAction*) this)->getValue();
		if (value[0] == '$') {
			if (event->instanceOf( "AttributionEvent" )) {
				actions->push_back( (LinkAction*) this );
			}
		}
	}

	if (actions->empty()) {
		delete actions;
		return NULL;
	}

	return actions;	
}

void LinkSimpleAction::run() {
	LinkAction::run();
}

void LinkSimpleAction::onRun( void ) {	
  	APRINT_NCLP("<==>");
	if (listener != NULL) {
		listener->runAction( (void*) this );
	}

	if (actionType == SimpleAction::ACT_START) {
		if (event->instanceOf( "PresentationEvent" )) {
			if (((PresentationEvent*) event)->getRepetitions() >= 1) {
				LinkAction::_sys->registerTimer( (util::DWORD) ((PresentationEvent*) event)->getRepetitionInterval(), boost::bind( &LinkSimpleAction::onRun, this ) );
				((PresentationEvent*) event)->decrementNumPresentation();
			}
		}
		/*
		LDEBUG("LinkSimpleAction", "run: notify action INIT");
		if (event != NULL) {
			LDEBUG("LinkSimpleAction", "'%s'", event->getId().c_str());
		}
		*/
		notifyProgressionListeners( true );
	} else {
		/*
		LDEBUG("LinkSimpleAction", "run: notify action END");
		if (event != NULL) {
			LDEBUG("LinkSimpleAction", "'%s'", event->getId().c_str());
		}
		*/
		notifyProgressionListeners( false );
	}
}
}
}
}
}
}
}
}
