/*
 * Data for pretty printing clock types
 */
#include <config.h>
#include <stdio.h>

#include "ntp.h"
#include "ntp_refclock.h"

struct clktype clktypes[] = {
	{ REFCLK_NONE,		"unspecified type (0)",
	  "UNKNOWN" },
	{ REFCLK_LOCALCLOCK,	"Undisciplined local clock (1)",
	  "LOCAL" },
	{ REFCLK_GPS_TRAK,	"TRAK 8810 GPS Receiver (2)",
	  "GPS_TRAK" },
	{ REFCLK_NONE,		"not used (3)",
	  "NOT_USED" },
	{ REFCLK_SPECTRACOM,	"Spectracom (generic) Receivers (4)",
	  "SPECTRACOM" },
	{ REFCLK_TRUETIME,	"TrueTime (generic) Receivers (5)",
	  "TRUETIME" },
	{ REFCLK_IRIG_AUDIO,	"IRIG Audio Decoder (6)",
	  "IRIG_AUDIO" },
	{ REFCLK_CHU_AUDIO,	"CHU Audio Demodulator/Decoder (7)",
	  "CHU_AUDIO" },
	{ REFCLK_PARSE,		"Generic reference clock driver (8)",
	  "GENERIC" },
	{ REFCLK_GPS_MX4200,	"Magnavox MX4200 GPS Receiver (9)",
	  "GPS_MX4200" },
	{ REFCLK_GPS_AS2201,	"Austron 2201A GPS Receiver (10)",
	  "GPS_AS2201" },
	{ REFCLK_GPS_ARBITER,	"Arbiter 1088A/B GPS Receiver (11)",
	  "GPS_ARBITER" },
	{ REFCLK_NONE,		"not used (12)",
	  "NOT_USED" },
	{ REFCLK_NONE,		"not used (13)",
	  "NOT_USED" },
	{ REFCLK_NONE,		"not used (14)",
	  "NOT_USED" },
	{ REFCLK_NONE,		"not used (15)",
	  "NOT_USED" },
	{ REFCLK_NONE,		"not used (16)",
	  "NOT_USED" },
	{ REFCLK_NONE,		"not used (17)",
	  "NOT_USED" },
	{ REFCLK_ACTS,		"Automated Computer Time Service (18)",
	  "ACTS_MODEM" },
	{ REFCLK_NONE,		"not used (19)",
	  "NOT_USED" },
	{ REFCLK_GPS_NMEA,	"Generic NMEA GPS Receiver (20)",
	  "GPS_NMEA" },
	{ REFCLK_GPS_VME,	"TrueTime GPS-VME Interface (21)",
	  "GPS_VME" },
	{ REFCLK_ATOM_PPS,	"PPS Clock Discipline (22)",
	  "PPS" },
	{ REFCLK_NONE,		"not used (23)",
	  "NOT_USED" },
	{ REFCLK_NONE,		"not used (24)",
	  "NOT_USED" },
	{ REFCLK_NONE,		"not used (25)",
	  "NOT_USED" },
	{ REFCLK_GPS_HP,	"HP 58503A GPS Time & Frequency Receiver (26)",
	  "GPS_HP" },
	{ REFCLK_ARCRON_MSF,	"ARCRON MSF (and DCF77) Receiver (27)",
	  "MSF_ARCRON" },
	{ REFCLK_SHM,		"Clock attached thru shared Memory (28)",
	  "SHM" },
	{ REFCLK_PALISADE,	"Trimble Navigation Palisade GPS (29)",
	  "GPS_PALISADE" },
	{ REFCLK_ONCORE,	"Motorola UT Oncore GPS (30)",
	  "GPS_ONCORE" },
	{ REFCLK_GPS_JUPITER,	"Rockwell Jupiter GPS (31)",
	  "GPS_JUPITER" },
	{ REFCLK_NONE,		"not used (32)",
	  "NOT_USED" },
	{ REFCLK_DUMBCLOCK,	"Dumb generic hh:mm:ss local clock (33)",
	  "DUMBCLOCK" },
	{ REFCLK_NONE,		"not used (34)",
	  "NOT_USED" },
	{ REFCLK_PCF,		"Conrad parallel port radio clock (35)",
	  "PCF"},
	{ REFCLK_NONE,		"not used (36)",
	  "NOT_USED" },
	{ REFCLK_FG,		"Forum Graphic GPS Dating Station (37)",
	  "GPS_FG"},
	{ REFCLK_HOPF_SERIAL,	"hopf Elektronic serial line receiver (38)",
	  "HOPF_S"},
	{ REFCLK_HOPF_PCI,	"hopf Elektronic PCI receiver (39)",
	  "HOPF_P"},
	{ REFCLK_JJY,		"JJY receiver (40)",
	  "JJY"},
	{ REFCLK_TT560,		"TrueTime 560 IRIG-B decoder (41)",
	  "TT_IRIG"},
	{ REFCLK_ZYFER,		"Zyfer GPStarplus receiver (42)",
	  "GPS_ZYFER" },
	{ REFCLK_IRIG_TPRO,	"KSI/Odetics TPRO/S IRIG Interface (12)",
	  "IRIG_TPRO" },
	{ REFCLK_NONE,		"not used (43)",
	  "NOT_USED" },
	{ REFCLK_NEOCLOCK4X,	"NeoClock4X DCF77 / TDF receiver (44)",
	  "NEOCLK4X"},
        { REFCLK_TSYNCPCI,      "Spectracom TSYNC PCI timing board (45)",
          "PCI_TSYNC"},
	{ REFCLK_GPSDJSON,	"GPSD JSON socket (46)",
	  "GPSD_JSON"},
	{ -1,			"", "" }
};

const char *
clockname(
	int num
	)
{
	register struct clktype *clk;
  
	for (clk = clktypes; clk->code != -1; clk++) {
		if (num == clk->code)
			return (clk->abbrev);
	}
	return (NULL);
}
