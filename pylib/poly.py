#!/usr/bin/env python
# -*- coding: utf-8 -*-
# SPDX-License-Identifier: BSD-2-clause
"""Handle bytes and strings in a polyglot fashion.

copied from ../ntpclient/ntpq.py  which got it from
https://gitlab.com/esr/practical-python-porting/blob/master/polystr-inclusion.py
see http://www.catb.org/esr/faqs/practical-python-porting/ for more information.
"""
import sys

master_encoding = 'latin-1'

# General notes on Python 2/3 compatibility:
#
# This code uses the following strategy to allow it to run on both Python 2
# and Python 3:
#
# - Use latin-1 encoding to transform binary data to/from Unicode when
#   necessary for operations where Python 3 expects Unicode; the
#   polystr and polybytes functions are used to do this so that
#   when running on Python 2, the byte string data is used unchanged.
#
# - Construct custom stdout and stderr streams when running
#   on Python 3 that force UTF-8 encoding, and wrap them around the
#   underlying binary buffers (in Python 2, the streams are binary
#   and are used unchanged); this ensures that the same transformation
#   is done on data from/to the standard streams, as is done on binary
#   data from/to files and subprocesses; the make_std_wrapper function
#   does this.
#
# anyone that changes this needs to test with all combinations of
# python2, python3, LC_ALL=ascii, LC_ALL=latin-1, LC_ALL=en_US.utf8, and
# piping output to a file.  While looking at the UTF-8 in the output.

forced_utf8 = False

if str is bytes:  # Python 2
    polystr = str
    polyunicode = unicode
    polybytes = bytes
    polyord = ord
    polychr = str
    polyinput = raw_input

    def string_escape(s):
        """String_escape/unicode_escape."""
        return s.decode('string_escape')

else:  # Python 3
    import io

    polyinput = input

    def polystr(o):
        """Polymorphic string factory function."""
        if isinstance(o, str):
            return o
        if not isinstance(o, bytes):
            return str(o)
        return str(o, encoding=master_encoding)

    polyunicode = polystr

    def polybytes(s):
        """Polymorphic string encoding function."""
        if isinstance(s, bytes):
            return s
        if not isinstance(s, str):
            return bytes(s)
        return bytes(s, encoding=master_encoding)

    def polyord(c):
        "Polymorphic ord() function"
        if isinstance(c, str):
            return ord(c)
        else:
            return c

    def polychr(c):
        "Polymorphic chr() function"
        if isinstance(c, int):
            return chr(c)
        else:
            return c

    def string_escape(s):
        """Polymorphic string_escape/unicode_escape."""
        # This hack is necessary because Unicode strings in Python 3 don't
        # have a decode method, so there's no simple way to ask it for the
        # equivalent of decode('string_escape') in Python 2. This function
        # assumes that it will be called with a Python 3 'str' instance
        return s.encode(master_encoding).decode('unicode_escape')

    def make_std_wrapper(stream):
        """Standard input/output wrapper factory function."""
        # This ensures that the encoding of standard output and standard
        # error on Python 3 matches the master encoding we use to turn
        # bytes to Unicode in polystr above
        # line_buffering=True ensures that interactive command sessions
        # work as expected
        return io.TextIOWrapper(stream.buffer, encoding="utf-8",
                                newline="\n", line_buffering=True)

    # This is the one situation where we *can* force unicode.
    if "UTF-8" != sys.stdout.encoding:
        forced_utf8 = True
        sys.stdout = make_std_wrapper(sys.stdout)
    if "UTF-8" != sys.stderr.encoding:
        forced_utf8 = True
        sys.stderr = make_std_wrapper(sys.stderr)
