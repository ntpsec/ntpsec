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

import datetime
import hashlib
import sys
import time


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
expire = str(expire+JAN_1970)

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

sha1 = hashlib.sha1()
print("%s %s  # %s" % (leap, tai, leap_txt))
sha1.update(leap)
sha1.update(tai)
print("#@ %s  # %s" % (expire, expire_txt))
sha1.update(expire)
print("#$ %s  # %s" % (update, update_txt))
sha1.update(update)
digest = sha1.hexdigest()
print("#h %s %s %s %s %s" %
      (digest[0:8], digest[8:16], digest[16:24], digest[24:32], digest[32:40]))

# end
