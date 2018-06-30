/****************************************************************************
 DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 Copyright(c) 2005 by LG Electronics Inc.

 All rights reserved. No part of this work may be reproduced,
 stored in a retrieval system, or transmitted by any means without
 prior written permission of LG Electronics Inc.


 FILE NAME   : mw_port_data.h
 VERSION     : 1.0
 AUTHOR      : ggandyt@lge.com
 DATE        : 2009.09.01
 DESCRIPTION : X_MOA Port data structure definition file
 HISTORY     :

 ******************************************************************************/
#ifndef _GINGA_N_PORT_DATA_H_
#define _GINGA_N_PORT_DATA_H_

#include "pem_moa_common.h"

/* ================================================================
 *  PG 4.1 Application Control
 * ================================================================ */

#define AM_BASE_DIR_MAX_LENGTH			256
#define AM_PATH_EXT_MAX_LENGTH			256
#define AM_INIT_ENTITY_MAX_LENGTH		256
#define AM_URL_MAX_LENGTH				256
#define AM_PARAM_MAX_LENGTH				256
#define AM_ICON_LOC_MAX_LENGTH			256
#define AM_NAME_MAX_LENGTH				50
#define AM_NAME_MAX_NUM					10
#define AM_PROFILE_MAX_NUM				10
#define AM_APP_ATTR_MAX_NUM				15

typedef enum {
	APP_SIGNAL_START	= 0x11,		// start by AIT
	APP_PROXY_START		= 0x12,		// start by ApplicationProxy

	APP_SIGNAL_DESTROY	= 0x20,		// stop by AIT Signal DESTROY (destroyXlet may fail)
	APP_SIGNAL_KILL		= 0x21,		// stop by AIT Signal KILL or by Service Selection or by TV System (destoryXlet by force)
	APP_PROXY_STOP		= 0x22,		// stop by ApplicationProxy
	APP_SIGNAL_KILL_ALL	= 0x23,		// stop all by TV system

	APP_BRIDGE_PAUSE	= 0x31,		// for bridge! I need documentation
	APP_BRIDGE_RESUME	= 0x41,		// for bridge! I need documentation

} APP_CONTROL_CODE;



// BRIDGE

typedef enum {
    NCL_SET_DOC		= 0X01,
    NCL_START_DOC	= 0X07,
    NCL_STOP_DOC	= 0X08,
    NCL_PAUSE_DOC	= 0X09,
    NCL_RESUME_DOC	= 0X0A
} NCL_CONTROL_CODE;

typedef enum {
    ADD_REGION = 0x0B,
    REMOVE_REGION,
    ADD_REGION_BASE,
    REMOVE_REGION_BASE,
    ADD_RULE,
    REMOVE_RULE,
    ADD_RULE_BASE,
    REMOVE_RULE_BASE,
    ADD_CONNECTOR,
    REMOVE_CONNECTOR,
    ADD_CONNECTOR_BASE,
    REMOVE_CONNECTOR_BASE,
    ADD_DESCRIPTOR,
    REMOVE_DESCRIPTOR,
    ADD_DESCRIPTOR_SWITCH,
    REMOVE_DESCRIPTOR_SWITCH,
    ADD_DESCRIPTOR_BASE,
    REMOVE_DESCRIPTOR_BASE,
    ADD_TRANSITION,
    REMOVE_TRANSITION,
    ADD_TRANSITION_BASE,
    REMOVE_TRANSITION_BASE,
    ADD_IMPORT_BASE,
    REMOVE_IMPORT_BASE,
    ADD_IMPORT_DOC_BASE,
    REMOVE_IMPORT_DOC_BASE,
    ADD_IMPORT_NCL,
    REMOVE_IMPORT_NCL,
    ADD_NODE,
    REMOVE_NODE,
    ADD_INTERFACE,
    REMOVE_INTERFACE,
    ADD_LINK,
    REMOVE_LINK,
    SET_PROPERTY // 0X2D
} NCL_EDIT_COMMAND;

typedef enum {
    NCL_EVENT_START = 1,
    NCL_EVENT_STOP = 2,
    NCL_EVENT_ABORT =4,
    NCL_EVENT_PAUSE=8,
    NCL_EVENT_RESUME = 16,
    NCL_EVENT_ATTRIBUTION_SET = 32
} NCL_EVENT_TYPE;

// BRIDGE



typedef struct _APP_EXEC_INFO {
	UINT16 protocol_id;	// protocol_id in transport_protocol_descriptor
	union {
		// case 1 : via OC/DC  (0x0001 or 0x0004
		UINT8 ctag;
		// case 2 : via IP (0x0002) => concatenation of url strings : delimiter "|"
		// case 3 : via Interaction Channel (0x0003) => concatenation of url_base, url_exts with delimeter "|" and ";"
		//			ex) url_base_1|url_ext_11|url_ext_12|url_ext_13;url_base_2|url_ext_21|url_ext_22|url_ext_23
		SINT8 urls[AM_URL_MAX_LENGTH];
	};

	// application_location_descriptor
	SINT8 base_dir[AM_BASE_DIR_MAX_LENGTH];
	SINT8 entitypath_ext[AM_PATH_EXT_MAX_LENGTH];
	SINT8 init_entity[AM_INIT_ENTITY_MAX_LENGTH];

	// application_descriptor
	SINT8 params[AM_PARAM_MAX_LENGTH];	// concatenation of parameber strings : delimiter "|"

} APP_EXEC_INFO;

typedef struct _APP_NAME {
    SINT8 lang_code[4];
    SINT8 name[AM_NAME_MAX_LENGTH];
} APP_NAME;

typedef struct _APP_PROFILE {
    UINT16 application_profile;
    UINT8 major;
    UINT8 minor;
    UINT8 micro;
} APP_PROFILE;

typedef struct _APP_ATTR_INFO {
    UINT16 onid;
    UINT16 tsid;
    UINT16 sid;

    UINT32 org_id;
    UINT16 app_id;

    UINT16 app_type;

    UINT8 icon_flags;
    UINT8 icon_locator[AM_ICON_LOC_MAX_LENGTH];

    UINT8 app_name_num;
    APP_NAME app_names[AM_NAME_MAX_NUM];

    UINT8 app_profile_num;
    APP_PROFILE app_profiles[AM_PROFILE_MAX_NUM];

    UINT8 service_bound;
    UINT8 startable;
    UINT8 visibility;
    UINT8 priority;
} APP_ATTR_INFO;

typedef enum {
    APP_ACTIVE     = 0x0,
    APP_DESTROYED  = 0x1,
    APP_LOADED     = 0x2,
    APP_PAUSED     = 0x3,
    APP_NOT_LOADED = 0x4
} APP_STATE;

#ifndef MAX_URL_SIZE
#define MAX_URL_SIZE			1024
#endif

typedef enum {
    // 0x00 ~ 0x0F : Reserved for general or common functionality
    CB_TYPE_HANDLER_GINGA_SERVICE     = 0x00,    // Handler for GINGA
    CB_TYPE_HANDLER_PLAYLIST          = 0x00,    // Handler of MHP, OCAP, or ACAP
    CB_TYPE_HANDLER_SERVICE           = 0x00,    // Handler of BDJ. Same as CB_TYPE_HANDLER_SERVICE.
    CB_TYPE_HANDLER_IPTV_VOD          = 0x00,    // Handler of IPTV. Same as CB_TYPE_HANDLER_SERVICE.
    CB_TYPE_HANDLER_SERVICE_CONTEXT   = 0x01,    // ServiceContext
    CB_TYPE_HANDLER_TITLE_CONTEXT     = 0x01,    // TitleContext
    CB_TYPE_HANDLER_RATE              = 0x02,    // The rate of Handler
    CB_TYPE_AUDIO_CLIP_END            = 0x03,    // Audio Clip
    CB_TYPE_HANDLER_BB_VIDEO_CONTENT  = 0x04,    // Broadband Video Content
    CB_TYPE_HANDLER_BB_AUDIO_CONTENT  = 0x05,    // Broadband Audio Content

    // 0x10 ~ 0x1F : Reserved for JMF & JavaTV controls
    CB_TYPE_MEDIA_SELECT_CONTROL      = 0x10,    // javax.tv.mediaMediaSelectControl
    CB_TYPE_CACHING_CONTROL           = 0x11,    // java.media.CachingControl

    // 0x20 ~ 0x2F : Reserved for JavaDTV controls
    CB_TYPE_MEDIA_TIME_EVENT_CONTROL  = 0x20,    // com.sun.dtv.media.timeline.MediaTimePositionControl

    // 0x30 ~ 0x3F : Reserved for DVB controls
    CB_TYPE_SUBTITLING_EVENT_CONTROL  = 0x30,    // org.dvb.media.SubtitlingEventControl
    CB_TYPE_LANGUAGE_CONTROL          = 0x30,    //
    CB_TYPE_VIDEO_FORMAT_CONTROL      = 0x31,    // org.dvb.media.VideoFormatControl

    // 0x40 ~ 0x4F : Reserved for OCAP controls
    CB_TYPE_CLOSED_CAPTIONING_CONTROL = 0x40,    // org.ocap.media.ClosedCaptioningControl

    // 0x50 ~ 0x5F : Reserved for BDJ controls
    CB_TYPE_ANGLE_CONTROL             = 0x50,    // org.bluray.media.AngleControl
    CB_TYPE_PLAYBACK_CONTROL_MARK     = 0x51,    // org.bluray.media.PlaybackControl
    CB_TYPE_PLAYBACK_CONTROL_PLAYITEM = 0x52,    // org.bluray.media.PlaybackControl
    CB_TYPE_UOMASK_TABLE_CONTROL      = 0x53,    // org.bluray.media.UOMaskTableControl
    CB_TYPE_PIP_CONTROL               = 0x54,    // org.bluray.media.PiPControl

    // 0x60 ~ 0x6F : Reserved for BDJ controls of Stereoscopic
    CB_TYPE_SUBTITLEALIGN_CONTROL     = 0x60,    // org.blurayx.s3d.media.AlignedSubtitleControl
    CB_TYPE_OFFSETSEQUENCE_CONTROL    = 0x61,    // org.blurayx.s3d.media.GraphicsOffsetSequenceControl
} MOA_JMF_CALLBACK_SIGNAL;

typedef enum {
    SIGNAL_MEDIA_TIME_POSITION_CHANGED = 0x00, SIGNAL_MEDIA_TIME_NOTIFICATION = 0x01
} MOA_JMF_CALLBACK_CONTROL_SIGNAL;

/* =================================================================================================
 *  PG Guide 1.2 : Debug
 * ================================================================================================= */
typedef enum {
   MOA_MW_INFO_JVM_HEAP_INFO = 0,	/* MW VM Heap Info */
   MOA_MW_INFO_APP_STATUS    = 1	/* Xlet Status     */
} MOA_MW_INFO;

/* =================================================================================================
 *  PG Guide 1.4 : OS System Calls
 * ================================================================================================= */
typedef struct _XME_VM_OPTIONS {
    SINT16 mw_max_pool;
    SINT16 mw_thd_sched;
    SINT16 mw_thd_base_prio;
    SINT16 mw_thd_weight_prio;
    SINT16 app_max_pool;
    SINT16 app_thd_sched;
    SINT16 app_thd_base_prio;
    SINT16 app_thd_weight_prio;
    SINT16 jit_mode;
    SINT16 rsvd[1];
} XME_VM_OPTIONS;

/* =================================================================================================
 *  PG Guide 3.1 : Audio
 * ================================================================================================= */
#define NUM_OF_LANGUAGE_CODE	4
#define MAX_NUM_OF_LANGS				16

/* =================================================================================================
 *  PG Guide 3.2 : Video
 * ================================================================================================= */

typedef enum {
    MEDIA_STATUS = 0x00,
    RATE_CHANGE = 0x01,
    MEDIA_TIME_POSITION_CHANGE = 0x01 << 1,
    MEDIA_TIME_NOTIFICATION = 0x01 << 2,
    FREEZE_RESUME = 0x01 << 3,
    VIDEO_FORMAT_CHANGE = 0x01 << 4,
    AUDIO_LANGUAGE_CHANGE = 0x01 << 5,
    TEXT_LANGUAGE_CHANGE = 0x01 << 6
} LISTENER_TYPE;

typedef enum {
    MEDIA_WITH_SERVICE = 0,
    VIDEO_WITH_SBTVD_TS,
    VIDEO_WITH_MPEG,
    VIDEO_WITH_MNG,
    AUDIO_WITH_MP4,
    AUDIO_WITH_MPEG,
    AUDIO_WITH_SBTVD_TS
} MEDIA_TYPE;

typedef enum {
    AUDIO_LANGUAGE_CONTROL = 1,
    TEXT_LANGUAGE_CONTROL
} LANGUAGE_CONTROL_TYPE;

typedef enum {
	VIDEO_ACTIVE_FORMAT = 1,
	VIDEO_ASPECT_RATIO,
	VIDEO_DECORDER_FORAMT,
	VIDEO_DISPLAY_ASPECT_RATIO
} GINGA_VIDEO_FORMAT_TYPE;

typedef enum {
    OVERLAY_TEXT_AVAILABLE         = 11,
    OVERLAY_TEXT_DESELECTED        = 12,
    OVERLAY_TEXT_SELECTED          = 13,
    OVERLAY_TEXT_LANGUAGE_CHANGE   = 14,
    OVERLAY_TEXT_NOT_AVAILABLE     = 15,
    NOT_USED                       = 20
} GINGA_SUBTITLE_STATUS;


typedef enum {
    MEDIA_SELECT = 1,
    MEDIA_ADD,
    MEDIA_REMOVE,
    MEDIA_REPLACE
} MEDIA_SELECT_COMMAND;

#define MAX_SERVICE_NAME_LENGTH			40

typedef struct _MOA_SERVICE {
    UINT8 niid;
    UINT16 onid;
    UINT16 tsid;
    UINT16 sid;
    UINT8 service_type;
    UINT8 service_name[MAX_SERVICE_NAME_LENGTH];
    UINT8 service_name_length;
} MOA_SERVICE;

typedef struct _MOA_MEDIA {
    SINT32 handlerId;
    union {
        struct {
            UINT8 niid;
            UINT16 onid;
            UINT16 tsid;
            UINT16 sid;
            UINT8 service_status;
        };
        SINT32 newRate;
        struct {
            SINT8 newType;
            SINT32 newFormat;
        };
        UINT64 newMediaTime;
        UINT32 eventId;
        struct {
            LANGUAGE_CONTROL_TYPE languageControlType;
            GINGA_SUBTITLE_STATUS eventStatus;
        };
    };
} MOA_MEDIA;



typedef enum {
	WITH_DECODING,
	WITHOUT_DECODING
} VIDEO_IS_DECODING;



/* =================================================================================================
 *  PG Guide ? : Broadcast Module.
 * ================================================================================================= */
#ifndef EVENT_DATA_MAX_SIZE
#define EVENT_DATA_MAX_SIZE				512
#endif

#ifndef MAX_EVENT_NAME_LENGTH
#define MAX_EVENT_NAME_LENGTH			255
#endif

typedef struct _STREAM_EVENT_INFO {
    UINT64 event_id;
    SINT8 event_name[MAX_EVENT_NAME_LENGTH];
    UINT32 duration_sec;
    UINT16 duration_micro;
    UINT64 event_time;
    UINT8 privateDataByte[EVENT_DATA_MAX_SIZE];
} STREAM_EVENT_INFO;


/****************************************
 * PG 2.5 User Property
 ****************************************/
#define USER_PROPERTY_VALUE_MAX_LENGTH 512
#define USER_PROPERTY_KEY_MAX_LENGTH 256
#define USER_PROPERTY_MAX_COUNT 20

typedef enum {
	READ_PROPERTY,
	WRITE_PROPERTY,
	REMOVE_PROPERTY
} PROPERTY_COMMAND;

/* ================================================================
*  PG 2.6 Network Device
* ================================================================ */

typedef enum {
	CONNECT_SUCCESS = 0,
	CONNECT_FAIL = 1,
	DATA_RATE_CHANGE = 2,
	DISCONNECTED = 3
} NETWORK_DEVICE_STATUS;


/* ================================================================
*  PG X.Y Bridge
* ================================================================ */

typedef struct _CONTROL_XLET {
	SINT32	controlCode;
	UINT32	org_id;				// organization id
	UINT16	app_id;				// app id
	SINT8	initialClass[256];	// class name with null terminated
	SINT8	classPath[1024];	// class path that is absolute path with null terminated
	UINT32	x;					// bound of x
	UINT32	y;					// bound of y
	UINT32	w;					// bound of width
	UINT32	h;					// bound of height
} CONTROL_XLET;


/* ================================================================================================= */
#endif /* _MW_PORT_DATA_H_ */
