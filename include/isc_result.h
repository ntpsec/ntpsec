/*
 * Copyright Internet Systems Consortium, Inc. ("ISC")
 * Copyright Internet Software Consortium.
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_RESULT_H
#define GUARD_ISC_RESULT_H 1

/* isc_result.h */

typedef unsigned int			isc_result_t;	/* Result */

#define ISC_R_SUCCESS			0	/* success */
#define ISC_R_NOMEMORY			1	/* out of memory */
#define ISC_R_NOTFOUND			23	/* not found */
#define ISC_R_FAILURE			25	/* generic failure */
#define ISC_R_NOTIMPLEMENTED		27	/* not implemented */
#define ISC_R_NOMORE			29	/* no more */
#define ISC_R_UNEXPECTED		34	/* unexpected error */
#define ISC_R_IGNORE			36	/* ignore */

#endif /* GUARD_ISC_RESULT_H */
