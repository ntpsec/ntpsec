# Some compilers decide they like to mimic GCC as close as possible.
# Unfortunately this usually does not apply to #pragma's.  There can also be
# quirks that need to be dodged specific to a compiler.
#
# There are also differences when it comes to coverage and profiling between
# compilers.  We'll want to support all to see how the source reacts under
# different compilers and architectures.
#
# This file exist as the definitive way to check what compiler we're working
# under it defines COMPILER_X in both config.h and the build system.

COMPILER_FRAG = """
#include <stdio.h>

int main(void) {
#ifdef __clang__
    printf("1");
#elif __INTEL_COMPILER
    printf("2");
#elif __GNUC__
    printf("3");
#elif __SUNPRO_C
    printf("4");
#else
    printf("255");
#endif
    return 0;
}
"""


def check_compiler(ctx):

    if ctx.env.ENABLE_CROSS:
        return

    defines = {
        1: ("COMPILER_CLANG",   "clang"),
        2: ("COMPILER_ICC",     "ICC"),
        3: ("COMPILER_GCC",     "GCC"),
        4: ("COMPILER_SUNCC",   "SUNCC"),
        255: ("COMPILER_GCC",   "Unknown (Defaulting to GCC)"),
    }

    ctx.check_cc(
        fragment=COMPILER_FRAG,
        msg="Checking compiler",
        define_name="COMPILER_INT",
        quote=False,
        execute=True,
        define_ret=True,
        mandatory=True,
    )

    compiler_int = int(ctx.get_define("COMPILER_INT"))

    ctx.undefine("COMPILER_INT")    # Not needed.

    define, name = defines[compiler_int]

    ctx.start_msg("Compiler found")

    # config.h
    ctx.define(define, 1, comment="Compiler detected during configure.")
    ctx.env[define] = True    # Build system.

    ctx.end_msg(name)
