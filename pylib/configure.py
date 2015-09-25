from waflib.Configure import conf
from util import msg, msg_setting
import sys, os


def cmd_configure(ctx):
	from check_type import check_type
	from check_sizeof import check_sizeof

	if ctx.options.list:
		from refclock import refclock_map
		print "ID    Description"
		print "~~    ~~~~~~~~~~~"
		for id in refclock_map:
			print "%-5s %s" % (id, refclock_map[id]["descr"])

		return


	ctx.load('compiler_c')
	ctx.load('bison')

	# This needs to be at the top since it modifies CC and AR
	if ctx.options.enable_fortify:
		from check_fortify import check_fortify
		check_fortify(ctx)


	if ctx.options.enable_debug_gdb:
		ctx.env.CFLAGS += ["-g"]

	if ctx.options.enable_saveconfig:
		ctx.define("SAVECONFIG", 1)

	if ctx.options.enable_debug:
		ctx.define("DEBUG", 1)
		ctx.env.BISONFLAGS += ["--debug"]

	ctx.env.CFLAGS += ["-Wall"]	# Default CFLAGS.


	# Wipe out and override flags with those from the commandline
	for flag in ctx.env.OPT_STORE:
		opt = flag.replace("--", "").upper() # XXX: find a better way.
		ctx.env[opt] = ctx.env.OPT_STORE[flag]


	ctx.find_program("yacc", var="BIN_YACC")
	ctx.find_program("awk", var="BIN_AWK")
	ctx.find_program("perl", var="BIN_PERL")
	ctx.find_program("sh", var="BIN_SH")
	ctx.find_program("asciidoc", var="BIN_ASCIIDOC", mandatory=False)

	if ctx.options.enable_doc and not ctx.env.BIN_ASCIIDOC:
		ctx.fatal("asciidoc is required in order to build documentation")
	elif ctx.options.enable_doc:
		ctx.env.ASCIIDOC_FLAGS = ["-f", "%s/docs/asciidoc.conf" % ctx.srcnode.abspath(), "-a", "stylesdir=%s/docs/" % ctx.srcnode.abspath()]
		ctx.env.ENABLE_DOC = True


	ctx.start_msg("Checking build target")
	from sys import platform
	if platform == "win32":
		ctx.env.PLATFORM_TARGET = "win"
	else:
		ctx.env.PLATFORM_TARGET = "unix"
	ctx.end_msg(ctx.env.PLATFORM_TARGET	)


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


	ctx.recurse("lib/isc")
	ctx.recurse("libntp")
	ctx.recurse("sntp")

	types = ["int32", "int32_t", "uint32_t", "uint_t", "size_t", "wint_t", "pid_t", "intptr_t", "uintptr_t"]

	for type in sorted(types):
		ctx.check_type(type)

	# XXX: hack
	ctx.env.PLATFORM_INCLUDES = ["/usr/local/include"]
	ctx.env.PLATFORM_LIBPATH = ["/usr/local/lib"]

	sizeofs = [
		("time.h",		"time_t"),
		(None,			"int"),
		(None,			"short"),
		(None,			"long"),
		(None,			"long long"),
		("pthread.h",	"pthread_t"),
		(None,			"signed char"),
	]

	for header, sizeof in sorted(sizeofs):
		ctx.check_sizeof(header, sizeof)

	# The protocol major number
	ctx.define("NTP_API",	4)

	ctx.define("NTP_KEYSDIR", "%s/etc" % ctx.env.PREFIX)
	ctx.define("GETSOCKNAME_SOCKLEN_TYPE", "socklen_t", quote=False)
	ctx.define("DFLT_RLIMIT_STACK", 50)
	ctx.define("DFLT_RLIMIT_MEMLOCK", 32)
	ctx.define("POSIX_SHELL", "/bin/sh")

	ctx.define("OPENSSL_VERSION_TEXT", "#XXX: Fixme")

	# Checking for multicast capability:
	#
	#    AC_COMPILE_IFELSE(
	#	[AC_LANG_PROGRAM(
	#	    [[
	#		#ifdef HAVE_NETINET_IN_H
	#		# include <netinet/in.h>
	#		#endif
	#	    ]],
	#	    [[
	#		struct ip_mreq ipmr;
	#		ipmr.imr_interface.s_addr = 0;
	#	    ]]
	#	)],
	#	[ntp_cv_multicast=yes],
	#	[]
	#   )
	ctx.define("MCAST", 1) # XXX: check for mcast support

	ctx.define("TYPEOF_IP_MULTICAST_LOOP", "u_char", quote=False) #XXX: check for mcast type

	ctx.define("OPEN_BCAST_SOCKET", 1)

	# Optional functions.
	functions = (
		('adjtimex', "sys/timex.h"),
		('arc4random', "stdlib.h"),
		('arc4random_buf', "stdlib.h"),
		('clock_gettime', "time.h"),
		('clock_settime', "time.h"),
		('getclock', "sys/timers.h"),
		('getdtablesize', "unistd.h"),	# Not POSIX; SVr4, 4.2BSD
		('getpassphrase', "stdlib.h"),	# Sun systems
		('plock', "sys/lock.h"),	# OSF/1, SVID2, SVID3, XPG2
		('res_init', "resolv.h"),
		("rtprio", "sys/rtprio.h"),	# Sun/BSD
		('settimeofday', "sys/time.h"),	# BSD - remove as nonstandard?
		('strlcpy', "string.h"),
		('strlcat', "string.h"),
		('sysconf', "unistd.h"),
		('timegm', "time.h"),
		)
	for (n, h) in functions:
		ctx.check_cc(function_name=n, header_name=h, mandatory=False)

	ctx.check_cc(header_name="stdbool.h", mandatory=True)

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
		"alloca.h",
		"arpa/nameser.h",
		"ieeefp.h",
		"ifaddrs.h",
		"libgen.h",
		"libintl.h",
		"libscf.h",
		"linux/if_addr.h",
		"linux/rtnetlink.h",
		"linux/serial.h",
		"machine/soundcard.h",
		"netinet/in_systm.h",
		"md5.h",
		"net/if6.h",
		"net/if_var.h",
		"netinet/in_var.h",
		"netinfo/ni.h",
		"netinet/ip.h",
		"priv.h",
		"resolv.h",
		"stdatomic.h",
		"sys/audioio.h",
		"sys/ioctl.h",
		"sys/lock.h",
		"sys/modem.h",
		"sys/param.h",
		"sys/prctl.h",
		"sys/ppsclock.h",
		"sys/procset.h",
		"sys/sockio.h",
		"sys/soundcard.h",
		"sys/sysctl.h",
		"sys/systune.h",
		"utime.h",
	)
	for hdr in optional_headers:
		if not ctx.check_cc(header_name=hdr, mandatory=False) \
		   and os.path.exists("/usr/include/" + hdr):
			# Sanity check...
			print "Compilation check failed but include exists!"

	# XXX: This needs fixing.
	for header in ["timepps.h", "sys/timepps.h"]:
		ctx.check_cc(
			fragment="""
#include <inttypes.h>
#include <%s>
int main() { return 0; }
""" % header,
					define_name="HAVE_%s" % header.replace(".","_").replace("/","_").upper(),
					msg = "Checking for %s" % header,
					mandatory = False
		)

	if ctx.get_define("HAVE_TIMEPPS_H") or ctx.get_define("HAVE_SYS_TIMEPPS_H"):
		ctx.define("HAVE_PPSAPI", 1)


	ctx.check_cc(lib="m")
	ctx.check_cc(lib="pthread")
	ctx.check_cc(lib="rt", mandatory=False)
	ctx.check_cc(lib="thr", mandatory=False)
	ctx.check_cc(lib="gcc_s", mandatory=False)

	ctx.check_cc(header_name="event2/event.h", includes=ctx.env.PLATFORM_INCLUDES)
	ctx.check_cc(feature="c cshlib", lib="event", libpath=ctx.env.PLATFORM_LIBPATH, uselib_store="LIBEVENT")
	ctx.check_cc(feature="c cshlib", lib="event_core", libpath=ctx.env.PLATFORM_LIBPATH, uselib_store="LIBEVENT_CORE")
	ctx.check_cc(feature="c cshlib", lib="event_pthreads", libpath=ctx.env.PLATFORM_LIBPATH, uselib_store="LIBEVENT_PTHREADS", use="LIBEVENT")



	# Check for Linux capability.
	ctx.check_cc(header_name="sys/capability.h", mandatory=False)
	ctx.check_cc(lib="cap", mandatory=False)

	if ctx.env.LIB_CAP:
		from check_cap import check_cap
		check_cap(ctx)

	if ctx.get_define("HAVE_CAPABILITY") and ctx.get_define("HAVE_SYS_CAPABILITY_H") and ctx.get_define("HAVE_SYS_PRCTL_H"):
		ctx.define("HAVE_LINUX_CAPABILITY", 1)


	ctx.check_cc(
		function_name="clock_gettime",
		header_name="time.h",
		use="RT",
		mandatory=False
	)

	ctx.check_cc(
		function_name="clock_settime",
		header_name="time.h",
		use="RT",
		mandatory=False
	)

	ctx.check_cc(
		function_name="settimeofday",
		header_name="sys/time.h",
		use="RT",
		mandatory=False
	)


	from check_sockaddr import check_sockaddr
	check_sockaddr(ctx)


	from check_posix_thread_version import check_posix_thread_version

	check_posix_thread_version(ctx)
	ctx.define('HAVE_PTHREADS', ctx.env.POSIX_THREAD_VERISON)


	if ctx.options.refclocks:
		from refclock import refclock_config
		refclock_config(ctx)


	if ctx.options.enable_ipv6:
		ctx.define("INCLUDE_IPV6_SUPPORT", 1)

	if ctx.options.enable_leap_smear:
		ctx.define("ENABLE_LEAP_SMEAR", 1)

	if ctx.options.enable_mssntp:
		ctx.define("ENABLE_MSSNTP", 1)

	if ctx.options.enable_lockclock:
		ctx.define("ENABLE_LOCKCLOCK", 1)

	if not ctx.options.disable_droproot:
		ctx.define("ENABLE_DROPROOT", 1)

	if not ctx.options.disable_dns_lookup:
		ctx.define("ENABLE_DNS_LOOKUP", 1)


	# There is an ENABLE_AUTOKEY as well, but as that feature
	# is not working and likely to be replaced it's not exposed
	# and can't be enabled.

	# There is an ENABLE_ASYMMETRIC that enables a section of the
	# protocol code having to do with handling very long asymmetric
	# delays, as in space communications. Likely this code has never
	# been enabled for production.

	# Won't be true under Windows, but is under every Unix-like OS.
	ctx.define("HAVE_WORKING_FORK", 1)

	# Does the kernel implement a phase-locked loop for timing?
	# All modern Unixes (in particular Linux and *BSD have this).
	#
	# The README for the (now deleted) kernel directory says this:
	# "If the precision-time kernel (KERNEL_PLL define) is
	# configured, the installation process requires the header
	# file /usr/include/sys/timex.h for the particular
	# architecture to be in place."
	#
	if ctx.check_cc(header_name="sys/timex.h", mandatory=False):
		ctx.define("HAVE_KERNEL_PLL", 1)

	# SO_REUSEADDR socket option is needed to open a socket on an
	# interface when the port number is already in use on another
	# interface. Linux needs this, NetBSD does not, status on
	# other platforms is unknown.  It is probably harmless to
	# have it on everywhere.
	ctx.define("NEED_REUSEADDR_FOR_IFADDRBIND", 1)

	# Fine-grained capabilities allow NTP to set the time after
	# dropping root.  Needs libcap to be linked.
	#if sys.platform_startswith("linux"):
	#	ctx.define("HAVE_LINUX_CAPABILITIES", 1)

	# HAVE_SOLARIS_PRIVS needs to be defined for the same effect
	# under Solaris.

	# Not yet known how to detect HP-UX at version < 8, but that needs this.
	# Shouldn't be an issue as 8.x shipped in January 1991!
	# ctx.define("NEED_RCVBUF_SLOP", 1)

	# Some Unixes allow use of signalled I/O for TCP and UDP I/O.
	# The following systems, notably including Linux and FreeBSD,
	# do *not* allow this:
	#
	# alpha*-dec-osf4*|alpha*-dec-osf5*)
	# *-convex-*)
	# *-dec-*)
	# *-pc-cygwin*)
	# *-sni-sysv*)
	# *-stratus-vos)
	# *-univel-sysv*)
	# *-*-irix6*)
	# *-*-freebsd*)
	# *-*-*linux*)
	# *-*-unicosmp*)
	# *-*-kfreebsd*)
	# m68k-*-mint*)
	#
	# Until we can test for system type better, don't enable this/
	#
	# ctx.define("HAVE_SIGNALED_IO", 1)

	if ctx.options.enable_crypto:
		from check_openssl import configure_ssl
		configure_ssl(ctx)


	ctx.define("NTPS_CFLAGS", " ".join(ctx.env.CFLAGS).replace("\"", "\\\""))
	ctx.define("NTPS_LDFLAGS", " ".join(ctx.env.LDFLAGS).replace("\"", "\\\""))

	ctx.start_msg("Writing configuration header:")
	ctx.write_config_header("config.h")
	ctx.end_msg("config.h", "PINK")


	msg("")
	msg("Build Options")
	msg_setting("CC", " ".join(ctx.env.CC))
	msg_setting("CFLAGS", " ".join(ctx.env.CFLAGS))
	msg_setting("LDFLAGS", " ".join(ctx.env.LDFLAGS))
	msg_setting("PREFIX", ctx.env.PREFIX)
	msg_setting("Debug Support", ctx.options.enable_debug)
	msg_setting("IPv6 Support", ctx.options.enable_ipv6)
	msg_setting("Refclocks", ctx.options.refclocks)
