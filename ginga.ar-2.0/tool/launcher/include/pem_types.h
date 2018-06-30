#ifndef __PEM_TYPES_H__
#define __PEM_TYPES_H__


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

#include <ginga_api.h>

#include "AppInfo.h"


typedef int (*AnyEventCallbackType)();
typedef int (*StopEventCallbackType)(AppInfo *);
typedef int (*FetchEventCallbackType)(const std::string &);
typedef int (*LoadMediaCallbackType)(const std::string &, int *, int);
typedef int (*MediaEventCallbackType)(int);
typedef int (*MediaSetTimeEventCallbackType)(int, int);
typedef int (*ResizeMediaCallbackType)( int, int, int, int, int);


/**
 *
 * operation = 0 - GET operation
 *             1 - SET opetation
 * handlerId = file handler ID
 * property  = which property to get or set
 * value     = which value to get or set
 * return SUCCESS or FAILURE
 */
typedef SINT8 (*MediaPropertyCallback)( const int op, const int handlerId, const int property, unsigned int *value );


typedef int (*GetPropertyCallbackType) (const std::string&,std::string*);
typedef int (*SetPropertyCallbackType) (const std::string&,const std::string&);

typedef int (*GetNativePropertyCallbackType) ( char * key, char * value );

typedef int (*KeySetUpdateCallback)(const std::string &name,const std::string &value);




//typedef br::pucrio::telemidia::ginga::ncl::PresentationEngineManager::notifyOfNCLPlayerEvent notifyOfNCLPlayerEvent;
typedef void (*notifyOfNCLPlayerEvent)(int,std::string);
typedef br::pucrio::telemidia::ginga::ncl::PresentationEngineManager::cbNCLStartXlet cbNCLStartXlet;
typedef br::pucrio::telemidia::ginga::ncl::PresentationEngineManager::notifyOfXletAppState notifyOfXletAppState;
typedef br::pucrio::telemidia::ginga::ncl::PresentationEngineManager::cbNCLControlXlet cbNCLControlXlet;
typedef br::pucrio::telemidia::ginga::ncl::PresentationEngineManager::cbNCLXletVisibility cbNCLXletVisibility;


#endif /* __PEM_TYPES_H__ */

