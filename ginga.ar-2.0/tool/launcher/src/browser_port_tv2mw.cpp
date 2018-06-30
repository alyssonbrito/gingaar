#include "browser_port_tv2mw.h"


static const char *appControlCodeToStr(int cc) {
	switch (cc) {
		case APP_SIGNAL_START     : return ((const char*) "APP_SIGNAL_START"    );
		case APP_PROXY_START      : return ((const char*) "APP_PROXY_START"     );
		case APP_SIGNAL_DESTROY   : return ((const char*) "APP_SIGNAL_DESTROY"  );
		case APP_SIGNAL_KILL      : return ((const char*) "APP_SIGNAL_KILL"     );
		case APP_PROXY_STOP       : return ((const char*) "APP_PROXY_STOP"      );
		case APP_SIGNAL_KILL_ALL  : return ((const char*) "APP_SIGNAL_KILL_ALL" );
		default                   : return ((const char*) "INVALID CC!"         );
	}
}

/* ================================================================
 *  PG 2.1 OS Primitive
 * ================================================================ */

/* ================================================================
 *  PG 2.2 Ginge-N Engine Control
 * ================================================================ */

/* ================================================================
 *  PG 2.3 Debug
 * ================================================================ */

void PORT_TV2N_putDebugKey(UINT8 *rcvMsg)
{
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);
	MSG_DEBUG_KEY *msg = (MSG_DEBUG_KEY *) rcvMsg;
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN - keyCode:%d(0x%x)\n", __FUNCTION__, msg->input);
	notify_pem_putKey(msg->input, 0);
	PEM_MWAGT_log(LOG_INFO, "%s: END - keyCode:%d(0x%x)\n", __FUNCTION__, msg->input);
}



/* ================================================================
 *  PG 2.4 System Property
 * ================================================================ */

void PORT_TV2N_notifyGetNativeProperty(UINT8 *recievedMsg) {
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);
	MSG_GET_NATIVE_SYSTEM_PROPERTY *msg = (MSG_GET_NATIVE_SYSTEM_PROPERTY *)recievedMsg;
	notify_pem_notifyGetNativeProperty((char*)msg->value);
	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);

}
/* ================================================================
 *  PG 2.5 User Property
 * ================================================================ */

void PORT_TV2N_notifyUserPropertyChanged(UINT8 *recievedMsg) {
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);
	MSG_USER_PROPERTY *msg = (MSG_USER_PROPERTY *)recievedMsg;
	//JNI_notifyUserPropertyChanged(msg->notificationType, msg->szKeyOrFilter, msg->szValue, msg->szNewValue);
	//
	notify_pem_notifyUserPropertyChanged(msg->notificationType,(char*)msg->szKeyOrFilter,(char*)msg->szValue,(char*)msg->szNewValue);
	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);

}


/* ================================================================
 *  PG 3.1 Service Selection
 * ================================================================ */
void PORT_TV2N_notifyServiceSelected(SINT64 /*transID*/, UINT8 * /*recievedMsg*/) {
	//MSG_MOA_MEDIA *msg = (MSG_MOA_MEDIA*)recievedMsg;
	//dont do anything now, we should wait until CONTROL APP
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);
	//JNI_notifyServiceSelected(transID, msg->niid, msg->onid, msg->tsid, msg->sid, msg->service_name, msg->service_name_length,	msg->service_type);
	//_PORT_NCL_KillAllApplication();
	//PORT_J2TV_attachStreamEvent(msg->onid, msg->tsid, msg->sid, msg->numOfCTags);
	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
}



/* ================================================================
 *  PG 3.2 Tuner
 * ================================================================ */



/* ================================================================
 *  PG 3.3 CM
 * ================================================================ */



/* ================================================================
 *  PG 4.1 Application Control
 * ================================================================ */

void PORT_TV2N_controlApp(UINT8 *rcvMsg) {
	MSG_APP_EXEC_INFO *msg = (MSG_APP_EXEC_INFO *) rcvMsg;

	PEM_MWAGT_log(LOG_INFO, "%s: code = 0x%x, "
		"( %s ) "
		"onid = 0x%x, "
		"tsid = 0x%x, "
		"sid = 0x%x, "
		"org_id = 0x%x, "
		"app_id = 0x%x\n",
		__FUNCTION__,
		msg->control_code,
		appControlCodeToStr(msg->control_code),
		msg->onid,
		msg->tsid,
		msg->sid,
		msg->org_id,
		msg->app_id);

	switch (msg->control_code) {
		case APP_SIGNAL_START:
		case APP_PROXY_START:
			notify_pem_can_run(msg);
			break;
		case APP_PROXY_STOP:
		case APP_SIGNAL_DESTROY:
		case APP_SIGNAL_KILL:
		case APP_SIGNAL_KILL_ALL:
			notify_pem_can_stop(msg);
			break;
		// For Bridge Control Code
		// Check if it could be used...
		// case APP_PAUSE:
		// case APP_RESUME:
		default:
			PEM_MWAGT_log(LOG_ERROR, "%s: Invalid Control Code!. Values is [%d]\n", __FUNCTION__, msg->control_code);
	}
	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
}

void PORT_TV2N_appRefresh(UINT8 * /*rcvMsg*/) {
	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN\n", __FUNCTION__);
	//JNI_appRefresh();
	PEM_MWAGT_log(LOG_INFO, "%s: END\n", __FUNCTION__);
}

/* ================================================================
 *  PG 5.1 Basic Media Control
 * ================================================================ */
void PORT_TV2N_notifyMediaStatus(SINT8 *recievedMsg) {
	MSG_MOA_MEDIA *msg = (MSG_MOA_MEDIA*)recievedMsg;

	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN - handlerId:%d\n", __FUNCTION__, msg->handlerId);

	switch(msg->msgType) {
		case MSG_TYPE_CB_PLAY_MEDIA_STATUS:
		case MSG_TYPE_CB_SELECT_MEDIA_STATUS:
			notify_pem_media_status(msg->handlerId, CB_TYPE_HANDLER_SERVICE, msg->status, -1, 0, 0, msg->niid, msg->onid, msg->tsid, msg->sid, msg->cTags, msg->numOfCTags);
			break;
		case MSG_TYPE_CB_MEDIA_RATE:
			notify_pem_media_status(msg->handlerId, CB_TYPE_HANDLER_RATE, msg->status, msg->value, 0, 0, 0, -1, -1, -1, 0, 0);
			break;
		case MSG_TYPE_CB_MEDIA_FORMAT:
			notify_pem_media_status(msg->handlerId, CB_TYPE_VIDEO_FORMAT_CONTROL, msg->newType, msg->newFormat, 0, 0, 0, -1, -1, -1, 0, 0);
			break;
		case MSG_TYPE_CB_MEDIA_LANGUAGE_CHANGE_NOTIFICATION:
			notify_pem_media_status(msg->handlerId, CB_TYPE_LANGUAGE_CONTROL, msg->languageControlType, msg->eventStatus, 0, 0, 0, -1, -1, -1, 0, 0);
			break;
		case MSG_TYPE_CB_MEDIA_TIME:
			notify_pem_media_status(msg->handlerId, CB_TYPE_MEDIA_TIME_EVENT_CONTROL, SIGNAL_MEDIA_TIME_POSITION_CHANGED, -1, msg->mMediaTime, msg->lMediaTime, 0, -1, -1, -1, 0, 0);
			break;
		case MSG_TYPE_CB_MEDIA_TIME_NOTIFICATION:
			notify_pem_StreamEvent(msg->handlerId);
			notify_pem_media_status(msg->handlerId, CB_TYPE_MEDIA_TIME_EVENT_CONTROL, SIGNAL_MEDIA_TIME_NOTIFICATION, msg->eventId, 0, 0, 0, -1, -1, -1, 0, 0);
			break;

		default:
			break;
	}
	PEM_MWAGT_log(LOG_INFO, "%s: END - hardlerId:%d\n", __FUNCTION__, msg->handlerId);
}

/* ================================================================
 *  PG 5.2 Media Select Control
 * ================================================================ */

/* ================================================================
 *  PG 6.1 Section Filter
 * ================================================================ */
void PORT_TV2N_putSection(UINT8 *recievedMsg) {
	MSG_SECTION_FILTER_INFO* msg = (MSG_SECTION_FILTER_INFO*)recievedMsg;
	SINT8 *pData;

	PEM_MWAGT_log(LOG_INFO, "%s BEGIN: tsType:%d, sfgID:0x%x, sfID:0x%x, data length:%d\n", __FUNCTION__, msg->tsType, msg->sfgID, msg->sfID, msg->data_length);
	if (PEM_PORT_OSA_readSHM(msg->msgType, &pData, msg->data_length) == -1) {
		return;
	}
	//JNI_putSection(msg->tsType, msg->sfgID, msg->sfID, pData, msg->data_length);
	if(pData !=NULL) free(pData);
	PEM_MWAGT_log(LOG_INFO, "%s END: tsType:%d, sfgID:0x%x, sfID:0x%x, data length:%d\n", __FUNCTION__, msg->tsType, msg->sfgID, msg->sfID, msg->data_length);
}

/* ================================================================
 *  PG 6.2 Broadcast (DSMCC)
 * ================================================================ */

void PORT_TV2N_notifyFileChanged(UINT8 *recievedMsg) {
	MSG_STRMSG* msg = (MSG_STRMSG*)recievedMsg;
	PEM_MWAGT_log(LOG_INFO, "%s BEGIN: changed file:%s\n", __FUNCTION__, msg->string_msg);
//	JNI_notifyFileChanged(msg->string_msg);
//	PEM_notifyFileChanged(msg->string_msg);
	PEM_MWAGT_log(LOG_INFO, "%s END: changed file:%s\n", __FUNCTION__, msg->string_msg);
}

void PORT_TV2N_notifyStreamEvent(UINT8 *recievedMsg) {
	MSG_STREAM_EVENT_INFO* msg = (MSG_STREAM_EVENT_INFO*) recievedMsg;
	PEM_MWAGT_log(LOG_INFO,
		"%s BEGIN. onid:%d, tsId:%d, sid:%d, cag:%d, (eventId:%ld, eventName:%s, duration_sec:%d, duration_micro:%d, event_time:%ld) \n",
		__FUNCTION__, msg->onid, msg->tsid, msg->sid, msg->cTag, msg->event_id,
		msg->event_name, msg->duration_sec,	msg->duration_micro, msg->event_time);

	/*JNI_notifyStreamEvent(msg->onid, msg->tsid, msg->sid, msg->cTag, msg->event_id,
			msg->event_name, msg->duration_sec, msg->duration_micro,
			msg->event_time, msg->privateDataByte);*/
	PEM_MWAGT_log(LOG_INFO,
		"%s END. onid:%d, tsId:%d, sid:%d, cag:%d, (eventId:%ld, eventName:%s, duration_sec:%d, duration_micro:%d, event_time:%ld) \n",
		__FUNCTION__, msg->onid, msg->tsid, msg->sid, msg->cTag, msg->event_id,
		msg->event_name, msg->duration_sec,	msg->duration_micro, msg->event_time);


}

void PORT_TV2N_notifyResolutionChanged(UINT8 *recievedMsg) {
	MSG_RESOLUTIONCHANGED_INFO* msg = (MSG_RESOLUTIONCHANGED_INFO*)recievedMsg;
	
	PEM_MWAGT_log(LOG_INFO, "%s BEGIN: resolution will be changed: width=%d, height=%d\n", __FUNCTION__, msg->width, msg->height);
	notify_pem_recommended_resolution_changed(msg->width, msg->height);
	PEM_MWAGT_log(LOG_INFO, "%s END: resolution has changed: width=%d, height=%d\n", __FUNCTION__, msg->width, msg->height);
}

void PORT_TV2N_notifyMediaStop(UINT8 *recievedMsg) {
	MSG_MEDIA_STOP_INFO* msg = (MSG_MEDIA_STOP_INFO*)recievedMsg;
	
	PEM_MWAGT_log(LOG_INFO, "%s BEGIN: Notify Media Stop \n", __FUNCTION__);
	notify_pem_media_stop(msg->handlerId);
	PEM_MWAGT_log(LOG_INFO, "%s END: Notify Media Stop \n", __FUNCTION__);
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
void PORT_TV2N_putKey(UINT8 *rcvMsg) {
	MSG_MOA_KEY* pMsg = (MSG_MOA_KEY*)rcvMsg;
	int iResult = 0;

	PEM_MWAGT_log(LOG_INFO, "%s: BEGIN - keyCode:%d(0x%x), keyType:%d(0x%x)\n", __FUNCTION__, pMsg->keyCode, pMsg->keyCode, pMsg->keyType, pMsg->keyType);
	iResult = notify_pem_putKey(pMsg->keyCode, pMsg->keyType);

	if (iResult == FAILURE){
		PEM_MWAGT_log(LOG_INFO, "%s: END - NOTICE: KEY RESERVED BUT NOT USED keyCode:%d(0x%x), keyType:%d(0x%x)\n", __FUNCTION__, pMsg->keyCode, pMsg->keyCode, pMsg->keyType, pMsg->keyType);
		PORT_N2TV_notifyUnusedKey(pMsg->keyCode, pMsg->keyType);
	} else {
		PEM_MWAGT_log(LOG_INFO, "%s: END - SUCCESS: keyCode:%d(0x%x), keyType:%d(0x%x) result:%d\n", __FUNCTION__, pMsg->keyCode, pMsg->keyCode, pMsg->keyType, pMsg->keyType, iResult);
	}
}


/* ================================================================
 *  PG 7.2 JavaDTV UI
 * ================================================================ */



/* ================================================================
 *  PG 7.3 SBTVD UI
 * ================================================================ */



 /* ================================================================
  *  PG 8.1 DEBUG
  * ================================================================ */

