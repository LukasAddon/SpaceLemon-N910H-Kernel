#!/sbin/busybox sh

BB=/sbin/busybox;

mount -o remount,rw /
mount -o remount,rw /system /system

#
# Stop Google Service and restart it on boot (dorimanx)
# This removes high CPU load and ram leak!
#
if [ "$($BB pidof com.google.android.gms | wc -l)" -eq "1" ]; then
	$BB kill $($BB pidof com.google.android.gms);
fi;
if [ "$($BB pidof com.google.android.gms.unstable | wc -l)" -eq "1" ]; then
	$BB kill $($BB pidof com.google.android.gms.unstable);
fi;
if [ "$($BB pidof com.google.android.gms.persistent | wc -l)" -eq "1" ]; then
	$BB kill $($BB pidof com.google.android.gms.persistent);
fi;
if [ "$($BB pidof com.google.android.gms.wearable | wc -l)" -eq "1" ]; then
	$BB kill $($BB pidof com.google.android.gms.wearable);
fi;

#
# Fast Random Generator (frandom) support on boot
#
chmod 444 /dev/erandom
chmod 444 /dev/frandom

#
# We need faster I/O so do not try to force moving to other CPU cores (dorimanx)
#
for i in /sys/block/*/queue; do
	echo "2" > $i/rq_affinity
done

# Tweak interextrem
# A53 Cluster
#echo "19000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/above_hispeed_delay
#echo "800" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/multi_enter_load
#echo "80000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/multi_enter_time
#echo "0" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/boost
#echo "" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/boostpulse
#echo "40000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/boostpulse_duration
#echo "85" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/go_hispeed_load
#echo "900000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/hispeed_freq
#echo "1" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/io_is_busy
#echo "360" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/multi_exit_load
#echo "320000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/multi_exit_time
#echo "400000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/multi_kfc_min_freq
#echo "200" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/single_enter_load
#echo "160000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/single_enter_time
#echo "30000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/min_sample_time
#echo "80 1100000:90 1300000:95" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/target_loads
#echo "7000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/timer_rate
#echo "80000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/timer_slack
#echo "90" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/single_exit_load
#echo "80000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/single_exit_time
#echo "400000" > /sys/devices/system/cpu/cpu0/cpufreq/interextrem/single_kfc_min_freq

# A57 Cluster
#echo "20000 1200000:39000 1500000:29000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/above_hispeed_delay
#echo "360" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/multi_enter_load
#echo "99000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/multi_enter_time
#echo "0" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/boost
#echo "" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/boostpulse
#echo "40000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/boostpulse_duration
#echo "90" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/go_hispeed_load
#echo "1000000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/hispeed_freq
#echo "1" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/io_is_busy
#echo "240" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/multi_exit_load
#echo "299000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/multi_exit_time
#echo "400000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/multi_kfc_min_freq
#echo "95" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/single_enter_load
#echo "199000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/single_enter_time
#echo "30000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/min_sample_time
#echo "80 1200000:90 1500000:95" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/target_loads
#echo "7000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/timer_rate
#echo "80000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/timer_slack
#echo "60" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/single_exit_load
#echo "99000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/single_exit_time
#echo "400000" > /sys/devices/system/cpu/cpu4/cpufreq/interextrem/single_kfc_min_freq

#
# Allow untrusted apps to read from debugfs (mitigate SELinux denials)
#
/system/xbin/supolicy --live \
	"allow untrusted_app debugfs file { open read getattr }" \
	"allow untrusted_app sysfs_lowmemorykiller file { open read getattr }" \
	"allow untrusted_app sysfs_devices_system_iosched file { open read getattr }" \
	"allow untrusted_app persist_file dir { open read getattr }" \
	"allow debuggerd gpu_device chr_file { open read getattr }" \
	"allow netd netd capability fsetid" \
	"allow netd { hostapd dnsmasq } process fork" \
	"allow { system_app shell } dalvikcache_data_file file write" \
	"allow { zygote mediaserver bootanim appdomain }  theme_data_file dir { search r_file_perms r_dir_perms }" \
	"allow { zygote mediaserver bootanim appdomain }  theme_data_file file { r_file_perms r_dir_perms }" \
	"allow system_server { rootfs resourcecache_data_file } dir { open read write getattr add_name setattr create remove_name rmdir unlink link }" \
	"allow system_server resourcecache_data_file file { open read write getattr add_name setattr create remove_name unlink link }" \
	"allow system_server dex2oat_exec file rx_file_perms" \
	"allow mediaserver mediaserver_tmpfs file execute" \
	"allow drmserver theme_data_file file r_file_perms" \
	"allow zygote system_file file write" \
	"allow atfwd property_socket sock_file write" \
	"allow untrusted_app sysfs_display file { open read write getattr add_name setattr remove_name }" \
	"allow debuggerd app_data_file dir search" \
	"allow sensors diag_device chr_file { read write open ioctl }" \
	"allow sensors sensors capability net_raw" \
	"allow init kernel security setenforce" \
	"allow netmgrd netmgrd netlink_xfrm_socket nlmsg_write" \
	"allow netmgrd netmgrd socket { read write open ioctl }"


mount -o remount,rw /
mount -o remount,rw /system /system

#
# Synapse start
#
$BB mount -t rootfs -o remount,rw rootfs
$BB chmod -R 755 /res/synapse
$BB chmod -R 755 /res/synapse/SkyHigh/*
#busybox ln -fs /res/synapse/uci /sbin/uci
/sbin/uci
# Synapse end
#

# $BB mkdir -p /mnt/ntfs
# $BB chmod 777 /mnt/ntfs
# $BB mount -o mode=0777,gid=1000 -t tmpfs tmpfs /mnt/ntfs

#
# Init.d
#
if [ ! -d /system/etc/init.d ]; then
	mkdir -p /system/etc/init.d/;
	chown -R root.root /system/etc/init.d;
	chmod 777 /system/etc/init.d/;
fi;

$BB run-parts /system/etc/init.d

#kill securitylogagent
rm -rf /system/app/SecurityLogAgent

iptables -t mangle -A POSTROUTING -o rmnet+ -j TTL --ttl-set 64

$BB mount -t rootfs -o remount,ro rootfs
$BB mount -o remount,ro /system /system
$BB mount -o remount,rw /data
