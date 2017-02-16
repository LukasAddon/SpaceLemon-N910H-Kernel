#!/sbin/sh
# Script by corola
# Wipe /data partition excluding private contents

rm -fR /system/app/*synapse*
rm -fR /data/app/*synapse*
rm -fR /data/data/com.af.synapse
rm -fR /data/media/0/Synapse

sync
