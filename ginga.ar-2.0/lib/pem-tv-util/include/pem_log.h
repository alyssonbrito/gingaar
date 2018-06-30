/**
 * \file   pem_log.h
 * \brief  Header for Native Log Management Module.
 *         It was ported from MOA Log (Ginga-J).
 * \author frank.yonamine@lge.com
 */
#ifndef __PEM_LOG_H__
#define __PEM_LOG_H__


#ifdef __cplusplus
extern "C" {
#endif


#include <stdarg.h>

#include "pem_log_config.h"

#include "pem_moa_common.h"

#define DEBUG_CONFIG_FILE_PATH "/mnt/lg/cmn_data/ginga_mw_storage"

#define DEBUG_CONFIG_FILE_NAME "ncl_debug.config"


#define COLOR_LOG_FATAL			"\033[0;31m"
#define COLOR_LOG_ERROR			"\033[0;31m"
#define COLOR_LOG_WARN			"\033[0;33m"
#define COLOR_LOG_INFO			"\033[0;32m"
#define COLOR_LOG_DEBUG			"\033[0;32m"

#define COLOR_LOG_END			"\033[0m"

#ifndef MAX_LOG_BUFFER
#define MAX_LOG_BUFFER		256
#endif

#define LOG_CATEGORY_SIZE 11


// Check these definitions. Some of them are not used...
typedef enum {
	LC_BRIDGE			= 0x00100,
	LC_LAUNCHER			= 0x00080,
	LC_PORT				= 0x00040,
	LC_MWAGT			= 0x00020,
	LC_TVAGT			= 0x00010,
	LC_OSA				= 0x00008,
	LC_PRINT			= 0x00004,
	LC_EMPTY1			= 0x00002,
	LC_EMPTY2			= 0x00001
} pem_log_category;

typedef enum {
	LOG_FATAL		= 0x0,
	LOG_ERROR		= 0x1,
	LOG_WARN		= 0x2,
	LOG_INFO		= 0x3,
	LOG_DEBUG		= 0x4,
	LOG_INVALID		= 0xff,
} pem_log_level;

/*! Log Print Format Constant */
typedef enum {
	LOG_FORMAT_SIMPLE	= 0x0,
	LOG_FORMAT_TRACE	= 0x1,
	LOG_FORMAT_TIME		= 0x2
} pem_log_format;


void print_banner(const char* fmt, ...);


int PEM_logInit(const char* title);
void PEM_setLogConfig(UINT32 logconfig);
void PEM_changeLogFormat(pem_log_format format);
void PEM_changeLogLevel(pem_log_level level);
const char* PEM_getLogLevelName(pem_log_level level);
const char* PEM_getCategoryName(pem_log_category category);
void PEM_writeLog(UINT32 category, UINT32 level, const char *fmt, va_list argp);
void PEM_log(UINT32 category_, UINT32 level_, const char *fmt, ...) ;


void PEM_debug(UINT32 category_, const char *fmt, ...);
void PEM_info(UINT32 category_, const char *fmt, ...);
void PEM_warn(UINT32 category_, const char *fmt, ...);
void PEM_error(UINT32 category_, const char *fmt, ...);
void PEM_fatal(UINT32 category_, const char *fmt, ...);


void PEM_OSA_log(UINT32 level_, const char *fmt, ...);
void PEM_PORT_log(UINT32 level_, const char *fmt, ...);
void PEM_LAUNCHER_log(UINT32 level_, const char *fmt, ...);
void PEM_TVAGT_log(UINT32 level_, const char *fmt, ...);
void PEM_MWAGT_log(UINT32 level_, const char *fmt, ...);
void PEM_BRIDGE_log(UINT32 level_, const char *fmt, ...);


UINT8 PEM_isEnableLog(UINT32 category_, UINT32 level_);
void PEM_enableLog(int on);
void PEM_enableColor(int on);

void PEM_printf(const char* fmt, ...);


#ifdef __cplusplus
}
#endif



#endif /* __PEM_LOG_H__ */
