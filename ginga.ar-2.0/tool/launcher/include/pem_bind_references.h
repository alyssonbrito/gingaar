#ifndef __PEM_BIND_REFERENCES_H__
#define __PEM_BIND_REFERENCES_H__

#include <string>
#include <cstring>

#include <sched.h>
#include <pthread.h>

#include <boost/thread.hpp>
#include <gtk/gtk.h>
#include <sys/prctl.h>


#include <pem-tv-util/pem_log.h>
#include <pem-tv-util/ginga_n_port_data.h>


#include "BrowserPresentationEngine.h"
#include "pem_util.h"
#include "gingancl_bridge.h"
#include "browser_port_mw2tv.h"

#include "pem_worker_key.h"

#include "pem_callbacks.h"


/*
 *
 * Type:
 *   USER_PROPERTY_CHANGED = 1
 *   USER_PROPERTY_REMOVED = 2
 *
 */
#define USER_PROPERTY_CHNAGED 1
#define USER_PROPERTY_REMOVED 2



typedef enum _KEY_MASK_INFO {
		GINGA_KEY_NONE			= 0x00000000,
		GINGA_KEY_ALL			= 0x00000001,
		GINGA_KEY_NUMERIC		= 0x00000002,
		GINGA_KEY_ARROW			= 0x00000004,
		GINGA_KEY_CONFIRM		= 0x00000010,
		GINGA_KEY_COLOURED		= 0x00000020,
		GINGA_KEY_BACK			= 0x00000800,
		GINGA_KEY_EXIT			= 0x00001000,
} KEY_MASK_INFO;


void printMsgAppExecInfo(MSG_APP_EXEC_INFO *);
void printAfterN2TVattach(SINT8 *, UINT32);



int loadMedia(const std::string &url, int *handlerId, int mediaType);
int playMedia(int handlerId);
int setMediaTime(int handlerId, int time);
int stopMedia(int handlerId);
int closeMedia(int handlerId);
int freezeMedia(int handlerId);
int resumeMedia(int handlerId);
int resizeMedia(int x, int y, int w, int h,int handlerId);



void notify_pem_can_stop(MSG_APP_EXEC_INFO *);
void notify_pem_can_run(MSG_APP_EXEC_INFO *);
int notify_pem_putKey(unsigned int, unsigned int);
int notify_pem_StreamEvent(SINT32 handlerId);
int notify_pem_notifyUserPropertyChanged(unsigned int changeType, char * key, char *oldValue, char * newValue);
int notify_pem_notifyGetNativeProperty(char * value);
int notify_pem_recommended_resolution_changed(UINT16, UINT16);
int notify_pem_media_stop(SINT32 handlerId);
SINT8 notify_pem_media_status(	SINT32 handlerId,
				SINT32 typeOfNotification,
				SINT32 typeOfEvent,
				SINT32 var1,
				UINT32 MMediaTime,
				UINT32 LMediaTime,
				UINT8 niid,
				UINT16 onid,
				SINT32 tsid,
				UINT16 sid,
				UINT8 *cTags,
				UINT8 numOfCTags);



SINT8 mediaProperty( const int op, const int handlerId, const int property, unsigned int *value );



// Runnable for pthread
void presentationManagerTask();


void browser_pem_execute(int argc, char *argv[]);

#endif /* __PEM_BIND_REFERENCES_H__ */

