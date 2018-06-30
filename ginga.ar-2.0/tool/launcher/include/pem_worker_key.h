#ifndef __PEM_WORKER_KEY_H__
#define __PEM_WORKER_KEY_H__


#define NCL_NEW_THREAD_WORKER


#ifdef NCL_NEW_THREAD_WORKER
#include "thread.h"
#include "wqueue.h"

//#define MAX_QUEUE_KEY 4

#define NCL_INT_EVT_KEY 0
#define NCL_INT_EVT_MSG 1

#define NCL_INT_EVT_MSG_LOAD 10
#define NCL_INT_EVT_MSG_KILL 11

#define NCL_MAX_RETRY_EVENTS 5

#endif /* *NCL_NEW_THREAD_WORKER */



//Presentation Engine global variable
extern BrowserPresentationEngine bpe;



#ifdef NCL_NEW_THREAD_WORKER

int _pem_run(MSG_APP_EXEC_INFO *msg);



class WorkKey
{
private:
	unsigned int m_evtType;
	unsigned int m_key;
	unsigned int m_keyType;
	MSG_APP_EXEC_INFO m_msg;

public:
	WorkKey(unsigned int evtType, unsigned int key, unsigned int keyType) : m_evtType(evtType),
										m_key(key),
										m_keyType(keyType) {}

	WorkKey(unsigned int evtType, unsigned int key, unsigned int keyType, MSG_APP_EXEC_INFO msg) : m_evtType(evtType),
												       m_key(key),
												       m_keyType(keyType),
												       m_msg(msg) {}

	~WorkKey() {}

	unsigned int getevtType()	{ return m_evtType; }
	unsigned int getkey()		{ return m_key;     }
	unsigned int getkeyType()	{ return m_keyType; }
	MSG_APP_EXEC_INFO* getMsg()	{ return &m_msg;    }
};



class ConsumerThread : public Thread {
private:
	wqueue<WorkKey*>& m_queue;

public:
	ConsumerThread(wqueue<WorkKey*>& queue) : m_queue(queue) {}

	void* run() {
		int result = 0;
		int iRetry = 0;
		while (1) {

			usleep(150000); //sleep for 150ms

			// Remove 1 item at a time and process it. Blocks if no items are
			// available to process.
			//PEM_debug(LC_PRINT, "thread %lu, - waiting for item...\n", (long unsigned int)self());

			WorkKey* item = m_queue.remove();
			if (item == NULL) {
				continue;
			}

			PEM_LAUNCHER_log(LC_PRINT, "thread %lu, item: getevtType - %d, getkey - %d getkeyType - %d\n",
									   (long unsigned int) self(),
									   item->getevtType(),
									   item->getkey(),
									   item->getkeyType());

			switch (item->getevtType()) {
				case NCL_INT_EVT_KEY:
					PEM_LAUNCHER_log(LOG_WARN, "call bpe.putKey\n");
					result = bpe.putKey(item->getkey(), item->getkeyType());
					break;

				case NCL_INT_EVT_MSG:
					if (item->getkey() == NCL_INT_EVT_MSG_KILL) {
						PEM_LAUNCHER_log(LOG_WARN, "bpe.stop was called\n");
						result = bpe.stop();
						PEM_LAUNCHER_log(LOG_WARN, "bpe.stop was finished\n");
					}
					else if (item->getkey() == NCL_INT_EVT_MSG_LOAD) {
						PEM_LAUNCHER_log(LOG_WARN, "_pem_run was called\n");

						result = _pem_run(item->getMsg());

						if ((result == STATE_APP_FAILURE || result == STATE_APP_IS_RUNNING) && (iRetry < NCL_MAX_RETRY_EVENTS)) {
							iRetry++;
							PEM_LAUNCHER_log(LOG_WARN, "re-add the item...\n");
							m_queue.add(item);
							continue;
						} else {
							PEM_LAUNCHER_log(LOG_WARN, "_pem_run was finished\n");
							iRetry = 0;
						}
					}
					break;
				default:
					PEM_LAUNCHER_log(LOG_ERROR, "Unknown Event Type\n");
			}

			delete item;
		}

		PEM_LAUNCHER_log(LOG_ERROR, "Who killed this thread?\n");
		return NULL;
	}
};




extern wqueue<WorkKey*>  queue;

extern ConsumerThread* KeyMain;

#endif /* *NCL_NEW_THREAD_WORKER */



#endif /* __PEM_WORKER_KEY_H__ */
