#! /bin/sh

# Copyright the NTPsec project contributors
#
# SPDX-License-Identifier: BSD-2-Clause

# keep this POSIX sh compatible. No bash-isms!
# replace |& with 2>&1 |

# This is a hack to build with various configuration options.
# The intent is to check building combinations that normal testing doesn't use.

# Stuff goes into various test-* directories.
# Running again starts by deleting everything in the directory.

# set pipefail to catch pipeline failures
# Unfortunately, it doesn't work on some older sh-es
if (set -o pipefail) 2>/dev/null
then
  set -o pipefail
fi

PURGE=""
if command -v pkg-config
then
  SECCOMP="$(pkg-config libseccomp --variable=includedir)"
  SECCOMPH="${SECCOMP}/seccomp.h"
else
  SECCOMP=""
  SECCOMPH=""
fi
LINUX=""
if [ "Linux" = "$(uname -s)" ] && [ -n "${SECCOMP}" ] && [ -f "${SECCOMPH}" ]
then
  # Not supported on CentOS 6
  LINUX="--enable-seccomp"
fi

if [ -z "${PYTHON}" ]
then
    PYTHON="python"
fi

DISABLE_NTS=""
if pkg-config --version 2>/dev/null 1>/dev/null
then
  if ! pkg-config openssl --atleast-version=1.1.1
  then
    DISABLE_NTS="--disable-nts"
  fi
else
  if ! "$(${PYTHON} ./wafhelpers/tlscheck.py)"
  then
    DISABLE_NTS="--disable-nts"
  fi
fi

fail ()
{
  echo                                          2>&1   | tee -a "${DIR:?}/test.log"
  echo "Trouble with ${PYTHON}-${DIR}-${1}"     2>&1   | tee -a "${DIR:?}/test.log"
  PURGE="${PURGE} ${PYTHON}-${DIR}-${1}"
}

doit ()
{
  DIR="test-${1}"
  [ ! -d "${DIR:?}" ] && mkdir "${DIR:?}"
  rm -rf "${DIR:?}"/*
  "${PYTHON}" ./waf configure ${DISABLE_NTS} --out="${DIR}" ${2} 2>&1 | tee    "${DIR:?}/test.log"
  if [ "$?" != 0 ]
  then
    fail "configure"
    return
  fi

  for STEP in build check bin_test install uninstall
  do
    echo                                        2>&1    | tee -a "${DIR}"/test.log
    DESTDIR=scratch "${PYTHON}" ./waf "${STEP}" 2>&1    | tee -a "${DIR}"/test.log
    if [ 0 != "$?" ]
    then
      fail "${STEP}"
      return
    fi
  done

  echo
  echo
}

# no --disable-manpage on default and all
doit default "--disable-debug-gdb"
doit minimal "--disable-droproot --disable-mdns-registration --disable-doc --disable-manpage --disable-debug-gdb --enable-pylib=none"

# This also tests refclocks without DEBUG
doit classic "--enable-classic-mode --refclock=all --disable-doc --disable-manpage --enable-pylib=ffi --disable-debug-gdb"

doit all     "--enable-Werror --enable-warnings --enable-attic --enable-debug --enable-debug-timing --refclock=all --enable-leap-smear --enable-mssntp --enable-early-droproot $LINUX --disable-doc --disable-manpage --enable-pylib=ext"

DOC=0
if [ "$(command -v asciidoctor)" ]
then
  DOC=1
else
  if [ "$(command -v xsltproc)" ]
  then
    if [ "$(command -v asciidoc)" ] || \
      [ "$(command -v asciidoc3)" ]
    then
      DOC=1
    fi
  fi
fi
if [ 0 != "${DOC}" ]
then
doit doc     ""
fi

# should try cross compile
RET=0
test -z "${PURGE}" || RET=1

echo

grep warning:                    test*/test.log
grep error:                      test*/test.log
grep "The configuration failed"  test*/test.log
grep ^Trouble                    test*/test.log
echo

echo "## $(${PYTHON} --version)"
if test -n "${PYTHONPATH}"
then
  echo "## PYTHONPATH is \"${PYTHONPATH}\""
fi

if ! (set -o pipefail) 2>/dev/null
then
  echo "### Old sh - no pipefail"
  echo "### We can't test for errors during build"
  echo "### You will have to scan the log files."
  PURGE="${PURGE} pipefail"
fi

if [ "Linux" = "$(uname -s)" ] && [ -z "$SECCOMP" ]
then
    echo
    echo "### Warning: Missing seccomp.h (on a Linux system)"
    echo
    PURGE="${PURGE} seccomp"
fi

if [ -n "$PURGE" ]
then
    echo "## errors encountered during execution:${PURGE}"
    exit 1
fi

exit $RET
