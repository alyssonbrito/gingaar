#ifndef __PEM_CALLBACKS_H__
#define __PEM_CALLBACKS_H__


#include <string>
#include <cstring>

#include <sched.h>
#include <pthread.h>

#include <boost/thread.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include <gtk/gtk.h>
#include <sys/prctl.h>


#include <pem-tv-util/pem_log.h>
#include <pem-tv-util/ginga_n_port_data.h>


#include "BrowserPresentationEngine.h"
#include "pem_util.h"
#include "gingancl_bridge.h"
#include "browser_port_mw2tv.h"

#include "pem_worker_key.h"

#include "AppInfo.h"


// Callbacks to Browser Presentation Engine
int onFetchFilesFromCarousel(const std::string &url);
int onBeforeStart(AppInfo * appInfo);
int onStoppedApp(AppInfo * appInfo);
int onGetNativeSystemProperty(char * key, char * value);
int onRequestUserProperty(char */*key*/,char */*value*/);
int onGetProperty(const std::string &key, std::string */*value*/);
int onSetProperty(const std::string &key, const std::string &value);
int onDelProperty(const std::string &key);
int onReserveKeys(int /*key*/);
int onRequestKeySet(const std::string &name,const std::string &value);
int onLoadMainFile(const std::string &filename);


#define PEM_LOG_INFO(fmt,...)		PEM_LAUNCHER_log (LC_PRINT, "[%s::%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PEM_LOG_DEBUG(fmt,...)		PEM_LAUNCHER_log (LC_PRINT, "[%s::%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PEM_LOG_ERROR(fmt,...)		PEM_LAUNCHER_log (LC_PRINT, "[%s::%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PEM_LOG_MEDIA(id)																						\
	if ((id) != 0) PEM_LAUNCHER_log(LC_PRINT, "[%s::%d] Hander ID: %d[0x%x]\n", __FUNCTION__, __LINE__, (id), (id));	\
	else           PEM_LAUNCHER_log(LC_PRINT, "[%s::%d] Hander ID is NULL\n", __FUNCTION__, __LINE__);					\

#define DETACH(appInfo) PORT_N2TV_detach((appInfo)->getOnId(),(appInfo)->getTsID(),(appInfo)->getSID(),(appInfo)->getCtag(),(appInfo)->getAttachUniqueKey())

#endif /* __PEM_CALLBACKS_H__ */

