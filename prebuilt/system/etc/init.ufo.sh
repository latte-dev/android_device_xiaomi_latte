#!/system/bin/sh

# This script is started from /init.rc.

if test ! -f /data/ufo.prop; then
    ln -s /system/vendor/etc/ufo.prop /data/ufo.prop
    chmod 644 /data/ufo.prop
fi

exit 0
