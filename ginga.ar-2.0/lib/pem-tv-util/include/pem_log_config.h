#ifndef __PEM_LOG_CONFIG_H__
#define __PEM_LOG_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/stat.h>

#include "pem_log.h"			// Retrieve some constant values!

#include "pem_moa_common.h"


typedef struct __GINGA_AR_CONFIGURATION {
	int enableLogLauncher;
	int enableLogEngine;
	int logLevel;
	int logMask;
	int enableColor;
} LauncherConfiguration;


#define IS_ENABLED(value)			( strcasecmp( (value), "true" ) == 0 )
#define IS_SUCCESS(value)			((value) == 1 ? "SUCCESS" : "FAILED" )
#define IS_ENABLED_STR(value)			( strcasecmp( (value), "true") == 0 ? "YES" : "NO" )


// Define me if you want more info about logging. Or I should define on build script.
#undef ENABLE_PRINT_CONFIG_MESSAGE


#ifdef ENABLE_PRINT_CONFIG_MESSAGE
#define PRINT_CONFIG_KEY_VALUE(key, value)	printf (key ": [%s]\n", IS_ENABLED_STR( (value) ))
#define PRINT_CONFIG_MSG(fmt,...)			printf(fmt, ##__VA_ARGS__)
#else
#define PRINT_CONFIG_KEY_VALUE(msg, value)
#define PRINT_CONFIG_MSG(fmt,...)
#endif


#define PRINT_ERROR_MSG(fmt,...)			printf(fmt, ##__VA_ARGS__)


//
static char* trim(char* str);
static int toValidLogLevel(char* value);
static int isFileExist(const char* filename);
static int initConfig(LauncherConfiguration *launcherConfig);
static int createAndSaveDefaultConfigFile(const char* filename, LauncherConfiguration *launcherConfig);
static int readConfigFile(const char* filename, LauncherConfiguration *launcherConfig);

int loadConfigFile(const char* filename, LauncherConfiguration *launcherConfig);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PEM_LOG_CONFIG_H__ */
