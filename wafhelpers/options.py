# Copyright the NTPsec project contributors
#
# SPDX-License-Identifier: BSD-2-Clause

def options_cmd(ctx, config):
    ctx.load("compiler_c")
    ctx.load("msvc")
    ctx.load('waf_unit_test')
    ctx.load('gnu_dirs')

    grp = ctx.add_option_group("NTP configure options")
    grp.add_option('--enable-debug', action='store_true',
                   default=False, help="Enable debugging code")
    grp.add_option('--disable-debug-gdb', action='store_true',
                   default=False, help="Disable GDB debugging symbols")
    grp.add_option('--enable-attic', action='store_true',
                   default=False, help="Enable building attic/*.")
    grp.add_option('--disable-nts', action='store_true',
                   default=False, help="Disable NTS.")
    grp.add_option('--disable-droproot', action='store_true',
                   default=False, help="Disable dropping root.")
    grp.add_option('--enable-early-droproot', action='store_true',
                   default=False,
                   help="Droproot earlier (breaks SHM and NetBSD).")
    grp.add_option('--enable-seccomp', action='store_true',
                   default=False, help="Enable seccomp (restricts syscalls).")
    grp.add_option('--disable-mdns-registration', action='store_true',
                   default=False, help="Disable MDNS registration.")
    grp.add_option(
        '--enable-classic-mode', action='store_true',
        default=False,
        help="Strict configuration and log-format "
             " compatibility with NTP Classic")
    grp.add_option('--enable-debug-timing', action='store_true',
                   default=False,
                   help="Collect timing statistics for debugging.")
    grp.add_option('--enable-pylib', action='store',
                   default='ffi', choices=['ext', 'ffi', 'none'],
                   help="""Choose which Python library to build.\n
ext, ffi, or none. defaults to ffi.""", nargs=1)

    grp = ctx.add_option_group("NTP cross compile options")
    grp.add_option('--cross-compiler', type=str,
                   help="Path to cross compiler CC. (enables cross-compiling)")
    grp.add_option('--cross-cflags', type=str, action='append',
                   help="Cross compiler CFLAGS.")
    grp.add_option('--cross-ldflags', type=str, action='append',
                   help="Cross compiler LDFLAGS.")

    grp = ctx.add_option_group("NTP configure features")
    grp.add_option('--enable-leap-smear', action='store_true',
                   default=False, help="Enable Leap Smearing.")
    grp.add_option('--enable-leap-testing', action='store_true',
                   default=False,
                   help="Enable leaps on other than 1st of month.")
    grp.add_option('--enable-mssntp', action='store_true',
                   default=False, help="Enable Samba MS SNTP support.")

    grp = ctx.add_option_group("Refclock configure options")
    grp.add_option(
        '--refclock', dest='refclocks',
        help="Comma-separated list of Refclock IDs to build (or \"all\")",
        type=str)
    grp.add_option('--list', action='store_true', default=False,
                   help="List available Refclocks")

    grp = ctx.add_option_group("NTP developer configure options")
    grp.add_option('--build-version-tag', type=str,
                   help="Append a tag to the version string (unused)")
    grp.add_option('--cflags', type=str, action='append',
                   help="Users should use CFLAGS in their environment.")
    grp.add_option('--build-desc', type=str, default="",
                   help="Arbitrary text string to append to extended version.")
    grp.add_option('--ldflags', type=str, action='append',
                   help="Users should use LDFLAGS in their environment.")
    grp.add_option('--check', action='store_true', default=False,
                   help="Run tests")
    grp.add_option('--enable-warnings', action='store_true',
                   default=False, help="Turn on a few more warnings")
    grp.add_option('--enable-Werror', action='store_true',
                   default=False, help="Turn on -Werror")
    grp.add_option(
        '--define', type=str, action='append',
        help="Force definition of symbol, wants value of form SYM=VAL.")
    grp.add_option('--undefine', type=str, action='append',
                   help="Force undefinition of symbol.")
    grp.add_option('--disable-fuzz', action='store_true',
                   default=False,
                   help="Feature gone")

    # This option group name needs to exactly match the name from
    # waflib/Tools/python.py so this option groups with the other Python
    # options.
    grp = ctx.add_option_group("Python Options")
    grp.add_option('--pyshebang', type=str,
                   default='/usr/bin/env python',
                   help="Shebang for Python scripts [Default: %(default)s}")
