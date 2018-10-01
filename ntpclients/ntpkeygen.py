#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
ntpkeygen - generate cryptographic keys for NTP clients and servers

All file names are like "ntpkey_<type>_<hostname>.<filestamp>", where
<type> is the file type, <hostname> the generating host name and
<filestamp> the generation time in NTP seconds. The NTP programs
expect generic names such as "ntpkey_<type>_whimsy.udel.edu" with the
association maintained by soft links. Following is a list of file
types.

ntpkey_AES_<hostname>.<filestamp>
AES (128-bit) keys used to compute CMAC mode authentcation
using shared key cryptography

The file can be edited by hand to support MD5 and SHA1 for
old digest mode authentcation.
'''

from __future__ import print_function

import os
import sys
import socket
import random
import time
import getopt
import stat
import ntp.util

#
# Cryptodefines
#
NUMKEYS = 10    # number of keys generated of each type
KEYSIZE = 16    # maximum key size


def gen_keys(ident, groupname):
    "Generate semi-random AES keys for versions of ntpd with CMAC support."
    with fheader("AES", ident, groupname) as wp:
        for i in range(1, NUMKEYS+1):
            key = ""
            for j in range(KEYSIZE):
                while True:
                    r = randomizer.randint(0x21, 0x7e)
                    if r != ord('#'):
                        break
                key += chr(r)
            wp.write("%2d AES %s\n" % (i, key))
        for i in range(1, NUMKEYS+1):
            key = ""
            for j in range(KEYSIZE):
                key += "%02x" % randomizer.randint(0x00, 0xff)
            wp.write("%2d AES %s\n" % (i + NUMKEYS, key))


#
# Generate file header and link
#
def fheader(fileid,     # file name id
            ulink,      # linkname
            owner       # owner name
            ):
    "Generate file header and link"
    try:
        filename = "ntpkey_%s_%s.%u" % (fileid, owner, int(time.time()))
        orig_umask = os.umask(stat.S_IWGRP | stat.S_IRWXO)
        wp = open(filename, "w")
        os.umask(orig_umask)

        linkname = "ntp.keys"
        if os.path.exists(linkname):
            os.remove(linkname)    # The symlink() line below matters
        os.symlink(filename, linkname)

        sys.stderr.write("Generating new %s file and link\n" % ulink)
        sys.stderr.write("%s->%s\n" % (linkname, filename))
        wp.write("# %s\n# %s\n" % (filename, time.ctime()))
        return wp
    except IOError:
        sys.stderr.write("Key file creation or link failed.\n")
        raise SystemExit(1)


if __name__ == '__main__':
    try:
        (options, arguments) = getopt.getopt(sys.argv[1:], "hMV",
                                             ["help", "version"])
    except getopt.GetoptError as e:
        print(e)
        raise SystemExit(1)

    for (switch, val) in options:
        if switch == '-M':
            # dummy MD5 option for backwards compatibility, ignored
            pass
        elif switch in ("-h", "--help"):
            print("usage: ntpkeygen")
            raise SystemExit(0)
        elif switch in ("-V", "--version"):
            print("ntpkeygen %s" % ntp.util.stdversion())
            raise SystemExit(0)

    # The seed is ignored by random.SystemRandom,
    # even though the docs do not say so.
    randomizer = random.SystemRandom()
    gen_keys("AES", socket.gethostname())
    raise SystemExit(0)

# end
