for rfkill in /sys/class/rfkill/rfkill[0-9]
do
	type=`cat $rfkill/type`
	if [ $type == "bluetooth" ]; then
		chown bluetooth:net_bt_stack $rfkill/type
		chmod 0660 $rfkill/state
		chown bluetooth:net_bt_stack $rfkill/state
	fi
done

exit 0
