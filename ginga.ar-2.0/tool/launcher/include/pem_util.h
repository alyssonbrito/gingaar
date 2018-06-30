/*
 * pem_util.h
 *
 *  Created on: 21/08/2013
 *      Author: douglas.lima
 */

#ifndef PEM_UTIL_H_
#define PEM_UTIL_H_

#include <pem-tv-util/ginga_n_port_data.h>

#include "ginga_api.h"

#define PEM_THREAD_PRIORITY 49

#if defined(WEBOS_USE_GM)
typedef int GM_STATUS_T;
typedef signed int GM_WINDOW_ID_T;
#endif

bool initializeGraphic(int width, int height);
bool finalizeGraphic();

int startsWith(const char* msg, const char* prefix);
int makeAbsolutePath(SINT8 *, APP_EXEC_INFO *, char *);
bool both_slashes(char a, char b);
unsigned long getThreadId();
void setupSignals();

#endif /* PEM_UTIL_H_ */
