
#ifndef __BROWSER_AGENT_H__
#define __BROWSER_AGENT_H__

#include <unistd.h> // for usleep

#include <pem-tv-util/pem_log.h>
#include <pem-tv-util/pem_moa_common.h>
#include <pem-tv-util/pem_moa_msg.h>
#include <pem-tv-util/pem_port_osa.h>

#include "browser_port_tv2mw.h"

#include "browser_port_mw2tv.h"


#define PEM_MWAGT_BEGIN(fmt,...)	PEM_MWAGT_log(LOG_DEBUG, "%s : BEGIN [" fmt "]\n", __FUNCTION__, ##__VA_ARGS__)
#define PEM_MWAGT_END(fmt,...)		PEM_MWAGT_log(LOG_DEBUG, "%s : END [" fmt "]\n",   __FUNCTION__, ##__VA_ARGS__)
#define PEM_MWAGT_DEBUG(fmt,...)	PEM_MWAGT_log(LOG_DEBUG, "[%s::%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PEM_MWAGT_FATAL(fmt,...)	PEM_MWAGT_log(LOG_FATAL, "[%s::%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)


void BA_init(void);
SINT8 BA_initMsgEnv(void);
SINT8 BA_sendMsgMW2TV(void *sndMsg, UINT32 sndMsgSize);
SINT8 BA_sendReceiveMsgMW2TV(void *sndMsg, UINT32 sndMsgSize, void *rcvMsg, UINT32 rcvMsgSize);
SINT8 BA_receiveMsgTV2MW(void *rcvMsg, UINT32 rcvMsgSize);
void BA_start(void);
void BA_main(void);
SINT8 BA_finish(void);
SINT8 BA_isMAReady(void);

#endif /* __BROWSER_AGENT_H__ */
