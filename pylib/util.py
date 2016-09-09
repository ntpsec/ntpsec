# Common utility functions

from __future__ import print_function

import socket

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

# end
