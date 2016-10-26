# Common utility functions

from __future__ import print_function

import socket
import sys
import subprocess

def canonicalize_dns(hostname):
    try:
        ai = socket.getaddrinfo(hostname, None, 0, 0, 0, socket.AI_CANONNAME)
    except socket.gaierror as e:
        print('getaddrinfo failed: %s' % e.strerr, file=sys.stderr)
        raise SystemExit(1)
    (family, socktype, proto, canonname, sockaddr) = ai[0]
    try:
        name = socket.getnameinfo(sockaddr, socket.NI_NAMEREQD)
    except socket.gaierror:
        return canonname.lower()
    return name[0].lower()

def termsize():
    "Return the current terminal size."
    # Should work under Linux and Solaris at least.
    # Alternatives at http://stackoverflow.com/questions/566746/how-to-get-console-window-width-in-python
    import shlex, subprocess, re
    output = subprocess.check_output(shlex.split('/bin/stty -a'))
    m = re.search('rows\D+(\d+); columns\D+(\d+);', output)
    if m:
        return int(m.group(1)), int(m.group(2))
    else:
        return (24, 80)

# end
