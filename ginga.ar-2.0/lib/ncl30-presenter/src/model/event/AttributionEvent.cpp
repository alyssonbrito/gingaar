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

#include "../../../include/model/AttributionEvent.h"
#include "../../../include/model/ExecutionObject.h"
#include <gingaplayer/player/settings.h>
#include <ncl30/components/NodeEntity.h>
#include <ncl30/components/ContentNode.h>
#include <ncl30/reuse/ReferNode.h>

using namespace ::br::pucrio::telemidia::ncl::components;
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;
using namespace ::br::pucrio::telemidia::ncl::reuse;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {
AttributionEvent::AttributionEvent( const std::string &id, components::ExecutionObject* eObject, PropertyAnchor* anchor, ncl30presenter::event::EventManager *manager, player::System *system ) :
		FormatterEvent( id, eObject, manager ) {

	Entity* entity;
	NodeEntity* dataObject;

	typeSet.insert( "AttributionEvent" );
	this->anchor = anchor;
	this->valueMaintainer = NULL;
	this->settingNode = false;
	this->assessments = new map<string, FormatterEvent*>;

	dataObject = (NodeEntity*) (getExecutionObject()->getDataObject());

	if (dataObject->instanceOf( "ContentNode" ) && ((ContentNode*) dataObject)->isSettingNode()) {

		settingNode = true;
	}

	if (dataObject->instanceOf( "ReferNode" )) {
		if (((ReferNode*) dataObject)->getInstanceType() == "instSame") {
			entity = ((ReferNode*) dataObject)->getDataEntity();
			if (entity->instanceOf( "ContentNode" ) && ((ContentNode*) entity)->isSettingNode()) {

				settingNode = true;
			}
		}
	}
	_sys = system;
}

AttributionEvent::~AttributionEvent() {
	delete assessments;
}

PropertyAnchor* AttributionEvent::getAnchor() {
	return anchor;
}

string AttributionEvent::getCurrentValue() {
	string propName;
	string maintainerValue = "";

	if (settingNode) {
		propName = anchor->getPropertyName();
		if (propName != "") {
			maintainerValue =  _sys->settings()->getProperty( propName );
		}
	} else {
		if (valueMaintainer != NULL) {
			maintainerValue = valueMaintainer->getPropertyValue( this );
		}

		if (maintainerValue == "") {
			maintainerValue = anchor->getPropertyValue();
		}
	}

	return maintainerValue;
}

bool AttributionEvent::setValue( string newValue ) {
	/*		if ((value == "" && newValue != "") ||
	 (newValue == "" && value != "") ||
	 (newValue != "" && value != "" && (newValue != value))) {

	 value = newValue;
	 return true;
	 }

	 return false;*/

	if (anchor->getPropertyValue() != newValue) {
		anchor->setPropertyValue( newValue );
		return true;
	}
	return false;
}

void AttributionEvent::setValueMaintainer( IAttributeValueMaintainer* valueMaintainer ) {

	this->valueMaintainer = valueMaintainer;
}

void AttributionEvent::setImplicitRefAssessmentEvent( string roleId, FormatterEvent* event ) {

	(*assessments)[roleId] = event;
}

FormatterEvent* AttributionEvent::getImplicitRefAssessmentEvent( string roleId ) {

	if (assessments->count( roleId ) == 0) {
		return NULL;
	}

	return (*assessments)[roleId];
}
}
}
}
}
}
}
}
