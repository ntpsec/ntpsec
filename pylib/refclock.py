refclock_map = {

	1: {
		"descr":	"Undisciplined Local Clock",
		"define":	"CLOCK_LOCAL",
		"file":		"local"
	},

	3: {
		"descr":	"PSTI/Traconex 1020 WWV/WWVH Receiver",
		"define":	"CLOCK_PST",
		"file":		"pst"
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
		"file":		"irig"
	},

	7: {
		"descr":	"Radio CHU Audio Demodulator/Decoder",
		"define":	"CLOCK_CHU",
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

	16: {
		"descr":	"Bancomm GPS/IRIG Receiver",
		"define":	"CLOCK_BANC",
		"file":		"bancomm"
	},

	18: {
		"descr":	"NIST/USNO/PTB Modem Time Services",
		"define":	"CLOCK_ACTS",
		"file":		"acts"
	},

	19: {
		"descr":	"Heath WWV/WWVH Receiver",
		"define":	"CLOCK_HEATH",
		"file":		"heath"
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
		"file":		"oncore"
	},

	31: {
		"descr":	"Rockwell Jupiter GPS",
		"define":	"CLOCK_JUPITER",
		"require":	["ppsapi"],
		"file":		"jupiter"
	},

	32: {
		"descr":	"Chrono-log K-series WWVB receiver",
		"define":	"CLOCKCHRONOLOG",
		"file":		"chronolog"
	},

	33: {
		"descr":	"Dumb Clock",
		"define":	"CLOCK_DUMBCLOCK",
		"file":		"dumbclock"
	},

	34: {
		"descr":	"Ultralink WWVB Receivers",
		"define":	"CLOCK_ULINK",
		"file":		"ulink"
	},

	35: {
		"descr":	"Conrad Parallel Port Radio Clock",
		"define":	"CLOCK__PCF",
		"file":		"pcf"
	},

	36: {
		"descr":	"NIST WWV/H time/frequency station",
		"define":	"CLOCK_WWV",
		"file":		"wwv"
	},

	37: {
		"descr":	"Forum Graphic GPS Dating station",
		"define":	"CLOCK_FG",
		"file":		"fg"
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

	43: {
		"descr":	"RIPE NCC interface for Trimble Palisade",
		"define":	"CLOCK_RIPENCC",
		"file":		"ripencc"
	},

	44: {
		"descr":	"NeoClock4X - DCF77 / TDF serial line",
		"define":	"CLOCK_NEOCLOCK4X",
		"file":		"neoclock4x"
	},

	45: {
		"descr":	"Spectracom TSYNC",
		"define":	"CLOCK_TSYNPCI",
		"file":		"tsyncpci"
	},

	46: {
		"descr":	"GPSD NG client protocol",
		"define":	"CLOCK_GPSDJSON",
		"file":		"gpsdjson"
	}
}
