#include "device.h"
#include "system.h"
#include "input/manager.h"
#include "generated/config.h"
#include "player.h"
#include "player/luaplayer.h"
#include "player/imageplayer.h"
#include "player/animeplayer.h"
#include "player/textplayer.h"
#include "player/soundplayer.h"
#include "player/videoplayer.h"
#include "player/htmlplayer.h"
#include "player/mngplayer.h"
#include "player/nclplayer.h"
#include "player/mirrorplayer.h"
#include "player/xletplayer.h"
#include <canvas/player.h>
#include <canvas/canvas.h>
#include <canvas/system.h>
#include <util/log.h>
#include <canvas/window.h>
#include <util/cfg/cfg.h>
#include <util/mcr.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <curl/curl.h>

#define EXTERNAL_RESOURCE_TIMEOUT 5

#include <ginga_api.h>

namespace player {

namespace fs = boost::filesystem;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

int Device::_numberOfMNGPlayers = 0;

#if 0
Device::Device( System *sys )
	: _sys(sys),_embeddedSize(0,0),setPropertyCallback(NULL),getPropertyCallback(NULL)
{
	_canvas = canvas::System::create();
}
#else
Device::Device( System *sys )
	: _sys(sys), _embeddedSize(0, 0)
{
	_canvas = canvas::System::create();


	// Callback should be initialized with NULL
	fetchFilesCallback = NULL;
	//getPropertyCallback = NULL;
	//setPropertyCallback = NULL;
	_reserveKeysCallback = NULL;
	_beforeInitCallback = NULL;
	loadMediaCallback = NULL;
	playMediaCallback = NULL;
	stopMediaCallback = NULL;
	closeMediaCallback = NULL;
	freezeMediaCallback = NULL;
	resumeMediaCallback = NULL;
	resizeMediaCallback = NULL;
	mediaPropertyCB = NULL;
}
#endif

Device::~Device()
{
	if (_players.size()) {
		LWARN("Device", "not all players destroyed: size=%d", _players.size());
	}

	delete _canvas;

	BOOST_FOREACH( const DownloadCache::value_type &val, _downloadCache ) {
		try {
			fs::remove( val.second );
		} catch (...) {
		}
	}
}

void Device::setEmbeddedSize( canvas::Size size ) {
	_embeddedSize = size;
}

System *Device::systemPlayer() const {
	return _sys;
}

//	Getters
canvas::System *Device::system() const {
	return _canvas;
}

bool Device::haveVideo() const {
	return true;
}

bool Device::haveAudio() const {
	return true;
}

canvas::Size Device::size() const {
	util::cfg::PropertyNode &root = util::cfg::get("gui.window.size");
	canvas::Size size;
	size.w = root.get<int>( "width");
	size.h = root.get<int>( "height");
	if (_embeddedSize.w > 0 && _embeddedSize.h > 0) {
		size.w = _embeddedSize.w;
		size.h = _embeddedSize.h;
	}
	return size;
}

//	Player
Player *Device::addPlayer( Player *player ) {
	//	Initialize player
	if (player->initialize()) {
		_players.push_back( player );
		return player;
	}
	else {
		LWARN("Device", "cannot initialize player");
		delete player;
		return NULL;
	}
}

Player *Device::create( enum type::type typeOfPlayer, bool isAlt ) {
	Player *player;

	LINFO("Device::create", "typeOfPlayer = %s", type::getName(typeOfPlayer));

	if (isAlt) {
		if (typeOfPlayer == type::audio || typeOfPlayer == type::video) {
			return NULL;
		}
	}

	switch (typeOfPlayer) {
		case type::audio:
			player = new SoundPlayer( this );
			break;
		case type::image:
			player = new ImagePlayer( this );
			break;
		// case type::anime:
		// 	player = new AnimePlayer( this );
		// 	break;
		case type::lua:
			player = new LuaPlayer( this );
			break;
		case type::text:
			player = new TextPlayer( this );
			break;
		case type::video:
			player = new VideoPlayer( this );
			break;
		case type::html:
			player = new HtmlPlayer( this );
			break;
		case type::mng:
			if (_numberOfMNGPlayers >= MAX_MNG_PLAYERS_NUMBER) {
				LWARN("Device", "cannot create mng player, the limit was reached: %d", MAX_MNG_PLAYERS_NUMBER);
				return NULL;
			}
			_numberOfMNGPlayers++;
			player = new MngPlayer( this );
			break;
		case type::ncl:
			player = new NclPlayer( this );
			break;
		case type::mirror:
			player = new MirrorPlayer( this );
			break;
		case type::xlet:
			player = new XletPlayer( this );
			break;
		default: {
			LWARN("Device", "cannot create player: type=%s", type::getName(typeOfPlayer) );
			return NULL;
		}
	};

	if (isAlt) {
		return player;
	}

	//	Add player to device
	return addPlayer( player );
}

Player *Device::create( const std::string &url, const std::string &mime, bool isAlt  ) {
	schema::type schema;
	std::string body;
	type::type typeOfPlayer;

	url::parse( url, schema, body );

	// LGESP: Change Request
	// Description: Before creating a valid player, we need to fetch file
	// from Carrousel to Temporary directory.
	onFetchFilesFromCarousel(url);

	//	Check schema
	if (schema == schema::file || schema == schema::http || schema == schema::https) {
		typeOfPlayer=type::get( schema, body, mime );
	}
	else if (schema == schema::sbtvd
		|| schema == schema::rtsp
		|| schema == schema::rtp)
	{
		if (body == "audio")
		{
			typeOfPlayer=type::audio;
		}
		else
		{
			typeOfPlayer=type::video;
		}
	}
	else if (schema == schema::mirror)
	{
		typeOfPlayer=type::mirror;
	}
	else {
		LWARN("Device", "schema not supported: schema=%s", schema::getName(schema) );
		return NULL;
	}

	LINFO("Device", "Create player: type=%s, schema=%s, body=%s, mime=%s",
		type::getName(typeOfPlayer), schema::getName(schema), body.c_str(), mime.c_str() );

	//	Setup src and type
	Player *player=create( typeOfPlayer, isAlt );
	if (player && !isAlt) {
		player->setProperty( "src", url );
		player->setProperty( "type", mime );
		if (typeOfPlayer == type::video) {
			player->setProperty( "plan", "video" );
		}
	}
	return player;
}

void Device::destroy( Player *player ) {
	LDEBUG("Device", "Destroy player: player=%p", player);
	assert(player);

	std::vector<Player *>::iterator it=std::find(
		_players.begin(), _players.end(), player );
	if (it != _players.end()) {
		if (type::fromBody(player->body()) == type::mng) {
			_numberOfMNGPlayers--;
		}
		player->finalize();
		delete player;
		_players.erase( it );
	}
	else {
		LWARN("Device", "player is not member of this device");
	}
}

//	Start/stop
void Device::start() {
}

void Device::stop() {

	Player *player;

	for (std::vector<Player *>::iterator it=_players.begin();it!=_players.end(); it++)
	{
		player = (Player *)*it;
		
		if (player->instanceOf( "SoundPlayer" ))
		{
			player->stop();
		}
	}
}

//	Initialize
bool Device::initialize( canvas::Surface* surface ) {
	system()->setInputCallback( boost::bind(&player::System::dispatchKey,_sys,_1,_2) );
	return system()->initialize( surface );
}

void Device::finalize() {
	system()->finalize();
}

void Device::exit() {
	_sys->dispatchKey( ::util::key::exit, true );
}

//	Keys
void Device::onReserveKeys( const ::util::key::Keys &/*keys*/ ) {
	if (_reserveKeysCallback != NULL) {
			(*_reserveKeysCallback)(1);
		}
}

void Device::dispatchKey( util::key::type keyValue, bool isUp ) {
	_sys->dispatchKey( keyValue, isUp );
}

bool Device::injectKey( util::key::type keyValue, bool isUp ) {
	return system()->window()->sendKey(keyValue, isUp);
}

void Device::dispatchEditingCommand( connector::EditingCommandData *event ) {
	_sys->dispatchEditingCommand( event );
}

//	External resources
bool Device::download( const std::string &url, std::string &newFile ) {
	bool result;

	DownloadCache::const_iterator it=_downloadCache.find( url );
	if (it != _downloadCache.end()) {
		newFile=(*it).second;
		result=true;
	}
	else {
		result = tryDownload( url, newFile );
		if (result) {
			_downloadCache[url] = newFile;
		}
	}
	return result;
}

bool Device::tryDownload( const std::string &url, std::string &file ) {
	CURL *curl;
	bool result=false;

	curl = curl_easy_init();
	if (curl) {
		//	Set basic options
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, EXTERNAL_RESOURCE_TIMEOUT);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		//	CURLOPT_FAILONERROR - request failure on HTTP response >= 400
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

		//	HTTPS options
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		//	Setup file name (temporary, with unknown extension)
		fs::path fsTemp = fs::temp_directory_path();
		fsTemp /= "ginga_player_file%%%%%%%%";

		//	Add temporary extension
		std::string fileToDownload = fs::unique_path(fsTemp).string();
		fileToDownload += ".unknown";

		//	Open file
		FILE *pagefile = fopen(fileToDownload.c_str(), "wb");
		if (pagefile) {
			//	Setup file
			curl_easy_setopt(curl, CURLOPT_FILE, pagefile);

			//	Perform operation
			result = curl_easy_perform(curl) == CURLE_OK;

			//	Close file
			fclose(pagefile);

			//	Check result
			if (result) {
				//	Check content type
				char *ct;
				result = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct) == CURLE_OK && ct;
				if (result) {
					const char *ext=type::getExtensionFromMime( ct );
					if (!ext) {
						schema::type schema;
						std::string body;
						url::parse( url, schema, body );
						fs::path filePath = body;
						std::string extension = filePath.extension().string();
						size_t pos=extension.find( "." );
						if (pos != std::string::npos) {
							extension = extension.substr( pos + 1 );
						}
						ext = extension.c_str();
					}
					if (ext) {
						//	Fix file extension
						file = fs::unique_path(fsTemp).string();
						file += ".";
						file += ext;

						//	Rename file
						fs::rename( fileToDownload, file );
						LDEBUG( "Device", "File download: name=%s, mime=%s\n",
							file.c_str(),  ct );

						result=true;
					}
					else {
						result=false;
					}
				}
			}
		}

		//	Cleanup
		curl_easy_cleanup(curl);
	}

	if (!result) {
		LERROR("Device", "cannot load external resource: url=%s", url.c_str());
	}

	return result;
}

//	Factory methods
canvas::MediaPlayer *Device::createMediaPlayer() {
	return system()->player()->create( system() );
}

canvas::WebViewer *Device::createWebViewer( canvas::Surface *surface ) {
	return system()->createWebViewer( surface );
}

canvas::MngViewer *Device::createMngViewer( canvas::Surface *surface ) {
	return system()->createMngViewer( surface );
}

//	Aux
void Device::enqueue( const boost::function<void (void)> &func ) {
	_sys->enqueue( func );
}


// LGESP: Change Request
// Description: File is copied from Carrousel to Temporary directory only
// on demand. This listener is invoked when "Player *Device::create(const std::
// string &url, const std::string &mime)" is invoked.
void Device::onFetchFile(const std::string &url) {
	if (fetchFilesCallback != NULL) {
	  LDEBUG("Device", "[Device::%s] Fetching file from url = [%s]", __FUNCTION__, url.c_str());
	  (*fetchFilesCallback)(url);
	}
}
void Device::loadMedia(const std::string &url, int *handlerId, int mediaType) {
	if (loadMediaCallback != 0) {
		LDEBUG("Device", "[Device::%s] Load Media from url = [%s] with media type [%d]", __FUNCTION__, url.c_str(), mediaType);
		(*loadMediaCallback)(url, handlerId, mediaType);
	}
}
void Device::playMedia(int handlerId) {
	if (playMediaCallback != 0) {
		LDEBUG("Device", "[Device::%s::%d]", __FUNCTION__, __LINE__);
		(*playMediaCallback)(handlerId);
	}
}
void Device::setMediaTime(int handlerId, int time) {
	if (setMediaTimeCallback != 0) {
		LDEBUG("Device", "[Device::%s::%d]", __FUNCTION__, __LINE__);
		(*setMediaTimeCallback)(handlerId, time);
	}
}
void Device::stopMedia(int handlerId) {
	if (stopMediaCallback != 0) {
		LDEBUG("Device", "[Device::%s::%d]", __FUNCTION__, __LINE__);
		(*stopMediaCallback)(handlerId);
	}
}
void Device::closeMedia(int handlerId) {
	if (closeMediaCallback != 0) {
		LDEBUG("Device", "[Device::%s::%d]", __FUNCTION__, __LINE__);
		(*closeMediaCallback)(handlerId);
	}
}
void Device::freezeMedia(int handlerId) {
	if (freezeMediaCallback != 0) {
		LDEBUG("Device", "[Device::%s::%d]", __FUNCTION__, __LINE__);
		(*freezeMediaCallback)(handlerId);
	}
}
void Device::resumeMedia(int handlerId) {
	if (resumeMediaCallback != 0) {
		LDEBUG("Device", "[Device::%s::%d]", __FUNCTION__, __LINE__);
		(*resumeMediaCallback)(handlerId);
	}
}
void Device::resizeMedia(int x, int y, int w, int h, int handlerId) {
	if (resizeMediaCallback != 0) {
		LDEBUG("Device", "[Device::%s::%d]", __FUNCTION__, __LINE__);
		(*resizeMediaCallback)(x,y,w,h,handlerId);
	}
}

void Device::setSoundLevel(unsigned int value, int handlerId) {
	if (mediaPropertyCB != 0) {
		LDEBUG("Device", "[Device::%s::%d] Handler Id = %d [0x%x], Value = [%d]", __FUNCTION__, __LINE__, handlerId, handlerId, value);
		(*mediaPropertyCB)(0 /* SET */, handlerId, 0 /* SOUND LEVEL */, &value);
	}
}

int Device::getSoundLevel(const int handlerId) {
	unsigned int value = 0;

	if (mediaPropertyCB != 0) {
		LDEBUG("Device", "[Device::%s::%d] Handler Id = %d [0x%x]", __FUNCTION__, __LINE__, handlerId, handlerId);
		(*mediaPropertyCB)(1 /* GET */, handlerId, 0 /* SOUND LEVEL */, &value);
	}

	LDEBUG("Device", "[Device::%s::%d] Value = [%d]", __FUNCTION__, __LINE__, value);
	return value;
}

void Device::setBalanceLevel(unsigned int value, int handlerId) {
	if (mediaPropertyCB != 0) {
		LDEBUG("Device", "[Device::%s::%d] Handler Id = %d [0x%x], Value = [%d]", __FUNCTION__, __LINE__, handlerId, handlerId, value);
		(*mediaPropertyCB)(0 /* SET */, handlerId, 1 /* BALANCE LEVEL */, &value);
	}
}

int Device::getBalanceLevel(const int handlerId) {
	unsigned int value = 0;

	if (mediaPropertyCB != 0) {
		LDEBUG("Device", "[Device::%s::%d] Handler Id = %d [0x%x]", __FUNCTION__, __LINE__, handlerId, handlerId);
		(*mediaPropertyCB)(1 /* GET */, handlerId, 1 /* BALANCE LEVEL */, &value);
	}

	LDEBUG("Device", "[Device::%s::%d] Value = [%d]", __FUNCTION__, __LINE__, value);
	return value;
}

void Device::setTrebleLevel(unsigned int value, int handlerId) {
	if (mediaPropertyCB != 0) {
		LDEBUG("Device", "[Device::%s::%d] Handler Id = %d [0x%x], Value = [%d]", __FUNCTION__, __LINE__, handlerId, handlerId, value);
		(*mediaPropertyCB)(0 /* SET */, handlerId, 2 /* TREBLE LEVEL */, &value);
	}
}

int Device::getTrebleLevel(const int handlerId) {
	unsigned int value = 0;

	if (mediaPropertyCB != 0) {
		LDEBUG("Device", "[Device::%s::%d] Handler Id = %d [0x%x]", __FUNCTION__, __LINE__, handlerId, handlerId);
		(*mediaPropertyCB)(1 /* GET */, handlerId, 2 /* TREBLE LEVEL */, &value);
	}

	LDEBUG("Device", "[Device::%s::%d] Value = [%d]", __FUNCTION__, __LINE__, value);
	return value;
}

void Device::setBassLevel(unsigned int value, int handlerId) {
	if (mediaPropertyCB != 0) {
		LDEBUG("Device", "[Device::%s::%d] Handler Id = %d [0x%x], Value = [%d]", __FUNCTION__, __LINE__, handlerId, handlerId, value);
		(*mediaPropertyCB)(0 /* SET */, handlerId, 3 /* BASS LEVEL */, &value);
	}
}

int Device::getBassLevel(const int handlerId) {
	unsigned int value = 0;

	if (mediaPropertyCB != 0) {
		LDEBUG("Device", "[Device::%s::%d] Handler Id = %d [0x%x]", __FUNCTION__, __LINE__, handlerId, handlerId);
		(*mediaPropertyCB)(1 /* GET */, handlerId, 3 /* BASS LEVEL */, &value);
	}

	LDEBUG("Device", "[Device::%s::%d] Value = [%d]", __FUNCTION__, __LINE__, value);
	return value;
}


/*void Device::onGetProperty(const std::string &key,std::string *value){
        LDEBUG("Device", "[Device::%s::%d] ", __FUNCTION__, __LINE__);
        if (getPropertyCallback != 0) {
                LDEBUG("Device", "[Device::%s::%d]", __FUNCTION__, __LINE__);
                (*getPropertyCallback)(key,value);
        }
}
void Device::onSetProperty(const std::string &key,const std::string &value)
{
        LDEBUG("Device", "[Device::%s::%d]", __FUNCTION__, __LINE__);
        if (setPropertyCallback != 0) {
                LDEBUG("Device", "[Device::%s::%d]", __FUNCTION__, __LINE__);
                (*setPropertyCallback)(key,value);
        }

}*/

void Device::onMediaStop( int handlerId ) {
	LDEBUG("Device", "onMediaStop: handlerId=%d", handlerId);
	Player *player;

	for (std::vector<Player *>::iterator it=_players.begin();it!=_players.end(); it++)
	{
		player = (Player *)*it;
		//player->stop();
		player->onStop();
	}
}

}
