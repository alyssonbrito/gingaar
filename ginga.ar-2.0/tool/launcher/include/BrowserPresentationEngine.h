#pragma once

// C/C++ Header Files
#include <string>
#include <cstdarg>

// *nix Header Files
#if !defined(_WIN32) && !defined(__mips__)
#include <signal.h>
#include <execinfo.h>
#include <linux/input.h>
#include <sys/prctl.h>
#endif

// Boost Header Files
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

// Ginga.Ar Header Files: Utils
#include <util/log.h>
#include <util/registrator.h>
#include <util/tool.h>
#include <util/cfg/cfg.h>
#include <util/cfg/xmlparser.h>
#include <util/cfg/cmd/commandline.h>

// Ginga.Ar Header Files: Ginga Player
#include <gingaplayer/system.h>
#include <gingaplayer/device.h>

// Ginga.Ar Header Files: Presentation Manager
#include <util/cfg/cfg.h>
#include <util/cfg/cmd/commandline.h>
#include <ncl30-presenter/presentationenginemanager.h>

// Common Variable Types
#include <pem-tv-util/pem_moa_common.h>
#include "pem_util.h"
#include "configuration.h"

#include <ginga_api.h>
#define BACKTRACE_FRAMES_MAX (20)

#include "pem_types.h"

#include "AppInfo.h"


class BrowserPresentationEngine {
public:
	BrowserPresentationEngine();
	virtual ~BrowserPresentationEngine();

	//
	int initialize(int, char *[]);
	int finalize(void);
	int execute();
	int stop();
	int destroy();
	//
	int putKey(unsigned int,unsigned int);
	int onMediaStop(SINT32 handlerId);
	int notifyUserPropertyChanged(unsigned int , char *, char *, char *);
	int notifyGetNativeProperty(char *value);
	int notifyRecommendedResolution(int, int);
	int notifyRequestKeySet(const std::string &);
	int notifyMediaStop(SINT32 handlerId);
	//
	int loadApplication(const std::string &);
	int changeToDirectory(boost::filesystem::path);
	int waitInit();

	//
	void setWindowTitle(std::string);

	// Set DEBUG ON
	void debug() const;

public:
	// Get Device reference
	player::Device *device()			{ return dev;		}

	// Check current Engine state
	bool isInitialized()				{ return _isInitialized;}

	void setRunning(bool bRunning)				{ _mtxRunning.lock(); _isRunning = bRunning; _mtxRunning.unlock();	}
	bool isRunning()				{ _mtxRunning.lock(); bool b = _isRunning; _mtxRunning.unlock(); return b;	}
	bool isWorking()				{ return _isWorking;	}
	void setWorking(bool bWorking){ _isWorking = bWorking;	}
	void onKeyPressed( ::util::key::type /*key*/);

	// Basic methods to get information from this Class.
	const std::string &name() const 		{ return _name; 	}
	const std::string &version() const		{ return _version;	}
	const std::string &build() const		{ return _build;	}
	const std::string &description() const		{ return _desc; 	}

	// Print Launcher info
	void printVersion() const	{ printf("%s version %s\n", name().c_str(), version().c_str() );	}
	void printBuild() const 	{ printf("%s\n", build().c_str() );					}




public:
	template<typename T>
	T getCfg(const std::string &);

	template<typename T>
	void registerToolProp(const std::string &prop, const char sname, const std::string &desc, T val, util::cfg::cmd::CommandLine &cmd, bool visible = true);

	// Register tool property without a short name
	template<typename T>
	void registerToolProp(const std::string &prop, const std::string &desc, T val, util::cfg::cmd::CommandLine &cmd, bool visible = true);

	template<typename T>
	void registerProp( const std::string &owner, const std::string &prop, const char sname, const std::string &desc, T val, util::cfg::cmd::CommandLine &cmd, bool visible = true);

	// Register property without a short name
	template<typename T>
	void registerProp(const std::string &owner, const std::string &prop, const std::string &desc, T val, util::cfg::cmd::CommandLine &cmd, bool visible = true);

	template<typename T>
	void registerCfg(const std::string &owner, const std::string &prop, const std::string &desc, T val, bool visible);

	void registerOpt(util::cfg::cmd::CommandLine &cmd, const std::string &owner, const std::string &prop, bool visible, char sname = 0);


protected:
	// Debug some member values
	void printArguments(int, char *[]);
	void printConfiguration();

	//
	const std::string &makeDescription(std::string &, std::string &) const;

	//
	int run(util::cfg::cmd::CommandLine &);

	// Prepare System and Device, and initialize them. It should be done once.
	void initializeDevice();

	//
	void addConfig( const std::string &xml ) { _cfgXmls.insert(xml); }
	void loadConfig();


	// Configuration hook methods
	void registerConfigs();
	void registerProperties(util::cfg::cmd::CommandLine &);


	void assignCallbacksToDevice();

// Listeners
public:
	// Setting Listeners
	void setOnBeforeStart(StopEventCallbackType callback)	{ _beforeStartCallback = callback;	}
	void setOnStopped(StopEventCallbackType callback)	{ _stoppedCallback = callback;		}
	void setOnLoadMainFile(FetchEventCallbackType callback)	{ _loadMainFileCallback = callback; }
	void setOnFetchFile(FetchEventCallbackType callback)	{ _fetchFilesCallback = callback;	}
	void setOnReserveKeys(MediaEventCallbackType callback)	{ _reserveKeysCallback = callback;	}
	void setOnKeyBeforeInit(AnyEventCallbackType callback)	{ _beforeInitCallback = callback;	}
	void setOnKeySetUpdate(KeySetUpdateCallback cb) 	{ _keySetUpdateCallback = cb; }

	// Media
	void setOnLoadMedia(LoadMediaCallbackType callback)		{ _loadMediaCallback = callback;        }
	void setOnMediaTime(MediaSetTimeEventCallbackType callback)		{ _setMediaTimeCallback = callback;        }
	void setOnPlayMedia(MediaEventCallbackType callback)		{ _playMediaCallback = callback;	}
	void setOnStopMedia(MediaEventCallbackType callback)		{ _stopMediaCallback = callback;	}
	void setOnCloseMedia(MediaEventCallbackType callback)		{ _closeMediaCallback = callback;	}
	void setOnFreezeMedia(MediaEventCallbackType callback)		{ _freezeMediaCallback = callback;	}
	void setOnResumeMedia(MediaEventCallbackType callback)		{ _resumeMediaCallback = callback;	}
	void setOnResizeMedia(ResizeMediaCallbackType callback)		{ _resizeMediaCallback = callback;	}



	void setOnMediaPropertyChanged(MediaPropertyCallback cb)	{ _mediaPropertyCallback = cb; }



	// Properties
	void setOnGetProperty(GetPropertyCallbackType callback) { _getPropertyCallback = callback;      }
	void setOnSetProperty(SetPropertyCallbackType callback) { _setPropertyCallback = callback;      }

	void setOnGetNativeProperty(GetNativePropertyCallbackType callback) { _getNativePropertyCallback = callback;      }

	// End of Setting Listeners

private:
	// Callback members
	StopEventCallbackType   _beforeStartCallback;
	StopEventCallbackType   _stoppedCallback;
	FetchEventCallbackType   _loadMainFileCallback;
	FetchEventCallbackType _fetchFilesCallback;
	LoadMediaCallbackType _loadMediaCallback;
	MediaSetTimeEventCallbackType   _setMediaTimeCallback;
	MediaEventCallbackType   _playMediaCallback;
	MediaEventCallbackType   _stopMediaCallback;
	MediaEventCallbackType   _closeMediaCallback;
	MediaEventCallbackType   _freezeMediaCallback;
	MediaEventCallbackType   _resumeMediaCallback;
	ResizeMediaCallbackType   _resizeMediaCallback;

	MediaPropertyCallback _mediaPropertyCallback;

	GetPropertyCallbackType _getPropertyCallback;
	SetPropertyCallbackType _setPropertyCallback;
	GetNativePropertyCallbackType _getNativePropertyCallback;
	MediaEventCallbackType _reserveKeysCallback;
	AnyEventCallbackType _beforeInitCallback;
	KeySetUpdateCallback _keySetUpdateCallback;



private:
	// Notification sent to registered listeners
	int notifyBeforeStart(void);
	int notifyLoadFileInvocation(const std::string &);
	int notifyStopped(void);

	// Apply resolution that was read from SI
	void applyRecommendedResolution();

	int configureDebug(unsigned int enable, unsigned int level);

	// Flags for Engine state
	bool _isInitialized;
	bool _isRunning;
	boost::mutex _mtxRunning;
	bool _isWorking;



private:
	std::string _name;
	std::string _desc;
	std::string _version;
	std::string _build;
	std::set<std::string> _cfgXmls;

	// Set Window Size
	int windowWidth;
	int windowHeight;

	// Set Canvas Size
	int canvasWidth;
	int canvasHeight;

	// Recommended Size
	int recommendedWidth;
	int recommendedHeight;

	player::System *sys;
	player::Device *dev;


	br::pucrio::telemidia::ginga::ncl::PresentationEngineManager *pem;

	util::cfg::cmd::CommandLine *cmd;

	boost::filesystem::path _home_directory;
	boost::filesystem::path _application_path;


/* Begin stuff for Bridge */
public:
	void setOnNCLPlayerEvent(notifyOfNCLPlayerEvent callback) { _onNCLPlayerEventCallback = callback;  }
	void setOnNCLStartXlet(const cbNCLStartXlet &callback) { _onNCLStartXletCallback = callback;      }
	void setOnNCLControlXlet(const cbNCLControlXlet &callback) { _onNCLControlXletCallback = callback;      }
	void setOnXletAppState(const notifyOfXletAppState &callback) { _onXletAppStateCallback = callback;      }
	void setOnNCLSetVisibility(const cbNCLXletVisibility &callback) { _onNCLSetVisibilityCallback = callback;      }

private:

	notifyOfNCLPlayerEvent _onNCLPlayerEventCallback;
	cbNCLStartXlet _onNCLStartXletCallback;
	cbNCLControlXlet _onNCLControlXletCallback;
	notifyOfXletAppState _onXletAppStateCallback;
	cbNCLXletVisibility _onNCLSetVisibilityCallback;
/* End stuff for Bridge */




//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
public:
	AppInfo appInfo;

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


};



template<typename T>
void BrowserPresentationEngine::registerToolProp(const std::string &prop, const char sname, const std::string &desc, T val, util::cfg::cmd::CommandLine &cmd, bool visible ) {
	registerProp("tool." + name(), prop, sname, desc, val, cmd, visible);
}

template<typename T>
void BrowserPresentationEngine::registerToolProp(const std::string &prop, const std::string &desc, T val, util::cfg::cmd::CommandLine &cmd, bool visible) {
	registerProp("tool." + name(), prop, desc, val, cmd, visible );
}

template<typename T>
void BrowserPresentationEngine::registerProp(const std::string &owner, const std::string &prop, const char sname, const std::string &desc, T val, util::cfg::cmd::CommandLine &cmd, bool visible) {
	registerCfg(owner, prop, desc, val, visible);
	registerOpt(cmd, owner, prop, visible, sname);
}

template<typename T>
void BrowserPresentationEngine::registerProp(const std::string &owner, const std::string &prop, const std::string &desc, T val, util::cfg::cmd::CommandLine &cmd, bool visible) {
	registerCfg(owner, prop, desc, val, visible);
	registerOpt(cmd, owner, prop, visible);
}

template<typename T>
void BrowserPresentationEngine::registerCfg(const std::string &owner, const std::string &prop, const std::string &desc, T val, bool visible) {
	util::cfg::get(owner).addValue(prop, desc, val);
	if (!visible) {
		util::cfg::get(owner).makeInvisible(prop);
	}
}

template<typename T>
T BrowserPresentationEngine::getCfg(const std::string &property) {
	return util::cfg::getValue<T>("tool." + name() + "." + property);
}

