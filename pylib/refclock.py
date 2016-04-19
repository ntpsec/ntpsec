from waflib.Configure import conf
from waflib.Logs import pprint

# Note: When you change this list. also check the following files:
# doc/recflock.txt
# include/ntp.h
# libntp/clocktypes.c
# ntpd/refclock_conf.c

refclock_map = {

	1: {
		"descr":	"Undisciplined Local Clock",
		"define":	"CLOCK_LOCAL",
		"file":		"local"
	},

	4: {
		"descr":	"Spectracom WWVB/GPS Receivers",
		"define":	"CLOCK_SPECTRACOM",
		"file":		"spectracom"
	},

	5: {
		"descr":	"TrueTime GPS/GOES/OMEGA Receivers",
		"define":	"CLOCK_TRUETIME",
		"file":		"true"
	},

	6: {
		"descr":	"IRIG Audio Decoder",
		"define":	"CLOCK_IRIG",
		"require":	["audio"],
		"file":		"irig"
	},

	7: {
		"descr":	"Radio CHU Audio Demodulator/Decoder",
		"define":	"CLOCK_CHU",
		"require":	["audio"],
		"file":		"chu"
	},

	8: {
		"descr":	"Generic Reference Driver (Parse)",
		"define":	"CLOCK_PARSE",
		"require":	["parse"],
		"file":		"parse"
	},

	9: {
		"descr":	"Magnavox MX4200 GPS Receiver",
		"define":	"CLOCK_MX4200",
		"require":	["ppsapi"],
		"file":		"mx4200"
	},

	10: {
		"descr":	"Austron 2200A/2201A GPS Receivers",
		"define":	"CLOCK_AS2201",
		"file":		"as2201"
	},

	11: {
		"descr":	"Arbiter 1088A/B GPS Receiver",
		"define":	"CLOCK_ARBITER",
		"file":		"arbiter"
	},

	18: {
		"descr":	"NIST/USNO/PTB Modem Time Services",
		"define":	"CLOCK_ACTS",
		"file":		"acts"
	},

	20: {
		"descr":	"Generic NMEA GPS Receiver",
		"define":	"CLOCK_NMEA",
		"file":		"nmea"
	},

	22: {
		"descr":	"PPS Clock Discipline",
		"define":	"CLOCK_ATOM",
		"require":	["ppsapi"],
		"file":		"atom"
	},

	26: {
		"descr":	"Hewlett Packard 58503A GPS Receiver",
		"define":	"CLOCK_HPGPS",
		"file":		"hpgps"
	},

	27: {
		"descr":	"Arcron MSF Receiver",
		"define":	"CLOCK_ARCRON_MSF",
		"file":		"arc"
	},

	28: {
		"descr":	"Shared Memory Driver",
		"define":	"CLOCK_SHM",
		"file":		"shm"
	},

	29: {
		"descr":	"Trimble Navigation Palisade GPS",
		"define":	"CLOCK_PALISADE",
		"file":		"palisade"
	},

	30: {
		"descr":	"Motorola UT Oncore GPS",
		"define":	"CLOCK_ONCORE",
		"require":	["ppsapi"],
		"file":		"oncore"
	},

	31: {
		"descr":	"Rockwell Jupiter GPS",
		"define":	"CLOCK_JUPITER",
		"require":	["ppsapi"],
		"file":		"jupiter"
	},

	33: {
		"descr":	"Dumb Clock",
		"define":	"CLOCK_DUMBCLOCK",
		"file":		"dumbclock"
	},

	35: {
		"descr":	"Conrad Parallel Port Radio Clock",
		"define":	"CLOCK_PCF",
		"file":		"pcf"
	},

	38: {
		"descr":	"hopf GPS/DCF77 6021/komp for Serial Line",
		"define":	"CLOCK_HOPF_SERIAL",
		"file":		"hopfser"
	},

	39: {
		"descr":	"hopf GPS/DCF77 6039 for PCI-Bus",
		"define":	"CLOCK_HOPF_PCI",
		"file":		"hopfpci"
	},

	40: {
		"descr":	"JJY Receivers",
		"define":	"CLOCK_JJY",
		"file":		"jjy"
	},

	42: {
		"descr":	"Zyfer GPStarplus Receiver",
		"define":	"CLOCK_ZYFER",
		"file":		"zyfer"
	},

	44: {
		"descr":	"NeoClock4X - DCF77 / TDF serial line",
		"define":	"CLOCK_NEOCLOCK4X",
		"file":		"neoclock4x"
	},

	45: {
		"descr":	"Spectracom TSYNC",
		"define":	"CLOCK_TSYNCPCI",
		"file":		"tsyncpci"
	},

	46: {
		"descr":	"GPSD NG client protocol",
		"define":	"CLOCK_GPSDJSON",
		"file":		"gpsdjson"
	}
}



@conf
def refclock_config(ctx):
	from refclock import refclock_map

	if ctx.options.refclocks == "all":
		ids = refclock_map.keys()
	else:
		# XXX: better error checking
		ids = ctx.options.refclocks.split(",")

	ctx.env.REFCLOCK_DEFINES = []
	ctx.env.REFCLOCK_SOURCE = []

	# Remove duplicate IDs while preserving order.
	unique_id = []
	[unique_id.append(x) for x in ids if x not in unique_id]


	refclock = False
	for id in unique_id:
		try:
			id = int(id)
		except ValueError:
			ctx.fatal("'%s' is not an integer." % id)

		if id not in refclock_map:
			ctx.fatal("'%s' is not a valid Refclock ID" % id)

		rc = refclock_map[id]

		if rc['define'] == "CLOCK_PARSE":
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

		ctx.start_msg("Enabling Refclock %s (%d):" % (rc["descr"], id))

		if "require" in rc:
			if "ppsapi" in rc["require"]:
				if not ctx.get_define("HAVE_PPSAPI"):
					ctx.end_msg("No")
					pprint("RED", "Refclock \"%s\" disabled, PPS API has not been detected as working." % rc["descr"])
					continue

			if "audio" in rc["require"]:
				if not ctx.env.AUDIO_ENABLE:
					ctx.end_msg("No")
					pprint("RED", "Refclock \"%s\" disabled, Audio (OSS) support is not available." % rc["descr"])
					continue


		ctx.env.REFCLOCK_SOURCE.append((rc["file"], rc["define"]))
		ctx.env["REFCLOCK_%s" % rc["file"].upper()] = True
		ctx.define(rc["define"], 1, comment="Enable '%s' refclock" % rc["descr"])
		ctx.env.REFCLOCK_LIST += [str(id)]

		ctx.end_msg("Yes")

		refclock = True

	if refclock:
		ctx.env.REFCLOCK_ENABLE = True
		ctx.define("REFCLOCK", 1, comment="Enable refclock support")
