#include "browser_agent.h"
#include "pem_bind_references.h"
#include "gingancl_bridge.h"



SINT8 gIsMAReady	= FALSE;	// MWAgent status

SINT32 gMW2TA_MSG_Q_ID	= -1;		// MsgQ for MW Process     -> TVAgent Thread
SINT32 gTA2MW_MSG_Q_ID	= -1;		// MsgQ for TVAgent Thread -> MW Process
SINT32 gTV2MA_MSG_Q_ID	= -1;		// MsgQ for TV Process     -> MWAgent Thread

SINT32 gMW2TV_SEM_ID	= -1;
SINT64 gMW_SELECTION_ID	= 0;


#define TV2MA_MSG_Q_KEY   0x118
#define MW2TA_MSG_Q_KEY   0x11A
#define TA2MW_MSG_Q_KEY   0x11C

#define MW2TV_SEM_KEY     7002

//Common Core Properties
char ncl_system_language[16] ;

void BA_init() {
	gIsMAReady		= FALSE;	// MWAgent status

	gMW2TA_MSG_Q_ID		= -1;		// MsgQ for MW Process -> TVAgent Thread
	gTA2MW_MSG_Q_ID		= -1;		// MsgQ for TVAgent Thread -> MW Process
	gTV2MA_MSG_Q_ID		= -1;		// MsgQ for TV Process -> MWAgent Thread

	gMW2TV_SEM_ID		= -1;
	gMW_SELECTION_ID	= 0;
}

SINT8 BA_initMsgEnv() {
	SINT8 ret = SUCCESS;

	PEM_MWAGT_BEGIN();

	// 1. create Message Queue for MW Process -> TVAgent Thread
	ret = PEM_PORT_OSA_createMsgQueue((UINT8 *) "MSGQ_MW2TA", MW2TA_MSG_Q_KEY, &gMW2TA_MSG_Q_ID);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to create MsgQ for MW Process -> TVAgent Thread, queue_key=0x%x", MW2TA_MSG_Q_KEY);
	}

	// 2. create Message Queue for TVAgent Thread -> MW Process
	ret = PEM_PORT_OSA_createMsgQueue((UINT8 *) "MSGQ_TA2MW", TA2MW_MSG_Q_KEY, &gTA2MW_MSG_Q_ID);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to create MsgQ for TVAgent Thread -> MW Process, queue_key=0x%x", TA2MW_MSG_Q_KEY);
	}

	// 3. create Message Queue for TV Process -> MWAgent Thread
	ret = PEM_PORT_OSA_createMsgQueue((UINT8 *) "MSGQ_TV2MA", TV2MA_MSG_Q_KEY, &gTV2MA_MSG_Q_ID);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to create MsgQ for TV Process -> MWAgent Thread, queue_key=0x%x", TV2MA_MSG_Q_KEY);
	}

	// 4. create Semaphore for MW -> TVAgent Thread
	ret = PEM_PORT_OSA_createSemaphore((SINT8 *) "sMW2TV", MW2TV_SEM_KEY, &gMW2TV_SEM_ID);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to create semaphore for MW2TV");
	}

	PEM_MWAGT_END("Result: %d", ret);
	return ret;
}

SINT8 BA_sendMsgMW2TV(void *sndMsg, UINT32 sndMsgSize) {
	SINT8 ret = SUCCESS;

	PEM_MWAGT_BEGIN("Message length = %d", sndMsgSize);

	PEM_PORT_OSA_takeSemaphore(gMW2TV_SEM_ID);
	ret = PEM_PORT_OSA_sendMsg(gMW2TA_MSG_Q_ID, sndMsg, sndMsgSize);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to send message!");
	}
	PEM_PORT_OSA_giveSemaphore(gMW2TV_SEM_ID);

	PEM_MWAGT_END("Result: %d", ret);
	return ret;
}

SINT8 BA_sendReceiveMsgMW2TV(void *sndMsg, UINT32 sndMsgSize, void *rcvMsg, UINT32 rcvMsgSize) {
	SINT8 ret = SUCCESS;

	PEM_MWAGT_BEGIN("Message length = %d", sndMsgSize);

	PEM_PORT_OSA_takeSemaphore(gMW2TV_SEM_ID);
	ret = PEM_PORT_OSA_sendMsg(gMW2TA_MSG_Q_ID, sndMsg, sndMsgSize);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to send message!");
		PEM_PORT_OSA_giveSemaphore(gMW2TV_SEM_ID);
		PEM_MWAGT_END("Result: %d", ret);
		return ret;
	}

	ret = PEM_PORT_OSA_receiveMsg(gTA2MW_MSG_Q_ID, rcvMsg, rcvMsgSize);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to receive message!");
	}
	PEM_PORT_OSA_giveSemaphore(gMW2TV_SEM_ID);

	PEM_MWAGT_END("Result: %d, Message length = %d", ret, rcvMsgSize);
	return ret;
}

SINT8 BA_receiveMsgTV2MW(void *rcvMsg, UINT32 rcvMsgSize) {
	SINT8 ret = SUCCESS;

	PEM_MWAGT_BEGIN("Message length = %d", rcvMsgSize);

	ret = PEM_PORT_OSA_receiveMsg(gTV2MA_MSG_Q_ID, rcvMsg, rcvMsgSize);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to receive message!");
	}

	PEM_MWAGT_END("Result: %d, Message length = %d", ret, rcvMsgSize);
	return ret;
}

SINT8 BA_finish() {
	SINT8 ret = SUCCESS;

	PEM_MWAGT_BEGIN();

	ret = PEM_PORT_OSA_removeMsgQueue(gMW2TA_MSG_Q_ID);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to finish MsgQ for MW Process -> TVAgent Thread, queue_key=0x%x", MW2TA_MSG_Q_KEY);
	}

	ret = PEM_PORT_OSA_removeMsgQueue(gTA2MW_MSG_Q_ID);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to finish MsgQ for TVAgent Thread -> MW Process, queue_key=0x%x", TA2MW_MSG_Q_KEY);
	}

	ret = PEM_PORT_OSA_removeMsgQueue(gTV2MA_MSG_Q_ID);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to finish MsgQ for TV Process -> MWAgent Thread, queue_key=0x%x", TV2MA_MSG_Q_KEY);
	}

	ret = PEM_PORT_OSA_removeSemaphore(gMW2TV_SEM_ID);
	if (ret == FAILURE) {
		PEM_MWAGT_FATAL("Fail to remove semaphore!");
	}

	PEM_MWAGT_END("Result: ", ret);
	return ret;
}


void BA_main() {
	int is_running = 1;
	SINT8 msg[MSG_MAX_LENGTH];

	PEM_MWAGT_BEGIN();

	char value[1024];

	SINT8 ret = 0;

	ret = PORT_N2TV_getNativeSystemProperty((SINT8 *)"system.language", (SINT8 *) &value);

	memset(ncl_system_language, 0, sizeof(ncl_system_language));

	if (ret == 1)
	{
		if (strcmp(value,"por"))
		{
			strcpy (ncl_system_language, "pt");
		}
		else if (strcmp(value,"spa"))
		{
			strcpy (ncl_system_language, "es");
		}
		else
		{
			strcpy (ncl_system_language, "pt");
		}
	}
	else
	{
		strcpy (ncl_system_language, "pt");
	}
	
	while (is_running) {

		if (gIsMAReady == FALSE) {
			PORT_N2TV_notifyMWReady();
			gIsMAReady = TRUE;
		}

		memset(&msg, 0x0, sizeof(SINT8) * MSG_MAX_LENGTH);

		PEM_MWAGT_DEBUG("Trying to get message...");
		if (BA_receiveMsgTV2MW((SINT8 *) &msg, MSG_MAX_LENGTH) == FAILURE) {
			PEM_MWAGT_FATAL("Receiving message error!");

			/////////////////////////////////////////////////
			// TODO : error handling
			usleep(1000 * 1000);
			continue;
		}


		PEM_MWAGT_DEBUG("Before handling msg, msgType = %d (%s)", msg[0], getMessageStr((MSG_MOA_TYPE_T) msg[0]));
		switch (msg[0]) {
			case null: /* If SIGTERM was sent, the message type would be NULL(0).
						  Ignore me because it's not an error! */
						  is_running  = 0;
						  break;

			/* ================================================================
			 *  PG 2.2 Ginge-N Engine Control
			 * ================================================================ */


			/* ================================================================
			 *  PG 2.3 Debug
			 * ================================================================ */
			case MSG_TYPE_DEBUG_KEY 						: { PORT_TV2N_putDebugKey((UINT8 *) msg); break; }


			/* ================================================================
			 *  PG 2.4 System Property
			 * ================================================================ */
			case MSG_TYPE_GET_NATIVE_SYSTEM_PROPERTY		: { PORT_TV2N_notifyGetNativeProperty((UINT8 *) msg); break;}

			/* ================================================================
			 *  PG 2.5 User Property
			 * ================================================================ */

			case MSG_TYPE_MOA_USER_PROPERTY_CHANGED			: { PORT_TV2N_notifyUserPropertyChanged((UINT8 *) msg); break;}

			/* ================================================================
			 *  PG 3.1 Service Selection
			 * ================================================================ */
			case MSG_TYPE_MOA_CH_STATUS				: {
				gMW_SELECTION_ID++;
				PORT_TV2N_notifyServiceSelected(gMW_SELECTION_ID, (UINT8 *) msg);
				break;
			}

#if 0

			/* ================================================================
			 *  PG 3.2 Tuner/CM
			 * ================================================================ */

			case MSG_TYPE_CB_TUNE					:
			case MSG_TYPE_CB_TUNE_CHANNEL			: { PORT_TV2N_notifyTuneEvent(msg); break; }

#endif
			/* ================================================================
			 *  PG 4.1 Application Control
			 * ================================================================ */
			case MSG_TYPE_APP_EXEC_INFO				: { PORT_TV2N_controlApp((UINT8 *) msg); break; }
//			case MSG_TYPE_APP_REFRESH				: { PORT_TV2N_appRefresh((UINT8 *) msg); break; }
//			case MSG_TYPE_APP_CHANGED				: { PORT_TV2N_appChanged(msg); break; }

			/* ================================================================
			 *  PG 5.1 Basic Media Control
			 * ================================================================ */
			case MSG_TYPE_CB_PLAY_MEDIA_STATUS			:
			case MSG_TYPE_CB_SELECT_MEDIA_STATUS			:
			case MSG_TYPE_CB_MEDIA_RATE				:
			case MSG_TYPE_CB_MEDIA_FORMAT				:
			case MSG_TYPE_CB_MEDIA_TIME				:
			case MSG_TYPE_CB_MEDIA_LANGUAGE_CHANGE_NOTIFICATION	:
			case MSG_TYPE_CB_MEDIA_TIME_NOTIFICATION		: { PORT_TV2N_notifyMediaStatus(msg); break; }

			/* ================================================================
			 *  PG 5.2 Media Select Control
			 * ================================================================ */

			/* ================================================================
			 *  PG 6.1 Section Filter
			 * ================================================================ */
//			case MSG_TYPE_CB_PUT_SECTION					: { PORT_TV2N_putSection((UINT8 *) msg); break; }


			/* ================================================================
			 *  PG 6.2 Broadcast (DSMCC)
			 * ================================================================ */
			case MSG_TYPE_CB_NOTIFY_FILE_CHANGED			: { PORT_TV2N_notifyFileChanged((UINT8 *) msg); break; }
			case MSG_TYPE_CB_NOTIFY_STREAM_EVENT			: { PORT_TV2N_notifyStreamEvent((UINT8 *) msg); break; }
			case MSG_TYPE_CB_RESOLUTION_CHANGED			: { PORT_TV2N_notifyResolutionChanged((UINT8 *) msg); break; }
			case MSG_TYPE_CB_MEDIA_STOP			: { PORT_TV2N_notifyMediaStop((UINT8 *) msg); break; }

#if 0
			/* ================================================================
			 *  PG 6.3 SI
			 * ================================================================ */
			case MSG_TYPE_CB_PUT_SBTVD_SI_SECTION			: { PORT_TV2N_putSBTVDSISection(msg); break; }
#endif

			/* ================================================================
			 *  PG 6.4 Stream
			 * ================================================================ */


			/* ================================================================
			 *  PG 7.1 Key Event
			 * ================================================================ */
			case MSG_TYPE_PUT_KEY							: { PORT_TV2N_putKey((UINT8 *) msg); break; }



			/* ================================================================
			 *  PG 7.2 JavaDTV UI
			 * ================================================================ */


			/* ================================================================
			 *  PG 7.3 SBTVD UI
			 * ================================================================ */

			/* ================================================================
			 *	PG 8.1 Bridge
			 * ================================================================ */
			case MSG_TYPE_BRIDGE_NCL_CREATENCLPLAYER		: { PORT_BRIDGE_createNclPlayer((UINT8 *) msg);      break; }
			case MSG_TYPE_BRIDGE_NCL_DISPOSENCLPLAYER		: { PORT_BRIDGE_diposeNclPlayer((UINT8 *) msg);      break; }
			case MSG_TYPE_BRIDGE_NCL_SETBOUNDS				: { PORT_BRIDGE_setBounds((UINT8 *) msg);            break; }
			case MSG_TYPE_BRIDGE_NCL_SETVISIBLE				: { PORT_BRIDGE_setVisible((UINT8 *) msg);           break; }
			case MSG_TYPE_BRIDGE_NCL_CONTROLDOC				: { PORT_BRIDGe_controlDoc((UINT8 *) msg);           break; }
			case MSG_TYPE_BRIDGE_NCL_GETPROPERTYVALUE		: { PORT_BRIDGE_getPropertyValue((UINT8 *) msg);     break; }
			case MSG_TYPE_BRIDGE_NCL_EDIT					: { PORT_BRIDGE_edit((UINT8 *) msg);                 break; }
			case MSG_TYPE_BRIDGE_NCL_SETTINGSNODEGETVALUE	: { PORT_BRIDGE_settingsNodeGetValue((UINT8 *) msg); break; }

			case MSG_TYPE_BRIDGE_NCL_NOTIFYXLETAPPSTATE		: { PORT_BRIDGE_notifyXletAppState((UINT8 *) msg);   break; }

			case MSG_TYPE_BRIDGE_NCL_NOTIFYPLAYEREVENT		: { PORT_BRIDGE_notifyPlayerEvent((UINT8 *) msg);    break; }
			case MSG_TYPE_BRIDGE_NCL_NOTIFYNCLEVENT			: { PORT_BRIDGE_notifyNclEvent((UINT8 *) msg);       break; }
			case MSG_TYPE_BRIDGE_NCL_CONTROLXLET			: { PORT_BRIDGE_controlXlet((UINT8 *) msg);          break; }

			default : {
				PEM_MWAGT_FATAL("Un-handled message! Message Type: %d (%s)...", msg[0], getMessageStr((MSG_MOA_TYPE_T) msg[0]));
			}
		}
		PEM_MWAGT_DEBUG("After handling msg, msgType = %d (%s)...", msg[0], getMessageStr((MSG_MOA_TYPE_T) msg[0]));
	}

	PEM_MWAGT_END("Exiting Browser Agent...");
}


SINT8 BA_isMAReady() {
	return gIsMAReady;
}

