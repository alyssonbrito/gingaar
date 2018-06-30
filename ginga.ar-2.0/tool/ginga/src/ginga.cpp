#include "../../../include/ginga_api.h"
#include "generated/config.h"
#include "ginga.h"
#include <ncl30-presenter/presentationenginemanager.h>
#include <gingaplayer/system.h>
#include <gingaplayer/device.h>
#include <util/log.h>
#include <util/cfg/cfg.h>
#include <util/cfg/cmd/commandline.h>
#include <boost/filesystem/operations.hpp>
#include <string.h>
#include <stdio.h>

#include <pem-tv-util/pem_moa_common.h>

#if defined(LG_LIBS)
#if defined(WEBOS_USE_GM)

///TODO: removed libgm dependency and tvservice would provide GM window resolution setting and visibility setting API.
///     prevent compile error because hybridtv/adapt/xxx/xxx/libgm had been removed now.
#define GM_TEMP

#if defined(GM_TEMP)
#include <dlfcn.h>

#define LIBGM_FILE_NAME "libgm.so.0"
#define GM_WINDOW_INVALID 0xffffffff
#define GM_OK 0
#define GM_FAILED (!GM_OK)
typedef int GM_STATUS_T;
typedef signed int GM_WINDOW_ID_T;

static void* _gmHandle = NULL;
static bool _bGmInit = false;

static GM_STATUS_T (*GM_SetWindowResolution_func)(signed int windowID, unsigned short width, unsigned short height);
static GM_STATUS_T (*GM_SetWindowVisibility_func)(signed int windowID, unsigned int bVisibility);

static void _unloadGM()
{
    _bGmInit = false;

    if (!_gmHandle) return;

    if (GM_SetWindowVisibility_func) {
        GM_WINDOW_ID_T winId = GM_WINDOW_INVALID;
        char* pWinIdStr = getenv("HTV_GM_WINDOW_ID");
        if (pWinIdStr) {
            winId = (GM_WINDOW_ID_T)atoi(pWinIdStr);
            GM_SetWindowVisibility_func(winId, FALSE);
        }
    }

    GM_SetWindowVisibility_func = NULL;
    GM_SetWindowResolution_func = NULL;

    dlclose(_gmHandle);
    _gmHandle = NULL;
}

static bool _loadGM()
{
    if (_bGmInit)
        return true;

    _gmHandle = dlopen(LIBGM_FILE_NAME, RTLD_LAZY | RTLD_GLOBAL);
    if (!_gmHandle) {
        _unloadGM();
        LERROR("LGDeviceAdapter", "dlopen %s failed - errno(%d)\n", LIBGM_FILE_NAME, errno);
        return false;
    }

    GM_SetWindowResolution_func = (GM_STATUS_T (*)(signed int, unsigned short, unsigned short))dlsym(_gmHandle, "GM_SetWindowResolution");
    if (!GM_SetWindowResolution_func) {
        _unloadGM();
        LERROR("LGDeviceAdapter", "dlsym %s failed - errno(%d)\n", "GM_SetWindowResolution", errno);
        return false;
    }

    GM_SetWindowVisibility_func = (GM_STATUS_T (*)(signed int, unsigned int))dlsym(_gmHandle, "GM_SetWindowVisibility");
    if (!GM_SetWindowVisibility_func) {
        _unloadGM();
        LERROR("LGDeviceAdapter", "dlsym %s failed - errno(%d)\n", "GM_SetWindowVisibility", errno);
        return false;
    }

    _bGmInit = true;

    return true;
}

static GM_STATUS_T GM_SetWindowResolution(signed int windowID, unsigned short width, unsigned short height)
{
    if (_loadGM())
        return GM_SetWindowResolution_func(windowID, width, height);
    return GM_FAILED;
}

static GM_STATUS_T GM_SetWindowVisibility(signed int windowID, unsigned int bVisibility)
{
    GM_STATUS_T ret = GM_FAILED;
    if (_loadGM())
        ret = GM_SetWindowVisibility_func(windowID, bVisibility);
    if (!bVisibility)
        _unloadGM();
    return ret;
}

#else
#include"gm_openapi.h"
#endif /* GM_TEMP */

#endif /* WEBOS_USE_GM */
#endif /* LG_LIBS */

namespace fs = boost::filesystem;
namespace bptgn = br::pucrio::telemidia::ginga::ncl;



//#if defined(LG_LIBS)
#include "ginga_api.h"
LGESP_INTEGRATION_STUB_IMPL();
//#endif

#if defined(LG_LIBS)
static bool initializeGraphic(int width, int height)
{
#if defined(WEBOS_USE_GM)
    GM_WINDOW_ID_T winId = GM_WINDOW_INVALID;
    char* pWinIdStr = getenv("HTV_GM_WINDOW_ID");
    if (pWinIdStr)
        winId = (GM_WINDOW_ID_T)atoi(pWinIdStr);

    if(winId != GM_WINDOW_INVALID)
    {
        if(GM_SetWindowResolution(winId, width, height) != GM_OK) {
            LERROR("LGDeviceAdapter", "GM_SetWindowResolution failed.\n");
        return false;
        }

        if(GM_SetWindowVisibility(winId, TRUE) != GM_OK) {
            LERROR("LGDeviceAdapter", "GM_SetWindowVisibility(Enable) failed.\n");
            return false;
        }
    }
    else
    {
        LERROR("LGDeviceAdapter", "HTV_GM_WINDOW_ID environment variable is not set or invalid.\n");
        return false;
    }
#endif /*WEBOS_USE_GM */

    return true;
}

static bool finalizeGraphic()
{
#if defined(WEBOS_USE_GM)
    GM_WINDOW_ID_T winId = GM_WINDOW_INVALID;
    char* pWinIdStr = getenv("HTV_GM_WINDOW_ID");
    if (pWinIdStr)
        winId = (GM_WINDOW_ID_T)atoi(pWinIdStr);

    if(winId != GM_WINDOW_INVALID)
    {
        if(GM_SetWindowVisibility(winId, FALSE) != GM_OK) {
            LERROR("LGDeviceAdapter", "GM_SetWindowVisibility(Disable) failed.\n");
            return 0;
        }
    }
    else
    {
        LERROR("LGDeviceAdapter", "HTV_GM_WINDOW_ID environment variable is not set or invalid.\n");
        return false;
    }
#endif /*WEBOS_USE_GM */

	return true;
}
#endif /* LG_LIBS */

CREATE_TOOL(Ginga,GINGA)

Ginga::Ginga(const std::string &name, const std::string &desc, const std::string &version, const std::string &build)
	: Tool(name, desc, version, build) {
}
Ginga::~Ginga() {
}

//Configuration
void Ginga::registerProperties( util::cfg::cmd::CommandLine &cmd ) {
	Tool::registerProperties( cmd );

	registerToolProp("ncl","Start ncl file", std::string(""), cmd);
	registerToolProp("baseId", 'i', "Set the BaseId name", std::string("-1"), cmd);	
	registerToolProp("size", 's', "Window size (1280x720 or 720x576)", std::string("1280x720"), cmd);
}

int Ginga::run( util::cfg::cmd::CommandLine &cmd ) {
	printf("\n   --------------------------| Ginga.ar %s |--------------------------\n", version().c_str());

	//	Set windows title
	util::cfg::setValue("gui.window.title", "Ginga");

	//	Set window size
	int windowWidth = 1280;
	int windowHeight = 720;

	if(getCfg<std::string> ("size") == "720x576") {
		windowWidth  = 720;
		windowHeight = 576;
	}

	util::cfg::setValue("gui.canvas.size.width",  windowWidth);
	util::cfg::setValue("gui.canvas.size.height", windowHeight);
	util::cfg::setValue("gui.window.size.width",  windowWidth);
	util::cfg::setValue("gui.window.size.height", windowHeight);


	fs::path url( getCfg<std::string> ("ncl") );

	if (!url.is_absolute()) {
		url = fs::current_path() / url;
	}

	if (exists(url) && is_regular_file(url)) {
		//	LG Electronics: #3751
		//	Description: Changing working directory, due to enabling relative paths
		//	in lua scripts.
		fs::path workingDir(url);
		if (chdir(workingDir.remove_filename().string().c_str()) != 0)
			LERROR("Ginga", "Cannot change working directory: %s", workingDir.string().c_str());

		player::System *sys = new player::System();
		player::Device *dev = NULL;

		UNUSED(cmd);
		dev = new player::Device( sys );		
		assert(dev);

		//	Initialize device and add to system
		if (dev->initialize()) {
			sys->addDevice( dev );
#if defined(LG_LIBS)
		initializeGraphic(windowWidth, windowHeight);
#endif

			//	Create presentation engine manager
			bptgn::PresentationEngineManager *pem = new bptgn::PresentationEngineManager(sys);
			pem->setBaseId( getCfg<std::string> ("baseId") );
			if (pem->startDocument( url.string() )) {
				//	LG Electronics: #4326
				//	Change focus and selection borders to noticeable colors
				sys->settings()->setProperty( "default.focusBorderColor", "blue" );
				sys->settings()->setProperty( "default.selBorderColor", "green" );
				pem->run();
			}
			delete pem;
		} else {
			LERROR( "Ginga", "Cannot initialize device" );
		}

		sys->destroy();
		delete sys;
#if defined(LG_LIBS)
		finalizeGraphic();
#endif
	} else {
		LERROR( "Ginga", "invalid ncl file path: url=%s", url.string().c_str());
	}

	LINFO("Ginga", "Process finished.");
	return 0;
}

void Ginga::printVersion() const {
	printf("Ginga.ar %s by LIFIA\n\
Based on Ginga 0.11.2\n\
Copyright 2002-2010 The TeleMidia PUC-Rio Team.\n\
Copyright 2010 LIFIA - Facultad de Informática - Univ. Nacional de La Plata\n", version().c_str());
}
