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

#include "../../include/connectors/AttributeAssessment.h"
#include "../../include/connectors/EventUtil.h"
#include <boost/lexical_cast.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
AttributeAssessment::AttributeAssessment( const std::string &role ) :
		Assessment(),
		Role() {

	Role::setLabel( role );
	eventType = EventUtil::EVT_ATTRIBUTION;
	attributeType = EventUtil::ATT_NODE_PROPERTY;
	key = "";
	offset = "";
	maxCon = 1;
	typeSet.insert( "AttributeAssessment" );
	typeSet.insert( "Role" );
}

void AttributeAssessment::setMaxCon( int max ) {
	maxCon = max;
}

void AttributeAssessment::setMinCon( int min ) {
	minCon = min;
}

short AttributeAssessment::getAttributeType() {
	return attributeType;
}

void AttributeAssessment::setAttributeType( short attribute ) {
	attributeType = attribute;
}

const std::string &AttributeAssessment::getKey() {
	return key;
}

void AttributeAssessment::setKey( const std::string &key ) {
	this->key = key;
}

const std::string &AttributeAssessment::getOffset() {
	return offset;
}

void AttributeAssessment::setOffset( const std::string &offset ) {
	this->offset = offset;
}

std::string AttributeAssessment::toString() {
	return boost::lexical_cast < std::string > (attributeType);
}
}
}
}
}
}
