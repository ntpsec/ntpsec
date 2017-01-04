from shlex import split


def check_fortify(ctx):
    ctx.find_program("sourceanalyzer", var="BIN_SOURCEANALYZER",
                     mandatory=True)

    if not ctx.options.fortify_flags:
        ctx.fatal(
            "You must supply --fortify-flags when using --enable-fortify.")

    ctx.env.CC = ctx.env.BIN_SOURCEANALYZER + \
        split(ctx.options.fortify_flags) + ctx.env.CC
    ctx.env.AR = ctx.env.BIN_SOURCEANALYZER + \
        split(ctx.options.fortify_flags) + ctx.env.AR
