/*
 * pem_util.cpp
 *
 * Created on: 21/08/2013
 *     Author: douglas.lima
 */

#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>
#include <execinfo.h>

#ifdef UNWIND_BACKTRACE
#include <libunwind.h>
#endif

#include <pem-tv-util/pem_log.h>

#include "pem_bind_references.h"
#include "ginga_api.h"
#include "configuration.h"
#include "browser_agent.h"
#include "pem_util.h"

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
#else
#include"gm_openapi.h"
#endif
#endif

#if defined(WEBOS_USE_GM)
static void* _gmHandle = NULL;
static bool  _bGmInit  = false;

static GM_STATUS_T (*GM_SetWindowResolution_func)(signed int windowID,
						  unsigned short width,
						  unsigned short height);
static GM_STATUS_T (*GM_SetWindowVisibility_func)(signed int windowID,
						  unsigned int bVisibility);

static void _unloadGM() {

	_bGmInit = false;

	if (!_gmHandle) {
		PEM_error(LC_PRINT, "[%s::%d] GM Handler is invalid!\n", __FUNCTION__, __LINE__);
		return;
	}

	if (GM_SetWindowVisibility_func) {
		GM_WINDOW_ID_T winId = GM_WINDOW_INVALID;

		char* pWinIdStr = getenv("HTV_GM_WINDOW_ID");
		if (pWinIdStr) {
			winId = (GM_WINDOW_ID_T) atoi(pWinIdStr);
			GM_SetWindowVisibility_func(winId, FALSE);
		}
		else {
			PEM_error(LC_PRINT, "[%s::%d] HTV_GM_WINDOW_ID environment variable is not set or invalid.\n",
					    __FUNCTION__, __LINE__);
		}
	}

	GM_SetWindowVisibility_func = NULL;
	GM_SetWindowResolution_func = NULL;

	dlclose(_gmHandle);
	_gmHandle = NULL;
}

static bool _loadGM() {
	if (_bGmInit) {
		PEM_warn(LC_PRINT, "[%s::%d] GM is already initialized!\n", __FUNCTION__, __LINE__);
		return true;
	}

	_gmHandle = dlopen(LIBGM_FILE_NAME, RTLD_LAZY | RTLD_GLOBAL);
	if (!_gmHandle) {
		_unloadGM();
		PEM_error(LC_PRINT, "[LGDeviceAdapter::%s::%d] dlopen %s failed - errno(%d)\n",
				    __FUNCTION__, __LINE__,
				    LIBGM_FILE_NAME, errno);
		return false;
	}

	GM_SetWindowResolution_func = (GM_STATUS_T (*)(signed int, unsigned short, unsigned short)) dlsym(_gmHandle, "GM_SetWindowResolution");
	if (!GM_SetWindowResolution_func) {
		_unloadGM();
		PEM_error(LC_PRINT, "[LGDeviceAdapter::%s::%d] dlsym %s failed - errno(%d)\n",
				    __FUNCTION__, __LINE__,
				    "GM_SetWindowResolution", errno);
		return false;
	}

	GM_SetWindowVisibility_func = (GM_STATUS_T (*)(signed int, unsigned int))dlsym(_gmHandle, "GM_SetWindowVisibility");
	if (!GM_SetWindowVisibility_func) {
		_unloadGM();
		PEM_error(LC_PRINT, "[LGDeviceAdapter::%s::%d] dlsym %s failed - errno(%d)\n",
				    __FUNCTION__, __LINE__,
				    "GM_SetWindowVisibility", errno);
		return false;
	}

	_bGmInit = true;
	return true;
}

static GM_STATUS_T GM_SetWindowResolution(signed int windowID,
					  unsigned short width,
					  unsigned short height) {
	if (_loadGM()) {
		return GM_SetWindowResolution_func(windowID, width, height);
	}

	return GM_FAILED;
}

static GM_STATUS_T GM_SetWindowVisibility(signed int windowID,
					  unsigned int bVisibility) {
	GM_STATUS_T ret = GM_FAILED;

	if (_loadGM()) {
		ret = GM_SetWindowVisibility_func(windowID, bVisibility);
	}

	if (!bVisibility) {
		_unloadGM();
	}

	return ret;
}
#endif /* WEBOS_USE_GM */



bool initializeGraphic(int width, int height) {
#if defined(WEBOS_USE_GM)
	GM_WINDOW_ID_T winId = GM_WINDOW_INVALID;
	char* pWinIdStr = getenv("HTV_GM_WINDOW_ID");

	if (pWinIdStr) {
		winId = (GM_WINDOW_ID_T)atoi(pWinIdStr);
	}

	PEM_debug(LC_PRINT, "[LGDeviceAdapter::%s::%d] Screen Size = (%d x %d), Window ID = [%d]\n",
			__FUNCTION__, __LINE__, width, height, winId);

	if (winId != GM_WINDOW_INVALID) {
		if(GM_SetWindowResolution(winId, width, height) != GM_OK) {
			PEM_error(LC_PRINT, "[%s::%d] GM_SetWindowResolution failed.\n",
					    __FUNCTION__, __LINE__);
			return false;
		}

		if(GM_SetWindowVisibility(winId, TRUE) != GM_OK) {
			PEM_error(LC_PRINT, "[%s::%d] GM_SetWindowVisibility(Enable) failed.\n",
					    __FUNCTION__, __LINE__);
			return false;
		}
	}
	else {
		PEM_error(LC_PRINT, "[%s::%d] HTV_GM_WINDOW_ID environment variable is not set or invalid.\n",
				    __FUNCTION__, __LINE__);
		return false;
	}
#endif /* WEBOS_USE_GM */
	PEM_debug(LC_PRINT, "[%s::%d] Initializing Graphic: Screen Size = (%d x %d)...\n",
						 __FUNCTION__, __LINE__,
						 width, height);
	return true;
}



bool finalizeGraphic() {
#if defined(WEBOS_USE_GM)
	GM_WINDOW_ID_T winId = GM_WINDOW_INVALID;
	char* pWinIdStr = getenv("HTV_GM_WINDOW_ID");

	if (pWinIdStr) {
		winId = (GM_WINDOW_ID_T)atoi(pWinIdStr);
	}

	if (winId != GM_WINDOW_INVALID) {
		if (GM_SetWindowVisibility(winId, FALSE) != GM_OK) {
			PEM_error(LC_PRINT, "[%s::%d] GM_SetWindowVisibility(Disable) failed.\n",
					    __FUNCTION__, __LINE__);
			return false;
		}
	}
	else {
		PEM_error(LC_PRINT, "[%s::%d] HTV_GM_WINDOW_ID environment variable is not set or invalid.\n",
				    __FUNCTION__, __LINE__);
		return false;
	}
#endif /* WEBOS_USE_GM */
	PEM_debug(LC_PRINT, "[%s::%d] Finalizing Graphic...\n",	__FUNCTION__, __LINE__);
	return true;
}



/******************************************************************************
 *                                                                            *
 *                               Utilities                                    *
 *                                                                            *
 ******************************************************************************/
int startsWith(const char* msg, const char* prefix) {
	char* sub = NULL;

	if (msg == NULL || prefix == NULL) {
		return -1;
	}
	sub = (char *) strstr(msg, prefix);

	if (sub != NULL) {
		if (msg == sub) {
			return 0;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}


/**
 *
 *
 * case 1:
 *     mount point : /tmp/ginga_oc_root/a.2afa/1854.1854.59328/64.2
 *     base_dir	: /
 *     init_entity : RebeldeTemp2_NCL/main.ncl
 *
 *     Application Absolute Path:
 *         wrong value: check double slash between "64.2" and "RebeldeTemp2_NCL".
 *             /tmp/ginga_oc_root/a.2afa/1854.1854.59328/64.2//RebeldeTemp2_NCL/main.ncl
 *
 *         correct value: slash was managed.
 *             /tmp/ginga_oc_root/a.2afa/1854.1854.59328/64.2/RebeldeTemp2_NCL/main.ncl
 *
 *
 * case 2:
 *     mount point : /tmp/ginga_oc_root/a.2afa/1854.1854.59328/64.2
 *     base_dir	: /sokoban
 *     init_entity : sokoban.ncl
 *
 *     Application Absolute Path:
 *         /tmp/ginga_oc_root/a.3bb/1.1.60064/11.2/sokoban/sokoban.ncl
 *
 *
 */
int makeAbsolutePath(SINT8 *mountPoint, APP_EXEC_INFO *appExecInfo, char *absolutePath) {
	if (mountPoint == NULL || appExecInfo == NULL || absolutePath == NULL) {
		return FAILURE;
	}

	strcpy(absolutePath, (char*)mountPoint);
	if (appExecInfo->base_dir[0] != '/') {
		strcat(absolutePath, "/");
	}

	strcat(absolutePath, (char*)appExecInfo->base_dir);

	if (appExecInfo->init_entity[0] != '/' && absolutePath[strlen(absolutePath) - 1] != '/') {
		strcat(absolutePath, "/");
	}

	strcat(absolutePath, (char*)appExecInfo->init_entity);
	return SUCCESS;
}

bool both_slashes(char a, char b) {
    return a == '/' && b == '/';
}

unsigned long getThreadId(){
    std::string threadId = boost::lexical_cast<std::string>(boost::this_thread::get_id());
    unsigned long threadNumber = 0;
    sscanf(threadId.c_str(), "%lx", &threadNumber);
    return threadNumber;
}


/**
 * Print stack trace.
 */


#ifdef UNWIND_BACKTRACE

static size_t obtain_backtrace(void ** dest, size_t destsize, void * context)
{
	unw_cursor_t cursor;
	unw_context_t uc;
	unw_context_t * ctx;
	size_t i = 0;
	int ret;

	if (context) {
		ctx = (unw_context_t *) context;
	} else {
		if (unw_getcontext(&uc) < 0)
		return 0;
		ctx = &uc;
	}

	if (unw_init_local(&cursor, ctx) < 0)
	return 0;

	do {
		unw_word_t ip;
		unw_word_t sp;

		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		unw_get_reg(&cursor, UNW_REG_SP, &sp);
		dest[i++] = (void*) ip;
	}while ((ret = unw_step(&cursor)) > 0 && i < destsize);

	return i;
}

static void show_backtrace(void * context)
{
	void * addr[32];
	size_t i = obtain_backtrace(addr, 32, context);

	PEM_fatal(LC_PRINT, "=== STACK TRACE ====\n");
	backtrace_symbols_fd(addr, i, 2);
	PEM_fatal(LC_PRINT, "=== STACK TRACE DONE ====\n");
}

static void handler(int sig, siginfo_t * psi , void * ctxarg)
{
	ucontext_t * ucontext = (ucontext_t *)ctxarg;
	show_backtrace(ucontext);
	signal(sig, SIG_DFL);
	kill(getpid(), sig);
}

void setupSignals(void)
{
	struct sigaction sa;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	// sa.sa_flags = SA_RESTART | SA_SIGINFO;
	sa.sa_flags = SA_SIGINFO | SA_ONSTACK;

	sigaction(SIGABRT, &sa, NULL);
	sigaction(SIGSEGV, &sa, NULL);
}

#if 0
void printStackTrace()
{
	unw_cursor_t cursor;
	unw_context_t context;

	unw_getcontext(&context);
	unw_init_local(&cursor, &context);
	PEM_fatal(LC_PRINT, "-- stacktrace follows\n");
	while (unw_step(&cursor) > 0)
	{
		unw_word_t offset, pc;
		char fname[64];

		unw_get_reg(&cursor, UNW_REG_IP, &pc);

		fname[0] = '\0';
		(void) unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);

		PEM_fatal(LC_PRINT, "%p : (%s+0x%x) [%p]\n", pc, fname, offset, pc);
	}
	PEM_fatal(LC_PRINT, "-- stacktrace end\n");
}
#endif

#else


/* PRINT_SIGNAL_RET to print return value of signal function */
#if 0
#define PRINT_SIGNAL_RET(type, res)					\
	PEM_warn(LC_PRINT, "Setting signal for %s: %s\n",		\
			   (type),					\
			   (SIG_ERR == (res) ? "FAIL" : "SUCCESS"));
#else
#define PRINT_SIGNAL_RET(type, res) (res)
#endif



static void sig_handler(int);
static void printStackTrace();


/**
 * Initialize signals using my own handler.
 */
void setupSignals() {
	PRINT_SIGNAL_RET("SIGSEGV", signal(SIGSEGV, sig_handler));
	PRINT_SIGNAL_RET("SIGHUP",  signal(SIGHUP,  sig_handler));
// For GDB tool, "SIGINT" is very important to break the execution flow
//	PRINT_SIGNAL_RET("SIGINT",  signal(SIGINT,  sig_handler));
	PRINT_SIGNAL_RET("SIGILL",  signal(SIGILL,  sig_handler));
	PRINT_SIGNAL_RET("SIGABRT", signal(SIGABRT, sig_handler));

#if HTV_NC_VER < 5
// In WebOS case, Process Manager could send SIGTERM to kill
// this process. And it is not an error...
	PRINT_SIGNAL_RET("SIGTERM", signal(SIGTERM, sig_handler));
#endif /* HTV_NC_VER < 5 */

	PRINT_SIGNAL_RET("SIGCHLD", signal(SIGCHLD, sig_handler));
}

void printStackTrace() {
	void *array[256];
	size_t size = backtrace(array, 256);
	char **strings = backtrace_symbols(array, 256);
	if (strings != NULL) {
		PEM_fatal(LC_PRINT, "-- stacktrace follows (%zd frames):\n", size);
		for (size_t i = 0; i < size; i++) {
			PEM_fatal(LC_PRINT, "    %s\n", strings[i]);
		}
		PEM_fatal(LC_PRINT, "-- end of stacktrace\n");
		free(strings);
	} else {
		PEM_fatal(LC_PRINT, "printStackTrace: error, could not generate stacktrace!\n");
	}
}

/**
 * Abort when receive any registered signals. All registered signals are set to default.
 * If stacktrace is available, print it.
 */
void sig_handler(int signum) {

	printStackTrace();

	char sigstr[8] = { "\0" };
	signal(SIGSEGV, SIG_DFL);
	signal(SIGHUP,  SIG_DFL);
	// For GDB tool, "SIGINT" is very important to break the execution flow
	//signal(SIGINT,  SIG_DFL);
	signal(SIGILL,  SIG_DFL);
	signal(SIGABRT, SIG_DFL);

#if HTV_NC_VER < 5
// In WebOS case, Process Manager could send SIGTERM to kill
// this process. And it is not an error...
	signal(SIGTERM, SIG_DFL);
#endif /* HTV_NC_VER < 5 */

	signal(SIGCHLD, SIG_DFL);

	switch (signum) {
		case SIGSEGV: strcpy(sigstr, "SIGSEGV"); break;
		case SIGHUP:  strcpy(sigstr, "SIGHUP");  break;
		case SIGINT:  strcpy(sigstr, "SIGINT");  break;
		case SIGILL:  strcpy(sigstr, "SIGILL");  break;

		case SIGABRT: strcpy(sigstr, "SIGABRT"); break;

#if HTV_NC_VER < 5
		// In WebOS case, Process Manager could send SIGTERM to kill
		// this process. And it is not an error...
		case SIGTERM: strcpy(sigstr, "SIGTERM"); break;
#endif /* HTV_NC_VER < 5 */

		case SIGCHLD: strcpy(sigstr, "SIGCHLD"); break;
		default:      strcpy(sigstr, "UNKNOWN");
	}

	PEM_info(LC_PRINT, "\n\nReceived signal [%s (%d)]\n", sigstr, signum);
	exit(1);
	//abort();
}

#endif
