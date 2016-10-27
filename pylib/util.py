# Common utility functions

from __future__ import print_function

import socket
import sys
import subprocess

def canonicalize_dns(hostname):
    portsuffix = ""
    if hostname.count(":") == 1:		# IPv4 with appended port
        (hostname, portsuffix) = hostname.split(":")
        portsuffix = ":" + portsuffix
    elif ']' in hostname:			# IPv6
        rbrak = hostname.rindex("]")
        if ":" in hostname[rbrak:]:
            portsep =  hostname.rindex(":")
            portsuffix = hostname[portsep:]
            hostname = hostname[:portsep]
            hostname = hostname[1:-1]	# Strip brackets
    try:
        ai = socket.getaddrinfo(hostname, None, 0, 0, 0, socket.AI_CANONNAME)
    except socket.gaierror as (s, _e):
        print('getaddrinfo failed: %s' % s, file=sys.stderr)
        raise SystemExit(1)
    (family, socktype, proto, canonname, sockaddr) = ai[0]
    try:
        name = socket.getnameinfo(sockaddr, socket.NI_NAMEREQD)
    except socket.gaierror:
        return canonname.lower() + portsuffix
    return name[0].lower() + portsuffix

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
