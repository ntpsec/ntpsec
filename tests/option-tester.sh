#! /bin/sh
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
SECCOMP="$(pkg-config libseccomp --variable=includedir)"
SECCOMP="$SECCOMP/seccomp.h"
LINUX=""
if [ `uname -s` = "Linux" -a -n "$SECCOMP" -a -f "$SECCOMPH" ]
then
  # Not supported on CentOS 6
  LINUX="--enable-seccomp"
fi

if [ -z ${PYTHON} ]
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
  if ! $PYTHON ../wafhelpers/tlscheck.py
  then
    DISABLE_NTS="--disable-nts"
  fi
fi

doit ()
{
  DIR=test-$1
  [ ! -d $DIR ] && mkdir $DIR
  rm -rf $DIR/*
  $PYTHON ./waf configure $DISABLE_NTS --out=$DIR $2 2>&1 | tee    $DIR/test.log
  WAF1=$?
  WAF2=0
  WAF3=0
  if [ "$WAF1" = 0 ]
  then
  echo                                 2>&1    | tee -a $DIR/test.log
  $PYTHON ./waf build                   2>&1    | tee -a $DIR/test.log
  WAF2=$?
  if [ "$WAF2" = 0 ]
  then
  echo                                 2>&1    | tee -a $DIR/test.log
  $PYTHON ./waf check                   2>&1    | tee -a $DIR/test.log
  WAF3=$?
  else
    PURGE="${PURGE} ${PYTHON}-${DIR}-build"
  fi
  else
    PURGE="${PURGE} ${PYTHON}-${DIR}-config"
  fi
  if [ "$WAF1" != 0 -o "$WAF2" != 0 -o "$WAF3" != 0 ]
  then
    echo                               2>&1   | tee -a $DIR/test.log
    echo "Trouble with $DIR"           2>&1   | tee -a $DIR/test.log
  fi
  if [ "$WAF3" != 0 ]
  then
    PURGE="${PURGE} ${PYTHON-}${DIR}3-check"
  fi
  echo
  echo
}

# no --disable-manpage on default and all
doit default ""
doit minimal "--disable-droproot --disable-mdns-registration --disable-doc --disable-manpage"

# This also tests refclocks without DEBUG
doit classic "--enable-classic-mode --refclock=all --disable-doc --disable-manpage"

doit all     "--enable-warnings --enable-debug --enable-debug-gdb --enable-debug-timing --refclock=all --enable-leap-smear --enable-mssntp --enable-early-droproot --disable-fuzz $LINUX --disable-doc --disable-manpage"

if [ "`which asciidoc 2>/dev/null`" != "" -a \
     "`which xsltproc 2>/dev/null`" != "" ]
then
doit doc     ""
fi

# should try cross compile

echo

grep warning:                    test*/test.log
grep error:                      test*/test.log
grep "The configuration failed"  test*/test.log
grep ^Trouble                    test*/test.log
echo

echo -n "## ";  $PYTHON --version
if test -n "$PYTHONPATH"
then
  echo "## PYTHONPATH is" \"$PYTHONPATH\"
fi

if ! (set -o pipefail) 2>/dev/null
then
  echo "### Old sh - no pipefail"
  echo "### We can't test for errors during build"
  echo "### You will have to scan the log files."
  PURGE="${PURGE} pipefail"
fi

if [ `uname -s` = "Linux" -a -z "$SECCOMP" ]
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
