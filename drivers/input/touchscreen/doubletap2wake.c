/*
 * drivers/input/touchscreen/doubletap2wake.c
 *
 *
 * Copyright (c) 2013, Dennis Rassmann <showp1984@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/input/doubletap2wake.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/input.h>
#ifndef CONFIG_POWERSUSPEND
#include <linux/lcd_notify.h>
#else
#include <linux/powersuspend.h>
#endif
#include <linux/hrtimer.h>
#include <asm-generic/cputime.h>
#include <linux/wakelock.h>

/* uncomment since no touchscreen defines android touch, do that here */
//#define ANDROID_TOUCH_DECLARED

/* if Sweep2Wake is compiled it will already have taken care of this */
//#ifdef CONFIG_TOUCHSCREEN_SWEEP2WAKE
//#define ANDROID_TOUCH_DECLARED
//#endif

/* Version, author, desc, etc */
#define DRIVER_AUTHOR "Dennis Rassmann <showp1984@gmail.com>"
#define DRIVER_DESCRIPTION "Doubletap2wake for almost any device"
#define DRIVER_VERSION "1.0"
#define LOGTAG "[doubletap2wake]: "

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPLv2");

/* Tuneables */
#define DT2W_DEBUG		0
#define DT2W_DEFAULT		0

#define DT2W_PWRKEY_DUR		60
#define DT2W_FEATHER		150
#define DT2W_TIME		50

/* Sensors */
static bool flg_sensor_prox_detecting = false;

/* Resources */
static struct wake_lock dt2w_wakelock;
int dt2w_switch = DT2W_DEFAULT;
bool dt2w_toggled = false;
static unsigned long long tap_time_pre = 0;
static int touch_x = 0, touch_y = 0, touch_nr = 0, x_pre = 0, y_pre = 0;
static bool touch_x_called = false, touch_y_called = false, touch_cnt = true;
static bool scr_suspended = false, exec_count = true;
#ifndef CONFIG_POWERSUSPEND
static struct notifier_block dt2w_lcd_notif;
#endif
static struct input_dev * doubletap2wake_pwrdev;
static DEFINE_MUTEX(pwrkeyworklock);
static struct work_struct dt2w_input_work;

/* Read cmdline for dt2w */
static int __init read_dt2w_cmdline(char *dt2w)
{
	if (strcmp(dt2w, "1") == 0) {
		pr_info("[cmdline_dt2w]: DoubleTap2Wake enabled. | dt2w='%s'\n", dt2w);
		dt2w_switch = 1;
	/*} else if (strcmp(dt2w, "2") == 0) {
		pr_info("[cmdline_dt2w]: DoubleTap2Wake fullscreen enabled. | dt2w='%s'\n", dt2w);
		dt2w_switch = 2;*/
	} else if (strcmp(dt2w, "0") == 0) {
		pr_info("[cmdline_dt2w]: DoubleTap2Wake disabled. | dt2w='%s'\n", dt2w);
		dt2w_switch = 0;
	} else {
		pr_info("[cmdline_dt2w]: No valid input found. Going with default: | dt2w='%u'\n", dt2w_switch);
	}
	return 1;
}
__setup("dt2w=", read_dt2w_cmdline);

/* reset on finger release */
static void doubletap2wake_reset(void) {
	if (wake_lock_active(&dt2w_wakelock))
		wake_unlock(&dt2w_wakelock);
	exec_count = true;
	touch_nr = 0;
	tap_time_pre = 0;
	x_pre = 0;
	y_pre = 0;
}

/* PowerKey work func */
static void doubletap2wake_presspwr(struct work_struct * doubletap2wake_presspwr_work) {
	if (!mutex_trylock(&pwrkeyworklock))
                return;
	input_event(doubletap2wake_pwrdev, EV_KEY, KEY_POWER, 1);
	input_event(doubletap2wake_pwrdev, EV_SYN, 0, 0);
	msleep(DT2W_PWRKEY_DUR);
	input_event(doubletap2wake_pwrdev, EV_KEY, KEY_POWER, 0);
	input_event(doubletap2wake_pwrdev, EV_SYN, 0, 0);
	msleep(DT2W_PWRKEY_DUR);
        mutex_unlock(&pwrkeyworklock);
	return;
}
static DECLARE_WORK(doubletap2wake_presspwr_work, doubletap2wake_presspwr);

/* PowerKey trigger */
static void doubletap2wake_pwrtrigger(void) {
	schedule_work(&doubletap2wake_presspwr_work);
        return;
}

/* unsigned */
static unsigned int calc_feather(int coord, int prev_coord) {
	int calc_coord = 0;
	calc_coord = coord-prev_coord;
	if (calc_coord < 0)
		calc_coord = calc_coord * (-1);
	return calc_coord;
}

/* init a new touch */
static void new_touch(int x, int y) {
	tap_time_pre = jiffies;
	x_pre = x;
	y_pre = y;
	touch_nr++;
	wake_lock_timeout(&dt2w_wakelock, HZ/4);
}

/* Doubletap2wake main function */
static void detect_doubletap2wake(int x, int y, bool st)
{
	if (!flg_sensor_prox_detecting) {
        	bool single_touch = st;
#if DT2W_DEBUG
        	pr_info(LOGTAG"x,y(%4d,%4d) tap_time_pre:%llu\n",
                	x, y, tap_time_pre);
#endif
		if (x < 0 || x > 1440 )
         		return;

		/*if (dt2w_switch == 1 && y < 1300)
        		return;*/

		if (dt2w_switch == 1 && (y < 0 || y > 2560))
        		return;

		if ((single_touch) && (dt2w_switch > 0) && (exec_count) && (touch_cnt)) {
			touch_cnt = false;
			if (touch_nr == 0) {
				new_touch(x, y);
			} else if (touch_nr == 1) {
				if ((calc_feather(x, x_pre) < DT2W_FEATHER) &&
			    	(calc_feather(y, y_pre) < DT2W_FEATHER) &&
			    	((jiffies-tap_time_pre) < DT2W_TIME))
					touch_nr++;
				else {
					doubletap2wake_reset();
					new_touch(x, y);
				}
			} else {
				doubletap2wake_reset();
				new_touch(x, y);
			}
			if ((touch_nr > 1)) {
				pr_info(LOGTAG"ON\n");
				exec_count = false;
				doubletap2wake_pwrtrigger();
				doubletap2wake_reset();
			}
		}
	}
}

static void dt2w_input_callback(struct work_struct *unused) {

	detect_doubletap2wake(touch_x, touch_y, true);

	return;
}

static void dt2w_input_event(struct input_handle *handle, unsigned int type,
				unsigned int code, int value) {
#if DT2W_DEBUG
	pr_info("doubletap2wake: code: %s|%u, val: %i\n",
		((code==ABS_MT_POSITION_X) ? "X" :
		(code==ABS_MT_POSITION_Y) ? "Y" :
		(code==ABS_MT_TRACKING_ID) ? "ID" :
		"undef"), code, value);
#endif
	if (!scr_suspended)
		return;

	if (code == ABS_MT_SLOT) {
		doubletap2wake_reset();
		return;
	}

	if (code == ABS_MT_TRACKING_ID && value == -1) {
		touch_cnt = true;
		schedule_work_on(0, &dt2w_input_work);
		return;
	}

	if (code == ABS_MT_POSITION_X) {
		touch_x = value;
		touch_x_called = true;
	}

	if (code == ABS_MT_POSITION_Y) {
		touch_y = value;
		touch_y_called = true;
	}
}

static int input_dev_filter(struct input_dev *dev) {
	if (strstr(dev->name, "touch")) {
		return 0;
	} else {
		return 1;
	}
}

static int dt2w_input_connect(struct input_handler *handler,
				struct input_dev *dev, const struct input_device_id *id) {
	struct input_handle *handle;
	int error;

	if (input_dev_filter(dev))
		return -ENODEV;

	handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
	if (!handle)
		return -ENOMEM;

	handle->dev = dev;
	handle->handler = handler;
	handle->name = "dt2w";

	error = input_register_handle(handle);
	if (error)
		goto err2;

	error = input_open_device(handle);
	if (error)
		goto err1;

	return 0;
err1:
	input_unregister_handle(handle);
err2:
	kfree(handle);
	return error;
}

static void dt2w_input_disconnect(struct input_handle *handle) {
	input_close_device(handle);
	input_unregister_handle(handle);
	kfree(handle);
}

static const struct input_device_id dt2w_ids[] = {
	{ .driver_info = 1 },
	{ },
};

static struct input_handler dt2w_input_handler = {
	.event		= dt2w_input_event,
	.connect	= dt2w_input_connect,
	.disconnect	= dt2w_input_disconnect,
	.name		= "dt2w_inputreq",
	.id_table	= dt2w_ids,
};

#ifdef CONFIG_POWERSUSPEND
static void dt2w_early_suspend(struct power_suspend *h) {
	scr_suspended = true;
}

static void dt2w_late_resume(struct power_suspend *h) {
	scr_suspended = false;
}

static struct power_suspend dt2w_power_suspend_handler = {
	.suspend = dt2w_early_suspend,
	.resume = dt2w_late_resume,
};
#endif

/* sensor stuff */
void sensor_prox_report(unsigned int detected)
{
	if (detected) {
		pr_info(LOGTAG"/sensor_prox_report] prox covered\n");
		flg_sensor_prox_detecting = true;
		
	} else {
		flg_sensor_prox_detecting = false;
		pr_info(LOGTAG"/sensor_prox_report] prox uncovered\n");
	}
}
EXPORT_SYMBOL(sensor_prox_report);

/*
 * SYSFS stuff below here
 */
static ssize_t dt2w_doubletap2wake_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	size_t count = 0;

	count += sprintf(buf, "%d\n", dt2w_switch);

	return count;
}

static ssize_t dt2w_doubletap2wake_dump(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	if (buf[0] >= '0' && buf[0] <= '2' && buf[1] == '\n')
                if (dt2w_switch != buf[0] - '0')
		        dt2w_switch = buf[0] - '0';

	dt2w_toggled = true;

	return count;
}

static DEVICE_ATTR(doubletap2wake, (S_IWUSR|S_IRUGO),
	dt2w_doubletap2wake_show, dt2w_doubletap2wake_dump);

static ssize_t dt2w_version_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	size_t count = 0;

	count += sprintf(buf, "%s\n", DRIVER_VERSION);

	return count;
}

static ssize_t dt2w_version_dump(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	return count;
}

static DEVICE_ATTR(doubletap2wake_version, (S_IWUSR|S_IRUGO),
	dt2w_version_show, dt2w_version_dump);

/*
 * INIT / EXIT stuff below here
 */
#ifdef ANDROID_TOUCH_DECLARED
extern struct kobject *android_touch_kobj;
#else
struct kobject *android_touch_kobj;
EXPORT_SYMBOL_GPL(android_touch_kobj);
#endif
static int __init doubletap2wake_init(void)
{
	int rc = 0;

	doubletap2wake_pwrdev = input_allocate_device();
	if (!doubletap2wake_pwrdev) {
		pr_err("Can't allocate suspend autotest power button\n");
		goto err_alloc_dev;
	}

	input_set_capability(doubletap2wake_pwrdev, EV_KEY, KEY_POWER);
	doubletap2wake_pwrdev->name = "dt2w_pwrkey";
	doubletap2wake_pwrdev->phys = "dt2w_pwrkey/input0";

	rc = input_register_device(doubletap2wake_pwrdev);
	if (rc) {
		pr_err("%s: input_register_device err=%d\n", __func__, rc);
		goto err_input_dev;
	}

	INIT_WORK(&dt2w_input_work, dt2w_input_callback);
	rc = input_register_handler(&dt2w_input_handler);
	if (rc)
		pr_err("%s: Failed to register dt2w_input_handler\n", __func__);

	wake_lock_init(&dt2w_wakelock, WAKE_LOCK_SUSPEND, "dt2w_wakelock");

#ifdef CONFIG_POWERSUSPEND
	register_power_suspend(&dt2w_power_suspend_handler);
#endif

#ifndef ANDROID_TOUCH_DECLARED
	android_touch_kobj = kobject_create_and_add("android_touch", NULL) ;
	if (android_touch_kobj == NULL) {
		pr_warn("%s: android_touch_kobj create_and_add failed\n", __func__);
	}
#endif
	rc = sysfs_create_file(android_touch_kobj, &dev_attr_doubletap2wake.attr);
	if (rc) {
		pr_warn("%s: sysfs_create_file failed for doubletap2wake\n", __func__);
	}
	rc = sysfs_create_file(android_touch_kobj, &dev_attr_doubletap2wake_version.attr);
	if (rc) {
		pr_warn("%s: sysfs_create_file failed for doubletap2wake_version\n", __func__);
	}

err_input_dev:
	input_free_device(doubletap2wake_pwrdev);
err_alloc_dev:
	pr_info(LOGTAG"%s done\n", __func__);

	return 0;
}

static void __exit doubletap2wake_exit(void)
{
#ifndef ANDROID_TOUCH_DECLARED
	kobject_del(android_touch_kobj);
#endif
#ifdef CONFIG_POWERSUSPEND
	unregister_power_suspend(&dt2w_power_suspend_handler);
#endif
	input_unregister_handler(&dt2w_input_handler);
	input_unregister_device(doubletap2wake_pwrdev);
	input_free_device(doubletap2wake_pwrdev);
	return;
}

module_init(doubletap2wake_init);
module_exit(doubletap2wake_exit);

