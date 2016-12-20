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
        "out": out,
        "OPT_STORE": {}
}

def help(ctx):
    "Be helpful, give a usage"
    print('''
Usage: waf <command>
    configure   Configure the project
    build       Build the project
    install     Install the project
    dist        Create a release

''')


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
#       ("check",   BuildContext)
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
#       ("info",        "cmd_info",     "Show build information / configuration.")
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

def afterparty(ctx):
    # Make magic links to support in-tree testing.
    # The idea is that all directories where the Python tools
    # listed above live should have an 'ntp' symlink so they
    # can import compiled Python modules from the build directory.
    # Also, they need to be able to see the Python extension
    # module built in libntp.
    if ctx.cmd == 'clean' or ctx.cmd == 'distclean':
        ctx.exec_command("rm -f wafhelpers/*.pyc pylib/__pycache__/*.pyc wafhelpers/__pycache__/*.pyc ntpd/version.h pylib/magic.py pylib/control.py")
    for x in ("ntpclients",):	# List used to be longer...
            path_build = ctx.bldnode.make_node("pylib")
            path_source = ctx.srcnode.make_node(x + "/ntp")
            relpath = "../" + path_build.path_from(ctx.srcnode)
            if ctx.cmd in ('install', 'build'):
                if not path_source.exists() or os.readlink(path_source.abspath()) != relpath:
                    try:
                        os.remove(path_source.abspath())
                    except OSError:
                        pass
                    os.symlink(relpath, path_source.abspath())
            elif ctx.cmd == 'clean':
                if path_source.exists():
                    #print "removing", path_source.abspath()
                    os.remove(path_source.abspath())
    bldnode = ctx.bldnode.abspath()
    if ctx.cmd in ('install', 'build'):
        os.system("ln -sf %s/libntp/ntpc.so %s/pylib/ntpc.so " % (bldnode, bldnode))

python_scripts = [
        "ntpclients/ntpdig",
        "ntpclients/ntpkeygen",
        "ntpclients/ntpmon",
        "ntpclients/ntpq",
        "ntpclients/ntpsweep",
        "ntpclients/ntptrace",
        "ntpclients/ntpviz",
        "ntpclients/ntpwait",
]

def build(ctx):
        ctx.load('waf', tooldir='wafhelpers/')
        ctx.load('bison')
        ctx.load('asciidoc', tooldir='wafhelpers/')
        ctx.load('rtems_trace', tooldir='wafhelpers/')

        if ctx.cmd == "build":
                # It's a waf gotcha that if there are object files (including
                # .pyc and .pyo files) in a source directory, compilation to
                # the build directory never happens.  This is how we foil that.
                ctx.add_pre_fun(lambda ctx: ctx.exec_command("rm -f pylib/*.py[co]"))

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
        ctx.recurse("libsodium")
        ctx.recurse("ntpd")
        ctx.recurse("ntpfrob")
        ctx.recurse("ntptime")
        ctx.recurse("pylib")
        ctx.recurse("attic")
        ctx.recurse("tests")

        scripts = ["ntpclients/ntpleapfetch"] + python_scripts

        ctx(
                features      = "subst",
                source        = scripts,
                target        = scripts,
                chmod         = Utils.O755,
                install_path  = "${PREFIX}/bin/"
        )

        ctx.add_post_fun(afterparty)
        if ctx.cmd == 'clean' or ctx.cmd == 'distclean':
            afterparty(ctx)

        ctx.manpage(8, "ntpclients/ntpleapfetch-man.txt")
        ctx.manpage(1, "ntpclients/ntpdig-man.txt")
        ctx.manpage(8, "ntpclients/ntpkeygen-man.txt")
        ctx.manpage(1, "ntpclients/ntpmon-man.txt")
        ctx.manpage(1, "ntpclients/ntpq-man.txt")
        ctx.manpage(1, "ntpclients/ntpsweep-man.txt")
        ctx.manpage(1, "ntpclients/ntptrace-man.txt")
        ctx.manpage(1, "ntpclients/ntpviz-man.txt")
        ctx.manpage(8, "ntpclients/ntpwait-man.txt")

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

        if ctx.cmd == "build":
                if not "PYTHONPATH" in os.environ:
                        print("--- PYTHONPATH is not set, "
                                "loading the Python ntp library may be troublesome ---")

#
# Miscellaneous utility productions
#

def ifdex(ctx):
    "Get a report on configuration symbols not accounted for."
    ctx.exec_command("ifdex -X build/config.h -X devel/ifdex-ignores .")

# See https://gitlab.com/esr/loccount
def loccount(ctx):
    "Report the SLOC count of the source tree."
    ctx.exec_command("loccount -x=build .")

def cxfreeze(ctx):
    "Create standalone binaries from Python scripts."
    ctx.exec_command("for prog in " + " ".join(python_scripts) + "; do cxfreeze $prog; done")

def linkcheck(ctx):
    "Report references without anchors in the documentation."
    ctx.exec_command("devel/linkcheck")

# The following sets edit modes for GNU EMACS
# Local Variables:
# mode:python
# End:
# end
