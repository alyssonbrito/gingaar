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

#include "../../include/connectors/ConnectorBase.h"
#include "../../include/connectors/Connector.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
ConnectorBase::ConnectorBase( const std::string &id ) :
		Base( id ) {
	typeSet.insert( "ConnectorBase" );
}

bool ConnectorBase::addConnector( Connector* connector ) {
	if (connector == NULL || containsConnector( connector ))
		return false;

	connectorSet.push_back( connector );
	return true;
}

bool ConnectorBase::addBase( Base* base, const std::string &alias, const std::string &location ) {

	if (base->instanceOf( "ConnectorBase" )) {
		return Base::addBase( base, alias, location );
	}
	return false;
}

void ConnectorBase::clear() {
	connectorSet.clear();
	Base::clear();
}

bool ConnectorBase::containsConnector( const std::string &connectorId ) {
	if (getConnectorLocally( connectorId ) != NULL
	)
		return true;
	else
		return false;
}

bool ConnectorBase::containsConnector( Connector* connector ) {
	std::vector<Connector*>::iterator i;
	for (i = connectorSet.begin(); i != connectorSet.end(); ++i) {
		if (*i == connector)
			return true;
	}
	return false;
}

Connector* ConnectorBase::getConnectorLocally( const std::string &connectorId ) {
	std::vector<Connector*>::iterator connectors;

	connectors = connectorSet.begin();
	while (connectors != connectorSet.end()) {
		if ((*connectors)->getId() == connectorId)
			return (*connectors);

		++connectors;
	}
	return NULL;
}

Connector* ConnectorBase::getConnector( const std::string &connectorId ) {
	std::string::size_type index;
	std::string prefix, suffix;
	ConnectorBase* base;

	index = connectorId.find_first_of( "#" );
	if (index == std::string::npos) {
		return getConnectorLocally( connectorId );
	}
	prefix = connectorId.substr( 0, index );
	index++;
	suffix = connectorId.substr( index, connectorId.length() - index );
	if (baseAliases->find( prefix ) != baseAliases->end()) {
		base = (ConnectorBase*) ((*baseAliases)[prefix]);
		return base->getConnector( suffix );

	} else if (baseLocations->find( prefix ) != baseLocations->end()) {
		base = (ConnectorBase*) ((*baseLocations)[prefix]);
		return base->getConnector( suffix );
	}
	return NULL;
}

bool ConnectorBase::removeConnector( const std::string &connectorId ) {
	Connector* connector;

	connector = getConnectorLocally( connectorId );
	return removeConnector( connector );
}

bool ConnectorBase::removeConnector( Connector* connector ) {
	std::vector<Connector*>::iterator i;
	for (i = connectorSet.begin(); i != connectorSet.end(); ++i) {
		if (*i == connector) {
			connectorSet.erase( i );
			return true;
		}
	}
	return false;
}
}
}
}
}
}
