/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementaci�n de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los t�rminos de la Licencia P�blica General Reducida GNU como es publicada por la
  Free Software Foundation, seg�n la versi�n 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANT�A; ni siquiera la garant�a impl�cita de COMERCIALIZACI�N o ADECUACI�N
  PARA ALG�N PROP�SITO PARTICULAR. Para m�s detalles, revise la Licencia P�blica
  General Reducida GNU.

    Ud. deber�a haber recibido una copia de la Licencia P�blica General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "ident.h"
#include "pool.h"
#include <stdlib.h>

namespace util {
namespace id {

bool isValid( const Ident &id ) {
	return id.use_count() > 0 && id->isValid();
}

IdentType::IdentType( Pool *pool, ID_TYPE id )
	: _pool(pool), _id( id )
{
}

IdentType::IdentType( const IdentType &other )
{
	copy( other );
}

IdentType::~IdentType()
{
	if (_pool) {
		_pool->free( _id );
	}
}

bool IdentType::isValid() const {
	return _id >= 0;
}

void IdentType::copy( const IdentType &other ) {
	if (_pool && _id >= 0) {
		_pool->free( _id );
	}
	_pool = other._pool;
	_id = other._id;
}

}
}

