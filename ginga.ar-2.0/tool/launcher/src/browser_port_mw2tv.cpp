#include "browser_port_mw2tv.h"


#define PEM_MWAGT_BEGIN(fmt,...)	PEM_MWAGT_log(LOG_DEBUG, "%s : BEGIN [" fmt "]\n", __FUNCTION__, ##__VA_ARGS__)
#define PEM_MWAGT_END(fmt,...)		PEM_MWAGT_log(LOG_DEBUG, "%s : END [" fmt "]\n",   __FUNCTION__, ##__VA_ARGS__)
#define PEM_MWAGT_DEBUG(fmt,...)	PEM_MWAGT_log(LOG_DEBUG, "[%s::%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PEM_MWAGT_FATAL(fmt,...)	PEM_MWAGT_log(LOG_FATAL, "[%s::%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define PEM_MWAGT_BRIDGE_CONTROL(cc,oid,aid,cs,cp)	PEM_MWAGT_log(LOG_DEBUG, "[%s::%d] Control Code: %d[0x%x], OrgID: %d[0x%x], AppID: %d[0x%x], Classname: [%s], Classpath: [%s]\n", __FUNCTION__, __LINE__,(cc),(cc),(oid),(oid),(aid),(aid),(cs),(cp))



#define _snprintf(result,length,format,...) snprintf((char*)(result),length,(char*)format,##__VA_ARGS__)
#define _strncpy(result,source,length) strncpy((char*)result,(char*)source,length)

/* ================================================================
 *  PG 2.2 Ginge-N Engine Control
 * ================================================================ */

SINT8 PORT_N2TV_notifyMWReady(void) {
	MSG_COMMON sndMsg;
	SINT8 ret = 0;
	PEM_MWAGT_log(LOG_INFO, "%s BEGIN\n", __FUNCTION__);
	memset(&sndMsg, 0, sizeof(sndMsg));
	sndMsg.msgType = MSG_TYPE_NOTIFY_MW_READY;

	ret = BA_sendMsgMW2TV(&sndMsg, sizeof(sndMsg));
	PEM_MWAGT_log(LOG_INFO, "%s END\n", __FUNCTION__);
	return ret;
}


/* ================================================================
 *  PG 2.4 System Property
 * ================================================================ */
SINT8 PORT_N2TV_getNativeSystemProperty(SINT8* key, SINT8 value[1024]) {

	SINT8 ret = 0;

	MSG_GET_NATIVE_SYSTEM_PROPERTY sndMsg;
	MSG_GET_NATIVE_SYSTEM_PROPERTY rcvMsg;

	PEM_MWAGT_log(LOG_INFO, "%s BEGIN : key = %s\n", __FUNCTION__, key);
	memset(&sndMsg, 0, sizeof(MSG_GET_NATIVE_SYSTEM_PROPERTY));
	memset(&rcvMsg, 0, sizeof(MSG_GET_NATIVE_SYSTEM_PROPERTY));

	sndMsg.msgType = MSG_TYPE_GET_NATIVE_SYSTEM_PROPERTY;
	_snprintf(sndMsg.key, STR_MAX_LENGTH, "%s", key);

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_GET_NATIVE_SYSTEM_PROPERTY), &rcvMsg, sizeof(MSG_GET_NATIVE_SYSTEM_PROPERTY));
	_snprintf(value, STR_MAX_LENGTH, "%s", rcvMsg.value);
	ret = rcvMsg.ret;

	PEM_MWAGT_log(LOG_INFO, "%s END : key = %s, value = %s\n", __FUNCTION__, key, value);
	return ret;
}



/* ================================================================
 *  PG 2.5 User Property
 * ================================================================ */
SINT8 PORT_N2TV_requestUserProperties(UINT8 theCommand,
				      const UINT8 *szTheFilter,
				      UINT8 nSizeOfTheFilter,
				      UINT8 PropertyValue[],
				      UINT8 *nSizeOfPropertyValue,
				      const UINT8 *szNewValue,
				      UINT8 nSizeOfValue) {

	PEM_MWAGT_log(LOG_INFO, "%s() BEGIN : [command : %d] [theFilter : %s]\n", __FUNCTION__, theCommand, szTheFilter);

	SINT8 			result = FAILURE;
	MSG_USER_PROPERTY	sendMsg;
	MSG_USER_PROPERTY	retMsg;

	memset(&sendMsg, 0, sizeof(MSG_USER_PROPERTY));
	memset(&retMsg, 0, sizeof(MSG_USER_PROPERTY));

	switch(theCommand) {
	case READ_PROPERTY:
		sendMsg.msgType = MSG_TYPE_GET_USER_PROPERTY;
		break;
	case WRITE_PROPERTY:
		sendMsg.msgType = MSG_TYPE_SET_USER_PROPERTY;
		sendMsg.nSizeOfValue = nSizeOfValue;
		if (nSizeOfValue > 0) {
			memcpy(sendMsg.szValue, szNewValue,
					sizeof(UINT8) * (USER_PROPERTY_VALUE_MAX_LENGTH > nSizeOfValue + 1 ? nSizeOfValue + 1 : USER_PROPERTY_VALUE_MAX_LENGTH));
		}
		break;
	case REMOVE_PROPERTY:
		sendMsg.msgType = MSG_TYPE_REMOVE_USER_PROPERTY;
		break;
	default:
		return FAILURE;
	}

	sendMsg.nSizeOfKeyOrFilter = nSizeOfTheFilter;
	if (nSizeOfTheFilter > 0) {
		memcpy(sendMsg.szKeyOrFilter, szTheFilter,
					sizeof(UINT8) * (USER_PROPERTY_KEY_MAX_LENGTH > nSizeOfTheFilter + 1 ? nSizeOfTheFilter + 1 : USER_PROPERTY_KEY_MAX_LENGTH));
	}

	BA_sendReceiveMsgMW2TV(&sendMsg, sizeof(MSG_USER_PROPERTY), &retMsg, sizeof(MSG_USER_PROPERTY));
	result = retMsg.ret;
	if (result == SUCCESS && theCommand != REMOVE_PROPERTY) {
		*nSizeOfPropertyValue = retMsg.nSizeOfValue;
		memcpy(PropertyValue, retMsg.szValue,
							sizeof(UINT8) * (USER_PROPERTY_VALUE_MAX_LENGTH > *nSizeOfPropertyValue + 1 ? *nSizeOfPropertyValue + 1 : USER_PROPERTY_VALUE_MAX_LENGTH));
	}

	PEM_MWAGT_log(LOG_INFO, "%s() END : [command : %d] [theFilter : %s]\n", __FUNCTION__, theCommand, szTheFilter);

	return result;

}

//SINT8 PORT_N2TV_getUserProperties(const UINT8 *szTheFilter, UINT8 nSizeOfTheFilter, UINT8 PropertyKeys[][USER_PROPERTY_KEY_MAX_LENGTH], UINT8 *nCountOfPropertyKeys, UINT8 PropertyValues[][USER_PROPERTY_VALUE_MAX_LENGTH], UINT8 *nCountOfPropertyValues) {
//	PEM_MWAGT_log(LOG_INFO, "%s() BEGIN : [theFilter : %s]\n", __FUNCTION__, szTheFilter);
//
//	SINT8 					result = FAILURE;
//	MSG_USER_PROPERTY		sendMsg;
//	MSG_COMMON				retMsg;
//	MSG_USER_PROPERTIES		*dataMsg;
//	memset(&sendMsg, 0, sizeof(MSG_USER_PROPERTY));
//	memset(&retMsg, 0, sizeof(MSG_COMMON));
//
//	sendMsg.msgType = MSG_TYPE_GET_USER_PROPERTIES;
//	sendMsg.nSizeOfKeyOrFilter = nSizeOfTheFilter;
//	if (nSizeOfTheFilter > 0) {
//		memcpy(sendMsg.szKeyOrFilter, szTheFilter,
//					sizeof(UINT8) * (USER_PROPERTY_KEY_MAX_LENGTH > nSizeOfTheFilter + 1 ? nSizeOfTheFilter + 1 : USER_PROPERTY_KEY_MAX_LENGTH));
//	}
//
//	BA_sendReceiveMsgMW2TV(&sendMsg, sizeof(MSG_USER_PROPERTY), &retMsg, sizeof(MSG_USER_PROPERTIES));
//
//	result = retMsg.sint8;
//	if (result == SUCCESS) {
//		if (PORT_OSA_readSHM(retMsg.msgType, (UINT8 **)&dataMsg, retMsg.uint32) == -1) {
//			return FAILURE;
//		}
//		if (dataMsg->nCountOfKeys > 0 && dataMsg->nCountOfValues > 0) {
//			memcpy(PropertyKeys, dataMsg->PropertyKeys, sizeof(UINT8) * dataMsg->nCountOfKeys * USER_PROPERTY_KEY_MAX_LENGTH);
//			memcpy(PropertyKeys, dataMsg->PropertyValues, sizeof(UINT8) * dataMsg->nCountOfValues * USER_PROPERTY_VALUE_MAX_LENGTH);
//			*nCountOfPropertyKeys = dataMsg->nCountOfKeys;
//			*nCountOfPropertyValues = dataMsg->nCountOfValues;
//		} else {
//			*nCountOfPropertyKeys = 0;
//			*nCountOfPropertyValues = 0;
//		}
//
//		if (dataMsg != NULL) free((UINT8 *)dataMsg);
//	}
//
//	PEM_MWAGT_log(LOG_INFO, "%s() END : [theFilter : %s]\n", __FUNCTION__, szTheFilter);
//
//	return SUCCESS;
//}



/* ================================================================
 *  PG 3.1 Service Selection
 * ================================================================ */
SINT8 PORT_N2TV_selectService(UINT8 /*niid*/, UINT16 onid, SINT32 tsid, UINT16 sid, UINT8 []/*cTags[]*/, UINT8 /*numOfComponents*/) {
	PEM_MWAGT_log(LOG_INFO, "%s() BEGIN : [onid = 0x%x] [tsid = 0x%x] [sid = 0x%x]\n", __FUNCTION__, onid, tsid, sid);
/*
	MSG_MEDIAINFO msg;
	memset(&msg, 0, sizeof(MSG_MEDIAINFO));

	msg.msgType = MSG_TYPE_SELECT_SERVICE;
	msg.niid	= niid;
	msg.onid	= onid;
	msg.tsid	= tsid;
	msg.sid		= sid;
	memcpy(msg.cTags, cTags, sizeof(UINT8) * numOfComponents);
	msg.numOfComponents	= numOfComponents;

	BA_sendMsgMW2TV(&msg, sizeof(msg));
	PEM_MWAGT_log(LOG_INFO, "%s() END : [onid = 0x%x] [tsid = 0x%d] [sid =0x%x]\n", __FUNCTION__, onid, tsid, sid);
*/
	return 0;
}

/* ================================================================
 *  PG 3.2 Tuner
 * ================================================================ */
SINT8 PORT_N2TV_getCurrentSIBasicInfo(UINT8 theNiId , SINT32 ** /*cResultArray*/, SINT32 * /*nCountOfCArray*/)
{
	PEM_MWAGT_log(LOG_INFO, "%s() BEGIN : [niId = %d]\n", __FUNCTION__, theNiId);

	SINT8 				result = FAILURE;
//	MSG_COMMON 			sendMsg;
/*
	MSG_BASIC_SI_INFO 	retMsg;

	memset(&sendMsg, 0, sizeof(MSG_COMMON));
	memset(&retMsg, 0, sizeof(MSG_BASIC_SI_INFO));

	sendMsg.msgType 	= MSG_TYPE_GET_BASIC_SI_INFO;
	sendMsg.uint8	 	= theNiId;

	BA_sendReceiveMsgMW2TV(&sendMsg, sizeof(MSG_COMMON), &retMsg, sizeof(MSG_BASIC_SI_INFO));
	result = retMsg.ret;
	*nCountOfCArray = retMsg.nCountOfArray;
	*cResultArray = (SINT32 *)malloc(sizeof(SINT32) * (retMsg.nCountOfArray + 1));
	memcpy(*cResultArray, retMsg.SiInfoArray, sizeof(SINT32) * retMsg.nCountOfArray);
	*cResultArray[retMsg.nCountOfArray] = '\0';

	PEM_MWAGT_log(LOG_INFO, "%s() END : [result = %d][niId = %d]\n", __FUNCTION__, result, theNiId);
*/
	return result;
}

SINT8 PORT_N2TV_tune(UINT8 theNiId, UINT16 theOnId, UINT16 theTsId) {

	PEM_MWAGT_log(LOG_INFO, "%s() BEGIN : [niId = %d][onId = %d][tsId = %d]\n", __FUNCTION__, theNiId, theOnId, theTsId);

	SINT8			result = FAILURE;
	MSG_TUNE_INFO		sendMsg;
	MSG_COMMON		retMsg;

	memset(&sendMsg, 0, sizeof(MSG_TUNE_INFO));
	memset(&retMsg, 0, sizeof(MSG_COMMON));

	sendMsg.msgType		= MSG_TYPE_TUNE;
	sendMsg.niId		= theNiId;
	sendMsg.onId		= theOnId;
	sendMsg.tsId		= theOnId;

	BA_sendReceiveMsgMW2TV(&sendMsg, sizeof(MSG_TUNE_INFO), &retMsg, sizeof(MSG_COMMON));
	result = retMsg.sint8;

	PEM_MWAGT_log(LOG_INFO, "%s() END : [result = %d][niId = %d]\n", __FUNCTION__, result, theNiId);

	return result;
}

/* ================================================================
 *  PG 3.3 CM
 * ================================================================ */
SINT8 PORT_N2TV_getNetworkAndTSName(UINT8 theNiId, UINT16 theOnId, UINT16 theTsId, UINT8 networkName[], UINT8 transportStreamName[]) {
	PEM_MWAGT_log(LOG_INFO, "%s() BEGIN : [niId = %d][onId = %d][tsId = %d]\n", __FUNCTION__, theNiId, theOnId, theTsId);

	SINT8			result = FAILURE;
	MSG_TUNE_INFO		sendMsg;
	MSG_NAME_SI_INFO	retMsg;

	memset(&sendMsg, 0, sizeof(MSG_TUNE_INFO));
	memset(&retMsg, 0, sizeof(MSG_NAME_SI_INFO));

	sendMsg.msgType		= MSG_TYPE_TUNE;
	sendMsg.niId		= theNiId;
	sendMsg.onId		= theOnId;
	sendMsg.tsId		= theTsId;

	BA_sendReceiveMsgMW2TV(&sendMsg, sizeof(MSG_TUNE_INFO), &retMsg, sizeof(MSG_NAME_SI_INFO));
	result = retMsg.ret;
	strcpy((char *) networkName, (char *) retMsg.NetworkName);
	strcpy((char *) transportStreamName, (char *) retMsg.TSName);

	PEM_MWAGT_log(LOG_INFO, "%s() END : [result = %d][niId = %d]\n", __FUNCTION__, result, theNiId);

	return result;

}

/* ================================================================
 *  PG 4.1 Application Control
 * ================================================================ */
void PORT_N2TV_requestAppControl(UINT8 /*controlCode*/, UINT32 /*oid*/, UINT16 /*aid*/) {
//	MSG_COMMON sndMsg;
/*
	PEM_MWAGT_log(LOG_INFO, "%s() BEGIN controlCode=%d, oid=0x%x, aid=0x%x\n", __FUNCTION__, controlCode, oid, aid);
	memset(&sndMsg, 0, sizeof(sndMsg));
	sndMsg.msgType = MSG_TYPE_REQUEST_APP_CONTROL;
	sndMsg.uint8 = controlCode;
	sndMsg.uint32 = oid;
	sndMsg.uint16 = aid;

	// no return
	BA_sendMsgMW2TV(&sndMsg, sizeof(sndMsg));
*/
	PEM_MWAGT_log(LOG_INFO, "%s() END\n", __FUNCTION__);
}

void PORT_N2TV_notifyAppState(UINT32 oid, UINT16 aid, UINT8 state) {
	MSG_COMMON sndMsg;

	PEM_MWAGT_log(LOG_INFO, "%s() BEGIN oid=0x%x, aid=0x%x, state=%d\n", __FUNCTION__, oid, aid, state);
	memset(&sndMsg, 0, sizeof(sndMsg));
	sndMsg.msgType = MSG_TYPE_NOTIFY_APP_STATE;
	sndMsg.uint32 = oid;
	sndMsg.uint16 = aid;
	sndMsg.uint8 = state;

	// no return
	BA_sendMsgMW2TV(&sndMsg, sizeof(sndMsg));
	PEM_MWAGT_log(LOG_INFO, "%s() END\n", __FUNCTION__);
}

SINT8 PORT_N2TV_getAppExecInfo(UINT32 oid, UINT16 aid, APP_EXEC_INFO *info) {
	MSG_COMMON sndMsg;
	MSG_APP_EXEC_INFO rcvMsg;
	SINT8 result = -1;

	PEM_MWAGT_log(LOG_DEBUG, "%s : BEGIN  oid=0x%x, aid=0x%x\n", __FUNCTION__, oid, aid);
	memset(&sndMsg, 0, sizeof(MSG_COMMON));
	sndMsg.msgType = MSG_TYPE_APP_EXEC_INFO;
	sndMsg.uint32 = oid;
	sndMsg.uint16 = aid;

	memset(&rcvMsg, 0x0, sizeof(MSG_APP_EXEC_INFO));
	if (BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_COMMON), &rcvMsg, sizeof(MSG_APP_EXEC_INFO)) == 0) {
		result = rcvMsg.result;
		if (result == 0) {
			memcpy(info, &rcvMsg.app_info, sizeof(APP_EXEC_INFO));
		} else {
			PEM_MWAGT_log(LOG_FATAL, "%s : PG API return fail\n", __FUNCTION__);
			info = NULL;
		}
	} else {
		PEM_MWAGT_log(LOG_FATAL, "%s : BA_sendReceiveMsgMW2TV fail\n", __FUNCTION__);
	}

	PEM_MWAGT_log(LOG_DEBUG, "%s : END  oid=0x%x, aid=0x%x\n", __FUNCTION__, oid, aid);
	return result;
}

SINT8 PORT_N2TV_getAppAttrInfo(UINT32 oid, UINT16 aid, APP_ATTR_INFO *info) {
	MSG_COMMON sndMsg;
	MSG_APP_ATTR_INFO rcvMsg;
	SINT8 result = -1;

	PEM_MWAGT_log(LOG_DEBUG, "%s : BEGIN  oid=0x%x, aid=0x%x\n", __FUNCTION__, oid, aid);
	memset(&sndMsg, 0, sizeof(MSG_COMMON));
	sndMsg.msgType = MSG_TYPE_APP_ATTR_INFO;
	sndMsg.uint32 = oid;
	sndMsg.uint16 = aid;

	memset(&rcvMsg, 0x0, sizeof(MSG_APP_ATTR_INFO));
	if (BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_COMMON), &rcvMsg, sizeof(MSG_APP_ATTR_INFO)) == 0) {
		result = rcvMsg.result;
		if (result == 0) {
			memcpy(info, &rcvMsg.app_info, sizeof(APP_ATTR_INFO));
		} else {
			PEM_MWAGT_log(LOG_FATAL, "%s : PG API return fail\n", __FUNCTION__);
			info = NULL;
		}
	} else {
		PEM_MWAGT_log(LOG_FATAL, "%s : BA_sendReceiveMsgMW2TV fail\n", __FUNCTION__);
	}

	PEM_MWAGT_log(LOG_DEBUG, "%s : END  oid=0x%x, aid=0x%x\n", __FUNCTION__, oid, aid);
	return result;
}


void PORT_N2TV_printAppDB(void) {
	MSG_COMMON sndMsg;

	PEM_MWAGT_log(LOG_DEBUG, "%s() BEGIN\n", __FUNCTION__);
	memset(&sndMsg, 0, sizeof(sndMsg));
	sndMsg.msgType = MSG_TYPE_PRINT_APP_DB;

	// no return
	BA_sendMsgMW2TV(&sndMsg, sizeof(sndMsg));
	PEM_MWAGT_log(LOG_INFO, "%s() END\n", __FUNCTION__);
}


/* ================================================================
 *  PG 5.1 Basic Media Control
 * ================================================================ */
SINT8 PORT_N2TV_loadMedia(UINT8 type,
			  UINT8 niid,
			  UINT16 onid,
			  SINT32 tsid,
			  UINT16 sid,
			  UINT16 pid,
			  UINT8 cTags[],
			  UINT8 numOfComponents,
			  UINT8 *mediaPath,
			  UINT8 sizeOfMediaPath,
			  SINT32 *handlerId) {

	SINT8           result = FAILURE;
	MSG_MEDIAINFO   sndMsg;
	MSG_COMMON      retMsg;

	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: [type = %d] [onid = 0x%x] [tsid = 0x%x] [sid = 0x%x] [pid = 0x%x] [numOfComponents = %d] [Media Path = %s] [Size of Media Path = %d] [handlerId: %d]\n",  __FUNCTION__, type, onid, tsid, sid, pid, numOfComponents, ((char *) mediaPath), sizeOfMediaPath, handlerId);

	memset(&sndMsg, 0, sizeof(MSG_MEDIAINFO));
	memset(&retMsg, 0, sizeof(MSG_COMMON));

	sndMsg.msgType   = MSG_TYPE_LOAD_MEDIA;
	sndMsg.videoType = type;
	sndMsg.niid      = niid;
	sndMsg.onid      = onid;
	sndMsg.tsid      = tsid;
	sndMsg.sid       = sid;
	sndMsg.pid       = pid;

	if (numOfComponents > 0) {
		memcpy(sndMsg.cTags, cTags, sizeof(UINT8) * numOfComponents);
	}

	sndMsg.numOfComponents = numOfComponents;

	if (sizeOfMediaPath > 0 /*&& sizeOfMediaPath < MAX_URL_SIZE*/) {
		_snprintf(sndMsg.mediaPath, MAX_URL_SIZE, "%s", mediaPath);
	}

	PEM_MWAGT_log(LOG_INFO, "%s, Load file: [%s]\n", __FUNCTION__, ((char *) mediaPath));

	sndMsg.sizeOfMediaPath = sizeOfMediaPath;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(MSG_COMMON));
	*handlerId = retMsg.sint32;
	result     = retMsg.sint8;

	PEM_MWAGT_log(LOG_INFO, "%s, END: [type = %d] [onid = 0x%x] [tsid = 0x%x] [sid = 0x%x] [result = %d] [handler Id = %d]\n", __FUNCTION__, type, onid, tsid, sid, result, *handlerId);
	return result;
}

SINT8 PORT_N2TV_setMediaTime(SINT32 handlerId, UINT64 mediaTime) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d\n", __FUNCTION__, handlerId);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON 		retMsg;
	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType = MSG_TYPE_SET_MEDIA_TIME;
	sndMsg.handlerId = handlerId;
	sndMsg.srcX = mediaTime;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d\n", __FUNCTION__, result, handlerId);
	return result;
}
SINT8 PORT_N2TV_playMedia(SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d\n", __FUNCTION__, handlerId);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON 		retMsg;
	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType = MSG_TYPE_PLAY_MEDIA;
	sndMsg.handlerId = handlerId;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d\n", __FUNCTION__, result, handlerId);
	return result;
}

SINT8 PORT_N2TV_stopMedia(SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d\n", __FUNCTION__, handlerId);

	MSG_MEDIA_CONTROLINFO 	sndMsg;
	MSG_COMMON 		retMsg;
	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType = MSG_TYPE_STOP_MEDIA;
	sndMsg.handlerId = handlerId;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d\n", __FUNCTION__, result, handlerId);
	return result;
}

SINT8 PORT_N2TV_closeMedia(SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d\n", __FUNCTION__, handlerId);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON 		retMsg;
	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType = MSG_TYPE_CLOSE_MEDIA;
	sndMsg.handlerId = handlerId;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d\n", __FUNCTION__, result, handlerId);
	return result;
}

SINT8 PORT_N2TV_freezeMedia(SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d\n", __FUNCTION__, handlerId);

	MSG_MEDIA_CONTROLINFO 	sndMsg;
	MSG_COMMON 		retMsg;
	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType = MSG_TYPE_FREEZE_MEDIA;
	sndMsg.handlerId = handlerId;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d\n", __FUNCTION__, result, handlerId);
	return result;
}

SINT8 PORT_N2TV_resumeMedia(SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d\n", __FUNCTION__, handlerId);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON 		retMsg;
	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType = MSG_TYPE_RESUME_MEDIA;
	sndMsg.handlerId = handlerId;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d\n", __FUNCTION__, result, handlerId);
	return result;
}

SINT8 PORT_N2TV_getVideoResolution(SINT32 theHandlerId,
				   SINT32 *videoWidth,
				   SINT32 *videoHeight,
				   VIDEO_IS_DECODING isDecoding) {
	SINT8 result = -1;
	MSG_MOA_MEDIA_CONTROL retMsg;
	MSG_COMMON sndMsg;

	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d\n", __FUNCTION__, theHandlerId);
	memset(&sndMsg, 0, sizeof(MSG_COMMON));
	memset(&retMsg,  0, sizeof(MSG_MOA_MEDIA_CONTROL));

	sndMsg.msgType = MSG_TYPE_GET_VIDEO_RESOLUTION;
	sndMsg.sint32  = theHandlerId;
	sndMsg.sint8   = isDecoding;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_COMMON), &retMsg, sizeof(MSG_MOA_MEDIA_CONTROL));
	result = retMsg.ret;

	*videoWidth = retMsg.videoWidth;
	*videoHeight = retMsg.videoHeight;

	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d, Video Resolution: (%d x %d)\n",
			       __FUNCTION__,
			      result,
			      theHandlerId,
			      *videoWidth,
			      *videoHeight);
	return result;
}

SINT8 PORT_N2TV_videoResizeWithClipping(SINT32 theHandlerId,
					SINT32 theClipX,
					SINT32 theClipY,
					SINT32 theClipWidth,
					SINT32 theClipHeight,
					SINT32 theDestX,
					SINT32 theDestY,
					SINT32 theDestWidth,
					SINT32 theDestHeight) {
	SINT8                   result = -1;
	MSG_MEDIA_CONTROLINFO   sndMsg;
	MSG_COMMON              retMsg;

	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d\n", __FUNCTION__, theHandlerId);

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType   = MSG_TYPE_RESIZE_VIDEO_WITH_CLIPPING;
	sndMsg.handlerId = theHandlerId;
	sndMsg.srcX      = theClipX;
	sndMsg.srcY      = theClipY;
	sndMsg.srcWidth  = theClipWidth;
	sndMsg.srcHeight = theClipHeight;
	sndMsg.destX     = theDestX;
	sndMsg.destY     = theDestY;
	sndMsg.destWidth = theDestWidth;
	sndMsg.destHeight= theDestHeight;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;

	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d\n", __FUNCTION__, result, theHandlerId);
	return result;
}

SINT8 PORT_N2TV_setSoundLevel(UINT32 value, SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d value:%d\n", __FUNCTION__, handlerId, value);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON            retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType     = MSG_TYPE_MANIPULATE_MEDIA_COMPONENTS;
	sndMsg.handlerId   = handlerId;
	sndMsg.commandType = SET_SOUND_LEVEL;
	sndMsg.audioLevel  = value;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));

	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d\n", __FUNCTION__, result, handlerId);
	return result;
}

SINT8 PORT_N2TV_getSoundLevel(UINT32 * value, SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d\n", __FUNCTION__, handlerId);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON            retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType     = MSG_TYPE_MANIPULATE_MEDIA_COMPONENTS;
	sndMsg.handlerId   = handlerId;
	sndMsg.commandType = GET_SOUND_LEVEL;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));

	result = retMsg.sint8;
	*value = retMsg.sint32;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d, value:%d\n", __FUNCTION__, result, handlerId, *value);
	return result;
}

SINT8 PORT_N2TV_setBalanceLevel(UINT32 value, SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d value:%d\n", __FUNCTION__, handlerId, value);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON            retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType     = MSG_TYPE_MANIPULATE_MEDIA_COMPONENTS;
	sndMsg.handlerId   = handlerId;
	sndMsg.commandType = SET_BALANCE_LEVEL;
	sndMsg.audioLevel  = value;

	//BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));

	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d\n", __FUNCTION__, result, handlerId);
	return result;
}

SINT8 PORT_N2TV_getBalanceLevel(UINT32 * value, SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d\n", __FUNCTION__, handlerId);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON            retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType     = MSG_TYPE_MANIPULATE_MEDIA_COMPONENTS;
	sndMsg.handlerId   = handlerId;
	sndMsg.commandType = GET_BALANCE_LEVEL;

	//BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));

	result = retMsg.sint8;
	*value = retMsg.sint32;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d, value:%d\n", __FUNCTION__, result, handlerId, *value);
	return result;
}

SINT8 PORT_N2TV_setTrebleLevel(UINT32 value, SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d value:%d\n", __FUNCTION__, handlerId, value);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON            retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType     = MSG_TYPE_MANIPULATE_MEDIA_COMPONENTS;
	sndMsg.handlerId   = handlerId;
	sndMsg.commandType = SET_TREBLE_LEVEL;
	sndMsg.audioLevel  = value;

	//BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d\n", __FUNCTION__, result, handlerId);
	return result;
}

SINT8 PORT_N2TV_getTrebleLevel(UINT32 * value, SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d\n", __FUNCTION__, handlerId);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON            retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType     = MSG_TYPE_MANIPULATE_MEDIA_COMPONENTS;
	sndMsg.handlerId   = handlerId;
	sndMsg.commandType = GET_TREBLE_LEVEL;

	//BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));

	result = retMsg.sint8;
	*value = retMsg.sint32;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d, value:%d\n", __FUNCTION__, result, handlerId, *value);
	return result;
}

SINT8 PORT_N2TV_setBassLevel(UINT32 value, SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d value:%d\n", __FUNCTION__, handlerId, value);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON            retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType     = MSG_TYPE_MANIPULATE_MEDIA_COMPONENTS;
	sndMsg.handlerId   = handlerId;
	sndMsg.commandType = SET_BASS_LEVEL;
	sndMsg.audioLevel  = value;

	//BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d\n", __FUNCTION__, result, handlerId);
	return result;
}

SINT8 PORT_N2TV_getBassLevel(UINT32 * value, SINT32 handlerId) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: handlerId:%d value:%d\n", __FUNCTION__, handlerId, value);

	MSG_MEDIA_CONTROLINFO sndMsg;
	MSG_COMMON            retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType     = MSG_TYPE_MANIPULATE_MEDIA_COMPONENTS;
	sndMsg.handlerId   = handlerId;
	sndMsg.commandType = GET_BASS_LEVEL;

	//BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));

	result = retMsg.sint8;
	*value = retMsg.sint32;
	PEM_MWAGT_log(LOG_INFO,"%s, END: ret:%d, handlerId:%d, value:%d\n", __FUNCTION__, result, handlerId, *value);
	return result;
}


/* ================================================================
 *  PG 6.1 Section Filter(com.sun.dtv.filtering) API
 * ================================================================ */

SINT8 PORT_N2TV_reserveSFs(BOOLEAN tsType, SINT16 sfgID, SINT8 filterNum) {

	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"PORT_N2TV_reserveSFs, BEGIN: tsType:%d, sfgID:0x%x, filterNum:%d\n", tsType, sfgID, filterNum);

	MSG_SECTION_FILTER_INFO sndMsg;
	MSG_COMMON              retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType   = MSG_TYPE_SECTION_FILTER_RESERVE_SF;
	sndMsg.tsType    = tsType;
	sndMsg.sfgID     = sfgID;
	sndMsg.filterNum = filterNum;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"PORT_N2TV_reserveSFs, END: ret:%d, tsType:%d, sfgID:0x%x, filterNum:%d\n", result, tsType, sfgID, filterNum);
	return result;
}

SINT8 PORT_N2TV_releaseSFs(BOOLEAN tsType, SINT16 sfgID) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"PORT_N2TV_reserveSFs, BEGIN: tsType:%d, sfgID:0x%x\n", tsType, sfgID);

	MSG_SECTION_FILTER_INFO sndMsg;
	MSG_COMMON              retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType = MSG_TYPE_SECTION_FILTER_RELEASE_SF;
	sndMsg.tsType  = tsType;
	sndMsg.sfgID   = sfgID;


	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"PORT_N2TV_reserveSFs, END: ret:%d, tsType:%d, sfgID:0x%x\n", result, tsType, sfgID);
	return result;

}

SINT8 PORT_N2TV_startFiltering(BOOLEAN tsType, SINT16 sfgID, SINT16 sfID, SINT16 pid, SINT16 table_id) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"PORT_N2TV_startFiltering, BEGIN: tsType:%d, sfgID:0x%x, sfID:0x%x, pid:0x%x, table_id:0x%x\n", tsType, sfgID, sfID, pid, table_id);

	MSG_SECTION_FILTER_INFO sndMsg;
	MSG_COMMON              retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType = MSG_TYPE_SECTION_FILTER_START_FILTERING;
	sndMsg.tsType  = tsType;
	sndMsg.sfgID   = sfgID;
	sndMsg.sfID    = sfID;
	sndMsg.pID     = pid;
	sndMsg.tableID = table_id;


	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"PORT_N2TV_startFiltering, END: ret:%d, tsType:%d, sfgID:0x%x, sfID:0x%x, pid:0x%x, table_id:0x%x\n", tsType, sfgID, sfID, pid, table_id);
	return result;
}

SINT8 PORT_N2TV_stopFiltering(BOOLEAN tsType, SINT16 sfgID, SINT16 sfID) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"PORT_N2TV_stopFiltering, BEGIN: tsType:%d, sfgID:0x%x, sfID:0x%x\n", tsType, sfgID, sfID);

	MSG_SECTION_FILTER_INFO sndMsg;
	MSG_COMMON              retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType = MSG_TYPE_SECTION_FILTER_STOP_FILTERING;
	sndMsg.tsType  = tsType;
	sndMsg.sfgID   = sfgID;
	sndMsg.sfID    = sfID;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;
	PEM_MWAGT_log(LOG_INFO,"PORT_N2TV_stopFiltering, END: ret:%d, tsType:%d, sfgID:0x%x, sfID:0x%x\n", tsType, sfgID, sfID);
	return result;
}


SINT8 PORT_N2TV_getNumOfAvailableSectionFilters(BOOLEAN tsType, SINT16* pNum) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"PORT_N2TV_getNumOfAvailableSectionFilters, BEGIN: tsType:%d\n", tsType);

	MSG_SECTION_FILTER_INFO sndMsg;
	MSG_COMMON              retMsg;

	memset(&sndMsg, 0, sizeof(sndMsg));
	memset(&retMsg, 0, sizeof(retMsg));

	sndMsg.msgType = MSG_TYPE_SECTION_FILTER_GET_AVAILABLE_SF_NUM;
	sndMsg.tsType  = tsType;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(sndMsg), &retMsg, sizeof(retMsg));
	result = retMsg.sint8;
	*pNum  = retMsg.sint16;
	PEM_MWAGT_log(LOG_INFO,"PORT_N2TV_getNumOfAvailableSectionFilters, END: ret:%d, tsType:%d, available sf Num:%d\n", tsType, *pNum);
	return result;

}


/* ================================================================
 *  PG 6.2 DSMCC
 * ================================================================ */
SINT8 PORT_N2TV_ocFileExist(SINT8* file_full_path) {
	SINT8 result = FAILURE;
	PEM_MWAGT_log(LOG_INFO, "%s() BEGIN : Absolute Path: [%s]\n", __FUNCTION__, file_full_path);

	MSG_STRMSG sndMsg;
	MSG_COMMON retMsg;

	memset(&sndMsg, 0, sizeof(MSG_STRMSG));
	memset(&retMsg, 0, sizeof(MSG_COMMON));

	sndMsg.msgType = MSG_TYPE_OC_FILE_EXIST;
	_snprintf(sndMsg.string_msg, MAX_URL_SIZE, "%s", file_full_path);

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_STRMSG), &retMsg, sizeof(MSG_COMMON));
	result = retMsg.sint8;

	if ( ( file_full_path[0] == '0' ) || ( file_full_path[0] == 's' && file_full_path[1] == 'b' ) ) {
		result = SUCCESS;
	}

	PEM_MWAGT_log(LOG_INFO,"%s(), END : Result: [%d], Absolute Path: [%s]\n", __FUNCTION__, result, file_full_path);
	return result;
}


SINT8 PORT_N2TV_copyOCFile(SINT8* file_full_path) {
	SINT8 result = FAILURE;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN :  Absolute Path: [%s]\n", __FUNCTION__, file_full_path);

	MSG_STRMSG sndMsg;
	MSG_COMMON retMsg;

	memset(&sndMsg, 0, sizeof(MSG_STRMSG));
	memset(&retMsg, 0, sizeof(MSG_COMMON));

	sndMsg.msgType = MSG_TYPE_COPY_OC_FILE;
	_snprintf(sndMsg.string_msg, MAX_URL_SIZE, "%s", file_full_path);

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_STRMSG), &retMsg, sizeof(MSG_COMMON));
	result = retMsg.sint8;

	if ( ( file_full_path[0] == '0' ) || ( file_full_path[0] == 's' && file_full_path[1] == 'b' ) ) {
		result = SUCCESS;
	}

	PEM_MWAGT_log(LOG_INFO,"%s(), END : Result: [%d], Absolute Path: [%s]\n", __FUNCTION__, result, file_full_path);
	return result;
}

SINT8 PORT_N2TV_getMimeType(SINT8* fileFullPath, SINT8* mimeType) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: fileFullPath:%s\n", __FUNCTION__, fileFullPath);

	MSG_STRMSG sndMsg;
	MSG_STRMSG retMsg;

	memset(&sndMsg, 0, sizeof(MSG_STRMSG));
	memset(&retMsg, 0, sizeof(MSG_STRMSG));

	sndMsg.msgType = MSG_TYPE_GET_FILE_MIME_TYPE;
	_snprintf(sndMsg.string_msg, MAX_URL_SIZE, "%s", fileFullPath);

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_STRMSG), &retMsg, sizeof(MSG_STRMSG));
	_snprintf(mimeType, MAX_URL_SIZE, "%s", retMsg.string_msg);
	result = retMsg.sint8;

	PEM_MWAGT_log(LOG_INFO,"%s. END result:%d, fileFullPath:%s, mimeType:%s\n", __FUNCTION__, result, fileFullPath, mimeType);
	return result;
}

SINT8 PORT_N2TV_attach(UINT16 onid, UINT16 tsid, UINT16 sid, UINT32 ctag_id, UINT32 orgId, UINT32 appId, SINT8* mountPoint, UINT32* pAttach_unique_key) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN : onid:0x%x, tsid:0x%x, serviceid:0x%x, ctag_id:0x%x, orgid:0x%x, appid:0x%x\n",
			       __FUNCTION__, onid, tsid, sid, ctag_id, orgId, appId);

	MSG_SERVICEINFO sndMsg;
	MSG_STRMSG      retMsg;

	memset(&sndMsg, 0, sizeof(MSG_SERVICEINFO));
	memset(&retMsg, 0, sizeof(MSG_STRMSG));

	sndMsg.msgType    = MSG_TYPE_ATTACH;
	sndMsg.on_id      = onid;
	sndMsg.ts_id      = tsid;
	sndMsg.service_id = sid;
	sndMsg.ctag       = ctag_id;
	sndMsg.orgid      = orgId;
	sndMsg.appid      = appId;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_SERVICEINFO), &retMsg, sizeof(MSG_STRMSG));
	_snprintf(mountPoint, MAX_URL_SIZE, "%s", retMsg.string_msg);
	*pAttach_unique_key = retMsg.uint32;
	result = retMsg.sint8;

	PEM_MWAGT_log(LOG_INFO,"%s, END: result:%d, onid:0x%x, tsid:0x%x, serviceid:0x%x, ctag_id:0x%x,  orgId:0x%x, appid:0x%x mountPoint:%s, attach_unique_id:%d\n",
				__FUNCTION__, result, onid, tsid, sid, ctag_id, orgId, appId, mountPoint, *pAttach_unique_key);
	return result;
}

SINT8 PORT_N2TV_detach(UINT16 onid, UINT16 tsid, UINT16 sid, UINT32 ctag, UINT32 attach_unique_id) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: onid:0x%x, tsid:0x%x, serviceid:0x%x, ctag_id:0x%x, attach_unique_id:%d\n", __FUNCTION__, onid, tsid, sid, ctag, attach_unique_id);

	MSG_SERVICEINFO sndMsg;
	MSG_COMMON      retMsg;

	memset(&sndMsg, 0, sizeof(MSG_SERVICEINFO));
	memset(&retMsg, 0, sizeof(MSG_COMMON));

	sndMsg.msgType          = MSG_TYPE_DETACH;
	sndMsg.on_id            = onid;
	sndMsg.ts_id            = tsid;
	sndMsg.service_id       = sid;
	sndMsg.ctag             = ctag;
	sndMsg.attach_unique_id = attach_unique_id;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_SERVICEINFO), &retMsg, sizeof(MSG_COMMON));
	result = retMsg.sint8;

	PEM_MWAGT_log(LOG_INFO,"%s, END: result:%d, onid:0x%x, tsid:0x%x, serviceid:0x%x, ctag_id:0x%x, attach_unique_id:%d\n", __FUNCTION__, result, onid, tsid, sid, ctag, attach_unique_id);
	return result;
}


SINT8 PORT_N2TV_addBroadCastFileListener(SINT8* fileFullPath) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s,BEGIN:  fileFullPath:%s\n", __FUNCTION__, fileFullPath);

	MSG_STRMSG sndMsg;
	MSG_COMMON retMsg;

	memset(&sndMsg, 0, sizeof(MSG_STRMSG));
	memset(&retMsg, 0, sizeof(MSG_COMMON));

	sndMsg.msgType = MSG_TYPE_ADD_BROADCAST_FILE_LISTENER;
	_snprintf(sndMsg.string_msg, MAX_URL_SIZE, "%s", fileFullPath);
	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_STRMSG), &retMsg, sizeof(MSG_COMMON));
	result = retMsg.sint8;

	PEM_MWAGT_log(LOG_INFO,"%s. END. result:%d, fileFullPath:%s\n", __FUNCTION__, result, fileFullPath);
	return result;
}

SINT8 PORT_N2TV_removeBroadCastFileListener(SINT8* fileFullPath) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: fileFullPath:%s\n", __FUNCTION__, fileFullPath);

	MSG_STRMSG sndMsg;
	MSG_COMMON retMsg;

	memset(&sndMsg, 0, sizeof(MSG_STRMSG));
	memset(&retMsg, 0, sizeof(MSG_COMMON));

	sndMsg.msgType = MSG_TYPE_REMOVE_BROADCAST_FILE_LISTENER;
	_snprintf(sndMsg.string_msg, MAX_URL_SIZE, "%s", fileFullPath);

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_STRMSG), &retMsg, sizeof(MSG_COMMON));
	result = retMsg.sint8;

	PEM_MWAGT_log(LOG_INFO,"%s. END: result:%d, fileFullPath:%s\n", __FUNCTION__, result, fileFullPath);
	return result;
}

SINT8 PORT_N2TV_attachStreamEvent(UINT16 onid, UINT16 tsid, UINT16 sid, UINT8 ctag) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: onid:0x%x, tsid:0x%x, serviceid:0x%x, ctag:0x%x\n", __FUNCTION__, onid, tsid, sid, ctag);

	MSG_SERVICEINFO sndMsg;
	MSG_COMMON      retMsg;

	memset(&sndMsg, 0, sizeof(MSG_SERVICEINFO));
	memset(&retMsg, 0, sizeof(MSG_COMMON));

	sndMsg.msgType = MSG_TYPE_ATTACH_STREAM_EVENT;
	sndMsg.on_id = onid;
	sndMsg.ts_id = tsid;
	sndMsg.service_id = sid;
	sndMsg.ctag = ctag;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_SERVICEINFO), &retMsg, sizeof(MSG_COMMON));
	result = retMsg.sint8;

	PEM_MWAGT_log(LOG_INFO,"%s, END: result:%d, onid:0x%x, tsid:0x%x, serviceid:0x%x, ctag_id:0x%x\n", __FUNCTION__, result, onid, tsid, sid, ctag);
	return result;
}


SINT8 PORT_N2TV_detachStreamEvent(UINT16 onid, UINT16 tsid, UINT16 sid, UINT8 ctag) {
	SINT8 result = -1;
	PEM_MWAGT_log(LOG_INFO,"%s, BEGIN: onid:0x%x, tsid:0x%x, serviceid:0x%x, ctag_id:0x%x\n", __FUNCTION__, onid, tsid, sid, ctag);

	MSG_SERVICEINFO sndMsg;
	MSG_COMMON      retMsg;

	memset(&sndMsg, 0, sizeof(MSG_SERVICEINFO));
	memset(&retMsg, 0, sizeof(MSG_COMMON));

	sndMsg.msgType    = MSG_TYPE_DETACH_STREAM_EVENT;
	sndMsg.on_id      = onid;
	sndMsg.ts_id      = tsid;
	sndMsg.service_id = sid;
	sndMsg.ctag       = ctag;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_SERVICEINFO), &retMsg, sizeof(MSG_COMMON));
	result = retMsg.sint8;

	PEM_MWAGT_log(LOG_INFO,"%s, END: result:%d, onid:0x%x, tsid:0x%x, serviceid:0x%x, ctag_id:0x%x\n", __FUNCTION__, result, onid, tsid, sid, ctag);
	return result;
}



/* ================================================================
 *  PG 6.3 SI
 * ================================================================ */



/* ================================================================
 *  PG 6.4 Stream
 * ================================================================ */



/* ================================================================
 *  PG 7.1 Key Event
 * ================================================================ */
void PORT_N2TV_notifyUnusedKey(UINT16 keyCode, UINT16 keyType) {
	MSG_MOA_KEY sndMsg = { MSG_TYPE_NOTIFY_UNUSED_KEY, keyCode, keyType };

	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN - keyCode:%d(0x%x), keyType:%d(0x%x)\n", __FUNCTION__, sndMsg.keyCode, sndMsg.keyCode, sndMsg.keyType, sndMsg.keyType);
	BA_sendMsgMW2TV(&sndMsg, sizeof(MSG_MOA_KEY));
	PEM_MWAGT_log(LOG_INFO, "%s: END - keyCode:%d(0x%x), keyType:%d(0x%x)\n", __FUNCTION__, sndMsg.keyCode, sndMsg.keyCode, sndMsg.keyType, sndMsg.keyType);
}
/* TODO Check it!
void PORT_N2TV_notifyUnusedKey(UINT16 keyCode, UINT16 keyType) {
        MSG_MOA_KEY sndMsg = { MSG_TYPE_NOTIFY_UNUSED_KEY, keyCode, keyType };

        PEM_MWAGT_log(LOG_INFO, "%s: BEGIN - keyCode:%d(0x%x), keyType:%d(0x%x)\n", __FUNCTION__, sndMsg.keyCode, sndMsg.keyCode, sndMsg.keyType, sndMsg.keyType);
        //2013.10.30 douglas.lima Engine wont send unused key
        //BA_sendMsgMW2TV(&sndMsg, sizeof(MSG_MOA_KEY));
        PEM_MWAGT_log(LOG_INFO, "%s: END - keyCode:%d(0x%x), keyType:%d(0x%x)\n", __FUNCTION__, sndMsg.keyCode, sndMsg.keyCode, sndMsg.keyType, sndMsg.keyType);
}
*/



void PORT_N2TV_registerKeySet(UINT32 keyMask) {
	MSG_PEM_KEY_SET sndMsg;

	memset(&sndMsg, 0, sizeof(MSG_PEM_KEY_SET));

	sndMsg.msgType = MSG_TYPE_REGISTER_KEY_SET;
	sndMsg.keyMask = keyMask;

	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN - keyMask:%d(0x%x) - type: [%d]\n",
				 __FUNCTION__,
				 keyMask, keyMask,
				 MSG_TYPE_REGISTER_KEY_SET);

	BA_sendMsgMW2TV(&sndMsg, sizeof(MSG_PEM_KEY_SET));

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
}

/* ================================================================
 *  PG 8.1 BRIDGE
 * ================================================================ */

SINT8 PORT_N2TV_controlXlet(SINT32 controlCode,
						   SINT32 org_id,
						   SINT32 app_id,
						   SINT8* initialClass,
						   SINT8* classPath,
						   UINT32 x,
						   UINT32 y,
						   SINT32 w,
						   SINT32 h) {
	SINT8 result = -1;
	MSG_CONTROL_XLET sndMsg;
	MSG_COMMON      retMsg;

	PEM_MWAGT_BEGIN();
	PEM_MWAGT_BRIDGE_CONTROL(controlCode, org_id, app_id, initialClass,classPath);

	memset(&sndMsg, 0, sizeof(MSG_CONTROL_XLET));
	memset(&retMsg, 0, sizeof(MSG_COMMON));

	sndMsg.msgType      = MSG_TYPE_BRIDGE_NCL_CONTROLXLET;
	sndMsg.controlCode  = controlCode;
	sndMsg.org_id       = org_id;
	sndMsg.app_id       = app_id;
	sndMsg.x            = x;
	sndMsg.y            = y;
	sndMsg.w            = w;
	sndMsg.h            = h;

	_strncpy(sndMsg.initialClass, initialClass, 256 * sizeof(UINT8));
	_strncpy(sndMsg.classPath, classPath, 1024 * sizeof(UINT8));

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_CONTROL_XLET), &retMsg, sizeof(MSG_COMMON));
	result = retMsg.sint8;

	PEM_MWAGT_END("Result: [%d]", result);
	return result;
}

SINT8 PORT_N2TV_setBridgeVisible(SINT32 playerId, SINT8 visible) {
	SINT8 result = -1;
	MSG_MOA_BRIDGE_VISIBLE sndMsg;
	MSG_COMMON            retMsg;

	PEM_MWAGT_BEGIN("Bridge Visibility [%d]", visible);

	memset(&sndMsg, 0, sizeof(MSG_MOA_BRIDGE_VISIBLE));
	memset(&retMsg, 0, sizeof(MSG_COMMON));

	sndMsg.msgType  = MSG_TYPE_BRIDGE_NCL_SETVISIBLE;
	sndMsg.playerId = playerId;
	sndMsg.visible  = visible;

	BA_sendReceiveMsgMW2TV(&sndMsg, sizeof(MSG_MOA_BRIDGE_VISIBLE), &retMsg, sizeof(MSG_COMMON));
	result = retMsg.sint8;

	PEM_MWAGT_END("Result: [%d]", result);
	return result;
}



#if 1
//MSG_TYPE_BRIDGE_NCL_CREATENCLPLAYER
void PORT_BRIDGE_createNclPlayer(UINT8 * msg) {
	APRINT_BRIDGE("==>");
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);
	//call someone

	int playerId = GINGA_ncl_bridge_createNCLPlayer();
	//how to return ID ?

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");
}
//MSG_TYPE_BRIDGE_NCL_DISPOSENCLPLAYER: { 
void PORT_BRIDGE_diposeNclPlayer(UINT8 * msg) {
	MSG_MOA_BRIDGE_DESTROY *_msg;
	APRINT_BRIDGE("==>");
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);

	_msg = (MSG_MOA_BRIDGE_DESTROY*) msg;
	GINGA_ncl_bridge_disposeNCLPlayer(_msg->playerId);
	//no return code

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");

}

//MSG_TYPE_BRIDGE_NCL_SETBOUNDS
void PORT_BRIDGE_setBounds(UINT8 * msg) {
	MSG_MOA_BRIDGE_BOUNDS *_msg;
	APRINT_BRIDGE("==>");

	_msg = (MSG_MOA_BRIDGE_BOUNDS*) msg;
	GINGA_ncl_bridge_setBounds(_msg->playerId, _msg->screen_width,
			_msg->screen_height, _msg->x1, _msg->y1, _msg->x2, _msg->y2);

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");
}
//MSG_TYPE_BRIDGE_NCL_SETVISIBLE
void PORT_BRIDGE_setVisible(UINT8 * msg) {
	MSG_MOA_BRIDGE_VISIBLE *_msg;
	APRINT_BRIDGE("==>");
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);

	_msg = (MSG_MOA_BRIDGE_VISIBLE*) msg;
	GINGA_ncl_bridge_setVisible(_msg->playerId, _msg->visible);

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");
}
//MSG_TYPE_BRIDGE_NCL_CONTROLDOC
void PORT_BRIDGe_controlDoc(UINT8 * msg) {
	MSG_MOA_BRIDGE_CONTROLDOC *_msg;
	APRINT_BRIDGE("==>");
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);

	_msg = (MSG_MOA_BRIDGE_CONTROLDOC*) msg;
	GINGA_ncl_bridge_controlDoc(_msg->playerId, _msg->controlCode, _msg->param);

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");
}
//MSG_TYPE_BRIDGE_NCL_GETPROPERTYVALUE
void PORT_BRIDGE_getPropertyValue(UINT8 * msg) {
	MSG_MOA_BRIDGE_PROPERTY *_msg;
	APRINT_BRIDGE("==>");
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);

	_msg = (MSG_MOA_BRIDGE_PROPERTY*) msg;
	GINGA_ncl_bridge_getPropertyValue(_msg->playerId, _msg->propertyId,
			_msg->value);

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");
}
//MSG_TYPE_BRIDGE_NCL_EDIT
void PORT_BRIDGE_edit(UINT8 * msg) {
	MSG_MOA_BRIDGE_EDIT *_msg;
	APRINT_BRIDGE("==>");
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);

	_msg = (MSG_MOA_BRIDGE_EDIT*) msg;
	GINGA_ncl_bridge_edit(_msg->playerId, _msg->command, _msg->param1,
			_msg->param2, _msg->param3);

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");
}
//MSG_TYPE_BRIDGE_NCL_SETTINGSNODEGETVALUE
void PORT_BRIDGE_settingsNodeGetValue(UINT8 * msg) {
	MSG_MOA_BRIDGE_SETTINGSNODE *_msg;
	APRINT_BRIDGE("==>");
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);

	_msg = (MSG_MOA_BRIDGE_SETTINGSNODE*) msg;
	GINGA_ncl_bridge_settingsNodesGetValue(_msg->nodeId, _msg->key,
			_msg->value);

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");
}
//MSG_TYPE_BRIDGE_NCL_NOTIFYXLETAPPSTATE
void PORT_BRIDGE_notifyXletAppState(UINT8* msg) {
	MSG_MOA_BRIDGE_XLETAPPSTATE *_msg;
	APRINT_BRIDGE("==>");
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);

	_msg = (MSG_MOA_BRIDGE_XLETAPPSTATE*) msg;
	GINGA_ncl_bridge_notifyXletAppState(_msg->org_id, _msg->app_id,
			_msg->state);

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");
}
//MSG_TYPE_BRIDGE_NCL_NOTIFYPLAYEREVENT
void PORT_BRIDGE_notifyPlayerEvent(UINT8 * msg) {
	MSG_MOA_BRIDGE_NCLPLAYEREVENT *_msg;
	APRINT_BRIDGE("==>");
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);

	_msg = (MSG_MOA_BRIDGE_NCLPLAYEREVENT*) msg;
	GINGA_ncl_bridge_notifyOfNCLPlayerEvent(_msg->playerId, _msg->eventType,
			_msg->value);

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");
}
//MSG_TYPE_BRIDGE_NCL_NOTIFYNCLEVENT
void PORT_BRIDGE_notifyNclEvent(UINT8 * msg) {
	MSG_MOA_BRIDGE_NCLEVENT *_msg;
	APRINT_BRIDGE("==>");
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);

	_msg = (MSG_MOA_BRIDGE_NCLEVENT*) msg;
	GINGA_ncl_bridge_notifyOfNCLEvent(_msg->eventType, _msg->param);

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");
}

//MSG_TYPE_BRIDGE_NCL_CONTROLXLET
void PORT_BRIDGE_controlXlet(UINT8 * msg) {
	MSG_CONTROL_XLET *_msg;
	APRINT_BRIDGE("==>");
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);

	_msg = (MSG_CONTROL_XLET*) msg;

	//GINGA_ncl_bridge_controlXlet(_msg->controlCode, _msg->org_id, _msg->app_id, _msg->initialClass, _msg->classPath, 0, 0, 0, 0);

	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
	APRINT_BRIDGE("<==");
}

#endif



