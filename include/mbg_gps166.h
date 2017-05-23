/*
 * Created: Sun Jul 20 09:20:50 1997
 *
 * File GPSSERIO.H Copyright (c) by Meinberg Funkuhren (www.meinberg.de)
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-clause
 *
 * Linkage to PARSE:
 * Copyright (c) 1997-2005 by Frank Kardel <kardel <AT> ntp.org>
 */
#ifndef GUARD_MBG_GPS166_H
#define GUARD_MBG_GPS166_H


/***************************************************************************
 *
 *  Definitions taken from Meinberg's gpsserio.h and gpsdefs.h files.
 *
 *  Author:  Martin Burnicki, Meinberg Funkuhren
 *
 *  Copyright (c) Meinberg Funkuhren, Bad Pyrmont, Germany
 *
 *  Description:
 *    Structures and codes to be used to access Meinberg GPS clocks via
 *    their serial interface COM0. COM0 should be set to a high baud rate,
 *    default is 19200.
 *
 *    Standard Meinberg GPS serial operation is to send the Meinberg
 *    standard time string automatically once per second, once per
 *    minute, or on request per ASCII '?'.
 *
 *    GPS parameter setup or parameter readout uses blocks of binary
 *    data which have to be isolated from the standard string. A block
 *    of data starts with a SOH code (ASCII Start Of Header, 0x01)
 *    followed by a message header with constant length and a block of
 *    data with variable length.
 *
 *    The first field (cmd) of the message header holds the command
 *    code resp. the type of data to be transmitted. The next field (len)
 *    gives the number of data bytes that follow the header. This number
 *    ranges from 0 to sizeof( MSG_DATA ). The third field (data_csum)
 *    holds a checksum of all data bytes and the last field of the header
 *    finally holds the checksum of the header itself.
 *
 ***************************************************************************/

/**
 * @brief GPS epoch bias from ordinary time_t epoch
 *
 * The Unix time_t epoch is usually 1970-01-01 00:00 whereas
 * the GPS epoch is 1980-01-06 00:00, so the difference is 10 years,
 * plus 2 days due to leap years (1972 and 1976), plus the difference
 * of the day-of-month (6 - 1), so:<br>
 *
 * time_t t = ( gps_week * ::SECS_PER_WEEK ) + sec_of_week + ::GPS_SEC_BIAS
 */
#define GPS_SEC_BIAS   315964800UL     // ( ( ( 10UL * 365UL ) + 2 + 5 ) * SECS_PER_DAY )

/**
 * @brief The type of a GPS command code
 *
 * @see ::GPS_CMD_CODES
 */
typedef uint16_t GPS_CMD;

/**
 * @brief Command codes for the binary protocol
 *
 * These codes specify commands and associated data types used by Meinberg's
 * binary protocol to exchange data with a device via serial port, direct USB,
 * or socket I/O.
 *
 * Some commands and associated data structures can be read (r) from a device, others
 * can be written (w) to the device, and some can also be sent automatically (a) by
 * a device after a ::GPS_AUTO_ON command has been sent to the device.
 * The individual command codes are marked with (rwa) accordingly, where '-' is used
 * to indicate that a particular mode is not supported.
 *
 * @note Not all command code are supported by all devices.
 * See the hints for a particular command.
 *
 * @note If ::GPS_ALM, ::GPS_EPH or a code named ..._IDX is sent to retrieve
 * some data from a device then an uint16_t parameter must be also supplied
 * in order to specify the index number of the data set to be returned.
 * The valid index range depends on the command code.
 * For ::GPS_ALM and ::GPS_EPH the index is the SV number which may be 0 or
 * ::MIN_SVNO_GPS to ::MAX_SVNO_GPS. If the number is 0 then all ::N_SVNO_GPS
 * almanacs or ephemeris data structures are returned.
 *
 * @see ::GPS_CMD_CODES_TABLE
 */
enum GPS_CMD_CODES
{ /* system data */
  GPS_AUTO_ON = 0x000,  ///< (-w-) no data, enable auto-msgs from device
  GPS_AUTO_OFF,         ///< (-w-) no data, disable auto-msgs from device
  GPS_SW_REV,           ///< (r--) deprecated, ::SW_REV, software revision, use only if ::GPS_RECEIVER_INFO not supp.
  GPS_BVAR_STAT,        ///< (r--) ::BVAR_STAT, status of buffered variables, only if ::GPS_MODEL_HAS_BVAR_STAT
  GPS_TIME,             ///< (-wa) ::TTM, current time or capture, or init board time
  GPS_POS_XYZ,          ///< (rw-) ::XYZ, current position in ECEF coordinates, only if ::GPS_MODEL_HAS_POS_XYZ
  GPS_POS_LLA,          ///< (rw-) ::LLA, current position in geographic coordinates, only if ::GPS_MODEL_HAS_POS_LLA
  GPS_TZDL,             ///< (rw-) ::TZDL, time zone / daylight saving, only if ::GPS_MODEL_HAS_TZDL
  GPS_PORT_PARM,        ///< (rw-) deprecated, ::PORT_PARM, use ::PORT_SETTINGS etc. if ::GPS_RECEIVER_INFO supported
  GPS_SYNTH,            ///< (rw-) ::SYNTH, synthesizer settings, only if ::GPS_HAS_SYNTH
  GPS_ANT_INFO,         ///< (r-a) ::ANT_INFO, time diff after antenna disconnect, only if ::GPS_MODEL_HAS_ANT_INFO
  GPS_UCAP,             ///< (r-a) ::TTM, user capture events, only if ::RECEIVER_INFO::n_ucaps > 0

  /* GPS data */
  GPS_CFGH = 0x100,     ///< (rw-) ::CFGH, SVs' configuration and health codes
  GPS_ALM,              ///< (rw-) req: uint16_t SV num, ::SV_ALM, one SV's almanac
  GPS_EPH,              ///< (rw-) req: uint16_t SV num, ::SV_EPH, one SV's ephemeris
  GPS_UTC,              ///< (rw-) ::UTC, GPS %UTC correction parameters
  GPS_IONO,             ///< (rw-) ::IONO, GPS ionospheric correction parameters
  GPS_ASCII_MSG         ///< (r--) ::ASCII_MSG, the GPS ASCII message
};


/* checksum used by some structures stored in non-volatile memory */
typedef uint16_t CSUM;

/**
 * @brief The header of a binary message.
 */
typedef struct
{
  GPS_CMD cmd;      ///< see ::GPS_CMD_CODES
  uint16_t len;     ///< length of the data portion appended after the header
  CSUM data_csum;   ///< checksum of the data portion appended after the header
  CSUM hdr_csum;    ///< checksum of the preceding header bytes

} GPS_MSG_HDR;


#define GPS_ID_STR_LEN      16
#define GPS_ID_STR_SIZE     ( GPS_ID_STR_LEN + 1 )

/**
 * @brief Software revision information
 *
 * Contains a software revision code, plus an optional
 * identifier for a customized version.
 */
typedef struct
{
  uint16_t code;               ///< Version number, e.g. 0x0120 means v1.20
  char name[GPS_ID_STR_SIZE];  ///< Optional string identifying a customized version
  uint8_t reserved;            ///< Reserved field to yield even structure size

} SW_REV;


/**
 * @brief GNSS satellite numbers
 *
 * @todo: Check if MAX_SVNO_GLN is 94 instead of 95, and thus
 *        N_SVNO_GLN is 30 instead of 31, as reported by Wikipedia.
 */
enum GNSS_SVNOS
{
  MIN_SVNO_GPS = 1,       ///< min. GPS satellite PRN number
  MAX_SVNO_GPS = 32,      ///< max. GPS satellite PRN number
  N_SVNO_GPS = 32,        ///< max. number of active GPS satellites

  MIN_SVNO_WAAS = 33,     ///< min. WAAS satellite number
  MAX_SVNO_WAAS = 64,     ///< max. WAAS satellite number
  N_SVNO_WAAS = 32,       ///< max. number of active WAAS satellites

  MIN_SVNO_GLONASS = 65,  ///< min. Glonass satellite number (64 + sat slot ID)
  MAX_SVNO_GLONASS = 95,  ///< max. Glonass satellite number (64 + sat slot ID)
  N_SVNO_GLONASS = 31     ///< max. number of active Glonass satellites
};


typedef uint16_t SVNO;    ///< the number of an SV (Space Vehicle, i.e. satellite)
typedef uint16_t HEALTH;  ///< an SV's 6 bit health code
typedef uint16_t CFG;     ///< an SV's 4 bit configuration code
typedef uint16_t IOD;     ///< Issue-Of-Data code


/**
 * @brief Status flags of battery buffered data
 *
 * Related to data received from the satellites, or data derived thereof.
 *
 * All '0' means OK, single bits set to '1' indicate
 * the associated type of GPS data is not available.
 *
 * @see ::BVAR_FLAGS
 */
typedef uint16_t BVAR_STAT;

/**
 * @brief Enumeration of flag bits used to define ::BVAR_FLAGS
 *
 * For each bit which is set this means the associated data set in
 * non-volatile memory is not available, or incomplete.
 * Most data sets will just be re-collected from the data streams sent
 * by the satellites. However, the receiver position has usually been
 * computed earlier during normal operation, and will be re-computed
 * when a sufficient number of satellites can be received.
 *
 * @see ::BVAR_STAT
 * @see ::BVAR_FLAGS
 * @see ::BVAR_FLAG_NAMES
 */
enum BVAR_FLAG_BITS
{
  BVAR_BIT_CFGH_INVALID,      ///< Satellite configuration and health parameters incomplete
  BVAR_BIT_ALM_NOT_COMPLETE,  ///< Almanac parameters incomplete
  BVAR_BIT_UTC_INVALID,       ///< %UTC offset parameters incomplete
  BVAR_BIT_IONO_INVALID,      ///< Ionospheric correction parameters incomplete
  BVAR_BIT_RCVR_POS_INVALID,  ///< No valid receiver position available
  N_BVAR_BIT                  ///< number of defined ::BVAR_STAT bits
};


/**
 * @brief Bit masks associated with ::BVAR_FLAG_BITS
 *
 * Used with ::BVAR_STAT.
 *
 * @see ::BVAR_STAT
 * @see ::BVAR_FLAG_BITS
 * @see ::BVAR_FLAG_NAMES
 */
enum BVAR_FLAGS
{
  BVAR_CFGH_INVALID     = ( 1UL << BVAR_BIT_CFGH_INVALID ),      ///< see ::BVAR_BIT_CFGH_INVALID
  BVAR_ALM_NOT_COMPLETE = ( 1UL << BVAR_BIT_ALM_NOT_COMPLETE ),  ///< see ::BVAR_BIT_ALM_NOT_COMPLETE
  BVAR_UTC_INVALID      = ( 1UL << BVAR_BIT_UTC_INVALID ),       ///< see ::BVAR_BIT_UTC_INVALID
  BVAR_IONO_INVALID     = ( 1UL << BVAR_BIT_IONO_INVALID ),      ///< see ::BVAR_BIT_IONO_INVALID
  BVAR_RCVR_POS_INVALID = ( 1UL << BVAR_BIT_RCVR_POS_INVALID ),  ///< see ::BVAR_BIT_RCVR_POS_INVALID
};


/**
 * @brief A structure used to hold time in GPS format
 *
 * Date and time refer to the linear time scale defined by GPS, with
 * the epoch starting at %UTC midnight at the beginning of January 6, 1980.
 *
 * GPS time is counted by the week numbers since the epoch, plus second
 * of the week, plus fraction of the second. The week number transmitted
 * by the satellites rolls over from 1023 to 0, but Meinberg devices
 * just continue to count the weeks beyond the 1024 week limit to keep
 * the receiver's internal time.
 *
 * %UTC time differs from GPS time since a number of leap seconds have
 * been inserted in the %UTC time scale after the GPS epoche. The number
 * of leap seconds is disseminated by the satellites using the ::UTC
 * parameter set, which also provides info on pending leap seconds.
 */
typedef struct
{
  uint16_t wn;     ///< the week number since GPS has been installed
  uint32_t sec;    ///< the second of that week
  uint32_t tick;   ///< fractions of a second, 1/::RECEIVER_INFO::ticks_per_sec units

} T_GPS;


/**
 * @brief Local date and time computed from GPS time
 *
 * The current number of leap seconds have to be added to get %UTC
 * from GPS time. Additional corrections could have been made according
 * to the time zone/daylight saving parameters ::TZDL defined by the user.
 * The status field can be checked to see which corrections
 * have actually been applied.
 *
 * @note Conversion from GPS time to %UTC and/or local time can only be
 * done if some valid ::UTC correction parameters are available in the
 * receiver's non-volatile memory.
 */
typedef struct
{
  int16_t year;           ///< year number, 0..9999
  int8_t month;           ///< month, 1..12
  int8_t mday;            ///< day of month, 1..31
  int16_t yday;           ///< day of year, 1..365, or 366 in case of leap year
  int8_t wday;            ///< day of week, 0..6 == Sun..Sat
  int8_t hour;            ///< hours, 0..23
  int8_t min;             ///< minutes, 0..59
  int8_t sec;             ///< seconds, 0..59, or 60 in case of inserted leap second
  int32_t frac;           ///< fractions of a second, 1/::RECEIVER_INFO::ticks_per_sec units
  int32_t offs_from_utc;  ///< local time offset from %UTC [sec]
  uint16_t status;        ///< status flags, see ::TM_GPS_STATUS_BIT_MASKS

} TM_GPS;



/**
 * @brief Status flag bits used to define ::TM_GPS_STATUS_BIT_MASKS
 *
 * These bits report info on the time conversion from GPS time to %UTC
 * and/or local time as well as device status info.
 *
 * @see ::TM_GPS_STATUS_BIT_MASKS
 */
enum TM_GPS_STATUS_BITS
{
  TM_BIT_UTC,          ///< %UTC correction has been made
  TM_BIT_LOCAL,        ///< %UTC has been converted to local time according to ::TZDL settings
  TM_BIT_DL_ANN,       ///< state of daylight saving is going to change
  TM_BIT_DL_ENB,       ///< daylight saving is in effect
  TM_BIT_LS_ANN,       ///< leap second pending
  TM_BIT_LS_ENB,       ///< current second is leap second
  TM_BIT_LS_ANN_NEG,   ///< set in addition to ::TM_BIT_LS_ANN if leap sec is negative
  TM_BIT_INVT,         ///< invalid time, e.g. if RTC battery bas been empty

  TM_BIT_EXT_SYNC,     ///< synchronized externally
  TM_BIT_HOLDOVER,     ///< in holdover mode after previous synchronization
  TM_BIT_ANT_SHORT,    ///< antenna cable short circuited
  TM_BIT_NO_WARM,      ///< OCXO has not warmed up
  TM_BIT_ANT_DISCONN,  ///< antenna currently disconnected
  TM_BIT_SYN_FLAG,     ///< TIME_SYN output is low
  TM_BIT_NO_SYNC,      ///< time sync actually not verified
  TM_BIT_NO_POS        ///< position actually not verified, LOCK LED off
};


/**
 * @brief Status flag masks used with ::TM_GPS::status
 *
 * These bits report info on the time conversion from GPS time to %UTC
 * and/or local time as well as device status info.
 *
 * @see ::TM_GPS_STATUS_BITS
 */
enum TM_GPS_STATUS_BIT_MASKS
{
  TM_UTC         = ( 1UL << TM_BIT_UTC ),          ///< see ::TM_BIT_UTC
  TM_LOCAL       = ( 1UL << TM_BIT_LOCAL ),        ///< see ::TM_BIT_LOCAL
  TM_DL_ANN      = ( 1UL << TM_BIT_DL_ANN ),       ///< see ::TM_BIT_DL_ANN
  TM_DL_ENB      = ( 1UL << TM_BIT_DL_ENB ),       ///< see ::TM_BIT_DL_ENB
  TM_LS_ANN      = ( 1UL << TM_BIT_LS_ANN ),       ///< see ::TM_BIT_LS_ANN
  TM_LS_ENB      = ( 1UL << TM_BIT_LS_ENB ),       ///< see ::TM_BIT_LS_ENB
  TM_LS_ANN_NEG  = ( 1UL << TM_BIT_LS_ANN_NEG ),   ///< see ::TM_BIT_LS_ANN_NEG
  TM_INVT        = ( 1UL << TM_BIT_INVT ),         ///< see ::TM_BIT_INVT

  TM_EXT_SYNC    = ( 1UL << TM_BIT_EXT_SYNC ),     ///< see ::TM_BIT_EXT_SYNC
  TM_HOLDOVER    = ( 1UL << TM_BIT_HOLDOVER ),     ///< see ::TM_BIT_HOLDOVER
  TM_ANT_SHORT   = ( 1UL << TM_BIT_ANT_SHORT ),    ///< see ::TM_BIT_ANT_SHORT
  TM_NO_WARM     = ( 1UL << TM_BIT_NO_WARM ),      ///< see ::TM_BIT_NO_WARM
  TM_ANT_DISCONN = ( 1UL << TM_BIT_ANT_DISCONN ),  ///< see ::TM_BIT_ANT_DISCONN
  TM_SYN_FLAG    = ( 1UL << TM_BIT_SYN_FLAG ),     ///< see ::TM_BIT_SYN_FLAG
  TM_NO_SYNC     = ( 1UL << TM_BIT_NO_SYNC ),      ///< see ::TM_BIT_NO_SYNC
  TM_NO_POS      = ( 1UL << TM_BIT_NO_POS )        ///< see ::TM_BIT_NO_POS
};


/* Two types of variables used to store a position. Type XYZ is */
/* used with a position in earth centered, earth fixed (ECEF) */
/* coordinates whereas type LLA holds such a position converted */
/* to geographic coordinates as defined by WGS84 (World Geodetic */
/* System from 1984). */

/**
 * @brief Sequence and number of components of a cartesian position
 */
enum XYZ_FIELDS { XP, YP, ZP, N_XYZ };  // x, y, z

/**
 * @brief A position in cartesian coordinates
 *
 * Usually earth centered, earth fixed (ECEF) coordinates,
 * in [m].
 *
 * @note In the original code this is an array of double.
 *
 * @see ::XYZ_FIELDS
 */
typedef l_fp XYZ[N_XYZ];


/**
 * @brief Sequence and number of components of a geographic position
 */
enum LLA_FIELDS { LAT, LON, ALT, N_LLA };  /* latitude, longitude, altitude */

/**
 * @brief A geographic position based on latitude, longitude, and altitude
 *
 * The geographic position associated to specific cartesian coordinates
 * depends on the characteristics of the ellipsoid used for the computation,
 * the so-called geographic datum. GPS uses the WGS84 (World Geodetic System
 * from 1984) ellipsoid by default.
 *
 * lon, lat in [rad], alt in [m]
 *
 * @note In the original code this is an array of double.
 *
 * @see ::LLA_FIELDS
 */
typedef l_fp LLA[N_LLA];


/**
 * @brief The name of a time zone
 *
 * @note Up to 5 printable characters, plus trailing zero
 */
typedef char TZ_NAME[6];

/**
 * @brief Antenna status and error at reconnect information
 *
 * The structure below reflects the status of the antenna,
 * the times of last disconnect/reconnect, and the board's
 * clock offset when it has synchronized again after the
 * disconnection interval.
 *
 * @note ::ANT_INFO::status changes back to ::ANT_RECONN only
 * after the antenna has been reconnected <b>and</b> the
 * receiver has re-synchronized to the satellite signal.
 * In this case ::ANT_INFO::delta_t reports the time offset
 * before resynchronization, i.e. how much the internal
 * time has drifted while the antenna was disconnected.
 */
typedef struct
{
  int16_t status;      ///< current status of antenna, see ::ANT_STATUS_CODES
  TM_GPS tm_disconn;   ///< time of antenna disconnect
  TM_GPS tm_reconn;    ///< time of antenna reconnect
  int32_t delta_t;     ///< clock offs at reconn. time in 1/::RECEIVER_INFO::ticks_per_sec units

} ANT_INFO;


/**
 * @brief Status code used with ::ANT_INFO::status
 */
enum ANT_STATUS_CODES
{
  ANT_INVALID,   ///< No other fields valid since antenna has not yet been disconnected
  ANT_DISCONN,   ///< Antenna is disconnected, tm_reconn and delta_t not yet set
  ANT_RECONN,    ///< Antenna has been disconnect, and receiver sync. after reconnect, so all fields valid
  N_ANT_STATUS_CODES  ///< the number of known status codes
};



/**
 * @brief Summary of configuration and health data of all satellites
 */
typedef struct
{
  CSUM csum;                  ///< checksum of the remaining bytes
  int16_t valid;              ///< flag data are valid

  T_GPS tot_51;               ///< time of transmission, page 51
  T_GPS tot_63;               ///< time of transmission, page 63
  T_GPS t0a;                  ///< complete reference time almanac

  CFG cfg[N_SVNO_GPS];        ///< 4 bit SV configuration code from page 63
  HEALTH health[N_SVNO_GPS];  ///< 6 bit SV health codes from pages 51, 63

} CFGH;



/**
 * @brief GPS %UTC correction parameters
 *
 * %UTC correction parameters basically as sent by the GPS satellites.
 *
 * The csum field is only used by the card's firmware to check the
 * consistency of the structure in non-volatile memory.
 *
 * The field labeled valid indicates if the parameter set is valid, i.e.
 * if it contains data received from the satellites.
 *
 * t0t, A0 and A1 contain fractional correction parameters for the current
 * GPS-%UTC time offset in addition to the whole seconds. This is evaluated
 * by the receivers' firmware to convert GPS time to %UTC time.
 *
 * The delta_tls field contains the current full seconds offset between
 * GPS time and %UTC, which corresponds to the number of leap seconds inserted
 * into the %UTC time scale since GPS was put into operation in January 1980.
 *
 * delta_tlfs holds the number of "future" leap seconds, i.e. the %UTC offset
 * after the next leap second event defined by WNlsf and DNt.
 *
 * The fields WNlsf and DNt specify the GPS week number and the day number
 * in that week for the end of which a leap second has been scheduled.
 *
 * @note: The satellites transmit WNlsf only as a signed 8 bit value, so it
 * can only define a point in time which is +/- 127 weeks off the current time.
 * The firmware tries to expand this based on the current week number, but
 * the result is ambiguous if the leap second occurs or occurred more
 * than 127 weeks in the future or past.
 *
 * So the leap second date should <b>only</b> be evaluated and displayed
 * in a user interface if the fields delta_tls and delta_tlsf have
 * different values, in which case there is indeed a leap second announcement
 * inside the +/- 127 week range.
 *
 * @note In the original code the type of A0 and A1 is double.
 */
typedef struct
{
  CSUM csum;          ///<  Checksum of the remaining bytes
  int16_t valid;      ///<  Flag indicating %UTC parameters are valid

  T_GPS t0t;          ///<  Reference Time %UTC Parameters [wn|sec]
  l_fp A0;            ///<  +- Clock Correction Coefficient 0 [sec]
  l_fp A1;            ///<  +- Clock Correction Coefficient 1 [sec/sec]

  uint16_t WNlsf;     ///<  Week number of nearest leap second
  int16_t DNt;        ///<  The day number at the end of which a leap second occurs
  int8_t delta_tls;   ///<  Current %UTC offset to GPS system time [sec]
  int8_t delta_tlsf;  ///<  Future %UTC offset to GPS system time after next leap second transition [sec]

} UTC;


/**
 * @brief GPS ASCII message
 */
typedef struct
{
  CSUM csum;       ///< checksum of the remaining bytes */
  int16_t valid;   ///< flag data are valid
  char s[23];      ///< 22 chars GPS ASCII message plus trailing zero

} ASCII_MSG;



void mbg_tm_str (char **, TM_GPS *, size_t, int);
void mbg_tgps_str (char **, T_GPS *, size_t);
void get_mbg_header (unsigned char **, GPS_MSG_HDR *);
void put_mbg_header (unsigned char **, GPS_MSG_HDR *);
void get_mbg_sw_rev (unsigned char **, SW_REV *);
void get_mbg_ascii_msg (unsigned char **, ASCII_MSG *);
void get_mbg_antinfo (unsigned char **, ANT_INFO *);
void get_mbg_cfgh (unsigned char **, CFGH *);
void get_mbg_utc (unsigned char **, UTC *);
void get_mbg_lla (unsigned char **, LLA);
void get_mbg_xyz (unsigned char **, XYZ);

CSUM mbg_csum (unsigned char *, unsigned int) __attribute__((pure));

#endif
/*
 * History:
 *
 * mbg_gps166.h,v
 * Revision 4.7  2006/06/22 18:41:43  kardel
 * clean up signedness (gcc 4)
 *
 * Revision 4.6  2005/10/07 22:11:56  kardel
 * bounded buffer implementation
 *
 * Revision 4.5.2.1  2005/09/25 10:23:48  kardel
 * support bounded buffers
 *
 * Revision 4.5  2005/06/25 10:58:45  kardel
 * add missing log keywords
 *
 * Revision 4.1  1998/06/12 15:07:30  kardel
 * fixed prototyping
 *
 * Revision 4.0  1998/04/10 19:50:42  kardel
 * Start 4.0 release version numbering
 *
 * Revision 1.1  1998/04/10 19:27:34  kardel
 * initial NTP VERSION 4 integration of PARSE with GPS166 binary support
 *
 * Revision 1.1  1997/10/06 20:55:38  kardel
 * new parse structure
 *
 */
