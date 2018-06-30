#include <cstdio>

#include <sys/prctl.h>

#include <boost/thread.hpp>

#include <signal.h>

#include <pem-tv-util/pem_log.h>

#include <ginga_api.h>

#include "pem_bind_references.h"

#include "configuration.h"
#include "browser_agent.h"
#include "pem_util.h"

static void initLimeB ( int, char ** );
static void finishLimeB ( void );
static void runEngine ( int, char ** );
static void sig_handler ( int );

static char _winId [ 64 ];
static char _size [ 64 ];

/******************************************************************************
 *                                                                            *
 *                      Main Entry for PEM Launcher                           *
 *                                                                            *
 ******************************************************************************/
static char* getValueRefFromArgs ( const char* psName, const int argc, const char** argv ) {
    char* p = NULL;
    int i;

    for ( i = 1; i < argc; ++i ) {
        p = strstr ( ( char* ) argv [ i ], psName );
        if ( p ) {
            p += strlen ( psName );
            if ( *p == '=' ) {
                ++p;
                break;
            }
            else if ( *p == '\0' )
                break;
        }
    }
    return p;
}

static int getWinIdFromArgs ( int* pWinId, const int argc, const char** argv ) {
    char* p = getValueRefFromArgs ( "--winid", argc, argv );

    if ( p && *p == '0' && tolower ( * ( p + 1 ) ) == 'x' ) {
        // don't use strtol() because some compilier make error on sign bit
        long long t = strtoll ( p, NULL, 0 );
        *pWinId = ( int ) t;
        return 1;
    }
    return 0;
}

static int getWinResFromArgs ( int* pnResWidth, int* pnResHeight, const int argc, const char** argv ) {
    char* p = getValueRefFromArgs ( "--size", argc, argv );
    if ( p && isdigit ( *p ) ) {
        char buf [ 16 ];
        char* t = strchr ( p, 'x' );
        if ( !t )
            t = strchr ( p, 'X' );
        if ( t ) {
            strncpy ( buf, p, t - p );
            buf [ t - p ] = '\0';
            *pnResWidth = atoi ( buf );
            *pnResHeight = atoi ( t + 1 );
            return 1;
        }
    }
    return 0;
}

static int setEnvFromArgs ( const int argc, const char** argv ) {
    int winId, width, height;
    if ( !getWinIdFromArgs ( &winId, argc, argv ) ) {
        return 0;
    }

    if ( !getWinResFromArgs ( &width, &height, argc, argv ) ) {
        return 0;
    }

    // set env var "HTV_GM_WINDOW_ID", "HTV_GM_WINDOW_WIDTH", "HTV_GM_WINDOW_HEIGHT"
    // for GLES settings in gtk
    char buf [ 32 ];
    snprintf ( buf, 32, "%d", winId );
    setenv ( "HTV_GM_WINDOW_ID", ( const char* ) buf, TRUE );
    snprintf ( buf, 32, "%d", width );
    setenv ( "HTV_GM_WINDOW_WIDTH", ( const char* ) buf, TRUE );
    snprintf ( buf, 32, "%d", height );
    setenv ( "HTV_GM_WINDOW_HEIGHT", ( const char* ) buf, TRUE );

    snprintf ( _winId, 64, "--winid=0x%x", winId );
    snprintf ( _size, 64, "--size=%dx%d", width, height );

    return 1;
}

int main ( int argc, char *argv [ ] ) {
    TV_printf ( "Initializing PEM Log manager...\n" );
    int iRet = PEM_logInit ( ( const char* ) "GINGA_NCL-MW-ENGINE" );

    PEM_info ( LC_PRINT, "Setting up signals...\n" );
    setupSignals ( );

    // CC could send SIGTERM to kill this process. It's not an issue!
    //signal ( SIGTERM, sig_handler );

    PEM_info ( LC_PRINT, "Agent is initializing\n" );

    BA_init ( );
    BA_initMsgEnv ( );
    if ( setEnvFromArgs ( argc, ( const char** ) argv ) == 0 ) {
        PEM_warn ( LC_PRINT, "Parameters are invalid!\n" );
    }

    //Main thread name
    prctl ( PR_SET_NAME, "tNCLmain", 0, 0, 0 );

    PEM_info ( LC_PRINT, "Engine is initializing...\n" );
    PEM_info ( LC_PRINT, "Log Initialization: [%s]\n", ( iRet ? "SUCCESS" : "FAIL" ) );
    if ( iRet ) {
        int new_argc = 4;
        char *argv_debug [ ] = { ( char* ) "NCLengine", _winId, _size, ( char* ) "--debug=all.Port.all" };
        initLimeB ( new_argc, argv_debug );

    }
    else {
        int new_argc = 3;
        char *argv_release [ ] = { ( char* ) "NCLengine", _winId, _size };

        initLimeB ( new_argc, argv_release );
    }

    PEM_info ( LC_PRINT, "Agent started! Running...\n" );
    BA_main ( );

    PEM_info ( LC_PRINT, "Agent stopped!\n" );
    BA_finish ( );

    PEM_info ( LC_PRINT, "Agent was finish...\n" );
    finishLimeB ( );
    return 0;
}


/******************************************************************************
 *                                                                            *
 * ----------------- Browser Presentation Engine Initialization --------------*
 *                                                                            *
 ******************************************************
 ******************************************************
 ******************************************************
 ******************************************************************************/
static void initLimeB ( int argc, char *argv [ ] ) {
    char buffer [ 80 ];

    sprintf ( buffer, "Ginga MW : NCL Initialization Started" );
    print_banner ( "%t", ( const char* ) buffer );

    sprintf ( buffer, "Build Release Date: %s", BUILD_RELEASE_DATE );
    print_banner ( "%l", ( const char* ) buffer );

    sprintf ( buffer, "   BRANCH Revision: %s", BUILD_GIT_REVISION );
    print_banner ( "%l", ( const char* ) buffer );

    sprintf ( buffer, "      TAG Revision: %s", BUILD_TAG_REVISION );
    print_banner ( "%l", ( char* ) buffer );

    print_banner ( "%n" );

    boost::thread ( boost::bind ( &runEngine, argc, argv ) );
}

/******************************************************************************
 *                                                                            *
 * ------------------ Browser Presentation Engine Finalization ---------------*
 *                                                                            *
 ******************************************************************************/
static void finishLimeB ( void ) {
    char buffer [ 80 ];
    sprintf ( buffer, "Ginga MW : NCL Finalization was done!" );
    print_banner ( "%t", ( const char* ) buffer );
}

static void runEngine ( int argc, char *argv [ ] ) {
    PEM_info ( LC_PRINT, "PEM_launcher thread created\n" );
    browser_pem_execute ( argc, argv );
}

static void sig_handler ( int signum ) {
    PEM_info ( LC_PRINT, "Request from Process Manager. Signal Number: [%d]\n", signum );

    PEM_info ( LC_PRINT, "GingaNCL Agent is stopping!\n" );
    BA_finish ( );
    PEM_info ( LC_PRINT, "GingaNCL Agent has stopped!\n" );

    finishLimeB ( );
    exit ( 1 );
}

