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
#pragma once

#include "../types.h"
#include "../id/pool.h"
#include <boost/function.hpp>


namespace util {
namespace io {

class Dispatcher {
public:
	Dispatcher();
	virtual ~Dispatcher();

	//	Initialization
	virtual bool initialize();
	virtual void finalize();

	//	Run/exit io event notification loop
	virtual void run()=0;
	virtual void exit()=0;

	//	Types
	typedef boost::function<void (void)> DispatcherTask;	
	typedef boost::function<void ( id::Ident & )> Callback;

	//	Post a event
	virtual void postEvent( const DispatcherTask &task )=0;

	//	Timer events
	virtual id::Ident addTimer( int ms, const Callback &callback )=0;
	virtual void stopTimer( id::Ident &id )=0;
	
	//	IO events
	virtual id::Ident addIO( DESCRIPTOR fd, const Callback &callback )=0;
	virtual void stopIO( id::Ident &id )=0;

	//	Socket events
	virtual id::Ident addSocket( SOCKET sock, const Callback &callback )=0;
	virtual void stopSocket( id::Ident &id )=0;	

	//	Instance creation
	static Dispatcher *create();

protected:
	id::Ident allocID();

private:
	id::Pool _pool;
};

}
}

