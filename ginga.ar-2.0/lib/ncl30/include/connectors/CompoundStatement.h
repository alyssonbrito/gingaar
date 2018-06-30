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

#ifndef _COMPOUNDSTATEMENT_H_
#define _COMPOUNDSTATEMENT_H_

#include "Statement.h"

#include <vector>
#include <string>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {

class Role;

class CompoundStatement: public Statement
{
public:
	static const short OP_AND = 0;
	static const short OP_OR = 1;

protected:
	std::vector<Statement*>* statements;
	short myOperator;
	bool negated;

public:
	CompoundStatement();
	CompoundStatement( Statement* p1, Statement* p2, short op );

	virtual ~CompoundStatement() {
	}
	;
	void setOperator( short op );
	short getOperator();
	std::vector<Statement*>* getStatements();
	void addStatement( Statement* statement );
	void removeStatement( Statement* statement );
	void setNegated( bool newNegated );
	bool isNegated();
	std::vector<Role*> *getRoles();
	bool instanceOf( const std::string &type ) {
		return Statement::instanceOf( type );
	}
};
}
}
}
}
}

#endif //_COMPOUNDSTATEMENT_H_
