
#include "config.h"
#include "ntp_types.h"
#include "ntpd.h"
#include "trimble.h"


cmd_info_t trimble_rcmds[] = {
	{ CMD_RDATAA, "CMD_RDATAA", "data channel A configuration (0x3D)", "trimble_channelA", RO },
	{ CMD_RALMANAC, "CMD_RALMANAC", "almanac data for sat (0x40)", "gps_almanac", RO },
	{ CMD_RCURTIME, "CMD_RCURTIME", "GPS time (0x41)", "gps_time", RO },
	{ CMD_RSPOSXYZ, "CMD_RSPOSXYZ", "single precision XYZ position (0x42)", "gps_position(XYZ)", RO|DEF },
	{ CMD_RVELOXYZ, "CMD_RVELOXYZ", "velocity fix (XYZ ECEF) (0x43)", "gps_velocity(XYZ)", RO|DEF },
	{ CMD_RBEST4, "CMD_RBEST4", "best 4 satellite selection (0x44)", "trimble_best4", RO|DEF },
	{ CMD_RVERSION, "CMD_RVERSION", "software version (0x45)", "trimble_version", RO|DEF },
	{ CMD_RRECVHEALTH, "CMD_RRECVHEALTH", "receiver health (0x46)", "trimble_receiver_health", RO|DEF },
	{ CMD_RSIGNALLV, "CMD_RSIGNALLV", "signal levels of all satellites (0x47)", "trimble_signal_levels", RO },
	{ CMD_RMESSAGE, "CMD_RMESSAGE", "GPS system message (0x48)", "gps-message", RO|DEF },
	{ CMD_RALMAHEALTH, "CMD_RALMAHEALTH", "almanac health page for all satellites (0x49)", "gps_almanac_health", RO },
	{ CMD_RSLLAPOS, "CMD_RSLLAPOS", "single LLA position (0x4A)", "gps_position(LLA)", RO|DEF },
	{ CMD_RMACHSTAT, "CMD_RMACHSTAT", "machine code / status (0x4B)", "trimble_status", RO|DEF },
	{ CMD_ROPERPARAM, "CMD_ROPERPARAM", "operating parameters (0x4C)", "trimble_opparam", RO },
	{ CMD_ROSCOFFSET, "CMD_ROSCOFFSET", "oscillator offset (0x4D)", "trimble_oscoffset", RO },
	{ CMD_RSETGPSTIME, "CMD_RSETGPSTIME", "response to set GPS time (0x4E)", "trimble_setgpstime", RO },
	{ CMD_RUTCPARAM, "CMD_RUTCPARAM", "UTC parameters (0x4F)", "gps_utc_correction", RO|DEF },
	{ CMD_RANALOGDIG, "CMD_RANALOGDIG", "analog to digital (0x53)", "trimble_analogdigital", RO },
	{ CMD_RSAT1BIAS, "CMD_RSAT1BIAS", "one-satellite bias & bias rate (0x54)", "trimble_sat1bias", RO },
	{ CMD_RIOOPTIONS, "CMD_RIOOPTIONS", "I/O options (0x55)", "trimble_iooptions", RO },
	{ CMD_RSTATLSTFIX, "CMD_RSTATLSTFIX", "status and values of last pos. and vel. (0x57)", "trimble_status_lastpos", RO },
	{ CMD_RLOADSSATDT, "CMD_RLOADSSATDT", "response to load satellite system data (0x58)", "trimble_loaddata", RO },
	{ CMD_RSATDISABLE, "CMD_RSATDISABLE", "satellite disable (0x59)", "trimble_satdisble", RO },
	{ CMD_RLASTRAW, "CMD_RLASTRAW", "last raw measurement (0x5A)", "trimble_lastraw", RO },
	{ CMD_RSTATSATEPH, "CMD_RSTATSATEPH", "satellite ephemeris status (0x5B)", "trimble_ephstatus", RO },
	{ CMD_RSTATTRACK, "CMD_RSTATTRACK", "tracking status (0x5C)", "trimble_tracking_status", RO|DEF },
	{ CMD_RADDITFIX, "CMD_RADDITFIX", "additional fix data (0x5E)", "trimble_addfix", RO },
	{ CMD_RALLINVIEW, "CMD_RALLINVIEW", "all in view satellite selection (0x6D)", "trimble_satview", RO|DEF },
	{ CMD_RPOSFILT, "CMD_RPOSFILT", "position filter parameters (0x72)", "trimble_posfilt", RO },
	{ CMD_RHEIGHTFILT, "CMD_RHEIGHTFILT", "height filter control (0x74)", "trimble_heightfilt", RO },
	{ CMD_RHIGH8CNT, "CMD_RHIGH8CNT", "high-8 (best 4) / high-6 (overdetermined) control (0x76)", "trimble_high8control", RO },
	{ CMD_RMAXAGE, "CMD_RMAXAGE", "DC MaxAge (0x78)", "trimble_dgpsmaxage", RO },
	{ CMD_RDGPSFIX, "CMD_RDGPSFIX", "differential position fix mode (0x82)", "trimble_dgpsfixmode", RO },
	{ CMD_RDOUBLEXYZ, "CMD_RDOUBLEXYZ", "double precision XYZ (0x83)", "gps_position_ext(XYZ)", RO|DEF },
	{ CMD_RDOUBLELLA, "CMD_RDOUBLELLA", "double precision LLA (0x84)", "gps_position_ext(LLA)", RO|DEF },
	{ CMD_RDGPSSTAT, "CMD_RDGPSSTAT", "differential correction status (0x85)", "trimble_dgpsstatus", RO },
	{ CMD_RSUPER, "CMD_RSUPER", "super paket (0x8F)", "", 0 },
	{ 0xFF, "", "", "", 0 }
};
