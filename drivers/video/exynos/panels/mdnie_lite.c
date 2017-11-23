/* linux/drivers/video/mdnie.c
 *
 * Register interface file for Samsung mDNIe driver
 *
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/device.h>
#include <linux/backlight.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/lcd.h>
#include <linux/fb.h>
#include <linux/pm_runtime.h>

#include "mdnie.h"
extern void setMainTableData(int scenario, int mode , int index , int value);
extern int getMainTableData(int scenario, int mode , int index);


#define MDNIE_SYSFS_PREFIX		"/sdcard/mdnie/"
#define PANEL_COORDINATE_PATH	"/sys/class/lcd/panel/color_coordinate"

int     scenario_lock = 0;
int mdnie_cs = 0;
int mdnie_cs_index = 0;

int mdnie_saturation [157] = {
        0xEC,
        0x98, //lce_on gain 0 00 0000
        0x24, //lce_color_gain 00 0000
        0x10, //lce_scene_change_on scene_trans 0 0000
        0x14, //lce_min_diff
        0xb3, //lce_illum_gain
        0x01, //lce_ref_offset 9
        0x0e,
        0x01, //lce_ref_gain 9
        0x00,
        0x66, //lce_block_size h v 000 000
        0xfa, //lce_bright_th
        0x2d, //lce_bin_size_ratio
        0x03, //lce_dark_th 000
        0x96, //lce_min_ref_offset
        0x07, //nr fa de cs gamma 0 0000
        0xff, //nr_mask_th
        0x00, //de_gain 10
        0xc0,                 //[18]
        0x01, //de_maxplus 11   [19]
        0x00,//                 [20]
        0x01, //de_maxminus 11  [21]
        0x00,
        0x14, //fa_edge_th
        0x00, //fa_step_p 13
        0x0a,
        0x00, //fa_step_n 13
        0x32,
        0x01, //fa_max_de_gain 13
        0xf4,
        0x0b, //fa_pcl_ppi 14
        0x8a,
        0x6e, //fa_skin_cr
        0x99, //fa_skin_cb
        0x1b, //fa_dist_left
        0x17, //fa_dist_right
        0x14, //fa_dist_down
        0x1e, //fa_dist_up
        0x02, //fa_div_dist_left
        0x5f,
        0x02, //fa_div_dist_right
        0xc8,
        0x03, //fa_div_dist_down
        0x33,
        0x02, //fa_div_dist_up
        0x22,
        0x10, //fa_px_min_weight
        0x10, //fa_fr_min_weight
        0x07, //fa_skin_zone_w
        0x07, //fa_skin_zone_h
        0x20, //fa_os_cnt_10_co
        0x2d, //fa_os_cnt_20_co
        0x01, //cs_gain 10
        0x20,
        0x00, // curve_1_b
        0x14, // curve_1_a
        0x00, // curve_2_b
        0x14, // curve_2_a
        0x00, // curve_3_b
        0x14, // curve_3_a
        0x00, // curve_4_b
        0x14, // curve_4_a
        0x03, // curve_5_b
        0x9a, // curve_5_a
        0x03, // curve_6_b
        0x9a, // curve_6_a
        0x03, // curve_7_b
        0x9a, // curve_7_a
        0x03, // curve_8_b
        0x9a, // curve_8_a
        0x07, // curve_9_b
        0x9e, // curve_9_a
        0x07, // curve10_b
        0x9e, // curve10_a
        0x07, // curve11_b
        0x9e, // curve11_a
        0x07, // curve12_b
        0x9e, // curve12_a
        0x0a, // curve13_b
        0xa0, // curve13_a
        0x0a, // curve14_b
        0xa0, // curve14_a
        0x0a, // curve15_b
        0xa0, // curve15_a
        0x0a, // curve16_b
        0xa0, // curve16_a
        0x16, // curve17_b
        0xa6, // curve17_a
        0x16, // curve18_b
        0xa6, // curve18_a
        0x16, // curve19_b
        0xa6, // curve19_a
        0x16, // curve20_b
        0xa6, // curve20_a
        0x05, // curve21_b
        0x21, // curve21_a
        0x0b, // curve22_b
        0x20, // curve22_a
        0x87, // curve23_b
        0x0f, // curve23_a
        0x00, // curve24_b
        0xFF, // curve24_a
        0x30, //linear_on ascr_skin_on strength 0 0 00000
        0x67, //ascr_skin_cb
        0xa9, //ascr_skin_cr
        0x37, //ascr_dist_up
        0x29, //ascr_dist_down
        0x19, //ascr_dist_right
        0x47, //ascr_dist_left
        0x00, //ascr_div_up 20
        0x25,
        0x3d,
        0x00, //ascr_div_down
        0x31,
        0xf4,
        0x00, //ascr_div_right
        0x51,
        0xec,
        0x00, //ascr_div_left
        0x1c,
        0xd8,
        0xff, //ascr_skin_Rr
        0x62, //ascr_skin_Rg
        0x6c, //ascr_skin_Rb
        0xff, //ascr_skin_Yr
        0xff, //ascr_skin_Yg
        0x00, //ascr_skin_Yb
        0xff, //ascr_skin_Mr
        0x00, //ascr_skin_Mg
        0xff, //ascr_skin_Mb
        0xff, //ascr_skin_Wr
        0xf4, //ascr_skin_Wg
        0xff, //ascr_skin_Wb
        0x00, //ascr_Cr
        0xff, //ascr_Rr
        0xff, //ascr_Cg
        0x00, //ascr_Rg
        0xff, //ascr_Cb
        0x00, //ascr_Rb
        0xff, //ascr_Mr
        0x00, //ascr_Gr
        0x00, //ascr_Mg
        0xff, //ascr_Gg
        0xff, //ascr_Mb
        0x00, //ascr_Gb
        0xff, //ascr_Yr
        0x00, //ascr_Br
        0xff, //ascr_Yg
        0x00, //ascr_Bg
        0x00, //ascr_Yb
        0xff, //ascr_Bb
        0xff, //ascr_Wr
        0x00, //ascr_Kr
        0xff, //ascr_Wg
        0x00, //ascr_Kg
        0xff, //ascr_Wb
        0x00, //ascr_Kb
        /* end */
};
int hook = 0;

#define IS_DMB(idx)			(idx == DMB_NORMAL_MODE)
#define IS_SCENARIO(idx)		((idx < SCENARIO_MAX) && !((idx > VIDEO_NORMAL_MODE) && (idx < CAMERA_MODE)))
#define IS_ACCESSIBILITY(idx)		(idx && (idx < ACCESSIBILITY_MAX))
#define IS_HBM(idx)			(idx >= 6)
#if defined(CONFIG_LCD_HMT)
#define IS_HMT(idx)			(idx >= HMT_MDNIE_ON && idx < HMT_MDNIE_MAX)
#endif

#define SCENARIO_IS_VALID(idx)	(IS_DMB(idx) || IS_SCENARIO(idx))

/* Split 16 bit as 8bit x 2 */
#define GET_MSB_8BIT(x)		((x >> 8) & (BIT(8) - 1))
#define GET_LSB_8BIT(x)		((x >> 0) & (BIT(8) - 1))

static struct class *mdnie_class;

/* Do not call mdnie write directly */
static int mdnie_write(struct mdnie_info *mdnie, struct mdnie_table *table, unsigned int num)
{
	int ret = 0;

	if (mdnie->enable)
		ret = mdnie->ops.write(mdnie->data, table->seq, num);

	return ret;
}

static int mdnie_write_table(struct mdnie_info *mdnie, struct mdnie_table *table)
{
	int i, ret = 0;
	struct mdnie_table *buf = NULL;

	for (i = 0; table->seq[i].len; i++) {
		if (IS_ERR_OR_NULL(table->seq[i].cmd)) {
			dev_err(mdnie->dev, "mdnie sequence %s %dth command is null,\n", table->name, i);
			return -EPERM;
		}
	}

	mutex_lock(&mdnie->dev_lock);

	buf = table;

	ret = mdnie_write(mdnie, buf, i);

	mutex_unlock(&mdnie->dev_lock);

	return ret;
}

static struct mdnie_table *mdnie_find_table(struct mdnie_info *mdnie)
{
	struct mdnie_table *table = NULL;

	mutex_lock(&mdnie->lock);

	if (IS_ACCESSIBILITY(mdnie->accessibility)) {
		table = &mdnie->tune->accessibility_table[mdnie->accessibility];
		goto exit;
#ifdef CONFIG_LCD_HMT
	} else if (IS_HMT(mdnie->hmt_mode)) {
		table = &mdnie->tune->hmt_table[mdnie->hmt_mode];
		goto exit;
#endif
	} else if (IS_HBM(mdnie->auto_brightness)) {
		if ((mdnie->scenario == BROWSER_MODE) || (mdnie->scenario == EBOOK_MODE))
			table = &mdnie->tune->hbm_table[HBM_ON_TEXT];
		else
			table = &mdnie->tune->hbm_table[HBM_ON];
		goto exit;
#if defined(CONFIG_TDMB)
	} else if (IS_DMB(mdnie->scenario)) {
		table = &mdnie->tune->dmb_table[mdnie->mode];
		goto exit;
#endif
	} else if (IS_SCENARIO(mdnie->scenario)) {
		table = &mdnie->tune->main_table[mdnie->scenario][mdnie->mode];
		goto exit;
	}

exit:
	mutex_unlock(&mdnie->lock);

	return table;
}

static void mdnie_update_sequence(struct mdnie_info *mdnie, struct mdnie_table *table)
{
	if (mdnie->tuning)
		mdnie_request_table(mdnie->path, table);

	mdnie_write_table(mdnie, table);
	return;
}

static void mdnie_update(struct mdnie_info *mdnie)
{
	struct mdnie_table *table = NULL;
	struct mdnie_scr_info *scr_info = mdnie->tune->scr_info;

	if (!mdnie->enable) {
		dev_err(mdnie->dev, "mdnie state is off\n");
		return;
	}

    if (hook) {
        if (mdnie->mode == READING) {
            mdnie->mode = DYNAMIC;
        }
        mdnie->scenario = VT_MODE;
        setMainTableData(mdnie->scenario ,mdnie->mode,  15 , mdnie_saturation[15]);
        setMainTableData(mdnie->scenario ,mdnie->mode,  52 , mdnie_saturation[52]);
		setMainTableData(mdnie->scenario ,mdnie->mode,  18 , mdnie_saturation[18]);
        setMainTableData(mdnie->scenario ,mdnie->mode,  19 , 255);
        setMainTableData(mdnie->scenario ,mdnie->mode,  20 , 255);
        setMainTableData(mdnie->scenario ,mdnie->mode,  21 , 255);
    } else {
        if (mdnie->scenario == VT_MODE && mdnie->mode == DYNAMIC) {

        }
    }

	table = mdnie_find_table(mdnie);
	if (!IS_ERR_OR_NULL(table) && !IS_ERR_OR_NULL(table->name)) {

		mdnie_update_sequence(mdnie, table);
		dev_info(mdnie->dev, "%s\n", table->name);

		mdnie->white_r = table->seq[scr_info->index].cmd[scr_info->white_r];
		mdnie->white_g = table->seq[scr_info->index].cmd[scr_info->white_g];
		mdnie->white_b = table->seq[scr_info->index].cmd[scr_info->white_b];
	}

	return;
}

static void update_color_position(struct mdnie_info *mdnie, unsigned int idx)
{
	u8 mode, scenario;
	mdnie_t *wbuf;
	struct mdnie_scr_info *scr_info = mdnie->tune->scr_info;

	dev_info(mdnie->dev, "%s: idx=%d\n", __func__, idx);

	mutex_lock(&mdnie->lock);

	for (mode = 0; mode < READING; mode++) {
		for (scenario = 0; scenario <= EMAIL_MODE; scenario++) {
			wbuf = mdnie->tune->main_table[scenario][mode].seq[scr_info->index].cmd;
			if (IS_ERR_OR_NULL(wbuf))
				continue;
			if (scenario != EBOOK_MODE) {
				wbuf[scr_info->white_r] = mdnie->tune->coordinate_table[mode][idx * 3 + 0];
				wbuf[scr_info->white_g] = mdnie->tune->coordinate_table[mode][idx * 3 + 1];
				wbuf[scr_info->white_b] = mdnie->tune->coordinate_table[mode][idx * 3 + 2];
			}
		}
	}

	mutex_unlock(&mdnie->lock);
}

static int get_panel_coordinate(struct mdnie_info *mdnie, int *result)
{
	int ret = 0;
	char *fp = NULL;
	int x, y;

	ret = mdnie_open_file(PANEL_COORDINATE_PATH, &fp);
	if (IS_ERR_OR_NULL(fp) || ret <= 0) {
		dev_info(mdnie->dev, "%s: open skip: %s, %d\n", __func__, PANEL_COORDINATE_PATH, ret);
		ret = -EINVAL;
		goto skip_color_correction;
	}

	ret = sscanf(fp, "%d, %d", &x, &y);
	if ((ret != 2) || (!x && !y)) {
		dev_info(mdnie->dev, "%s: %d, %d\n", __func__, x, y);
		ret = -EINVAL;
		goto skip_color_correction;
	}

	result[1] = mdnie->tune->color_offset[0](x, y);
	result[2] = mdnie->tune->color_offset[1](x, y);
	result[3] = mdnie->tune->color_offset[2](x, y);
	result[4] = mdnie->tune->color_offset[3](x, y);

	ret = mdnie_calibration(result);
	dev_info(mdnie->dev, "%s: %d, %d, idx=%d\n", __func__, x, y, ret);

skip_color_correction:
	mdnie->color_correction = 1;
	if (!IS_ERR_OR_NULL(fp))
		kfree(fp);

	return ret;
}

static ssize_t mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", mdnie->mode);
}

static ssize_t mode_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	unsigned int value = 0;
	int ret;
	int result[5] = {0,};

	ret = kstrtoul(buf, 0, (unsigned long *)&value);
	if (ret < 0)
		return ret;

	dev_info(dev, "%s: value=%d\n", __func__, value);

	if (value >= MODE_MAX) {
		value = STANDARD;
		return -EINVAL;
	}

	mutex_lock(&mdnie->lock);
	mdnie->mode = value;
	mutex_unlock(&mdnie->lock);

	if (!mdnie->color_correction) {
		ret = get_panel_coordinate(mdnie, result);
		if (ret > 0)
			update_color_position(mdnie, ret);
	}

	mdnie_update(mdnie);

	return count;
}


static ssize_t scenario_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", mdnie->scenario);
}

static ssize_t scenario_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	unsigned int value;
	int ret;

	ret = kstrtoul(buf, 0, (unsigned long *)&value);
	if (ret < 0)
		return ret;

	if (scenario_lock == 0) {
		dev_info(dev, "%s: value=%d\n", __func__, value);
		if (!SCENARIO_IS_VALID(value))
			value = UI_MODE;

		mutex_lock(&mdnie->lock);
		mdnie->scenario = value;
		mutex_unlock(&mdnie->lock);

		mdnie_update(mdnie);		
	}
	return count;
}

static ssize_t tuning_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	char *pos = buf;
	struct mdnie_table *table = NULL;
	int i, idx;

	pos += sprintf(pos, "++ %s: %s\n", __func__, mdnie->path);

	if (!mdnie->tuning) {
		pos += sprintf(pos, "tunning mode is off\n");
		goto exit;
	}

	if (strncmp(mdnie->path, MDNIE_SYSFS_PREFIX, sizeof(MDNIE_SYSFS_PREFIX) - 1)) {
		pos += sprintf(pos, "file path is invalid, %s\n", mdnie->path);
		goto exit;
	}

	table = mdnie_find_table(mdnie);
	if (!IS_ERR_OR_NULL(table) && !IS_ERR_OR_NULL(table->name)) {
		mdnie_request_table(mdnie->path, table);
		for (idx = 0; table->seq[idx].len; idx++) {
			for (i = 0; i < table->seq[idx].len; i++)
				pos += sprintf(pos, "0x%02x ", table->seq[idx].cmd[i]);
		}
		pos += sprintf(pos, "\n");
	}

exit:
	pos += sprintf(pos, "-- %s\n", __func__);

	return pos - buf;
}

static ssize_t tuning_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	int ret;

	if (sysfs_streq(buf, "0") || sysfs_streq(buf, "1")) {
		ret = kstrtoul(buf, 0, (unsigned long *)&mdnie->tuning);
		if (ret < 0)
			return ret;
		if (!mdnie->tuning)
			memset(mdnie->path, 0, sizeof(mdnie->path));

		dev_info(dev, "%s: %s\n", __func__, mdnie->tuning ? "enable" : "disable");
	} else {
		if (!mdnie->tuning)
			return count;

		if (count > (sizeof(mdnie->path) - sizeof(MDNIE_SYSFS_PREFIX))) {
			dev_err(dev, "file name %s is too long\n", mdnie->path);
			return -ENOMEM;
		}

		memset(mdnie->path, 0, sizeof(mdnie->path));
		snprintf(mdnie->path, sizeof(MDNIE_SYSFS_PREFIX) + count-1, "%s%s", MDNIE_SYSFS_PREFIX, buf);
		dev_info(dev, "%s: %s\n", __func__, mdnie->path);

		mdnie_update(mdnie);
	}

	return count;
}

static ssize_t accessibility_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", mdnie->accessibility);
}

static ssize_t accessibility_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	unsigned int value, s[9], i = 0;
	int ret;
	mdnie_t *wbuf;
	struct mdnie_scr_info *scr_info = mdnie->tune->scr_info;

	ret = sscanf(buf, "%d %x %x %x %x %x %x %x %x %x",
		&value, &s[0], &s[1], &s[2], &s[3],
		&s[4], &s[5], &s[6], &s[7], &s[8]);

	dev_info(dev, "%s: value=%d\n", __func__, value);

	if (ret < 0)
		return ret;
	else {
		if (value >= ACCESSIBILITY_MAX)
			value = ACCESSIBILITY_OFF;

		mutex_lock(&mdnie->lock);
		mdnie->accessibility = value;
		if (value == COLOR_BLIND) {
			if (ret != 10) {
				mutex_unlock(&mdnie->lock);
				return -EINVAL;
			}
			wbuf = &mdnie->tune->accessibility_table[COLOR_BLIND].seq[scr_info->index].cmd[scr_info->color_blind];
			while (i < ARRAY_SIZE(s)) {
				wbuf[i * 2 + 0] = GET_LSB_8BIT(s[i]);
				wbuf[i * 2 + 1] = GET_MSB_8BIT(s[i]);
				i++;
			}

			dev_info(dev, "%s: %s\n", __func__, buf);
		}
		mutex_unlock(&mdnie->lock);

		mdnie_update(mdnie);
	}

	return count;
}

static ssize_t color_correct_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	char *pos = buf;
	int i, idx, result[5] = {0,};

	if (!mdnie->color_correction)
		return -EINVAL;

	idx = get_panel_coordinate(mdnie, result);

	for (i = 1; i < ARRAY_SIZE(result); i++)
		pos += sprintf(pos, "f%d: %d, ", i, result[i]);
	pos += sprintf(pos, "tune%d\n", idx);

	return pos - buf;
}

static ssize_t bypass_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", mdnie->bypass);
}

static ssize_t bypass_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	struct mdnie_table *table = NULL;
	unsigned int value;
	int ret;

	ret = kstrtoul(buf, 0, (unsigned long *)&value);

	dev_info(dev, "%s :: value=%d\n", __func__, value);

	if (ret < 0)
		return ret;
	else {
		if (value >= BYPASS_MAX)
			value = BYPASS_OFF;

		value = (value) ? BYPASS_ON : BYPASS_OFF;

		mutex_lock(&mdnie->lock);
		mdnie->bypass = value;
		mutex_unlock(&mdnie->lock);

		table = &mdnie->tune->bypass_table[value];
		if (!IS_ERR_OR_NULL(table)) {
			mdnie_write_table(mdnie, table);
			dev_info(mdnie->dev, "%s\n", table->name);
		}
	}

	return count;
}

static ssize_t auto_brightness_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);

	return sprintf(buf, "%d, hbm: %d\n", mdnie->auto_brightness, mdnie->hbm);
}

static ssize_t auto_brightness_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	unsigned int value;
	int ret;
	static unsigned int update;

	ret = kstrtoul(buf, 0, (unsigned long *)&value);
	if (ret < 0)
		return ret;

	dev_info(dev, "%s: value=%d\n", __func__, value);

	mutex_lock(&mdnie->lock);
	update = (IS_HBM(mdnie->auto_brightness) != IS_HBM(value)) ? 1 : 0;
	mdnie->hbm = IS_HBM(value) ? HBM_ON : HBM_OFF;
	mdnie->auto_brightness = value;
	mutex_unlock(&mdnie->lock);

	if (update)
		mdnie_update(mdnie);

	return count;
}

/* Temporary solution: Do not use this sysfs as official purpose */
static ssize_t mdnie_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	char *pos = buf;
	struct mdnie_table *table = NULL;
	int i, j;
	u8 *buffer;

	if (!mdnie->enable) {
		dev_err(mdnie->dev, "mdnie state is off\n");
		goto exit;
	}

	table = mdnie_find_table(mdnie);

	for (i = 0; table->seq[i].len; i++) {
		if (IS_ERR_OR_NULL(table->seq[i].cmd)) {
			dev_err(mdnie->dev, "mdnie sequence %s %dth command is null,\n", table->name, i);
			goto exit;
		}
	}

	pos += sprintf(pos, "+ %s\n", table->name);

	for (j = 0; table->seq[j].len; j++) {
		if (!table->update_flag[j]) {
			mdnie->ops.write(mdnie->data, &table->seq[j], 1);
			continue;
		}

		buffer = kzalloc(table->seq[j].len, GFP_KERNEL);

		mdnie->ops.read(mdnie->data, table->seq[j].cmd[0], buffer, table->seq[j].len - 1);

		pos += sprintf(pos, "  0:\t0x%02x\t0x%02x\n", table->seq[j].cmd[0], table->seq[j].cmd[0]);
		for (i = 0; i < table->seq[j].len - 1; i++) {
			pos += sprintf(pos, "%3d:\t0x%02x\t0x%02x", i + 1, table->seq[j].cmd[i+1], buffer[i]);
			if (table->seq[j].cmd[i+1] != buffer[i])
				pos += sprintf(pos, "\t(X)");
			pos += sprintf(pos, "\n");
		}

		kfree(buffer);
	}

	pos += sprintf(pos, "- %s\n", table->name);

exit:
	return pos - buf;
}

static ssize_t sensorRGB_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	return sprintf(buf, "%d %d %d\n", mdnie->white_r,
		mdnie->white_g, mdnie->white_b);
}

static ssize_t sensorRGB_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	struct mdnie_table *table = NULL;
	unsigned int white_red, white_green, white_blue;
	int ret;
	struct mdnie_scr_info *scr_info = mdnie->tune->scr_info;

	ret = sscanf(buf, "%d %d %d"
		, &white_red, &white_green, &white_blue);
	if (ret < 0)
		return ret;

	if (mdnie->enable && (mdnie->accessibility == ACCESSIBILITY_OFF)
		&& (mdnie->mode == AUTO)
		&& ((mdnie->scenario == BROWSER_MODE)
		|| (mdnie->scenario == EBOOK_MODE))) {
		dev_info(dev, "%s, white_r %d, white_g %d, white_b %d\n",
			__func__, white_red, white_green, white_blue);

		table = mdnie_find_table(mdnie);

		memcpy(&(mdnie->table_buffer),
			table, sizeof(struct mdnie_table));
		memcpy(mdnie->sequence_buffer,
			table->seq[scr_info->index].cmd,
			table->seq[scr_info->index].len);
		mdnie->table_buffer.seq[scr_info->index].cmd
			= mdnie->sequence_buffer;

		mdnie->table_buffer.seq[scr_info->index].cmd
			[scr_info->white_r] = (unsigned char)white_red;
		mdnie->table_buffer.seq[scr_info->index].cmd
			[scr_info->white_g] = (unsigned char)white_green;
		mdnie->table_buffer.seq[scr_info->index].cmd
			[scr_info->white_b] = (unsigned char)white_blue;

		mdnie->white_r = white_red;
		mdnie->white_g = white_green;
		mdnie->white_b = white_blue;

		mdnie_update_sequence(mdnie, &(mdnie->table_buffer));
	}

	return count;
}

#ifdef CONFIG_LCD_HMT
static ssize_t hmtColorTemp_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);

	// LukasAddon - return only digit
	//return sprintf(buf, "hmt_mode: %d\n", mdnie->hmt_mode);
	return sprintf(buf, "%d\n", mdnie->hmt_mode);
}

static ssize_t hmtColorTemp_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct mdnie_info *mdnie = dev_get_drvdata(dev);
	unsigned int value;
	int ret;

	ret = kstrtoul(buf, 0, (unsigned long *)&value);
	if (ret < 0)
		return ret;


	if (value != mdnie->hmt_mode && value < HMT_MDNIE_MAX) {
		mutex_lock(&mdnie->lock);
		mdnie->hmt_mode = value;
		mutex_unlock(&mdnie->lock);
		mdnie_update(mdnie);
	}

	return count;
}
#endif

static ssize_t scenario_lock_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	size_t count = 0;

	count += sprintf(buf, "%d\n", scenario_lock);

	return count;
}

static ssize_t scenario_lock_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	if (buf[0] >= '0' && buf[0] <= '1' && buf[1] == '\n')
                if (scenario_lock != buf[0] - '0')
		        scenario_lock = buf[0] - '0';

	return count;
}
//////////////////////////////////////////////////////////
static ssize_t mdnie_saturation_show(struct device *dev,
                             struct device_attribute *attr, char *buf)
{
    int ret;
    int adress;
    int value;
    size_t count = 0;
    struct mdnie_info *mdnie = dev_get_drvdata(dev);
//    ret = sscanf(buf, "%d"
//            , &adress);
    //count += sprintf(buf, "%u\n",getMainTableData(mdnie->scenario ,mdnie->mode, mdnie_cs_index));
    count += sprintf(buf, "%d\n",mdnie_saturation[52]);
    return count;
}

static ssize_t mdnie_saturation_store(struct device *dev,
                              struct device_attribute *attr, const char *buf, size_t count)
{
    int ret;
    int adress;
    int value;
    struct mdnie_info *mdnie = dev_get_drvdata(dev);
    ret = sscanf(buf, "%d",
             &value);
    mdnie_cs = value;
    mdnie_saturation[52] = value;
    //setMainTableData(mdnie->scenario ,mdnie->mode,  adress , value);
    mdnie_update(mdnie);
    return count;
}
//////////////////////////////////////////////////////////
static ssize_t mdnie_sharp_show(struct device *dev,
                                     struct device_attribute *attr, char *buf)
{
    int ret;
    int adress;
    int value;
    size_t count = 0;
    struct mdnie_info *mdnie = dev_get_drvdata(dev);
//    ret = sscanf(buf, "%d"
//            , &adress);
    //count += sprintf(buf, "%u\n",getMainTableData(mdnie->scenario ,mdnie->mode, mdnie_cs_index));
    count += sprintf(buf, "%d\n",mdnie_saturation[18]);
    return count;
}

static ssize_t mdnie_sharp_store(struct device *dev,
                                      struct device_attribute *attr, const char *buf, size_t count)
{
    int ret;
    int adress;
    int value;
    struct mdnie_info *mdnie = dev_get_drvdata(dev);
    ret = sscanf(buf, "%d",
                 &value);
    mdnie_cs = value;
    mdnie_saturation[18] = value;
    //setMainTableData(mdnie->scenario ,mdnie->mode,  adress , value);
    mdnie_update(mdnie);
    return count;
}
//////////////////////////////////////////////////////
static ssize_t mdnie_cs_show(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
    struct mdnie_info *mdnie = dev_get_drvdata(dev);
    size_t count = 0;

    count += sprintf(buf, "%u\n",getMainTableData(mdnie->scenario ,mdnie->mode, mdnie_cs_index));

    return count;
}

static ssize_t mdnie_cs_store(struct device *dev,
                                   struct device_attribute *attr, const char *buf, size_t count)
{
    int ret;
    int adress;
    int value;
    struct mdnie_info *mdnie = dev_get_drvdata(dev);

    ret = sscanf(buf, "%d"
            , &value);

    mdnie_cs = value;
    setMainTableData(mdnie->scenario ,mdnie->mode,  mdnie_cs_index , mdnie_cs);
    mdnie_update(mdnie);

    return count;
}
//////////////////////////////////////////////////////////////////////////
static ssize_t hook_show(struct device *dev,
                             struct device_attribute *attr, char *buf)
{
    struct mdnie_info *mdnie = dev_get_drvdata(dev);
    size_t count = 0;

    count += sprintf(buf, "%d\n",hook);

    return count;
}

static ssize_t hook_store(struct device *dev,
                              struct device_attribute *attr, const char *buf, size_t count)
{
    int ret;
    int adress;
    int value;
    struct mdnie_info *mdnie = dev_get_drvdata(dev);
    ret = sscanf(buf, "%d"
            , &value);
    hook = value;
    if (hook == 0) {
        setMainTableData(mdnie->scenario ,mdnie->mode,  52 , 1);
        setMainTableData(mdnie->scenario ,mdnie->mode,  15 , 7);

        setMainTableData(mdnie->scenario ,mdnie->mode,  18 , 192);
        setMainTableData(mdnie->scenario ,mdnie->mode,  19 , 1);
        setMainTableData(mdnie->scenario ,mdnie->mode,  20 , 0);
        setMainTableData(mdnie->scenario ,mdnie->mode,  21 , 1);
    }
    mdnie_update(mdnie);
    return count;
}
//////////////////////////////////////////////////////////////////////////
static ssize_t mdnie_cs_index_show(struct device *dev,
                             struct device_attribute *attr, char *buf)
{
    size_t count = 0;

    count += sprintf(buf, "%u\n",mdnie_cs_index);

    return count;
}

static ssize_t mdnie_cs_index_store(struct device *dev,
                              struct device_attribute *attr, const char *buf, size_t count)
{
    int ret;
    int adress;
    int value;
    struct mdnie_info *mdnie = dev_get_drvdata(dev);

    ret = sscanf(buf, "%d"
            , &value);

    mdnie_cs_index = value;

    mdnie_update(mdnie);

    return count;
}



static struct device_attribute mdnie_attributes[] = {
	__ATTR(mode, 0664, mode_show, mode_store),
	__ATTR(scenario, 0664, scenario_show, scenario_store),
	__ATTR(tuning, 0664, tuning_show, tuning_store),
	__ATTR(accessibility, 0664, accessibility_show, accessibility_store),
	__ATTR(color_correct, 0444, color_correct_show, NULL),
	__ATTR(bypass, 0664, bypass_show, bypass_store),
	__ATTR(auto_brightness, 0664, auto_brightness_show, auto_brightness_store),
	__ATTR(mdnie, 0444, mdnie_show, NULL),
	__ATTR(sensorRGB, 0664, sensorRGB_show, sensorRGB_store),
	__ATTR(scenario_lock, 0664, scenario_lock_show, scenario_lock_store),
    __ATTR(mdnie_cs_index, 0664, mdnie_cs_index_show, mdnie_cs_index_store),
    __ATTR(mdnie_cs, 0664, mdnie_cs_show, mdnie_cs_store),
    __ATTR(hook, 0664, hook_show, hook_store),
	__ATTR(mdnie_saturation, 0664, mdnie_saturation_show, mdnie_saturation_store),
    __ATTR(mdnie_sharp, 0664, mdnie_sharp_show, mdnie_sharp_store),
#ifdef CONFIG_LCD_HMT
	__ATTR(hmt_color_temperature, 0664, hmtColorTemp_show, hmtColorTemp_store),
#endif
	__ATTR_NULL,
};

static int fb_notifier_callback(struct notifier_block *self,
				 unsigned long event, void *data)
{
	struct mdnie_info *mdnie;
	struct fb_event *evdata = data;
	int fb_blank;

	switch (event) {
	case FB_EVENT_BLANK:
		break;
	default:
		return 0;
	}

	mdnie = container_of(self, struct mdnie_info, fb_notif);

	fb_blank = *(int *)evdata->data;

	dev_info(mdnie->dev, "%s: %d\n", __func__, fb_blank);

	if (evdata->info->node != 0)
		return 0;

	if (fb_blank == FB_BLANK_UNBLANK) {
		mutex_lock(&mdnie->lock);
		mdnie->enable = 1;
		mutex_unlock(&mdnie->lock);

		mdnie_update(mdnie);
	} else if (fb_blank == FB_BLANK_POWERDOWN) {
		mutex_lock(&mdnie->lock);
		mdnie->enable = 0;
		mutex_unlock(&mdnie->lock);
	}

	return 0;
}

static int mdnie_register_fb(struct mdnie_info *mdnie)
{
	memset(&mdnie->fb_notif, 0, sizeof(mdnie->fb_notif));
	mdnie->fb_notif.notifier_call = fb_notifier_callback;
	return fb_register_client(&mdnie->fb_notif);
}

int mdnie_register(struct device *p, void *data, mdnie_w w, mdnie_r r, struct mdnie_tune *tune)
{
	int ret = 0;
	struct mdnie_info *mdnie;
	static unsigned int mdnie_no;

	if (IS_ERR_OR_NULL(mdnie_class)) {
		mdnie_class = class_create(THIS_MODULE, "mdnie");
		if (IS_ERR_OR_NULL(mdnie_class)) {
			pr_err("failed to create mdnie class\n");
			ret = -EINVAL;
			goto error0;
		}
		mdnie_class->dev_attrs = mdnie_attributes;
	}

	mdnie = kzalloc(sizeof(struct mdnie_info), GFP_KERNEL);
	if (!mdnie) {
		pr_err("failed to allocate mdnie\n");
		ret = -ENOMEM;
		goto error1;
	}

	mdnie->dev = device_create(mdnie_class, p, 0, &mdnie, !mdnie_no ? "mdnie" : "mdnie%d", mdnie_no);
	if (IS_ERR_OR_NULL(mdnie->dev)) {
		pr_err("failed to create mdnie device\n");
		ret = -EINVAL;
		goto error2;
	}

	mdnie_no++;
	mdnie->scenario = UI_MODE;
	mdnie->mode = STANDARD;
	mdnie->enable = 0;
	mdnie->tuning = 0;
	mdnie->accessibility = ACCESSIBILITY_OFF;
	mdnie->bypass = BYPASS_OFF;

	mdnie->data = data;
	mdnie->ops.write = w;
	mdnie->ops.read = r;

	mdnie->tune = tune;

	mutex_init(&mdnie->lock);
	mutex_init(&mdnie->dev_lock);

	dev_set_drvdata(mdnie->dev, mdnie);

	mdnie_register_fb(mdnie);

	mdnie->enable = 1;
	mdnie_update(mdnie);

	dev_info(mdnie->dev, "registered successfully\n");

	return 0;

error2:
	kfree(mdnie);
error1:
	class_destroy(mdnie_class);
error0:
	return ret;
}

