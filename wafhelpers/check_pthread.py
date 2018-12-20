from waflib.Logs import pprint

PTHREAD_FRAG = """
#include <pthread.h>
int main(void) {
        pthread_mutex_t mutex;
        pthread_mutex_init(&mutex, NULL);
        pthread_mutex_destroy(&mutex);
        return 0;
}

"""


def check_pthread_header_lib(ctx):
    if ctx.options.disable_dns_lookup:
      # threads only used by DNS lookup
      # libcrypto uses pthread_once, but that's not our problem
      return
    ctx.check(header_name="pthread.h", includes=ctx.env.PLATFORM_INCLUDES,
              mandatory=False, comment="pthread header")
    ctx.check(feature="c cshlib", lib="pthread",
              libpath=ctx.env.PLATFORM_LIBPATH, mandatory=False,
              comment="pthread library")
    ctx.check_cc(lib="thr", mandatory=False,
                 comment="thr library, required by some operating systems.")

    if (ctx.get_define("HAVE_PTHREAD_H") and
            (ctx.env.LIB_PTHREAD or ctx.env.LIB_THR)):
        ctx.env.PTHREAD_HEADER_LIB = True


def check_pthread_run(ctx):
    if ctx.env.ENABLE_CROSS:
        if ctx.env.PTHREAD_HEADER_LIB:  # XXX Remove when variant builds exist
            ctx.define("HAVE_PTHREAD", 1, comment="pthread support")
            ctx.env.PTHREAD_ENABLE = True
        return

    ctx.check(
        fragment=PTHREAD_FRAG,
        define_name="HAVE_PTHREAD",
        features="c",
        use="PTHREAD THR",
        msg="Checking if pthread works",
        includes=ctx.env.PLATFORM_INCLUDES,
        export_includes=ctx.env.PLATFORM_INCLUDES,
        mandatory=False,
        comment="pthread support"
    )

    check_sanity(ctx, ctx.env.PTHREAD_HEADER_LIB, "pthread")

    # XXX if pthread is part of 'libc' this will pass
    # even if the header isn't detected.
    if not ctx.get_define("HAVE_PTHREAD"):
        ctx.fatal("Error: POSIX threads are required in order to build.")
    else:
        ctx.env.PTHREAD_ENABLE = True
        ctx.define("HAVE_PTHREAD", 1, comment="pthread support")


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
