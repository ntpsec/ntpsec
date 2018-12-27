/*
 * Our methods, one per linked module
 */
#ifndef GUARD_NTPFROB_H
#define GUARD_NTPFROB_H

#include <stdbool.h>      /* for bool */
#include <stdint.h>       /* for int64_t */
#include "ntp_fp.h"       /* for l_fp */

typedef enum {plain_text, raw, json} iomode;

extern void bumpclock(int64_t);
extern void get_clocktime(l_fp *now);
extern void do_dump(const iomode mode, const int force) __attribute__((noreturn));
extern void jitter(const iomode mode) __attribute__((noreturn));
extern void ppscheck(const char *device) __attribute__((noreturn));
extern void precision(const iomode mode);
extern void stepback(void);
extern void tickadj(const bool mode, const int tick);
extern void err(const char *legend) __attribute__((noreturn));

/*end */
#endif	/* GUARD_NTPFROB_H */
