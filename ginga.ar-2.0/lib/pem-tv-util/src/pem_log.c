/**
 * \file   pem_log.c
 * \brief  Header for Native Log Management Module.
 *         It was ported from MOA Log (Ginga-J).
 * \author frank.yonamine@lge.com
 * \author douglas.lima@lge.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>

#include "pem_moa_common.h"
#include "pem_log.h"

/*-----------------------------------------------------------------
 * Log Settings
 * ----------------------------------------------------------------
 */
static UINT32 LOG_CONFIG;
static UINT32 LOG_ENABLE = 1;
static UINT32 LOG_COLORS = 0;

static SINT8  debugStoragePath[1024];

static LauncherConfiguration launcherConfig;

static pem_log_format CURRENT_LOG_FORMAT = LOG_FORMAT_SIMPLE;
static pem_log_level  CURRENT_LOG_LEVEL  = LOG_FATAL;


// Constant values for Banner
static const char    line[] = "><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><";
static const size_t  max_len_line      = 79 + 1; // Length of "line", +1 for '\0'
static const char    left_paragraph[]  = ">< ";
static const char    right_paragraph[] = " ><";
static const size_t  max_len_paragraph = 4; // Length of "paragraph"

static void print_banner_line(const char* message);


/**
 * Format a line according to the message length.
 * If the message length is greater thant "max_len_line", the
 * message would be truncated and "..." will be shown on the
 * last three characters.
 */
static void print_banner_line(const char* message) {

	char buffer[max_len_line];
	memset(buffer, 0x20, max_len_line);
	buffer[max_len_line] = 0x00;

	size_t valid_area = ( (1 + max_len_line) - (max_len_paragraph + max_len_paragraph) );
	char spaces[valid_area];

	memset(spaces, 0x20, valid_area);
	spaces[valid_area] = 0x00;

	size_t i = strlen(message);
	size_t j = strlen(spaces);

	if (i < j) {
		snprintf(&spaces[1], valid_area, "%s", message);
		memset(&spaces[i + 1], 0x20, valid_area - i);
		spaces[valid_area - 1] = 0x00;
	}
	else if (i > j) {
		strncpy(&spaces[1], message, valid_area);
		spaces[valid_area - 4] = 0x2e;
		spaces[valid_area - 3] = 0x2e;
		spaces[valid_area - 2] = 0x2e;
		spaces[valid_area - 1] = 0x00;
	}
	else {
		strcpy(spaces, message);
	}

	snprintf(buffer, max_len_line, "%s%s%s", left_paragraph, spaces, right_paragraph);


	PEM_info(LC_PRINT, "%s\n", buffer);
}

/**
 * Print a pre formatted banner with title and line contents if available.
 *
 * @param fmt a list of arguments to be printed on the screen.
 *            %t - Title of the Banner
 *            %l - New Line of the Banner
 *
 * @note Developers can use a list of "%l". This function will print a new
 *       line with the content plus top and bottom line.
 */
void print_banner(const char* fmt, ...) {
	const char* p;
	va_list list;
	va_start(list, fmt);

	for (p = fmt; *p != 0; p++) {
		if (*p != '%') {
			// Only print formatted values
			//PEM_printf("%c", *p);
			continue;
		}

		switch (*++p) {
			case 'n':
				{
					PEM_info(LC_PRINT, "%s\n", line);
				}
				break;
			case 'l':
				{
					char* s = va_arg(list, char*);
					print_banner_line(s);
				}
				break;
			case 't':
				{
					char* s = va_arg(list, char*);
					PEM_info(LC_PRINT, "%s\n", line);
					print_banner_line(s);
					PEM_info(LC_PRINT, "%s\n", line);
				}
				break;
			default:
				{
					PRINT_CONFIG_MSG("Invalid value to be printed @%d in %s\n", __LINE__, __FILE__);
				}
		}
	}

	va_end(list);
}




/*!
 * \brief 		Initialize Log Module
 *
 * \return
 * 	0 LOG_DISABLED
 * 	1 LOG_ENABLED
 *
 * \par Remarks
 * TV MODE        LOG LEVEL
 * EVENT/DEBUG    LOG_ENABLED
 * RELEASE        LOG_DISABLED
 *
 * \par Example
 * \code
 *     PEM_logInit();
 * \endcode
 */
int PEM_logInit(const char* title) {
	SINT8	res             = -1;
	int	enableLog       = 0;
	int	enableLogEngine = 0;
	int	logConfig       = LC_PRINT;
	int	logLevel        = 0;

{
	char buffer[max_len_line];
	sprintf(buffer, "Initialize PEM_log (%s)", (title == 0 ? "" : title));
	print_banner("%l", buffer);
}

	// TODO The path should be set by any caller!
	char configPath[] = { "/mnt/lg/cmn_data/ginga_mw_storage/ginga_ncl.config" };

	loadConfigFile(configPath, &launcherConfig);

	PEM_enableLog(launcherConfig.enableLogLauncher);
	PEM_changeLogLevel(launcherConfig.logLevel);
	PEM_enableColor(launcherConfig.enableColor);
	PEM_setLogConfig(launcherConfig.logMask);

	return launcherConfig.enableLogEngine;
}

/*!
 * \brief
 *
 * \param[i]
 * \return		None
 * \par Remarks
 *			None
 *
 * \par Example
 * \code
 *	// Log Config: (7c080000) base 16, (2080899072) base 10
 *	PEM_setLogConfig(LC_LAUNCHER | LC_PORT | LC_MWAGT | LC_BRIDGE | LC_TVAGT | LC_PRINT | LC_OSA);
 * \endcode
 */
void PEM_setLogConfig(UINT32 logconfig) {
	LOG_CONFIG = logconfig;
}

/*!
 * \brief
 *
 * \param[in]
 *
 * \return 		None
 *
 * \par Remarks
 * 			None
 *
 * \par Example
 * \code
 * 		PEM_changeLogLevel(LOG_FORMAT_SIMPLE);
 * 		PEM_changeLogLevel(LOG_FORMAT_TRACE);
 * 		PEM_changeLogLevel(LOG_FROMAT_TIME);
 * \endcode
 */
void PEM_changeLogFormat(pem_log_format format) {
	switch(format) {
		case LOG_FORMAT_SIMPLE:
			PEM_printf(">< | Change Log Format | : change to LOG_FORMAT_SIMPLE \n");
			break;
		case LOG_FORMAT_TRACE:
			PEM_printf(">< | Change Log Format | : change to LOG_FORMAT_TRACE \n");
			break;
		case LOG_FORMAT_TIME:
			PEM_printf(">< | Change Log Format | : change to LOG_FORMAT_TIME \n");
			break;
		default:
			PEM_printf(">< | Change Log Format | : Invaild Format Value 0x%X\n", format);
			PEM_printf(">< | Change Log Format | : change to Default Value(LOG_FORMAT_TIME)\n");
			format = LOG_FORMAT_SIMPLE;
	}
	CURRENT_LOG_FORMAT = format;
}

/*!
 * \brief
 *
 * \param[in]
 *
 * \return 		None
 *
 * \par Remarks
 * 				None
 *
 * \par Example
 * \code
 * 		PEM_changeLogLevel(LOG_WARN);
 * 		PEM_changeLogLevel(LOG_INFO);
 * \endcode
 */
void PEM_changeLogLevel(pem_log_level level) {
	pem_log_level beforeLevel = CURRENT_LOG_LEVEL;

	if (level < LOG_FATAL || level > LOG_DEBUG) {
		CURRENT_LOG_LEVEL = LOG_FATAL;
		PEM_printf(">< | Change Log Level Error | : Invalid Log Level|0x%X|\n", level);
		PEM_printf(">< | Change Log Level Error | : so set Log Level to default|FATAL|\n");
	} else {
		CURRENT_LOG_LEVEL = level;
	}

	PEM_printf(">< | Change Log Level | : |%s| --> |%s|\n", PEM_getLogLevelName(beforeLevel),
								PEM_getLogLevelName(CURRENT_LOG_LEVEL));
}

/*!
 * \brief
 *
 * \param[in]
 *
 * \return
 *
 * \par Remarks
 * 				None
 *
 * \par Example
 * \code
 * 		printf("LogLevel = %s\n", PEM_getLogLevelName(DEBUG);
 * \endcode
 */
const char *PEM_getLogLevelName(pem_log_level level) {
	switch(level) {
		case LOG_FATAL : 	return "FATAL";
		case LOG_ERROR : 	return "ERROR";
		case LOG_WARN  : 	return "WARN ";
		case LOG_INFO  : 	return "INFO ";
		case LOG_DEBUG : 	return "DEBUG";
	}
	return "";
}

/*!
 * \brief
 *
 * \param[in]
 *
 * \return
 *
 * \par Remarks
 * 				None
 *
 * \par Example
 * \code
 * 		printf("LogCategory = %s\n", PEM_getCategoryName(LC_ServiceContext);
 * \endcode
 */
const char *PEM_getCategoryName(pem_log_category category) {
	switch(category) {
		case LC_BRIDGE:		return "     NBRIDGE";
		case LC_PORT:		return "        PORT";
		case LC_LAUNCHER:	return "    LAUNCHER";
		case LC_OSA:		return "         OSA";
		case LC_MWAGT:		return "ENGINE_AGENT";
		case LC_TVAGT:		return "    TV_AGENT";
		case LC_PRINT:		return "       PRINT";
		case LC_EMPTY1:		return "      EMPTY1";
		case LC_EMPTY2:		return "      EMPTY2";
		default:
			PEM_printf(">< | Error | : Invalid Category name |0x%X|. Ignoring...\n", category);
	}
	return "";
}

/*!
 * \brief
 *
 * \param[in]	category
 * \param[in]	level
 * \param[in]	fmt
 * \param[in]	...
 *
 * \return  	None
 *
 * \par Remarks
 * 				None
 *
 * \par Example
 * \code
 * 		PEM_writeLog("main.c", 30, "testFun", LC_SM, LOG_ERROR, "%s = %d", "Value ", 10);
 * \endcode
 */
void PEM_writeLog(UINT32 category, UINT32 level, const char *fmt, va_list argp) {
	// The length of 'buf' should be the sum of the length of 'buf2' and (prefix + postfix).
	// The following code is bad approach.
	SINT8	buf[MAX_LOG_BUFFER  + 1] = {0, };
	SINT8	buf2[MAX_LOG_BUFFER + 1] = {0, };
	SINT32	maxForLog  = 0;
	SINT32	lenToAdded = 0;
	static	UINT8*	postfix = (UINT8*) COLOR_LOG_END;

	if (LOG_ENABLE == 0) {
		return ;
	}

	if (level > CURRENT_LOG_LEVEL) {
		return;
	}

	if (((LOG_CONFIG & category) == 0) && level > LOG_ERROR) {
		return;
	}

	// ex) "xxxxxxxxxx...xxxxxx\0\0"
	//      +- index = 0        | +- index = MAX_LOG_BUFFER
	//                          +--- index = MAX_LOG_BUFFER - 1
	//     So the maximum number of characters to be printed including "\0" is MAX_LOG_BUFFER - 1.
	buf[MAX_LOG_BUFFER]  = '\0';		// dummy null character for safty
	buf2[MAX_LOG_BUFFER] = '\0';		// dummy null character for safty

	maxForLog = MAX_LOG_BUFFER - 1 - strlen(postfix);	// '-1' means the end of string ('\0').

	switch(category) {
		case LC_PORT:
		case LC_LAUNCHER:
		case LC_OSA:
		case LC_MWAGT:
		case LC_TVAGT:
		case LC_BRIDGE:
			if (LOG_COLORS) {
				switch(level) {
					case LOG_FATAL : snprintf(buf, maxForLog, COLOR_LOG_FATAL "B>|%s| |FATAL| : ", PEM_getCategoryName(category)); break;
					case LOG_ERROR : snprintf(buf, maxForLog, COLOR_LOG_ERROR "B>|%s| |ERROR| : ", PEM_getCategoryName(category)); break;
					case LOG_WARN  : snprintf(buf, maxForLog, COLOR_LOG_WARN  "B>|%s| |WARN | : ", PEM_getCategoryName(category)); break;
					case LOG_INFO  : snprintf(buf, maxForLog, COLOR_LOG_INFO  "B>|%s| |INFO | : ", PEM_getCategoryName(category)); break;
					case LOG_DEBUG : snprintf(buf, maxForLog, COLOR_LOG_DEBUG "B>|%s| |DEBUG| : ", PEM_getCategoryName(category)); break;
				}
			}
			else {
				switch(level) {
					case LOG_FATAL : snprintf(buf, maxForLog, "B>|%s| |FATAL| : ", PEM_getCategoryName(category)); break;
					case LOG_ERROR : snprintf(buf, maxForLog, "B>|%s| |ERROR| : ", PEM_getCategoryName(category)); break;
					case LOG_WARN  : snprintf(buf, maxForLog, "B>|%s| |WARN | : ", PEM_getCategoryName(category)); break;
					case LOG_INFO  : snprintf(buf, maxForLog, "B>|%s| |INFO | : ", PEM_getCategoryName(category)); break;
					case LOG_DEBUG : snprintf(buf, maxForLog, "B>|%s| |DEBUG| : ", PEM_getCategoryName(category)); break;
				}
			}
			break;

		case LC_PRINT:
			if (LOG_COLORS) {
				switch(level) {
					// all fatal color
					case LOG_FATAL : snprintf(buf, maxForLog, COLOR_LOG_FATAL "B>|%s| |FATAL| : ", PEM_getCategoryName(category)); break;
					case LOG_ERROR : snprintf(buf, maxForLog, COLOR_LOG_ERROR "B>|%s| |ERROR| : ", PEM_getCategoryName(category)); break;
					case LOG_WARN  : snprintf(buf, maxForLog, COLOR_LOG_WARN  "B>|%s| |WARN | : ", PEM_getCategoryName(category)); break;
					case LOG_INFO  : snprintf(buf, maxForLog, COLOR_LOG_INFO  "B>|%s| |INFO | : ", PEM_getCategoryName(category)); break;
					case LOG_DEBUG : snprintf(buf, maxForLog, COLOR_LOG_DEBUG "B>|%s| |DEBUG| : ", PEM_getCategoryName(category)); break;
				}
			}
			else {
				switch(level) {
					// all fatal color
					case LOG_FATAL : snprintf(buf, maxForLog, "B>|%s| |FATAL| : ", PEM_getCategoryName(category)); break;
					case LOG_ERROR : snprintf(buf, maxForLog, "B>|%s| |ERROR| : ", PEM_getCategoryName(category)); break;
					case LOG_WARN  : snprintf(buf, maxForLog, "B>|%s| |WARN | : ", PEM_getCategoryName(category)); break;
					case LOG_INFO  : snprintf(buf, maxForLog, "B>|%s| |INFO | : ", PEM_getCategoryName(category)); break;
					case LOG_DEBUG : snprintf(buf, maxForLog, "B>|%s| |DEBUG| : ", PEM_getCategoryName(category)); break;
				}
			}
			break;

		default :
			snprintf(buf, maxForLog, "B>|            | : ");
			return;
	}

	vsnprintf(buf2, maxForLog, fmt, argp);

	lenToAdded = maxForLog - strlen(buf);
	strncat(buf, buf2, (lenToAdded < 0 ? 0 : lenToAdded));

	if (LOG_COLORS) {
		strcat(buf, postfix);
	}

	PEM_printf(buf);
}

void PEM_log(UINT32 category_, UINT32 level_, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(category_, level_, fmt, argp);
	va_end(argp);
}

void PEM_debug(UINT32 category_, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(category_, LOG_DEBUG, fmt, argp);
	va_end(argp);
}

void PEM_info(UINT32 category_, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(category_, LOG_INFO, fmt, argp);
	va_end(argp);
}

void PEM_warn(UINT32 category_, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(category_, LOG_WARN, fmt, argp);
	va_end(argp);
}

void PEM_error(UINT32 category_, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(category_, LOG_ERROR, fmt, argp);
	va_end(argp);
}

void PEM_fatal(UINT32 category_,  const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(category_, LOG_FATAL, fmt, argp);
	va_end(argp);
}

void PEM_OSA_log(UINT32 level_, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(LC_OSA, level_, fmt, argp);
	va_end(argp);
}

void PEM_PORT_log(UINT32 level_, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(LC_PORT, level_, fmt, argp);
	va_end(argp);
}

void PEM_LAUNCHER_log(UINT32 level_, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(LC_LAUNCHER, level_, fmt, argp);
	va_end(argp);
}

void PEM_MWAGT_log(UINT32 level_, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(LC_MWAGT, level_, fmt, argp);
	va_end(argp);
}

void PEM_BRIDGE_log(UINT32 level_, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(LC_BRIDGE, level_, fmt, argp);
	va_end(argp);
}

void PEM_TVAGT_log(UINT32 level_, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	PEM_writeLog(LC_TVAGT, level_, fmt, argp);
	va_end(argp);
}

UINT8 PEM_isEnableLog(UINT32 category_, UINT32 level_)
{
	if ((LOG_CONFIG & category_) &&  level_ <= CURRENT_LOG_LEVEL) {
		return 1;
	}

	return 0;
}

void PEM_enableLog(int on) {
	LOG_ENABLE = on;
}

void PEM_enableColor(int on) {
	LOG_COLORS = on;
}

void PEM_printf(const char* fmt, ...) {
	va_list argp;

	va_start(argp, fmt);
	vprintf(fmt, argp);
	va_end(argp);
}

