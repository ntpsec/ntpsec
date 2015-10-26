/*
 * Copyright (C) 2004, 2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_BINDEVT_H
#define GUARD_ISC_BINDEVT_H 1

/*
 * This is used for the event log for both logging the messages and
 * later on by the event viewer when looking at the events
 */

/*
 * Values are 32 bit values layed out as follows:
 *
 *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  +---+-+-+-----------------------+-------------------------------+
 *  |Sev|C|R|     Facility          |               Code            |
 *  +---+-+-+-----------------------+-------------------------------+
 *
 *  where
 *
 *      Sev - is the severity code
 *
 *          00 - Success
 *          01 - Informational
 *          10 - Warning
 *          11 - Error
 *
 *      C - is the Customer code flag
 *
 *      R - is a reserved bit
 *
 *      Facility - is the facility code
 *
 *      Code - is the facility's status code
 *
 *
 * Define the facility codes
 */
 

/*
 * Define the severity codes
 */


/*
 * MessageId: BIND_ERR_MSG
 *
 * MessageText:
 *
 *  %1
 */
#define BIND_ERR_MSG		((DWORD)0xC0000001L)

/*
 * MessageId: BIND_WARN_MSG
 *
 * MessageText:
 *
 *  %1
 */
#define BIND_WARN_MSG		((DWORD)0x80000002L)

/*
 * MessageId: BIND_INFO_MSG
 *
 * MessageText:
 *
 *  %1
 */
#define BIND_INFO_MSG		((DWORD)0x40000003L)

#endif /* GUARD_ISC_BINDEVT_H */
