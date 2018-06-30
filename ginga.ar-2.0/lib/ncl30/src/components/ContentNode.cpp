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

#include "../../include/components/ContentNode.h"
#include "../../include/components/Content.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <cstdio>
namespace fs = boost::filesystem;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
ContentNode::ContentNode( const std::string &uid, Content* someContent ) :
		NodeEntity( uid, someContent ) {

	typeSet.insert( "ContentNode" );
	typeSet.insert( "DocumentNode" );

	_type = "";
	_url = "";
	// must set to false before a new isSettingNode call
	isSettingNodeType = false;
	isSettingNodeType = isSettingNode();
}

ContentNode::~ContentNode() {
}

bool ContentNode::isSettingNode() {
	std::string nodeType = getNodeType();

	if (isSettingNodeType) {
		return true;
	}

	if (nodeType == "") {
		return false;
	}

	//W3C (and RFC2045) type value isn't sensitive
	boost::to_upper( nodeType );
	return (nodeType == "APPLICATION/X-GINGA-SETTINGS" || nodeType == "APPLICATION/X-NCL-SETTINGS");

}

const std::string &ContentNode::getNodeType( void ) const {
	return _type;
}

void ContentNode::setNodeType( const std::string &type ) {
	_type = type;

	// must set to false before a new isSettingNode call
	isSettingNodeType = false;
	isSettingNodeType = isSettingNode();
}

void ContentNode::setUrl( const std::string &documentUrl, const std::string &url ) {

	std::string fixedUrl = fixFileUrl ( url );

	if (!boost::find_first( fixedUrl, "://" )) {
		fs::path path( fixedUrl );
		if (!path.is_complete()) {
			path = documentUrl / path;
		}
		_url = "file://" + path.string();
	} else {
		_url = fixedUrl;
	}
}

const std::string &ContentNode::getUrl( void ) const {
	return _url;
}


std::string ContentNode::fixFileUrl( const std::string &rawUrl ) const {
	const std::string locator ( "file:///" );
	std::string url = rawUrl;
	if ( url.find ( locator ) != std::string::npos ) {
		url = url.substr ( locator.length ( ) );
	}
	return url;
}



}
}
}
}
}
