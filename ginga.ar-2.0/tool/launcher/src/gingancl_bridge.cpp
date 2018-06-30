/*
 * LGESP Brazil
 * Ginga Bridge implementation  
 *
 * author: alysson.brito@lge.com
 * date: 2013.09.12
 * */

#include <assert.h>

#include <pem-tv-util/pem_moa_msg.h>
#include <pem-tv-util/pem_log.h>

#include <ginga_api.h>
#include "browser_port_mw2tv.h"

#include "BrowserPresentationEngine.h"

#include "gingancl_bridge.h"

#define GINGA_BRIDGE_ERROR -1
#define GINGA_BRIDGE_SUCCESS 0
extern BrowserPresentationEngine bpe;

//moduel variables
static int _ginga_ncl_bridge_playerId = -1;

//mutex stuff
static pthread_mutex_t _ginga_ncl_bridge_sem_id;
#define GINGA_NCL_BRIDGE_LOCK() pthread_mutex_lock(&_ginga_ncl_bridge_sem_id)
#define GINGA_NCL_BRIDGE_TRYLOCK() pthread_mutex_trylock(&__ginga_ncl_bridge_sem_id)
#define GINGA_NCL_BRIDGE_UNLOCK() pthread_mutex_unlock(&_ginga_ncl_bridge_sem_id)
#define GINGA_NCL_BRIDGE_CREATE() pthread_mutex_init(&_ginga_ncl_bridge_sem_id,NULL)
#define GINGA_NCL_BRIDGE_DESTROY() pthread_mutex_destroy(&_ginga_ncl_bridge_sem_id)


//init
void ginga_ncl_bridge_init(void) {
	APRINT_BRIDGE("<==>");
	assert(GINGA_NCL_BRIDGE_CREATE() == 0);
	_ginga_ncl_bridge_playerId = -1;

}

//increse by 1 in a protected region
static int _ginga_ncl_bridge_getPlayer(void) {
	APRINT_BRIDGE("==>");
	GINGA_NCL_BRIDGE_LOCK();
	if (_ginga_ncl_bridge_playerId == -1) {
		APRINT_BRIDGE("Creating player");
		_ginga_ncl_bridge_playerId = 1;
	}
	GINGA_NCL_BRIDGE_UNLOCK();
	APRINT_BRIDGE("<==");
	return _ginga_ncl_bridge_playerId;
}
static int _ginga_ncl_bridge_releasePlayer(int playerId) {
	APRINT_BRIDGE("==>");
	GINGA_NCL_BRIDGE_LOCK();
	if (_ginga_ncl_bridge_playerId == 1) {
		APRINT_BRIDGE("Releasing player");
		_ginga_ncl_bridge_playerId = -1;
	} else {
		APRINT_BRIDGE("It is not allocated.CurValue[%d].  Doing Nothing",_ginga_ncl_bridge_playerId);
	}
	GINGA_NCL_BRIDGE_UNLOCK();
	APRINT_BRIDGE("<==");
	return _ginga_ncl_bridge_playerId;
}
SINT32 GINGA_ncl_bridge_createNCLPlayer(void) {
	APRINT_BRIDGE("==>");
	if (_ginga_ncl_bridge_getPlayer() != 1) {
		//error.
		APRINT_BRIDGE("ERROR");
	} APRINT_BRIDGE("<== Returning[%d]",_ginga_ncl_bridge_playerId);
	return _ginga_ncl_bridge_playerId;
} //funct

void GINGA_ncl_bridge_disposeNCLPlayer(SINT32 playerId) {
	APRINT_BRIDGE("==> playerId[%d]",playerId);
	if (_ginga_ncl_bridge_releasePlayer(playerId) != -1) {
		//error.
		APRINT_BRIDGE("ERROR");
	} APRINT_BRIDGE("<== playerId[%d]",playerId);
} //funct

SINT32 GINGA_ncl_bridge_setBounds(SINT32 playerId, SINT32 screen_width,
		SINT32 screen_height, SINT32 x1, SINT32 y1, SINT32 x2, SINT32 y2) {
	int returnCode = GINGA_BRIDGE_ERROR;

	APRINT_BRIDGE("==> playerId[%d] screen_width[%d] screen_height[%d] x1[%d] y1[%d] x2[%d] y2[%d]",
			playerId,screen_width, screen_height,x1,y1,x2,y2);
	//sent to graphics
	APRINT_BRIDGE("<== ReturnCode[%d] playerId[%d]" ,returnCode,playerId);
	return returnCode;
}


void /*SINT32*/ GINGA_ncl_bridge_setVisible(SINT32 playerId, BOOLEAN visible) {

	int returnCode = GINGA_BRIDGE_ERROR;

	APRINT_BRIDGE("==> playerId[%d] visible[%d]",playerId,visible);

	PORT_N2TV_setBridgeVisible ( playerId, ( visible ? 1  /* TRUE */ : 0  /* FALSE */ ) );

	APRINT_BRIDGE("<== Returning[%d] playerId[%d] visible[%d]",returnCode,playerId,visible);

	//return returnCode;
}

/**
 * [in] coltrolCode NCL_CONTROL_CODE (NCL_SET_DOC, pause_doc, resume, stop)
 */
SINT32 GINGA_ncl_bridge_controlDoc(SINT32 playerId, SINT32 controlCode,
		SINT8* param) {
	int returnCode = GINGA_BRIDGE_ERROR;
	APRINT_BRIDGE("==> playerId[%d] controlCode[%d]",playerId,controlCode);
	if (playerId != _ginga_ncl_bridge_playerId) {
		APRINT_BRIDGE("<==ERROR. Returning [%d] playerId[%d] controlCode[%d] param[%d]",GINGA_BRIDGE_ERROR,playerId,controlCode,param[0]);
		return GINGA_BRIDGE_ERROR;
	}

	switch (controlCode) {
	case NCL_SET_DOC: {
		//param:abs filepath
		APRINT_BRIDGE("NCL_SET_DOC filepath[%s]",param);
	}
		break;
	case NCL_START_DOC: {
		//interfaceID
		APRINT_BRIDGE("NCL_START_DOC interfaceId[%s]",param);
	}
		break;
	case NCL_STOP_DOC: {
		//no param
		APRINT_BRIDGE("NCL_STOP_DOC");
	}
		break;
	case NCL_PAUSE_DOC: {
		//no param
		APRINT_BRIDGE("NCL_PAUSE_DOC");
	}
		break;
	case NCL_RESUME_DOC: {
		//no param
		APRINT_BRIDGE("NCL_RESUME_DOC");
	}
		break;
	default: {
		//no param
		APRINT_BRIDGE("Uknown control code. ERROR");
		returnCode = GINGA_BRIDGE_ERROR;
	}
	}

	APRINT_BRIDGE("<== Returning[%d] playerId[%d] controlCode[%d]", returnCode,playerId,controlCode);
	return returnCode;

}

SINT32 GINGA_ncl_bridge_getPropertyValue(SINT32 playerId, SINT8* propertyId,
		SINT8 value[/*1024*/]) {
	int returnCode = GINGA_BRIDGE_ERROR;
	APRINT_BRIDGE("==> playerId[%d] propertyId[%d]",playerId,*propertyId);

	//getProperty(propertyId,value)

	APRINT_BRIDGE("<== Returning:[%d] value[%s] playerId[%d] propertyId[%d]",returnCode,value,playerId,*propertyId);
	return (returnCode);
}

SINT32 GINGA_ncl_bridge_edit(SINT32 playerId, SINT32 command, SINT8* param1,
		SINT8* param2, SINT8* param3) {
	int returnCode = GINGA_BRIDGE_ERROR;
	APRINT_BRIDGE("==> playerId[%d] command[%d]",playerId,command);

	switch (command) {
	case ADD_REGION: {
		//param1:regionBaseId
		//param2:regionId
		//param3:regionStr
		APRINT_BRIDGE("ADD_REGION");
	}
		break;
	case REMOVE_REGION: {
		//param1:regionBaseId
		APRINT_BRIDGE("REMOVE_REGION");
	}
		break;
	case ADD_REGION_BASE: {
		//param1:regionBaseStr
		APRINT_BRIDGE("ADD_REGION_BASE");
	}
		break;
	case REMOVE_REGION_BASE: {
		//param1:regionBaseId
		APRINT_BRIDGE("REMOVE_REGION_BASE");
	}
		break;
	case ADD_RULE: {
		//param1:ruleStr
		APRINT_BRIDGE("ADD_RULE");
	}
		break;
	case REMOVE_RULE: {
		//param1:ruleId
		APRINT_BRIDGE("REMOVE_RULE");
	}
		break;
	case ADD_RULE_BASE: {
		//param1:ruleBaseStr
		APRINT_BRIDGE("ADD_RULE_BASE");
	}
		break;
	case REMOVE_RULE_BASE: {
		//param1:ruleBaseId
		APRINT_BRIDGE("REMOVE_RULE_BASE");
	}
		break;
	case ADD_CONNECTOR: {
		//param1:connectorStr
		APRINT_BRIDGE("ADD_CONNECTOR");
	}
		break;
	case REMOVE_CONNECTOR: {
		//param1:connectorId
		APRINT_BRIDGE("REMOVE_CONNECTOR");
	}
		break;
	case ADD_CONNECTOR_BASE: {
		//param1:connectorBaseStr
		APRINT_BRIDGE("ADD_CONNECTOR_BASE");
	}
		break;
	case REMOVE_CONNECTOR_BASE: {
		//param1:connectorBaseId
		APRINT_BRIDGE("REMOVE_CONNECTOR_BASE");
	}
		break;
	case ADD_DESCRIPTOR: {
		//param1:descriptorStr
		APRINT_BRIDGE("ADD_DESCRIPTOR");
	}
		break;
	case REMOVE_DESCRIPTOR: {
		//param1:descriptorId
		APRINT_BRIDGE("REMOVE_DESCRIPTOR");
	}
		break;
	case ADD_DESCRIPTOR_SWITCH: {
		//param1:descriptorSwitchStr
		APRINT_BRIDGE("ADD_DESCRIPTOR_SWITCH");
	}
		break;
	case REMOVE_DESCRIPTOR_SWITCH: {
		//param1:descriptorSwitchId
		APRINT_BRIDGE("ADD_DESCRIPTOR_SWITCH");
	}
		break;
	case ADD_DESCRIPTOR_BASE: {
		//param1:descriptorBaseStr
		APRINT_BRIDGE("ADD_DESCRIPTOR_BASE");
	}
		break;
	case REMOVE_DESCRIPTOR_BASE: {
		//param1:descriptorBaseId
		APRINT_BRIDGE("REMOVE_DESCRIPTOR_BASE");
	}
		break;
	case ADD_TRANSITION: {
		//param1:transitionStr
		APRINT_BRIDGE("ADD_TRANSITION");
	}
		break;
	case REMOVE_TRANSITION: {
		//param1:transitionId
		APRINT_BRIDGE("REMOVE_TRANSITION");
	}
		break;
	case ADD_TRANSITION_BASE: {
		//param1:transitionBaseStr
		APRINT_BRIDGE("ADD_TRANSITION_BASE");
	}
		break;
	case REMOVE_TRANSITION_BASE: {
		//param1:transitionBaseId
		APRINT_BRIDGE("REMOVE_TRANSITION_BASE");
	}
		break;
	case ADD_IMPORT_BASE: {
		//param1:docBaseId
		//param2:importBaseStr
		APRINT_BRIDGE("ADD_IMPORT_BASE");
	}
		break;
	case REMOVE_IMPORT_BASE: {
		//param1:docBaseId
		//param2:importBaseId
		APRINT_BRIDGE("REMOVE_IMPORT_BASE");
	}
		break;
	case ADD_IMPORT_DOC_BASE: {
		//param1:importDocBaseStr
		APRINT_BRIDGE("ADD_IMPORT_DOC_BASE");
	}
		break;
	case REMOVE_IMPORT_DOC_BASE: {
		//param1:importDocBaseId
		APRINT_BRIDGE("REMOVE_IMPORT_DOC_BASE");
	}
		break;
	case ADD_IMPORT_NCL: {
		//param1:importNCLStr
		APRINT_BRIDGE("ADD_IMPORT_NCL");
	}
		break;
	case REMOVE_IMPORT_NCL: {
		//param1:importNCLId
		APRINT_BRIDGE("REMOVE_IMPORT_NCL");
	}
		break;
	case ADD_NODE: {
		//param1:compositeId
		//param2:nodeStr
		APRINT_BRIDGE("ADD_NODE");
	}
		break;
	case REMOVE_NODE: {
		//param1:compositeId
		//param2:nodeId
		APRINT_BRIDGE("REMOVE_NODE");
	}
		break;
	case ADD_INTERFACE: {
		//param1:nodeId
		//param2:interfaceStr
		APRINT_BRIDGE("ADD_INTERFACE");
	}
		break;
	case REMOVE_INTERFACE: {
		//param1:nodeId
		//param2:interfaceId
		APRINT_BRIDGE("REMOVE_INTERFACE");
	}
		break;
	case ADD_LINK: {
		//param1:compositeId
		//param2:linkStr
		APRINT_BRIDGE("ADD_LINK");
	}
		break;
	case REMOVE_LINK: {
		//param1:compositeId
		//param2:linkId
		APRINT_BRIDGE("REMOVE_LINK");
	}
		break;
	case SET_PROPERTY: {
		//param1:propertyId
		//param2:value
		APRINT_BRIDGE("SET_PROPERTY");
	}
		break;
	default: {
		APRINT_BRIDGE("COMMAND INVALID");

	}
	}      //switch

	APRINT_BRIDGE("<== Returning:[%d] playerId[%d] command[%d]",returnCode,playerId,command);
	return (returnCode);
}

/**
 *@return:
 * 0 if succeeds
 * -1 in case of errors
 *
 *  located in 7.2.4 table 12 of 15606-2
 *  system.langauge | caption | subtitel ..
 *  user
 *       |age | location | genre | xxx
 *  default
 *       |focusBorderColor | setBorderColor | xxx
 *  service
 *       |currentFocus | currentKeyMaster | xxx
 *  si
 *       |numberOfServices | numberOfParticlaServices | channelNumber
 *  channel
 *       |keyCapture | virtualKeyboard | keyboardBounds | xxx
 *  shared
 *       |xxx
 */
SINT32 GINGA_ncl_bridge_settingsNodesGetValue(SINT8* nodeId, SINT8* key,
		SINT8 value[/*1024*/]) {
	int returnCode = GINGA_BRIDGE_ERROR;
	APRINT_BRIDGE("==> nodeId[%s] key[%s]",nodeId,key);
	//if valid
	APRINT_BRIDGE("<== Returning[%d] nodeId[%s] key[%s] value[%s]",returnCode,nodeId,key,value);
	return (returnCode);
}

//------------------------------------------------------------------------------
//NCL Event handling in JavaXlet
//------------------------------------------------------------------------------

//
//gingaNCL Engine -> GingaCC -> Ginga_J Engien API
//

/*EMBEDDED NCL*/
void GINGA_ncl_bridge_notifyOfNCLPlayerEvent(SINT32 playerId, SINT32 eventType,
		SINT8 /*out*/value[/*1024*/]) {
	int returnCode = GINGA_BRIDGE_ERROR;
	APRINT_BRIDGE("==> playerId[%d] eventType[%d] value[%s]",playerId,eventType,value);

	APRINT_BRIDGE("<== Returning:[%d] playerId[%d] eventType[%d] value[%s]",returnCode,playerId,eventType,value);
}

/*EMBEDDED GINGA_J*/
//NCLet aplication
/**
 * Notify the vent of NCL environment 
 * @param
 * [in] eventType: NCL_EVENT_TYPE
 *@return
 * none
 */
void GINGA_ncl_bridge_notifyOfNCLEvent(SINT32 eventType,
		SINT8 value[/*1024*/]) {
	APRINT_BRIDGE("==> eventType[%d] value[%s]",eventType,value);

	APRINT_BRIDGE("<== eventType[%d] value[%s]",eventType,value);
}

void GINGA_ncl_bridge_notifyXletAppState(UINT32 org_id, UINT16 app_id,
		UINT8 state) {
	APRINT_BRIDGE("==> org_id[%d] app_id[%d] state[%d]",org_id,app_id,state);
	//----------------
	//
	//------------------
	APRINT_BRIDGE("<== org_id[%d] app_id[%d] state[%d]",org_id,app_id,state);
}

/**
 * @param
 * [in]controlCode: APP_CONTROL-code (APP_SGINAL_START, APP_PAUSE, APP_RESUME, APP_
 * [in] org_id
 * [in] app_id
 * [in] initalClass: the value in x-mainclass attribute
 * [in] classPath: ABSOLUTE path for class path of xlet. the valie in 'x-classpath' attribute should be converted to absolute path
 * 	Refer to ABNT NBR 15606-7, 5.6.3.4
 * Note: the result of this function will be notified via GINGA_J2TV_notifyAppState
 *
 */
/* Preciso dele mesmo????
void GINGA_ncl_bridge_controlXlet(SINT32 controlCode,
				  SINT32 org_id,
				  SINT32 app_id,
				  SINT8 * initialClass,
				  SINT8 * classPath,
				  UINT32 x,
				  UINT32 y,
				  SINT32 w,
				  SINT32 h) {

	PORT_N2TV_controlXlet(controlCode, org_id, app_id, initialClass, classPath, x, y, w, h);

}
*/

void GINGA_ncl_bridge_notifyOfNCLPlayerEventcb(int eventType, std::string value) {
	APRINT_BRIDGE("==> eventType[%d] value[%s]",eventType,value.c_str()); APRINT_BRIDGE("<==");
}


void GINGA_ncl_bridge_NCLStartXlet(const std::string & initialClass,
				   const std::string & classPath,
				   int x,
				   int y,
				   unsigned int width,
				   unsigned int height) {

	PEM_BRIDGE_BEGIN();

	XletAppInfo xlet = bpe.appInfo.getXletInfo();

	XletAppRect rect ( x, y, width, height );
	xlet.setRect(rect);
	xlet.setClassName(initialClass);
	xlet.setClassPath(classPath);
	bpe.appInfo.setXletInfo( xlet );

	std::string tmp = bpe.appInfo.getPath();
	tmp += classPath;

	SINT8 * classname = ( SINT8 * ) xlet.getClassName().c_str();
	SINT8 * classpath = ( SINT8 * ) tmp.c_str();

	PORT_N2TV_controlXlet( APP_SIGNAL_START, bpe.appInfo.getOrgID(), bpe.appInfo.getAppID(), classname, classpath, x, y, width, height );
	PEM_BRIDGE_END();
}

void GINGA_ncl_bridge_NCLControlXlet(long controlCode) {
	PEM_BRIDGE_BEGIN();

	XletAppInfo xlet = bpe.appInfo.getXletInfo();

	std::string tmp = bpe.appInfo.getPath();
	tmp += xlet.getClassPath();

	SINT8 * classname = ( SINT8 * ) xlet.getClassName().c_str();
	SINT8 * classpath = ( SINT8 * ) tmp.c_str();

	PORT_N2TV_controlXlet( controlCode,
						   bpe.appInfo.getOrgID(),
						   bpe.appInfo.getAppID(),
						   classname,
						   classpath,
						   xlet.getRect().x,
						   xlet.getRect().y,
						   xlet.getRect().w,
						   xlet.getRect().h );

	PEM_BRIDGE_END();
}


void GINGA_ncl_bridge_notifyOfXletAppState(unsigned char state) {
	PEM_BRIDGE_BEGIN();
	PEM_BRIDGE_END();
}

