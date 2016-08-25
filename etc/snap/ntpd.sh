#!/bin/sh
if [ ! -e "$SNAP_DATA/etc/ntpd.conf" ]; then
    echo "make packaged default config user accessible in $SNAP_DATA"
    mkdir -p "$SNAP_DATA/etc/"
    cp -v $SNAP/etc/* "$SNAP_DATA/etc/"

    # set default driftfile path to a snap writable dir
    echo "driftfile $SNAP_DATA/ntp.drift" > "$SNAP_DATA/etc/use-minimal-logging"
fi

# avoid failing to include config files
# (https://gitlab.com/NTPsec/ntpsec/issues/98)
cd "$SNAP_DATA/etc/"

exec "$SNAP/usr/local/sbin/ntpd" "$@"
