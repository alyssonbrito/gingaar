#pragma once

#include "types.h"
#include <canvas/types.h>
#include <vector>
#include <map>
#include <string>

namespace connector {
	class EditingCommandData;
}

namespace canvas {
	class MngViewer;
}
namespace player {

class System;
class Player;

class Device {
public:
	Device( System *sys );
	virtual ~Device();

	//	Player methods
	Player *create( const std::string &url, const std::string &mime, bool isAlt = false );
	virtual Player *create( type::type typeOfPlayer, bool isAlt = false );
	void destroy( Player *player );

	//	Initialize
	virtual bool initialize( canvas::Surface* surface = NULL );
	virtual void finalize();

	//	Start/stop
	virtual void start();
	virtual void stop();
	void exit();

	//	Input manager
	virtual void onReserveKeys( const ::util::key::Keys &keys );
	void dispatchKey( ::util::key::type keystroke, bool isUp );
	void dispatchEditingCommand( connector::EditingCommandData *event );
	bool injectKey( ::util::key::type keystroke, bool isUp );

	//	External resources
	bool download( const std::string &url, std::string &newFile );

	//	Factory methods
	virtual canvas::MediaPlayer *createMediaPlayer();
	virtual canvas::WebViewer *createWebViewer( canvas::Surface * );
	virtual canvas::MngViewer *createMngViewer( canvas::Surface * );

	//	Getters
	canvas::Size size() const;
	System *systemPlayer() const;
	canvas::System *system() const;
	virtual bool haveVideo() const;
	virtual bool haveAudio() const;

	void setEmbeddedSize( canvas::Size size );

	static const int MAX_MNG_PLAYERS_NUMBER = 80;
	static int _numberOfMNGPlayers;

	// LGESP: Change Request
	// Description: File is copied from Carrousel to Temporary directory only
	// on demand. This listener is invoked when "Player *Device::create(const std::
	// string &url, const std::string &mime)" is invoked.
public:
        typedef int (*FetchFilesCallback)(const std::string &url);
        void setOnFetchFile(FetchFilesCallback cb) { fetchFilesCallback = cb; }
        void onFetchFile(const std::string &url);

/*        typedef int (*GetPropertyCallback)(const std::string &key,std::string *value);
        void setOnGetProperty(GetPropertyCallback cb) { getPropertyCallback = cb; }
        void onGetProperty(const std::string &key,std::string *value);

        typedef int (*SetPropertyCallback)(const std::string &key,const std::string &value);
        void setOnSetProperty(SetPropertyCallback cb) { setPropertyCallback = cb; }
        void onSetProperty(const std::string &key,const std::string &value);*/

        typedef int (*ReserveKeysCallback)(int);
        void setOnReserveKeys(ReserveKeysCallback callback)	{ _reserveKeysCallback = callback;	};
        void onReserveKeys(int Key);

        typedef int (*AnyEventCallbackType)();
        void setOnKeyBeforeInit(AnyEventCallbackType callback)	{ _beforeInitCallback = callback;	}

private:
        FetchFilesCallback fetchFilesCallback;
        //GetPropertyCallback getPropertyCallback;
        //SetPropertyCallback setPropertyCallback;
        ReserveKeysCallback _reserveKeysCallback;
        AnyEventCallbackType _beforeInitCallback;

        // end of LGESP
        // LGESP: Change Request
public:
	typedef int (*MediaCallback)( int handlerId );


	typedef int (*MediaCallback__1)(const std::string &url, int *handlerId, int mediaType);
	typedef int (*MediaCallback__2)( int x, int y, int w, int h,int handlerId);
	typedef int (*MediaCallback__3)( int handlerId, int time);
	void loadMedia(const std::string &ur, int *handlerId, int mediaType);
	void playMedia(int handlerId);
	void setMediaTime(int handlerId, int time);
	void stopMedia(int handlerId);
	void closeMedia(int handlerId);
	void freezeMedia(int handlerId);
	void resumeMedia(int handlerId);
	void resizeMedia(int x, int y, int w, int h,int handlerId);
	void onMediaStop( int handlerId );

	// Sound Properties
	void setSoundLevel(unsigned int value, int handlerId);
	int getSoundLevel(const int handlerId);

	void setBalanceLevel(unsigned int value, int handlerId);
	int getBalanceLevel(const int handlerId);

	void setTrebleLevel(unsigned int value, int handlerId);
	int getTrebleLevel(const int handlerId);

	void setBassLevel(unsigned int value, int handlerId);
	int getBassLevel(const int handlerId);

	// Set callbacks
	void setOnLoadMedia(MediaCallback__1 cb)	{ loadMediaCallback = cb; }
	void setOnPlayMedia(MediaCallback cb)		{ playMediaCallback = cb; }
	void setOnMediaTime(MediaCallback__3 cb)	{ setMediaTimeCallback = cb; }
	void setOnStopMedia(MediaCallback cb)		{ stopMediaCallback = cb; }
	void setOnCloseMedia(MediaCallback cb)		{ closeMediaCallback = cb; }
	void setOnFreezeMedia(MediaCallback cb)		{ freezeMediaCallback = cb; }
	void setOnResumeMedia(MediaCallback cb)		{ resumeMediaCallback = cb; }
	void setOnResizeMedia(MediaCallback__2 cb)	{ resizeMediaCallback = cb; }



	/**
	 *
	 * operation = 0 - GET operation
	 *             1 - SET opetation
	 * handlerId = file handler ID
	 * property  = which property to get or set
	 * value     = which value to get or set
	 * return SUCCESS or FAILURE
	 */
	typedef signed char SINT8;
	typedef SINT8 (*MediaPropertyCallback)( const int op, const int handlerId, const int property, unsigned int *value );
	void setOnMediaPropertyChanged(MediaPropertyCallback cb) { mediaPropertyCB = cb; }


private:
 	MediaCallback__1 loadMediaCallback;
	MediaCallback playMediaCallback;
	MediaCallback__3 setMediaTimeCallback;
	MediaCallback stopMediaCallback;
	MediaCallback closeMediaCallback;
	MediaCallback freezeMediaCallback;
	MediaCallback resumeMediaCallback;
	MediaCallback__2 resizeMediaCallback;

	MediaPropertyCallback mediaPropertyCB;

protected:
	typedef std::map<std::string,std::string> DownloadCache;
	Player *addPlayer( Player *player );
	void enqueue( const boost::function<void (void)> &func );
	bool tryDownload( const std::string &url, std::string &file );

private:
	System *_sys;
	canvas::System *_canvas;
	std::vector<Player *> _players;
	DownloadCache _downloadCache;
	canvas::Size _embeddedSize;
};

}

