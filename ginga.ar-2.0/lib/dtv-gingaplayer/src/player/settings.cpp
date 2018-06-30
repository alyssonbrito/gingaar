 #include "settings.h"
#include <canvas/color.h>
#include <util/log.h>
#include <util/string.h>
#include <util/cfg/cfg.h>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <util/cfg/configregistrator.h>

REGISTER_INIT_CONFIG(channel) {
	root().addNode( "channel" )
		.addValue( "keyCapture", "", "interactive_keys" );
}

REGISTER_INIT_CONFIG(quirks) {
	root().addNode( "quirks" )
		.addValue( "windowQuirkDoNotBlink", "This quirk enables LGE workaround to overcome platform initialization blink effect", true )
		.addValue( "internalUnitTests", "This quirk enables LGE workaround to support internal Unit Tests", false);
}

static bool checkFloat( const std::string &value, float &f ) {
	std::string v = value;
	boost::trim( v );
	bool percentual = v.find( "%" ) == v.size() - 1;
	if (percentual) {
		boost::trim_right_if( v, boost::is_any_of( "%" ) );
	}
	try {
    f = boost::lexical_cast<float>( v );
	} catch (boost::bad_lexical_cast &) {
    LWARN("Settings", "Conversion error[%s]",v.c_str());
		return false;
	}
	f = percentual ? f/100 : f;
	return (0.0 <= f) && (f <= 1.0);
}

typedef signed char SINT8;
namespace player {
namespace settings {

static bool _initStatic = true;
static std::vector<std::string> _system_returnBitRate;
static std::vector<std::string> _system_screenSize;
static std::vector<std::string> _system_screenGraphicSize;
static std::vector<std::string> _system_audioType;
static std::vector<std::string> _system_devNumber;
static std::vector<std::string> _system_classType;
static std::vector<std::string> _system_info;
static std::map<std::string, std::string> _system_gingaJ;
static std::map<std::string, std::string> _system;
static std::map<std::string, std::string> _user;
static std::map<std::string, int> _tokens;
static std::string _system_ncl;
static std::string _system_GingaNCL;

Settings::Settings() {

	APRINT_PROPERTY("<==>");
	
	getNativePropertyCallback = NULL;
}

Settings::~Settings() {
	APRINT_PROPERTY("<==>");
}

void Settings::init() {
	APRINT_PROPERTY("<==>");
	if (_initStatic) {
#define DO_ENUM_TOKENS_MAP(t,s) { _tokens[s] = tokens::t; }
		TOKEN_LIST(DO_ENUM_TOKENS_MAP)
#undef DO_ENUM_TOKENS_MAP

		if (!util::cfg::getValue<bool>( "quirks.internalUnitTests" )) {
			_system["language"]		= "pt";
			_system["caption"]		= "pt";
			_system["subtitle"]		= "pt";
			_system["CPU"]		= "1000";
			_system["memory"]		= "1024";
			_system["operatingSystem"]	= "Linux";
		} else {
			_system["language"]		= "es";
			_system["caption"]		= "es";
			_system["subtitle"]		= "es";
			_system["CPU"]		= "";
			_system["memory"]		= "";
			_system["operatingSystem"]	= "";
		}
		_system["classNumber"]		= "1";
		_system["luaVersion"]		= "5.1";
		_system["luaSupportedEventClasses"]	= "tcp,user,ncl,key";
		_system["nclVersion"]		= "3.0";
		_system["gingaNCLProfile"]		= "Enhanced";
		_system["gingaNCLVersion"]		= "LG Ginga 2.0";
		_system["GingaJ"]		= "GingaJ";
		_system["javaConfiguration"]		= "CDC 1.1.2";
		_system["javaProfile"]		= "Foundation Profile 1.1.2";

		char tmp[50];
		snprintf( tmp, sizeof(tmp), "(%d, %d)",
			util::cfg::getValue<int>("gui.window.size.width"),
			util::cfg::getValue<int>("gui.window.size.height") );
		_system_screenSize.push_back(std::string(tmp, strlen(tmp)));

		snprintf( tmp, sizeof(tmp), "(%d, %d)",
			util::cfg::getValue<int>("gui.canvas.size.width"),
			util::cfg::getValue<int>("gui.canvas.size.height") );
		_system_screenGraphicSize.push_back(std::string(tmp, strlen(tmp)));

		_system_audioType.push_back(std::string("stereo"));
		_system_returnBitRate.push_back(std::string("0"));
		_system_devNumber.push_back(std::string("1"));
		_system_classType.push_back(std::string("active"));
		_system_info.push_back(std::string("AudioPlayer HtmlPlayer ImagePlayer JavaPlayer LuaPlayer MngPlayer NclPlayer TextPlayer VideoPlayer"));

		_system_audioType.push_back(std::string("mono"));
		_system_returnBitRate.push_back(std::string("0"));
		_system_devNumber.push_back(std::string("2"));
		_system_classType.push_back(std::string("passive"));
		_system_info.push_back(std::string("___"));

		_system_ncl = "3.0";
		_system_GingaNCL = "LG Ginga 2.0";
		_system_gingaJ["version"] = "LG GingaJ 1.0";

		_user["age"] = " ";
		_user["location"] = " ";
		_user["genre"] = " ";

		_initStatic = false;
	}

	_si.clear();
	_defaults.clear();
	_service.clear();
	_channel.clear();
	_shared.clear();
	_global.clear();

	if (!util::cfg::getValue<bool>( "quirks.internalUnitTests" )) {
		_defaults["focusBorderColor"] = "blue";
		_defaults["selBorderColor"]   = "green";
	} else {
		_defaults["focusBorderColor"] = "white";
		_defaults["selBorderColor"]   = "white";
	}
	_defaults["focusBorderWidth"] = "3";
	_focusBorderWidth = 3;
	_defaults["focusBorderTransparency"] = "0";
	_focusBorderTransparency = 0.0f;

	_service["currentFocus"] = "0";
	_service["currentKeyMaster"] = "";

	_si["numberOfServices"] = "0";
	_si["numberOfPartialServices"] = "0";
	_si["channelNumber"] = "1";

	_channel["keyCapture"] = "interactive_keys";
	_channel["virtualKeyboard"] = "false";
	_channel["keyboardBounds"] = "0, 0, 0, 0";
}

int getGroup(const std::string &name) {
	APRINT_PROPERTY("==> name[%s]",name.c_str());
	std::map<std::string, int>::const_iterator it = _tokens.find(name);
	if (it == _tokens.end()) {
		APRINT_PROPERTY("<== DID NOT get it name[%s]",name.c_str());
		return tokens::unknown;
	}
	APRINT_PROPERTY("<== Got it! Name[%s]",name.c_str());
	return it->second;
}

bool parsName( const std::string &name, int &group, std::string &varName ) {
	APRINT_PROPERTY("<==> name[%s] group[%d] varName[%s]",
		name.c_str(),group,varName.c_str());
	std::vector<std::string> names;
	boost::split( names, name, boost::is_any_of( "." ) );

	if (names.size() < 2) {
		group = tokens::unknown;
		varName = name;
		return true;
	}

	group = getGroup(names[0]);
	if ((group < tokens::system) || (tokens::shared < group)) {
		return false;
	}

	if (names.size() > 2) {
		if ((group != tokens::system) || names.size() > 3) {
			return false;
		}
		if (names[1] == "ncl") {
			group = tokens::ncl;
		} else if (names[1] == "GingaNCL") {
			group = tokens::GingaNCL;
		} else {
			return false;
		}
		varName = names[2];
	} else {
		varName = names[1];
	}
	return true;
}

void Settings::setProperty( int group, const std::string &varName, const std::string &value, bool fromNCL ) {
	APRINT_PROPERTY("==> group[%d] varName[%s] value[%s] fromNCL[%d]",
		group,varName.c_str(),value.c_str(),fromNCL);

	bool needUpdatePlayers = false;
	if (varName.empty() || (!fromNCL && group != tokens::defaults)) {
		APRINT_PROPERTY("<== not found");
		return;
	}

	switch (group) {
		case tokens::unknown: {
			APRINT_PROPERTY("tokens::unknown");
			_global[varName] = value;
			break;
		}
		case tokens::system:
			APRINT_PROPERTY("tokens::system");
		case tokens::user:
			APRINT_PROPERTY("tokens::user");
		case tokens::si: {
			APRINT_PROPERTY("tokens::si");
			//LERROR("Player::settings", "%s is a read only group of environment variables", _tokens[group-1].c_str());
			break;
		}
		case tokens::shared: {
			APRINT_PROPERTY("tokens::shared");
			_shared[varName] = value;
			break;
		}
		case tokens::defaults: {
			APRINT_PROPERTY("tokens::default");
			if (value.empty()) {
				break;
			}
			if (varName == "focusBorderWidth") {
				std::string v = value;
				boost::trim( v );
				int i;
				try {
					i = boost::lexical_cast<int>(v);
				} catch (boost::bad_lexical_cast &) {
					LWARN("Settings", "Conversion error[%s]",v.c_str());
					break;
				}
				needUpdatePlayers = true;
				_focusBorderWidth = i;
			} else if (varName == "focusBorderTransparency") {
				float tmp = 0.0;
				if (checkFloat( value, tmp )) {
					_focusBorderTransparency = tmp;
					needUpdatePlayers = true;
				} else {
					break;
				}
			} else if (varName == "focusBorderColor" || varName == "selBorderColor") {
				canvas::Color tmp;
				if (!canvas::color::get(value.c_str(), tmp)) {
					break;
				}
				needUpdatePlayers = true;
			}
			_defaults[varName] = value;
			break;
		}
		case tokens::service: {
			APRINT_PROPERTY("tokens::service");
			_service[varName] = value;
			break;
		}
		case tokens::channel: {
			APRINT_PROPERTY("tokens::channel");
			if (!value.empty()) {
				_channel[varName] = value;
				if(varName == "keyCapture") {
					util::cfg::setValue<std::string>("channel.keyCapture", value);
				}
			}
			break;
		}
		default:
			APRINT_PROPERTY("DEFAULT");
			break;
	}
	if (needUpdatePlayers && !_updatePlayers.empty()) {
		APRINT_PROPERTY("update players");
		_updatePlayers();
	}
	APRINT_PROPERTY("<==");
}

void Settings::setProperty( const std::string &name, const std::string &value, bool fromNCL ) {
	APRINT_PROPERTY("==> name[%s]  value[%s]  fromNCL[%d]", name.c_str(), value.c_str(), fromNCL);
	std::string varName;
	int group;
	if (parsName( name, group, varName )) {
		APRINT_PROPERTY("setting  name[%s]  value[%s]  fromNCL[%d]", name.c_str(), value.c_str(), fromNCL);
		setProperty( group, varName, value, fromNCL);
	}
	APRINT_PROPERTY("<== name[%s]  value[%s]  fromNCL[%d]", name.c_str(), value.c_str(), fromNCL);
}

const std::string Settings::getProperty( const std::string &name ) {
	APRINT_PROPERTY("==> name[%s]",name.c_str());
	std::string varName;
	std::string res = "";
	int group;
	if (parsName( name, group, varName )) {
		if (group == tokens::system) {
			size_t index = 0;
			std::string prop;
			size_t pos = varName.find( "(" );
			if (pos != std::string::npos) {
				size_t pos1 = varName.find( ")" );
				if (pos1 == std::string::npos) {
					APRINT_PROPERTY("<== res[%s]",res.c_str());
					return res;
				}
				std::string ind = varName.substr(pos + 1, pos1 - pos - 1);
				if (!ind.empty()) {
					try{
						index = boost::lexical_cast<int>(ind.c_str());
					} catch (boost::bad_lexical_cast &) {
						APRINT_ERROR("EXCEPTION");
						LWARN("Settings", "Conversion error[%s]",ind.c_str());
						index=0;
					}//catch
				}//if empty
				index =  (index < 2)? 0 : index--;
				prop = varName.substr(0, pos);
			} else {
				APRINT_PROPERTY("---");
				prop = varName;
			}

			int childGroup = getPropertyOrGroup( group, prop, res, false );
			if (tokens::screenSize <= childGroup && childGroup <= tokens::info) {
				APRINT_PROPERTY("---");
				getIndexProperty( childGroup, index, res );
			}
		} else {
			APRINT_PROPERTY("---");
			getPropertyOrGroup( group, varName, res, false );
		}
	}
	APRINT_PROPERTY("<== res[%s]",res.c_str());
	return res;
}

void Settings::getIndexProperty( int group, int index, std::string &res ) {
	APRINT_PROPERTY("==> group[%d] index[%d]",group,index);
	if (index < 0) {
		index = 0;
	}

	switch (group) {
		case tokens::screenSize: {
			APRINT_PROPERTY("tokens::screenSize");
			if (index < _system_screenSize.size()) {
					res = _system_screenSize[index];
			}
			break;
		}
		case tokens::screenGraphicSize: {
			APRINT_PROPERTY("tokens::screenGraphicSize");
			if (index < _system_screenGraphicSize.size()) {
				res = _system_screenGraphicSize[index];
			}
			break;
		}
		case tokens::audioType: {
			APRINT_PROPERTY("tokens::audioType");
			if (index < _system_audioType.size()) {
				res = _system_audioType[index];
			}
			break;
		}
		case tokens::returnBitRate: {
			APRINT_PROPERTY("tokens::returnBitRate");
			if (index < _system_returnBitRate.size()) {
				res = _system_returnBitRate[index];
			}
			break;
		}
		case tokens::devNumber: {
			APRINT_PROPERTY("tokens::devNumber");
			if (index < _system_devNumber.size()) {
				res = _system_devNumber[index];
			}
			break;
		}
		case tokens::classType: {
			APRINT_PROPERTY("tokens::classType");
			if (index < _system_classType.size()) {
				res = _system_classType[index];
			}
			break;
		}
		case tokens::info: {
			APRINT_PROPERTY("tokens::info");
			if (index < _system_info.size()) {
				res = _system_info[index];
			}
			break;
		}
		default:{
			APRINT_PROPERTY("DEFAULT");
		}break;
	}
	APRINT_PROPERTY("<==");
}

int Settings::getPropertyOrGroup( int group, const std::string &varName, std::string &res, bool checkGlobal ) {
	APRINT_PROPERTY("==> group[%d] varName[%s] checkGlobal[%d]",group,varName.c_str(),checkGlobal);
	int resGroup = tokens::unknown;
	switch (group) {
		case tokens::unknown: {
			APRINT_PROPERTY("tokens::unknown");
			if (checkGlobal) {
				int i = getGroup( varName );
				if (tokens::unknown < i && i < tokens::ncl) {
					APRINT_PROPERTY("<==");
					return i;
				}
			}
			std::map<std::string, std::string>::const_iterator it = _global.find(varName);
			if (it != _global.end()) {
				APRINT_PROPERTY("got on default");
				res = it->second;
			}
			break;
		}
		case tokens::user: {
			APRINT_PROPERTY("tokens::user");
			std::map<std::string, std::string>::const_iterator it = _user.find(varName);
			if (it != _user.end()) {
				res = it->second;
			}
			break;
		}
		case tokens::defaults: {
			APRINT_PROPERTY("tokens::defaults");
			std::map<std::string, std::string>::const_iterator it = _defaults.find(varName);
			if (it != _defaults.end()) {
				res = it->second;
			}
			break;
		}
		case tokens::service: {
			APRINT_PROPERTY("tokens::service");
			std::map<std::string, std::string>::const_iterator it = _service.find(varName);
			if (it != _service.end()) {
				res = it->second;
			}
			break;
		}
		case tokens::si: {
			APRINT_PROPERTY("tokens::si");
			
			if (varName == "channelNumber")
			{
				SINT8 ret = 0;
				char value[1024];
					
				ret = getNativePropertyCallback((char *)"si.channelNumber", value);	
				
				res = std::string(value);
			}
			else
			{
				std::map<std::string, std::string>::const_iterator it = _si.find(varName);
				if (it != _si.end()) {
					res = it->second;
				}
			}
			break;
		}
		case tokens::channel: {
			APRINT_PROPERTY("tokens::channel");
			std::map<std::string, std::string>::const_iterator it = _channel.find(varName);
			if (it != _channel.end()) {
				res = it->second;
			}
			break;
		}
		case tokens::shared: {
			APRINT_PROPERTY("tokens::shared");
			std::map<std::string, std::string>::const_iterator it = _shared.find(varName);
			if (it != _shared.end()) {
				res = it->second;
			}
			break;
		}
		case tokens::ncl: {
			APRINT_PROPERTY("tokens:ncl");
			if (varName == "version") {
				res = _system_ncl;
			}
			break;
		}
		case tokens::GingaNCL: {
			APRINT_PROPERTY("GingaNCL!!");
			if (varName == "version") {
				res = _system_GingaNCL;
			}
			break;
		}
		case tokens::system: {
			APRINT_PROPERTY("tokens:system");
			int i = getGroup(varName);
			if (tokens::ncl <= i && i <= tokens::GingaJ) { //GingaJ is the last one now
				APRINT_PROPERTY("group");
				resGroup = i;
			} else {
				APRINT_PROPERTY("!!! group");
				std::map<std::string, std::string>::const_iterator it = _system.find(varName);
				if (it != _system.end()) {
					res = it->second;
				}else{
					APRINT_PROPERTY("Not found");

				}
			}
			break;
		}
		case tokens::GingaJ:{
			APRINT_PROPERTY("tokens:GingaJ");
			std::map<std::string, std::string>::const_iterator it = _system_gingaJ.find(varName);
			if (it != _system.end()) {
				APRINT_PROPERTY("FOUND! ");
				res = _system_gingaJ[varName];
			}else{
				APRINT_PROPERTY("NOT FOUND GingaJ property");
			}
			break;
		}break;
		default:{
			APRINT_PROPERTY("DEFAULT");
		}break;
	};
	APRINT_PROPERTY("<==");
	return resGroup;
}

void Settings::setPlayersImageUpdateCallback( UpdatePlayersCallback &func ) {
	APRINT_PROPERTY("<==>");
	_updatePlayers = func;
}
void Settings::onGetNativeProperty ( char * key, char * value ) {

	LDEBUG( "Settings", "[Settings::%s::%d] ", __FUNCTION__, __LINE__ );
	if ( getNativePropertyCallback != 0 ) {
		LDEBUG( "Settings", "[Settings::%s::%d]", __FUNCTION__, __LINE__ );
		( *getNativePropertyCallback ) ( key, value );
	}
}
}
}
