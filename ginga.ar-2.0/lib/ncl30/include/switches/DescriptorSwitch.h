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

#ifndef _DESCRIPTORSWITCH_H_
#define _DESCRIPTORSWITCH_H_

#include "../descriptor/GenericDescriptor.h"
namespace bptnd = ::br::pucrio::telemidia::ncl::descriptor;

#include <string>
#include <iostream>
#include <vector>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace switches {

class Rule;

class DescriptorSwitch: public bptnd::GenericDescriptor
{
private:
	std::vector<GenericDescriptor*> descriptorList;
	std::vector<Rule*> ruleList;
	GenericDescriptor* defaultDescriptor;
	GenericDescriptor* selectedDescriptor;

public:
	explicit DescriptorSwitch( const std::string &id );
	virtual ~DescriptorSwitch() {
	}
	;
	bool addDescriptor( unsigned int index, GenericDescriptor* descriptor, Rule* rule );

	bool addDescriptor( GenericDescriptor* descriptor, Rule* rule );
	void clear();
	bool containsRule( Rule* rule );
	void exchangeDescriptorsAndRules( unsigned int index1, unsigned int index2 );

	int indexOfRule( Rule *rule );
	GenericDescriptor* getDefaultDescriptor();
	unsigned int indexOfDescriptor( GenericDescriptor* descriptor );
	GenericDescriptor* getDescriptor( unsigned int index );
	GenericDescriptor* getDescriptor( const std::string &descriptorId );
	GenericDescriptor* getDescriptor( Rule* rule );
	Rule* getRule( unsigned int index );
	unsigned int getNumDescriptors();
	unsigned int getNumRules();
	bool removeDescriptor( unsigned int index );
	bool removeDescriptor( GenericDescriptor* descriptor );
	bool removeRule( Rule *rule );
	void setDefaultDescriptor( GenericDescriptor* descriptor );
	void select( GenericDescriptor* descriptor );
	void selectDefault();
	GenericDescriptor* getSelectedDescriptor();
};
}
}
}
}
}

#endif //_DESCRIPTORSWITCH_H_
