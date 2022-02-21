#! /usr/bin/env sh

keychain --agents pgp,ssh id_gitlab id_service ${KEY} 
### BEGIN -- stuff swiped from ~/devel/release
git commit -Ssecurity@ntpsec.org -a -s -m 'version ${V}'
git tag -u ${KEY} -m 'version ${V}' NTPsec_${UV}
sed '0,/^== .*/s/^== .*/== Repository Head ==\n\n&/' -i NEWS.adoc
git add NEWS.adoc
git commit -Ssecurity@ntpsec.org -a -s -m 'start new NEWS.adoc section'
git push
git push --tags

gpg -u ${KEY} -a --output ${TAR}.asc --detach-sign ${TAR}
shasum -a 256 ${TAR} >${TAR}.sum
gpg -u ${KEY} -a --output ${TAR}.sum.asc --clearsign ${TAR}.sum
scp ntpsec-${V}.tar* ${DST}
### END -- stuff swiped from ~/devel/release
keychain -k mine
curl --header "PRIVATE-TOKEN: ${cookie}" --header 'Content-Type: application/json' --request PUT --data '{"released_at":"${gitlogtime}"}' "https://gitlab.com/api/v4/projects/553594/releases/${tag}"
git clean -dxf
git reset --hard
