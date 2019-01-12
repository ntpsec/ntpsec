from waflib.Configure import conf
from waflib.Logs import pprint

# Note: When you change this list. also check the following files:
# doc/refclock.adoc
# include/ntp_refclock.h
# ntpd/refclock_conf.c

refclock_map = {

    "local": {
        "descr":    "Undisciplined Local Clock",
        "define":   "CLOCK_LOCAL",
        "file":     "local"
    },

    "spectracom": {
        "descr":    "Spectracom GPS Receivers",
        "define":   "CLOCK_SPECTRACOM",
        "file":     "spectracom"
    },

    "truetime": {
        "descr":    "TrueTime GPS/GOES/OMEGA Receivers",
        "define":   "CLOCK_TRUETIME",
        "file":     "truetime"
    },

    "generic": {
        "descr":    "Generic Reference Driver (Parse)",
        "define":   "CLOCK_GENERIC",
        "require":  ["parse"],
        "file":     "generic"
    },

    "arbiter": {
        "descr":    "Arbiter 1088A/B GPS Receiver",
        "define":   "CLOCK_ARBITER",
        "file":     "arbiter"
    },

    "modem": {
        "descr":    "NIST/USNO/PTB Modem Time Services",
        "define":   "CLOCK_MODEM",
        "file":     "modem"
    },

    "nmea": {
        "descr":    "Generic NMEA GPS Receiver",
        "define":   "CLOCK_NMEA",
        "file":     "nmea"
    },

    "pps": {
        "descr":    "PPS Clock Discipline",
        "define":   "CLOCK_PPS",
        "require":  ["ppsapi"],
        "file":     "pps"
    },

    "hpgps": {
        "descr":    "Hewlett Packard 58503A GPS Receiver",
        "define":   "CLOCK_HPGPS",
        "file":     "hpgps"
    },

    "shm": {
        "descr":    "Shared Memory Driver",
        "define":   "CLOCK_SHM",
        "file":     "shm"
    },

    "trimble": {
        "descr":    "Trimble Navigation GPSes",
        "define":   "CLOCK_TRIMBLE",
        "file":     "trimble"
    },

    "oncore": {
        "descr":    "Motorola UT Oncore GPS",
        "define":   "CLOCK_ONCORE",
        "require":  ["ppsapi"],
        "file":     "oncore"
    },

    "jjy": {
        "descr":    "JJY Receivers",
        "define":   "CLOCK_JJY",
        "file":     "jjy"
    },

    "zyfer": {
        "descr":    "Zyfer GPStarplus Receiver",
        "define":   "CLOCK_ZYFER",
        "file":     "zyfer"
    },

    "neoclock": {
        "descr":    "NeoClock4X - DCF77 / TDF serial line",
        "define":   "CLOCK_NEOCLOCK",
        "file":     "neoclock"
    },

    "gpsd": {
        "descr":    "GPSD NG client protocol",
        "define":   "CLOCK_GPSDJSON",
        "file":     "gpsd"
    }
}


@conf
def refclock_config(ctx):
    if ctx.options.refclocks == "all":
        ids = refclock_map.keys()
    else:
        # XXX: better error checking
        ids = ctx.options.refclocks.split(",")

    ctx.env.REFCLOCK_SOURCE = []

    # Remove duplicate IDs while preserving order.
    unique_id = []
    [unique_id.append(x) for x in ids if x not in unique_id]

    refclock = False
    for id in unique_id:
        if id not in refclock_map:
            ctx.fatal("'%s' is not a valid Refclock ID" % id)

        rc = refclock_map[id]

        if rc['define'] == "CLOCK_GENERIC":
            parse_clocks = (
                "CLOCK_COMPUTIME",
                "CLOCK_DCF7000",
                "CLOCK_HOPF6021",
                "CLOCK_MEINBERG",
                "CLOCK_RAWDCF",
                "CLOCK_RCC8000",
                "CLOCK_SCHMID",
                "CLOCK_SEL240X",
                "CLOCK_TRIMTAIP",
                "CLOCK_TRIMTSIP",
                "CLOCK_VARITEXT",
                "CLOCK_WHARTON_400A",
            )
            for subtype in parse_clocks:
                ctx.define(subtype, 1, comment="Enable individual parse clock")

        ctx.start_msg("Enabling Refclock %s (%s):" % (rc["descr"], id))

        if "require" in rc:
            if "ppsapi" in rc["require"]:
                if not ctx.get_define("HAVE_PPSAPI"):
                    ctx.end_msg("No")
                    pprint("RED",
                           "Refclock \"%s\" disabled, PPS API has not "
                           "been detected as working." % rc["descr"])
                    continue

        ctx.env.REFCLOCK_SOURCE.append((rc["file"], rc["define"]))
        ctx.env["REFCLOCK_%s" % rc["file"].upper()] = True
        ctx.define(rc["define"], 1,
                   comment="Enable '%s' refclock" % rc["descr"])
        ctx.env.REFCLOCK_LIST += [str(id)]

        ctx.end_msg("Yes")

        refclock = True

    if refclock:
        ctx.env.REFCLOCK_ENABLE = True
        ctx.define("REFCLOCK", 1, comment="Enable refclock support")
