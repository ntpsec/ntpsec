#!/usr/bin/env python
"""\
make-leap-seconds.py - make leap second file for testing

Optional args are date of leap second: YYYY-MM-DD
and expiration date of file.

Defaults are start of tomorrow (UTC), and 28 days after the leap.
"Start of tomorow" is as soon as possible for testing.

"""
# SPDX-License-Identifier: BSD-2-Clause
from __future__ import print_function, division

import sha, sys, time, datetime

master_encoding = 'latin-1'

if str is bytes:  # Python 2
    polystr = str
    polybytes = bytes

    def string_escape(s):
        return s.decode('string_escape')

    def make_wrapper(fp):
        return fp

else:  # Python 3
    import io

    def polystr(o):
        "Polymorphic string factory function"
        if isinstance(o, str):
            return o
        if not isinstance(o, bytes):
            return str(o)
        return str(o, encoding=master_encoding)

    def polybytes(s):
        "Polymorphic string encoding function"
        if isinstance(s, bytes):
            return s
        if not isinstance(s, str):
            return bytes(s)
        return bytes(s, encoding=master_encoding)

    def string_escape(s):
        "Polymorphic string_escape/unicode_escape"
        # This hack is necessary because Unicode strings in Python 3 don't
        # have a decode method, so there's no simple way to ask it for the
        # equivalent of decode('string_escape') in Python 2. This function
        # assumes that it will be called with a Python 3 'str' instance
        return s.encode(master_encoding).decode('unicode_escape')

    def make_wrapper(fp):
        "Wrapper factory function to enforce master encoding"
        # This can be used to wrap normally binary streams for API
        # compatibility with functions that need a text stream in
        # Python 3; it ensures that the binary bytes are decoded using
        # the master encoding we use to turn bytes to Unicode in
        # polystr above
        # newline="\n" ensures that Python 3 won't mangle line breaks
        return io.TextIOWrapper(fp, encoding=master_encoding, newline="\n")

    def make_std_wrapper(stream):
        "Standard input/output wrapper factory function"
        # This ensures that the encoding of standard output and standard
        # error on Python 3 matches the master encoding we use to turn
        # bytes to Unicode in polystr above
        # line_buffering=True ensures that interactive command sessions
        # work as expected
        return io.TextIOWrapper(stream.buffer, encoding=master_encoding,
                                newline="\n", line_buffering=True)

    sys.stdin = make_std_wrapper(sys.stdin)
    sys.stdout = make_std_wrapper(sys.stdout)
    sys.stderr = make_std_wrapper(sys.stderr)

JAN_1970 = 2208988800   # convert Unix/POSIX epoch to NTP epoch
epoch = datetime.datetime.utcfromtimestamp(0)
args = sys.argv[1:]

leap = time.time()
days = int(leap/86400)
leap = (days+1)*86400

if len(args) > 0:
  leapdate = datetime.datetime.strptime(args[0], "%Y-%m-%d")
  leap = (leapdate - epoch).total_seconds()
  leap = int(leap)
  args = args[1:]

expire = leap + 28*86400
if len(args) > 0:
  expiredate = datetime.datetime.strptime(args[0], "%Y-%m-%d")
  expire = (expiredate - epoch).total_seconds()
  expire = int(expire)
  args = args[1:]
leap_txt = time.asctime(time.gmtime(leap))
leap = str(leap+JAN_1970)
expire_txt = time.asctime(time.gmtime(expire))
expire=str(expire+JAN_1970)

update = int(time.time())
update_txt = time.asctime(time.gmtime(update))
update = str(update+JAN_1970)


tai = "40"          # hardwired

# File format
#
#  # is comment
#  #$ xxx  Update Date
#  #@ xxx  Expiration Date
#  #h SHA1 hash of payload
#
#  #$       3676924800
#  #@       3707596800
#  2272060800      10      # 1 Jan 1972
#  #h      dacf2c42 2c4765d6 3c797af8 2cf630eb 699c8c67
#
#  All dates use NTP epoch of 1900-01-01

sha1 = sha.new()
print("%s %s  # %s" % (leap, tai, leap_txt))
sha1.update(leap)
sha1.update(tai)
print("#@ %s  # %s" % (expire, expire_txt))
sha1.update(expire)
print("#$ %s  # %s" % (update, update_txt))
sha1.update(update)
digest = sha1.hexdigest()
print("#h %s %s %s %s %s" % \
 (digest[0:8], digest[8:16], digest[16:24], digest[24:32], digest[32:40]))

# end
