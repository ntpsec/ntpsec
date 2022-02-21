#! /usr/bin/env sh

# set up a virtual machine and shove this all off there...

../buildprep --all

git clone https://gitlab.com/NTPsec/ntpsec 
cd ntpsec

sed -r "1s|[ \t]*([\d]+)[ \t]*| `date +s`|g" -i ../PIVOT.h
echo "${VERSION}" > ../VERSION
sed "0,/^== .*/s/^== .*/== $((date -uIday)): ${VERSION}  ==/" -i ../NEWS.adoc
git add ../NEWS.adoc ../PIVOT.h ../VERSION

./waf configure $FLAG_CONFIG
./waf build $FLAG_BUILD

for I in `seq 5`;do
    echo -rn "\x07"
    sleep 1
done
