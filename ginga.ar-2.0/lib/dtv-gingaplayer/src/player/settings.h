#pragma once

#include <string>
#include <util/cfg/cfg.h>
#include <map>

namespace player {
namespace settings {

class Module;

//!!! Do not change the order
namespace tokens {
#define TOKEN_LIST(fnc) \
	fnc( system, "system" ) \
	fnc( user, "user" ) \
	fnc( defaults, "default" ) \
	fnc( service, "service" ) \
	fnc( si, "si" ) \
	fnc( channel, "channel" ) \
	fnc( shared, "shared" ) \
	fnc( ncl, "ncl" ) \
	fnc( GingaNCL, "GingaNCL" ) \
	fnc( screenSize, "screenSize" ) \
	fnc( screenGraphicSize, "screenGraphicSize" ) \
	fnc( audioType, "audioType" ) \
	fnc( returnBitRate, "returnBitRate" ) \
	fnc( devNumber, "devNumber" ) \
	fnc( classType, "classType" ) \
	fnc( info, "info" ) \
	fnc( GingaJ, "GingaJ" ) \

#define DO_ENUM_TOKEN(t,s) t,
enum type {
	unknown = 0,
	TOKEN_LIST(DO_ENUM_TOKEN)
	LAST
};
#undef DO_ENUM_TOKEN
}

class Settings {
public:
	Settings();
	virtual ~Settings();

	void init();
	void setProperty( const std::string &name, const std::string &value, bool fromNCL=true );
	void setProperty( int group, const std::string &varName,  const std::string &value, bool fromNCL=true );
	const std::string getProperty( const std::string &name );
	int getPropertyOrGroup(int group, const std::string &varName, std::string &res, bool checkGlobal);
	void getIndexProperty(int group, int index, std::string &res);
	inline float getFocusBorderTransparency() {return  _focusBorderTransparency; }
	inline int getFocusBorderWidth() { return _focusBorderWidth; }

	typedef boost::function<void (void)> UpdatePlayersCallback;
	void setPlayersImageUpdateCallback( UpdatePlayersCallback &func );
	typedef int (*GetNativePropertyCallback)( char * key, char * value );
	void setOnGetNativeProperty(GetNativePropertyCallback cb) { getNativePropertyCallback = cb; }
	void onGetNativeProperty( char * key, char * value );
	GetNativePropertyCallback getNativePropertyCallback;	
private:
	float _focusBorderTransparency;
	int _focusBorderWidth;
	std::map<std::string, std::string> _si;
	std::map<std::string, std::string> _defaults;
	std::map<std::string, std::string> _service;
	std::map<std::string, std::string> _channel;
	std::map<std::string, std::string> _shared;
	std::map<std::string, std::string> _global;
	UpdatePlayersCallback _updatePlayers;
};

}
}
