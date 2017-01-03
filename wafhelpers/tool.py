from waflib.Logs import pprint


def check_sanity(ctx, cond, lib):
        define = "HAVE_%s" % lib.upper()

        if cond and (not ctx.get_define(define)):
                pprint("RED",
                       "Warning %s headers detected, binaries do not build/run"
                       % lib)
        elif (not cond) and ctx.get_define(define):
                pprint("RED",
                       "Warning %s headers not detected, binaries build/run"
                       % lib)
