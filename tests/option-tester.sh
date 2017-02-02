#!/usr/bin/env bash

# This is a hack to build with various configuration options.
# The intent is to check cases that normal testing doesn't use.

# Stuff goes into various test-* directories.
# Running again starts by deleting everything in the directory.


doit ()
{
  DIR=test-$1
  mkdir $DIR
  rm -rf $DIR/*
  ./waf configure --out=$DIR $2          2>&1 | tee    $DIR/test.log
  WAF1=${PIPESTATUS[0]}
  WAF2=0
  WAF3=0
  if [ "$WAF1" = 0 ]
  then
  echo                                   2>&1 | tee -a $DIR/test.log
  ./waf build                            2>&1 | tee -a $DIR/test.log
  WAF2=${PIPESTATUS[0]}
  if [ "$WAF2" = 0 ]
  then
  echo                                   2>&1 | tee -a $DIR/test.log
  ./waf check                            2>&1 | tee -a $DIR/test.log
  WAF3=${PIPESTATUS[0]}
  fi
  fi
  if [ "$WAF1" != 0 -o "$WAF2" != 0 -o "$WAF3" != 0 ] 
  then
    echo                                      | tee -a $DIR/test.log
    echo "Trouble with $DIR"                  | tee -a $DIR/test.log
  fi
  echo
  echo
}

doit minimal "--disable-debug --disable-droproot --disable-dns-lookup --disable-kernel-pll --disable-mdns-registration"
doit nodebug "--disable-debug --refclock=all"
doit all     "--enable-lockclock --enable-leap-smear --enable-mssntp --refclock=all"

if [ `uname -s` = Linux ]
then
doit linux   "--enable-classic-mode --enable-early-droproot --enable-seccomp"
fi

if [ `which asciidoc` != "" -a `which xsltproc` != "" ]
then
doit doc     "--enable-doc"
doit doconly "--enable-doc-only"
fi

echo
grep warning: test*/test.log
grep error:   test*/test.log
grep ^Trouble test*/test.log
echo
