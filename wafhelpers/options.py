def options_cmd(ctx, config):
    ctx.load("compiler_c")
    ctx.load("msvc")
    ctx.load('waf_unit_test')
    ctx.load('gnu_dirs')

    def callback_flags(option, opt, value, parser):
        config["OPT_STORE"].setdefault(opt, []).append(value)

    grp = ctx.add_option_group("NTP configure options")
    grp.add_option('--enable-debug', action='store_true',
                   default=False, help="Enable debugging code")
    grp.add_option('--enable-debug-gdb', action='store_true',
                   default=False, help="Enable GDB debugging symbols")
    grp.add_option('--disable-droproot', action='store_true',
                   default=False, help="Disable dropping root.")
    grp.add_option('--enable-early-droproot', action='store_true',
                   default=False,
                   help="Droproot earlier (breaks SHM and NetBSD).")
    grp.add_option('--enable-seccomp', action='store_true',
                   default=False, help="Enable seccomp (restricts syscalls).")
    grp.add_option('--disable-dns-lookup', action='store_true',
                   default=False, help="Disable DNS lookups.")
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

    grp = ctx.add_option_group("NTP cross compile options")
    grp.add_option('--cross-compiler', type='string',
                   help="Path to cross compiler CC. (enables cross-compiling)")
    grp.add_option('--cross-cflags', type='string',  action="callback",
                   callback=callback_flags, help="Cross compiler CFLAGS.")
    grp.add_option('--cross-ldflags', type='string', action="callback",
                   callback=callback_flags, help="Cross compiler LDFLAGS.")

    grp = ctx.add_option_group("NTP configure features")
    grp.add_option('--enable-leap-smear', action='store_true',
                   default=False, help="Enable Leap Smearing.")
    grp.add_option('--enable-leap-testing', action='store_true',
                   default=False,
                   help="Enable leaps on other than 1st of month.")
    grp.add_option('--enable-mssntp', action='store_true',
                   default=False, help="Enable Samba MS SNTP support.")
    grp.add_option('--enable-lockclock', action='store_true',
                   default=False, help="Enable NIST lockclock scheme.")

    grp = ctx.add_option_group("Refclock configure options")
    grp.add_option(
        '--refclock', dest='refclocks',
        help="Comma-separated list of Refclock IDs to build (or \"all\")",
        type='string')
    grp.add_option('--list', action='store_true', default=False,
                   help="List available Refclocks")

    grp = ctx.add_option_group("NTP developer configure options")
    grp.add_option('--build-version-tag', type='string',
                   help="Append a tag to the version string (unused)")
    grp.add_option('--cflags', type='string', action="callback",
                   callback=callback_flags,
                   help="Users should use CFLAGS in their environment.")
    grp.add_option('--build-epoch', type='int', default=None,
                   help="Force epoch, or use SOURCE_DATE_EPOCH in environment")
    grp.add_option('--ldflags', type='string', action="callback",
                   callback=callback_flags,
                   help="Users should use LDFLAGS in their environment.")
    grp.add_option('--check', action='store_true', default=False,
                   help="Run tests")
    grp.add_option('--enable-rtems-trace', action='store_true',
                   default=False, help="Enable RTEMS Trace.")
    grp.add_option('--enable-warnings', action='store_true',
                   default=False, help="Enable annoying CC warnings")
    grp.add_option('--rtems-trace-path', type='string', default="",
                   help="Path to rtems-tld.")
    grp.add_option(
        '--define', type='string', action="callback",
        callback=callback_flags,
        help="Force definition of symbol, wants value of form SYM=VAL.")
    grp.add_option('--undefine', type='string', action="callback",
                   callback=callback_flags,
                   help="Force undefinition of symbol.")

    grp = ctx.add_option_group("NTP documentation configure options")
    grp.add_option('--enable-doc', action='store_true',
                   default=False, help="Build NTP documentation")
    grp.add_option('--enable-a2x-xmllint', action='store_true',
                   default=False,
                   help="Build NTP documentation with a2x XML lint")
    grp.add_option('--disable-manpage', action='store_true',
                   default=False, help="Disable Manpage building.")
