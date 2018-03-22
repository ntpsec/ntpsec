#!/usr/bin/env bash
# sh on NetBSD and FreeBSD says:
#   sh: ${PIPESTATUS[...}: Bad substitution

# This is a clone of option-tester.sh
# to build with python3 and do minimal (version) testing.

if [ "`which python3 2>/dev/null`" = "" ]
then
  echo "# Error: No python3 on this system."
  exit 1
fi

doit ()
{
  DIR=test-$1
  [ ! -d $DIR ] && mkdir $DIR
  rm -rf $DIR/*
  python3 ./waf configure --out=$DIR $2  |& tee    $DIR/test.log
  WAF1=${PIPESTATUS[0]}
  WAF2=0
  WAF3=0
  if [ "$WAF1" = 0 ]
  then
  echo                                   |& tee -a $DIR/test.log
  python3 ./waf build                    |& tee -a $DIR/test.log
  WAF2=${PIPESTATUS[0]}
  if [ "$WAF2" = 0 ]
  then
  echo                                   |& tee -a $DIR/test.log
  python3 ./waf check                    |& tee -a $DIR/test.log
  WAF3=${PIPESTATUS[0]}
  fi
  fi
  if [ "$WAF1" != 0 -o "$WAF2" != 0 -o "$WAF3" != 0 ] 
  then
    echo                                 |& tee -a $DIR/test.log
    echo "Trouble with $DIR"             |& tee -a $DIR/test.log
  else
    echo -n "VERSION: "                                  |& tee -a $DIR/test.log
    ./$DIR/main/ntpd/ntpd --version                      |& tee -a $DIR/test.log
    echo -n "VERSION: "                                  |& tee -a $DIR/test.log
    python3 ./$DIR/main/ntpclients/ntpq --version        |& tee -a $DIR/test.log
    echo -n "VERSION: "                                  |& tee -a $DIR/test.log
    python3 ./$DIR/main/ntpclients/ntpdig --version      |& tee -a $DIR/test.log
    if [ `uname -s` != "NetBSD" ]
    then
      # no Python/curses on NetBSD
      echo -n "VERSION: "                                |& tee -a $DIR/test.log
      python3 ./$DIR/main/ntpclients/ntpmon --version    |& tee -a $DIR/test.log
    fi
    # if [ "`which gpsmon 2>/dev/null`" != "" ]
    # then
    # needs GPSD library - don't know how to test for python3 version
    # echo -n "VERSION: "                                |& tee -a $DIR/test.log
    # python3 ./$DIR/main/ntpclients/ntploggps --version |& tee -a $DIR/test.log
    # fi
    echo -n "VERSION: "                                  |& tee -a $DIR/test.log
    python3 ./$DIR/main/ntpclients/ntplogtemp --version  |& tee -a $DIR/test.log
  fi
  echo
  echo
}

# Build with only one set of options.  The options don't change
# the Python side of things.

doit python3 "--disable-droproot --disable-dns-lookup --disable-mdns-registration --disable-manpage"

echo "PYTHONPATH is" \"$PYTHONPATH\"
grep VERSION: test*/test.log
echo
grep warning:                    test*/test.log
grep error:                      test*/test.log
grep "The configuration failed"  test*/test.log
grep ^Trouble                    test*/test.log
echo
