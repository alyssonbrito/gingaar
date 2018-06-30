#ifndef _PORT_OSA_H_
#define _PORT_OSA_H_

#include "pem_moa_common.h"
#include "pem_moa_msg.h"
#include "pem_log.h"

#include "ginga_n_port_data.h"


#ifdef __cplusplus
extern "C" {
#endif


SINT8 *PEM_getErrorCode(SINT32 error);

SINT8 PEM_PORT_OSA_createTask(UINT8 *pName, void *pFunction, void *arg, UINT32 stack_size, UINT32 priority, UINT32 *pTID);


SINT8 PEM_PORT_OSA_createSemaphore(SINT8 *name, UINT32 sem_key, SINT32 *pSid);
SINT8 PEM_PORT_OSA_takeSemaphore(SINT32 sid);
SINT8 PEM_PORT_OSA_giveSemaphore(SINT32 sid);
SINT8 PEM_PORT_OSA_removeSemaphore(SINT32 sid);


SINT8 PEM_PORT_OSA_createMsgQueue(UINT8 *name, UINT32 queue_key, SINT32 *queue_id);
SINT8 PEM_PORT_OSA_sendMsg(SINT32 queue_id, void *pMessage, UINT32 length);
SINT8 PEM_PORT_OSA_receiveMsg(SINT32 queue_id, void *pMessage, UINT32 length);
SINT8 PEM_PORT_OSA_removeMsgQueue(SINT32 queue_id);


SINT32 PEM_PORT_OSA_getSHMKey(UINT8 data_type);
SINT8 PEM_PORT_OSA_writeSHM(UINT8 data_type, void *data, UINT32 size);
SINT8 PEM_PORT_OSA_readSHM(UINT8 data_type, void *data, UINT32 size);


SINT8 PEM_PORT_FS_fileExist(SINT8 * filePath);
SINT8 PEM_PORT_FS_makeDir(SINT8 * filePath);


#ifdef __cplusplus
}
#endif

#endif
