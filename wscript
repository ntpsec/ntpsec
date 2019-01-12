from __future__ import print_function

from datetime import datetime
import itertools
import os
import shlex
import sys
import time

from waflib import Build
from waflib import Context
from waflib import Scripting
from waflib import Utils
from waflib.Build import (BuildContext, CleanContext, InstallContext,
                          StepContext, ListContext)
from waflib.Context import BOTH
from waflib.Errors import WafError
from waflib.Logs import pprint
from waflib.Tools import waf_unit_test

# Avoid writing .pyc files in wafhelpers/
sys.dont_write_bytecode = True

from wafhelpers.options import options_cmd
from wafhelpers.probes import probe_header, probe_function
from wafhelpers.test import test_write_log, test_print_log


pprint.__doc__ = None

APPNAME='ntpsec'

out = "build"

config = {
    "out": out,
    "OPT_STORE": {}
}


def help(ctx):
    "Be helpful, give a usage"
    print('''
Usage: waf <command>
    build       Build the project
    check       Run tests
    configure   Configure the project
    dist        Create a release
    install     Install the project
    loccount    Show SLOC count of the source tree
    uninstall   Uninstall the project

''')


def options(ctx):
    options_cmd(ctx, config)
    ctx.recurse("pylib")


def configure(ctx):
    class oc(Build.BuildContext):
        cmd = 'oc'

        def exec_command(self, cmd, **kw):
            kw['output'] = BOTH
            try:
                err, out = self.cmd_and_log(cmd, **kw)
            except WafError as e:
                self.logger.debug('WafError')
                return e.returncode
            if (len(out) and any(word in out for word
                                 in ['err', 'err:', 'error', 'error:',
                                     'ignored', 'illegal', 'unknown',
                                     'unrecognized', 'warning'])):
                self.logger.debug('noooo %r' % out)
                return 1
            if err:
                self.logger.debug('noooo %r' % err)
                return 1
            return 0

    def msg(str):
        pprint("YELLOW", str)

    def msg_setting(name, val):
        pprint("NORMAL", "  %-30s: " % name, sep="")
        pprint("YELLOW", val)

    srcnode = ctx.srcnode.abspath()
    bldnode = ctx.bldnode.abspath()

    ctx.run_build_cls = 'check'
    ctx.load('waf', tooldir='wafhelpers/')
    ctx.load('waf_unit_test')
    ctx.load('pytest')
    ctx.load('gnu_dirs')

    with open("VERSION", "r") as f:
        ntpsec_release = f.read().split(" ")[0].strip()

    ctx.env.OPT_STORE = config["OPT_STORE"]

    opt_map = {}
    # Wipe out and override flags with those from the commandline
    for flag in ctx.env.OPT_STORE:
        if flag == "--undefine":
            for sym in ctx.env.OPT_STORE[flag]:
                ctx.undefine(sym)
        elif flag == "--define":
            for symval in ctx.env.OPT_STORE[flag]:
                (sym, val) = symval.split("=")
                try:
                    ctx.define(sym, int(val))
                except ValueError:
                    ctx.define(sym, val)
        else:
            opt = flag.replace("--", "").upper()
            opt_map[opt] = ctx.env.OPT_STORE[flag]

    msg("--- Configuring host ---")
    ctx.setenv('host', ctx.env.derive())

    ctx.load('compiler_c')
    ctx.start_msg('Checking compiler version')
    ctx.end_msg("%s" % ".".join(ctx.env.CC_VERSION))
    # Ensure m4 is present, or bison will fail with SIGPIPE
    ctx.find_program('m4')
    ctx.load('bison')

    for opt in opt_map:
        ctx.env[opt] = opt_map[opt]

    if ctx.options.enable_rtems_trace:
        ctx.find_program("rtems-tld", var="BIN_RTEMS_TLD",
                         path_list=[ctx.options.rtems_trace_path,
                                    '${BINDIR}'])
        ctx.env.RTEMS_TEST_ENABLE = True
        ctx.env.RTEMS_TEST_FLAGS = [
            "-C", "%s/devel/trace/ntpsec-trace.ini" % srcnode,
            "-W", "%s/ntpsec-wrapper" % bldnode,
            "-P", "%s/devel/trace/" % srcnode,
            "-f", "-I%s" % bldnode,
            "-f", "-I%s/include/" % srcnode,
        ]

    # Not needed to build.  Used by utility scripts.
    ctx.find_program("awk", var="BIN_AWK", mandatory=False)
    ctx.find_program("sh", var="BIN_SH", mandatory=False)

    # used to make man and html pages
    ctx.find_program("asciidoc", var="BIN_ASCIIDOC", mandatory=False)
    # make sure asciidoc is new enough.
    # based on check_python_version() from waf
    if ctx.env.BIN_ASCIIDOC:
        # https://lists.ntpsec.org/pipermail/devel/2016-July/001778.html
        asciidocminver = (8, 6, 0)
        # Get asciidoc version string
        cmd = ctx.env.BIN_ASCIIDOC + ['--version']
        # example output: asciidoc 8.6.9
        lines = ctx.cmd_and_log(cmd).split()[1].split(".")
        assert len(lines) == 3, "found %r lines, expected 3: %r" \
            % (len(lines), lines)
        asciidocver_tuple = (int(lines[0]), int(lines[1]), int(lines[2]))

        # Compare asciidoc version with the minimum required
        result = (asciidocver_tuple >= asciidocminver)

        asciidocver_full = '.'.join(map(str, asciidocver_tuple[:3]))
        asciidocminver_str = '.'.join(map(str, asciidocminver))
        ctx.msg('Checking for asciidoc version >= %s' % (asciidocminver_str,),
                asciidocver_full, color=result and 'GREEN' or 'YELLOW')

        if not result:
            del ctx.env.BIN_ASCIIDOC
    ctx.find_program("a2x", var="BIN_A2X", mandatory=False)
    ctx.find_program("xsltproc", var="BIN_XSLTPROC", mandatory=False)

    ctx.env.ENABLE_DOC = False
    if ctx.env.BIN_ASCIIDOC and ctx.env.BIN_XSLTPROC and ctx.env.BIN_A2X:
        ctx.env.ENABLE_DOC = True

    if ctx.options.enable_doc and not ctx.env.ENABLE_DOC:
        ctx.fatal("asciidoc and xsltproc are required in order "
                  "to build documentation")
    elif ctx.options.enable_doc:
        ctx.env.ASCIIDOC_FLAGS = []
        ctx.env.ENABLE_DOC_USER = ctx.options.enable_doc

    # XXX: conditionally build this with --disable-man?
    # Should it build without docs enabled?
    ctx.env.A2X_FLAGS = ["--format", "manpage"]
    if not ctx.options.enable_a2x_xmllint:
        ctx.env.A2X_FLAGS += ["--no-xmllint"]

    # Disable manpages within build()
    if ctx.options.disable_manpage:
        ctx.env.DISABLE_MANPAGE = True

    source_date_epoch = os.getenv('SOURCE_DATE_EPOCH', None)
    if ctx.options.build_epoch is not None:
        build_epoch = ctx.options.build_epoch
        ctx.define("BUILD_EPOCH", build_epoch, comment="Using --build-epoch")
    elif source_date_epoch:
        if not source_date_epoch.isdigit():
            ctx.fatal("ERROR: malformed SOURCE_DATE_EPOCH")
        build_epoch = int(source_date_epoch)
        ctx.define("BUILD_EPOCH", build_epoch, comment="Using SOURCE_DATE_EPOCH")
    else:
        build_epoch = int(time.time())
        ctx.define("BUILD_EPOCH", build_epoch, comment="Using default")

    build_epoch_formatted = datetime.utcfromtimestamp(build_epoch).strftime("%Y-%m-%dT%H:%M:%SZ")
    if ((os.path.exists(".git") and
            ctx.find_program("git", var="BIN_GIT", mandatory=False))):
        cmd = ctx.env.BIN_GIT + shlex.split("log -1 --format=%h")
        git_short_hash = ctx.cmd_and_log(cmd).strip()

        ctx.env.NTPSEC_VERSION = "%s+" % ntpsec_release
        ctx.env.NTPSEC_VERSION_EXTENDED = "%s+ %s (git rev %s)" % (ntpsec_release, build_epoch_formatted, git_short_hash)
    else:
        ctx.env.NTPSEC_VERSION = "%s" % ntpsec_release
        ctx.env.NTPSEC_VERSION_EXTENDED = "%s %s" % (ntpsec_release, build_epoch_formatted)
    ctx.define("NTPSEC_VERSION", ctx.env.NTPSEC_VERSION)
    ctx.define("NTPSEC_VERSION_EXTENDED", ctx.env.NTPSEC_VERSION_EXTENDED)

    # We require some things that C99 doesn't enable, like pthreads.
    # These flags get propagated to both the host and main parts of the build.
    #
    # _POSIX_C_SOURCE
    #      If ==1, like _POSIX_SOURCE;
    #      if >=2 add IEEE Std 1003.2;
    #      if >=199309L, add IEEE Std 1003.1b-1993;
    #      if >=199506L, add IEEE Std 1003.1c-1995;
    #      if >=200112L, all of IEEE 1003.1-2004
    #      if >=200809L, all of IEEE 1003.1-2008
    #
    # FIXME: We'd like this to be -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=600
    # rather than -D_GNU_SOURCE, but that runs into problems in two places:
    # (1) The ISC net handling stuff, where struct in6_addr’ loses a member
    # named s6_addr32 that the macros need, and (2) three BSD functions
    # related to chroot jailing in the sandbox code.
    #
    # Note that _POSIX_C_SOURCE >= 199506L and _GNU_SOURCE both turn on
    # _POSIX_PTHREAD_SEMANTICS and _REENTRANT
    #
    ctx.env.CFLAGS = ["-std=c99", "-D_GNU_SOURCE"] + ctx.env.CFLAGS

    msg("--- Configuring main ---")
    ctx.setenv("main", ctx.env.derive())

    from wafhelpers.check_sizeof import check_sizeof

    for opt in opt_map:
        ctx.env[opt] = opt_map[opt]

    if ctx.options.cross_compiler:
        ctx.env.ENABLE_CROSS = True

        ctx.start_msg("Using Cross compiler CC:")
#           ctx.get_cc_version(ctx.env.CC, gcc=True)
        ctx.end_msg(ctx.options.cross_compiler)

        ctx.env.CC = shlex.split(ctx.options.cross_compiler)
        ctx.env.LINK_CC = shlex.split(ctx.options.cross_compiler)

        if ctx.env["CROSS-CFLAGS"]:
            # Lexically split each part of the CFLAGS, then chain the lists
            iter = [shlex.split(x) for x in opt_map["CROSS-CFLAGS"]]
            ctx.env.CFLAGS = list(itertools.chain.from_iterable(iter))

        if ctx.env["CROSS-LDFLAGS"]:
            # Lexically split each part of the LDFLAGS, then chain the lists
            iter = [shlex.split(x) for x in opt_map["CROSS-LDFLAGS"]]
            ctx.env.LDFLAGS = list(itertools.chain.from_iterable(iter))

    if ctx.options.list:
        from wafhelpers.refclock import refclock_map
        print("ID    Description")
        print("~~    ~~~~~~~~~~~")
        for id in refclock_map:
            print("%-5s %s" % (id, refclock_map[id]["descr"]))

        return

    # These are required by various refclocks
    # needs to be tested before CFLAGS are set
    if ctx.check_endianness() == "big":
        ctx.define("WORDS_BIGENDIAN", 1)

    if ctx.options.enable_leap_testing:
        ctx.define("ENABLE_LEAP_TESTING", 1,
                   comment="Enable leap seconds on other than 1st of month.")

    # check for some libs first.  some options, like stack protector,
    # may depend on some libs, like -lssp
    ctx.check_cc(lib="m", comment="Math library")
    ctx.check_cc(lib="rt", mandatory=False, comment="realtime library")
    ctx.check_cc(lib="execinfo", mandatory=False,
                 comment="BSD backtrace library")
    ret = ctx.check_cc(lib="bsd", mandatory=False,
                       comment="BSD compatibility library")
    if ret:
        ctx.env.LDFLAGS += ["-lbsd"]

    # -lssp and -lssp_nonshared may be needed by older gcc to
    # support "-fstack-protector-all"
    ret = ctx.check_cc(lib="ssp", mandatory=False,
                       comment="libssp")
    if ret:
        ctx.env.LDFLAGS += ["-lssp"]

    ret = ctx.check_cc(lib="ssp_nonshared", mandatory=False,
                       comment="libssp_nonshared")
    if ret:
        ctx.env.LDFLAGS += ["-lssp_nonshared"]

    cc_test_flags = [
        ('f_stack_protector_all', '-fstack-protector-all'),
        ('PIC', '-fPIC'),
        ('PIE', '-pie -fPIE'),
        # this quiets most of macOS warnings on -fpie
        ('unused', '-Qunused-arguments'),
        # This is a useless warning on any architecture with a barrel
        # shifter, which includes Intel and ARM and basically
        # everything nowadays. Even so, we'd enable it out of
        # perfectionism, but the GCC directives that ought to be
        # useful for forcing structure alignment in order to suppress
        # it locally don't seem to be working quite right.
        #('w_cast_align', "-Wcast-align"),
        ('w_cast_qual', "-Wcast-qual"),
        ('w_disabled_optimization', "-Wdisabled-optimization"),
        ('w_float_equal', "-Wfloat-equal"),
        ('w_format', '-Wformat'),
        ('w_format_security', '-Wformat-security'),
        # fails on OpenBSD 6
        ('w_format_signedness', '-Wformat-signedness'),
        ('w_implicit_function_declaration', "-Wimplicit-function-declaration"),
        ('w_init_self', '-Winit-self'),
        ('w_invalid_pch', '-Winvalid-pch'),
        ('w_missing_declarations', '-Wmissing-declarations'),
        ('w_multichar', '-Wmultichar'),
        ('w_packed', '-Wpacked'),
        ('w_pointer_arith', '-Wpointer-arith'),
        ('w_shadow', '-Wshadow'),
        # fails on clang
        ('w_suggest_attribute_noreturn', "-Wsuggest-attribute=noreturn"),
        ('w_write_strings', '-Wwrite-strings'),
        ]

    # Check which linker flags are supported
    ld_hardening_flags = [
        ("z_now", "-Wl,-z,now"),     # no deferred symbol resolution
    ]

    # we prepend our options to CFLAGS, this allows user provided
    # CFLAGS to override our computed CFLAGS
    if ctx.options.enable_debug_gdb:
        ctx.env.CFLAGS = ["-g"] + ctx.env.CFLAGS
        ctx.define("USEBACKTRACE", "1", quote=False)
    else:
        # not gdb debugging
        cc_test_flags += [
            ('LTO', '-flto'),                   # link time optimization
            ]
        ld_hardening_flags += [
            ('stripall', "-Wl,--strip-all"),    # Strip binaries
            ]

    if ctx.options.enable_debug:
        ctx.define("DEBUG", 1, comment="Enable debug mode")
        ctx.env.BISONFLAGS += ["--debug"]

    if ctx.options.enable_warnings:
        # turn on some annoying warnings
        ctx.env.CFLAGS = [
            # "-Wall",                # for masochists
            # "-Waggregate-return",   # breaks ldiv(), ntpcal_daysplit(),  etc.
            # "-Wcast-align",         # fails on RasPi, needs fixing.
            # "-Wbad-function-cast",  # ntpd casts long<->double a lot
            # "-Wformat-nonliteral",  # complains about a used feature
            "-Winline",               # some OS have inline issues.
            # "-Wmissing-format-attribute", # false positives
            # "-Wnested-externs",     # incompatible w/ Unity...
            # "-Wpadded",             # duck... over 3k warnings
            # "-Wredundant-decls",    # incompatible w/ Unity
            "-Wswitch-default",       # warns on Bison bug
        ] + ctx.env.CFLAGS
        cc_test_flags += [
            ('w_implicit_fallthru', "-Wimplicit-fallthrough=3"),
            # Fails on Solaris, OpenBSD 6, and RasPi
            # Complains about a Bison bug
            # Cannot be suppressed
            # ('w_sign_conversion', "-Wsign-conversion"),
            # fails on clang, lots of false positives and Unity complaints
            # ('w_suggest_attribute_const', "-Wsuggest-attribute=const"),
            # fails on clang, lot's of false positives and Unity complaints
            # ('w_suggest_attribute_pure', "-Wsuggest-attribute=pure"),
            ]

    ctx.env.CFLAGS = [
        # -O1 will turn on -D_FORTIFY_SOURCE=2 for us
        "-O1",
        "-Wall",
        "-Wextra",
        "-Wmissing-prototypes",
        "-Wstrict-prototypes",
        "-Wundef",
        "-Wunused",
        ] + ctx.env.CFLAGS

    # gotta be tricky to test for -Wsuggest-attribute=const
    FRAGMENT = '''
int tmp;
int main(int argc, char **argv) {
        (void)argc; (void)argv;
        tmp = argc;
        return argc;
}
'''

    # check if C compiler supports some flags
    old_run_build_cls = ctx.run_build_cls
    ctx.run_build_cls = 'oc'
    for (name, ccflag) in cc_test_flags:
        ctx.check(cflags=ccflag,
                  define_name='HAS_' + name,
                  fragment=FRAGMENT,
                  mandatory=False,
                  msg='Checking if C compiler supports ' + ccflag,
                  run_build_cls='oc')

    ctx.run_build_cls = old_run_build_cls

    if ctx.env.HAS_PIC:
        ctx.env.CFLAGS = ["-fPIC"] + ctx.env.CFLAGS

    if ctx.env.HAS_PIE:
        ctx.env.LINKFLAGS_NTPD += [
            "-pie",
            ]
        ctx.env.CFLAGS_bin = ["-fPIE", "-pie"] + ctx.env.CFLAGS
        ld_hardening_flags += [
            ('relro', "-Wl,-z,relro"),  # hardening, marks some read only,
            ]

    if ctx.env.HAS_unused:
        ctx.env.CFLAGS = ['-Qunused-arguments'] + ctx.env.CFLAGS

    # XXX: -flto currently breaks link of ntpd
    if ctx.env.HAS_LTO and False:
        ctx.env.CFLAGS = ["-flto"] + ctx.env.CFLAGS

    # debug warnings that are not available with all compilers
    if ctx.env.HAS_w_implicit_fallthru:
        ctx.env.CFLAGS = ['-Wimplicit-fallthrough=3'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_suggest_attribute_const:
        ctx.env.CFLAGS = ['-Wsuggest-attribute=const'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_suggest_attribute_noreturn:
        ctx.env.CFLAGS = ['-Wsuggest-attribute=noreturn'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_suggest_attribute_pure:
        ctx.env.CFLAGS = ['-Wsuggest-attribute=pure'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_format_security:
        ctx.env.CFLAGS = ['-Wformat-security'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_format_signedness:
        ctx.env.CFLAGS = ['-Wformat-signedness'] + ctx.env.CFLAGS
    # should be before other -Wformat-* in CFLAGS
    if ctx.env.HAS_w_format:
        ctx.env.CFLAGS = ['-Wformat'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_float_equal:
        ctx.env.CFLAGS = ['-Wfloat-equal'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_init_self:
        ctx.env.CFLAGS = ['-Winit-self'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_write_strings:
        ctx.env.CFLAGS = ['-Wwrite-strings'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_pointer_arith:
        ctx.env.CFLAGS = ['-Wpointer-arith'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_invalid_pch:
        ctx.env.CFLAGS = ['-Winvalid-pch'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_implicit_function_declaration:
        ctx.env.CFLAGS = ['-Wimplicit-function-declaration'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_disabled_optimization:
        ctx.env.CFLAGS = ['-Wdisabled-optimization'] + ctx.env.CFLAGS
    #if ctx.env.HAS_w_cast_align:
    #    ctx.env.CFLAGS = ['-Wcast-align'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_missing_declarations:
        ctx.env.CFLAGS = ['-Wmissing-declarations'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_cast_qual:
        ctx.env.CFLAGS = ['-Wcast-qual'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_packed:
        ctx.env.CFLAGS = ['-Wpacked'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_shadow:
        ctx.env.CFLAGS = ['-Wshadow'] + ctx.env.CFLAGS
    #if ctx.env.HAS_w_sign_conversion:
    #    ctx.env.CFLAGS = ['-Wsign-conversion'] + ctx.env.CFLAGS
    if ctx.env.HAS_f_stack_protector_all:
        ctx.env.CFLAGS = ['-fstack-protector-all'] + ctx.env.CFLAGS

    # old gcc takes -z,relro, but then barfs if -fPIE available and used.
    # ("relro", "-Wl,-z,relro"), # marks some sections read only
    old_run_build_cls = ctx.run_build_cls
    ctx.run_build_cls = 'oc'
    for (name, ldflag) in ld_hardening_flags:
        ctx.check(define_name='HAS_' + name,
                  fragment=FRAGMENT,
                  ldflags=ldflag,
                  mandatory=False,
                  msg='Checking if linker supports ' + ldflag,
                  run_build_cls='oc')
        if ctx.env['HAS_' + name]:
            ctx.env.LDFLAGS += [ldflag]

    ctx.run_build_cls = old_run_build_cls

    if ctx.env.CC_NAME == "sun":
        # we are sun, placeholder
        ctx.env.CFLAGS += []
    elif ctx.env.CC_NAME == "clang":
        # used on macOS, FreeBSD,
        # FORTIFY needs LTO to work well
        if ctx.env.DEST_OS not in ["darwin", "freebsd"]:
            # -flto and friends breaks tests on macOS
            #ctx.env.CFLAGS = [
            #    "-flto"
            #    "-fsanitize=cfi",           # hardening
            #    "-fsanitize=safe-stack",    # hardening
            #    ] + ctx.env.CFLAGS
            ctx.env.LDFLAGS += [
                "-Wl,-z,relro",  # hardening, marks some section read only,
                ]
    # else:  # gcc, probably

    # Exclude Unity's support for printing floating point numbers since it triggers warnings
    # with -Wfloat-equal
    ctx.env.CFLAGS = ['-DUNITY_EXCLUDE_FLOAT_PRINT'] + ctx.env.CFLAGS

    # XXX: hack
    if ctx.env.DEST_OS in ["freebsd", "openbsd"]:
        ctx.env.PLATFORM_INCLUDES = ["/usr/local/include"]
        ctx.env.PLATFORM_LIBPATH = ["/usr/local/lib"]
    elif ctx.env.DEST_OS == "netbsd":
        ctx.env.PLATFORM_INCLUDES = ["/usr/pkg/include"]
        ctx.env.PLATFORM_LIBPATH = ["/usr/lib", "/usr/pkg/lib"]
    elif ctx.env.DEST_OS == "win32":
        ctx.load("msvc")
    elif ctx.env.DEST_OS == "darwin":
        # macports location
        if os.path.isdir("/opt/local/include"):
            ctx.env.PLATFORM_INCLUDES = ["/opt/local/include"]
        if os.path.isdir("/opt/local/lib"):
            ctx.env.PLATFORM_LIBPATH = ["/opt/local/lib"]
        # OS X needs this for IPv6
        ctx.define("__APPLE_USE_RFC_3542", 1,
                   comment="Needed for IPv6 support")
    elif ctx.env.DEST_OS == "sunos":
        # Declare compatibility with the POSIX.1-2001 standard, and any
        # headers/interfaces not in conflict with that standard
        ctx.define("_POSIX_C_SOURCE", "200112L", quote=False)
        ctx.define("__EXTENSIONS__", "1", quote=False)

    structures = (
        ("struct if_laddrconf", ["sys/types.h", "net/if6.h"], False),
        ("struct if_laddrreq", ["sys/types.h", "net/if6.h"], False),
        ("struct timex", ["sys/time.h", "sys/timex.h"], True),
        ("struct ntptimeval", ["sys/time.h", "sys/timex.h"], False),
    )
    for (s, h, r) in structures:
        ctx.check_cc(type_name=s, header_name=h, mandatory=r)

    # waf's SNIP_FIELD should likely include this header itself
    # This is needed on some systems to get size_t for following checks
    ctx.check_cc(auto_add_header_name=True,
                 header_name="stddef.h",
                 define_name="",           # omit from config.h
                 mandatory=False)

    structure_fields = (
        ("struct timex", "time_tick", ["sys/time.h", "sys/timex.h"]),
        ("struct timex", "modes", ["sys/time.h", "sys/timex.h"]),
        ("struct ntptimeval", "time.tv_nsec", ["sys/time.h", "sys/timex.h"]),
        ("struct ntptimeval", "tai", ["sys/time.h", "sys/timex.h"]),
        # first in glibc 2.12
    )
    for (s, f, h) in structure_fields:
        ctx.check_cc(type_name=s, field_name=f, header_name=h, mandatory=False)

    # mostly used by timetoa.h and timespecops.h
    sizeofs = [
        ("time.h",      "time_t"),
        (None,          "long"),
    ]

    for header, sizeof in sorted(sizeofs, key=lambda x: x[1:]):
        check_sizeof(ctx, header, sizeof)

    # Check via pkg-config first, then fall back to a direct search
    if not ctx.check_cfg(
        package='libcrypto', uselib_store='CRYPTO',
        args=['libcrypto', '--cflags', '--libs'],
        msg="Checking for OpenSSL (via pkg-config)",
        define_name='', mandatory=False,
    ):
        ctx.check_cc(msg="Checking for OpenSSL's crypto library",
                     lib="crypto", mandatory=True)

    # Optional functions.  Do all function checks here, otherwise
    # we're likely to duplicate them.
    optional_functions = (
        ('_Unwind_Backtrace', ["unwind.h"]),
        ('adjtimex', ["sys/time.h", "sys/timex.h"]),
        ('backtrace_symbols_fd', ["execinfo.h"]),
        ('closefrom', ["stdlib.h"]),
        ('ntp_adjtime', ["sys/time.h", "sys/timex.h"]),     # BSD
        ('ntp_gettime', ["sys/time.h", "sys/timex.h"]),     # BSD
        ('res_init', ["netinet/in.h", "arpa/nameser.h", "resolv.h"]),
        ('sched_setscheduler', ["sched.h"]),
        ('strlcpy', ["string.h"]),
        ('strlcat', ["string.h"]) 
    )
    for ft in optional_functions:
            probe_function(ctx, function=ft[0], prerequisites=ft[1])

    # This area is still work in progress
    # Need to disable making symbols
    #   but not until killing off HAVE_TIMER_CREATE

    # Sanity checks to give a sensible error message
    required_functions = (
        # MacOS doesn't have timer_create ??
        ('timer_create', ["signal.h", "time.h"], "RT", False),
        # Very old versions of OpenSSL don't have cmac.h
        #  We could add ifdefs, but old crypto is deprecated in favor of CMAC
        #  and so far, all the systems that we want to support are new enough.
        ('CMAC_CTX_new', ["openssl/cmac.h"], "CRYPTO", True) )
    for ft in required_functions:
            probe_function(ctx, function=ft[0],
                prerequisites=ft[1], use=ft[2],
                mandatory=ft[3])



    # check for BSD versions outside of libc
    if not ctx.get_define("HAVE_STRLCAT"):
        ret = probe_function(ctx, function='strlcat',
                    prerequisites=['bsd/string.h'])
        if ret:
            ctx.define("HAVE_STRLCAT", 1, comment="Using bsd/strlcat")

    if not ctx.get_define("HAVE_STRLCPY"):
        ret = probe_function(ctx, function='strlcpy',
                    prerequisites=['bsd/string.h'])
        if ret:
            ctx.define("HAVE_STRLCPY", 1, comment="Using bsd/strlcpy")

    # Nobody uses the symbol, but this seems like a good sanity check.
    ctx.check_cc(header_name="stdbool.h", mandatory=True,
                 comment="Sanity check.")

    # This is a list of every optional include header in the
    # codebase that is guarded by a directly corresponding HAVE_*_H symbol.
    #
    # In some cases one HAVE symbol controls inclusion of more
    # than one header.  In these cases only the one header name
    # matching the pattern of the HAVE_*_H symbol name is listed
    # here, so we can invert the relationship to generate tests
    # for all the symbols.
    #
    # Some of these are cruft from ancient big-iron systems and should
    # be removed.
    optional_headers = (
        "alloca.h",
        ("arpa/nameser.h", ["sys/types.h"]),
        "bsd/string.h",     # bsd emulation
        ("ifaddrs.h", ["sys/types.h"]),
        ("linux/if_addr.h", ["sys/socket.h"]),
        ("linux/rtnetlink.h", ["sys/socket.h"]),
        "linux/serial.h",
        "net/if6.h",
        ("net/route.h", ["sys/types.h", "sys/socket.h", "net/if.h"]),
        "netinfo/ni.h",     # Apple
        "priv.h",           # Solaris
        "stdatomic.h",
        "sys/clockctl.h",   # NetBSD
        "sys/ioctl.h",
        "sys/modem.h",      # Apple
        "sys/sockio.h",
        ("sys/sysctl.h", ["sys/types.h"]),
        ("timepps.h", ["inttypes.h"]),
        ("sys/timepps.h", ["inttypes.h", "sys/time.h"]),
        ("sys/timex.h", ["sys/time.h"]),
    )
    for hdr in optional_headers:
        if isinstance(hdr, str):
            if ctx.check_cc(header_name=hdr, mandatory=False,
                            comment="<%s> header" % hdr):
                continue
        else:
            (hdr, prereqs) = hdr
            if probe_header(ctx, hdr, prereqs):
                continue
        if os.path.exists("/usr/include/" + hdr):
            # Sanity check...
            print("Compilation check failed but include exists %s" % hdr)

    if ((ctx.get_define("HAVE_TIMEPPS_H") or
            ctx.get_define("HAVE_SYS_TIMEPPS_H"))):
        ctx.define("HAVE_PPSAPI", 1, comment="Enable the PPS API")

    # Check for Solaris capabilities
    if ctx.get_define("HAVE_PRIV_H") and ctx.env.DEST_OS == "sunos":
        ctx.define("HAVE_SOLARIS_PRIVS", 1,
                   comment="Enable Solaris Privileges (Solaris only)")

    from wafhelpers.check_sockaddr import check_sockaddr
    check_sockaddr(ctx)

    # Check for Solaris's service configuration facility library
    ctx.check_cc(header_name="libscf.h", lib="scf", mandatory=False,
                 uselib_store="SCF")

    # Some systems don't have sys/timex.h eg OS X, OpenBSD...
    if ctx.get_define("HAVE_SYS_TIMEX_H"):
        ctx.env.HEADER_SYS_TIMEX_H = True

    if ctx.options.refclocks:
        from wafhelpers.refclock import refclock_config
        refclock_config(ctx)

    # NetBSD (used to) need to recreate sockets on changed routing.
    # Perhaps it still does. If so, this should be set.  The autoconf
    # build set it "if the OS clears cached routes when more specifics
    # become available".
    # ctx.define("OS_MISSES_SPECIFIC_ROUTE_UPDATES", 1)

    if ctx.options.enable_leap_smear:
        ctx.define("ENABLE_LEAP_SMEAR", 1,
                   comment="Enable experimental leap smearing code")

    if ctx.options.enable_mssntp:
        ctx.define("ENABLE_MSSNTP", 1,
                   comment="Enable MS-SNTP extensions "
                   " https://msdn.microsoft.com/en-us/library/cc212930.aspx")

    if ctx.options.enable_lockclock:
        if ctx.env.REFCLOCK_LOCAL:
            ctx.define("ENABLE_LOCKCLOCK", 1,
                       comment="Enable NIST 'lockclock'")
        else:
            import waflib.Errors
            raise waflib.Errors.WafError(
                "NIST 'lockclock' requires refclock 'local'")

    if not ctx.options.disable_droproot:
        ctx.define("ENABLE_DROPROOT", 1,
                   comment="Drop root after initialising")
    if ctx.options.enable_early_droproot:
        ctx.define("ENABLE_EARLY_DROPROOT", 1,
                   comment="Enable early drop root")

    if not ctx.options.disable_dns_lookup:
        ctx.define("ENABLE_DNS_LOOKUP", 1,
                   comment="Enable DNS lookup of hostnames")

    # This is true under every Unix-like OS.
    ctx.define("HAVE_WORKING_FORK", 1,
               comment="Whether a working fork() exists")

    # SO_REUSEADDR socket option is needed to open a socket on an
    # interface when the port number is already in use on another
    # interface. Linux needs this, NetBSD does not, status on
    # other platforms is unknown.  It is probably harmless to
    # have it on everywhere.
    ctx.define("NEED_REUSEADDR_FOR_IFADDRBIND", 1,
               comment="Whether SO_REUSEADDR is needed to open "
               "same sockets on alternate interfaces, required "
               "by Linux at least")

    from wafhelpers.check_vsprintfm import check_vsprintfm
    check_vsprintfm(ctx)

    # Check for directory separator
    if ctx.env.DEST_OS == "win32":
        sep = "\\"
    else:
        sep = "/"

    ctx.define("DIR_SEP", "'%s'" % sep, quote=False,
               comment="Directory separator used")

    if ctx.get_define("HAVE_SYS_SYSCTL_H"):
        ctx.define("HAVE_IFLIST_SYSCTL", 1,
                   comment="Whether sysctl interface exists")

    # Header/library checks

    if not ctx.options.disable_droproot and ctx.env.DEST_OS == "linux":
        ctx.check_cc(header_name="sys/prctl.h", mandatory=False)
        ctx.check_cc(header_name="sys/capability.h", mandatory=False)
        ctx.check_cc(lib="cap", comment="Capability library", mandatory=False)

        if ((ctx.get_define("HAVE_SYS_CAPABILITY_H") and
                ctx.get_define("HAVE_SYS_PRCTL_H") and ctx.env.LIB_CAP)):
            ctx.define("HAVE_LINUX_CAPABILITY", 1)

    if ctx.options.enable_seccomp:
        if ctx.env.DEST_OS != "linux":
            ctx.fatal("seccomp is only supported on Linux")

        # Check via pkg-config first, then fall back to a direct search
        if not ctx.check_cfg(
            package='libseccomp', args=['--libs', '--cflags'],
            uselib_store='SECCOMP', define_name='HAVE_SECCOMP_H',
            mandatory=False
        ):
            ctx.check_cc(header_name="seccomp.h")
            ctx.check_cc(lib="seccomp")

    from wafhelpers.check_pthread import check_pthread_header_lib
    check_pthread_header_lib(ctx)

    if not ctx.options.disable_mdns_registration:
        ctx.check_cc(header_name="dns_sd.h", lib="dns_sd", mandatory=False,
                     uselib_store="DNS_SD")

    if not ctx.options.disable_dns_lookup:
        from wafhelpers.check_pthread import check_pthread_run
        check_pthread_run(ctx)

    # Solaris needs -lsocket and -lnsl for socket code
    if ctx.env.DEST_OS == "sunos":
        ctx.check(features="c cshlib", lib="socket", mandatory=False)
        ctx.check(features="c cshlib", lib="nsl", mandatory=False)

    if ctx.options.enable_classic_mode:
        ctx.define("ENABLE_CLASSIC_MODE", 1)
    else:
        ctx.undefine("ENABLE_CLASSIC_MODE")

    # Ugly hack to examine config symbols
    for sym in ctx.env.DEFINES:
        if sym.startswith("NTP_SIZEOF_TIME_T="):
            timesize = int(sym.split("=")[1])
            if timesize < 8:
                msg("WARNING: This system has a 32-bit time_t.")
                msg("WARNING: Your ntpd will fail on 2038-01-19T03:14:07Z.")

    # before write_config()
    droproot_type = ""
    if ctx.is_defined("HAVE_LINUX_CAPABILITY"):
        droproot_type = "Linux"
    elif ctx.is_defined("HAVE_SOLARIS_PRIVS"):
        droproot_type = "Solaris"
    elif ctx.is_defined("HAVE_SYS_CLOCKCTL_H"):
        droproot_type = "NetBSD"
    else:
        droproot_type = "None"

    # write_config() removes symbols
    ctx.start_msg("Writing configuration header:")
    ctx.write_config_header("config.h")
    ctx.end_msg("config.h", "PINK")

    def yesno(x):
        if x:
            return "Yes"
        return "No"

    msg("")
    msg("Build Options")
    msg_setting("CC", " ".join(ctx.env.CC))
    msg_setting("CFLAGS", " ".join(ctx.env.CFLAGS))
    msg_setting("LDFLAGS", " ".join(ctx.env.LDFLAGS))
    msg_setting("LINKFLAGS_NTPD", " ".join(ctx.env.LINKFLAGS_NTPD))
    msg_setting("PREFIX", ctx.env.PREFIX)
    msg_setting("Droproot Support", droproot_type)
    msg_setting("Debug Support", yesno(ctx.options.enable_debug))
    msg_setting("Refclocks", ", ".join(sorted(ctx.env.REFCLOCK_LIST)))
    msg_setting("Build Manpages",
                yesno(ctx.env.ENABLE_DOC and not ctx.env.DISABLE_MANPAGE))

    ctx.recurse("pylib")
    # Convert the Python directories to absolute paths.
    # This makes them behave the same as PREFIX.
    ctx.env.PYTHONDIR = os.path.abspath(ctx.env.PYTHONDIR)
    ctx.env.PYTHONARCHDIR = os.path.abspath(ctx.env.PYTHONARCHDIR)
    msg_setting("PYTHONDIR", ctx.env.PYTHONDIR)
    msg_setting("PYTHONARCHDIR", ctx.env.PYTHONARCHDIR)


class check(BuildContext):
    cmd = 'check'
    variant = "main"


def bin_test(ctx):
    """Run binary check, use after tests."""
    from wafhelpers.bin_test import cmd_bin_test
    cmd_bin_test(ctx, config)

def bin_test_summary(ctx):
    """Display results of binary check, use after tests."""
    from wafhelpers.bin_test import bin_test_summary
    bin_test_summary(ctx)

# Borrowed from https://www.rtems.org/
variant_cmd = (
    ("build", BuildContext),
    ("clean", CleanContext),
    ("install", InstallContext),
    ("step", StepContext),
    ("list", ListContext),
    # ("check", BuildContext)
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
    ("install", "init_handler", None),
    ("uninstall", "init_handler", None),
    ("build", "init_handler", None),
    ("clean", "init_handler", None),
    ("list", "init_handler", None),
    ("step", "init_handler", None),
)


for command, func, descr in commands:
    class tmp1(Context.Context):
        if descr:
            __doc__ = descr
        cmd = command
        fun = func
        if ((command in
            'install uninstall build clean list step'
             )):
            execute = Scripting.autoconfigure(Context.Context.execute)
# end borrowed code


def afterparty(ctx):
    # Make magic links to support in-tree testing.
    #
    # The idea is that all directories where the Python tools live should
    # have an 'ntp' symlink so they can import Python modules from the pylib
    # directory.
    #
    # Note that this setup is applied to the build tree, not the
    # source tree.  Only the build-tree copies of the programs are
    # expected to work.
    for x in ("ntpclients", "tests/pylib",):
        # List used to be longer...
        path_build = ctx.bldnode.make_node("pylib")
        path_source = ctx.bldnode.make_node(x + "/ntp")
        relpath = ("../" * (x.count("/")+1)) + path_build.path_from(ctx.bldnode)
        if ctx.cmd in ('install', 'build'):
            if ((not path_source.exists() or
                    os.readlink(path_source.abspath()) != relpath)):
                try:
                    os.remove(path_source.abspath())
                except OSError:
                    pass
                os.symlink(relpath, path_source.abspath())


python_scripts = [
    "ntpclients/ntpdig.py",
    "ntpclients/ntpkeygen.py",
    "ntpclients/ntpq.py",
    "ntpclients/ntpsweep.py",
    "ntpclients/ntptrace.py",
    "ntpclients/ntpviz.py",
    "ntpclients/ntpwait.py",
    "ntpclients/ntplogtemp.py",
    "ntpclients/ntpsnmpd.py",
]


def build(ctx):
    from waflib.Logs import verbose
    ctx.load('waf', tooldir='wafhelpers/')
    ctx.load('asciidoc', tooldir='wafhelpers/')
    ctx.load('rtems_trace', tooldir='wafhelpers/')

    if ctx.cmd == "build":
        # It's a waf gotcha that if there are object files (including
        # .pyc and .pyo files) in a source directory, compilation to
        # the build directory never happens.  This is how we foil that.
        ctx.add_pre_fun(lambda ctx: ctx.exec_command("rm -f pylib/*.py[co]"))

    if verbose > 0:  # Pass Verbosity to asciidoc and a2x
        ctx.env.A2X_FLAGS += ["-v"]
        ctx.env.ASCIIDOC_FLAGS += ["-v"]

    if ctx.env.ENABLE_DOC_USER:
        if ctx.variant != "main":
            ctx.recurse("docs")

    if ctx.variant == "host":
        ctx.recurse("ntpd")
        return

    if ctx.env.REFCLOCK_GENERIC or ctx.env.REFCLOCK_TRIMBLE:
        # required by the generic and Trimble refclocks
        ctx.recurse("libparse")
    ctx.recurse("libntp")
    ctx.recurse("ntpd")
    ctx.recurse("ntpfrob")
    ctx.recurse("ntptime")
    ctx.recurse("pylib")
    ctx.recurse("attic")
    ctx.recurse("tests")

    if ctx.env['PYTHON_CURSES']:
        python_scripts.append("ntpclients/ntpmon.py")
    if ctx.env['PYTHON_GPS']:
        python_scripts.append("ntpclients/ntploggps.py")

    # Make sure the python scripts compile, but don't install them
    ctx(
        features="py",
        source=python_scripts,
        install_path=None,
    )

    scripts = ["ntpclients/ntpleapfetch"] + python_scripts

    ctx(
        features="subst",
        source=scripts,
        target=[x.replace('.py', '') for x in scripts],
        chmod=Utils.O755,
        install_path='${BINDIR}',
    )

    ctx.add_post_fun(afterparty)
    if ctx.cmd == 'clean':
        afterparty(ctx)

    if ctx.env['PYTHON_GPS']:
        ctx.manpage(1, "ntpclients/ntploggps-man.adoc")
    ctx.manpage(1, "ntpclients/ntpdig-man.adoc")
    if ctx.env['PYTHON_CURSES']:
        ctx.manpage(1, "ntpclients/ntpmon-man.adoc")
    ctx.manpage(1, "ntpclients/ntpq-man.adoc")
    ctx.manpage(1, "ntpclients/ntpsweep-man.adoc")
    ctx.manpage(1, "ntpclients/ntptrace-man.adoc")
    ctx.manpage(1, "ntpclients/ntpviz-man.adoc")
    ctx.manpage(1, "ntpclients/ntplogtemp-man.adoc")
    ctx.manpage(8, "ntpclients/ntpkeygen-man.adoc")
    ctx.manpage(8, "ntpclients/ntpleapfetch-man.adoc")
    ctx.manpage(8, "ntpclients/ntpwait-man.adoc")
    ctx.manpage(8, "ntpclients/ntpsnmpd-man.adoc")

    # Skip running unit tests on a cross compile build
    if not ctx.env.ENABLE_CROSS:
        # Force re-running of tests.  Same as 'waf --alltests'
        if ctx.cmd == "check":
            ctx.options.all_tests = True

            # Print log if -v is supplied
            if verbose > 0:
                ctx.add_post_fun(test_print_log)

        # Test binaries
        ctx.add_post_fun(bin_test)

        # Write test log to a file
        ctx.add_post_fun(test_write_log)

        # Print a summary at the end
        ctx.add_post_fun(waf_unit_test.summary)
        ctx.add_post_fun(waf_unit_test.set_exit_code)
        ctx.add_post_fun(bin_test_summary)
    else:
        pprint("YELLOW", "Unit test runner skipped on a cross-compiled build.")
        from waflib import Options
        Options.options.no_tests = True

    if ctx.cmd == "build":
        if "PYTHONPATH" not in os.environ:
            print("--- PYTHONPATH is not set, "
                  "loading the Python ntp library may be troublesome ---")
        elif ctx.env.PYTHONARCHDIR not in os.environ["PYTHONPATH"]:
            print("--- PYTHONARCHDIR not in PYTHONPATH, "
                  "loading the Python ntp library may be troublesome ---")

#
# Boot script setup
#


def systemdenable(ctx):
    "Enable boot time start with systemd. Must run as root."
    ctx.exec_command("cp etc/ntpd.service etc/ntp-wait.service "
                     "/usr/lib/systemd/system/")


def systemddisable(ctx):
    "Disable boot time start with systemd. Must run as root."
    ctx.exec_command("rm -f /usr/lib/systemd/system/ntpd.service "
                     "/usr/lib/systemd/system/ntp-wait.service")

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
    ctx.exec_command("for prog in " + " ".join(python_scripts) +
                     "; do cxfreeze $prog; done")


def linkcheck(ctx):
    "Report references without anchors in the documentation."
    ctx.exec_command("devel/linkcheck docs/")

# The following sets edit modes for GNU EMACS
# Local Variables:
# mode:python
# End:
# end
