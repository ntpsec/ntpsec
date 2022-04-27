# -*- coding: utf-8 -*-
# SPDX-License-Identifier: BSD-2-Clause
"""Access libntp funtions from Python."""
from __future__ import absolute_import
import ctypes
import ctypes.util
import errno
import os
import os.path
import sys
import ntp.poly

LIB = 'ntpc'


def _fmt():
    """Produce library naming scheme."""
    if sys.platform.startswith('darwin'):
        return 'lib%s.dylib'
    if sys.platform.startswith('win32'):
        return '%s.dll'
    if sys.platform.startswith('cygwin'):
        return 'lib%s.dll'
    return 'lib%s.so'


def _importado():
    """Load the ntpc library or throw an OSError trying."""
    ntpc_paths = []         # places to look

    j = __file__.split(os.sep)[:-1]
    ntpc_paths.append(os.sep.join(j + [_fmt() % LIB]))

    ntpc_path = ctypes.util.find_library(LIB)
    if ntpc_path:
        ntpc_paths.append(ntpc_path)

    return _dlo(ntpc_paths)


def _dlo(paths):
    """Try opening library from a list."""
    for ntpc_path in paths:
        try:
            lib = ctypes.CDLL(ntpc_path, use_errno=True)
            wrap_version = "@NTPSEC_VERSION_EXTENDED@"
            clib_version = ntp.poly.polystr(ctypes.c_char_p.in_dll(lib, 'version').value)
            if clib_version != wrap_version:
                sys.stderr.write("ntp.ntpc wrong version '%s' != '%s'\n" % (clib_version, wrap_version))
            return lib
        except OSError:
            pass
    raise OSError("Can't find %s library" % LIB)


_ntpc = _importado()
progname = ctypes.c_char_p.in_dll(_ntpc, 'progname')
# log_sys = ctypes.c_bool.in_dll(_ntpc, 'syslogit')
# log_term = ctypes.c_bool.in_dll(_ntpc, 'termlogit')
# log_pid = ctypes.c_bool.in_dll(_ntpc, 'termlogit_pid')
# log_time = ctypes.c_bool.in_dll(_ntpc, 'msyslog_include_timestamp')

TYPE_SYS = ctypes.c_int.in_dll(_ntpc, 'SYS_TYPE').value
TYPE_PEER = ctypes.c_int.in_dll(_ntpc, 'PEER_TYPE').value
TYPE_CLOCK = ctypes.c_int.in_dll(_ntpc, 'CLOCK_TYPE').value


def checkname(name):
    """Check if name is a valid algorithm name."""
    _ntpc.do_checkname.restype = ctypes.c_int
    mid_bytes = ntp.poly.polybytes(name)
    _ntpc.do_checkname.argtypes = [ctypes.c_char_p]
    return _ntpc.do_checkname(mid_bytes)


def mac(data, key, name):
    """Compute HMAC or CMAC from data, key, and algorithm name."""
    resultlen = ctypes.c_size_t()
    result = (ctypes.c_ubyte * 64)()
    result.value = b'\0' * 64
    _ntpc.do_mac.restype = None
    _ntpc.do_mac(ntp.poly.polybytes(name),
                 ntp.poly.polybytes(data), len(data),
                 ntp.poly.polybytes(key), len(key),
                 ctypes.byref(result), ctypes.byref(resultlen))
    return ntp.poly.polybytes(bytearray(result)[:resultlen.value])


def setprogname(in_string):
    """Set program name for logging purposes."""
    mid_bytes = ntp.poly.polybytes(in_string)
    _setprogname(mid_bytes)


def _lfp_wrap(callback, in_string):
    """NTP l_fp to other Python-style format."""
    mid_bytes = ntp.poly.polybytes(in_string)
    out_value = callback(mid_bytes)
    err = ctypes.get_errno()
    if err == errno.EINVAL:
        raise ValueError('ill-formed hex date')
    return out_value


def statustoa(i_type, i_st):
    """Convert a time stamp to something readable."""
    mid_str = _statustoa(i_type, i_st)
    return ntp.poly.polystr(mid_str)


def prettydate(in_string):
    """Convert a time stamp to something readable."""
    mid_str = _lfp_wrap(_prettydate, in_string)
    return ntp.poly.polystr(mid_str)


def lfptofloat(in_string):
    """NTP l_fp to Python-style float time."""
    return _lfp_wrap(_lfptofloat, in_string)


def msyslog(level, in_string):
    """Log send a message to terminal or output."""
    mid_bytes = ntp.poly.polybytes(in_string)
    _msyslog(level, mid_bytes)


# Set return type and argument types of hidden ffi handlers
_msyslog = _ntpc.msyslog
_msyslog.restype = None
_msyslog.argtypes = [ctypes.c_int, ctypes.c_char_p]

_setprogname = _ntpc.ntpc_setprogname
_setprogname.restype = None
_setprogname.argtypes = [ctypes.c_char_p]

_prettydate = _ntpc.ntpc_prettydate
_prettydate.restype = ctypes.c_char_p
_prettydate.argtypes = [ctypes.c_char_p]

_lfptofloat = _ntpc.ntpc_lfptofloat
_lfptofloat.restype = ctypes.c_double
_lfptofloat.argtypes = [ctypes.c_char_p]

# Status string display from peer status word.
_statustoa = _ntpc.statustoa
_statustoa.restype = ctypes.c_char_p
_statustoa.argtypes = [ctypes.c_int, ctypes.c_int]

# Adjust system time by slewing.
adj_systime = _ntpc.ntpc_adj_systime
adj_systime.restype = ctypes.c_bool
adj_systime.argtypes = [ctypes.c_double]

# Adjust system time by stepping.
step_systime = _ntpc.ntpc_step_systime
step_systime.restype = ctypes.c_bool
step_systime.argtypes = [ctypes.c_double]
