#!/bin/sh
# check parsing of config file
# output is in ntpd's order so input much match that.

# assumes we are cd-ed to build directory

# This only works on code built with --enable-saveconfig

RAW=../ntpd/complete.conf
IN=main/ntpd/complete.conf
OUT=main/ntpd/compsave.conf

# may have to prune unsupported features
#  rlimit requires MEMLOCK
cp -p $RAW $IN

./main/ntpd/ntpd -n --configfile $IN --saveconfigquit $OUT
if test "$?" -ge "0"
then
  exit $?
fi

ls -l $IN $OUT
diff $IN $OUT
RESULT=$?
echo Result: $RESULT

exit $RESULT


