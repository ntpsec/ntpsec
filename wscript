config = {
	# "NTPS for NTPSec this avoids any naming collissions
	"NTPS_VERSION_MAJOR": 1,
	"NTPS_VERSION_MINOR": 0,
	"NTPS_VERSION_REV":	 0
}

from waflib import Utils

out="build"

from pylib.configure import cmd_configure

OPT_STORE = {} # Storage for options to pass into configure


def options(ctx):
	ctx.load("compiler_c")

	def callback_flags(option, opt, value, parser):
		OPT_STORE.setdefault(opt, []).append(value)

	grp = ctx.add_option_group("NTP build options")
	grp.add_option('--enable-debug', action='store_true', default=False, help="Enable debugging code")
	grp.add_option('--enable-debug-gdb', action='store_true', default=False, help="Enable GDB debugging symbols")
	grp.add_option('--enable-ipv6', action='store_true', default=False, help="Enable IPv6")
	grp.add_option('--enable-doc', action='store_true', default=False, help="Build NTP documentation")
	grp.add_option('--enable-crypto', action='store_true', default=False, help="Enable OpenSSL.")


	grp = ctx.add_option_group("NTP features")
	grp.add_option('--enable-leap-smear', action='store_true', default=False, help="Enable Leap Smearing.")
	grp.add_option('--enable-mssntp', action='store_true', default=False, help="Enable Samba MSS NTP support.")

	grp = ctx.add_option_group("Refclock options")
	grp.add_option('--refclock', dest='refclocks', help="Comma-separated list of Refclock IDs to build (or \"all\")", type='string')
	grp.add_option('--list', action='store_true', default=False, help="List available Refclocks")

	grp = ctx.add_option_group("NTP developer options")
	grp.add_option('--enable-saveconfig', action='store_true', help="Enable configuration saving on exit.")
	grp.add_option('--build-version-tag', type='string', help="Append a tag to the version string.")
	grp.add_option('--cflags', type='string', action="callback", callback=callback_flags, help="Users should use CFLAGS in their environment.")
	grp.add_option('--ldflags', type='string', action="callback", callback=callback_flags, help="Users should use LDFLAGS in their environment.")



def configure(ctx):
	ctx.env.NTPS_VERSION_MAJOR = config["NTPS_VERSION_MAJOR"]
	ctx.env.NTPS_VERSION_MINOR = config["NTPS_VERSION_MINOR"]
	ctx.env.NTPS_VERSION_REV = config["NTPS_VERSION_REV"]

	ctx.env.NTPS_VERSION = "%s.%s.%s" % (ctx.env.NTPS_VERSION_MAJOR, ctx.env.NTPS_VERSION_MINOR, ctx.env.NTPS_VERSION_REV)
	ctx.define("NTPS_VERSION_MAJOR", ctx.env.NTPS_VERSION_MAJOR)
	ctx.define("NTPS_VERSION_MINOR", ctx.env.NTPS_VERSION_REV)
	ctx.define("NTPS_VERSION_REV", ctx.env.NTPS_VERSION_REV)




	ctx.load('waf', tooldir='pylib/')

	ctx.env.OPT_STORE = OPT_STORE

	cmd_configure(ctx)


def build(ctx):
	ctx.load('waf', tooldir='pylib/')
	ctx.load('bison')

	if ctx.env.ENABLE_DOC:
		ctx.load('asciidoc', tooldir='pylib/')
		ctx.recurse("docs")


	ctx.recurse("lib/isc")
	if ctx.env.REFCLOCK_PARSE: # Only required by the parse refclock
		ctx.recurse("libparse")
	ctx.recurse("libntp")
	ctx.recurse("sntp")
	ctx.recurse("ntpd")
	ctx.recurse("ntpdate")
	ctx.recurse("ntpq")
#	ctx.recurse("ntpsnmpd")
#	ctx.recurse("clockstuff") - 
#	ctx.recurse("kernel")
	ctx.recurse("ntp-keygen") 
#	ctx.recurse("unity")
#	ctx.recurse("tests")


	subst_source = [
		"ntpd/complete.conf.in",
		"scripts/deprecated/freq_adj.in",
		"scripts/ntp-wait/ntp-wait.in",
		"scripts/ntpsweep/ntpsweep.in",
		"scripts/ntptrace/ntptrace.in",
		"scripts/update-leap/update-leap.in"
	]

	ctx(
		features    = "subst",
		source      = subst_source,
		target		= [x.replace(".in", "") for x in subst_source],
		chmod		= Utils.O755
	)

