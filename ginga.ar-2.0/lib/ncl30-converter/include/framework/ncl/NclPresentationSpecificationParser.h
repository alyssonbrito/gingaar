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

#ifndef NCLPRESENTATIONSPECIFICATIONPARSER_H_
#define NCLPRESENTATIONSPECIFICATIONPARSER_H_

#include "../ModuleParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl{
namespace descriptor{
	class GenericDescriptor;
}
}
namespace converter {
namespace framework {
	class DocumentParser;
}
}
}
}
}
namespace bptcf = ::br::pucrio::telemidia::converter::framework;
namespace bptnd = ::br::pucrio::telemidia::ncl::descriptor;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {

class NclImportParser;

class NclPresentationSpecificationParser: public ModuleParser
{
private:
	NclImportParser *importParser;
	void *presentationControlParser;

public:
	explicit NclPresentationSpecificationParser( bptcf::DocumentParser *documentParser );
	void *parseDescriptor( DOMElement *parentElement );
	virtual void *createDescriptor( DOMElement *parentElement )=0;

	virtual void addDescriptorParamToDescriptor( void *parentObject, void *childObject )=0;

	void *parseDescriptorBase( DOMElement *parentElement );

	virtual void *createDescriptorBase( DOMElement *parentElement )=0;

	virtual void addImportBaseToDescriptorBase( void *parentObject, void *childObject )=0;

	virtual void addDescriptorSwitchToDescriptorBase( void *parentObject, void *childObject )=0;

	virtual void addDescriptorToDescriptorBase( void *parentObject, void *childObject )=0;

	void *parseDescriptorBind( DOMElement *parentElement );

	virtual void *createDescriptorBind( DOMElement *parentElement )=0;

	void *parseDescriptorParam( DOMElement *parentElement );

	virtual void *createDescriptorParam( DOMElement *parentElement )=0;

	virtual bptnd::GenericDescriptor* createDummyDescriptorBase()=0;

	NclImportParser *getImportParser();
	void setImportParser( NclImportParser *importParser );
	void *getPresentationControlParser();
	void setPresentationControlParser( void *presentationControlParser );
};
}
}
}
}
}
}

#endif /*NCLPRESENTATIONSPECIFICATIONPARSER_H_*/
