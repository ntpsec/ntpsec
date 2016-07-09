from waflib.Configure import conf
from util import msg, msg_setting
from probes import *
import sys, os
from util import parse_version

def cmd_configure(ctx, config):
	srcnode = ctx.srcnode.abspath()
	bldnode = ctx.bldnode.abspath()

	ctx.load('waf', tooldir='pylib/')
	ctx.load('waf_unit_test')

	from pylib.util import parse_version
	parse_version(config)

	ctx.env.NTPS_RELEASE = config["NTPS_RELEASE"]
	ctx.env.NTPS_VERSION_MAJOR = config["NTPS_VERSION_MAJOR"]
	ctx.env.NTPS_VERSION_MINOR = config["NTPS_VERSION_MINOR"]
	ctx.env.NTPS_VERSION_REV = config["NTPS_VERSION_REV"]

	ctx.env.NTPS_VERSION = "%s.%s.%s" % (ctx.env.NTPS_VERSION_MAJOR, ctx.env.NTPS_VERSION_MINOR, ctx.env.NTPS_VERSION_REV)
	ctx.define("NTPS_VERSION_MAJOR", ctx.env.NTPS_VERSION_MAJOR, comment="Major version number")
	ctx.define("NTPS_VERSION_MINOR", ctx.env.NTPS_VERSION_MINOR, comment="Minor version number")
	ctx.define("NTPS_VERSION_REV", ctx.env.NTPS_VERSION_REV, comment="Revision version number")

	ctx.env.OPT_STORE = config["OPT_STORE"]

	opt_map = {}
	# Wipe out and override flags with those from the commandline
	for flag in ctx.env.OPT_STORE:
		opt = flag.replace("--", "").upper() # XXX: find a better way.
		opt_map[opt] = ctx.env.OPT_STORE[flag]

	msg("--- Configuring host ---")
	ctx.setenv('host', ctx.env.derive())

	ctx.load('compiler_c')

	if not ctx.env.NTPS_RELEASE:
		ctx.load('bison')

	for opt in opt_map:
		ctx.env[opt] = opt_map[opt]

	from compiler import check_compiler
	check_compiler(ctx)



	if ctx.options.enable_rtems_trace:
		ctx.find_program("rtems-tld", var="BIN_RTEMS_TLD", path_list=[ctx.options.rtems_trace_path, ctx.env.BINDIR])
		ctx.env.RTEMS_TEST_ENABLE = True
		ctx.env.RTEMS_TEST_FLAGS = ["-C", "%s/devel/trace/ntpsec-trace.ini" % srcnode,
									"-W", "%s/ntpsec-wrapper" % bldnode,
									"-P", "%s/devel/trace/" % srcnode,
									"-f", "-I%s" % bldnode,
									"-f", "-I%s/include/" % srcnode,
									"-f", "-I%s/libisc/include/" % srcnode,
									"-f", "-I%s/libisc/unix/include/" % srcnode]

	# Not needed to build.  Used by utility scripts.
	ctx.find_program("awk", var="BIN_AWK", mandatory=False)
	ctx.find_program("perl", var="BIN_PERL", mandatory=False)
	ctx.find_program("sh", var="BIN_SH", mandatory=False)

	# used to make man and html pages
	ctx.find_program("asciidoc", var="BIN_ASCIIDOC", mandatory=False)
	ctx.find_program("a2x", var="BIN_A2X", mandatory=False)
	ctx.find_program("xsltproc", var="BIN_XSLTPROC", mandatory=False)

	ctx.env.ENABLE_DOC = False
	if ctx.env.BIN_ASCIIDOC and ctx.env.BIN_XSLTPROC and ctx.env.BIN_A2X:
		ctx.env.ENABLE_DOC = True


	if (ctx.options.enable_doc or ctx.options.enable_doc_only) and not ctx.env.ENABLE_DOC:
		ctx.fatal("asciidoc and xsltproc are required in order to build documentation")
	elif (ctx.options.enable_doc or ctx.options.enable_doc_only):
		ctx.env.ASCIIDOC_FLAGS = ["-f", "%s/docs/asciidoc.conf" % ctx.srcnode.abspath()]
		ctx.env.ENABLE_DOC_ONLY = ctx.options.enable_doc_only
		ctx.env.ENABLE_DOC_USER = ctx.options.enable_doc
		ctx.env.PATH_DOC = ctx.options.path_doc

	# XXX: conditionally build this with --disable-man?  Should it build without docs enabled?
	ctx.env.A2X_FLAGS = ["--format", "manpage", "--asciidoc-opts=--conf-file=%s/docs/asciidoc.conf" % ctx.srcnode.abspath()]
	if not ctx.options.enable_a2x_xmllint:
		ctx.env.A2X_FLAGS += ["--no-xmllint"]


	# Disable manpages within build()
	if ctx.options.disable_manpage:
		ctx.env.DISABLE_MANPAGE = True

	from os.path import exists
	from waflib.Utils import subprocess
	if exists(".git") and ctx.find_program("git", var="BIN_GIT", mandatory=False):
		ctx.start_msg("DEVEL: Getting revision")
		cmd = ["git", "log", "-1", "--format=%H"]
		p = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=None)
		ctx.env.NTPS_REVISION, stderr = p.communicate()
		ctx.env.NTPS_REVISION = ctx.env.NTPS_REVISION.replace("\n", "")
		ctx.end_msg(ctx.env.NTPS_REVISION)

	ctx.start_msg("Building version")
	ctx.env.NTPS_VERSION_STRING = ctx.env.NTPS_VERSION

	if ctx.env.NTPS_REVISION:
		ctx.env.NTPS_VERSION_STRING += "-%s" % ctx.env.NTPS_REVISION[:7]

	if ctx.options.build_version_tag:
		ctx.env.NTPS_VERSION_STRING += "-%s" % ctx.options.build_version_tag

	ctx.define("NTPS_VERSION_STRING", ctx.env.NTPS_VERSION_STRING)
	ctx.end_msg(ctx.env.NTPS_VERSION_STRING)


	msg("--- Configuring main ---")
	ctx.setenv("main", ctx.env.derive())

	# XXX: temp hack to fix --enable-doc-only
	ctx.env.ENABLE_DOC_ONLY = ctx.options.enable_doc_only

	# The rest is not needed for documentation building.
	if ctx.options.enable_doc_only:
		return

	from check_type import check_type
	from check_sizeof import check_sizeof
	from check_structfield import check_structfield

	for opt in opt_map:
		ctx.env[opt] = opt_map[opt]

	if ctx.options.cross_compiler:
		ctx.env.ENABLE_CROSS = True

		ctx.start_msg("Using Cross compiler CC:")
#		ctx.get_cc_version(ctx.env.CC, gcc=True)
		ctx.end_msg(ctx.options.cross_compiler)

		ctx.env.CC = [ctx.options.cross_compiler]
		ctx.env.LINK_CC = [ctx.options.cross_compiler]

		if ctx.env["CROSS-CFLAGS"]:
			ctx.env.CFLAGS = opt_map["CROSS-CFLAGS"]

		if ctx.env["CROSS-LDFLAGS"]:
			ctx.env.LDFLAGS = opt_map["CROSS-LDFLAGS"]


	if ctx.options.list:
		from refclock import refclock_map
		print "ID    Description"
		print "~~    ~~~~~~~~~~~"
		for id in refclock_map:
			print "%-5s %s" % (id, refclock_map[id]["descr"])

		return

	# This needs to be at the top since it modifies CC and AR
	if ctx.options.enable_fortify:
		from check_fortify import check_fortify
		check_fortify(ctx)


	if ctx.options.enable_debug_gdb:
		ctx.env.CFLAGS += ["-g"]

	if not ctx.options.disable_debug:
		ctx.define("DEBUG", 1, comment="Enable debug mode")
		ctx.env.BISONFLAGS += ["--debug"]

	# We require some things that C99 doesn't enable, like pthreads.
	# Thus -std=gnu99 rather than -std=c99 here.
	ctx.env.CFLAGS += ["-Wall", "-Wextra", "-std=gnu99"]	# Default CFLAGS.


	# Check target platform.
	ctx.start_msg("Checking build target")
	from sys import platform
	if platform == "win32":
		ctx.env.PLATFORM_TARGET = "win"
	elif platform == "darwin":
		ctx.env.PLATFORM_TARGET = "osx"
	elif platform.startswith("freebsd"):
		ctx.env.PLATFORM_TARGET = "freebsd"
	elif platform.startswith("netbsd"):
		ctx.env.PLATFORM_TARGET = "netbsd"
	elif platform.startswith("openbsd"):
		ctx.env.PLATFORM_TARGET = "openbsd"
	else:
		ctx.env.PLATFORM_TARGET = "unix"
	ctx.end_msg(ctx.env.PLATFORM_TARGET	)

	ctx.define("PLATFORM_%s" % ctx.env.PLATFORM_TARGET.upper(), 1, comment="Operating system detected by Python (%s)" % platform)


	# XXX: hack
	if ctx.env.PLATFORM_TARGET in ["freebsd", "osx", "openbsd"]:
		ctx.env.PLATFORM_INCLUDES = ["/usr/local/include"]
		ctx.env.PLATFORM_LIBPATH = ["/usr/local/lib"]
	elif ctx.env.PLATFORM_TARGET == "netbsd":
		ctx.env.PLATFORM_LIBPATH = ["/usr/lib"]
	elif ctx.env.PLATFORM_TARGET == "win":
		ctx.load("msvc")

	# OS X needs this for IPv6
	if ctx.env.PLATFORM_TARGET == "osx":
		ctx.define("__APPLE_USE_RFC_3542", 1, comment="Needed for IPv6 support")

	# int32_t and uint32_t probes aren't really needed, POSIX guarantees
	# them.  But int64_t and uint64_t are not guaranteed to exist on 32-bit
	# machines.
	# Used by timevalops and timespecops in tests/libntp/
	# May go away when that is cleaned up.
	types = ["uint64_t"]

	for inttype in sorted(types):
		ctx.check_type(inttype, ["stdint.h", "sys/types.h"])

	net_types = (
		("struct if_laddrconf", ["sys/types.h", "net/if6.h"]),
		("struct if_laddrreq", ["sys/types.h", "net/if6.h"]),
		)
	for (f, h) in net_types:
		ctx.check_type(f, h)

	structures = (
		("struct timex", ["sys/time.h", "sys/timex.h"]),
		("struct ntptimeval", ["sys/time.h", "sys/timex.h"]),
		)
	for (s, h) in structures:
		ctx.check_type(s, h)

	structure_fields = (
		("time_tick", "timex", ["sys/time.h", "sys/timex.h"]),
		("modes", "timex", ["sys/time.h", "sys/timex.h"]),
		("time.tv_nsec", "ntptimeval", ["sys/time.h", "sys/timex.h"]),
		)
	for (f, s, h) in structure_fields:
		ctx.check_structfield(f, s, h)

	# mostly used by timetoa.h and timespecops.h
	sizeofs = [
		("time.h",		"time_t"),
		(None,			"int"),
		(None,			"long"),
		(None,			"long long"),
	]

	for header, sizeof in sorted(sizeofs):
		ctx.check_sizeof(header, sizeof)

	# The protocol major number
	ctx.define("NTP_API", 4, comment="Protocol major number.")

	ctx.define("NTP_KEYSDIR", "%s/etc" % ctx.env.PREFIX, comment="NTP key file directory")
	ctx.define("GETSOCKNAME_SOCKLEN_TYPE", "socklen_t", quote=False, comment="socklen type")
	ctx.define("DFLT_RLIMIT_STACK", 50, comment="Default stack size")

	probe_multicast(ctx, "MCAST", "Checking for multicast capability")

	ctx.define("TYPEOF_IP_MULTICAST_LOOP", "u_char", quote=False, comment="Multicast loop type") #XXX: check for mcast type

	# These are helpful and don't break Linux or *BSD
	ctx.define("OPEN_BCAST_SOCKET", 1, comment="Whether to open a broadcast socket")
	ctx.define("HAS_ROUTING_SOCKET", 1, comment="Whether a routing socket exists")

	ctx.check_cc(lib="edit", mandatory=False, comment="libedit library")
	ctx.check_cc(lib="m", comment="Math library")
	ctx.check_cc(lib="ossaudio", mandatory=False, comment="ossaudio for NetBSD")  # NetBSD audio
	ctx.check_cc(lib="rt", mandatory=False, comment="realtime library")
	ctx.check_cc(lib="curses", mandatory=False, comment="curses library, required for readline on OpenBSD") # Required for readline on OpenBSD.
	ctx.check_cc(lib="readline", use="CURSES", mandatory=False, comment="readline library")
	ctx.check_cc(lib="gcc_s", mandatory=False, comment="GCC runtime library.")

	# Find OpenSSL. Must happen before function checks
	if ctx.options.enable_crypto:
		from check_openssl import configure_ssl
		configure_ssl(ctx)

	# Optional functions.  Do all function checks here, otherwise
	# we're likely to duplicate them.
	functions = (
		('adjtimex', ["sys/time.h", "sys/timex.h"]),
		('closefrom', ["stdlib.h"]),
		('clock_gettime', ["time.h"], "RT"),
		('clock_settime', ["time.h"], "RT"),
		('EVP_MD_do_all_sorted', ["openssl/evp.h"], "CRYPTO"),
		('getclock', ["sys/timers.h"]),
		('getpassphrase', ["stdlib.h"]),		# Sun systems
		('getrusage', ["sys/time.h", "sys/resource.h"]),
		('MD5Init', ["md5.h"], "CRYPTO"),
		('ntp_adjtime', ["sys/time.h", "sys/timex.h"]),		# BSD
		('ntp_gettime', ["sys/time.h", "sys/timex.h"]),		# BSD
		('res_init', ["resolv.h"]),
		("rtprio", ["sys/rtprio.h"]),		# Sun/BSD
		('sched_setscheduler', ["sched.h"]),
		('settimeofday', ["sys/time.h"], "RT"),	# BSD
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
	ctx.check_cc(header_name="stdbool.h", mandatory=True, comment="Sanity check.")

	# This is a list of every optional include header in the
	# codebase that is guarded by a directly corresponding HAVE_*_H symbol.
	#
	# In some cases one HAVE symbol controls inclusion of more than one
	# header; there is an example of this in ntp/audio.c.  In these cases
	# only the one header name matching the pattern of the HAVE_*_H symbol
	# name is listed here, so we can invert the relationship to generate
	# tests for all the symbols.
	#
	# Some of these are cruft from ancient big-iron systems and should
	# be removed.
	optional_headers = (
		"dns_sd.h",		# NetBSD, Apple, mDNS
		"histedit.h",		# Apple
		("ifaddrs.h", ["sys/types.h"]),
		"libscf.h",		# Solaris
		("linux/if_addr.h", ["sys/socket.h"]),
		("linux/rtnetlink.h", ["sys/socket.h"]),
		"linux/serial.h",
		#"linux/seccomp.h",	- Doesn't build yet, investigate
		"machine/soundcard.h",
		("md5.h", ["sys/types.h"]),
		"net/if6.h",
		("net/route.h", ["sys/types.h","sys/socket.h","net/if.h"]),
		"netinfo/ni.h",		# Apple
		"priv.h",               # Solaris
		("readline/readline.h",["stdio.h"]),
		("readline/history.h", ["stdio.h","readline/readline.h"]),
		("resolv.h", ["sys/types.h","netinet/in.h","arpa/nameser.h"]),
		"semaphore.h",
		"stdatomic.h",
		"sys/audioio.h",
		"sys/clockctl.h",	# NetBSD
		"sys/ioctl.h",
		"sys/modem.h",          # Apple
		"sys/sockio.h",
		"sys/soundcard.h",
		("sys/sysctl.h", ["sys/types.h"]),
		("timepps.h", ["inttypes.h"]),
		("sys/timepps.h", ["inttypes.h", "sys/time.h"]),
		"utmpx.h",       # missing on RTEMS and OpenBSD
		("sys/timex.h", ["sys/time.h"]),
		"sys/audio.h"
	)
	for hdr in optional_headers:
		if type(hdr) == type(""):
			if ctx.check_cc(header_name=hdr, mandatory=False, comment="<%s> header" % hdr):
				continue
		else:
			(hdr, prereqs) = hdr
			if probe_header_with_prerequisites(ctx, hdr, prereqs):
				continue
		if os.path.exists("/usr/include/" + hdr):
			# Sanity check...
			print "Compilation check failed but include exists %s" % hdr

	if ctx.get_define("HAVE_TIMEPPS_H") or ctx.get_define("HAVE_SYS_TIMEPPS_H"):
		ctx.define("HAVE_PPSAPI", 1, comment="Enable the PPS API")


	# Check for Solaris capabilities
	if ctx.get_define("HAVE_PRIV_H") and sys.platform == "Solaris":
		ctx.define("HAVE_SOLARIS_PRIVS", 1, comment="Enable Solaris Privileges (Solaris only)")

	from check_sockaddr import check_sockaddr
	check_sockaddr(ctx)

	# Some systems don't have sys/timex.h eg OS X, OpenBSD...
	if ctx.get_define("HAVE_SYS_TIMEX_H"):
		ctx.env.HEADER_SYS_TIMEX_H = True

	# Some systems don't have sys/audio.h eg OS X, OpenBSD...
	if ctx.get_define("HAVE_SYS_AUDIO_H") or \
	   ctx.get_define("HAVE_SYS_SOUNDCARD_H") or \
	   ctx.get_define("HAVE_MACHINE_SOUNDCARD_H"):
		ctx.env.HAVE_AUDIO = True  # makes util/tg2

	if ctx.options.refclocks:
		from refclock import refclock_config

		# Enable audio when the right headers exist.
		if ctx.get_define("HAVE_SYS_AUDIOIO_H") or \
				ctx.get_define("HAVE_SYS_SOUNDCARD_H") or \
				ctx.get_define("HAVE_MACHINE_SOUNDCARD_H"):
			ctx.env.AUDIO_ENABLE = True

		refclock_config(ctx)

	# NetBSD (used to) need to recreate sockets on changed routing.
	# Perhaps it still does. If so, this should be set.  The autoconf
	# build set it "if the OS clears cached routes when more specifics
	# become available".
	# ctx.define("OS_MISSES_SPECIFIC_ROUTE_UPDATES", 1)

	if ctx.options.enable_leap_smear:
		ctx.define("ENABLE_LEAP_SMEAR", 1, comment="Enable experimental leap smearing code")

	if ctx.options.enable_mssntp:
		ctx.define("ENABLE_MSSNTP", 1, comment="Enable MS-SNTP extensions https://msdn.microsoft.com/en-us/library/cc212930.aspx")

	if ctx.options.enable_lockclock:
		ctx.define("ENABLE_LOCKCLOCK", 1, comment="Enable NIST 'lockclock'")

	if not ctx.options.disable_droproot:
		ctx.define("ENABLE_DROPROOT", 1, comment="Drop root after initialising")

	if not ctx.options.disable_dns_lookup:
		ctx.define("ENABLE_DNS_LOOKUP", 1, comment="Enable DNS lookup of hostnames")

	if not ctx.options.disable_dns_retry:
		ctx.define("ENABLE_DNS_RETRY", 1, comment="Retry DNS lookups after an initial failure")


	# There is an ENABLE_ASYMMETRIC that enables a section of the
	# protocol code having to do with handling very long asymmetric
	# delays, as in space communications. Likely this code has never
	# been enabled for production.

	# Won't be true under Windows, but is under every Unix-like OS.
	ctx.define("HAVE_WORKING_FORK", 1, comment="Whether a working fork() exists")

	# Does the kernel implement a phase-locked loop for timing?
	# All modern Unixes (in particular Linux and *BSD have this).
	#
	# The README for the (now deleted) kernel directory says this:
	# "If the precision-time kernel (KERNEL_PLL define) is
	# configured, the installation process requires the header
	# file /usr/include/sys/timex.h for the particular
	# architecture to be in place."
	#
	if ctx.get_define("HAVE_SYS_TIMEX_H"):
		ctx.define("HAVE_KERNEL_PLL", 1, comment="Whether phase-locked loop for timing exists")

	# SO_REUSEADDR socket option is needed to open a socket on an
	# interface when the port number is already in use on another
	# interface. Linux needs this, NetBSD does not, status on
	# other platforms is unknown.  It is probably harmless to
	# have it on everywhere.
	ctx.define("NEED_REUSEADDR_FOR_IFADDRBIND", 1, comment="Whether SO_REUSEADDR is needed to open same sockets on alternate interfaces, required by Linux at least")

	# Not yet known how to detect HP-UX at version < 8, but that needs this.
	# Shouldn't be an issue as 8.x shipped in January 1991!
	# ctx.define("NEED_RCVBUF_SLOP", 1)

	# It should be possible to use asynchronous I/O with notification
	# by SIGIO on any Unix conformant to POSIX.1-2001. But the code to
	# do this is untested and there are historical reasons to suspect
	# it might not work reliably on all platforms.  Enable cautiously
	# and test carefully.
	# ctx.define("ENABLE_SIGNALED_IO", 1)

	# Used in libntp/audio.c:
	#	[[
	#	    #ifdef HAVE_MACHINE_SOUNDCARD_H
	#	    # include <machine/soundcard.h>
	#	    #endif
	#	    #ifdef HAVE_SYS_SOUNDCARD_H
	#	    # include <sys/soundcard.h>
	#	    #endif
	#	]],
	#	[[
	#	    extern struct snd_size *ss;
	#	    return ss->rec_size;
	#	]]
	# ctx.define("HAVE_STRUCT_SND_SIZE", 1)

        # These are required by the SHA2 code and various refclocks
        if sys.byteorder == "little":
                pass
        elif sys.byteorder == "big":
                ctx.define("WORDS_BIGENDIAN", 1)
        else:
                print "Can't determine byte order!"

	probe_vsprintfm(ctx, "VSNPRINTF_PERCENT_M",
			    "Checking for %m expansion in vsnprintf(3)")

	# Define CFLAGS/LDCFLAGS for -vv support.
	ctx.define("NTPS_CFLAGS", " ".join(ctx.env.CFLAGS).replace("\"", "\\\""), comment="CFLAGS used when compiled")
	ctx.define("NTPS_LDFLAGS", " ".join(ctx.env.LDFLAGS).replace("\"", "\\\""), comment="LDFLAGS used when compiled")


	# Check for directory separator
	if ctx.env.PLATFORM_TARGET == "win":
		sep = "\\"
	else:
		sep = "/"

	ctx.define("DIR_SEP", "'%s'" % sep, quote=False, comment="Directory separator used")

	# libisc/
	# XXX: Hack that needs to be fixed properly for all platforms
	ctx.define("ISC_PLATFORM_NORETURN_PRE", "", quote=False)
	ctx.define("ISC_PLATFORM_NORETURN_POST", "__attribute__((__noreturn__))", quote=False)
	ctx.define("ISC_PLATFORM_HAVEIFNAMETOINDEX", 1)
	ctx.define("ISC_PLATFORM_HAVEIN6PKTINFO", 1)
	ctx.define("ISC_PLATFORM_HAVEIPV6", 1)
	ctx.define("ISC_PLATFORM_HAVESCOPEID", 1)

	if ctx.get_define("HAVE_SYS_SYSCTL_H"):
		ctx.define("HAVE_IFLIST_SYSCTL", 1, comment="Whether sysctl interface exists")


	# Header checks
	from pylib.check_cap import check_cap_header
	check_cap_header(ctx)

	from pylib.check_libevent2 import check_libevent2_header
	check_libevent2_header(ctx)

	from pylib.check_pthread import check_pthread_header_lib
	check_pthread_header_lib(ctx)

	if not ctx.options.disable_mdns_registration:
		from pylib.check_mdns import check_mdns_header
		check_mdns_header(ctx)


	# Run checks
	from pylib.check_cap import check_cap_run
	check_cap_run(ctx)

	from pylib.check_libevent2 import check_libevent2_run
	check_libevent2_run(ctx)


	from pylib.check_pthread import check_pthread_run
	check_pthread_run(ctx)

	if not ctx.options.disable_mdns_registration:
		from pylib.check_mdns import check_mdns_run
		check_mdns_run(ctx)

	if ctx.options.enable_classic_mode:
		ctx.define("ENABLE_CLASSIC_MODE", 1)
	else:
		ctx.undefine("ENABLE_CLASSIC_MODE")

	if ctx.env.PTHREAD_ENABLE:
		ctx.define("ISC_PLATFORM_USETHREADS", 1)


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
	msg_setting("Build Manpages", yesno(ctx.env.ENABLE_DOC and not ctx.env.DISABLE_MANPAGE))

	if ctx.options.enable_debug:
		msg("")
		msg("*** --enable-debug ignored.  (default on now)")
		msg("")

