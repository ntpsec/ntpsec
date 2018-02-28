#!/usr/bin/env bash
# sh on NetBSD and FreeBSD says:
#   sh: ${PIPESTATUS[...}: Bad substitution

# This is a hack to build with various configuration options.
# The intent is to check building combinations that normal testing doesn't use.

# Stuff goes into various test-* directories.
# Running again starts by deleting everything in the directory.

LINUX=""
if [ `uname -s` = "Linux" -a -f /usr/include/seccomp.h ]
then
  # Not supported on CentOS 6
  LINUX="--enable-seccomp"
fi

doit ()
{
  DIR=test-$1
  [ ! -d $DIR ] && mkdir $DIR
  rm -rf $DIR/*
  ./waf configure --out=$DIR $2          |& tee    $DIR/test.log
  WAF1=${PIPESTATUS[0]}
  WAF2=0
  WAF3=0
  if [ "$WAF1" = 0 ]
  then
  echo                                   |& tee -a $DIR/test.log
  ./waf build                            |& tee -a $DIR/test.log
  WAF2=${PIPESTATUS[0]}
  if [ "$WAF2" = 0 ]
  then
  echo                                   |& tee -a $DIR/test.log
  ./waf check                            |& tee -a $DIR/test.log
  WAF3=${PIPESTATUS[0]}
  fi
  fi
  if [ "$WAF1" != 0 -o "$WAF2" != 0 -o "$WAF3" != 0 ] 
  then
    echo                                         |& tee -a $DIR/test.log
    echo "Trouble with $DIR"                     |& tee -a $DIR/test.log
  else
    echo -n "VERSION: "                          |& tee -a $DIR/test.log
    ./$DIR/main/ntpd/ntpd --version              |& tee -a $DIR/test.log
    echo -n "VERSION: "                          |& tee -a $DIR/test.log
    ./$DIR/main/ntpclients/ntpq --version        |& tee -a $DIR/test.log
    echo -n "VERSION: "                          |& tee -a $DIR/test.log
    ./$DIR/main/ntpclients/ntpdig --version      |& tee -a $DIR/test.log
    echo -n "VERSION: "                          |& tee -a $DIR/test.log
    ./$DIR/main/ntpclients/ntpmon --version      |& tee -a $DIR/test.log
if [ "`which gpsmon 2>/dev/null`" != "" ]
then
    # needs GPSD library
    echo -n "VERSION: "                          |& tee -a $DIR/test.log
    ./$DIR/main/ntpclients/ntploggps --version   |& tee -a $DIR/test.log
fi
    echo -n "VERSION: "                          |& tee -a $DIR/test.log
    ./$DIR/main/ntpclients/ntplogtemp --version  |& tee -a $DIR/test.log
  fi
  echo
  echo
}

# no --disable-manpage on default and all
doit default ""
doit minimal "--disable-droproot --disable-dns-lookup --disable-mdns-registration --disable-manpage"

# This also tests refclocks without DEBUG
doit classic "--enable-classic-mode --refclock=all --disable-manpage"

doit all     "--enable-debug --enable-debug-gdb --enable-debug-timing --refclock=all --enable-lockclock --enable-leap-smear --enable-mssntp --enable-early-droproot $LINUX"

if [ "`which asciidoc 2>/dev/null`" != "" -a \
     "`which xsltproc 2>/dev/null`" != "" ]
then
doit doc     "--enable-doc --disable-manpage"
fi

# should try cross compile

echo
if [ `uname -s` = "Linux" -a ! -f /usr/include/seccomp.h ]
then
    echo
    echo "### Warning: Missing seccomp.h (on a Linux system)"
    echo
fi
echo "PYTHONPATH is" \"$PYTHONPATH\"
grep VERSION: test*/test.log
echo
grep warning:                    test*/test.log
grep error:                      test*/test.log
grep "The configuration failed"  test*/test.log
grep ^Trouble                    test*/test.log
echo
