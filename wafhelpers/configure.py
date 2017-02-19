from __future__ import print_function

import os
from wafhelpers.probes \
    import probe_header_with_prerequisites, probe_function_with_prerequisites
from wafhelpers.util import msg, msg_setting, parse_version


def cmd_configure(ctx, config):
    srcnode = ctx.srcnode.abspath()
    bldnode = ctx.bldnode.abspath()

    ctx.run_build_cls = 'check'
    ctx.load('waf', tooldir='wafhelpers/')
    ctx.load('waf_unit_test')

    parse_version(config)

    ctx.env.NTPSEC_VERSION_MAJOR = config["NTPSEC_VERSION_MAJOR"]
    ctx.env.NTPSEC_VERSION_MINOR = config["NTPSEC_VERSION_MINOR"]
    ctx.env.NTPSEC_VERSION_REV = config["NTPSEC_VERSION_REV"]

    ctx.env.NTPSEC_VERSION = "%s.%s.%s" % (ctx.env.NTPSEC_VERSION_MAJOR,
                                           ctx.env.NTPSEC_VERSION_MINOR,
                                           ctx.env.NTPSEC_VERSION_REV)
    ctx.define("NTPSEC_VERSION_MAJOR", ctx.env.NTPSEC_VERSION_MAJOR,
               comment="Major version number")
    ctx.define("NTPSEC_VERSION_MINOR", ctx.env.NTPSEC_VERSION_MINOR,
               comment="Minor version number")
    ctx.define("NTPSEC_VERSION_REV", ctx.env.NTPSEC_VERSION_REV,
               comment="Revision version number")

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
                                    ctx.env.BINDIR])
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
        ctx.env.HTMLDIR = ctx.options.htmldir

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

    ctx.env.SBINDIR = ctx.options.sbindir
    ctx.env.MANDIR = ctx.options.mandir

    from os.path import exists
    from waflib.Utils import subprocess
    if ((exists(".git")
         and ctx.find_program("git", var="BIN_GIT", mandatory=False))):
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

        ctx.env.CC = [ctx.options.cross_compiler]
        ctx.env.LINK_CC = [ctx.options.cross_compiler]

        if ctx.env["CROSS-CFLAGS"]:
            ctx.env.CFLAGS = opt_map["CROSS-CFLAGS"]

        if ctx.env["CROSS-LDFLAGS"]:
            ctx.env.LDFLAGS = opt_map["CROSS-LDFLAGS"]

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

    cc_test_flags = [
        ('PIE', '-pie -fPIE'),
        ('gnu99', '-std=gnu99'),
        # this quiets most of macOS warnings on -fpie
        ('unused', '-Qunused-arguments'),
        ]

    if ctx.options.enable_debug_gdb:
        ctx.env.CFLAGS += ["-g"]
    else:
        cc_test_flags += [
            ('LTO', '-flto'),
            ]

    if not ctx.options.disable_debug:
        ctx.define("DEBUG", 1, comment="Enable debug mode")
        ctx.env.BISONFLAGS += ["--debug"]

    ctx.env.CFLAGS += [
        # -O1 will turn on -D_FORTIFY_SOURCE=2 for us
        "-O1",
        "-Wall",
        "-Wextra",
        "-Wstrict-prototypes",
        ]

    # Check which linker flags are supported
    ld_hardening_flags = [
        ("-z now", "-Wl,-z,now"),     # no deferred symbol resolution
    ]

    # check if C compiler supports some flags
    for (name, ccflag) in cc_test_flags:
        cmd = [ctx.env.CC_NAME, ccflag]
        # print("cmd: %s" % cmd)
        ctx.start_msg("Checking for compiler hardening flag: %s" % name)
        try:
            ctx.cmd_and_log(cmd)
        except Exception as e:
            if not any(word in e.stderr for word
                       in ['ignored', 'illegal', 'unknown', 'unrecognized']):
                ctx.env.CFLAGS += [ccflag]
                ctx.end_msg("yes")
                ctx.env['HAS_' + name] = True
            else:
                ctx.env['HAS_' + name] = False
                ctx.end_msg("no", color="YELLOW")

    # We require some things that C99 doesn't enable, like pthreads.
    # Thus -std=gnu99 rather than -std=c99 here, if the compiler supports
    # it.
    if ctx.env.HAS_gnu99:
        ctx.env.CFLAGS += [
            "-std=gnu99",
            ]
    else:
        ctx.env.CFLAGS += [
            "-std=c99",
            ]

    if ctx.env.HAS_PIE:
        ctx.env.CFLAGS += [
            "-fPIE",
            "-pie",
            ]
        ld_hardening_flags += [
            ('PIE', "-fPIE"),           # hardening
            ('relro', "-Wl,-z,relro"),  # hardening, marks some read only,
            ]

    if ctx.env.HAS_unused:
        ctx.env.CFLAGS += [
            '-Qunused-arguments',
            ]

    # XXX: -flto currently breaks link of ntpd
    if ctx.env.HAS_LTO and False:
        ctx.env.CFLAGS += [
            "-flto",
            ]

    if ctx.options.disable_debug:
        # not debugging
        ld_hardening_flags += [
            ('--strip-all', "-Wl,--strip-all"),    # Strip binaries
            ]

    # old gcc takes -z,relro, but then barfs is -fPIE available and used.
    # ("relro", "-Wl,-z,relro"), # marks some sections read only

    for (name, ldflag) in ld_hardening_flags:
        cmd = [ctx.env.CC_NAME, ldflag]
        # print("cmd: %s" % cmd)
        ctx.start_msg("Checking for linker hardening flag: %s" % name)
        try:
            ctx.cmd_and_log(cmd)
        except Exception as e:
            if not any(word in e.stderr for word
                       in ['ignored', 'illegal', 'unknown', 'unrecognized']):
                ctx.env.LDFLAGS += [ldflag]
                ctx.end_msg("yes")
            else:
                ctx.end_msg("no", color="YELLOW")

    if ctx.env.CC_NAME == "sun":
        # we are sun, placeholder
        ctx.env.CFLAGS += []
    elif ctx.env.CC_NAME == "clang":
        # used on macOS, FreeBSD,
        # FORTIFY needs LTO to work well
        ctx.env.CFLAGS += [
            "-fstack-protector-all",    # hardening
            "-D_FORTIFY_SOURCE=2",      # hardening
            ]
        if ctx.env.DEST_OS not in ["darwin", "freebsd"]:
            # -flto breaks tests on macOS
            ctx.env.CFLAGS += [
                "-flto",                    # hardening, needed for sanitize
                "-fsanitize=cfi",           # hardening
                "-fsanitize=safe-stack",    # hardening
                ]
            ctx.env.LDFLAGS += [
                "-Wl,-z,relro",  # hardening, marks some section read only,
                ]
    else:
        # gcc, probably
        ctx.env.CFLAGS += [
            "-fstack-protector-all",    # hardening
            ]

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

    # int32_t and uint32_t probes aren't really needed, POSIX guarantees
    # them.  But int64_t and uint64_t are not guaranteed to exist on 32-bit
    # machines.  The calendar and ISC code needs them.
    types = ["uint64_t"]

    for inttype in sorted(types):
        ctx.check_cc(header_name=["stdint.h", "sys/types.h"],
                     mandatory=False,
                     type_name=inttype)

    structures = (
        ("struct if_laddrconf", ["sys/types.h", "net/if6.h"]),
        ("struct if_laddrreq", ["sys/types.h", "net/if6.h"]),
        ("struct timex", ["sys/time.h", "sys/timex.h"]),
        ("struct ntptimeval", ["sys/time.h", "sys/timex.h"]),
    )
    for (s, h) in structures:
        ctx.check_cc(type_name=s, header_name=h, mandatory=False)

    # waf's SNIP_FIELD should likely include this header itself
    ctx.check_cc(auto_add_header_name=True,
                 header_name="stddef.h",
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
        ("time.h",          "time_t"),
        (None,          "int"),
        (None,          "long"),
        (None,          "long long"),
    ]

    for header, sizeof in sorted(sizeofs, key=lambda x: x[1:]):
        check_sizeof(ctx, header, sizeof)

    ctx.define("GETSOCKNAME_SOCKLEN_TYPE", "socklen_t", quote=False,
               comment="socklen type")
    ctx.define("DFLT_RLIMIT_STACK", 50, comment="Default stack size")

    ctx.define("TYPEOF_IP_MULTICAST_LOOP", "u_char", quote=False,
               comment="Multicast loop type")   # XXX: check for mcast type

    # These are helpful and don't break Linux or *BSD
    ctx.define("OPEN_BCAST_SOCKET", 1,
               comment="Whether to open a broadcast socket")
    ctx.define("HAS_ROUTING_SOCKET", 1,
               comment="Whether a routing socket exists")

    ctx.check_cc(lib="m", comment="Math library")
    ctx.check_cc(lib="rt", mandatory=False, comment="realtime library")

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
        ('getdtablesize', ["unistd.h"]),
        ('getrusage', ["sys/time.h", "sys/resource.h"]),
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

    if ((ctx.get_define("HAVE_TIMEPPS_H")
         or ctx.get_define("HAVE_SYS_TIMEPPS_H"))):
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

    if not ctx.options.disable_dns_retry:
        ctx.define("ENABLE_DNS_RETRY", 1,
                   comment="Retry DNS lookups after an initial failure")

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
    if ((ctx.get_define("HAVE_SYS_TIMEX_H")
         and not ctx.options.disable_kernel_pll)):
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

    if not ctx.options.disable_dns_retry:
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
    msg_setting("PREFIX", ctx.env.PREFIX)
    msg_setting("Debug Support", yesno(not ctx.options.disable_debug))
    msg_setting("Refclocks", ", ".join(ctx.env.REFCLOCK_LIST))
    msg_setting("Build Manpages",
                yesno(ctx.env.ENABLE_DOC and not ctx.env.DISABLE_MANPAGE))

    if ctx.options.enable_debug:
        msg("")
        msg("*** --enable-debug ignored.  (default on now)")
        msg("")
