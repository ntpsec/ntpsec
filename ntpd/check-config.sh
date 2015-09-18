#!/bin/sh
# check parsing of config file
# output is in ntpd's order so input much match that.

# assumes we are cd-ed to build directory

# This only works on code built with --enable-saveconfig

RAW=../ntpd/complete.conf.in
IN=ntpd/complete.conf
OUT=ntpd/compsave.conf            # from xxx

# may have to prune unsupported features
#  rlimit requires MEMLOCK
cp -p $RAW $IN

./ntpd/ntpd --configfile $IN --saveconfigquit $OUT
if test "$?" -ge "0"
then
  exit $?
fi

ls -l $IN $OUT
diff $IN $OUT
RESULT=$?
echo Result: $RESULT

exit $RESULT


