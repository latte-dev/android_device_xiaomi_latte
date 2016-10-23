#!/system/bin/sh

# Rfkill enables by default all radios it is responsible for at start.
# It is up to each radio user space SW stack to switch rfkill off at
# boot time but this is not always the case.
# This script forces radios disabling in case their SW stack is not handling it.
# It should be called from android init.rc at boot time

wifi="false"
bt="false"
gps="false"
fm="false"
nfc="false"

# Parameters parsing
while [[ $# > 0 ]]
do
param="$1"
case $param in
    wifi)
    wifi="true"
    ;;
    bluetooth)
    bt="true"
    ;;
    gps)
    gps="true"
    ;;
    fm)
    fm="true"
    ;;
    nfc)
    nfc="true"
    ;;
    *)
    # unknown option
    ;;
esac
shift
done

# Force radios disabling through /dev/rfkill
if [ $wifi == "true" ]; then
    echo "\x00\x00\x00\x00\x01\x03\x01" > /dev/rfkill
fi
if [ $bt == "true" ]; then
    echo "\x00\x00\x00\x00\x02\x03\x01" > /dev/rfkill
fi
if [ $gps == "true" ]; then
    echo "\x00\x00\x00\x00\x06\x03\x01" > /dev/rfkill
fi
if [ $fm == "true" ]; then
    echo "\x00\x00\x00\x00\x07\x03\x01" > /dev/rfkill
fi
if [ $nfc == "true" ]; then
    echo "\x00\x00\x00\x00\x08\x03\x01" > /dev/rfkill
fi

exit 0
