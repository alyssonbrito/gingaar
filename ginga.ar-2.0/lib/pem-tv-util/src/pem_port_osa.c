#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#include "pem_moa_common.h"
#include "pem_moa_msg.h"

#include "pem_port_osa.h"


#define PEM_OSA_FATAL_WITH_ERROR(fmt,errno)		PEM_OSA_log( LOG_FATAL, "[%s::%d] " fmt ". Error: %s (%d)\n", __FUNCTION__, __LINE__, PEM_getErrorCode((errno)), errno)
#define PEM_OSA_DEBUG(fmt,...)					PEM_OSA_log( LOG_DEBUG, "[%s::%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)



SINT8 *PEM_getErrorCode ( SINT32 error ) {
	switch ( error ) {
		case EACCES:		return "EACCES";
		case EEXIST:		return "EEXIST";
		case EINVAL:		return "EINVAL";
		case ENOENT:		return "ENOENT";
		case ENOMEM:		return "ENOMEM";
		case ENOSPC:		return "ENOSPC";
		case E2BIG:			return "E2BIG";
		case EAGAIN:		return "EAGAIN";
		case EFAULT:		return "EFAULT";
		case EFBIG:			return "EFBIG";
		case EIDRM:			return "EIDRM";
		case EINTR:			return "EINTR";
		case ERANGE:		return "ERANGE";
		case ENOMSG:		return "ENOMSG";
		case ENFILE:		return "ENFILE";
		case EPERM:			return "EPERM";
		default:			return "Ukown Error";
	}
}

SINT8 PEM_PORT_OSA_createTask ( UINT8 *pName, void *pFunction, void *arg, UINT32 stack_size, UINT32 priority, UINT32 *pTID ) {
	pthread_t pthreadID;
	struct sched_param param;
	pthread_attr_t attr;
	int result;

	pthread_attr_init ( &attr );
	if ( stack_size != 0 ) {
		result = pthread_attr_setstacksize ( &attr, stack_size );
		if ( result != 0 ) {
			errno = result;
			PEM_OSA_FATAL_WITH_ERROR( "pthread_attr_setstacksize failed", errno );
			return -1;
		}
	}
	pthread_attr_getschedparam ( &attr, &param );
	param.sched_priority = priority;
	pthread_attr_setschedparam ( &attr, &param );

	/* Don't leave thread/stack around after exit for join */
	pthread_attr_setdetachstate ( &attr, PTHREAD_CREATE_DETACHED );

	result = pthread_create ( &pthreadID, &attr, pFunction, NULL );
	pthread_attr_destroy ( &attr );

	if ( result != 0 ) {
		errno = result;
		PEM_OSA_FATAL_WITH_ERROR( "pthread_create failed", errno );
		return FAILURE;
	}
	else {
		*pTID = ( UINT32 ) pthreadID;
		PEM_OSA_DEBUG( "thread created : name = [%s]", pName );
		return SUCCESS;
	}
}

SINT8 PEM_PORT_OSA_createSemaphore ( SINT8 *name, UINT32 sem_key, SINT32 *pSid ) {
	int sid;
	short init [ 1 ] = { 1 };

	PEM_OSA_DEBUG ("name = %s, sem_key = %d, sid = %d", name, sem_key, sid );

	if ( ( sid = semget ( ( key_t ) sem_key, 1, IPC_CREAT | 0777 ) ) == -1 ) {
		PEM_OSA_FATAL_WITH_ERROR("semget failed!", errno);
		return FAILURE;
	}

	semctl ( sid, 1, SETALL, init );
	*pSid = ( SINT32 ) sid;
	return SUCCESS;
}

SINT8 PEM_PORT_OSA_takeSemaphore ( SINT32 sid ) {
	struct sembuf sem_b;

	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;

	PEM_OSA_DEBUG ("sid = %d", sid );

	if ( semop ( sid, &sem_b, 1 ) == -1 ) {
		PEM_OSA_FATAL_WITH_ERROR("semop failed!", errno);
		return FAILURE;
	}

	return SUCCESS;
}

SINT8 PEM_PORT_OSA_giveSemaphore ( SINT32 sid ) {
	struct sembuf sem_b;

	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;

	PEM_OSA_DEBUG ("sid = %d", sid );

	if ( semop ( sid, &sem_b, 1 ) == -1 ) {
		PEM_OSA_FATAL_WITH_ERROR("semop failed!", errno);
		return FAILURE;
	}

	return SUCCESS;
}

SINT8 PEM_PORT_OSA_removeSemaphore ( SINT32 sid ) {

	PEM_OSA_DEBUG ("sid = %d", sid );

	if ( semctl ( sid, 0, IPC_RMID ) == -1 ) {
		PEM_OSA_FATAL_WITH_ERROR("semctl failed!", errno);
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * create new MessageQueue or return MessageQueue if already exists
 *
 * @param queue_id
 * @param queue_key
 *
 */
SINT8 PEM_PORT_OSA_createMsgQueue ( UINT8 *name, UINT32 queue_key, SINT32 *queue_id ) {
	int qid = 0;

	PEM_OSA_DEBUG("name = %s, queue_key = %d\n", qid);

	if ( ( qid = msgget ( ( key_t ) queue_key, 0666 | IPC_CREAT ) ) == -1 ) {
		PEM_OSA_FATAL_WITH_ERROR("msgget failed!", errno);
		return FAILURE;
	}

	PEM_OSA_DEBUG("queue_id = %d\n", qid);

	*queue_id = ( SINT32 ) qid;
	return SUCCESS;
}

/**
 * send Message
 *
 * @param queue_id
 * @param pMessage
 * @param length
 */
SINT8 PEM_PORT_OSA_sendMsg ( SINT32 queue_id, void *pMessage, UINT32 length ) {

	PEM_OSA_DEBUG("queue_id = %d, message length = %d, max message length = %d", queue_id, length, MSG_MAX_LENGTH);

	if ( length > MSG_MAX_LENGTH ) {
		PEM_OSA_DEBUG("message length is bigger than MAX_LENGTH!");
		return FAILURE;
	}

	if ( msgsnd ( queue_id, pMessage, length - sizeof(SLONG), 0 /*msgflg*/) == -1 ) {
		PEM_OSA_FATAL_WITH_ERROR("msgsnd failed!", errno);
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * receive Message
 *
 * @param queue_id
 * @param pMessage
 * @param length
 */
SINT8 PEM_PORT_OSA_receiveMsg ( SINT32 queue_id, void *pMessage, UINT32 length ) {
	ssize_t bytesRead = 0;

	PEM_OSA_DEBUG("queue_id = %d, message length = %d, max message length = %d", queue_id, length, MSG_MAX_LENGTH);

	if ( length > MSG_MAX_LENGTH ) {
		PEM_OSA_DEBUG("message length is bigger than MAX_LENGTH!");
		return FAILURE;
	}

	if ( ( bytesRead = msgrcv ( queue_id, pMessage, length - sizeof(SLONG), 0 /*msgtyp*/, 0 /*msgflg*/) ) < 0 ) {
		PEM_OSA_FATAL_WITH_ERROR("msgrcv failed!", errno);
		return FAILURE;
	}

	PEM_OSA_DEBUG("queue_id = %d, received message length = %d", queue_id, bytesRead);
	return SUCCESS;
}

/**
 * remove Queue
 *
 * @param queue_id
 */
SINT8 PEM_PORT_OSA_removeMsgQueue ( SINT32 queue_id ) {
	if ( msgctl ( queue_id, IPC_RMID, 0 ) == -1 ) {
		PEM_OSA_FATAL_WITH_ERROR("msgctl failed!", errno);
		return FAILURE;
	}
	return SUCCESS;
}

/**
 *
 * @return shm_key to be used for specified data_type
 */
SINT32 PEM_PORT_OSA_getSHMKey ( UINT8 data_type ) {
	const SINT32 master_key = 0x7000;
	return ( master_key | data_type );
}

SINT8 PEM_PORT_OSA_writeSHM ( UINT8 data_type, void *data, UINT32 size ) {
	SINT32 shm_key;
	SINT32 shm_segment_id;
	void* shared_memory;

	shm_key = PEM_PORT_OSA_getSHMKey ( data_type );

	PEM_OSA_DEBUG("data_type = %d", data_type);

	if ( ( shm_segment_id = shmget ( shm_key, size, IPC_CREAT | S_IRUSR | S_IWUSR ) ) == -1 ) {
		PEM_OSA_FATAL_WITH_ERROR("shmget failed!", errno);
		return FAILURE;
	}

	// attach shared memory to process
	if ( ( shared_memory = shmat ( shm_segment_id, 0 /* shmaddr */, 0 /* shmflg */) ) == ( void * ) -1 ) {
		PEM_OSA_FATAL_WITH_ERROR("shmat failed!", errno);
		return FAILURE;
	}

	// write data to shared memory
	memcpy ( shared_memory, data, size );

	// detach shared memory from process
	shmdt ( shared_memory );

	return SUCCESS;
}

/**
 * memory allocation of *data should be done outside of PORT_OSA_readSHM
 */
SINT8 PEM_PORT_OSA_readSHM ( UINT8 data_type, void *data, UINT32 size ) {
	SINT32 shm_key;
	SINT32 shm_segment_id;
	void* shared_memory;

	if ( size <= 0 ) {
		data = NULL;
		return SUCCESS;
	}

	shm_key = PEM_PORT_OSA_getSHMKey ( data_type );

	PEM_OSA_DEBUG("data_type = %d", data_type);

	if ( ( shm_segment_id = shmget ( shm_key, size, S_IRUSR | S_IWUSR ) ) == -1 ) {
		PEM_OSA_FATAL_WITH_ERROR("shmget failed!", errno);
		return FAILURE;
	}

	// attach shared memory to process
	if ( ( shared_memory = shmat ( shm_segment_id, 0 /* shmaddr */, 0 /* shmflg */) ) == ( void * ) -1 ) {
		PEM_OSA_FATAL_WITH_ERROR("shmget failed!", errno);
		return FAILURE;
	}

	//  memory allocation of *data should be done outside of PORT_OSA_readSHM
	//if ((*data = (void *) PORT_MALLOC(size)) == NULL) return FAILURE;

	// read data from shared memory
	memcpy ( data, shared_memory, size );

	// detach shared memory from process
	shmdt ( shared_memory );

	// remove shared_memory
	shmctl ( shm_segment_id, IPC_RMID, 0 );

	return SUCCESS;
}

SINT8 PEM_PORT_FS_fileExist ( SINT8 *filePath ) {
	struct stat statbuf;
	return ( ( stat ( filePath, &statbuf ) == 0 ) ? SUCCESS : FAILURE );
}

SINT8 PEM_PORT_FS_makeDir ( SINT8 * filePath ) {
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	return ( ( mkdir ( filePath, mode ) == 0 ) ? SUCCESS : FAILURE );
}
