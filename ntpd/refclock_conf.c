/*
 * refclock_conf.c - reference clock configuration
 *
 * This is the only place in the code that knows how to map driver numeric types
 * to driver method tables and their attributes.
 */
#include "config.h"

#include <stdio.h>
#include <sys/types.h>

#include "ntpd.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"

struct refclock refclock_none = {
	NULL,		/* basename of driver */
	NULL,		/* start up driver */
	NULL,		/* shut down driver */
	NULL,		/* transmit poll message */
	NULL,		/* not used (old hpgps_control) */
	NULL,		/* initialize driver */
	NULL,		/* timer - not used */
};

/*
 * This is the only place in the code that knows about the mapping between
 * old-style numeric driver types and the drivers.
 *
 * The symbols in the comments are no longer defined. If you compact or
 * reorder this table, legacy ntpq instances that think they can
 * deduce driver types from address fields will get them wrong.
 * Also the addresses generated into statistics files won't match
 * what people expect - the type in the third octet will be off.
 */
struct refclock * const refclock_conf[] = {
	&refclock_none,		/* 0 REFCLK_NONE */
	&refclock_local,	/* 1 REFCLK_LOCAL */
	&refclock_none,		/* 2 was: REFCLK_GPS_TRAK */
	&refclock_none,		/* 3 was: REFCLK_WWV_PST */
	&refclock_spectracom, 	/* 4 REFCLK_SPECTRACOM */
	&refclock_true,		/* 5 REFCLK_TRUETIME */
	&refclock_none,		/* 6 was: REFCLK_IRIG_AUDIO */
	&refclock_none,		/* 7 was: REFCLK_CHU_AUDIO */
	&refclock_parse,	/* 8 REFCLK_GENERIC */
	&refclock_none,		/* 9 was: REFCLK_GPS_MX4200 */
	&refclock_none,		/* 10 was: REFCLK_GPS_AS2201 */
	&refclock_arbiter,	/* 11 REFCLK_GPS_ARBITER */
	&refclock_none,		/* 12 was: REFCLK_IRIG_TPRO */
	&refclock_none,		/* 13 was: REFCLK_ATOM_LEITCH */
	&refclock_none,		/* 14 was: REFCLOCK_MSF_EES */
	&refclock_none,		/* 15 was: OLD TrueTime GPS/TM-TMD Receiver */
	&refclock_none,		/* 16 was: REFCLK_IRIG_BANCOMM */
	&refclock_none,		/* 17 was: REFCLK_GPS_DATUM */
	&refclock_modem,	/* 18 REFCLK_ACTS */
	&refclock_none,		/* 19 was: REFCLK_WWV_HEATH */
	&refclock_nmea,		/* 20 REFCLK_GPS_NMEA */
	&refclock_none,		/* 21 was: REFCLK_GPS_VME */
	&refclock_pps,		/* 22 REFCLK_ATOM_PPS */
	&refclock_none,		/* 23 not used */
	&refclock_none,		/* 24 not used */
	&refclock_none,		/* 25 not used */
	&refclock_hpgps,	/* 26 REFCLK_GPS_HP */
	&refclock_none, 	/* 27 was: REFCLK_ARCRON_MSF */
	&refclock_shm,		/* 28 REFCLK_SHM */
	&refclock_trimble,	/* 29 REFCLK_PALISADE */
	&refclock_oncore,	/* 30 REFCLK_ONCORE */
	&refclock_none,		/* 31 was: REFCLK_GPS_JUPITER */
	&refclock_none,		/* 32 was: REFCLK_CHRONOLOG */
	&refclock_none,		/* 33 was: REFCLK_DUMBCLOCK */
	&refclock_none,		/* 34 was: REFCLOCK_ULINK */
	&refclock_none,		/* 35 was: REFCLOCK_PCF */
	&refclock_none,		/* 36 was: REFCLOCK_WWV_AUDIO */
	&refclock_none,		/* 37 was: REFCLOCK_FG */
	&refclock_none,		/* 38 was: REFCLK_HOPF_SERIAL */
	&refclock_none,		/* 39 was: REFCLK_HOPF_PCI */
	&refclock_jjy,		/* 40 REFCLK_JJY */
	&refclock_none,		/* 41 was: REFCLK_TT560 */
	&refclock_zyfer,	/* 42 REFCLK_ZYFER */
	&refclock_none,		/* 43 was: REFCLK_RIPENCC */
	&refclock_neoclock4x,	/* 44 REFCLK_NEOCLOCK4X */
	&refclock_none, 	/* 45 was: REFCLK_TSYNCPCI */
	&refclock_gpsdjson	/* 46 REFCLK_GPSDJSON */
};

const uint8_t num_refclock_conf = sizeof(refclock_conf)/sizeof(struct refclock *);
