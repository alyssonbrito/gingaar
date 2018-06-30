#include "player.h"
#include "device.h"
#include "property/propertyimpl.h"
#include "property/compositeproperty.h"
#include "player/timelinetimer.h"
#include <util/log.h>
#include <util/mcr.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <algorithm>

namespace player {

namespace fs = boost::filesystem;

Player::Player( Device *dev )
{
	_dev         = dev;
	_initialized = false;
	_playing     = false;
	_pause       = false;
	_schema      = schema::unknown;
	_timer       = new TimeLineTimer( dev->systemPlayer() );
	_duration    = 0;
}

Player::~Player()
{
	assert(!_initialized);
	BOOST_FOREACH( const Properties::value_type &value, _properties ) {
		delete value.second;
	}
	delete _timer;
}

bool Player::initialized() const {
	return _initialized;
}

bool Player::initialize() {
	LDEBUG("player", "Initialize");

	bool result=true;
	if (!initialized()) {
		result=init();
	}

	if (result) {
		registerProperties();
		_initialized=true;
	}

	return result;
}

bool Player::init() {
	return true;
}

void Player::finalize() {
	LDEBUG("player", "Finalize");
	if (initialized()) {
		stop();
		fin();
		_initialized=false;
	}
}

void Player::fin() {
}

//	Timers
void Player::createTimer( util::DWORD ms, const TimerCallback &fnc ) {
	_timer->addTimer( ms, fnc );
}

void Player::onStopped(const OnStatusChanged &fnc) {
	_onStopped = fnc;
}

void Player::onStarted(const OnStatusChanged &fnc){
	_onStarted = fnc;
}

void Player::onPaused(const OnPauseStatusChanged &fnc){
	_onPaused = fnc;
}

bool Player::isPlaying() const {
	return _playing;
}

bool Player::isPaused() const {
	return _pause;
}

bool Player::play() {
	play(0);
}

bool Player::play(double time) {
	LDEBUG("player", "play");
	assert(initialized());

	//	Stop if playing
	stop();

	//	Mark all properties modified
	markModified();

	//	Can play?
	if (canPlay()) {
		//	Initialize src
		url::parse( _url, _schema, _body );

		_playing = startPlay(time);
		if (_playing) {
			//  LG Electronics: #4307
			//  Timer should start before script running.
			//  Content examples are Idolos_2012.
			_timer->start();
			applyChanges();
		}
		if(!_onStarted.empty()){
			_onStarted();
		}
	}
	return _playing;
}

bool Player::canPlay() const {
	bool result=true;
	LDEBUG("player", "Can play begin");
	BOOST_FOREACH( const Properties::value_type &value, _properties ) {
		if (!value.second->canStart()) {
			LWARN("player", "property can't start, name=%s", property::getPropertyName(value.first));
			result=false;
			break;
		}
	}
	LDEBUG("player", "Can play: result=%d", result );
	return result;
}

void Player::pausePlay( bool /*pause*/ ) {
}

void Player::abortPlay() {
	stop();
}

void Player::pause( bool pause ) {
	LDEBUG("player", "Pause: pause=%d", pause );
	if (isPlaying() && _pause != pause) {
		pausePlay( pause );
		if (!pause) {
			applyChanges();
			_timer->unpause();
		}
		else {
			_timer->pause();
		}
		_pause = pause;
		if(!_onPaused.empty()){
			_onPaused( pause );
		}
	}
}

void Player::stop( void ) {
	assert(initialized());

	if (isPlaying()) {
		LDEBUG("player", "Stop");
		stopPlay();
		_timer->stop();
		_playing = false;
		_pause   = false;
		if(!_onStopped.empty()){
			_onStopped();
		}
	}
}

void Player::abort( void ) {
	assert(initialized());

	if (isPlaying()) {
		LDEBUG("player", "abort");
		abortPlay();
	}
}

void Player::onStop() {
		if(!_onStopped.empty()){
			_onStopped();
		}
}

void Player::markModified() {
	BOOST_FOREACH( const Properties::value_type &value, _properties ) {
		value.second->markModified();
	}
}

void Player::applyChanges() {
	bool needRefresh=false;
	bool changed=false;
	LDEBUG("player", "Apply changes");

	//	Apply changes on all properties
	BOOST_FOREACH( const Properties::value_type &value, _properties ) {
		Property *prop = value.second;
		if (prop->changed()) {
			needRefresh |= prop->apply();
			changed=true;
		}
	}

	if (changed) {
		apply( needRefresh );
	}
}

void Player::apply( bool needRefresh ) {
	//	If need refresh the player, do work
	if (needRefresh) {
		beginRefresh();
		refresh();
		endRefresh();
	}

	onPropertyChanged();
}

void Player::beginRefresh() {
}

void Player::refresh() {
}

void Player::endRefresh() {
}

void Player::onPropertyChanged() {
}

//	Callback initialization
void Player::setEventCallback( const event::Callback &/*callback*/ ) {
}

void Player::setInputEventCallback( const event::InputCallback &/*callback*/ ) {
}

//	Properties
std::string Player::rootPath() const {
	if (_schema == schema::file) {
		return fs::path(_body).branch_path().string();
	}
	return _body;
}

util::DWORD Player::uptime() const {
	return _timer->uptime();
}

bool Player::isApplication() const {
	return false;
}

bool Player::isVisible() const {
	return false;
}

bool Player::checkUrl( const std::string &url ) {
	schema::type schema;
	std::string body;
	bool result=false;

	//	Parse url
	url::parse( url, schema, body );
	if (schema != schema::unknown) {
		result=supportSchemma( schema );
		
#if 0 //Check why this change, with this video resize does not work
		if (schema == schema::http || schema == schema::file || schema == schema::https)
		{
			result = true;
		}
		else
		{
			result = false;
		}		
#endif
		
		if (result) {
			if (schema == schema::file) {
				result=check::fileExists( body );
			}
			else if (!supportRemote() && (schema == schema::http || schema == schema::https)) {
				std::string fileDownloaded;
				result = false;
				if (_dev->download( url, fileDownloaded )) {
					std::string newUrl = "file://" + fileDownloaded;
					if (checkUrl( newUrl )) {
						_url = newUrl;
						result = true;
					}
				}
			}
		} else if (schema == schema::mirror) {
			result = true;
		} else if (schema == schema::sbtvd) {
			result = true;
		}
	}
	return result;
}

bool Player::supportSchemma( schema::type sch ) const {
	return sch == schema::http || sch == schema::file || sch == schema::https;
}

bool Player::supportRemote() const {
	return false;
}

schema::type Player::schema() const {
	return _schema;
}

const std::string &Player::body() const {
	return _body;
}

const std::string &Player::url() const {
	return _url;
}

const std::string &Player::type() const {
	return _type;
}

void Player::registerProperties() {
	{	//	Source property
		PropertyImpl<std::string> *prop = new PropertyImpl<std::string>( true, _url );
		prop->setCheck( boost::bind(&Player::checkUrl,this,_1) );
		addProperty( property::type::src, prop );
	}

	{	//	Type property
		PropertyImpl<std::string> *prop = new PropertyImpl<std::string>( true, _type );
		addProperty( property::type::type, prop );
	}

	{	//	Explicit duration
		PropertyImpl<int> *prop = new PropertyImpl<int>( false, _duration );
		prop->setCheck( boost::bind( std::greater_equal<int>(), _1, 0) );
		prop->setApply( boost::bind(&Player::applyDuration,this) );
		addProperty( property::type::explicitDur, prop );
	}
}

void Player::addProperty( property::type::Type type, Property *prop ) {
	Property *old = getProperty( type );
	if (old) {
		CompositeProperty *tmp = new CompositeProperty();
		tmp->add( old );
		tmp->add( prop );
		_properties[type] = tmp;
	}
	else {
		_properties[type] = prop;
	}
}

Property *Player::getProperty( property::type::Type type ) const {
	Properties::const_iterator it=_properties.find( type );
	if (it != _properties.end()) {
		return (*it).second;
	}
	return NULL;
}

Property *Player::getProperty( const std::string &name ) const {
	return getProperty( property::getProperty( name.c_str() ) );
}

//	Explicit duration
void Player::applyDuration() {
	if (_duration > 0) {
		_timer->addTimer( _duration, boost::bind(&Player::onDurationExpired,this,_duration) );
	}
}

void Player::onDurationExpired( int dur ) {
	if (dur == _duration) {
		stop();
	}
}

//	Getters
Device *Player::device() const {
	return _dev;
}

canvas::Surface* Player::getSurface() const {
	return NULL;
}

void Player::dispatchKey( util::key::type key, bool isUp ) {
}

void Player::updateImage() {
}

int Player::getMediaType()
{
	int mediaType = 0; /* MEDIA_WITH_SERVICE */
	schema::type schema;
	std::string body;
	type::type typeOfPlayer;

	url::parse( url(), schema, body );

	typeOfPlayer = type::fromBody( body );

	boost::filesystem::path file( body );
	std::string extension = file.extension().string();
	size_t pos = extension.find( "." );

	if (pos != std::string::npos) {
		extension = extension.substr( pos + 1 );
	}

	std::string ext = extension.c_str();

	if (ext == "mp4" || ext == "mpg4") {
		if (typeOfPlayer == type::audio)
			mediaType = 4; /* AUDIO_WITH_MP4 */
	}
	else if (ext == "mp2" || ext == "mpg") {
		if (typeOfPlayer == type::audio)
			mediaType = 5; /* AUDIO_WITH_MPEG */
		else if (typeOfPlayer == type::video)
			mediaType = 2; /* VIDEO_WITH_MPEG */
	}
	else if (ext == "ts") {
		if (typeOfPlayer == type::audio)
			mediaType = 6; /* AUDIO_WITH_SBTVD_TS */
		else if (typeOfPlayer == type::video)
			mediaType = 1; /* VIDEO_WITH_SBTVD_TS */
	}
	else if (ext == "mng") {
		if (typeOfPlayer == type::video)
			mediaType = 3; /* VIDEO_WITH_MNG */
	}

	return mediaType;
}

bool Player::instanceOf( std::string s ) {
	return (typeSet.find( s ) != typeSet.end());
}
}

