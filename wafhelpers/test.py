# Copyright the NTPsec project contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from __future__ import print_function

from waflib.Logs import pprint
from pylib.poly import polystr


def test_write_log(ctx):
    file_out = "%s/test.log" % ctx.bldnode.abspath()

    log = getattr(ctx, 'utest_results', [])

    if not log:
        return

    with open(file_out, "w") as fp:
        for binary, retval, lines, error in ctx.utest_results:
            fp.write("BINARY      : %s\n" % binary)
            fp.write("RETURN VALUE: %s\n" % retval)
            fp.write("\n*** stdout ***\n")
            fp.write(polystr(lines))
            fp.write("\n*** stderr ***\n")
            fp.write(polystr(error))
            fp.write("\n\n\n")

    pprint("BLUE", "Wrote test log to: ", file_out)


def test_print_log(ctx):
    for binary, retval, lines, error in ctx.utest_results:
        pprint("YELLOW", "BINARY      :", binary)
        pprint("YELLOW", "RETURN VALUE:", retval)
        print("")

        if retval or error:
            pprint("RED", "****** ERROR ******\n")

            print("\n*** stdout ***\n")
            print(polystr(lines))
            print("\n*** stderr ***\n")
            print(polystr(error))

        if (not retval) and (not error):
            pprint("GREEN", "****** LOG ******\n", polystr(lines))

        print("")
