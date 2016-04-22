#!/system/bin/sh

mount -o remount,rw /system

# Inject SuperSU
if [ ! -f /system/xbin/su ]; then
	# Make necessary folders
	mkdir /system/etc/init.d

	# Extract SU from ramdisk to correct locations
	rm -rf /system/bin/app_process
	rm -rf /system/bin/install-recovery.sh
	cp /sbin/su/supolicy /system/xbin/
	cp /sbin/su/su /system/xbin/
	cp /sbin/su/libsupol.so /system/lib64/
	cp /sbin/su/install-recovery.sh /system/etc/
	cp /sbin/su/99SuperSUDaemon /system/etc/init.d/

	# Begin SuperSU install process
	cp /system/xbin/su /system/xbin/daemonsu
	cp /system/xbin/su /system/xbin/sugote
	cp /system/bin/sh /system/xbin/sugote-mksh
	mkdir -p /system/bin/.ext
	cp /system/xbin/su /system/bin/.ext/.su

	cp /system/bin/app_process64 /system/bin/app_process_init
	mv /system/bin/app_process64 /system/bin/app_process64_original

	echo 1 > /system/etc/.installed_su_daemon

	chmod 755 /system/xbin/su
	chmod 755 /system/xbin/daemonsu
	chmod 755 /system/xbin/sugote
	chmod 755 /system/xbin/sugote-mksh
	chmod 755 /system/xbin/supolicy
	chmod 777 /system/bin/.ext
	chmod 755 /system/bin/.ext/.su
	chmod 755 /system/bin/app_process_init
	chmod 755 /system/bin/app_process64_original
	chmod 644 /system/lib64/libsupol.so
	chmod 755 /system/etc/install-recovery.sh
	chmod 644 /system/etc/.installed_su_daemon
	
	ln -s /system/etc/install-recovery.sh /system/bin/install-recovery.sh
	ln -s /system/xbin/daemonsu /system/bin/app_process
	ln -s /system/xbin/daemonsu /system/bin/app_process64

	/system/xbin/su --install
fi

# Inject Busybox if not present
#if [ ! -f /system/xbin/busybox ]; then
#	cp /system/xbin/
#	chmod 755 /system/xbin/busybox
#	/system/xbin/busybox --install -s /system/xbin
#fi
	
# Kill securitylogagent
if [ -d /system/app/SecurityLogAgent ]; then
	rm -fR /system/app/SecurityLogAgent
	rm -fR /system/priv-app/KLMSAgent
	rm -fR /system/app/*Knox*
	rm -fR /system/app/*KNOX*
	rm -fR /system/container
	rm -fR /system/app/Bridge
	rm -fR /system/app/BBCAgent
	rm -fR /system/priv-app/FotaClient
	rm -fR /system/priv-app/SyncmIDM
	rm -fR /system/priv-app/*FWUpdate*
fi

# Private Mode fix
if [ ! -f /system/priv-app/PersonalPageService/PersonalPageService_Fix.apk ]; then
	rm -f /system/priv-app/PersonalPageService/*
	rm -fR /system/priv-app/PersonalPageService/*
	cp -f /sbin/su/PersonalPageService_Fix.apk /system/priv-app/PersonalPageService/
	chmod 0644 /system/priv-app/PersonalPageService/PersonalPageService_Fix.apk
fi

# DRM Video fix
if [ ! -f /system/lib/liboemcrypto.so.bak ]; then
	mv /system/lib/liboemcrypto.so /system/lib/liboemcrypto.so.bak
fi
	
# Enforce init.d script perms on any post-root added files
chmod 755 /system/etc/init.d
chmod 755 /system/etc/init.d/*

# Run init.d scripts
mount -t rootfs -o remount,rw rootfs
run-parts /system/etc/init.d

#  Wait for 5 seconds from boot before starting the SuperSU daemon
sleep 5
/system/xbin/daemonsu --auto-daemon &

sync