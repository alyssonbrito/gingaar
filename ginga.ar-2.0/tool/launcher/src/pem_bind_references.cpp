#include "pem_bind_references.h"


BrowserPresentationEngine bpe;

wqueue<WorkKey*>  queue;

ConsumerThread* KeyMain;



/******************************************************************************
 *                                                                            *
 *                        Print values of some members                        *
 *                                                                            *
 ******************************************************************************/
/**
 *
 */
void printMsgAppExecInfo(MSG_APP_EXEC_INFO *msg) {
	PEM_info(LC_PRINT, "><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><\n");
	PEM_info(LC_PRINT, "><><><><                   MSG_APP_EXEC_INFO                    ><><><><\n");
	PEM_info(LC_PRINT, "><><><><                                                        ><><><><\n");
	PEM_info(LC_PRINT, "><><><>< ControlCode = [0x%04x] [%s]\n", msg->control_code, (msg->control_code == 0x11 || msg->control_code == 0x12) ? "LOAD" : "DESTROY");
	PEM_info(LC_PRINT, "><><><>< onid        = [0x%04x]                                 ><><><><\n", msg->onid);
	PEM_info(LC_PRINT, "><><><>< tsid        = [0x%04x]                                 ><><><><\n", msg->tsid);
	PEM_info(LC_PRINT, "><><><>< sid         = [0x%04x]                                 ><><><><\n", msg->sid);
	PEM_info(LC_PRINT, "><><><>< org_id      = [0x%04x]                                 ><><><><\n", msg->org_id);
	PEM_info(LC_PRINT, "><><><>< app_id      = [0x%04x]                                 ><><><><\n", msg->app_id);
	PEM_info(LC_PRINT, "><><><><                                                        ><><><><\n");
	PEM_info(LC_PRINT, "><><><><                     APP_EXEC_INFO                      ><><><><\n");
	PEM_info(LC_PRINT, "><><><><                                                        ><><><><\n");
	PEM_info(LC_PRINT, "><><><>< ctag        = [0x%04x]                                 ><><><><\n", msg->app_info.ctag);
	PEM_info(LC_PRINT, "><><><>< protocol_id = [0x%04x]                                 ><><><><\n", msg->app_info.protocol_id);
	PEM_info(LC_PRINT, "><><><>< base_dir    = [%s] ><><><><\n", msg->app_info.base_dir);
	PEM_info(LC_PRINT, "><><><>< init_entity = [%s] ><><><><\n", msg->app_info.init_entity);
	PEM_info(LC_PRINT, "><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><\n");
}

/**
 *
 */
void printAfterN2TVattach(SINT8 *mountPoint, UINT32 uniqueKey) {
	PEM_info(LC_PRINT, "><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><\n");
	PEM_info(LC_PRINT, "><><><><                 After N2TV attach                      ><><><><\n");
	PEM_info(LC_PRINT, "><><><><                                                        ><><><><\n");
	PEM_info(LC_PRINT, "><><><>< Mount Point       = [%s] ><><><><\n", mountPoint);
	PEM_info(LC_PRINT, "><><><>< Attach Unique Key = [% 4d]                            ><><><><\n", uniqueKey);
	PEM_info(LC_PRINT, "><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><\n");
}


/**
 *
 */
int loadMedia(const std::string &url, int *handlerId, int mediaType)  {
	int ret = -1;
	PEM_LOG_DEBUG("url = [%s]", (char *) url.c_str());

	ret = PORT_N2TV_loadMedia((UINT8) mediaType,
							   -1,   /* niid            */
							   bpe.appInfo.getOnId(),   /* onid            */
							   bpe.appInfo.getTsID(),   /* tsid            */
							   bpe.appInfo.getSID(),    /* sid             */
							   -1,   /* pid             */
							   0,   /* ctags           */
							   0,    /* numOfComponents */
							   (UINT8*)  url.c_str(),
							   (UINT8)   url.size(),
							   (SINT32*) handlerId);

#if 0
	ret = PORT_N2TV_loadMedia((UINT8) mediaType,
							   -1   /* niid            */,
							   -1   /* onid            */,
							   -1   /* tsid            */,
							   -1   /* sid             */,
							   -1   /* pid             */,
							   NULL /* ctags           */,
							   0    /* numOfComponents */,
							   (UINT8*)  url.c_str(),
							   (UINT8)   url.size(),
							   (SINT32*) handlerId);
#endif

	PEM_LOG_MEDIA(*handlerId);
	return ret;
}

/**
 *
 */
int playMedia(int handlerId) {
	PEM_LOG_MEDIA(handlerId);
	return PORT_N2TV_playMedia(handlerId);
}

/**
 *
 */
int setMediaTime(int handlerId, int time) {
	PEM_LOG_MEDIA(handlerId);
	return PORT_N2TV_setMediaTime(handlerId, time);
}
/**
 *
 */
int stopMedia(int handlerId) {
	PEM_LOG_MEDIA(handlerId);
	return PORT_N2TV_stopMedia(handlerId);
}

/**
 *
 */
int closeMedia(int handlerId) {
	PEM_LOG_MEDIA(handlerId);
	return PORT_N2TV_closeMedia(handlerId);
}

/**
 *
 */
int freezeMedia(int handlerId) {
	PEM_LOG_MEDIA(handlerId);
	return PORT_N2TV_freezeMedia(handlerId);
}

/**
 *
 */
int resumeMedia(int handlerId) {
	PEM_LOG_MEDIA(handlerId);
	return PORT_N2TV_resumeMedia(handlerId);
}

/**
 *
 */
int resizeMedia(int x, int y, int w, int h, int handlerId) {
	int videoWidth  = -1;
	int videoHeight = -1;

	PEM_debug(LC_PRINT, "[%s::%d] Handler ID = %d[0x%x], "
					    "Position = (%d, %d), Screen Size = (%d x %d)\n",
					     __FUNCTION__,
					     __LINE__,
					     handlerId,
					     handlerId,
					     x,
					     y,
					     w,
					     h);

#if 0
	if (PORT_N2TV_getVideoResolution(handlerId, &videoWidth, &videoHeight, WITHOUT_DECODING) == SUCCESS) {

		if ((videoWidth > 1280 && videoWidth <= 1920) && (videoHeight > 720 && videoHeight <= 1080)) {
			/* Resize factor: 1.5 = 1920 / 1280 = 1080 / 720
			 * When the screen size is 1920 x 1080 and application
			 * was developed in 1280 x 720 (also, coordinates was
			 * designed in 1280 x 720) we need to multiply them by
			 * factor.
			 */
			x *= 1.5f;
			y *= 1.5f;
			w *= 1.5f;
			h *= 1.5f;
		}
	}

	PEM_debug(LC_PRINT, "[%s::%d] Handler ID = %d[0x%x] cannot get Video resolution...\n", __FUNCTION__, __LINE__, handlerId, handlerId);
	return FAILURE;
#endif
		return PORT_N2TV_videoResizeWithClipping(handlerId,
							 0, 0, videoWidth, videoHeight,
							 x, y, w, h);
}




/******************************************************************************
 *                                                                            *
 *                  Send notifications to Presentation Engine                 *
 *                                                                            *
 ******************************************************************************/




int notify_pem_notifyUserPropertyChanged(unsigned int changeType, char * key, char *oldValue, char * newValue){
	int result = bpe.notifyUserPropertyChanged(changeType,
						   key,
						   oldValue,
						   newValue);
	return result;
}

int notify_pem_notifyGetNativeProperty(char * value){
	int result = bpe.notifyGetNativeProperty(value);
	return result;
}
/**
 *
 */
int notify_pem_putKey(unsigned int key, unsigned int keyType) {
	int result = FAILURE;
	PEM_LOG_DEBUG("key = %d[0x%x], key type = %d[0x%x]", key, key, keyType, keyType);
	queue.clear();
	WorkKey* item = new WorkKey(NCL_INT_EVT_KEY, key, keyType);
	queue.add(item);
	result = SUCCESS;
	return result;
}


/**
 *
 */
int notify_pem_StreamEvent(SINT32 handlerId) {
	PEM_LOG_MEDIA(handlerId);
	bpe.onMediaStop(handlerId);
	return 0;
}


/**
 *
 */
int notify_pem_recommended_resolution_changed(UINT16 width, UINT16 height) {
	int result = SUCCESS;
	PEM_LOG_DEBUG("New Recommended Resolution request: (%d x %d)", width, height);
	bpe.notifyRecommendedResolution((int) width, (int) height);
	PEM_LOG_DEBUG("Result: [%d]", result);
	return result;
}


/**
 *
 */
int notify_pem_media_stop(SINT32 handlerId) {
	int result = SUCCESS;
	PEM_LOG_DEBUG("Notify Media Stop");
	bpe.notifyMediaStop(handlerId);
	PEM_LOG_DEBUG("Result: [%d]", result);
	return result;
}


/**
 *
 */
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
				UINT8 numOfCTags) {
	int result = FAILURE;
	PEM_LOG_DEBUG("HandlerId: [%d][%p]", handlerId);
	PEM_LOG_DEBUG("Result: [%d]", result);
	return result;
}


/**
 *
 */
void notify_pem_can_stop(MSG_APP_EXEC_INFO *msg) {
	printMsgAppExecInfo(msg);

	if (0 == bpe.appInfo.getAttachUniqueKey()) {
		PEM_LOG_DEBUG("attachkey is empty");
	}

	if (true == bpe.isWorking()) {
		PEM_LOG_DEBUG("another job is already working");
		return;
	}

	if (msg->control_code == APP_SIGNAL_KILL_ALL){
		bpe.appInfo.setOnId(msg->onid);
		bpe.appInfo.setTsID(msg->tsid);
		bpe.appInfo.setSID(msg->sid);
		bpe.appInfo.setCtag(msg->app_info.ctag);
	}

	queue.clear();
	WorkKey* item = new WorkKey(NCL_INT_EVT_MSG, NCL_INT_EVT_MSG_KILL, 0);
	queue.add(item);
}


int _pem_run(MSG_APP_EXEC_INFO *msg) {
	int res = -1;
	int MAX_PATH_LENGTH = 256;
	SINT8 mountPoint[MAX_PATH_LENGTH];
	char absolutePath[MAX_PATH_LENGTH];
	UINT32 attachUniqueKey;

	printMsgAppExecInfo(msg);

	if (bpe.isRunning()){
		PEM_LOG_ERROR("Engine is still processing last event");
		return STATE_APP_IS_RUNNING;
	}


	if (bpe.isWorking()){
		PEM_LOG_ERROR("Engine is still working on last event");
		return STATE_APP_IS_RUNNING;
	}

	res = PORT_N2TV_attach(msg->onid, msg->tsid, msg->sid, msg->app_info.ctag, msg->org_id, msg->app_id, mountPoint, &attachUniqueKey);
	if (res == FAILURE) {
		PEM_LOG_ERROR("Attach failed");
		return STATE_APP_FAILURE;
	}
	printAfterN2TVattach(mountPoint, attachUniqueKey);

	bpe.appInfo.setOnId(msg->onid);
	bpe.appInfo.setSID(msg->sid);
	bpe.appInfo.setCtag(msg->app_info.ctag);
	bpe.appInfo.setOrgID(msg->org_id);
	bpe.appInfo.setAppID(msg->app_id);
	bpe.appInfo.setTsID(msg->tsid);
	bpe.appInfo.setAttachUniqueKey(attachUniqueKey);

	makeAbsolutePath(mountPoint, &msg->app_info, absolutePath);

	res = bpe.loadApplication(std::string (absolutePath));
	if (res == FAILURE) {
		PEM_LOG_ERROR("Load Application failed");

		res = PORT_N2TV_detach(msg->onid, msg->tsid, msg->sid,
							   msg->app_info.ctag, attachUniqueKey);
		if (res != SUCCESS) {
			PEM_LOG_ERROR("Could not detach");
		}
		return STATE_APP_FAILURE;

	}

	onRequestKeySet ("channel.keyCapture", "interactive_keys");

	PEM_LOG_DEBUG("Result = %d", res);
	return STATE_APP_SUCCESS;

}



/**
 *
 */
void notify_pem_can_run(MSG_APP_EXEC_INFO *msg) {
	queue.clear();
	WorkKey* item = new WorkKey(NCL_INT_EVT_MSG, NCL_INT_EVT_MSG_LOAD, 0, *msg);
	queue.add(item);
}



SINT8 mediaProperty( const int op, const int handlerId, const int property, unsigned int *value ) {
	PEM_debug(LC_PRINT, "[%s::%d] Handler ID = %d[0x%x] value = [%d]\n", __FUNCTION__, __LINE__, handlerId, handlerId, *value);
	SINT8 res = FAILURE;
	if (op == 0) // Operation: SET
	{
		switch(property) {
			case 0:
				res = PORT_N2TV_setSoundLevel(*value, handlerId);
				break;
			case 1:
				res = PORT_N2TV_setBalanceLevel(*value, handlerId);
				break;
			case 2:
				res = PORT_N2TV_setTrebleLevel(*value, handlerId);
				break;
			case 3:
				res = PORT_N2TV_setBassLevel(*value, handlerId);
				break;
			default:
				PEM_debug(LC_PRINT, "[%s::%d] Invalid Property!= [%d]\n", __FUNCTION__, __LINE__, property);
		}
	}
	else if (op == 1) // Operation: GET
	{
		switch(property) {
			case 0:
				res = PORT_N2TV_getSoundLevel(value, handlerId);
				break;
			case 1:
				res = PORT_N2TV_getBalanceLevel(value, handlerId);
				break;
			case 2:
				res = PORT_N2TV_getTrebleLevel(value, handlerId);
				break;
			case 3:
				res = PORT_N2TV_getBassLevel(value, handlerId);
				break;
			default:
				PEM_debug(LC_PRINT, "[%s::%d] Invalid Property!= [%d]\n", __FUNCTION__, __LINE__, property);
		}
	}
	else
	{
		PEM_debug(LC_PRINT, "[%s::%d] Invalid Operation!= [%d]\n", __FUNCTION__, __LINE__, op);
		res = FAILURE;
	}

	PEM_debug(LC_PRINT, "[%s::%d] Media Property result: [%s]\n", __FUNCTION__, __LINE__, (res == 0 ? "SUCCESS" : "FAILURE")); /* TODO Improve the function result! */
	return res;
}



void browser_pem_execute(int argc, char *argv[]) {
	KeyMain = new ConsumerThread(queue);
	KeyMain->start();

	bpe.setOnBeforeStart(onBeforeStart);
	bpe.setOnLoadMainFile(onLoadMainFile);
	bpe.setOnFetchFile(onFetchFilesFromCarousel);
	bpe.setOnReserveKeys(onReserveKeys);
	bpe.setOnStopped(onStoppedApp);
	bpe.setOnGetProperty(onGetProperty);
	bpe.setOnGetNativeProperty(onGetNativeSystemProperty);
	bpe.setOnSetProperty(onSetProperty);
	bpe.setOnLoadMedia(&loadMedia);
	bpe.setOnMediaTime(&setMediaTime);
	bpe.setOnPlayMedia(&playMedia);
	bpe.setOnStopMedia(&stopMedia);
	bpe.setOnCloseMedia(&closeMedia);
	bpe.setOnFreezeMedia(&freezeMedia);
	bpe.setOnResumeMedia(&resumeMedia);
	bpe.setOnResizeMedia(&resizeMedia);
	bpe.setOnMediaPropertyChanged(&mediaProperty);
	bpe.setOnKeySetUpdate(&onRequestKeySet);
	bpe.setOnNCLPlayerEvent(GINGA_ncl_bridge_notifyOfNCLPlayerEventcb);
	bpe.setOnNCLStartXlet(GINGA_ncl_bridge_NCLStartXlet);
	bpe.setOnNCLControlXlet(GINGA_ncl_bridge_NCLControlXlet);
	//bpe.setOnXletAppState(GINGA_ncl_bridge_notifyOfXletAppState);
	bpe.setOnNCLSetVisibility(GINGA_ncl_bridge_setVisible);

	bpe.initialize(argc, argv);
}
