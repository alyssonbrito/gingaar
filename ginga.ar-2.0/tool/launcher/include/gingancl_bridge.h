/*
 * LGESP Brazil
 * Ginga Bridge implementation  
 *
 * author: alysson.brito@lge.com
 * date: 2013.09.12
 * */

#ifndef GINGA_NCL_BRIDGE__
#define GINGA_NCL_BRIDGE__

#include <pem-tv-util/pem_moa_common.h>
#include <pem-tv-util/ginga_n_port_data.h>
#include <pem-tv-util/pem_log.h>

#include <string>

#include "AppInfo.h"

#define PEM_BRIDGE_BEGIN(fmt,...)	PEM_BRIDGE_log(LOG_DEBUG, "[%s] : BEGIN [" fmt "]\n", __FUNCTION__, ##__VA_ARGS__)
#define PEM_BRIDGE_END(fmt,...)		PEM_BRIDGE_log(LOG_DEBUG, "[%s] : END [" fmt "]\n",   __FUNCTION__, ##__VA_ARGS__)
#define PEM_BRIDGE_DEBUG(fmt,...)	PEM_BRIDGE_log(LOG_DEBUG, "[%s::%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PEM_BRIDGE_FATAL(fmt,...)	PEM_BRIDGE_log(LOG_FATAL, "[%s::%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

/**
 *@return:
 -1 if failt
 playerId (>0)
 
 * */
SINT32 GINGA_ncl_bridge_createNCLPlayer(void);

/*
 *@return:
	none
 */
void GINGA_ncl_bridge_disposeNCLPlayer(SINT32 playerId);

#if 1
/*
 *@return:
	-1 in error
	0 in OK
 */
SINT32 GINGA_ncl_bridge_setBounds(int playerId, int screen_width, int  screen_height, int x1, int y1, int x2, int y2);
/*
 *@return:
	0. If it succeeds, -1 for indicate failures
 */
/*SINT32*/ void GINGA_ncl_bridge_setVisible(SINT32 playerId, BOOLEAN visible);
#endif

/**
 * @param:
 * [in] playerId: ncl player id
 * [in] coltrolCode NCL_CONTROL_CODE (NCL_SET_DOC, pause_doc, resume, stop)
 *
 *@result:
 * 0 if succeeds
 * -1 in case of errors
 */
SINT32 GINGA_ncl_bridge_controlDoc(SINT32 playerId, SINT32 controlCode, SINT8* param);

/**
 * @param:
 * playerId
 * propertyId
 * [out] value, property value, max 1024 length
 *@result:
 * 0 if succeeds
 * -1 in case of errors
 */
SINT32 GINGA_ncl_bridge_getPropertyValue(SINT32 playerId, SINT8* propertyId, SINT8 value[/*1024*/]);



/**
 * @param:
 * [in] playerId
 * [in] command: NCL_EDIT_COMMAND
 * [in] param1: string param 1, 1024, '\0' terminated
 * [in] param2: string param 1, 1024, '\0' terminated
 * [in] param3: string param 1, 1024, '\0' terminated
 *
 *@return
 * 0 if succeeds
 * -1 in case of errors
 *
 *
 * command 
 * add_region
 * remove_region
 * ...
 *
 *
 *
 */
SINT32 GINGA_ncl_bridge_edit(SINT32 playerId, SINT32 command, SINT8* param1, SINT8* param2, SINT8* param3);

/**
 *@return:
 * 0 if succeeds
 * -1 in case of errors
 *
 *  located in 7.2.4 table 12 of 15606-2
 *  system.langauge | caption | subtitel ..
 *  user.age | location | genre | xxx
 *  default.focusBorderColor | setBorderColor | xxx
 *  service.currentFocus | currentKeyMaster | xxx
 *  si.numberOfServices | numberOfParticlaServices | channelNumber
 *  channel.keyCapture | virtualKeyboard | keyboardBounds | xxx
 *  shared.xxx
 */
SINT32 GINGA_ncl_bridge_settingsNodesGetValue(SINT8* nodeId, SINT8* key, SINT8 value[/*1024*/]);

//------------------------------------------------------------------------------
//NCL Event handling in JavaXlet
//------------------------------------------------------------------------------


//
//gingaNCL Engine -> GingaCC -> Ginga_J Engien API
//

/*EMBEDDED NCL*/
/**
 *Notify the event of specified NCL player
 *start/pause,resume,stio,attribution_set
 * @param: 
 * playerId[in]: ncl player id
 * eventType[in] type of event (NCL_EVENT_TYPE)
 * value[out] the identified of the node or acnhor related to the event
 * t and always ends with '\0'
 * MAX 1024 
 */ 
void GINGA_ncl_bridge_notifyOfNCLPlayerEvent(SINT32 playerId, SINT32 eventType, SINT8 value[/*1024*/]);




/*EMBEDDED GINGA_J*/
//NCLet aplication

/**
 * Notify the vent of NCL environment 
 * @param
 * [in] eventType: NCL_EVENT_TYPE
 *@return
 * none
 */
void GINGA_ncl_bridge_notifyOfNCLEvent(SINT32 eventType, SINT8 value[/*1024*/]);



//ControljavaXlet in NCL app
//v
void GINGA_ncl_bridge_notifyXletAppState(UINT32 org_id, UINT16 app_id, UINT8 state);

#if 0
#ifndef APP_CONTROL_CODE
typedef enum{
	GINGA_BRIDGE_APP_SIGNAL_START=0X11,
	GINGA_BRIDGE_APP_PAUSE=0x31,
	GINGA_BRIDGE_APP_RESUME=0x41,
	GINGA_BRIDGE_APP_SIGNAL_KILL=0x21 /*STOP and ABORT*/
}GINGA_BRIDGE_APP_CONTROL_CODE;
#endif
#endif


/**
 * @param
 * [in]controlCode: APP_CONTROL-code (APP_SGINAL_START, APP_PAUSE, APP_RESUME, APP_
 * [in] org_id
 * [in] app_id
 * [in] initalClass: the value in x-mainclass attribute
 * [in] classPath: ABSOLUTE path for calss path of xlet. the valie in 'x-classpath' attribute should be converted to absolute path
 * 	Refer to ABNT NBR 15606-7, 5.6.3.4
 * Note: the result of this function will be notified via GINGA_J2TV_notifyAppState
 *
 */
void GINGA_ncl_bridge_controlXlet(SINT32 controlCode,
				  SINT32 org_id,
				  SINT32 app_id,
				  SINT8 * initialClass,
				  SINT8 * classPath,
				  UINT32 x,
				  UINT32 y,
				  SINT32 w,
				  SINT32 h);


//callbacks
void GINGA_ncl_bridge_notifyOfNCLPlayerEventcb(int eventType,std::string value);
void GINGA_ncl_bridge_NCLStartXlet(const std::string &initialClass,const std::string &classPath,
        int x,int y, unsigned int width, unsigned int height);
void GINGA_ncl_bridge_NCLControlXlet(long controlCode);
void GINGA_ncl_bridge_notifyOfXletAppState(unsigned char state);

#endif //GINGA_NCL_BRIDGE__

