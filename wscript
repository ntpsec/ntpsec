from waflib import Utils
from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext, StepContext, ListContext
from waflib import Context, Errors
from waflib import Scripting
from waflib.Logs import pprint
import os

pprint.__doc__ = None

out="build"

from wafhelpers.configure import cmd_configure
from waflib.Tools import waf_unit_test
from wafhelpers.test import test_write_log, test_print_log
from wafhelpers.options import options_cmd

config = {
	"NTPSEC_RELEASE": False,
	"out": out,
	"OPT_STORE": {}
}

# Release procedure:
#   1. waf configure
#   2. waf build
#   3. Edit wscript and set NTPSEC_RELEASE to True
#   4. waf dist

# Snapshot procedure:
#   Steps 1-3 as above.
#   4. waf dist --build-snapshot

def help(ctx):
    "Be helpful, give a usage"
    print('''
Usage: waf <command>
    configure	Configure the project
    build	Build the project
    install     Install the project
    dist        Create a release

''')

def dist(ctx):
	from wafhelpers.dist import dist_cmd
	dist_cmd(ctx, config)


def options(ctx):
	options_cmd(ctx, config)
	ctx.recurse("pylib")

def configure(ctx):
	from wafhelpers.configure import cmd_configure
	cmd_configure(ctx, config)
	ctx.recurse("pylib")

from waflib.Build import BuildContext
class check(BuildContext):
	cmd = 'check'
	variant = "main"

def bin_test(ctx):
	"""Run binary check, use after tests."""
	from wafhelpers.bin_test import cmd_bin_test
	cmd_bin_test(ctx, config)

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

def linkmaker(ctx):
    # Make magic links to support in-tree testing.
    # The idea is that all directories where the Python tools
    # listed above live should have an 'ntp' symlink so they
    # can import compiled Python modules from the build directory.
    # Also, they need to be able to see the Python extension
    # module built in libntp.
    print("Making in-tree links...") 
    bldnode = ctx.bldnode.abspath()
    srcnode = ctx.srcnode.abspath()
    for d in ("ntpq", "ntpstats", "ntpsweep", "ntptrace", "ntpwait"):
	    os.system("ln -sf %s/pylib %s/%s/ntp" % (bldnode, srcnode, d))
    os.system("ln -sf %s/libntp/ntpc.so %s/pylib/ntpc.so " % (bldnode, bldnode))

def build(ctx):
	ctx.load('waf', tooldir='wafhelpers/')
	ctx.load('bison')
	ctx.load('asciidoc', tooldir='wafhelpers/')
	ctx.load('rtems_trace', tooldir='wafhelpers/')

	if ctx.env.ENABLE_DOC_USER:
		if ctx.variant != "main":
			ctx.recurse("docs")

	if ctx.env.ENABLE_DOC_ONLY:
		return


	if ctx.variant == "host":
		ctx.recurse("ntpd")
		return


	ctx.recurse("libisc")
	if ctx.env.REFCLOCK_GENERIC: # Only required by the generic refclock
		ctx.recurse("libparse")
	ctx.recurse("libntp")
	if ctx.env.LIBEVENT2_ENABLE:
		ctx.recurse("ntpdig")
	ctx.recurse("libsodium")
	ctx.recurse("ntpd")
	ctx.recurse("ntpfrob")
	ctx.recurse("ntpkeygen")
	ctx.recurse("ntptime")
	ctx.recurse("pylib")
	ctx.recurse("util")
	ctx.recurse("tests")

	scripts = [
		"ntpleapfetch/ntpleapfetch",
		"ntpq/ntpq",
		"ntpstats/ntpviz",
		"ntptrace/ntptrace",
		"ntpwait/ntpwait",
		"ntpsweep/ntpsweep",
	]

	ctx(
		features    = "subst",
		source      = scripts,
		target	    = scripts,
		chmod	    = Utils.O755,
		install_path = "${PREFIX}/bin/"
	)

	ctx.add_post_fun(linkmaker)

	ctx.manpage(8, "ntpleapfetch/ntpleapfetch-man.txt")
	ctx.manpage(1, "ntptrace/ntptrace-man.txt")
	ctx.manpage(1, "ntpstats/ntpviz-man.txt")
	ctx.manpage(8, "ntpwait/ntpwait-man.txt")
	ctx.manpage(1, "ntpsweep/ntpsweep-man.txt")

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
		from waflib import Options
		Options.options.no_tests = True
