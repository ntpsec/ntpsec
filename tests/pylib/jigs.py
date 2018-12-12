#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function, division

import socket
import select
import os.path
import time
import ntp.poly


class FileJig:
    def __init__(self, returns=[""]):
        self.data = []
        self.flushed = False
        self.readline_return = returns

    def __enter__(self):
        return self

    def __exit__(self, *args):
        return False

    def __iter__(self):
        return self.readline_return.__iter__()

    def write(self, data):
        self.data.append(data)
        self.flushed = False

    def flush(self):
        self.flushed = True

    def readline(self):
        if len(self.readline_return) > 0:
            return self.readline_return.pop(0)
        return ""

    def readlines(self):
        ret = self.readline_return
        self.readline_return = []
        return ret


class SocketJig:
    def __init__(self):
        self.data = []
        self.return_data = []
        self.closed = False
        self.connected = None
        self.fail_connect = False
        self.fail_send = 0

    def sendall(self, data):
        if self.fail_send > 0:
            self.fail_send -= 1
            raise socket.error()
        self.data.append(data)

    def close(self):
        self.closed = True

    def connect(self, addr):
        if self.fail_connect is True:
            err = socket.error()
            err.strerror = "socket!"
            err.errno = 16
            raise err
        self.connected = addr

    def recv(self, bytecount):
        if len(self.return_data) > 0:
            current = self.return_data.pop(0)
            if len(current) > bytecount:
                ret = current[:bytecount]
                current = current[bytecount:]
                self.return_data.insert(0, current)  # push unwanted data
                return ret
            else:
                return current
        return None


class HasherJig:
    def __init__(self):
        self.update_calls = []
        self.digest_size = 0

    def update(self, data):
        self.update_calls.append(data)
        if len(data) > 0:
            self.digest_size += 1

    def digest(self):
        return ntp.poly.polybytes("blah" * 4)  # 16 byte hash


class SocketModuleJig:
    error = socket.error
    gaierror = socket._socket.gaierror
    herror = socket.herror
    SOCK_DGRAM = socket.SOCK_DGRAM
    IPPROTO_UDP = socket.IPPROTO_UDP
    AF_UNSPEC = socket.AF_UNSPEC
    AI_NUMERICHOST = socket.AI_NUMERICHOST
    AI_CANONNAME = socket.AI_CANONNAME
    EAI_NONAME = socket.EAI_NONAME
    if hasattr(socket, "EAI_NODATA"):
        EAI_NODATA = socket.EAI_NODATA
    else:  # FreeBSD does not have EAI_NODATA (removed from POSIX)
        EAI_NODATA = None
    NI_NAMEREQD = socket.NI_NAMEREQD

    def __init__(self):
        self.gai_calls = []
        self.gai_error_count = 0
        self.gai_returns = []
        self.gni_calls = []
        self.gni_error_count = 0
        self.gni_returns = []
        self.socket_calls = []
        self.socket_fail = False
        self.socket_fail_connect = False
        self.socketsReturned = []
        self.inet_ntop_calls = []
        self.getfqdn_calls = []
        self.getfqdn_returns = []
        self.ghba_calls = []
        self.ghba_returns = []

    def getaddrinfo(self, host, port, family=None, socktype=None,
                    proto=None, flags=None):
        self.gai_calls.append((host, port, family, socktype, proto, flags))
        if self.gai_error_count > 0:
            self.gai_error_count -= 1
            err = self.gaierror("blah")
            err.errno = socket.EAI_NONAME
            raise err
        return self.gai_returns.pop(0)

    def getnameinfo(self, addr, flags):
        self.gni_calls.append((addr, flags))
        if self.gni_error_count > 0:
            self.gni_error_count -= 1
            err = self.gaierror("blah")
            err.errno = socket.EAI_NONAME
            raise err
        return self.gni_returns.pop(0)

    def socket(self, family, socktype, protocol):
        self.socket_calls.append((family, socktype, protocol))
        if self.socket_fail is True:
            err = self.error()
            err.strerror = "error!"
            err.errno = 23
            raise err
        sock = SocketJig()
        if self.socket_fail_connect is True:
            sock.fail_connect = True
        self.socketsReturned.append(sock)
        return sock

    def inet_ntop(self, addr, family):
        self.inet_ntop_calls.append((addr, family))
        return "canon.com"

    def getfqdn(self, name=""):
        self.getfqdn_calls.append(name)
        return self.getfqdn_returns.pop(0)

    def gethostbyaddr(self, addr):
        self.ghba_calls.append(addr)
        ret = self.ghba_returns.pop(0)
        if ret is None:
            raise self.herror
        return ret


class GetpassModuleJig:
    def __init__(self):
        self.getpass_calls = []
        self.getpass_returns = []

    def getpass(self, prompt, stream=None):
        self.getpass_calls.append((prompt, stream))
        return self.getpass_returns.pop(0)


class HashlibModuleJig:
    def __init__(self):
        self.new_calls = []
        self.hashers_returned = []

    def new(self, name):
        self.new_calls.append(name)
        h = HasherJig()
        self.hashers_returned.append(h)
        return h


class SelectModuleJig:
    error = select.error

    def __init__(self):
        self.select_calls = []
        self.select_fail = 0
        self.do_return = []
        self.fqdn_calls = []
        self.fqdn_returns = []

    def select(self, ins, outs, excepts, timeout=0):
        self.select_calls.append((ins, outs, excepts, timeout))
        if self.select_fail > 0:
            self.select_fail -= 1
            raise select.error
        if len(self.do_return) == 0:  # simplify code that doesn't need it
            self.do_return.append(True)
        doreturn = self.do_return.pop(0)
        if doreturn is True:
            return (ins, [], [])
        else:
            return ([], [], [])

    def getfqdn(self, name=""):
        self.fqdn_calls.append(name)
        return self.fqdn_returns.pop(0)


class path_mod:
    def __init__(self):
        self.isdir_calls = []
        self.isdir_returns = []
        self.getmtime_calls = []
        self.getmtime_returns = []
        self.join_calls = []

    def isdir(self, dirname):
        self.isdir_calls.append(dirname)
        return self.isdir_returns.pop(0)

    def join(self, *args):
        self.join_calls.append(args)
        return os.path.join(*args)

    def basename(self, pathname):
        self.join_calls.append(pathname)
        return os.path.basename(pathname)

    def getmtime(self, filename):
        self.getmtime_calls.append(filename)
        return self.getmtime_returns.pop(0)


class OSModuleJig:
    def __init__(self):
        self.isatty_calls = []
        self.isatty_returns = []
        self.path = path_mod()  # Need os.path

    def isatty(self, fd):
        self.isatty_calls.append(fd)
        return self.isatty_returns.pop(0)


class FcntlModuleJig:
    def __init__(self):
        self.ioctl_calls = []
        self.ioctl_returns = []

    def ioctl(self, fd, op, arg=0, mutate_flag=False):
        self.ioctl_calls.append((fd, op, arg, mutate_flag))
        return self.ioctl_returns.pop(0)


class ShutilModuleJig:
    def __init__(self):
        self.gts_calls = []
        self.gts_returns = []

    def get_terminal_size(self, default=(80, 24)):
        self.gts_calls.append(default)
        return self.gts_returns.pop(0)


class TimeModuleJig:
    def __init__(self):
        self.time_calls = 0
        self.time_returns = []

    def time(self):
        self.time_calls += 1
        return self.time_returns.pop(0)

    def strftime(self, f, t=None):
        if t is None:
            return time.strftime(f)
        else:
            return time.strftime(f, t)

    def gmtime(self, *args, **kwargs):
        return time.gmtime(*args, **kwargs)


class GzipModuleJig:
    def __init__(self):
        self.open_calls = []
        self.files_returned = []

    def open(self, filename, filemode):
        self.open_calls.append((filename, filemode))
        return self.files_returned.pop(0)


class GlobModuleJig:
    def __init__(self):
        self.glob_calls = []
        self.glob_returns = []

    def glob(self, pathname):
        self.glob_calls.append(pathname)
        ret = self.glob_returns.pop(0)
        return ret
