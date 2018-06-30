#ifndef _PEM_MOA_MSG_H_
#define _PEM_MOA_MSG_H_

#include "ginga_n_port_data.h"

#include "pem_port_osa.h"
#include "pem_moa_common.h"


#ifdef __cplusplus
extern "C" {
#endif



#define STR_MAX_LENGTH			1024
#define MSG_MAX_LENGTH			8192
#define APPID_MAX_LENGTH		13

#ifndef MAX_NUM_CTAGS
#define MAX_NUM_CTAGS			10
#endif

#ifndef MAX_URL_SIZE
#define MAX_URL_SIZE			1024
#endif

/*-------------------------------------------------------------------------
 * Define Message Type
 *-------------------------------------------------------------------------
 */

#define UTIL_MESSAGE_LIST(fnc)							\
										\
	fnc(MSG_TYPE_COMMON)							\
	fnc(MSG_TYPE_EMPTY)							\
	fnc(MSG_TYPE_STR_COMMON)						\
										\
	fnc(MSG_TYPE_GET_TIME)							\
										\
	fnc(MSG_TYPE_NOTIFY_MW_READY)						\
										\
	fnc(MSG_TYPE_DEBUG_KEY)							\
										\
	fnc(MSG_TYPE_GET_NATIVE_SYSTEM_PROPERTY)				\
	fnc(MSG_TYPE_SET_MOA_DEBUG_CONFIG)					\
										\
	fnc(MSG_TYPE_GET_USER_PROPERTY)						\
	fnc(MSG_TYPE_SET_USER_PROPERTY)						\
	fnc(MSG_TYPE_REMOVE_USER_PROPERTY)					\
	fnc(MSG_TYPE_GET_USER_PROPERTIES)					\
	fnc(MSG_TYPE_MOA_USER_PROPERTY_CHANGED)					\
										\
	fnc(MSG_TYPE_SELECT_SERVICE)						\
	fnc(MSG_TYPE_MOA_CH_STATUS)						\
										\
	fnc(MSG_TYPE_GET_BASIC_SI_INFO)						\
	fnc(MSG_TYPE_TUNE)							\
	fnc(MSG_TYPE_CB_TUNE)							\
	fnc(MSG_TYPE_GET_NETWORK_AND_TS_NAME_INFO)				\
	fnc(MSG_TYPE_TUNE_CHANNEL)						\
	fnc(MSG_TYPE_CB_TUNE_CHANNEL)						\
										\
	fnc(MSG_TYPE_APP_EXEC_INFO)						\
	fnc(MSG_TYPE_APP_ATTR_INFO)						\
	fnc(MSG_TYPE_NOTIFY_APP_STATE)						\
	fnc(MSG_TYPE_PRINT_APP_DB)						\
										\
	fnc(MSG_TYPE_NOTIFY_APP_CONTROL)					\
	fnc(MSG_TYPE_APP_REFRESH)						\
	fnc(MSG_TYPE_APP_ADDED)							\
	fnc(MSG_TYPE_APP_CHANGED)						\
	fnc(MSG_TYPE_APP_REMOVED)						\
										\
	fnc(MSG_TYPE_LOAD_MEDIA)						\
	fnc(MSG_TYPE_PLAY_DRIPFEED_MEDIA)					\
	fnc(MSG_TYPE_PLAY_MEDIA)						\
	fnc(MSG_TYPE_STOP_MEDIA)						\
	fnc(MSG_TYPE_CLOSE_MEDIA)						\
	fnc(MSG_TYPE_FREEZE_MEDIA)						\
	fnc(MSG_TYPE_RESUME_MEDIA)						\
	fnc(MSG_TYPE_SET_MEDIA_TIME)						\
	fnc(MSG_TYPE_GET_MEDIA_TIME)						\
	fnc(MSG_TYPE_SET_MEDIA_RATE)						\
	fnc(MSG_TYPE_GET_MEDIA_RATE)						\
	fnc(MSG_TYPE_ADD_MEDIA_LISTENER)					\
	fnc(MSG_TYPE_REMOVE_MEDIA_LISTENER)					\
	fnc(MSG_TYPE_GET_VIDEO_RESOLUTION)					\
	fnc(MSG_TYPE_MANIPULATE_MEDIA_COMPONENTS)				\
	fnc(MSG_TYPE_RESIZE_VIDEO_WITH_CLIPPING)				\
	fnc(MSG_TYPE_GET_CURRENT_SELECTED_MEDIA_INFO)				\
	fnc(MSG_TYPE_GET_BITRATE_INFO)						\
	fnc(MSG_TYPE_GET_FRAMERATE_INFO)					\
	fnc(MSG_TYPE_GET_SUPPORT_AUDIO_INFO)					\
	fnc(MSG_TYPE_GET_CURRENT_AUDIO_INFO)					\
	fnc(MSG_TYPE_GET_VIDEO_FORAMT_INFO)					\
	fnc(MSG_TYPE_IS_SUPPORT_DFC)						\
	fnc(MSG_TYPE_GET_SUPPORT_LANGUAGES)					\
	fnc(MSG_TYPE_GET_CURRENT_LANGUAGE)					\
	fnc(MSG_TYPE_SET_CURRENT_LANGUAGE)					\
	fnc(MSG_TYPE_IS_TEXT_ON)						\
	fnc(MSG_TYPE_SET_TEXT_ON_OFF)						\
	fnc(MSG_TYPE_OPERTATE_AUDIO_CLIP)					\
	fnc(MSG_TYPE_CB_PLAY_MEDIA_STATUS)					\
	fnc(MSG_TYPE_CB_SELECT_MEDIA_STATUS)					\
	fnc(MSG_TYPE_CB_MEDIA_RATE)						\
	fnc(MSG_TYPE_CB_MEDIA_FORMAT)						\
	fnc(MSG_TYPE_CB_MEDIA_TIME)						\
	fnc(MSG_TYPE_CB_MEDIA_TIME_NOTIFICATION)				\
	fnc(MSG_TYPE_CB_MEDIA_LANGUAGE_CHANGE_NOTIFICATION)			\
										\
 	fnc(MSG_TYPE_SECTION_FILTER_RESERVE_SF)					\
	fnc(MSG_TYPE_SECTION_FILTER_RELEASE_SF)					\
	fnc(MSG_TYPE_SECTION_FILTER_START_FILTERING)				\
	fnc(MSG_TYPE_SECTION_FILTER_STOP_FILTERING)				\
	fnc(MSG_TYPE_SECTION_FILTER_GET_AVAILABLE_SF_NUM)			\
	fnc(MSG_TYPE_CB_PUT_SECTION)						\
										\
	fnc(MSG_TYPE_ATTACH)							\
	fnc(MSG_TYPE_DETACH)							\
	fnc(MSG_TYPE_OC_FILE_EXIST)						\
	fnc(MSG_TYPE_COPY_OC_FILE)						\
	fnc(MSG_TYPE_GET_FILE_MIME_TYPE)					\
	fnc(MSG_TYPE_ADD_BROADCAST_FILE_LISTENER)				\
	fnc(MSG_TYPE_REMOVE_BROADCAST_FILE_LISTENER)				\
	fnc(MSG_TYPE_CB_NOTIFY_FILE_CHANGED)					\
	fnc(MSG_TYPE_ATTACH_STREAM_EVENT)					\
	fnc(MSG_TYPE_DETACH_STREAM_EVENT)					\
	fnc(MSG_TYPE_CB_NOTIFY_STREAM_EVENT)					\
										\
	fnc(MSG_TYPE_CB_RESOLUTION_CHANGED)					\
	fnc(MSG_TYPE_CB_MEDIA_STOP)					\
										\
	fnc(MSG_TYPE_CB_PUT_SBTVD_SI_SECTION)					\
										\
	fnc(MSG_TYPE_PUT_KEY)							\
	fnc(MSG_TYPE_NOTIFY_UNUSED_KEY)						\
	fnc(MSG_TYPE_REGISTER_KEY_SET)						\
										\
	fnc(MSG_TYPE_BRIDGE_NCL_CREATENCLPLAYER)				\
	fnc(MSG_TYPE_BRIDGE_NCL_DISPOSENCLPLAYER)				\
	fnc(MSG_TYPE_BRIDGE_NCL_SETBOUNDS)					\
	fnc(MSG_TYPE_BRIDGE_NCL_SETVISIBLE)					\
	fnc(MSG_TYPE_BRIDGE_NCL_CONTROLDOC)					\
	fnc(MSG_TYPE_BRIDGE_NCL_GETPROPERTYVALUE)				\
	fnc(MSG_TYPE_BRIDGE_NCL_EDIT)						\
	fnc(MSG_TYPE_BRIDGE_NCL_SETTINGSNODEGETVALUE)				\
	fnc(MSG_TYPE_BRIDGE_NCL_NOTIFYXLETAPPSTATE)				\
	fnc(MSG_TYPE_BRIDGE_NCL_NOTIFYPLAYEREVENT)				\
	fnc(MSG_TYPE_BRIDGE_NCL_NOTIFYNCLEVENT)					\
	fnc(MSG_TYPE_BRIDGE_NCL_CONTROLXLET)					\

#define DO_MESSAGE_KEY(c) c,
typedef enum {
	null = 0,
	UTIL_MESSAGE_LIST ( DO_MESSAGE_KEY )
	MSG_TYPE_MAX,
} MSG_MOA_TYPE_T;
#undef DO_MESSAGE_KEY




extern const char* getMessageStr(MSG_MOA_TYPE_T index);





/*-------------------------------------------------------------------------
 * Define Message Structure
 *-------------------------------------------------------------------------
 */


/* =================================================================================================
 *  Common Message
 * ================================================================================================= */
typedef struct _COMMON {
	SLONG			msgType;
	SINT8			sint8;
	UINT8			uint8;
	SINT16			sint16;
	UINT16			uint16;
	SINT32			sint32;
	UINT32			uint32;
	SINT64			sint64;
	UINT64			uint64;
	SLONG			slong;
} MSG_COMMON;

typedef struct _STRMSG {
	SLONG			msgType;
	SINT8			sint8;
	UINT32			uint32;
	SINT8			string_msg[MAX_URL_SIZE];//max size is 1023. last is null termination.
} MSG_STRMSG;


/* ================================================================
 *  PG 2.3 Debug
 * ================================================================ */
typedef struct _MSG_DEBUG_KEY {
	SLONG			msgType;
	UINT32			input;
} MSG_DEBUG_KEY;



/* ================================================================
 *  PG 2.4 System Property
 * ================================================================ */
typedef struct _GET_NATIVE_SYSTEM_PROPERTY {
	SLONG			msgType;
	SINT8			key[STR_MAX_LENGTH];
	SINT8			value[STR_MAX_LENGTH];
	SINT8			ret;
} MSG_GET_NATIVE_SYSTEM_PROPERTY;


/* ================================================================
 *  PG 2.5 User Property
 * ================================================================ */
typedef struct _MSG_USER_PROPERTY {
	SLONG			msgType;
	UINT8			szKeyOrFilter[USER_PROPERTY_KEY_MAX_LENGTH];
	UINT8			nSizeOfKeyOrFilter;
	UINT8			szValue[USER_PROPERTY_VALUE_MAX_LENGTH];
	UINT8			nSizeOfValue;
	UINT8			szNewValue[USER_PROPERTY_VALUE_MAX_LENGTH];
	UINT8			nSizeOfNewValue;
	UINT8 			notificationType;
	SINT8			ret;

} MSG_USER_PROPERTY;

typedef struct _MSG_USER_PROPERTIES {
	SLONG			msgType;
	UINT8 			PropertyKeys[USER_PROPERTY_MAX_COUNT][USER_PROPERTY_KEY_MAX_LENGTH];
	UINT8			nCountOfKeys;
	UINT8 			PropertyValues[USER_PROPERTY_MAX_COUNT][USER_PROPERTY_VALUE_MAX_LENGTH];
	UINT8			nCountOfValues;
	SINT8			ret;
} MSG_USER_PROPERTIES;

/* ================================================================
 *  PG 3.2 Tuner
 * ================================================================ */
typedef enum {
	PREVIOUS_CHANNEL,
	NEXT_CHANNEL,
	SPECIFIC_CHANNEL
} TUNE_TYPE;

typedef struct _MSG_NAME_SI_INFO {
	SLONG			msgType;
	UINT8			NetworkName[257];
	UINT8			TSName[65];
	SINT8			ret;
} MSG_NAME_SI_INFO;

typedef struct _MSG_TUNE_INFO {
	SLONG			msgType;
	TUNE_TYPE		tuneType;
	UINT8			niId;
	UINT16			onId;
	UINT16			tsId;
	UINT16			svcId;
	SLONG			callBackId;
} MSG_TUNE_INFO;

/* ================================================================
 *  PG 4.1 Application Control
 * ================================================================ */
typedef struct _MSG_APP_EXEC_INFO {
	SLONG			msgType;
	SINT8			result;
	UINT8			control_code;
	UINT16			onid;
	UINT16			tsid;
	UINT16			sid;
	UINT32			org_id;
	UINT16			app_id;
	APP_EXEC_INFO	app_info;
} MSG_APP_EXEC_INFO;

typedef struct _MSG_APP_ATTR_INFO {
	SLONG			msgType;
	SINT8			result;
	UINT16			onid;
	UINT16			tsid;
	UINT16			sid;
	UINT32			org_id;
	UINT16			app_id;
	APP_ATTR_INFO	app_info;
} MSG_APP_ATTR_INFO;

typedef struct _MSG_APP_CHANGED {
	SLONG			msgType;
	UINT32			num_added_app;
	UINT32			num_changed_app;
	UINT32			num_removed_app;
} MSG_APP_CHANGED;

/* ================================================================
 * PG 5.1 Basic Media Control
 * ================================================================ */
typedef enum _AUDIO_CLIP_COMMAND {
	GET_DEFAULT_LEVEL = 1,
	IS_MUTE,
	SET_MUTE,
	SET_LEVEL,
	GET_LEVEL
} AUDIO_CLIP_COMMAND;

typedef enum _AUDIO_COMMAND {
	SET_SOUND_LEVEL = 0,
	GET_SOUND_LEVEL,
	SET_BALANCE_LEVEL,
	GET_BALANCE_LEVEL,
	SET_TREBLE_LEVEL,
	GET_TREBLE_LEVEL,
	SET_BASS_LEVEL,
	GET_BASS_LEVEL
} _AUDIO_COMMAND;

typedef struct _MEDIAINFO {
	SLONG			msgType;
	UINT8			videoType;
	union {
		struct {
			UINT8			niid;
			UINT16			onid;
			SINT32			tsid;
			UINT16			sid;
			UINT8			cTags[MAX_NUM_CTAGS];
			UINT8			numOfComponents;
			UINT16			pid;
			SINT8			ret;
			SINT32			handlerId;
		};
		struct {
			UINT8			mediaPath[MAX_URL_SIZE];
			UINT8			sizeOfMediaPath;
		};
	};
	SINT8			value;
} MSG_MEDIAINFO;

typedef struct _MSG_MEDIA_CONTROLINFO {
	SLONG	msgType;
	SINT32  handlerId;

	union {
		struct {
			SINT32 srcX;
			SINT32 srcY;
			SINT32 srcWidth;
			SINT32 srcHeight;
			SINT32 destX;
			SINT32 destY;
			SINT32 destWidth;
			SINT32 destHeight;
		};
		struct {
			UINT8 controlType;
			SINT8 languageCode[NUM_OF_LANGUAGE_CODE];
		};
		struct {
			UINT8  commandType;
			SINT32 audioLevel;
		};
		UINT8 niId;
	};
} MSG_MEDIA_CONTROLINFO;

#ifndef MAX_SERVICE_NAME_LENGTH
#define MAX_SERVICE_NAME_LENGTH 40
#endif

typedef struct _MSG_MOA_MEDIA
{
	SLONG	msgType;
	UINT8	status;
	UINT16 	onid;
	UINT16 	tsid;
	UINT16 	sid;
	SINT32  handlerId;

	union {
		struct {
			SINT32	value;
			UINT32	mMediaTime;
			UINT32	lMediaTime;
		};
		struct {
			UINT8 	service_type;
			UINT8 	service_name[MAX_SERVICE_NAME_LENGTH];
			UINT8   service_name_length;
			UINT8   niid;
//			SINT64	transID;
		};
		struct {
			UINT8	cTags[MAX_NUM_CTAGS];
			UINT8	numOfCTags;
		};
		struct {
			SINT8	newType;
			SINT32	newFormat;
		};
		struct {
			SINT8	languageControlType;
			SINT32	eventStatus;
		};
		UINT32 eventId;
	};

} MSG_MOA_MEDIA;

typedef struct _MSG_MOA_MEDIA_CONTROL
{
	SLONG	msgType;
	SINT32	handlerId;
	union {
		struct {
			SINT32	videoWidth;
			SINT32	videoHeight;
		};
		struct {
			SINT32	minValue;
			SINT32	avgValue;
			SINT32	maxValue;
		};
		struct {
			SINT32	audioLayer;
			SINT32	channel;
			SINT32	samplingRate;
			SINT32	lowFrequencyChannel;
			SINT32	multilingualMode;
		};
		SINT32 value;
	};

	SINT8	ret;
} MSG_MOA_MEDIA_CONTROL;

typedef struct _MSG_MOA_MEDIA_CONTROL_SERVICEINFO
{
	SLONG	msgType;
	UINT16	onId;
	SINT32	tsId;
	UINT16	sId;
	UINT8	cTags[MAX_NUM_CTAGS];
	UINT8	numOfComponents;
	SINT8	ret;
} MSG_MOA_MEDIA_CONTROL_SERVICEINFO;

typedef struct _MSG_MOA_MEDIA_CONTROL_LANGUAGEINFO
{
	SLONG	msgType;
	SINT8	ret;
	SINT8	languageCodes[MAX_NUM_OF_LANGS][NUM_OF_LANGUAGE_CODE];
	UINT8	numOfLanguageCode;
} MSG_MOA_MEDIA_CONTROL_LANGUAGEINFO;

/* ================================================================
 *  PG 6.1 Section Filter
 * ================================================================ */

typedef struct _MSG_SECTION_FILTER_INFO {
	SLONG			msgType;
	BOOLEAN			tsType;
	SINT16			sfgID;
	SINT16			sfID;
	SINT16			pID;
	SINT16			tableID;
	SINT8			filterNum;
	UINT32			data_length;
} MSG_SECTION_FILTER_INFO;



/* ================================================================
 *  PG 6.2 Broadcast (DSMCC)
 * ================================================================ */


typedef struct _SERVICEINFO {
	SLONG			msgType;
	UINT16			on_id;		// UINT16 onid, UINT32 freq
	UINT16			ts_id;		//tsid
	UINT16			service_id;		//service id
	UINT32			carouselId;
	UINT32			ctag;
	UINT32			orgid;
	UINT32			appid;
	UINT32			attach_unique_id;
} MSG_SERVICEINFO;


typedef struct _MSG_STREAM_EVENT_INFO {
	SLONG			msgType;
	UINT16			onid;
	UINT16			tsid;
	UINT16			sid;
	UINT8			cTag;
	UINT64			event_id;
	SINT8			event_name[MAX_EVENT_NAME_LENGTH]; //max size is 512. last is null termination.
	UINT32			duration_sec;
	UINT16			duration_micro;
	UINT64			event_time;
	UINT8			privateDataByte[EVENT_DATA_MAX_SIZE]; //max size is 243bit in current Spec.
} MSG_STREAM_EVENT_INFO;


typedef struct _MSG_RESOLUTIONCHANGED_INFO {
	SLONG			msgType;
	UINT16			width;
	UINT16			height;
} MSG_RESOLUTIONCHANGED_INFO;

typedef struct _MSG_MEDIA_STOP_INFO {
	SLONG			msgType;
	SINT32			handlerId;
} MSG_MEDIA_STOP_INFO;
/* ================================================================
 *  PG 6.3 SI
 * ================================================================ */

typedef struct _MSG_SBTVD_SI_SECTION_INFO
{
	SINT32	msgType;
	UINT32	niid;
	UINT32	freq;
	UINT32	onid;
	UINT32	tsid;
	UINT32	pid;
	UINT32	length;
} MSG_SBTVD_SI_SECTION_INFO;


/* =================================================================================================
 *  PG 7.1 Key Event
 * ================================================================================================= */
typedef struct _MSG_MOA_KEY
{
	SLONG	msgType;
	UINT16	keyCode;
	UINT16	keyType;
} MSG_MOA_KEY;

typedef struct _MSG_PEM_KEY_SET
{
	SLONG	msgType;
	UINT32	keyMask;
} MSG_PEM_KEY_SET;


/* ================================================================
 *  PG 7.1 Key Event
 * ================================================================ */

typedef struct _MSG_MEDIATIMENOTIFICATION_INFO {
	SINT32			msgType;
	SINT32			handlerId;
	UINT8			niid;
	UINT16			onid;
	UINT16			tsid;
	UINT16			sid;
	UINT8			service_status;
	SINT32			newRate;
	SINT8			newType;
	SINT32			newFormat;
	UINT64			newMediaTime;
	UINT32			eventId;
	LANGUAGE_CONTROL_TYPE	languageControlType;
	GINGA_SUBTITLE_STATUS	eventStatus;
} MSG_MEDIATIMENOTIFICATION_INFO;

/* ================================================================
 *  PG 8.1 Bridge
 * ================================================================ */

typedef struct _MSG_CONTROL_XLET {
	SLONG	msgType;
	SINT32	controlCode;
	UINT32	org_id;				// organization id
	UINT16	app_id;				// app id
	SINT8	initialClass[256];		// class name with null terminated
	SINT8	classPath[1024];		// class path that is absolute path with null terminated
	UINT32	x;				// bound of x
	UINT32	y;				// bound of y
	UINT32	w;				// bound of width
	UINT32	h;				// bound of height
} MSG_CONTROL_XLET;


#define MSG_MOA_BRIDGE_MAX_STRING_LENGTH 1024
typedef struct _MSG_MOA_BRIDGE_DESTROY
{
	SLONG	msgType;
	SINT32	playerId;
} MSG_MOA_BRIDGE_DESTROY;

typedef struct _MSG_MOA_BRIDGE_BOUNDS
{
	SLONG	msgType;
	SINT32	playerId;
	SINT32	screen_width;
	SINT32	screen_height;
	SINT32	x1;
	SINT32	y1;
	SINT32	x2;
	SINT32	y2;
} MSG_MOA_BRIDGE_BOUNDS;


typedef struct _MSG_MOA_BRIDGE_VISIBLE
{
	SLONG	msgType;
	SINT32	playerId;
	SINT8 visible;
} MSG_MOA_BRIDGE_VISIBLE;


typedef struct _MSG_MOA_BRIDGE_CONTROLDOC
{
	SLONG	msgType;
	SINT32	playerId;
	SINT32	controlCode;
	SINT8	param[MSG_MOA_BRIDGE_MAX_STRING_LENGTH];
} MSG_MOA_BRIDGE_CONTROLDOC;


typedef struct _MSG_MOA_BRIDGE_PROPERTY
{
	SLONG	msgType;
	SINT32	playerId;
	SINT8	propertyId[MSG_MOA_BRIDGE_MAX_STRING_LENGTH];
	SINT8	value[MSG_MOA_BRIDGE_MAX_STRING_LENGTH];
} MSG_MOA_BRIDGE_PROPERTY;


typedef struct _MSG_MOA_BRIDGE_EDIT
{
	SLONG	msgType;
	SINT32	playerId;
	SINT32	command;
	SINT8	param1[MSG_MOA_BRIDGE_MAX_STRING_LENGTH];
	SINT8	param2[MSG_MOA_BRIDGE_MAX_STRING_LENGTH];
	SINT8	param3[MSG_MOA_BRIDGE_MAX_STRING_LENGTH];
} MSG_MOA_BRIDGE_EDIT;

typedef struct _MSG_MOA_BRIDGE_SETTINGSNODE
{
	SLONG	msgType;
	SINT32  playerId;
	SINT8   nodeId[MSG_MOA_BRIDGE_MAX_STRING_LENGTH];
	SINT8   key[MSG_MOA_BRIDGE_MAX_STRING_LENGTH];
	SINT8   value[MSG_MOA_BRIDGE_MAX_STRING_LENGTH];
} MSG_MOA_BRIDGE_SETTINGSNODE;

typedef struct _MSG_MOA_BRIDGE_NCLPLAYEREVENT
{
	SLONG	msgType;
	SINT32	playerId;
	SINT32	eventType;
	SINT8	value[MSG_MOA_BRIDGE_MAX_STRING_LENGTH];
} MSG_MOA_BRIDGE_NCLPLAYEREVENT;

typedef struct _MSG_MOA_BRIDGE_NCLEVENT
{
	SLONG	msgType;
	SINT32  eventType;
	SINT8   param[MSG_MOA_BRIDGE_MAX_STRING_LENGTH];
} MSG_MOA_BRIDGE_NCLEVENT;

typedef struct _MSG_MOA_BRIDGE_XLETAPPSTATE
{
	SLONG	msgType;
	SINT32  org_id;
	SINT16  app_id;
	SINT8	state;
} MSG_MOA_BRIDGE_XLETAPPSTATE;



#ifdef __cplusplus
}
#endif


#endif /* _PEM_MOA_MSG_H_ */
