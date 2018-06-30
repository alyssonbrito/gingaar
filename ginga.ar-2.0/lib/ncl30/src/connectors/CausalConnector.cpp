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

#include "../../include/connectors/CausalConnector.h"
#include "../../include/connectors/CompoundAction.h"
#include "../../include/connectors/CompoundStatement.h"
#include "../../include/connectors/CompoundCondition.h"
#include "../../include/connectors/SimpleAction.h"
#include "../../include/connectors/SimpleCondition.h"
#include "../../include/connectors/AssessmentStatement.h"
#include "../../include/connectors/Action.h"
#include "../../include/connectors/Role.h"
#include "../../include/connectors/ConditionExpression.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
CausalConnector::CausalConnector( const std::string &id ) :
		Connector( id ),
		conditionExpression( NULL ),
		actionExpression( NULL ) {

	typeSet.insert( "CausalConnector" );
}

CausalConnector::CausalConnector( const std::string &id, ConditionExpression *condition, Action *action ) :
		Connector( id ) {

	typeSet.insert( "CausalConnector" );
	conditionExpression = (TriggerExpression*) condition;
	actionExpression = (Action*) action;
}

Action *CausalConnector::getAction() {
	return actionExpression;
}

ConditionExpression *CausalConnector::getConditionExpression() {
	return conditionExpression;
}

void CausalConnector::setAction( Action *newAction ) {
	actionExpression = newAction;
}

void CausalConnector::setConditionExpression( ConditionExpression *newConditionExpression ) {

	conditionExpression = newConditionExpression;
}

void CausalConnector::getConditionRoles( ConditionExpression *condition, std::vector<Role*> *roles ) {

	std::vector<Role*> *childRoles;

	if (conditionExpression->instanceOf( "SimpleCondition" )) {
		roles->push_back( (SimpleCondition*) condition );

	} else {
		if (conditionExpression->instanceOf( "AssessmentStatement" )) {
			childRoles = ((AssessmentStatement*) condition)->getRoles();

		} else if (conditionExpression->instanceOf( "CompoundCondition" )) {

			childRoles = ((CompoundCondition*) condition)->getRoles();

		} else {
			childRoles = ((CompoundStatement*) condition)->getRoles();
		}

		std::vector<Role*>::iterator it;
		for (it = childRoles->begin(); it != childRoles->end(); ++it) {
			roles->push_back( *it );
		}

		delete childRoles;
	}
}

void CausalConnector::getActionRoles( Action *action, std::vector<Role*> *roles ) {

	std::vector<Role*>* childRoles;

	if (actionExpression->instanceOf( "SimpleAction" )) {
		roles->push_back( (SimpleAction*) action );

	} else {
		childRoles = ((CompoundAction*) action)->getRoles();
		std::vector<Role*>::iterator it;
		for (it = childRoles->begin(); it != childRoles->end(); ++it) {
			roles->push_back( *it );
		}

		delete childRoles;
	}
}

std::vector<Role*> *CausalConnector::getRoles() {
	std::vector<Role*> *roles;
	roles = new std::vector<Role*>;
	getConditionRoles( conditionExpression, roles );
	getActionRoles( actionExpression, roles );
	return roles;
}
}
}
}
}
}
