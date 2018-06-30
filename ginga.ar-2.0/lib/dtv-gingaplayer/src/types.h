#pragma once

#include <string>

#define SCHEMA_TYPES_LIST(fnc)	\
	fnc(unknown,unknown) \
	fnc(file,file) \
	fnc(http,http) \
	fnc(https,https) \
	fnc(rtsp,rtsp) \
	fnc(rtp,rtp) \
	fnc(mirror,ncl-mirror) \
	fnc(sbtvd,sbtvd-ts)

#define PLAYER_TYPES_LIST(fnc) \
	fnc(ncl) \
	fnc(image) \
	fnc(text) \
	fnc(lua) \
	fnc(video) \
	fnc(audio) \
	fnc(mng) \
	fnc(html) \
	fnc(xlet) \
	fnc(anime) \
	fnc(mirror)
	
#define MIME_TYPES_LIST(fnc) \
	fnc(html,  htm,  TEXT/HTML) \
	fnc(html,  html, TEXT/HTML) \
	fnc(text,  txt,  TEXT/PLAIN) \
	fnc(text,  css,  TEXT/CSS) \
	fnc(text,  xml,  TEXT/XML) \
	fnc(text,  srt,  TEXT/SRT) \
	fnc(image, bmp,  IMAGE/BMP) \
	fnc(image, png,  IMAGE/PNG) \
	fnc(image, gif,  IMAGE/GIF) \
	fnc(image, jpg,  IMAGE/JPEG) \
	fnc(image, jpeg, IMAGE/JPEG) \
	fnc(audio, wav,  AUDIO/BASIC) \
	fnc(audio, ac3,  AUDIO/AC3) \
	fnc(audio, mpa,  AUDIO/MPA) \
	fnc(audio, mp3,  AUDIO/MP3) \
	fnc(audio, mp4,  AUDIO/MP4) \
	fnc(audio, mp2,  AUDIO/MP2) \
	fnc(audio, mpg,  AUDIO/MPG) \
	fnc(audio, mpg,  AUDIO/MPEG) \
	fnc(audio, mp4,  AUDIO/MPEG4) \
	fnc(audio, mpg4, AUDIO/MPEG4) \
	fnc(video, mp4,  VIDEO/MPEG4) \
	fnc(video, mpg,  VIDEO/MPEG) \
	fnc(video, mpv,  VIDEO/MPV) \
	fnc(video, ts,   VIDEO/MPEG) \
	fnc(video, mts,  VIDEO/MPEG) \
	fnc(video, mpeg, VIDEO/MPEG) \
	fnc(video, mov,  VIDEO/QUICKTIM) \
	fnc(video, avi,  VIDEO/X-MSVIDEO) \
	fnc(mng,   mng,  VIDEO/X-MNG) \
	fnc(lua,   lua,  APPLICATION/X-GINGA-NCLUA) \
	fnc(lua,   lua,  APPLICATION/X-NCL-NCLUA) \
	fnc(xlet,  class,APPLICATION/X-GINGA-NCLET) \
	fnc(xlet,  class,APPLICATION/X-NCL-NCLET) \
	fnc(xlet,  jar,  APPLICATION/X-GINGA-NCLET) \
	fnc(xlet,  jar,  APPLICATION/X-NCL-NCLET) \
	fnc(ncl,   ncl,  APPLICATION/X-GINGA-NCL) \
	fnc(ncl,   ncl,  APPLICATION/X-NCL-NCL)

namespace player {

namespace schema {
#define DO_ENUM_SCHEMA_TYPES(n,t) n,
enum type {
	SCHEMA_TYPES_LIST(DO_ENUM_SCHEMA_TYPES)
	LAST
};
#undef DO_ENUM_SCHEMA_TYPES

const char *getName( type schType );

}	//	namespace schema

namespace type {

#define DO_ENUM_PLAYER_TYPES(t) t,
enum type {
	unknown,
	PLAYER_TYPES_LIST(DO_ENUM_PLAYER_TYPES)
	LAST
};
#undef DO_ENUM_PLAYER_TYPES

const char *getName( type player );
const char *getExtensionFromMime( const std::string &mimeType );
type fromBody( const std::string &body );
type fromMime( const std::string &mimeType );
type get( schema::type schema, const std::string &body, const std::string &mimeType );

} //	namespace schema

namespace url {

//	Parse url into schema and body
void parse( const std::string &url, schema::type &schema, std::string &body );

}	//	namespace url

}

