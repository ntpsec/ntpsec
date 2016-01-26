from waflib import Utils
from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext, StepContext, ListContext
from waflib import Context, Errors
from waflib import Scripting
from waflib.Logs import pprint

config = {
	"NTPS_RELEASE": False # Set to 'True' if this is a release
}

out="build"

from pylib.configure import cmd_configure
from waflib.Tools import waf_unit_test
from pylib.test import test_write_log, test_print_log

OPT_STORE = {} # Storage for options to pass into configure

def parse_version():
        with open("VERSION", "r") as f:
                version_string = f.read().strip()
        [major,minor,rev] = version_string.split(".")
        return {
                # "NTPS" for NTPSec -- this avoids any naming collisions
                "NTPS_VERSION_MAJOR" : int(major),
                "NTPS_VERSION_MINOR" : int(minor),
                "NTPS_VERSION_REV" : int(rev)
        }

config.update(parse_version())

def dist(ctx):
		from os import path
		from shutil import copyfile
		files_man = []

		if not config["NTPS_RELEASE"]:
			ctx.fatal("NTPS_RELEASE must be set to True")


		# XXX: Redo to not use globs.
		bldnode = ctx.path.make_node(out)
		for section in [1, 5, 8]:
			files_man += bldnode.ant_glob("**/*.%d" % section)

		# Need a more reliable check.
		if not files_man:
			ctx.fatal("You must configure and build first with NTPS_RELEASE set to false")

		for man in files_man:
			src = man.abspath()
			dst = src.replace("%s/main/" % bldnode.abspath(), "")
			print "Copying %s -> %s" % (src, dst)
			copyfile(src, dst)

		files = [
			("build/host/ntpd/ntp_parser.tab.c", "ntpd/ntp_parser.tab.c"),
			("build/host/ntpd/ntp_parser.tab.h", "ntpd/ntp_parser.tab.h")
		]

		for src, dst in files:
			if not path.exists(src):
				ctx.fatal("%s doesn't exist please configure and build first.  NTPS_RELEASE must be set to False" % src)
			print "Copying %s -> %s" % (src, dst)
			copyfile(src, dst)

		ctx.base_name = "ntpsec-%d.%d.%d" % \
						(config["NTPS_VERSION_MAJOR"], \
						config["NTPS_VERSION_MINOR"], \
						config["NTPS_VERSION_REV"])
		if ctx.options.build_version_tag:
			ctx.base_name = "%s-%s" % (ctx.base_name, ctx.options.build_version_tag)

def options(ctx):
	ctx.load("compiler_c")
	ctx.load("msvc")
	ctx.load('waf_unit_test')

	def callback_flags(option, opt, value, parser):
		OPT_STORE.setdefault(opt, []).append(value)

	grp = ctx.add_option_group("NTP configure options")
	grp.add_option('--enable-debug', action='store_true', default=False, help="(ignored)")
	grp.add_option('--disable-debug', action='store_true', default=False, help="Disable debugging code")
	grp.add_option('--enable-debug-gdb', action='store_true', default=False, help="Enable GDB debugging symbols")
	grp.add_option('--enable-crypto', action='store_true', default=False, help="Enable OpenSSL.")
	grp.add_option('--disable-droproot', action='store_true', default=False, help="Disable dropping root.")
	grp.add_option('--disable-dns-lookup', action='store_true', default=False, help="Disable DNS lookups.")
	grp.add_option('--disable-dns-retry', action='store_true', default=False, help="Disable retrying DNS lookups.")
	grp.add_option('--disable-mdns-registration', action='store_true', default=False, help="Disable MDNS registration.")

	grp = ctx.add_option_group("NTP cross compile options")
	grp.add_option('--cross-compiler', type='string', help="Path to cross compiler CC. (enables cross-compiling)")
	grp.add_option('--cross-cflags', type='string',  action="callback", callback=callback_flags, help="Cross compiler CFLAGS.")
	grp.add_option('--cross-ldflags', type='string', action="callback", callback=callback_flags, help="Cross compiler LDFLAGS.")

	grp = ctx.add_option_group("NTP configure features")
	grp.add_option('--enable-leap-smear', action='store_true', default=False, help="Enable Leap Smearing.")
	grp.add_option('--enable-mssntp', action='store_true', default=False, help="Enable Samba MSS NTP support.")
	grp.add_option('--enable-lockclock', action='store_true', default=False, help="Enable NIST lockclock scheme.")

	grp = ctx.add_option_group("Refclock configure options")
	grp.add_option('--refclock', dest='refclocks', help="Comma-separated list of Refclock IDs to build (or \"all\")", type='string')
	grp.add_option('--list', action='store_true', default=False, help="List available Refclocks")

	grp = ctx.add_option_group("NTP developer configure options")
	grp.add_option('--enable-saveconfig', action='store_true', help="Enable configuration saving on exit.")
	grp.add_option('--build-version-tag', type='string', help="Append a tag to the version string.")
	grp.add_option('--cflags', type='string', action="callback", callback=callback_flags, help="Users should use CFLAGS in their environment.")
	grp.add_option('--ldflags', type='string', action="callback", callback=callback_flags, help="Users should use LDFLAGS in their environment.")
	grp.add_option('--enable-fortify', action='store_true', help="Enable HP Fortify.")
	grp.add_option('--fortify-flags', type='string', action='store', help="Fortify flags.")
	grp.add_option('--check', action='store_true', default=False, help="Run tests")
	grp.add_option('--enable-rtems-trace', action='store_true', default=False, help="Enable RTEMS Trace.")
	grp.add_option('--rtems-trace-path', type='string', default="", help="Path to rtems-tld.")

	grp = ctx.add_option_group("NTP documentation configure options")
	grp.add_option('--enable-doc', action='store_true', default=False, help="Build NTP documentation")
	grp.add_option('--enable-doc-only', action='store_true', default=False, help="Only build NTP documentation")
	grp.add_option('--enable-a2x-xmllint', action='store_true', default=False, help="Build NTP documentation with a2x XML lint")
	grp.add_option('--disable-manpage', action='store_true', default=False, help="Disable Manpage building.")
	grp.add_option('--path-doc', type='string', action='store', default=None, help="Force doc install directory.")


def configure(ctx):
	ctx.env.NTPS_RELEASE = config["NTPS_RELEASE"]
	ctx.env.NTPS_VERSION_MAJOR = config["NTPS_VERSION_MAJOR"]
	ctx.env.NTPS_VERSION_MINOR = config["NTPS_VERSION_MINOR"]
	ctx.env.NTPS_VERSION_REV = config["NTPS_VERSION_REV"]

	ctx.env.NTPS_VERSION = "%s.%s.%s" % (ctx.env.NTPS_VERSION_MAJOR, ctx.env.NTPS_VERSION_MINOR, ctx.env.NTPS_VERSION_REV)
	ctx.define("NTPS_VERSION_MAJOR", ctx.env.NTPS_VERSION_MAJOR)
	ctx.define("NTPS_VERSION_MINOR", ctx.env.NTPS_VERSION_REV)
	ctx.define("NTPS_VERSION_REV", ctx.env.NTPS_VERSION_REV)


	ctx.load('waf', tooldir='pylib/')
	ctx.load('waf_unit_test')

	ctx.env.OPT_STORE = OPT_STORE

	cmd_configure(ctx)


from waflib.Build import BuildContext
class check(BuildContext):
	cmd = 'check'
	variant = "main"

# Borrowed from https://www.rtems.org/
variant_cmd = (
	("build",   BuildContext),
	("clean",   CleanContext),
	("install", InstallContext),
	("step",    StepContext),
	("list",    ListContext),
#	("check",   BuildContext)
)

for v in ["host", "main"]:
    # the reason for creating these subclasses is just for __doc__ below...
	for cmd, cls in variant_cmd:
		class tmp(cls):
			__doc__ = "%s %s" % (cmd, v)
			cmd = "%s_%s" % (cmd, v)
			variant = v

def init_handler(ctx):
	cmd = ctx.cmd
	if cmd == 'init_handler':
		cmd = 'build'

	def make_context(name):
		for x in Context.classes:
			if x.cmd == name and x.fun != 'init_handler':
				return x()
		ctx.fatal('No class for %r' % cmd)

	# By default we want to iterate over each variant.
	for v in ["host", "main"]:
		obj = make_context(cmd)
		obj.variant = v
		pprint("YELLOW", "--- %sing %s ---" % (cmd, v))
		obj.execute()

commands = (
	("install",     "init_handler", None),
	("uninstall",   "init_handler", None),
	("build",       "init_handler", None),
	("clean",       "init_handler", None),
	("list",        "init_handler", None),
	("step",        "init_handler", None),
#	("info",        "cmd_info",     "Show build information / configuration.")
)


for command, func, descr in commands:
	class tmp(Context.Context):
		if descr:
			__doc__ = descr
		cmd = command
		fun = func
		if command in 'install uninstall build clean list step docs bsp info':
			execute = Scripting.autoconfigure(Context.Context.execute)
# end borrowed code



def build(ctx):

	ctx.load('waf', tooldir='pylib/')
	ctx.load('bison')
	ctx.load('asciidoc', tooldir='pylib/')
	ctx.load('rtems_trace', tooldir='pylib/')

	if ctx.env.ENABLE_DOC:
		if ctx.variant != "main":
			ctx.recurse("docs")

	if ctx.env.ENABLE_DOC_ONLY:
		return


	if ctx.variant == "host":
		ctx.recurse("ntpd")
		return


	ctx.recurse("libisc")
	if ctx.env.REFCLOCK_PARSE: # Only required by the parse refclock
		ctx.recurse("libparse")
	ctx.recurse("libntp")
	if ctx.env.LIBEVENT2_ENABLE:
		ctx.recurse("ntpdig")
	ctx.recurse("libsodium")
	ctx.recurse("ntpd")
	ctx.recurse("ntpfrob")
	ctx.recurse("ntpq")
	ctx.recurse("ntpkeygen")
	ctx.recurse("ntptime")
	ctx.recurse("util")
	ctx.recurse("tests")


	subst_scripts = [
		"ntpwait/ntpwait",
		"ntptrace/ntptrace",
		"util/ntpsweep/ntpsweep",
	]

	ctx(
		features    = "subst",
		source      = subst_scripts,
		target	    = [x.replace(".in", "") for x in subst_scripts],
		chmod	    = Utils.O755
	)

	ctx.manpage(8, "ntpleapfetch/ntpleapfetch-man.txt")
	ctx.manpage(8, "ntpwait/ntpwait-man.txt")
	ctx.manpage(1, "ntptrace/ntptrace-man.txt")


	# Skip running unit tests on a cross compile build
	if not ctx.env.ENABLE_CROSS:
		# Force re-running of tests.  Same as 'waf --alltests'
		if ctx.cmd == "check":
			ctx.options.all_tests = True

			# Print log if -v is supplied
			if ctx.options.verbose:
				ctx.add_post_fun(test_print_log)

		# Write test log to a file
		ctx.add_post_fun(test_write_log)

		# Print a summary at the end
		ctx.add_post_fun(waf_unit_test.summary)
	else:
		pprint("YELLOW", "Unit test runner skipped on a cross-compiled build.")
