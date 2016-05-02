#!/system/bin/sh

mount -o remount,rw /system

if [ ! -f /system/xbin/su ]; then
	#make necessary folders
	mkdir /system/etc/init.d

	#extract SU from ramdisk to correct locations
	rm -rf /system/bin/app_process
	rm -rf /system/bin/install-recovery.sh
	cp /sbin/su/supolicy /system/xbin/
	cp /sbin/su/su /system/xbin/
	cp /sbin/su/libsupol.so /system/lib/
	cp /sbin/su/install-recovery.sh /system/etc/
	cp /sbin/su/init.sec.boot.sh /system/etc/
	cp /sbin/su/99SuperSUDaemon /system/etc/init.d/

	#begin supersu install process
	cp /system/xbin/su /system/xbin/daemonsu
	cp /system/xbin/su /system/xbin/sugote
	cp /system/bin/sh /system/xbin/sugote-mksh
	mkdir -p /system/bin/.ext
	cp /system/xbin/su /system/bin/.ext/.su

	cp /system/bin/app_process32 /system/bin/app_process_init
	mv /system/bin/app_process32 /system/bin/app_process32_original

	echo 1 > /system/etc/.installed_su_daemon

	chown 0:0 /system/xbin/su
	chmod 755 /system/xbin/su
	chcon u:object_r:system_file:s0 /system/xbin/su

	chown 0:0 /system/xbin/daemonsu
	chmod 755 /system/xbin/daemonsu
	chcon u:object_r:system_file:s0 /system/xbin/daemonsu

	chown 0:0 /system/xbin/sugote
	chmod 755 /system/xbin/sugote
	chcon u:object_r:zygote_exec:s0 /system/xbin/sugote

	chown 0:0 /system/xbin/sugote-mksh
	chmod 755 /system/xbin/sugote-mksh
	chcon u:object_r:system_file:s0 /system/xbin/sugote-mksh

	chown 0:0 /system/xbin/supolicy
	chmod 755 /system/xbin/supolicy
	chcon u:object_r:system_file:s0 /system/xbin/supolicy

	chown 0:0 /system/bin/.ext
	chmod 777 /system/bin/.ext
	chcon u:object_r:system_file:s0 /system/bin/.ext

	chown 0:0 /system/bin/.ext/.su
	chmod 755 /system/bin/.ext/.su
	chcon u:object_r:system_file:s0 /system/bin/.ext/.su

	chown 0:2000 /system/bin/app_process_init
	chmod 755 /system/bin/app_process_init
	chcon u:object_r:system_file:s0 /system/bin/app_process_init

	chown 0:2000 /system/bin/app_process32_original
	chmod 755 /system/bin/app_process32_original
	chcon u:object_r:zygote_exec:s0 /system/bin/app_process32_original

	chown 0:0 /system/lib/libsupol.so
	chmod 644 /system/lib/libsupol.so
	chcon u:object_r:system_file:s0 /system/lib/libsupol.so

	chown 0:0 /system/etc/install-recovery.sh
	chmod 755 /system/etc/install-recovery.sh
	chcon u:object_r:system_file:s0 /system/etc/install-recovery.sh

	chown 0:0 /system/etc/.installed_su_daemon
	chmod 644 /system/etc/.installed_su_daemon
	chcon u:object_r:system_file:s0 /system/etc/.installed_su_daemon

	ln -s /system/etc/install-recovery.sh /system/bin/install-recovery.sh
	ln -s /system/xbin/daemonsu /system/bin/app_process
	ln -s /system/xbin/daemonsu /system/bin/app_process32

	/system/xbin/su --install
fi

#enforce init.d script perms on any post-root added files
chmod 755 /system/etc/init.d
chmod 755 /system/etc/init.d/*

#inject busybox if not present
if [ ! -f /system/xbin/busybox ]; then
	cp /sbin/busybox /system/xbin/
	chmod 755 /system/xbin/busybox
	/system/xbin/busybox --install -s /system/xbin
fi

#kill securitylogagent
rm -rf /system/app/SecurityLogAgent

# fix gapps wakelock
sleep 40
su -c "pm enable com.google.android.gms/.update.SystemUpdateActivity"
su -c "pm enable com.google.android.gms/.update.SystemUpdateService"
su -c "pm enable com.google.android.gms/.update.SystemUpdateService$ActiveReceiver"
su -c "pm enable com.google.android.gms/.update.SystemUpdateService$Receiver"
su -c "pm enable com.google.android.gms/.update.SystemUpdateService$SecretCodeReceiver"
su -c "pm enable com.google.android.gsf/.update.SystemUpdateActivity"
su -c "pm enable com.google.android.gsf/.update.SystemUpdatePanoActivity"
su -c "pm enable com.google.android.gsf/.update.SystemUpdateService"
su -c "pm enable com.google.android.gsf/.update.SystemUpdateService$Receiver"
su -c "pm enable com.google.android.gsf/.update.SystemUpdateService$SecretCodeReceiver

sync
