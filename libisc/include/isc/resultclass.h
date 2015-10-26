/*
 * Copyright (C) 2004-2007, 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_RESULTCLASS_H
#define GUARD_ISC_RESULTCLASS_H 1


/*! \file isc/resultclass.h
 * \brief Registry of Predefined Result Type Classes
 *
 * A result class number is an unsigned 16 bit number.  Each class may
 * contain up to 65536 results.  A result code is formed by adding the
 * result number within the class to the class number multiplied by 65536.
 *
 * Classes < 1024 are reserved for ISC use.
 * Result classes >= 1024 and <= 65535 are reserved for application use.
 */

#define ISC_RESULTCLASS_FROMNUM(num)		((num) << 16)
#define ISC_RESULTCLASS_TONUM(rclass)		((rclass) >> 16)
#define ISC_RESULTCLASS_SIZE			65536
#define ISC_RESULTCLASS_INCLASS(rclass, result) \
	((rclass) == ((result) & 0xFFFF0000))


#define	ISC_RESULTCLASS_ISC		ISC_RESULTCLASS_FROMNUM(0)
#define	ISC_RESULTCLASS_DNS		ISC_RESULTCLASS_FROMNUM(1)
#define	ISC_RESULTCLASS_DST		ISC_RESULTCLASS_FROMNUM(2)
#define	ISC_RESULTCLASS_DNSRCODE	ISC_RESULTCLASS_FROMNUM(3)
#define	ISC_RESULTCLASS_OMAPI		ISC_RESULTCLASS_FROMNUM(4)
#define	ISC_RESULTCLASS_ISCCC		ISC_RESULTCLASS_FROMNUM(5)
#define	ISC_RESULTCLASS_DHCP		ISC_RESULTCLASS_FROMNUM(6)


#endif /* GUARD_ISC_RESULTCLASS_H */
