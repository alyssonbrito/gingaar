#ifndef __GINGA_N2TV_API_H__
#define __GINGA_N2TV_API_H__

#include "ginga_n_port_data.h"


/* ================================================================
 *  PG 2.1 OS Primitive
 * ================================================================ */
void GINGA_N2TV_sleepTime (UINT32 millisec);
SINT8 GINGA_N2TV_getClock(SINT64 *clock);
SINT8 GINGA_N2TV_getTime (SINT64 *currentTime);


/* ================================================================
 *  PG 2.2 Ginge-N Engine Control
 * ================================================================ */
void GINGA_N2TV_startGingaNTask(void);
void GINGA_N2TV_stopGingaNTask(void);
void GINGA_N2TV_notifyEngineReady(void);


/* ================================================================
 *  PG 2.3 Debug
 * ================================================================ */



/* ================================================================
 *  PG 2.4 System Property
 * ================================================================ */
SINT8 GINGA_N2TV_getNativeSystemProperty(SINT8* key, SINT8 value[1024]);


/* ================================================================
 *  PG 2.5 User Property
 * ================================================================ */
SINT8 GINGA_N2TV_getUserProperty(UINT8 theKeyString[USER_PROPERTY_KEY_MAX_LENGTH], UINT8 nSizeOftheKey, UINT8 szValue[USER_PROPERTY_KEY_MAX_LENGTH], UINT8 *nSizeOfValue);
SINT8 GINGA_N2TV_removeUserProperty(UINT8 theKeyString[USER_PROPERTY_KEY_MAX_LENGTH], UINT8 nSizeOftheKey);
SINT8 GINGA_N2TV_setUserProperty(UINT8 theKeyString[USER_PROPERTY_KEY_MAX_LENGTH], UINT8 nSizeOftheKey, UINT8 theNewString[USER_PROPERTY_VALUE_MAX_LENGTH], UINT8 nSizeOftheNewString, UINT8 szOldValue[USER_PROPERTY_VALUE_MAX_LENGTH], UINT8 *nSizeOfOldValue);


/* ================================================================
 *  PG 3.1 Service Selection
 * ================================================================ */
void GINGA_N2TV_selectService(UINT8 niid, UINT16 onid, SINT32 tsid, UINT16 sid, UINT8 cTags[], UINT8 numOfComponents);


/* ================================================================
 *  PG 3.2 Tuner
 * ================================================================ */


/* ================================================================
 *  PG 3.3 CM
 * ================================================================ */



/* ================================================================
 *  PG 4.1 Application Control
 * ================================================================ */
void GINGA_N2TV_requestAppControl(UINT8 controlCode, UINT32 oid, UINT16 aid);
void GINGA_N2TV_notifyAppState(UINT32 oid, UINT16 aid, UINT8 state);
SINT8 GINGA_N2TV_getAppExecInfo(UINT32 oid, UINT16 aid, APP_EXEC_INFO *info);
SINT8 GINGA_N2TV_getAppAttrInfo(UINT32 oid, UINT16 aid, APP_ATTR_INFO *info);


/* =================================================================================================
 *  4.3 COMMON : Native Layer APIs
 * ================================================================================================= */
// TODO Is it __REALLY__ used?
SINT8 GINGA_N2TV_getMimeType(SINT8* file_full_path, SINT8 value[MAX_URL_SIZE]);


/* ================================================================
 *  PG 5.1 Basic Media Control
 * ================================================================ */
//TODO REMOVE ME! UINT8 GINGA_N2TV_loadMediaClip(UINT8 *filename, SINT32 *handlerId);
SINT8 GINGA_N2TV_loadMedia(UINT8 type, UINT8 niid, UINT16 onid, SINT32 tsid, UINT16 sid, UINT16 pid, UINT8 cTags[], UINT8 numOfComponents, UINT8 mediaPath[], UINT8 sizeOfMediaPath, SINT32 *handlerId);
SINT8 GINGA_N2TV_playMedia(SINT32 handlerId);
SINT8 GINGA_N2TV_playDripfeedMedia(SINT8* data, UINT32 numOfdata);
SINT8 GINGA_N2TV_stopMedia(SINT32 handlerId);
SINT8 GINGA_N2TV_closeMedia(SINT32 handlerId);

SINT8 GINGA_N2TV_resumeMedia(SINT32 handlerId);
SINT8 GINGA_N2TV_freezeMedia(SINT32 handlerId);

SINT8 GINGA_N2TV_setMediaTime(SINT32 handlerId, UINT64 mediaTime);
SINT8 GINGA_N2TV_getMediaTime(SINT32 handlerId, UINT64 *mediaTime);

SINT8 GINGA_N2TV_setMediaRate(SINT32 handlerId, SINT16 rate);
SINT8 GINGA_N2TV_getMediaRate(SINT32 handlerId, SINT16 *rate);




/*
void GINGA_N2TV_addMediaListener(LISTENER_TYPE listenerType, SINT32 handlerId);
void GINGA_N2TV_removeMediaListener(LISTENER_TYPE listenerType, SINT32 handlerId);
void GINGA_N2TV_addMediaTimeNotificationListener(SINT32 handlerId, UINT32 eventId, UINT64 mediaTime);
void GINGA_N2TV_removeMediaTimeNotificationListener(SINT32 handlerId, UINT32 eventId);
*/




SINT8 GINGA_N2TV_getVideoResolutionAfterDecoding(SINT32 theHandlerId, SINT32 *videoWidth, SINT32 *videoHeight);
SINT8 GINGA_N2TV_getVideoResolutionBeforeDecoding(SINT32 theHandlerId, SINT32 *videoWidth, SINT32 *videoHeight);
SINT8 GINGA_N2TV_resizeVideoWithClipping(SINT32 theHandlerId, SINT32 theClipX, SINT32 theClipY, SINT32 theClipWidth, SINT32 theClipHeight, SINT32 theDestX, SINT32 theDestY, SINT32 theDestWidth, SINT32 theDestHeight);
SINT8 GINGA_N2TV_getBitRateInfo(SINT32 theHandlerId, SINT32 *minValue, SINT32 *avgValue, SINT32 *maxValue);
SINT8 GINGA_N2TV_getFrameRateInfo(SINT32 theHandlerId, SINT32 *minValue, SINT32 *avgValue, SINT32 *maxValue);
SINT8 GINGA_N2TV_getSupportAudioInfo(SINT32 *audioLayer, SINT32 *channel, SINT32 *samplingRate, SINT32 *lowFrequencyChannel, SINT32 *multilingualMode);
SINT8 GINGA_N2TV_getCurrentAudioInfo(SINT32 theHandlerId, SINT32 *audioLayer, SINT32 *channel, SINT32 *samplingRate, SINT32 *lowFrequencyChannel, SINT32 *multilingualMode);
SINT8 GINGA_N2TV_getActiveFormatInfo(SINT32 theHandlerId, SINT32 *value);
SINT8 GINGA_N2TV_getAspectRatioInfo(SINT32 theHandlerId, SINT32 *value);
SINT8 GINGA_N2TV_getDecorderFormatInfo(SINT32 theHandlerId, SINT32 *value);
SINT8 GINGA_N2TV_getDisplayAspectRatioInfo(SINT32 theHandlerId, SINT32 *value);
SINT8 GINGA_N2TV_isSupportDFC(SINT32 theDFCValue);
SINT8 GINGA_N2TV_getSupportAudioLanguages(SINT32 theHandlerId, SINT8 languages[][NUM_OF_LANGUAGE_CODE], UINT8 *numOfLanguages);
SINT8 GINGA_N2TV_getSupportTextLanguages(SINT32 theHandlerId, SINT8 languages[][NUM_OF_LANGUAGE_CODE], UINT8 *numOfLanguages);
SINT8 GINGA_N2TV_getCurrentAudioLanguage(SINT32 theHandlerId, SINT8 languages[NUM_OF_LANGUAGE_CODE]);
SINT8 GINGA_N2TV_getCurrentTextLanguage(SINT32 theHandlerId, SINT8 languages[NUM_OF_LANGUAGE_CODE]);
SINT8 GINGA_N2TV_setCurrentAudioLanguage(SINT32 theHandlerId, SINT8 languages[NUM_OF_LANGUAGE_CODE]);
SINT8 GINGA_N2TV_setCurrentTextLanguage(SINT32 theHandlerId, SINT8 languages[NUM_OF_LANGUAGE_CODE]);
SINT8 GINGA_N2TV_isTextOn(SINT32 theHandlerId, UINT8 *isOn);
SINT8 GINGA_N2TV_setTextOnOff(SINT32 theHandlerId, UINT8 theOnOff);
SINT8 GINGA_N2TV_getAudioClipDefaultLevel(SINT32 theHandlerId, SINT32 *theDefaultLevel);
SINT8 GINGA_N2TV_isAudioClipMute(SINT32 theHandlerId, SINT32 *theIsMute);
SINT8 GINGA_N2TV_setAudioClipMute(SINT32 theHandlerId, SINT32 theMuteValue);
SINT8 GINGA_N2TV_setAudioClipLevel(SINT32 theHandlerId, SINT32 theLevel);
SINT8 GINGA_N2TV_getAudioClipLevel(SINT32 theHandlerId, SINT32 *theLevel);
SINT8 GINGA_N2TV_setBalanceLevel(SINT32 handlerId, SINT32 theLevel);
SINT8 GINGA_N2TV_getBalanceLevel(SINT32 handlerId, SINT32 *theLevel);
SINT8 GINGA_N2TV_setTrebleLevel(SINT32 handlerId, SINT32 theLevel);
SINT8 GINGA_N2TV_getTrebleLevel(SINT32 handlerId, SINT32 *theLevel);
SINT8 GINGA_N2TV_setBassLevel(SINT32 handlerId, SINT32 theLevel);
SINT8 GINGA_N2TV_getBassLevel(SINT32 handlerId, SINT32 *theLevel);


/* ================================================================
 *  PG 5.2 Media Select Control
 * ================================================================ */
SINT8 GINGA_N2TV_selectMediaComponents(UINT8 theNiid, SINT32 theHandlerId, UINT16 onId, UINT32 tsId, UINT16 sId, UINT8 cTags[], UINT8 numOfComponents);
SINT8 GINGA_N2TV_addMediaComponent(UINT8 theNiid, SINT32 theHandlerId, UINT16 onId, UINT32 tsId, UINT16 sId, UINT8 cTag);
SINT8 GINGA_N2TV_removeMediaComponent(UINT8 theNiid, SINT32 theHandlerId, UINT16 onId, UINT32 tsId, UINT16 sId, UINT8 cTag);
SINT8 GINGA_N2TV_replaceMediaComponent(UINT8 theNiid, SINT32 theHandlerId, UINT16 onId, UINT32 tsId, UINT16 sId, UINT8 fromTag, UINT8 toTag);
SINT8 GINGA_N2TV_getCurrentSelectedMediaInfo(UINT8 theNiid, SINT32 theHandlerId, UINT16 *onId, UINT32 *tsId, UINT16 *sId, UINT8 cTags[], UINT8 *sizeOfCTags);



/* ================================================================
 *  PG 6.1 Section Filter
 * ================================================================ */



/* ================================================================
 *  PG 6.2 DSMCC
 * ================================================================ */
SINT8 GINGA_N2TV_attach							(UINT16 onid, UINT16 tsid, UINT16 sid, UINT32 ctag_id, UINT32 org_id, UINT32 app_id, SINT8* pMountPoint, UINT32* pAttach_unique_key);
SINT8 GINGA_N2TV_detach							(UINT16 onid, UINT16 tsid, UINT16 sid, UINT32 ctag, UINT32 attach_unique_key);
SINT8 GINGA_N2TV_ocFileExist					(SINT8* file_full_path);
SINT8 GINGA_N2TV_copyOCFile						(SINT8* file_full_path);
SINT8 GINGA_N2TV_addBroadCastFileListener		(SINT8* file_full_path);
SINT8 GINGA_N2TV_removeBroadCastFileListener	(SINT8* file_full_path);
SINT8 GINGA_N2TV_attachStreamEvent				(UINT16 onid, UINT16 tsid, UINT16 sid, UINT8 cTag);
SINT8 GINGA_N2TV_detachStreamEvent				(UINT16 onid, UINT16 tsid, UINT16 sid, UINT8 cTag);




/* ================================================================
 *  PG 6.3 SI
 * ================================================================ */



/* ================================================================
 *  PG 6.4 Stream
 * ================================================================ */



/* ================================================================
 *  PG 7.1 Key Event
 * ================================================================ */
void GINGA_N2TV_notifyUnusedKey(UINT16 keyCode, UINT16 keyType);
void GINGA_N2TV_registerKeySet(UINT32 keyMask);



/* ================================================================
 *  PG 7.2 JavaDTV UI
 * ================================================================ */



/* ================================================================
 *  PG 7.3 SBTVD UI
 * ================================================================ */


/* ================================================================
 *  PG X.Y Bridge
 * ================================================================ */
SINT8 GINGA_N2TV_controlXlet ( CONTROL_XLET * control );
SINT8 GINGA_N2TV_setXletVisibility ( SINT8 visible );

#endif /* __GINGA_N2TV_API_H__ */
