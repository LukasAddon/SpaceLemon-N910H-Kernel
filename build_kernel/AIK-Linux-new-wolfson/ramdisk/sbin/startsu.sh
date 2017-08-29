#!/system/bin/sh

#  Start SuperSU daemon
#  Wait for 5 seconds from boot before starting the SuperSU daemon
sleep 5
/system/xbin/daemonsu --auto-daemon &

