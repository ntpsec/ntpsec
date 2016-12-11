/*
 * vint64ops.c - operations on 'vint64' values
 *
 * Written by Juergen Perlinger (perlinger@ntp.org) for the NTP project.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: NTP
 * ----------------------------------------------------------------------
 * This is an attempt to get the vint64 calculations stuff centralised.
 */

#include <config.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "ntp_types.h"
#include "ntp_fp.h"
#include "vint64ops.h"

/* -------------------------------------------------------------------------*/

int
icmpv64(
	const vint64 * lhs,
	const vint64 * rhs
	)
{
	int res;

	res = (vint64s(*lhs) > vint64s(*rhs))
	    - (vint64s(*lhs) < vint64s(*rhs));

	return res;
}

/* -------------------------------------------------------------------------*/

int
ucmpv64(
	const vint64 * lhs,
	const vint64 * rhs
	)
{
	int res;
	
	res = (vint64u(*lhs) > vint64u(*rhs))
	    - (vint64u(*lhs) < vint64u(*rhs));

	return res;
}

/* -------------------------------------------------------------------------*/

vint64
addv64(
	const vint64 *lhs,
	const vint64 *rhs
	)
{
	vint64 res;

	setvint64u(res, vint64u(*lhs) + vint64u(*rhs));

	return res;
}

/* -------------------------------------------------------------------------*/

vint64
subv64(
	const vint64 *lhs,
	const vint64 *rhs
	)
{
	vint64 res;

	setvint64u(res, vint64u(*lhs) - vint64u(*rhs));

	return res;
}

/* -------------------------------------------------------------------------*/

vint64
addv64i32(
	const vint64 * lhs,
	int32_t        rhs
	)
{
	vint64 res;

	res = *lhs;
	setvint64u(res, vint64u(res) + rhs);

	return res;
}

/* -------------------------------------------------------------------------*/

vint64
subv64i32(
	const vint64 * lhs,
	int32_t        rhs
	)
{
	vint64 res;

	res = *lhs;
	setvint64s(res, vint64s(res) - rhs);

	return res;
}

/* -------------------------------------------------------------------------*/

vint64
addv64u32(
	const vint64 * lhs,
	uint32_t       rhs
	)
{
	vint64 res;

	res = *lhs;
	setvint64u(res, vint64u(res) + rhs);

	return res;
}

/* -------------------------------------------------------------------------*/

vint64
subv64u32(
	const vint64 * lhs,
	uint32_t       rhs
	)
{
	vint64 res;

	res = *lhs;
	setvint64u(res, vint64u(res) - rhs); 

	return res;
}
