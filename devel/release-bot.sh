#! /usr/bin/env sh

V=1.2.1+62
BLOGDIR=/home/jamesb/ntpsec-blog/_posts/
DEST=jamesb@www.jamesb.com:/var/www/html/ftp/
GPGKEY=69D1AAE20C7293813340FD812366A9A061CBFD54
GITLABKEY=id_gitlab
DESTKEY=id_jamesb192
GITLAB_TOKEN="20C7293813340FD812"

FLAG_CONFIG="-p"
FLAG_BUILD="-ppvj1"

TAG=$(echo $V|sed "s|[\.-\+]|_|g")
TAR="ntpsec-${V}"

set +e +x

/usr/bin/env sh ../buildprep --all

git clone https://gitlab.com/NTPsec/ntpsec 
cd ntpsec

sed -r "1s|[ \t]*([\d]+)[ \t]*| `date +s`|g" -i ../PIVOT.h
echo "${VERSION}" > ../VERSION
sed "0,/^== .*/s/^== .*/== $((date -uIday)): ${VERSION}  ==/" -i ../NEWS.adoc
git add ../NEWS.adoc ../PIVOT.h ../VERSION

./waf configure $FLAG_CONFIG
./waf build $FLAG_BUILD

/usr/bin/env sh make-tarball

BLURB=$(sed -n "0,/^==/d;0,/^==/{/^==/d;p}" < NEWS.adoc)
echo ${BLURB}
cat - >${BLOGDIR}/$(date -u "+%Y-%m-%d")-${TAR}.adoc <<EEND
---
layout: post
title:  "Version ${V}"
date:   $(date -uIsec)
---
== version $(V)

The NTPsec Project is pleased to announce the tagging of version 1.2.1

${BLURB}

For other changes since the previous release, please consult
the project NEWS.adoc file at 
https://gitlab.com/NTPsec/ntpsec/-/blob/master/NEWS.adoc

== Getting this release

You can clone the git repo from 
https://gitlab.com/NTPsec/ntpsec.git
and you can download the release tarballs with sums and signatures from 
ftp://ftp.ntpsec.org/pub/releases/

This release is signed with the GPG key id
${GPGKEY}
EEEND

kaychain -a gpg,ssh GPGKEY DESTKEY GITLABKEY

git commit -S${GPGKEY} -a -s -m 'version ${V}'
git tag -u ${GPGKEY} -m 'version ${V}' ${TAG}

sed '0,/^== .*/s/^== .*/== Repository Head ==\n\n&/' -i NEWS.adoc

git add NEWS.adoc
git commit -S${GPGKEY} -a -s -m 'start new NEWS.adoc section'
git push origin HEAD:master --tags

for format in tar.{gz,bz2,xz} zip;do
    gpg -u ${GPGKEY} -a --output ${TAR}.${format}.asc --detach-sign ${TAR}.${format};
    shasum -s 256 ${TAR}.${format} > ${TAR}.${format}.sum
    gpg -u ${GPGKEY} -a --output ${TAR}.${format}.sum.asc --detach-sign ${TAR}.${format}.sum;
done

scp ntpsec-${V}* ${DEST}

keyhain -k mine

git clean -dxf
git reset --hard

git checkout -B master

curl --header "PRIVATE-TOKEN: ${GITLAB_TOKEN}" --header 'Content-Type: application/json' --request PUT --data '{"released_at":"${gitlogtime}"}' "https://gitlab.com/api/v4/projects/553594/releases/${TAG}"
