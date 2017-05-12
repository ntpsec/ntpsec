from __future__ import print_function

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

from wafhelpers.options import options_cmd
from wafhelpers.probes \
    import probe_header_with_prerequisites, probe_function_with_prerequisites
from wafhelpers.test import test_write_log, test_print_log

pprint.__doc__ = None

out = "build"

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
    ctx.load('gnu_dirs')

    with open("VERSION", "r") as f:
        version_string = f.read().split(" ")[0].strip()
        [major, minor, rev] = version_string.split(".")
        ctx.env.NTPSEC_VERSION_MAJOR = int(major)
        ctx.env.NTPSEC_VERSION_MINOR = int(minor)
        ctx.env.NTPSEC_VERSION_REV = int(rev)

    ctx.env.NTPSEC_VERSION = "%s.%s.%s" % (ctx.env.NTPSEC_VERSION_MAJOR,
                                           ctx.env.NTPSEC_VERSION_MINOR,
                                           ctx.env.NTPSEC_VERSION_REV)
# These are not currently used via config.h so remove clutter.
# Leave as comments in case we want them tomorrow.
#    ctx.define("NTPSEC_VERSION_MAJOR", ctx.env.NTPSEC_VERSION_MAJOR,
#               comment="Major version number")
#    ctx.define("NTPSEC_VERSION_MINOR", ctx.env.NTPSEC_VERSION_MINOR,
#               comment="Minor version number")
#    ctx.define("NTPSEC_VERSION_REV", ctx.env.NTPSEC_VERSION_REV,
#               comment="Revision version number")

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
            "-f", "-I%s/libisc/include/" % srcnode,
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
        ctx.env.ASCIIDOC_FLAGS = ["-f", "%s/docs/asciidoc.conf"
                                  % ctx.srcnode.abspath()]
        ctx.env.ENABLE_DOC_USER = ctx.options.enable_doc

    # XXX: conditionally build this with --disable-man?
    # Should it build without docs enabled?
    ctx.env.A2X_FLAGS = ["--format", "manpage",
                         "--asciidoc-opts=--conf-file=%s/docs/asciidoc.conf"
                         % ctx.srcnode.abspath()]
    if not ctx.options.enable_a2x_xmllint:
        ctx.env.A2X_FLAGS += ["--no-xmllint"]

    # Disable manpages within build()
    if ctx.options.disable_manpage:
        ctx.env.DISABLE_MANPAGE = True

    from waflib.Utils import subprocess
    if ((os.path.exists(".git") and
            ctx.find_program("git", var="BIN_GIT", mandatory=False))):
        ctx.start_msg("DEVEL: Getting revision")
        cmd = ["git", "log", "-1", "--format=%H"]
        p = subprocess.Popen(cmd, stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE, universal_newlines=True)
        ctx.env.NTPSEC_REVISION, stderr = p.communicate()
        ctx.env.NTPSEC_REVISION = ctx.env.NTPSEC_REVISION.replace("\n", "")
        ctx.end_msg(ctx.env.NTPSEC_REVISION)

    ctx.start_msg("Building version")
    ctx.env.NTPSEC_VERSION_STRING = ctx.env.NTPSEC_VERSION

    if ctx.env.NTPSEC_REVISION:
        ctx.env.NTPSEC_VERSION_STRING += "-%s" % ctx.env.NTPSEC_REVISION[:7]

    if ctx.options.build_version_tag:
        ctx.env.NTPSEC_VERSION_STRING += "-%s" % ctx.options.build_version_tag

    ctx.define("NTPSEC_VERSION_STRING", ctx.env.NTPSEC_VERSION_STRING)
    ctx.end_msg(ctx.env.NTPSEC_VERSION_STRING)

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
    if ctx.options.enable_leap_smear:
        ctx.define("ENABLE_LEAP_SMEAR", 1,
                   comment="Enable experimental leap smearing code")

    # check for some libs first.  some options, like stack protector,
    # may depend on some libs, like -lssp
    ctx.check_cc(lib="m", comment="Math library")
    ctx.check_cc(lib="rt", mandatory=False, comment="realtime library")
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
        ('gnu99', '-std=gnu99'),
        # this quiets most of macOS warnings on -fpie
        ('unused', '-Qunused-arguments'),
        # ('w_cast_align', "-Wcast-align"), # fails on RasPi, needs fixing.
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
            "-Wcast-align",           # fails on RasPi, needs fixing.
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
            # fails on Solaris and OpenBSD 6
            # complains about a Bison bug
            ('w_sign_conversion', "-Wsign-conversion"),
            # fails on clang, lot's of false positives and Unity complaints
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
                  msg='Checking if C compiler supports ' + name,
                  run_build_cls='oc')

    ctx.run_build_cls = old_run_build_cls

    # We require some things that C99 doesn't enable, like pthreads.
    # Thus -std=gnu99 rather than -std=c99 here, if the compiler supports
    # it.
    if ctx.env.HAS_gnu99:
        ctx.env.CFLAGS = ["-std=gnu99"] + ctx.env.CFLAGS
    else:
        ctx.env.CFLAGS = ["-std=c99"] + ctx.env.CFLAGS

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
    if ctx.env.HAS_w_cast_align:
        ctx.env.CFLAGS = ['-Wcast-align'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_missing_declarations:
        ctx.env.CFLAGS = ['-Wmissing-declarations'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_cast_qual:
        ctx.env.CFLAGS = ['-Wcast-qual'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_packed:
        ctx.env.CFLAGS = ['-Wpacked'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_shadow:
        ctx.env.CFLAGS = ['-Wshadow'] + ctx.env.CFLAGS
    if ctx.env.HAS_w_sign_conversion:
        ctx.env.CFLAGS = ['-Wsign-conversion'] + ctx.env.CFLAGS
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
                  msg='Checking if ld compiler supports ' + name,
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
            # -flto breaks tests on macOS
            ctx.env.CFLAGS = [
                "-fsanitize=cfi",           # hardening
                "-fsanitize=safe-stack",    # hardening
                ] + ctx.env.CFLAGS
            ctx.env.LDFLAGS += [
                "-Wl,-z,relro",  # hardening, marks some section read only,
                ]
    # else:  # gcc, probably

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
        ctx.define("_POSIX_PTHREAD_SEMANTICS", "1", quote=False,
                   comment="Needed for POSIX function definitions on Solaris")

    # XXX: needed for ntp_worker, for now
    if ctx.env.DEST_OS == "openbsd":
        ctx.define("PLATFORM_OPENBSD", "1", quote=False)

    structures = (
        ("struct if_laddrconf", ["sys/types.h", "net/if6.h"]),
        ("struct if_laddrreq", ["sys/types.h", "net/if6.h"]),
        ("struct timex", ["sys/time.h", "sys/timex.h"]),
        ("struct ntptimeval", ["sys/time.h", "sys/timex.h"]),
    )
    for (s, h) in structures:
        ctx.check_cc(type_name=s, header_name=h, mandatory=False)

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

    # These are helpful and don't break Linux or *BSD
    ctx.define("OPEN_BCAST_SOCKET", 1,
               comment="Whether to open a broadcast socket")

    # Find OpenSSL. Must happen before function checks
    # Versions older than 0.9.7d were deemed incompatible in NTP Classic.
    SNIP_OPENSSL_VERSION_CHECK = """
    #include <openssl/evp.h>
    int main(void) {
    #if OPENSSL_VERSION_NUMBER < 0x0090704fL
    #error OpenSSL must be at least 0.9.7
    #endif
        return 0;
    }
    """

    openssl_headers = (
        "openssl/evp.h",
        "openssl/rand.h",
        "openssl/objects.h",
    )
    for hdr in openssl_headers:
        ctx.check_cc(header_name=hdr, includes=ctx.env.PLATFORM_INCLUDES)
    # FIXME! Ignoring the result...
    ctx.check_cc(lib="crypto")
    ctx.check_cc(comment="OpenSSL support",
                 fragment=SNIP_OPENSSL_VERSION_CHECK,
                 includes=ctx.env.PLATFORM_INCLUDES,
                 msg="Checking OpenSSL >= 0.9.7",
                 )

    # Optional functions.  Do all function checks here, otherwise
    # we're likely to duplicate them.
    functions = (
        ('adjtimex', ["sys/time.h", "sys/timex.h"]),
        ('closefrom', ["stdlib.h"]),
        ('clock_gettime', ["time.h"], "RT"),
        ('clock_settime', ["time.h"], "RT"),
        ('getrusage', ["sys/time.h", "sys/resource.h"]),
        ('nanotime', ["sys/time.h"]),     # Old OS X
        ('ntp_adjtime', ["sys/time.h", "sys/timex.h"]),     # BSD
        ('ntp_gettime', ["sys/time.h", "sys/timex.h"]),     # BSD
        ('res_init', ["resolv.h"]),
        ('sched_setscheduler', ["sched.h"]),
        ('strlcpy', ["string.h"]),
        ('strlcat', ["string.h"]),
        ('timer_create', ["time.h"])
    )
    for ft in functions:
        if len(ft) == 2:
            probe_function_with_prerequisites(ctx, function=ft[0],
                                              prerequisites=ft[1])
        else:
            probe_function_with_prerequisites(ctx, function=ft[0],
                                              prerequisites=ft[1],
                                              use=ft[2])

    # check for BSD versions outside of libc
    if not ctx.get_define("HAVE_STRLCAT"):
        ret = probe_function_with_prerequisites(ctx, function='strlcat',
                                                prerequisites=['bsd/string.h'])
        if ret:
            ctx.define("HAVE_STRLCAT", 1, comment="Using bsd/strlcat")

    if not ctx.get_define("HAVE_STRLCPY"):
        ret = probe_function_with_prerequisites(ctx, function='strlcpy',
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
        "dns_sd.h",         # NetBSD, Apple, mDNS
        "bsd/string.h",     # bsd emulation
        ("ifaddrs.h", ["sys/types.h"]),
        ("linux/if_addr.h", ["sys/socket.h"]),
        ("linux/rtnetlink.h", ["sys/socket.h"]),
        "linux/serial.h",
        "net/if6.h",
        ("net/route.h", ["sys/types.h", "sys/socket.h", "net/if.h"]),
        "netinfo/ni.h",     # Apple
        "priv.h",           # Solaris
        ("resolv.h", ["sys/types.h", "netinet/in.h", "arpa/nameser.h"]),
        "semaphore.h",
        "stdatomic.h",
        "sys/clockctl.h",       # NetBSD
        "sys/ioctl.h",
        "sys/modem.h",      # Apple
        "sys/sockio.h",
        ("sys/sysctl.h", ["sys/types.h"]),
        ("timepps.h", ["inttypes.h"]),
        ("sys/timepps.h", ["inttypes.h", "sys/time.h"]),
        "utmpx.h",       # missing on RTEMS and OpenBSD
        ("sys/timex.h", ["sys/time.h"]),
    )
    for hdr in optional_headers:
        if isinstance(hdr, str):
            if ctx.check_cc(header_name=hdr, mandatory=False,
                            comment="<%s> header" % hdr):
                continue
        else:
            (hdr, prereqs) = hdr
            if probe_header_with_prerequisites(ctx, hdr, prereqs):
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
    if ctx.options.enable_seccomp:
        ctx.define("ENABLE_SECCOMP", 1,
                   comment="Enable seccomp")

    if not ctx.options.disable_dns_lookup:
        ctx.define("ENABLE_DNS_LOOKUP", 1,
                   comment="Enable DNS lookup of hostnames")

    # This is true under every Unix-like OS.
    ctx.define("HAVE_WORKING_FORK", 1,
               comment="Whether a working fork() exists")

    # Does the kernel implement a phase-locked loop for timing?
    # All modern Unixes (in particular Linux and *BSD) have this.
    #
    # The README for the (now deleted) kernel directory says this:
    # "If the precision-time kernel (KERNEL_PLL define) is
    # configured, the installation process requires the header
    # file /usr/include/sys/timex.h for the particular
    # architecture to be in place."
    #
    if ((ctx.get_define("HAVE_SYS_TIMEX_H") and
            not ctx.options.disable_kernel_pll)):
        ctx.define("HAVE_KERNEL_PLL", 1,
                   comment="Whether phase-locked loop for timing "
                   "exists and is enabled")

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

    # libisc/
    # XXX: Hack that needs to be fixed properly for all platforms
    ctx.define("ISC_PLATFORM_NORETURN_PRE", "", quote=False)
    ctx.define("ISC_PLATFORM_NORETURN_POST",
               "__attribute__((__noreturn__))", quote=False)

    if ctx.get_define("HAVE_SYS_SYSCTL_H"):
        ctx.define("HAVE_IFLIST_SYSCTL", 1,
                   comment="Whether sysctl interface exists")

    # Header/library checks
    from wafhelpers.check_cap import check_cap
    check_cap(ctx)

    from wafhelpers.check_seccomp import check_seccomp
    check_seccomp(ctx)

    from wafhelpers.check_pthread import check_pthread_header_lib
    check_pthread_header_lib(ctx)

    if not ctx.options.disable_mdns_registration:
        from wafhelpers.check_mdns import check_mdns_header
        check_mdns_header(ctx)

    if not ctx.options.disable_dns_lookup:
        from wafhelpers.check_pthread import check_pthread_run
        check_pthread_run(ctx)

    if not ctx.options.disable_mdns_registration:
        from wafhelpers.check_mdns import check_mdns_run
        check_mdns_run(ctx)

    # Solaris needs -lsocket and -lnsl for socket code
    if ctx.env.DEST_OS == "sunos":
        ctx.check(features="c cshlib", lib="socket", mandatory=False)
        ctx.check(features="c cshlib", lib="nsl", mandatory=False)

    if ctx.options.enable_classic_mode:
        ctx.define("ENABLE_CLASSIC_MODE", 1)
    else:
        ctx.undefine("ENABLE_CLASSIC_MODE")

    if ctx.options.enable_debug_timing:
        ctx.define("ENABLE_DEBUG_TIMING", 1)
    else:
        ctx.undefine("ENABLE_DEBUG_TIMING")

    # Ugly hack to examine config symbols
    for sym in ctx.env.DEFINES:
        if sym.startswith("NTP_SIZEOF_TIME_T="):
            timesize = int(sym.split("=")[1])
            if timesize < 8:
                msg("WARNING: This system has a 32-bit time_t.")
                msg("WARNING: Your ntpd will fail on 2038-01-19T03:14:07Z.")

    source_date_epoch = os.getenv('SOURCE_DATE_EPOCH', None)
    if ctx.options.build_epoch:
        ctx.define("BUILD_EPOCH", ctx.options.build_epoch,
                   comment="Using --build-epoch")
    elif source_date_epoch:
        if not source_date_epoch.isdigit():
            msg("ERROR: malformed SOURCE_DATE_EPOCH")
            sys.exit(1)
        ctx.define("BUILD_EPOCH", int(source_date_epoch),
                   comment="Using SOURCE_DATE_EPOCH")
    else:
        ctx.define("BUILD_EPOCH", int(time.time()), comment="Using default")

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
    msg_setting("Debug Support", yesno(ctx.options.enable_debug))
    msg_setting("Refclocks", ", ".join(ctx.env.REFCLOCK_LIST))
    msg_setting("Build Manpages",
                yesno(ctx.env.ENABLE_DOC and not ctx.env.DISABLE_MANPAGE))

    ctx.recurse("pylib")


class check(BuildContext):
    cmd = 'check'
    variant = "main"


def bin_test(ctx):
    """Run binary check, use after tests."""
    from wafhelpers.bin_test import cmd_bin_test
    cmd_bin_test(ctx, config)

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
    # ("info", "cmd_info", "Show build information / configuration.")
)


for command, func, descr in commands:
    class tmp1(Context.Context):
        if descr:
            __doc__ = descr
        cmd = command
        fun = func
        if ((command in
            'install uninstall build clean list step docs bsp info'
             )):
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
        ctx.exec_command("rm -f wafhelpers/*.pyc pylib/__pycache__/*.pyc "
                         "wafhelpers/__pycache__/*.pyc ntpd/version.h")
    for x in ("ntpclients",):
        # List used to be longer...
        path_build = ctx.bldnode.make_node("pylib")
        path_source = ctx.srcnode.make_node(x + "/ntp")
        relpath = "../" + path_build.path_from(ctx.srcnode)
        if ctx.cmd in ('install', 'build'):
            if ((not path_source.exists() or
                    os.readlink(path_source.abspath()) != relpath)):
                try:
                    os.remove(path_source.abspath())
                except OSError:
                    pass
                os.symlink(relpath, path_source.abspath())
        elif ctx.cmd == 'clean':
            if path_source.exists():
                # print "removing", path_source.abspath()
                os.remove(path_source.abspath())
    bldnode = ctx.bldnode.abspath()
    if ctx.cmd in ('install', 'build'):
        os.system("cd %s/pylib; ln -sf ../libntp/ntpc.so ntpc.so "
                  % (bldnode,))

python_scripts = [
    "ntpclients/ntploggps",
    "ntpclients/ntpdig",
    "ntpclients/ntpkeygen",
    "ntpclients/ntpmon",
    "ntpclients/ntpq",
    "ntpclients/ntpsweep",
    "ntpclients/ntptrace",
    "ntpclients/ntpviz",
    "ntpclients/ntpwait",
    "ntpclients/ntplogtemp",
]


def build(ctx):
    ctx.load('waf', tooldir='wafhelpers/')
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

    if ctx.variant == "host":
        ctx.recurse("ntpd")
        return

    ctx.recurse("libisc")
    if ctx.env.REFCLOCK_GENERIC:
        # Only required by the generic refclock
        ctx.recurse("libparse")
    ctx.recurse("libntp")
    ctx.recurse("ntpd")
    ctx.recurse("ntpfrob")
    ctx.recurse("ntptime")
    ctx.recurse("pylib")
    ctx.recurse("attic")
    ctx.recurse("tests")

    scripts = ["ntpclients/ntpleapfetch"] + python_scripts

    ctx(
        features="subst",
        source=scripts,
        target=scripts,
        chmod=Utils.O755,
        install_path='${BINDIR}',
    )

    ctx.add_post_fun(afterparty)
    if ctx.cmd == 'clean' or ctx.cmd == 'distclean':
        afterparty(ctx)

    ctx.manpage(1, "ntpclients/ntploggps-man.txt")
    ctx.manpage(1, "ntpclients/ntpdig-man.txt")
    ctx.manpage(1, "ntpclients/ntpmon-man.txt")
    ctx.manpage(1, "ntpclients/ntpq-man.txt")
    ctx.manpage(1, "ntpclients/ntpsweep-man.txt")
    ctx.manpage(1, "ntpclients/ntptrace-man.txt")
    ctx.manpage(1, "ntpclients/ntpviz-man.txt")
    ctx.manpage(1, "ntpclients/ntplogtemp-man.txt")
    ctx.manpage(8, "ntpclients/ntpkeygen-man.txt")
    ctx.manpage(8, "ntpclients/ntpleapfetch-man.txt")
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
        ctx.add_post_fun(waf_unit_test.set_exit_code)
    else:
        pprint("YELLOW", "Unit test runner skipped on a cross-compiled build.")
        from waflib import Options
        Options.options.no_tests = True

    if ctx.cmd == "build":
        if "PYTHONPATH" not in os.environ:
            print("--- PYTHONPATH is not set, "
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
