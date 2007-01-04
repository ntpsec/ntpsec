/* GLOBAL.H - RSAREF types and constants */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */

/*
 * Note: the modifications are necessary for little-endian machines
 */
#include "ntp_types.h"			/* local modification */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_ 1

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef u_int32 UINT4;			/* local modification */

/* BYTE defines a unsigned character */
typedef unsigned char BYTE;		/* local modification for RSAEuro */

#ifndef NULL_PTR
#define NULL_PTR ((POINTER)0)
#endif

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) x = *(&x);
#endif

#endif /* end _GLOBAL_H_ */
