#ifndef __BROWSER_PORT_MW2TV_H__
#define __BROWSER_PORT_MW2TV_H__


#include <pem-tv-util/pem_log.h>
#include <pem-tv-util/pem_moa_common.h>
#include <pem-tv-util/pem_moa_msg.h>
#include <pem-tv-util/pem_port_osa.h>

#include <pem-tv-util/ginga_n_port_data.h>

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h> // for usleep

#include "browser_agent.h"


/* ================================================================
 *  PG 2.1 OS Primitive
 * ================================================================ */
void PORT_N2TV_sleepTime(UINT32 ticks);
SINT8 PORT_N2TV_getClock(UINT32 *MClock, UINT32 *LClock);
SINT8 PORT_N2TV_getTime(UINT64 *time);
SINT8 PORT_N2TV_isNetworkConnectionAvailable(void);


/* ================================================================
 *  PG 2.2 Ginge-J Engine Control
 * ================================================================ */
SINT8 PORT_N2TV_notifyMWReady(void);


/* ================================================================
 *  PG 2.3 Debug
 * ================================================================ */
void PORT_N2TV_natvieMemShow(void);
void PORT_N2TV_MoaShowMM(void);
void PORT_N2TV_MoaMarkRange(void);
void PORT_N2TV_CvmGC(void);
void PORT_N2TV_CvmSetDebugFlags(UINT32 value);
void PORT_N2TV_dumpClassReferences(void);


/* ================================================================
 *  PG 2.4 System Property
 * ================================================================ */
SINT8 PORT_N2TV_getNativeSystemProperty(SINT8* key, SINT8 value[1024]);


/* ================================================================
 *  PG 2.5 User Property
 * ================================================================ */
SINT8 PORT_N2TV_requestUserProperties(UINT8 theCommand, const UINT8 *szTheFilter, UINT8 nSizeOfTheFilter, UINT8 PropertyValue[], UINT8 *nSizeOfPropertyValue, const UINT8 *szNewValue, UINT8 nSizeOfValue);
SINT8 PORT_N2TV_getUserProperties(const UINT8 *szTheFilter, UINT8 nSizeOfTheFilter, UINT8 PropertyKeys[][USER_PROPERTY_KEY_MAX_LENGTH], UINT8 *nCountOfPropertyKeys, UINT8 PropertyValues[][USER_PROPERTY_VALUE_MAX_LENGTH], UINT8 *nCountOfPropertyValues);


/* ================================================================
 *  PG 3.1 Service Selection
 * ================================================================ */
SINT8 PORT_N2TV_selectService(UINT8 niid, UINT16 onid, SINT32 tsid, UINT16 sid, UINT8 cTags[], UINT8 numOfComponents);


/* ================================================================
 *  PG 3.2 Tuner
 * ================================================================ */
SINT8 PORT_N2TV_getCurrentSIBasicInfo(UINT8 theNiid , SINT32 **cResultArray, SINT32 *sizeOfCArray);
SINT8 PORT_N2TV_tune(UINT8 theNiId, UINT16 theOnId, UINT16 theTsId);

/* ================================================================
 *  PG 3.3 CM
 * ================================================================ */
SINT8 PORT_N2TV_getNetworkAndTSName(UINT8 theNiId, UINT16 theOnId, UINT16 theTsId, UINT8 networkName[], UINT8 transportStreamName[]);


/* ================================================================
 *  PG 4.1 Application Control
 * ================================================================ */
void PORT_N2TV_requestAppControl(UINT8 controlCode, UINT32 oid, UINT16 aid);
void PORT_N2TV_notifyAppState(UINT32 oid, UINT16 aid, UINT8 state);
void PORT_N2TV_printAppDB(void);


/* ================================================================
 *  PG 5.1 Basic Media Control
 * ================================================================ */
SINT8 PORT_N2TV_getInputVideoSize(SINT32 *width, SINT32 *height);
SINT8 PORT_N2TV_loadAudioClip(SINT8 type, SINT32 sound_id, SINT8 *path, UINT8 *data, UINT32 size, SINT32 *handler);
SINT8 PORT_N2TV_closeAudioClip(SINT32 handler);
SINT8 PORT_N2TV_playAudioClip(SINT32 handler);
SINT8 PORT_N2TV_stopAudioClip(SINT32 handler);
SINT8 PORT_N2TV_getAudioClipDuration(UINT32 *MDuration, UINT32 *LDuration);
SINT8 PORT_N2TV_loadMedia(UINT8 type, UINT8 niid, UINT16 onid, SINT32 tsid, UINT16 sid, UINT16 pid, UINT8 cTags[], UINT8 numOfComponents, UINT8 *mediaPath, UINT8 sizeOfMediaPath, SINT32 *handlerId);
SINT8 PORT_N2TV_loadMediaClip(char *filename, SINT32 *handlerId);
SINT8 PORT_N2TV_playMedia(SINT32 handlerId);
SINT8 PORT_N2TV_stopMedia(SINT32 handlerId);
SINT8 PORT_N2TV_closeMedia(SINT32 handlerId);
SINT8 PORT_N2TV_setMediaTime(SINT32 handlerId, UINT64 mediaTime);
SINT8 PORT_N2TV_getMediaTime(SINT32 handlerId, UINT32 *MTime, UINT32 *LTime);
SINT8 PORT_N2TV_setMediaRate(SINT32 handlerId, SINT16 theRate);
SINT8 PORT_N2TV_getMediaRate(SINT32 handlerId, SINT16 *theRate);
void PORT_N2TV_addListener(LISTENER_TYPE type, SINT32 handlerId, UINT32 eventId, UINT64 eventTime, const UINT8 *theFilter, UINT8 sizeOfTheFilter);
void PORT_N2TV_removeListener(LISTENER_TYPE type, SINT32 handlerId, UINT32 eventId, const UINT8 *theFilter, UINT8 sizeOfTheFilter);
SINT8 PORT_N2TV_playVideoDrips(SINT8* data, UINT32 length);
SINT8 PORT_N2TV_getVideoResolution(SINT32 theHandlerId, SINT32 *videoWidth, SINT32 *videoHeight, VIDEO_IS_DECODING isDecoding);
SINT8 PORT_N2TV_videoResizeWithClipping(SINT32 theHandlerId, SINT32 theClipX, SINT32 theClipY, SINT32 theClipWidth, SINT32 theClipHeight, SINT32 theDestX, SINT32 theDestY, SINT32 theDestWidth, SINT32 theDestHeight);

SINT8 PORT_N2TV_setSoundLevel(UINT32 value, SINT32 handlerId);
SINT8 PORT_N2TV_getSoundLevel(UINT32 * value, SINT32 handlerId);
SINT8 PORT_N2TV_setBalanceLevel(UINT32 value, SINT32 handlerId);
SINT8 PORT_N2TV_getBalanceLevel(UINT32 * value, SINT32 handlerId);
SINT8 PORT_N2TV_setTrebleLevel(UINT32 value, SINT32 handlerId);
SINT8 PORT_N2TV_getTrebleLevel(UINT32 * value, SINT32 handlerId);
SINT8 PORT_N2TV_setBassLevel(UINT32 value, SINT32 handlerId);
SINT8 PORT_N2TV_getBassLevel(UINT32 * value, SINT32 handlerId);



SINT8 PORT_N2TV_getBitRateInformation(SINT32 theHandlerId, SINT32 *minValue, SINT32 *avgValue, SINT32 *maxValue);
SINT8 PORT_N2TV_getFrameRateInformation(SINT32 theHandlerId, SINT32 *minValue, SINT32 *avgValue, SINT32 *maxValue);
SINT8 PORT_N2TV_getSupportAudioInformation(SINT32 *audioLayer, SINT32 *channel, SINT32 *samplingRate, SINT32 *lowFrequencyChannel, SINT32 *multilingualMode);
SINT8 PORT_N2TV_getCurrentAudioInformation(SINT32 theHandlerId, SINT32 *audioLayer, SINT32 *channel, SINT32 *samplingRate, SINT32 *lowFrequencyChannel, SINT32 *multilingualMode);
SINT8 PORT_N2TV_getVideoFormatInformation(SINT32 theHandlerId, GINGA_VIDEO_FORMAT_TYPE requiredType, SINT32 *value);
SINT8 PORT_N2TV_isSupportDFC(SINT32 theDFC);
SINT8 PORT_N2TV_getSupportedLanguages(SINT32 theHandlerId, LANGUAGE_CONTROL_TYPE theType, SINT8 languages[][NUM_OF_LANGUAGE_CODE], UINT8 *numOfLanguages);
SINT8 PORT_N2TV_getCurrentLanguage(SINT32 theHandlerId, LANGUAGE_CONTROL_TYPE theType, SINT8 language[NUM_OF_LANGUAGE_CODE]);
SINT8 PORT_N2TV_setCurrentLanguage(SINT32 theHandlerId, LANGUAGE_CONTROL_TYPE theType, SINT8 cLanguage[NUM_OF_LANGUAGE_CODE]);
SINT8 PORT_N2TV_isTextOn(SINT32 theHandlerId, UINT8 *isOn);
SINT8 PORT_N2TV_setTextOnOff(SINT32 theHandlerId, UINT8 theOnOff);
SINT8 PORT_N2TV_freezeMedia(SINT32 handlerId);
SINT8 PORT_N2TV_resumeMedia(SINT32 handlerId);
SINT8 PORT_requestAudioClipOperation(SINT32 handlerId, AUDIO_CLIP_COMMAND theCommand, SINT32 theSetValue, SINT32 *theResult);


/* ================================================================
 *  PG 5.2 Media Select Control
 * ================================================================ */
SINT8 PORT_N2TV_maniplateComponents(MEDIA_SELECT_COMMAND command, SINT32 theHandlerId, UINT8 theNiid, UINT16 onId, UINT32 tsId, UINT16 sId, UINT8 cTags[], UINT8 numOfComponents);
SINT8 PORT_N2TV_getCurrentMediaSelection(UINT8 theNiid, SINT32 theHandlerId, UINT16 *onId, UINT32 *tsId, UINT16 *sId, UINT8 cTags[], UINT8 *sizeOfCTags);

/* ================================================================
 *  PG 6.1 Section Filter
 * ================================================================ */

SINT8 PORT_N2TV_reserveSFs(BOOLEAN tsType, SINT16 sfgID, SINT8 filterNum);
SINT8 PORT_N2TV_releaseSFs(BOOLEAN tsType, SINT16 sfgID);
SINT8 PORT_N2TV_startFiltering(BOOLEAN tsType, SINT16 sfgID, SINT16 sfID, SINT16 pid, SINT16 table_id);
SINT8 PORT_N2TV_stopFiltering(BOOLEAN tsType, SINT16 sfgID, SINT16 sfID);
SINT8 PORT_N2TV_getNumOfAvailableSectionFilters(BOOLEAN tsType, SINT16* pNum);

/* ================================================================
 *  PG 6.2 Broadcast (DSMCC)
 * ================================================================ */
SINT8 PORT_N2TV_ocFileExist(SINT8* file_full_path);
SINT8 PORT_N2TV_copyOCFile(SINT8* file_full_path);
SINT8 PORT_N2TV_getMimeType(SINT8* fileFullPath, SINT8* mimeType);
SINT8 PORT_N2TV_attach(UINT16 onid, UINT16 tsid, UINT16 sid, UINT32 ctag_id, UINT32 orgId, UINT32 appId, SINT8* mountPoint, UINT32* pAttach_unique_key);
SINT8 PORT_N2TV_detach(UINT16 onid, UINT16 tsid, UINT16 sid, UINT32 ctag, UINT32 attach_unique_id);
SINT8 PORT_N2TV_addBroadCastFileListener(SINT8* fileFullPath);
SINT8 PORT_N2TV_removeBroadCastFileListener(SINT8* fileFullPath);
SINT8 PORT_N2TV_attachStreamEvent(UINT16 onid, UINT16 tsid, UINT16 sid, UINT8 ctag);
SINT8 PORT_N2TV_detachStreamEvent(UINT16 onid, UINT16 tsid, UINT16 sid, UINT8 ctag);


/* ================================================================
 *  PG 6.3 SI
 * ================================================================ */



/* ================================================================
 *  PG 6.4 Stream
 * ================================================================ */



/* ================================================================
 * PG 7.1 Key Event
 * ================================================================ */
void PORT_N2TV_notifyUnusedKey(UINT16 keyCode, UINT16 keyType);
void PORT_N2TV_registerKeySet(UINT32 keyMask);



/* ================================================================
 *  PG 7.2 JavaDTV UI
 * ================================================================ */



/* ================================================================
 *  PG 7.3 SBTVD UI
 * ================================================================ */
SINT8 PORT_N2TV_controlXlet(SINT32 controlCode,
			   SINT32 org_id,
			   SINT32 app_id,
			   SINT8* initialClass,
			   SINT8* classPath,
			   UINT32 x,
			   UINT32 y,
			   SINT32 w,
			   SINT32 h);
SINT8 PORT_N2TV_setBridgeVisible(SINT32 playerId,
				SINT8 visible);


#if 1
//MSG_TYPE_BRIDGE_NCL_CREATENCLPLAYER
void PORT_BRIDGE_createNclPlayer(UINT8 * msg);
//MSG_TYPE_BRIDGE_NCL_DISPOSENCLPLAYER
void PORT_BRIDGE_diposeNclPlayer(UINT8 * msg);
//MSG_TYPE_BRIDGE_NCL_SETBOUNDS
void PORT_BRIDGE_setBounds(UINT8 * msg);
//MSG_TYPE_BRIDGE_NCL_SETVISIBLE
void PORT_BRIDGE_setVisible(UINT8 * msg);
//MSG_TYPE_BRIDGE_NCL_CONTROLDOC
void PORT_BRIDGe_controlDoc(UINT8 * msg);
//MSG_TYPE_BRIDGE_NCL_GETPROPERTYVALUE
void PORT_BRIDGE_getPropertyValue(UINT8 * msg);
//MSG_TYPE_BRIDGE_NCL_EDIT: 
void PORT_BRIDGE_edit(UINT8 * msg);
//MSG_TYPE_BRIDGE_NCL_SETTINGSNODEGETVALUE
void PORT_BRIDGE_settingsNodeGetValue(UINT8 * msg);

//MSG_TYPE_BRIDGE_NCL_NOTIFYXLETAPPSTATE
void PORT_BRIDGE_notifyXletAppState(UINT8 * msg);

//MSG_TYPE_BRIDGE_NCL_NOTIFYPLAYEREVENT
void PORT_BRIDGE_notifyPlayerEvent(UINT8 * msg);
//MSG_TYPE_BRIDGE_NCL_NOTIFYNCLEVENT
void PORT_BRIDGE_notifyNclEvent(UINT8 * msg);
//MSG_TYPE_BRIDGE_NCL_CONTROLXLET
void PORT_BRIDGE_controlXlet(UINT8 * msg);
#endif

#endif /* __BROWSER_PORT_MW2TV_H__ */
