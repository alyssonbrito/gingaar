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

#ifndef PRESENTATIONENGINEMANAGER_H_
#define PRESENTATIONENGINEMANAGER_H_

//#include <ncl30/NclDocument.h>
// Workaround for Bridge issue (XletPlayer is including a file in wrong way)
#include <NclDocument.h>
// Workaround for Bridge issue (XletPlayer is including a file in wrong way)

#include <util/keydefs.h>
#include <boost/function.hpp>
#include <string>
#include <canvas/rect.h>


// Platform Dependent. It should be put in a #ifdef... And it's not a good approach...
#include <pem-tv-util/ginga_n_port_data.h>


namespace player {
	class System;
	class Device;
namespace settings {
	class Settings;
}
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {


class FormatterMediator;
class EmbeddedPem;

class PresentationEngineManager {
public:
	PresentationEngineManager( player::System *sys );
	virtual ~PresentationEngineManager();

	bool run();

	bool startDocument( const std::string &file, bool embedded = false );
	void stopDocument();
	void stopEmbeddedDocument();
	void qStopDocument();
	void setBaseId( const std::string &id );
	bool setDocument( const std::string &file );
	
	//	test cases key callback
	typedef boost::function<void( ::util::key::type key )> KeyPressCallback;
	static void onKeyPress( const KeyPressCallback &callback );

	//	Enqueue task into presentation thread
	void enqueue( const boost::function<void (void)> &func );

	player::Device *mainScreen() const;
	virtual void* runEmbedded( const std::string &url, player::System *sys );
	virtual void stopEmbedded( void *window );

	// It's needed to create set/get
	std::string path;

protected:
	void userEventReceived( ::util::key::type key, bool isUp );

	//	Getters
	player::System *sys();
	const std::string &baseId();
	virtual FormatterMediator *formatter();

	virtual void initFormatter( const std::string &file );
	virtual void delFormatter();
	virtual void onStart();
public:
	virtual void onStop();
public:
	virtual bool nclControlDoc( int controlCode, std::string prm );
	virtual bool nclEdit( int command, std::string prm1, std::string prm2, std::string prm3 );
	virtual std::string nclSettingsNodesGetValue( std::string name );
	virtual std::string nclGetPropertyValue( std::string name );


	virtual /*bool*/ void nclSetVisible( int id, bool visible );
	typedef boost::function<void (int id, bool visible)> cbNCLXletVisibility;
	void registerNCLPlayerSetVisibility( const cbNCLXletVisibility &callback );


	virtual bool nclSetBounds( int width, int height, int x1, int y1, int x2, int y2 );
	typedef boost::function<void (int eventType, std::string value)> notifyOfNCLPlayerEvent;
	void RegisterNCLPlayerEvent( const notifyOfNCLPlayerEvent &callback );
	void notifyNCLPlayerEvent( short actionType, std::string propValue );

	virtual void nclStartXlet( const std::string &initialClass, const std::string &classPath, int x, int y, unsigned int width,unsigned int height );
	typedef boost::function<void (const std::string &initialClass, const std::string &classPath, int x, int y, unsigned int width, unsigned int height)> cbNCLStartXlet;
	void registerNCLStartXlet( const cbNCLStartXlet &callback );

	virtual void nclControlXlet( long controlCode );
	typedef boost::function<void (long controlCode)> cbNCLControlXlet;
	void registerNCLControlXlet( const cbNCLControlXlet &callback );

	void nclNotifyXletAppState( unsigned char state );
	typedef boost::function<void (unsigned char state)> notifyOfXletAppState;
	virtual void registerXletAppStateCallback( const notifyOfXletAppState &callback );

	typedef boost::shared_ptr<EmbeddedPem> EmbeddedPtr;
	typedef std::vector<EmbeddedPtr> Managers;
	//typedef int (*notifyOnStopDoc)(void);
	typedef boost::function<int (void)> notifyOnStopDoc;
	void setOnStopDocument(const notifyOnStopDoc &_callback);

private:
	player::System *_sys;
	FormatterMediator* _formatter;
	bool _paused;
	bool _stopped;
	std::string _currentPrivateBaseId;
	player::settings::Settings *_settings;
	Managers _managers;
	std::string _file;
	notifyOnStopDoc _onStopCallback;
	notifyOfNCLPlayerEvent _NCLPlayerEvent;
	notifyOfXletAppState _XletAppState;
	cbNCLStartXlet _NCLStartXlet;
	cbNCLControlXlet _NCLControlXlet;
	cbNCLXletVisibility _NCLPlayerSetVisibility;
};

}
}
}
}
}

#endif /*PRESENTATIONENGINEMANAGER_H_*/
