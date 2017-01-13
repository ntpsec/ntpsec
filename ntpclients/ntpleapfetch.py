#!/usr/bin/env python
"""\
ntpleapfetch - leap-seconds file manager/updater

Usage: ntpleapfetch [-46] [-c command] [-e expiry] [-f config] [-F]
                    [-h|--help] [-i interval] [-l] [-L] [-p 4|6]
                    [-P syslog_facility] [-q] [-r retries] [-s source-url]
                    [-t tmpfile] [-v] [-z path] [-Z] [localfile]

See the manual page for details.
"""
# SPDX-License-Identifier: BSD-2-Clause
# This code runs identically under Python 2 and Python 3. Keep it that way!
# Python translation of a bash script written by Timothe Litt (litt at acm dot
# org)

# The follow getopt flags are legacy will generate a warning to stderr, if
# used:
# -4, -6, -c, -l, -L, -p, -P, -t, -z, -Z

# Finally, logging to syslog by default was a design error, violating
# Unix principles; that has been fixed. To get this behavior when
# running in a script, redirect standard error to logger(1).

from __future__ import absolute_import, division, print_function

import getopt
import hashlib
import os
import os.path
import sys
import tempfile
import time

try:
    from urllib.request import urlopen, URLError
except ImportError:
    from urllib2 import urlopen, URLError
try:
    import selinux
    HAS_SELINUX = selinux.is_selinux_enabled()
except ImportError:
    HAS_SELINUX = False

try:
    options, arguments = getopt.getopt(sys.argv[1:],
                                       "46c:e:f:Fhi:lLp:P:qr:s:t:vz:Z",
                                       ["help"])
except getopt.GetoptError as e:
    sys.stderr.write("%s\n" % e)
    raise SystemExit(1)

# Where to get the file
leapsrc = "https://hpiers.obspm.fr/iers/bul/bulc/ntp/leap-seconds.list"
# Alternatives: ftp://time.nist.gov/pub/leap-seconds.list,
#             https://www.ietf.org/timezones/data/leap-seconds.list

# How many times to try to download new file
maxtries = 6
interval = 10

# Where to find ntp config file
ntpconf = "/etc/ntp.conf"

# How long before expiration to get updated file (in days)
prefetch = 60

# Where to put temporary copy before it's moved into place
tmpfile = tempfile.NamedTemporaryFile(delete=False)

cronjob = os.getenv("CRONJOB", False)
if cronjob == "1":
    sys.stderr.write("WARNING: environment variable CRONJOB is ignored\n")

# Quiet mode
quiet = False

# Path to leapfile, determine from ntp.conf if possible
leapfile = None

# Force updates no matter what
force = False

# Verbose output
verbose = False

for (switch, val) in options:
    try:
        if switch in ["-4", "-6", "-c", "-l", "-L", "-p", "-P", "-t", "-z",
                      "-Z"]:
            sys.stderr.write("legacy flag %s is ignored\n" % switch)
        elif switch == "-e":
            prefetch = int(val)
        elif switch == "-f":
            ntpconf = val
        elif switch == "-F":
            force = True
        elif switch == "-i":
            interval = int(val)
        elif switch == "-q":
            quiet = True
        elif switch == "-r":
            maxtries = int(val)
        elif switch == "-s":
            leapsrc = val
        elif switch == "-v":
            verbose = True
        elif switch == "-h" or switch == "--help":
            print(__doc__, file=sys.stderr)
            raise SystemExit(0)
    except ValueError:
        sys.stderr.write("Invalid argument.\n")
        sys.stderr.write(__doc__)
        raise SystemExit(1)

if len(arguments):
    leapfile = arguments[0]


def verify_leapfile(filename, expiration_prefetch):
    """
    Validate a leap-seconds file checksum

    File format: (full description in files)
    # marks comments, except:
    #$ number : the NTP date of the last update
    #@ number : the NTP date that the file expires
    Date (seconds since 1900) leaps : leaps is the # of seconds to add for
      times >= Date
    Date lines have comments.
    #h hex hex hex hex hex is the SHA-1 checksum of the data & dates, excluding
      whitespace w/o leading zeroes
    """
    data = ''
    lastupdate = None
    expiration = None
    filehash = None

    try:
        for line in open(filename):
            if '#$' in line:
                lastupdate = line.partition('#$')[2].strip()
                data += lastupdate
            elif '#@' in line:
                expiration = line.partition('#@')[2].strip()
                data += expiration
            elif '#h' in line:
                filehash = line.partition('#h')[2].strip().replace(' ', '')
            else:
                data += line.partition('#')[0]
    except IOError as e:
        sys.stderr.write("Unable to open %s: %s\n" % (filename, e))
        return False

    data = data.replace(' ', '').replace('\t', '')

    datahash = hashlib.sha1(data).hexdigest()

    if filehash == datahash:
        if verbose:
            sys.stderr.write("Checksum of %s validated\n" % filename)
    else:
        sys.stderr.write("Checksum of %s is invalid\n" % filename)
        if not filehash:
            filehash = "(no checksum record found in file)"
        sys.stderr.write("EXPECTED: %s\n" % filehash)
        sys.stderr.write("COMPUTED: %s\n" % datahash)
        return False

    # Convert from NTP epoch to Unix epoch (70 year offset)
    expiration = int(expiration) - 2208988800
    # Convert days to seconds
    expiration_prefetch *= 86400
    # Check expiration date
    if expiration < time.mktime(time.gmtime()):
        sys.stderr.write("File expired on %s\n" %
                         time.strftime("%a, %d %b %Y %H:%M:%S +0000",
                                       time.gmtime(expiration)))
        return False
    elif expiration < time.mktime(time.gmtime()) + expiration_prefetch:
        sys.stderr.write("File will expire on %s\n" %
                         time.strftime("%a, %d %b %Y %H:%M:%S +0000",
                                       time.gmtime(expiration)))
        return False

    return True

if __name__ == '__main__':
    # Parse ntp.conf for leapfile directive
    ntpleapfile = None
    try:
        for line in open(ntpconf):
            if 'leapfile' in line:
                ntpleapfile = line.partition('leapfile')[2].strip()
    except IOError as e:
        sys.stderr.write("Unable to open ntp configuration file %s: %s\n" %
                         (ntpconf, e))
        raise SystemExit(1)

    if not ntpleapfile:
        sys.stderr.write("%s does not specify a leapfile\n" % ntpconf)

    # Allow placing the file someplace else - testing
    if leapfile:
        if leapfile != ntpleapfile:
            sys.stderr.write("Requested install to %s, but %s specifies %s\n" %
                             (leapfile, ntpconf, ntpleapfile))
        ntpleapfile = leapfile
    else:
        if ntpleapfile:
            leapfile = ntpleapfile
        else:
            sys.stderr.write("No leapfile target location specified\n")
            raise SystemExit(1)

    # Verify the current file
    # If it validates and is not expired/is not expiring soon, exit
    # Otherwise, download a new one
    if not force and os.path.exists(ntpleapfile) and verify_leapfile(ntpleapfile, prefetch):
        if not quiet:
            sys.stderr.write("Not time to replace %s\n" % ntpleapfile)
        raise SystemExit(0)

    iteration = 0
    while True:
        iteration += 1
        if verbose:
            sys.stderr.write("Attempting download from %s, try %d..\n" %
                             (leapsrc, iteration))

        try:
            urldata = urlopen(leapsrc)
        except URLError as e:
            if iteration > maxtries:
                break
            if verbose:
                if hasattr(e, 'reason'):
                    sys.stderr.write("Download failed: %s\n" % e.reason)
                elif hasattr(e, 'code'):
                    sys.stderr.write("Download failed: error code %s\n" %
                                     e.code)
                if not quiet:
                    sys.stderr.write("Waiting %d minutes before retrying...\n" %
                                     interval)
            time.sleep(interval * 60)

        if not quiet:
            sys.stderr.write("Download of %s succeeded\n" % leapsrc)

        tmpfile.write(urldata.read())
        tmpfile.close()
        if not verify_leapfile(tmpfile.name, prefetch):
            # There is no point in retrying, as the file on the server is
            # almost certainly corrupt.
            sys.stderr.write("Downloaded file %s rejected -- saved for diagnosis\n" %
                             tmpfile.name)
            raise SystemExit(1)

        # Set correct permissions on temporary file
        reference_file = ntpleapfile
        if not os.path.exists(ntpleapfile):
            if not quiet:
                sys.stderr.write("%s was missing, creating new copy - check permissions\n" %
                                 leapfile)
            open(leapfile, 'w').close()
            # Can't copy permissions from old file, copy from ntpconf instead
            reference_file = ntpconf
        statinfo = os.stat(reference_file)
        try:
            os.chmod(tmpfile.name, statinfo.st_mode)
        except OSError as e:
            sys.stderr.write("chmod of %s failed: %s\n" %
                             (tmpfile.name, e.strerror))
            raise SystemExit(1)
        try:
            os.chown(tmpfile.name, statinfo.st_uid, statinfo.st_gid)
        except OSError as e:
            sys.stderr.write("chown of %s failed: %s\n" %
                             (tmpfile.name, e.strerror))
            raise SystemExit(1)
        if HAS_SELINUX:
            try:
                status, context = selinux.getfilecon(reference_file)
            except OSError as e:
                sys.stderr.write("Unable to get SELINUX context for %s: %s" %
                                 (reference_file, e.strerror))
                raise SystemExit(1)
            try:
                selinux.setfilecon(tmpfile.name, context)
            except OSError as e:
                sys.stderr.write("Unable to set SELINUX context for %s: %s" %
                                 (tmpfile.name, e.strerror))
                raise SystemExit(1)

        # Replace current file with validated new one
        try:
            os.rename(tmpfile.name, leapfile)
            sys.stderr.write("Installed new %s from %s\n" %
                             (leapfile, leapsrc))
        except OSError:
            sys.stderr.write("Install %s => %s failed -- saved for diagnosis\n" %
                             (tmpfile.name, leapfile))
            raise SystemExit(1)

        raise SystemExit(0)

    # Failed and out of retries
    sys.stderr.write("Download from %s failed after %d attempts\n" %
                     (leapsrc, iteration))
    try:
        os.remove(tmpfile.name)
    except OSError:
        pass
    raise SystemExit(1)
# EOF
