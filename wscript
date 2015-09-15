VERSION_MAJOR = "1"
VERSION_MINOR = "0"
VERSION_REV   = "0"

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
	grp.add_option('--cflags', type='string', action="callback", callback=callback_flags, help="This overrides any internal or environment settings can be used multiple times.")
	grp.add_option('--ldflags', type='string', action="callback", callback=callback_flags, help="This overrides any internal or environment settings can be used multiple times.")


	grp = ctx.add_option_group("NTP features")
	grp.add_option('--enable-leap-smear', action='store_true', default=False, help="Enable Leap Smearing.")
	grp.add_option('--enable-mssntp', action='store_true', default=False, help="Enable Samba MSS NTP support.")

	grp = ctx.add_option_group("Refclock options")
	grp.add_option('--refclock', dest='refclocks', help="Comma-separated list of Refclock IDs to build (or \"all\")", type='string')
	grp.add_option('--list', action='store_true', default=False, help="List available Refclocks")

	grp = ctx.add_option_group("NTP developer options")
	grp.add_option('--enable-saveconfig', action='store_true', help="Enable configuration saving on exit.")



def configure(ctx):
	ctx.env.VERSION_MAJOR = VERSION_MAJOR
	ctx.env.VERSION_MINOR = VERSION_MINOR
	ctx.env.VERSION_REV = VERSION_REV
	ctx.env.VERSION_FULL = "%s.%s.%s" % (VERSION_MAJOR, VERSION_MINOR, VERSION_REV)


	ctx.load('waf', tooldir='pylib/')

	ctx.env.OPT_STORE = OPT_STORE

	cmd_configure(ctx)


def build(ctx):
	ctx.load('waf', tooldir='pylib/')
	ctx.load('bison')

	if ctx.env.ENABLE_DOC:
		print "hi"
		ctx.load('asciidoc', tooldir='pylib/')
		ctx.recurse("docs")


	ctx.recurse("lib/isc")
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

