/****************************************************************************
		DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
		Copyright(c) 2006 by LG Electronics Inc.

	All rights reserved. No part of this work may be reproduced,
	stored in a retrieval system, or transmitted by any means without
	prior written permission of LG Electronics Inc.


	FILE NAME   : pem_moa_common.h
	VERSION     : 1.0
	AUTHOR      : kihwa@lge.com
	DATE        : 2007.04.18
	DESCRIPTION : Common Data Type Definition
	HISTORY     :

******************************************************************************/

#ifndef _PEM_MOA_COMMON_H_
#define _PEM_MOA_COMMON_H_

#include <stdlib.h>

/*----------------------------------------------------------------------------
 *	type definitions
 *---------------------------------------------------------------------------*/
#ifndef SINT8
	typedef signed char __SINT8;
	#define SINT8 __SINT8
#endif

#ifndef UINT8
	typedef unsigned char __UINT8;
	#define UINT8 __UINT8
#endif

#ifndef SINT16
	typedef short __SINT16;
	#define SINT16 __SINT16
#endif

#ifndef UINT16
	typedef unsigned short __UINT16;
	#define UINT16 __UINT16
#endif

#ifndef SINT32
	typedef int __SINT32;
	#define SINT32 __SINT32
#endif

#ifndef UINT32
	typedef unsigned int __UINT32;
	#define UINT32 __UINT32
#endif

#ifndef SLONG
	typedef long __SLONG;
	#define SLONG __SLONG
#endif

#ifndef ULONG
	typedef unsigned long __ULONG;
	#define ULONG __ULONG
#endif

#ifdef OS20
	#ifndef SINT64
		typedef struct _SINT64
		{
			UINT32 l;
			SINT32 h;
		} __SINT64;
		#define SINT64 __SINT64
	#endif
	#ifndef UINT64
		typedef struct _UINT64
		{
			UINT32 l;
			UINT32 h;
		} __UINT64;
		#define UINT64 __UINT64
	#endif
#else
	#ifndef UINT64
		typedef unsigned long long  __UINT64;
		#define UINT64 __UINT64
	#endif
	#ifndef SINT64
		typedef signed long long  __SINT64;
		#define SINT64 __SINT64
	#endif
#endif

#ifndef SINT64_T
	typedef struct _SINT64_T
	{
		UINT32 l;
		SINT32 h;
	} __SINT64_T;
	#define SINT64_T __SINT64_T
#endif

#ifndef UINT64_T
	typedef struct _UINT64_T
	{
		UINT32 l;
		UINT32 h;
	} __UINT64_T;
	#define UINT64_T __UINT64_T
#endif

#ifndef BOOLEAN
	typedef	unsigned char __BOOLEAN;
	#define BOOLEAN __BOOLEAN
#endif

#ifndef NULL
	#define NULL		(0)
#endif

#ifndef FALSE
	#define FALSE		(0)
#endif
#ifndef TRUE
	#define TRUE		(1)
#endif

#ifndef SUCCESS
	#define SUCCESS		(0)
#endif
#ifndef FAILURE
	#define FAILURE		(-1)
#endif

#ifndef OK
	#define OK			(0)
#endif
#ifndef ERROR
	#define ERROR		(-1)
#endif

#ifndef ENABLE
	#define ENABLE		(1)
#endif
#ifndef DISABLE
	#define DISABLE		(0)
#endif



/*----------------------------------------------------------------------------
 *	Finite State Machine for Applications
 *---------------------------------------------------------------------------*/
#define STATE_APP_SUCCESS			0
#define STATE_APP_IS_RUNNING		1
#define STATE_APP_FAILURE			2




/*----------------------------------------------------------------------------
 *	MACRO_FUNCTION Define
 *---------------------------------------------------------------------------*/
//#define TV_printf(_fmt, _arg...) aprint0n(_fmt, ##_arg)
#define TV_printf printf


#endif /* _PEM_MOA_COMMON_H_ */

