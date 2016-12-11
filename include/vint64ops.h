/*
 * vint64ops.h - operations on 'vint64' values
 *
 * Written by Juergen Perlinger <perlinger@ntp.org> for the NTP project. 
 * ----------------------------------------------------------------------
 * This is an attempt to get the vint64 calculations stuff centralised.
 *
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: NTP
 */
#ifndef GUARD_VINT64OPS_H
#define GUARD_VINT64OPS_H

/* signed/unsigned compare. returns 1/0/-1 if lhs >/=/< rhs */
extern int icmpv64(const vint64 * lhs,	const vint64 * rhs);
extern int ucmpv64(const vint64 * lhs,	const vint64 * rhs);

/* add / subtract */
extern vint64 addv64(const vint64 *lhs, const vint64 *rhs);
extern vint64 addv64i32(const vint64 * lhs, int32_t rhs);
extern vint64 addv64u32(const vint64 * lhs, uint32_t rhs);

extern vint64 subv64(const vint64 *lhs, const vint64 *rhs);
extern vint64 subv64i32(const vint64 * lhs, int32_t rhs);
extern vint64 subv64u32(const vint64 * lhs, uint32_t rhs);

#endif /* GUARD_VINT64OPS_H */
