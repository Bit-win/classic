/*
 * SPDX-FileCopyrightText: 2022 Unisoc (Shanghai) Technologies Co., Ltd
 * SPDX-License-Identifier: LicenseRef-Unisoc-General-1.0
 *
 * Copyright 2022 Unisoc (Shanghai) Technologies Co., Ltd.
 * Licensed under the Unisoc General Software License, version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * https://www.unisoc.com/en_us/license/UNISOC_GENERAL_LICENSE_V1.0-EN_US
 * Software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 * See the Unisoc General Software License, version 1.0 for more details.
 */
/*History
*Date                  Modification                                 Reason
*
*/

#define LOG_TAG "c3z03_ov32d40_backst"

#include "c3z03_ov32d40_backst.h"
pthread_mutex_t ov32d40_sensor_mutex;
static struct sensor_ic_drv_cxt *ov32d40_sns_drv_cxt = NULL;

/*==============================================================================
 * Description:
 * write register value to sensor
 * please modify this function acording your spec
 *============================================================================*/

static void ov32d40_drv_write_reg2sensor(cmr_handle handle,
                                         struct sensor_i2c_reg_tab *reg_info) {
    SENSOR_IC_CHECK_PTR_VOID(reg_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);

    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_int i = 0;

    for (i = 0; i < reg_info->size; i++) {

        hw_sensor_write_reg(sns_drv_cxt->hw_handle,
                            reg_info->settings[i].reg_addr,
                            reg_info->settings[i].reg_value);
    }
}

/*==============================================================================
 * Description:
 * write gain to sensor registers buffer
 * please modify this function acording your spec
 *============================================================================*/
static void ov32d40_drv_write_gain(cmr_handle handle,
                                   struct sensor_aec_i2c_tag *aec_info,
                                   cmr_u32 gain) {
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

	if (aec_info->again->size) {
		gain = (gain < SENSOR_BASE_GAIN) ? SENSOR_BASE_GAIN : gain;
		gain = (gain > SENSOR_MAX_GAIN) ? SENSOR_MAX_GAIN : gain;

		aec_info->again->settings[0].reg_value = (gain >> 8) & 0xff;
		aec_info->again->settings[1].reg_value = gain & 0xff;
	}

    if (aec_info->dgain->size) {

        /*TODO*/

        /*END*/
    }
}

/*==============================================================================
 * Description:
 * write frame length to sensor registers buffer
 * please modify this function acording your spec
 *============================================================================*/
static void ov32d40_drv_write_frame_length(cmr_handle handle,
                                           struct sensor_aec_i2c_tag *aec_info,
                                           cmr_u32 frame_len) {
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if (aec_info->frame_length->size) {
        /*TODO*/

        aec_info->frame_length->settings[0].reg_value = (frame_len >> 8) & 0xff;
        aec_info->frame_length->settings[1].reg_value = frame_len & 0xff;

        /*END*/
    }
}

/*==============================================================================
 * Description:
 * write shutter to sensor registers buffer
 * please pay attention to the frame length
 * please modify this function acording your spec
 *============================================================================*/
static void ov32d40_drv_write_shutter(cmr_handle handle,
                                      struct sensor_aec_i2c_tag *aec_info,
                                      cmr_u32 shutter) {
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if (aec_info->shutter->size) {
        /*TODO*/

        aec_info->shutter->settings[0].reg_value = (shutter >> 16) & 0xff;
        aec_info->shutter->settings[1].reg_value = (shutter >> 8) & 0xff;
        aec_info->shutter->settings[2].reg_value = shutter & 0xff;
        /*END*/
    }
}

/*==============================================================================
 * Description:
 * write exposure to sensor registers and get current shutter
 * please pay attention to the frame length
 * please don't change this function if it's necessary
 *============================================================================*/
static void ov32d40_drv_calc_exposure(cmr_handle handle, cmr_u32 shutter,
                                      cmr_u32 dummy_line, cmr_u16 mode,
                                      struct sensor_aec_i2c_tag *aec_info) {
    cmr_u32 dest_fr_len = 0;
    cmr_u32 cur_fr_len = 0;
    cmr_u32 fr_len = 0;
    float fps = 0.0;
    cmr_u16 frame_interval = 0x00;

    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    sns_drv_cxt->frame_length_def = sns_drv_cxt->trim_tab_info[mode].frame_line;
    sns_drv_cxt->line_time_def = sns_drv_cxt->trim_tab_info[mode].line_time;
    cur_fr_len = sns_drv_cxt->sensor_ev_info.preview_framelength;
    fr_len = sns_drv_cxt->frame_length_def;

    dummy_line = dummy_line > FRAME_OFFSET ? dummy_line : FRAME_OFFSET;
    dest_fr_len =
        ((shutter + dummy_line) > fr_len) ? (shutter + dummy_line) : fr_len;
    sns_drv_cxt->frame_length = dest_fr_len;

    if (shutter < SENSOR_MIN_SHUTTER)
        shutter = SENSOR_MIN_SHUTTER;

    if (cur_fr_len > shutter) {
        fps = 1000000000.0 /
              (cur_fr_len * sns_drv_cxt->trim_tab_info[mode].line_time);
    } else {
        fps = 1000000000.0 / ((shutter + dummy_line) *
                              sns_drv_cxt->trim_tab_info[mode].line_time);
    }

    frame_interval = (cmr_u16)(
        ((shutter + dummy_line) * sns_drv_cxt->line_time_def) / 1000000);
    SENSOR_LOGD(
        "mode = %d, exposure_line = %d, dummy_line= %d, frame_interval= %d ms, fps = %f",
        mode, shutter, dummy_line, frame_interval, fps);

    if (dest_fr_len != cur_fr_len) {
        sns_drv_cxt->sensor_ev_info.preview_framelength = dest_fr_len;
        ov32d40_drv_write_frame_length(handle, aec_info, dest_fr_len);
    }
    sns_drv_cxt->sensor_ev_info.preview_shutter = shutter;
    ov32d40_drv_write_shutter(handle, aec_info, shutter);
    if(sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle,
                          SENSOR_EXIF_CTRL_EXPOSURETIME, shutter);
    }
}

static void ov32d40_drv_calc_gain(cmr_handle handle, cmr_uint isp_gain,
                                  struct sensor_aec_i2c_tag *aec_info) {
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u32 sensor_gain = 0;

    sensor_gain = isp_gain < ISP_BASE_GAIN ? ISP_BASE_GAIN : isp_gain;
    sensor_gain = sensor_gain * SENSOR_BASE_GAIN / ISP_BASE_GAIN;//sensor_basegain:256   isp_basegain:128

    /*sensor min gain=sensor_basegain x 1.125*/
    if (sensor_gain < SENSOR_MIN_GAIN)
        sensor_gain = SENSOR_MIN_GAIN;

    /*sensor mmax gain=sensor_basegain x 62*/
    if (SENSOR_MAX_GAIN < sensor_gain)
        sensor_gain = SENSOR_MAX_GAIN;

    SENSOR_LOGD("isp_gain = 0x%x,sensor_gain=0x%x", (unsigned int)isp_gain,
                sensor_gain);

    sns_drv_cxt->sensor_ev_info.preview_gain = sensor_gain;
    ov32d40_drv_write_gain(handle, aec_info, sensor_gain);
}

/*==============================================================================
 * Description:
 * sensor power on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int ov32d40_drv_power_on(cmr_handle handle, cmr_uint power_on) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;

    SENSOR_AVDD_VAL_E dvdd_val = module_info->dvdd_val;
    SENSOR_AVDD_VAL_E avdd_val = module_info->avdd_val;
    SENSOR_AVDD_VAL_E iovdd_val = module_info->iovdd_val;
    BOOLEAN power_down = g_ov32d40_mipi_raw_info.power_down_level;
    BOOLEAN reset_level = g_ov32d40_mipi_raw_info.reset_pulse_level;

    if (SENSOR_TRUE == power_on) {
        hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);

        usleep(1 * 1000);
        hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, iovdd_val);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, avdd_val);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, dvdd_val);

        usleep(1 * 1000);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, EX_MCLK);
        usleep(1 * 1000);
        hw_sensor_power_down(sns_drv_cxt->hw_handle, !power_down);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, !reset_level);
        usleep(5 * 1000);

        hw_sensor_set_mipi_level(sns_drv_cxt->hw_handle, 1);
    } else {

        hw_sensor_set_mipi_level(sns_drv_cxt->hw_handle, 0);

        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
        usleep(1 * 1000);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
    }

    SENSOR_LOGI("(1:on, 0:off): %lu", power_on);
    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * calculate fps for every sensor mode according to frame_line and line_time
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int ov32d40_drv_init_fps_info(cmr_handle handle) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
    struct sensor_trim_tag *trim_info = sns_drv_cxt->trim_tab_info;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;

    SENSOR_LOGI("E");
    if (!fps_info->is_init) {
        cmr_u32 i, modn, tempfps = 0;
        SENSOR_LOGI("start init");
        for (i = 0; i < SENSOR_MODE_MAX; i++) {
            // max fps should be multiple of 30,it calulated from line_time and
            // frame_line
            tempfps = trim_info[i].line_time * trim_info[i].frame_line;
            if (0 != tempfps) {
                tempfps = 1000000000 / tempfps;
                modn = tempfps / 30;
                if (tempfps > modn * 30)
                    modn++;
                fps_info->sensor_mode_fps[i].max_fps = modn * 30;
                if (fps_info->sensor_mode_fps[i].max_fps > 30) {
                    fps_info->sensor_mode_fps[i].is_high_fps = 1;
                    fps_info->sensor_mode_fps[i].high_fps_skip_num =
                        fps_info->sensor_mode_fps[i].max_fps / 30;
                }
                if (fps_info->sensor_mode_fps[i].max_fps >
                    static_info->max_fps) {
                    static_info->max_fps = fps_info->sensor_mode_fps[i].max_fps;
                }
            }
            SENSOR_LOGI("mode %d,tempfps %d,frame_len %d,line_time: %d ", i,
                        tempfps, trim_info[i].frame_line,
                        trim_info[i].line_time);
            SENSOR_LOGI("mode %d,max_fps: %d ", i,
                        fps_info->sensor_mode_fps[i].max_fps);
            SENSOR_LOGI("is_high_fps: %d,highfps_skip_num %d",
                        fps_info->sensor_mode_fps[i].is_high_fps,
                        fps_info->sensor_mode_fps[i].high_fps_skip_num);
        }
        fps_info->is_init = 1;
    }
    SENSOR_LOGI("X");
    return rtn;
}

static cmr_int ov32d40_drv_get_static_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_ex_info *ex_info = (struct sensor_ex_info *)param;
    cmr_u32 up = 0;
    cmr_u32 down = 0;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(ex_info);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;

    // make sure we have get max fps of all settings.
    if (!fps_info->is_init) {
        ov32d40_drv_init_fps_info(handle);
    }
    ex_info->f_num = static_info->f_num;
    ex_info->focal_length = static_info->focal_length;
    ex_info->max_fps = static_info->max_fps;
    ex_info->max_adgain = static_info->max_adgain;
    ex_info->ois_supported = static_info->ois_supported;
    ex_info->pdaf_supported = static_info->pdaf_supported;
    ex_info->exp_valid_frame_num = static_info->exp_valid_frame_num;
    ex_info->clamp_level = static_info->clamp_level;
    ex_info->adgain_valid_frame_num = static_info->adgain_valid_frame_num;
    ex_info->preview_skip_num = module_info->preview_skip_num;
    ex_info->capture_skip_num = module_info->capture_skip_num;
    ex_info->name = (cmr_s8 *)g_ov32d40_mipi_raw_info.name;
    ex_info->sensor_version_info =
        (cmr_s8 *)g_ov32d40_mipi_raw_info.sensor_version_info;
    memcpy(&ex_info->fov_info, &static_info->fov_info,
           sizeof(static_info->fov_info));
    ex_info->pos_dis.up2hori = up;
    ex_info->pos_dis.hori2down = down;
    sensor_ic_print_static_info((cmr_s8 *)SENSOR_NAME, ex_info);

    return rtn;
}

static cmr_int ov32d40_drv_get_fps_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_MODE_FPS_T *fps_info = (SENSOR_MODE_FPS_T *)param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(fps_info);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_fps_info *fps_data = sns_drv_cxt->fps_info;

    // make sure have inited fps of every sensor mode.
    if (!fps_data->is_init) {
        ov32d40_drv_init_fps_info(handle);
    }
    cmr_u32 sensor_mode = fps_info->mode;
    fps_info->max_fps = fps_data->sensor_mode_fps[sensor_mode].max_fps;
    fps_info->min_fps = fps_data->sensor_mode_fps[sensor_mode].min_fps;
    fps_info->is_high_fps = fps_data->sensor_mode_fps[sensor_mode].is_high_fps;
    fps_info->high_fps_skip_num =
        fps_data->sensor_mode_fps[sensor_mode].high_fps_skip_num;
    SENSOR_LOGI("mode %d, max_fps: %d", fps_info->mode, fps_info->max_fps);
    SENSOR_LOGI("min_fps: %d", fps_info->min_fps);
    SENSOR_LOGI("is_high_fps: %d", fps_info->is_high_fps);
    SENSOR_LOGI("high_fps_skip_num: %d", fps_info->high_fps_skip_num);

    return rtn;
}

/*block size 16x16 normal pos(x,y) mirror(16-x,y) flip(x,16-y)
 * mirror&&flip(16-x, 16-y)*/

#if 0 // defined(CONFIG_DUAL_MODULE)
// static int  mirror_diasble_factor = -1; //enable: -1  diable: 1
// static int  flip_disable_factor = -1; //enable: -1  diable: 1

static const cmr_u16 ov32d40_pd_is_right[] = {0, 1, 1, 0};
static const cmr_u16 ov32d40_pd_row[] = {2, 6, 10, 14}; //{14, 10, 6, 2};//y

static const cmr_u16 ov32d40_pd_col[] = {10, 10, 2, 2}; //{2, 2, 10, 10};//x
static const struct pd_pos_info ov32d40_pd_pos_r[] = {
    {10, 6}, {2, 10},
};

static const struct pd_pos_info ov32d40_pd_pos_l[] = {
    {10, 2}, {2, 14},
};

#else
// static int  mirror_diasble_factor = 1; //enable: -1  diable: 1
// static int  flip_disable_factor = 1; //enable:-1  diable: 1

//pd pattern for 3264x1836
static const cmr_u16 ov32d40_3264x1836_pd_is_right[] = {1, 0, 1, 0};

static const cmr_u16 ov32d40_3264x1836_pd_row[] = {2, 2, 6, 6}; //y

static const cmr_u16 ov32d40_3264x1836_pd_col[] = {5, 6, 1, 2}; //x

static const struct pd_pos_info ov32d40_3264x1836_pd_pos_r[] = {
    {5, 2}, {1, 6},
};

static const struct pd_pos_info ov32d40_3264x1836_pd_pos_l[] = {
    {6, 2}, {2, 6},
};

static const cmr_u16 ov32d40_pd_is_right[] = {1, 0, 1, 0};

static const cmr_u16 ov32d40_pd_row[] = {2, 2, 6, 6};

static const cmr_u16 ov32d40_pd_col[] = {5, 6, 1, 2};
static const struct pd_pos_info ov32d40_pd_pos_r[] = {
    {5, 2}, {1, 6},
};

static const struct pd_pos_info ov32d40_pd_pos_l[] = {
    {6, 2}, {2, 6},
};
#endif
static const cmr_u32 ov32d40_pd_sns_mode[] = {0, 1, 1, 1};
#define PDAF_BLOCK_SIZE_W 8
#define PDAF_BLOCK_SIZE_H 8

struct pdaf_coordinate_tab ov32d40_pd_coordinate_table[] = {
    {.number = 2,
     .pos_info = {0, 1},
    },
    {.number = 2,
     .pos_info = {0, 1},
    },
};

struct pdaf_block_descriptor ov32d40_pd_seprator_helper = {
    .block_width = 2,
    .block_height = 2,
    .line_width = 2,
    .sensor_line_alignment = 8,
    .platform_line_alignment = 4,
    .block_pattern = CROSS_PATTERN,
    .pd_line_coordinate = ov32d40_pd_coordinate_table,
};

struct sensor_pdaf_iszfull_info ov32d40_pd_iszfull_info = {
    .cell_num = 2,
    .effective_pixel_w = 6528,
    .effective_pixel_h = 4896,
    .full_pd_offset_x = 0,
    .full_pd_offset_y = 0,
    .full_pd_area_w = 6528,
    .full_pd_area_h = 4896,
};

struct sensor_pdaf_info ov32d40_pdaf_info_array[] = {
	/* pd setting for sensor mode0 */
	{0},
	/* pd setting for sensor mode1 */
	{
	.pd_is_right = (cmr_u16 *)ov32d40_3264x1836_pd_is_right,
	.pd_pos_row = (cmr_u16 *)ov32d40_3264x1836_pd_row,
	.pd_pos_col = (cmr_u16 *)ov32d40_3264x1836_pd_col,
	.pd_pos_r = (struct pd_pos_info *)ov32d40_3264x1836_pd_pos_r,
	.pd_pos_l = (struct pd_pos_info *)ov32d40_3264x1836_pd_pos_l,
	.pd_pos_size = NUMBER_OF_ARRAY(ov32d40_3264x1836_pd_pos_r),
	.pd_offset_x = 0,
	.pd_offset_y = 6,
	.pd_end_x = 3264,
	.pd_end_y = 1830,
	.pd_size_w = VIDEO_TRIM_W,
	.pd_size_h = VIDEO_TRIM_H,
	.pd_block_w = 0,
	.pd_block_h = 0,
	.pd_block_num_x = 408,
	.pd_block_num_y = 228,
	.pd_density_x = 8,
	.pd_density_y = 4,
	.pd_pitch_x = 8,
	.pd_pitch_y = 8,
	.vendor_type = SENSOR_VENDOR_OV32D40,
	.vch2_info.bypass = 0,
	.vch2_info.vch2_vc = 1,
	.vch2_info.vch2_data_type = 0x19,
	.vch2_info.vch2_mode = 0x02,
	.sns_mode = ov32d40_pd_sns_mode,
	.descriptor = &ov32d40_pd_seprator_helper,
	.sns_orientation = 0, /*1: mirror+flip; 0: normal*/
    /*for PD_ALGO_4.x */
  	.image_process_type = 1, // 0:default; 1:only crop; 2:binning+crop;
  	.crop_offset_x = 352,
  	.crop_offset_y = 504,
    /*for PD_ALGO_5.0 */
    .pdaf_type = SENSOR_PDAF_TYPE2_ENABLE,
    .full_process_type = PDAF_MAP_BINNING_CROP,
    .full_crop_x = 0,
    .full_crop_y = 306,
    .bin_ratio_w = 2,
    .bin_ratio_h = 2,
	},
	/* pd setting for sensor mode2 */
	{
	.pd_is_right = (cmr_u16 *)ov32d40_pd_is_right,
	.pd_pos_row = (cmr_u16 *)ov32d40_pd_row,
	.pd_pos_col = (cmr_u16 *)ov32d40_pd_col,
	.pd_pos_r = (struct pd_pos_info *)ov32d40_pd_pos_r,
	.pd_pos_l = (struct pd_pos_info *)ov32d40_pd_pos_l,
	.pd_pos_size = NUMBER_OF_ARRAY(ov32d40_pd_pos_r),
	.pd_offset_x = 0,
	.pd_offset_y = 0,
	.pd_end_x = SNAPSHOT_TRIM_W,
	.pd_end_y = SNAPSHOT_TRIM_H,
	.pd_size_w = SNAPSHOT_TRIM_W,
	.pd_size_h = SNAPSHOT_TRIM_H,
	.pd_block_w = 0,
	.pd_block_h = 0,
	.pd_block_num_x = 408,
	.pd_block_num_y = 306,
	.pd_density_x = 8,
	.pd_density_y = 4,
	.pd_pitch_x = 8,
	.pd_pitch_y = 8,
	.vendor_type = SENSOR_VENDOR_OV32D40,
	.vch2_info.bypass = 0,
	.vch2_info.vch2_vc = 1,
	.vch2_info.vch2_data_type = 0x19,
	.vch2_info.vch2_mode = 0x02,
	.sns_mode = ov32d40_pd_sns_mode,
	.descriptor = &ov32d40_pd_seprator_helper,
	.sns_orientation = 0, /*1: mirror+flip; 0: normal*/
    /*for PD_ALGO_4.x */
  	.image_process_type = 0, // 0:default; 1:only crop; 2:binning+crop;
  	.crop_offset_x = 0,
  	.crop_offset_y = 0,
    /*for PD_ALGO_5.0 */
    .pdaf_type = SENSOR_PDAF_TYPE2_ENABLE,
    .full_process_type = PDAF_MAP_BINNING,
    .full_crop_x = 0,
    .full_crop_y = 0,
    .bin_ratio_w = 2,
    .bin_ratio_h = 2,
	},
    /* pd setting for sensor mode3 */
    {
	.pd_is_right = (cmr_u16 *)ov32d40_pd_is_right,
	.pd_pos_row = (cmr_u16 *)ov32d40_pd_row,
	.pd_pos_col = (cmr_u16 *)ov32d40_pd_col,
	.pd_pos_r = (struct pd_pos_info *)ov32d40_pd_pos_r,
	.pd_pos_l = (struct pd_pos_info *)ov32d40_pd_pos_l,
	.pd_pos_size = NUMBER_OF_ARRAY(ov32d40_pd_pos_r),
	.pd_offset_x = 0,
	.pd_offset_y = 0,
	.pd_end_x = SNAPSHOT_TRIM_W,
	.pd_end_y = SNAPSHOT_TRIM_H,
	.pd_size_w = SNAPSHOT_TRIM_W,
	.pd_size_h = SNAPSHOT_TRIM_H,
	.pd_block_w = 0,
	.pd_block_h = 0,
	.pd_block_num_x = 408,
	.pd_block_num_y = 306,
	.pd_density_x = 8,
	.pd_density_y = 4,
	.pd_pitch_x = 8,
	.pd_pitch_y = 8,
	.vendor_type = SENSOR_VENDOR_OV32D40,
	.vch2_info.bypass = 0,
	.vch2_info.vch2_vc = 1,
	.vch2_info.vch2_data_type = 0x19,
	.vch2_info.vch2_mode = 0x02,
	.sns_mode = ov32d40_pd_sns_mode,
	.descriptor = &ov32d40_pd_seprator_helper,
	.sns_orientation = 0, /*1: mirror+flip; 0: normal*/
    /*for PD_ALGO_4.x */
  	.image_process_type = 0, // 0:default; 1:only crop; 2:binning+crop;
  	.crop_offset_x = 0,
  	.crop_offset_y = 0,
    /*for PD_ALGO_5.0 */
    .pdaf_type = SENSOR_PDAF_TYPE2_ENABLE,
    .full_process_type = PDAF_MAP_BINNING,
    .full_crop_x = 0,
    .full_crop_y = 0,
    .bin_ratio_w = 2,
    .bin_ratio_h = 2,
	},
    
	/* add more pdaf setting here */
};

static struct sensor_pdaf_info_tab ov32d40_pdaf_info_tab = {
    .sns_mode_num = NUMBER_OF_ARRAY(ov32d40_pd_sns_mode),
	.pd_sns_mode = ov32d40_pd_sns_mode,
	.pdaf_init_index = 2,
	.pd_init_size_w = SNAPSHOT_WIDTH,
	.pd_init_size_h = SNAPSHOT_HEIGHT,
	.pd_info_tab = ov32d40_pdaf_info_array,
    .pd_iszfull_info = &ov32d40_pd_iszfull_info,
};

cmr_int ov32d40_drv_pdaf_data_process(void *buffer_handle);

static cmr_int ov32d40_drv_get_pdaf_info(cmr_handle handle, void *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_pdaf_info_tab *pdaf_info_tab = NULL;
    struct sensor_pdaf_info *pdaf_info = NULL;
    cmr_int pd_info_num = 0;
    int i = 0;
    SENSOR_IC_CHECK_PTR(param);

    SENSOR_LOGV("E");

    pdaf_info_tab = (struct sensor_pdaf_info_tab *)param;
    
    pdaf_info_tab->sns_mode_num = ov32d40_pdaf_info_tab.sns_mode_num;
    pdaf_info_tab->pd_sns_mode = ov32d40_pdaf_info_tab.pd_sns_mode;
    pdaf_info_tab->pdaf_init_index = ov32d40_pdaf_info_tab.pdaf_init_index;
    pdaf_info_tab->pd_init_size_w = ov32d40_pdaf_info_tab.pd_init_size_w;
    pdaf_info_tab->pd_init_size_h = ov32d40_pdaf_info_tab.pd_init_size_h;
    pdaf_info_tab->pd_info_tab = ov32d40_pdaf_info_tab.pd_info_tab;
    pdaf_info_tab->pd_iszfull_info = ov32d40_pdaf_info_tab.pd_iszfull_info;

    pd_info_num = ov32d40_pdaf_info_tab.sns_mode_num;
    for(i = 0; i < pd_info_num; i++) {
        if(pdaf_info_tab->pd_sns_mode[i]){
            pdaf_info = &(pdaf_info_tab->pd_info_tab[i]);
            pdaf_info->pdaf_format_converter = ov32d40_drv_pdaf_data_process;
            pdaf_info->pd_data_size = pdaf_info->pd_block_num_x * pdaf_info->pd_block_num_y *
                                      pdaf_info->pd_pos_size * 2 * 5;
            SENSOR_LOGD("pd data size:%d", pdaf_info->pd_data_size);
        }
    }
    SENSOR_LOGD("sensor pdaf_init_index %d", pdaf_info_tab->pdaf_init_index);
    return rtn;
}
#if SUPPORT_4IN1
static const cmr_u32 sns_4in1_mode[] = {0, 0, 0, 1};
static cmr_int ov32d40_drv_get_4in1_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_4in1_info *sn_4in1_info = NULL;
    SENSOR_IC_CHECK_PTR(param);

    SENSOR_LOGI("E");

    sn_4in1_info = (struct sensor_4in1_info *)param;
    sn_4in1_info->is_4in1_supported = 0;
    sn_4in1_info->limited_4in1_width = SNAPSHOT_WIDTH;
    sn_4in1_info->limited_4in1_height = SNAPSHOT_HEIGHT;
    sn_4in1_info->sns_mode = sns_4in1_mode;

    return rtn;
}

static cmr_int ov32d40_drv_4in1_init(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;

    SENSOR_LOGV("E");

    return rtn;
}

static cmr_int ov32d40_drv_4in1_process(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;

    SENSOR_LOGV("E");

    return rtn;
}

static cmr_int ov32d40_drv_4in1_deinit(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;

    SENSOR_LOGV("E");

    return rtn;
}
#endif

#if defined(FACTORY_BUILD)
static cmr_int ov32d40_drv_get_temperature(cmr_handle handle, cmr_uint param) {
    pthread_mutex_lock(&ov32d40_sensor_mutex);
    SENSOR_LOGI("E");
    unsigned char value = 0;
    char value_c[32];
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4D12, 0x01);
    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x4D13);
    // value[1] = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x4D14);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4D12, 0x00);
    snprintf(value_c, sizeof(value_c),  "sensor_temp=%d",value);
    SENSOR_LOGI("%s", value_c);
    property_set("vendor.debug.sensor.temperature", value_c);

    SENSOR_LOGI("X");
    pthread_mutex_unlock(&ov32d40_sensor_mutex);
    return SENSOR_SUCCESS;
}
#endif

cmr_int ov32d40_drv_pdaf_data_process(void *buffer_handle) {
    if(!buffer_handle)
        return SENSOR_FAIL;
    cmr_u32 sensor_mode = 0;
    ov32d40_sns_drv_cxt->ops_cb.get_mode(ov32d40_sns_drv_cxt->caller_handle, &sensor_mode);
    if(!ov32d40_pd_sns_mode[sensor_mode]) {
        SENSOR_LOGE("sensor mode %d pdaf type2 unsupported!", sensor_mode);
        return SENSOR_FAIL;
    }
    sensor_pdaf_format_convertor(buffer_handle, s_ov32d40_static_info[0].static_info.pdaf_supported,
                                 (cmr_u32 *)(&ov32d40_pdaf_info_array[sensor_mode]));
    return SENSOR_SUCCESS;
}
/*==============================================================================
 * Description:
 * cfg otp setting
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int ov32d40_drv_access_val(cmr_handle handle, cmr_uint param) {
    cmr_int ret = SENSOR_FAIL;
    SENSOR_VAL_T *param_ptr = (SENSOR_VAL_T *)param;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param_ptr);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("sensor ov32d40: param_ptr->type=%x", param_ptr->type);

#if defined(FACTORY_BUILD)
    ov32d40_drv_get_temperature(handle,param);
#endif

    switch (param_ptr->type) {
    case SENSOR_VAL_TYPE_GET_STATIC_INFO:
        ret = ov32d40_drv_get_static_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_FPS_INFO:
        ret = ov32d40_drv_get_fps_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_PDAF_INFO:
        ret = ov32d40_drv_get_pdaf_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_SET_SENSOR_CLOSE_FLAG:
        ret = sns_drv_cxt->is_sensor_close = 1;
        break;
#if SUPPORT_4IN1
    case SENSOR_VAL_TYPE_GET_4IN1_INFO:
        ret = ov32d40_drv_get_4in1_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_INIT:
        ret = ov32d40_drv_4in1_init(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_PROC:
        ret = ov32d40_drv_4in1_process(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_DEINIT:
        ret = ov32d40_drv_4in1_deinit(handle, param_ptr->pval);
        break;
#endif
    default:
        break;
    }
    ret = SENSOR_SUCCESS;

    return ret;
}

#define OTP_OFFSET 0x0000
#define OTP_SIZE 2
#define EEPROM_I2C_ADDR_ST 0xa0
#define ST_VENDOR_ID 0x0a
cmr_u8 otp_data_buf[OTP_SIZE]= {0x00};
static cmr_int get_vendor_id(cmr_handle handle) {
    cmr_int rtn = SENSOR_SUCCESS;
    otp_data_buf[0] = (OTP_OFFSET & 0xff00 ) >> 8;
    otp_data_buf[1] = OTP_OFFSET & 0x00ff;

    rtn = hw_sensor_read_i2c(handle, EEPROM_I2C_ADDR_ST >> 1, otp_data_buf,
                    OTP_SIZE << 16 | SENSOR_I2C_REG_16BIT);
    SENSOR_LOGI("otp_data_buf[0] = 0x%x,otp_data_buf[1] =0x%x\n", otp_data_buf[0],otp_data_buf[1]);

    if (rtn != HW_SUCCESS) {
        SENSOR_LOGE("read otp data fail return %d", rtn);
    return SENSOR_FAIL;
    }
    return rtn;
}

/*==============================================================================
 * Description:
 * identify sensor id
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int ov32d40_drv_identify(cmr_handle handle, cmr_uint param) {
    cmr_u16 pid_value = 0x00;
    cmr_u16 ver_value = 0x00;
    cmr_int ret_value = SENSOR_FAIL;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("mipi raw identify");

    get_vendor_id(sns_drv_cxt->hw_handle);
    if (otp_data_buf[1] == ST_VENDOR_ID) {
        SENSOR_LOGI("This is st module c3z03_ov32d40_backst");
    } else {
        return SENSOR_FAIL;
    }

    pid_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, ov32d40_PID_ADDR);

    if (ov32d40_PID_VALUE == pid_value) {
        ver_value =
            hw_sensor_read_reg(sns_drv_cxt->hw_handle, ov32d40_VER_ADDR);
        SENSOR_LOGI("Identify: pid_value = %x, ver_value = %x", pid_value,
                    ver_value);
        if (ov32d40_VER_VALUE == ver_value) {
            SENSOR_LOGI("this is ov32d40 sensor");
            ret_value = SENSOR_SUCCESS;
        } else {
            SENSOR_LOGI("sensor identify fail, pid_value = %x, ver_value = %x",
                        pid_value, ver_value);
        }
    } else {
        SENSOR_LOGI("sensor identify fail, pid_value = %x, ver_value = %x",
                    pid_value, ver_value);
    }

    return ret_value;
}

/*==============================================================================
 * Description:
 * before snapshot
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int ov32d40_drv_before_snapshot(cmr_handle handle, cmr_uint param) {
    cmr_u32 cap_shutter = 0;
    cmr_u32 prv_shutter = 0;
    cmr_u32 prv_gain = 0;
    cmr_u32 cap_gain = 0;
    cmr_u32 capture_mode = param & 0xffff;
    cmr_u32 preview_mode = (param >> 0x10) & 0xffff;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    cmr_u32 prv_linetime = sns_drv_cxt->trim_tab_info[preview_mode].line_time;
    cmr_u32 cap_linetime = sns_drv_cxt->trim_tab_info[capture_mode].line_time;

    SENSOR_LOGI("preview_mode=%d,capture_mode = %d", preview_mode,
                capture_mode);
    SENSOR_LOGI("preview_shutter = 0x%x, preview_gain = 0x%x",
                sns_drv_cxt->sensor_ev_info.preview_shutter,
                (unsigned int)sns_drv_cxt->sensor_ev_info.preview_gain);

    if (preview_mode == capture_mode) {
        cap_shutter = sns_drv_cxt->sensor_ev_info.preview_shutter;
        cap_gain = sns_drv_cxt->sensor_ev_info.preview_gain;
        goto snapshot_info;
    }

    prv_shutter = sns_drv_cxt->sensor_ev_info.preview_shutter;
    prv_gain = sns_drv_cxt->sensor_ev_info.preview_gain;

    if (sns_drv_cxt->ops_cb.set_mode)
        sns_drv_cxt->ops_cb.set_mode(sns_drv_cxt->caller_handle, capture_mode);
    if (sns_drv_cxt->ops_cb.set_mode_wait_done)
        sns_drv_cxt->ops_cb.set_mode_wait_done(sns_drv_cxt->caller_handle);

    cap_shutter = prv_shutter * prv_linetime / cap_linetime * BINNING_FACTOR;
    cap_gain = prv_gain;

    SENSOR_LOGI("capture_shutter = 0x%x, capture_gain = 0x%x", cap_shutter,
                cap_gain);

    ov32d40_drv_calc_exposure(handle, cap_shutter, 0, capture_mode,
                              &ov32d40_aec_info);
    ov32d40_drv_write_reg2sensor(handle, ov32d40_aec_info.frame_length);
    ov32d40_drv_write_reg2sensor(handle, ov32d40_aec_info.shutter);

    sns_drv_cxt->sensor_ev_info.preview_gain = cap_gain;
    ov32d40_drv_write_gain(handle, &ov32d40_aec_info, cap_gain);
    ov32d40_drv_write_reg2sensor(handle, ov32d40_aec_info.again);
    ov32d40_drv_write_reg2sensor(handle, ov32d40_aec_info.dgain);

snapshot_info:
    if (!sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle, SENSOR_EXIF_CTRL_EXPOSURETIME, cap_shutter);
    } else {
        sns_drv_cxt->exif_info.exposure_line = cap_shutter;
    }

    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * get the shutter from isp and write senosr shutter register
 * please don't change this function unless it's necessary
 *============================================================================*/
static cmr_int ov32d40_drv_write_exposure(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 size_index = 0x00;

    struct sensor_ex_exposure *ex = (struct sensor_ex_exposure *)param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_HANDLE(ex);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    exposure_line = ex->exposure;
    dummy_line = ex->dummy;
    size_index = ex->size_index;

    ov32d40_drv_calc_exposure(handle, exposure_line, dummy_line, size_index,
                              &ov32d40_aec_info);
    ov32d40_drv_write_reg2sensor(handle, ov32d40_aec_info.frame_length);
    ov32d40_drv_write_reg2sensor(handle, ov32d40_aec_info.shutter);

    return ret_value;
}

/*==============================================================================
 * Description:
 * write gain value to sensor
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int ov32d40_drv_write_gain_value(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;

    pthread_mutex_lock(&ov32d40_sensor_mutex);
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    ov32d40_drv_calc_gain(handle, param, &ov32d40_aec_info);
    ov32d40_drv_write_reg2sensor(handle, ov32d40_aec_info.again);
    ov32d40_drv_write_reg2sensor(handle, ov32d40_aec_info.dgain);
    pthread_mutex_unlock(&ov32d40_sensor_mutex);

    return ret_value;
}

/*==============================================================================
 * Description:
 * read ae control info
 * please don't change this function unless it's necessary
 *============================================================================*/
static cmr_int ov32d40_drv_read_aec_info(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;
    struct sensor_aec_reg_info *info = (struct sensor_aec_reg_info *)param;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 mode = 0x00;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(info);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    info->aec_i2c_info_out = &ov32d40_aec_info;
    exposure_line = info->exp.exposure;
    dummy_line = info->exp.dummy;
    mode = info->exp.size_index;

    ov32d40_drv_calc_exposure(handle, exposure_line, dummy_line, mode,
                              &ov32d40_aec_info);
    ov32d40_drv_calc_gain(handle, info->gain, &ov32d40_aec_info);

    return ret_value;
}

static cmr_int ov32d40_drv_set_master_FrameSync(cmr_handle handle,
                                                cmr_uint param) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("E");

    /* 0x381a/0x381b (row timing) : Time to send Fsync.
        calc method: 2*VTS -16. If needed, can be fine-tuned. Current settings
       are based on 20fps*/



    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * mipi stream on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int ov32d40_drv_stream_on(cmr_handle handle, cmr_uint param) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    pthread_mutex_lock(&ov32d40_sensor_mutex);
    SENSOR_LOGI("E");

    char value1[PROPERTY_VALUE_MAX];
    property_get("persist.vendor.cam.colorbar", value1, "0");
    if (!strcmp(value1, "1")) {
		// sensor output colorbar
		// sensor output normal: 
		// 0x5000,0x35 
		// 0x5101,0x00
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x5000, 0x37);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x5101, 0x01);
    }

    char value2[PROPERTY_VALUE_MAX];
    property_get("vendor.cam.hw.framesync.on", value2, "1");
    if (!strcmp(value2, "1")) {
#if defined(CONFIG_DUAL_MODULE)
       //ov32d40_drv_set_master_FrameSync(handle, param);
#endif
    }

    /*TODO*/

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3716, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3717, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3729, 0x7c);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x01);

    /*END*/

	/*delay*/
    // usleep(5*1000);
    pthread_mutex_unlock(&ov32d40_sensor_mutex);

    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * mipi stream off
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int ov32d40_drv_stream_off(cmr_handle handle, cmr_uint param) {
    pthread_mutex_lock(&ov32d40_sensor_mutex);
    SENSOR_LOGI("E");
    unsigned char value = 0;
    cmr_u16 sleep_time = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0100);
    if (value != 0x00) {
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3716, 0x08);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3717, 0x20);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3729, 0x7e);
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x00);
        if (!sns_drv_cxt->is_sensor_close) {
            sleep_time = (sns_drv_cxt->sensor_ev_info.preview_framelength *
                        sns_drv_cxt->line_time_def / 1000000) + 10;
            usleep(sleep_time * 1000);
            SENSOR_LOGI("stream_off delay_ms %d", sleep_time);
        }
    } else {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3716, 0x08);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3717, 0x20);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3729, 0x7e);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x00);
    }
    sns_drv_cxt->is_sensor_close = 0;

    usleep(22*1000);
    SENSOR_LOGI("X");
    pthread_mutex_unlock(&ov32d40_sensor_mutex);
    return SENSOR_SUCCESS;
}

static cmr_int
ov32d40_drv_handle_create(struct sensor_ic_drv_init_para *init_param,
                          cmr_handle *sns_ic_drv_handle) {
    cmr_int ret = SENSOR_SUCCESS;
    struct sensor_ic_drv_cxt *sns_drv_cxt = NULL;
    void *pri_data = NULL;

    ret = sensor_ic_drv_create(init_param, sns_ic_drv_handle);
    sns_drv_cxt = *sns_ic_drv_handle;
	ov32d40_sns_drv_cxt = *sns_ic_drv_handle;

    sns_drv_cxt->sensor_ev_info.preview_shutter =
        PREVIEW_FRAME_LENGTH - FRAME_OFFSET;
    sns_drv_cxt->sensor_ev_info.preview_gain = SENSOR_BASE_GAIN;
    sns_drv_cxt->sensor_ev_info.preview_framelength = PREVIEW_FRAME_LENGTH;

    sns_drv_cxt->frame_length_def = PREVIEW_FRAME_LENGTH;
    sns_drv_cxt->line_time_def = PREVIEW_LINE_TIME;

    ov32d40_drv_write_frame_length(
        sns_drv_cxt, &ov32d40_aec_info,
        sns_drv_cxt->sensor_ev_info.preview_framelength);
    ov32d40_drv_write_gain(sns_drv_cxt, &ov32d40_aec_info,
                           sns_drv_cxt->sensor_ev_info.preview_gain);
    ov32d40_drv_write_shutter(sns_drv_cxt, &ov32d40_aec_info,
                              sns_drv_cxt->sensor_ev_info.preview_shutter);

    sensor_ic_set_match_module_info(sns_drv_cxt,
                                    ARRAY_SIZE(s_ov32d40_module_info_tab),
                                    s_ov32d40_module_info_tab);
    sensor_ic_set_match_resolution_info(
        sns_drv_cxt, ARRAY_SIZE(s_ov32d40_resolution_tab_raw),
        s_ov32d40_resolution_tab_raw);
    sensor_ic_set_match_trim_info(sns_drv_cxt,
                                  ARRAY_SIZE(s_ov32d40_resolution_trim_tab),
                                  s_ov32d40_resolution_trim_tab);
    sensor_ic_set_match_static_info(
        sns_drv_cxt, ARRAY_SIZE(s_ov32d40_static_info), s_ov32d40_static_info);
    sensor_ic_set_match_fps_info(sns_drv_cxt,
                                 ARRAY_SIZE(s_ov32d40_mode_fps_info),
                                 s_ov32d40_mode_fps_info);

    /*init exif info,this will be deleted in the future*/
    ov32d40_drv_init_fps_info(sns_drv_cxt);
    pthread_mutex_init(&ov32d40_sensor_mutex, NULL);

    /*add private here*/
    return ret;
}

static cmr_int ov32d40_drv_handle_delete(cmr_handle handle, void *param) {
    cmr_int ret = SENSOR_SUCCESS;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    pthread_mutex_destroy(&ov32d40_sensor_mutex);
    ret = sensor_ic_drv_delete(handle, param);
    return ret;
}
void *sensor_ic_open_lib(void)
{
     return &g_ov32d40_mipi_raw_info;
}

static cmr_int ov32d40_drv_get_private_data(cmr_handle handle, cmr_uint cmd,
                                            void **param) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);

    ret = sensor_ic_get_private_data(handle, cmd, param);
    return ret;
}

/*==============================================================================
 * Description:
 * all ioctl functoins
 * you can add functions reference SENSOR_IOCTL_FUNC_TAB_T from sensor_drv_u.h
 *
 * add ioctl functions like this:
 * .power = ov32d40_power_on,
 *============================================================================*/
static struct sensor_ic_ops s_ov32d40_ops_tab = {
    .create_handle = ov32d40_drv_handle_create,
    .delete_handle = ov32d40_drv_handle_delete,
    .get_data = ov32d40_drv_get_private_data,
    /*---------------------------------------*/
    .power = ov32d40_drv_power_on,
    .identify = ov32d40_drv_identify,
    .ex_write_exp = ov32d40_drv_write_exposure,
    .write_gain_value = ov32d40_drv_write_gain_value,

//#if defined(CONFIG_DUAL_MODULE)
    .read_aec_info = ov32d40_drv_read_aec_info,
//#endif

    .ext_ops = {
            [SENSOR_IOCTL_BEFORE_SNAPSHOT].ops = ov32d40_drv_before_snapshot,
            [SENSOR_IOCTL_STREAM_ON].ops = ov32d40_drv_stream_on,
            [SENSOR_IOCTL_STREAM_OFF].ops = ov32d40_drv_stream_off,
            /* expand interface,if you want to add your sub cmd ,
             *  you can add it in enum {@SENSOR_IOCTL_VAL_TYPE}
             */
            [SENSOR_IOCTL_ACCESS_VAL].ops = ov32d40_drv_access_val,
    }
};
