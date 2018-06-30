//LGESP 2013.05.10
#pragma once
#ifndef _GINGA_API_H_
#define _GINGA_API_H_
#include <stdio.h>
#include <string>
#include <execinfo.h>

#if 0

#define APRINT(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_ERROR(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_UTIL(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_BRIDGE(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_PLAYER(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_CANVAS(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_PROPERTY(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_LUA(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_MSGQ(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_DSMCC(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_GRAPHICS(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_NCL3(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_NCLC(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_NCLP(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_KEYS(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define APRINT_MEDIA(fmt,...) fprintf(stderr,"[%s][%s][%d]" fmt "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)

#endif

#ifndef APRINT
#define APRINT(fmt,...)
#endif

#ifndef APRINT_ERROR
#define APRINT_ERROR(fmt,...)
#endif

#ifndef APRINT_BRIDGE
#define APRINT_BRIDGE(fmt,...)
#endif

#ifndef APRINT_PLAYER
#define APRINT_PLAYER(fmt,...)
#endif

#ifndef APRINT_CANVAS
#define APRINT_CANVAS(fmt,...)
#endif

#ifndef APRINT_PROPERTY
#define APRINT_PROPERTY(fmt,...)
#endif

#ifndef APRINT_LUA
#define APRINT_LUA(fmt,...)
#endif

#ifndef APRINT_MSGQ
#define APRINT_MSGQ(fmt,...)
#endif

#ifndef APRINT_DSMCC
#define APRINT_DSMCC(fmt,...)
#endif

#ifndef APRINT_GRAPHICS
#define APRINT_GRAPHICS(fmt,...)
#endif

#ifndef APRINT_NCL3
#define APRINT_NCL3(fmt,...)
#endif

#ifndef APRINT_NCLC
#define APRINT_NCLC(fmt,...)
#endif

#ifndef APRINT_NCLP
#define APRINT_NCLP(fmt,...)
#endif

#ifndef APRINT_KEYS
#define APRINT_KEYS(fmt,...)
#endif

#ifndef APRINT_MEDIA
#define APRINT_MEDIA(fmt,...)
#endif


#define GET_ID(_var) (_var)==NULL?"NULL":(_var)->getId().c_str()

#define LGESP_INTEGRATION_STUB_IMPL() \
	int browser_engine_loadMediaClip(char *url,long *handlerId) {return 0;}\
	int browser_engine_playMedia(long handlerId) {return 0;} \
	int browser_engine_stopMedia(long handlerId) {return 0;} \
	int browser_engine_closeMedia(long handlerId) {return 0;} \
	int browser_engine_freezeMedia(long handlerId) {return 0;} \
	int browser_engine_resumeMedia(long handlerId) {return 0;} \
	int browser_engine_resizeMedia(long handlerId,int srcX, int srcY,int srcW, int srcH,int destX, int destY, int destW, int destH) {return 0;} \
	int onFetchFilesFromCarousel(const std::string &a) {return 0;}



#define PRINT_STACK_TRACE {                                            \
    const int max_size = 128;                                          \
    void *array[max_size];                                             \
    size_t size = backtrace (array, max_size);                         \
    char** strings = backtrace_symbols (array, size);                  \
    if (strings != NULL) {                                             \
        printf ("--[ PRINT_STACK_TRACE ]--------------------\n");      \
        printf ("- Obtained %zd stack frames.\n", size);               \
        for (size_t i = 0; i < size; i++) {                            \
            printf ("    %3zd/%3zd - %s\n", i, size, strings[i]);      \
        }                                                              \
        printf ("--------------\n\n");                                 \
        free (strings);                                                \
    }                                                                  \
    else {                                                             \
        printf ("PRINT_STACK_TRACE could not generate stacktrace!\n"); \
    }                                                                  \
}                                                                      \


#define GINGA_SUCCESS    0
#define GINGA_ERROR     -1


//DSMCC
//------------------------------------------------------------------
int onFetchFilesFromCarousel (const std::string &);

//Media API
//------------------------------------------------------------------
int browser_engine_loadMediaClip(char *url,long *handlerId);
int browser_engine_playMedia(long handlerId);
int browser_engine_stopMedia(long handlerId);
int browser_engine_closeMedia(long handlerId);
int browser_engine_freezeMedia(long handlerId);
int browser_engine_resumeMedia(long handlerId);
int browser_engine_resizeMedia(long handlerId,int srcX, int srcY,int srcW, int srcH,int destX, int destY, int destW, int destH);


#endif
//_GINGA_API_H_
