/*
 * SPDX-FileCopyrightText: 2023 Unisoc (Shanghai) Technologies Co., Ltd
 * SPDX-License-Identifier: LicenseRef-Unisoc-General-1.0
 *
 * Copyright 2023 Unisoc (Shanghai) Technologies Co., Ltd.
 * Licensed under the Unisoc General Software License, version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * https://www.unisoc.com/en_us/license/UNISOC_GENERAL_LICENSE_V1.0-EN_US
 * Software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 * See the Unisoc General Software License, version 1.0 for more details.
 */
 /*
 * V1.0
 */
/*History
*Date                  Modification                                 Reason
*
*/

#define LOG_TAG "s5kjn1"

#include "sensor_s5kjn1_mipi_raw.h"
/*add the func of the reading of the otp info in the OTP,ping.luan@transsion.com,2021-05-12*/
#include "sensor_s5kjn1otp.h"
/*read otp data one time,when first identify*/
static cmr_int otp_read_once = 0;
/*add end. ping.luan@transsion.com, 2021-05-12*/
static struct sensor_ic_drv_cxt *s5kjn1_sns_drv_cxt = NULL;

/*==============================================================================
 * Description:
 * write register value to sensor
 * please modify this function acording your spec
 *============================================================================*/

static void s5kjn1_drv_write_reg2sensor(cmr_handle handle,
                                        struct sensor_i2c_reg_tab *reg_info) {
    cmr_int i = 0;
    SENSOR_IC_CHECK_PTR_VOID(reg_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);

    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

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
static void s5kjn1_drv_write_gain(cmr_handle handle,
                                  struct sensor_aec_i2c_tag *aec_info,
                                  cmr_u32 gain) {
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if (aec_info->again->size) {
        /*TODO*/
        aec_info->again->settings[0].reg_value = gain;
        /*END*/
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
static void s5kjn1_drv_write_frame_length(cmr_handle handle,
                                          struct sensor_aec_i2c_tag *aec_info,
                                          cmr_u32 frame_len) {
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if (aec_info->frame_length->size) {
        /*TODO*/
        aec_info->frame_length->settings[0].reg_value = frame_len;
        /*END*/
    }
}

/*==============================================================================
 * Description:
 * write shutter to sensor registers buffer
 * please pay attention to the frame length
 * please modify this function acording your spec
 *============================================================================*/
static void s5kjn1_drv_write_shutter(cmr_handle handle,
                                     struct sensor_aec_i2c_tag *aec_info,
                                     cmr_u32 shutter) {
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if (aec_info->shutter->size) {
        /*TODO*/
        aec_info->shutter->settings[0].reg_value = shutter;
        /*END*/
    }
}

/*==============================================================================
 * Description:
 * write exposure to sensor registers and get current shutter
 * please pay attention to the frame length
 * please don't change this function if it's necessary
 *============================================================================*/
static void s5kjn1_drv_calc_exposure(cmr_handle handle, cmr_u32 shutter,
                                     cmr_u32 dummy_line, cmr_u16 mode, cmr_u32 exp_time,
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

    SENSOR_LOGI("fps = %f", fps);

    frame_interval = (cmr_u16)(
        ((shutter + dummy_line) * sns_drv_cxt->line_time_def) / 1000000);
    SENSOR_LOGI(
        "mode = %d, exposure_line = %d, dummy_line= %d, frame_interval= %d ms",
        mode, shutter, dummy_line, frame_interval);

    if (dest_fr_len != cur_fr_len) {
        sns_drv_cxt->sensor_ev_info.preview_framelength = dest_fr_len;
        s5kjn1_drv_write_frame_length(handle, aec_info, dest_fr_len);
    }

    sns_drv_cxt->sensor_ev_info.preview_exptime= exp_time;
    sns_drv_cxt->sensor_ev_info.preview_shutter = shutter;
    s5kjn1_drv_write_shutter(handle, aec_info, shutter);

    if (sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->ops_cb.set_exif_info(
            sns_drv_cxt->caller_handle, SENSOR_EXIF_CTRL_EXPOSURETIME_BYTIME, exp_time);
    }
}

static void s5kjn1_drv_calc_gain(cmr_handle handle, cmr_uint isp_gain,
                                 struct sensor_aec_i2c_tag *aec_info) {
    cmr_u32 sensor_gain = 0;
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    sensor_gain = isp_gain < ISP_BASE_GAIN ? ISP_BASE_GAIN : isp_gain;
    sensor_gain = sensor_gain * SENSOR_BASE_GAIN / ISP_BASE_GAIN;

    if (SENSOR_MAX_GAIN < sensor_gain)
        sensor_gain = SENSOR_MAX_GAIN;

    SENSOR_LOGI("isp_gain = 0x%x,sensor_gain=0x%x", (unsigned int)isp_gain,
                sensor_gain);

    sns_drv_cxt->sensor_ev_info.preview_gain = sensor_gain;
    s5kjn1_drv_write_gain(handle, aec_info, sensor_gain);
}

/*==============================================================================
 * Description:
 * sensor power on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5kjn1_drv_power_on(cmr_handle handle, cmr_uint power_on) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;

    SENSOR_AVDD_VAL_E dvdd_val = module_info->dvdd_val;
    SENSOR_AVDD_VAL_E avdd_val = module_info->avdd_val;
    SENSOR_AVDD_VAL_E iovdd_val = module_info->iovdd_val;
    BOOLEAN power_down = g_s5kjn1_mipi_raw_info.power_down_level;
    BOOLEAN reset_level = g_s5kjn1_mipi_raw_info.reset_pulse_level;

    if (SENSOR_TRUE == power_on) {
        hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);

        usleep(1 * 1000);
        hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, iovdd_val);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, dvdd_val);
        usleep(1 * 1000);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, avdd_val);
        usleep(1 * 1000);
        hw_sensor_power_down(sns_drv_cxt->hw_handle, !power_down);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, !reset_level);
        usleep(2 * 1000);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, EX_MCLK);
        usleep(2 * 1000);
    } else {
        usleep(1 * 1000);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
        usleep(1 * 1000);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        usleep(1*1000);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
    }

    SENSOR_LOGI("(1:on, 0:off): %lu", power_on);
    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * calculate fps for every sensor mode according to frame_line and line_time
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5kjn1_drv_init_fps_info(cmr_handle handle) {

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

static cmr_int s5kjn1_drv_get_static_info(cmr_handle handle, cmr_u32 *param) {

    cmr_u32 up = 0;
    cmr_u32 down = 0;
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ex_info *ex_info = (struct sensor_ex_info *)param;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;

    // make sure we have get max fps of all settings.
    if (!fps_info->is_init) {
        s5kjn1_drv_init_fps_info(handle);
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
    ex_info->name = (cmr_s8 *)g_s5kjn1_mipi_raw_info.name;
    ex_info->sensor_version_info =
        (cmr_s8 *)g_s5kjn1_mipi_raw_info.sensor_version_info;
    memcpy(&ex_info->fov_info, &static_info->fov_info,
           sizeof(static_info->fov_info));
    ex_info->pos_dis.up2hori = up;
    ex_info->pos_dis.hori2down = down;

    sensor_ic_print_static_info((cmr_s8 *)SENSOR_NAME, ex_info);

    return rtn;
}

static cmr_int s5kjn1_drv_get_fps_info(cmr_handle handle, cmr_u32 *param) {

    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    SENSOR_MODE_FPS_T *fps_info = (SENSOR_MODE_FPS_T *)param;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_fps_info *fps_data = sns_drv_cxt->fps_info;

    // make sure have inited fps of every sensor mode.
    if (!fps_data->is_init) {
        s5kjn1_drv_init_fps_info(handle);
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

/*==============================================================================
 * Description:
 * Get PDAF info for every sensor with SIN_MODULE or DUAL_MODULE
 * please modify this function acording your sensor spec && pdaf map
 *============================================================================*/


static const cmr_u16 s5kjn1_pd_is_right[] = {1, 0, 1, 0, 1, 0, 1, 0};

//x
static const cmr_u16 s5kjn1_pd_col[] = {
	2, 3, 0, 1, 4, 5, 6, 7
 };
//y
static const cmr_u16 s5kjn1_pd_row[] = {
	0, 0, 3, 3, 4, 4, 7, 7
};
static const struct pd_pos_info s5kjn1_pd_pos_l[] = {
    {3, 0}, {1, 3}, {5, 4}, {7, 7}
  };

static const struct pd_pos_info s5kjn1_pd_pos_r[] = {
    {2, 0}, {0, 3}, {4, 4}, {6, 7}
  };

static const cmr_u32 s5kjn1_pd_sns_mode[] = {0, 1, 0};

struct pdaf_coordinate_tab s5kjn1_pd_coordinate_table[] = {
    {.number = 2,
     .pos_info = {1, 0},
    },
    {.number = 2,
     .pos_info = {1, 0},
    },
    {.number = 2,
     .pos_info = {1, 0},
    },
    {.number = 2,
     .pos_info = {1, 0},
    }
};

struct pdaf_block_descriptor s5kjn1_pd_seprator_helper = {
    .block_width =1,
    .block_height = 8,
    .coordinate_tab = {0, 1, 0, 1, 0, 1, 0, 1},
    .line_width =1,
    .block_pattern = LINED_UP,
    .pd_line_coordinate = s5kjn1_pd_coordinate_table,
};

struct sensor_pdaf_info s5kjn1_pdaf_info_array[] = {
	/* pd setting for sensor mode0 */
	{0},
	/* pd setting for sensor mode1 */
	{
	.pd_is_right = (cmr_u16 *)s5kjn1_pd_is_right,
	.pd_pos_row = (cmr_u16 *)s5kjn1_pd_row,
	.pd_pos_col = (cmr_u16 *)s5kjn1_pd_col,
	.pd_pos_r = (struct pd_pos_info *)s5kjn1_pd_pos_r,
	.pd_pos_l = (struct pd_pos_info *)s5kjn1_pd_pos_l,
	.pd_pos_size = NUMBER_OF_ARRAY(s5kjn1_pd_pos_r),
	.pd_offset_x = 8,
	.pd_offset_y = 8,
	.pd_end_x = 4072,
	.pd_end_y = 3064,
	.pd_block_w = 0,
	.pd_block_h = 0,
	.pd_block_num_x = 508,
	.pd_block_num_y = 382,
	.pd_density_x = 8,
	.pd_density_y = 2,
	.pd_pitch_x = 508,
	.pd_pitch_y = 382,
	.vch2_info.bypass = 0,
	.vch2_info.vch2_vc = 0,
	.vch2_info.vch2_data_type = 0x30,
	.vch2_info.vch2_mode = 0x01,
	.sns_mode = s5kjn1_pd_sns_mode,
	.descriptor = &s5kjn1_pd_seprator_helper,
	.sns_orientation = 0, /*1: mirror+flip; 0: normal*/
    .pd_size_w = PREVIEW_WIDTH,
	.pd_size_h = PREVIEW_HEIGHT,
	.image_process_type = 0, // 0:default; 1:only crop; 2:binning+crop;
	.crop_offset_x = 0,
	.crop_offset_y = 0,
	},
	/* pd setting for sensor mode2 */
    {0},
    /* pd setting for sensor mode3 */
    
	
	/* add more pdaf setting here */
	
};

static struct sensor_pdaf_info_tab s5kjn1_pdaf_info_tab = {
    .sns_mode_num = NUMBER_OF_ARRAY(s5kjn1_pd_sns_mode),
	.pd_sns_mode = s5kjn1_pd_sns_mode,
	.pdaf_init_index = 1,
	.pd_init_size_w = PREVIEW_WIDTH,
	.pd_init_size_h = PREVIEW_HEIGHT,
	.pd_info_tab = s5kjn1_pdaf_info_array,
};

cmr_int s5kjn1_drv_pdaf_data_process(void *buffer_handle);

static cmr_int s5kjn1_drv_get_pdaf_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_pdaf_info_tab *pdaf_info_tab = NULL;
    struct sensor_pdaf_info *pdaf_info = NULL;
    cmr_int pd_info_num = 0;
    int i = 0;
    SENSOR_IC_CHECK_PTR(param);

    SENSOR_LOGV("E");

    pdaf_info_tab = (struct sensor_pdaf_info_tab *)param;
    
    pdaf_info_tab->sns_mode_num = s5kjn1_pdaf_info_tab.sns_mode_num;
    pdaf_info_tab->pd_sns_mode = s5kjn1_pdaf_info_tab.pd_sns_mode;
    pdaf_info_tab->pdaf_init_index = s5kjn1_pdaf_info_tab.pdaf_init_index;
    pdaf_info_tab->pd_init_size_w = s5kjn1_pdaf_info_tab.pd_init_size_w;
    pdaf_info_tab->pd_init_size_h = s5kjn1_pdaf_info_tab.pd_init_size_h;
    pdaf_info_tab->pd_info_tab = s5kjn1_pdaf_info_tab.pd_info_tab;

    pd_info_num = s5kjn1_pdaf_info_tab.sns_mode_num;
    for(i = 0; i < pd_info_num; i++) {
        if(pdaf_info_tab->pd_sns_mode[i]){
            pdaf_info = &(pdaf_info_tab->pd_info_tab[i]);
            pdaf_info->pdaf_format_converter = s5kjn1_drv_pdaf_data_process;
            pdaf_info->pd_data_size = pdaf_info->pd_block_num_x * pdaf_info->pd_block_num_y *
                                      pdaf_info->pd_pos_size * 2 * 5;
            SENSOR_LOGD("pd data size:%d", pdaf_info->pd_data_size);
        }
    }
    SENSOR_LOGE("sensor pdaf_init_index %d", pdaf_info_tab->pdaf_init_index);
    return rtn;
}

cmr_int s5kjn1_drv_pdaf_data_process(void *buffer_handle) {
    if(!buffer_handle)
        return SENSOR_FAIL;
    cmr_u32 sensor_mode = 0;
    s5kjn1_sns_drv_cxt->ops_cb.get_mode(s5kjn1_sns_drv_cxt->caller_handle, &sensor_mode);
    if(!s5kjn1_pd_sns_mode[sensor_mode]) {
        SENSOR_LOGE("sensor mode %d pdaf type2 unsupported!", sensor_mode);
        return SENSOR_FAIL;
    }
    sensor_pdaf_format_convertor(buffer_handle, s_s5kjn1_static_info[0].static_info.pdaf_supported,
                                 (cmr_u32 *)(&s5kjn1_pdaf_info_array[sensor_mode]));
    return SENSOR_SUCCESS;
}
/*==============================================================================
 * Description:
 * add the interface of remosaic
 * please modify this function acording your sensor spec && remosaic guide
 *============================================================================*/
static const cmr_u32 sns_4in1_mode[] = {0, 0, 1};

static cmr_int s5kjn1_drv_get_4in1_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_4in1_info *sn_4in1_info = NULL;
    SENSOR_IC_CHECK_PTR(param);

    SENSOR_LOGI("E\n");

    sn_4in1_info = (struct sensor_4in1_info *)param;
    sn_4in1_info->is_4in1_supported = 1;
    sn_4in1_info->limited_4in1_width = PREVIEW_WIDTH;
    sn_4in1_info->limited_4in1_height = PREVIEW_HEIGHT;
    sn_4in1_info->sns_mode = sns_4in1_mode;
	sn_4in1_info->input_format = DATA_RAW10;//for remosaic input
	sn_4in1_info->output_format = DATA_BYTE2;//for remosaic output

    return rtn;
}


#include "sprd_fcell_ss.h"
#include "dlfcn.h"
#include <fcntl.h>
#include <pthread.h>
struct mThreadParam
{
    uint8_t *pIn;
    uint8_t *pOut;
    uint32_t count;
};
#define IMG_WIDTH    SNAPSHOT_WIDTH
#define IMG_HEIGHT   SNAPSHOT_HEIGHT
#define XTALK_BLC    64

void *handlelib;
uint16 *pOutImage; // for mipi_raw to raw16

#define XTC_TOTAL_LEN 8006

static cmr_int s5kjn1_drv_4in1_init(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;

    int ret = RET_OK;
    int imgsize = IMG_WIDTH * IMG_HEIGHT * 2;
    ssfcell_init init;
    
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
#if 1
    // Step 1: Init Fcell Library
	  init.xtalk = &xtc_data_buf[1];
	
	init.width = IMG_WIDTH;
    init.height = IMG_HEIGHT;
	init.xtalk_len = XTC_TOTAL_LEN;
    init.pedestal = XTALK_BLC;
	init.bayer_order = BAYER_GBRG;
#else
    struct otp_parser_section_with_version xtalk_data;
    struct otp_parser_section_with_version dpc_data;
    void *otp_parser_handle = NULL;

	cmr_int (*otp_parser_ptr)(
			void *raw_data, enum otp_parser_cmd cmd, cmr_uint eeprom_num,
			cmr_int camera_id, cmr_int raw_height, cmr_int raw_width, void *result);
	
		otp_parser_handle = dlopen("libcam_otp_parser.so", RTLD_NOW);
		if (otp_parser_handle == NULL) {
			char const *err_str = dlerror();
			SENSOR_LOGE("%s", err_str ? err_str : "unknown");
			return SENSOR_FAIL;
		} else {
			SENSOR_LOGV("dlopen libcam_otp_parser.so success");
		}
	
		otp_parser_ptr = dlsym(otp_parser_handle, "otp_parser");
		if (otp_parser_ptr == NULL) {
			dlclose(otp_parser_handle);
			SENSOR_LOGE(
				"load libcam_otp_parser.so: couldn't find symbol otp_parser");
			return SENSOR_FAIL;
		} else {
			SENSOR_LOGV("link libcam_otp_parser.so symbol success");
		}
	
		rtn = otp_parser_ptr(param_ptr, OTP_PARSER_SECTION_XTALK_4IN1,
							 OTP_EEPROM_SINGLE, sns_drv_cxt->sensor_id, IMG_HEIGHT,
							 IMG_WIDTH, &xtalk_data);
		if (rtn) {
			SENSOR_LOGE("OTP_PARSER_SECTION_XTALK_4IN1 fail");
			dlclose(otp_parser_handle);
			return SENSOR_FAIL;
		} else {
		    SENSOR_LOGE("OTP_PARSER_SECTION_XTALK_4IN1 success");

		}
		
		init.xtalk = xtalk_data.data_addr;		
		init.xtalk_len = xtalk_data.data_size;	
		init.pedestal = XTALK_BLC;
		init.bayer_order = BAYER_GBRG;
		
		rtn = otp_parser_ptr(param_ptr, OTP_PARSER_SECTION_DPC_4IN1,
							 OTP_EEPROM_SINGLE, sns_drv_cxt->sensor_id, IMG_HEIGHT,
							 IMG_WIDTH, &dpc_data);
		if (rtn) {			
			SENSOR_LOGE("OTP_PARSER_SECTION_DPC_4IN1 fail");
			dlclose(otp_parser_handle);
			return SENSOR_FAIL;
		} else {
		    SENSOR_LOGE("OTP_PARSER_SECTION_DPC_4IN1 success");
		}
	    dlclose(otp_parser_handle);

#endif
    pOutImage = malloc(imgsize);

    handlelib = dlopen("libsprd_fcell_ss.so", RTLD_NOW);
    if (handlelib == NULL) {
        char const *err_str = dlerror();
        SENSOR_LOGE("dlopen error %s", err_str ? err_str : "unknown");
    }

    /* Get the address of the struct hal_module_info. */
    const char *sym = "ss4c_init";
    typedef int (*func_init)(ssfcell_init); // unsigned char *, unsigned char
                                            // *);
    func_init init_ss4c = (int *)dlsym(handlelib, sym);
    if (init_ss4c == NULL) {
        SENSOR_LOGE("load: couldn't find symbol %s", sym);
        return SENSOR_FAIL;
    } else {
        SENSOR_LOGI("link symbol success");
    }
    init_ss4c(init); //(unsigned char *)param, (unsigned char *)param);

    return rtn;
}


void *raw_process1(void *data) {
     struct mThreadParam *param = (struct mThreadParam *)data;
     uint32_t count = param->count;
     uint8_t *pIn = param->pIn;
     uint8_t *pOut = param->pOut;
     for (uint32_t i = 0; i < count; i = i+4) {
         *pOut = *pIn >> 2 | ((*(pIn+1)& 0b00000011) << 6);
         *(pOut+1) = *(pIn+2) >> 2 | ((*(pIn+3)& 0b00000011) << 6);
         *(pOut+2) = *(pIn+4) >> 2 | ((*(pIn+5)& 0b00000011) << 6);
         *(pOut+3) = *(pIn+6) >> 2 | ((*(pIn+7)& 0b00000011) << 6);
         *(pOut+4) = ((*(pIn+6)& 0b00000011) <<6) |((*(pIn+4)& 0b00000011) <<4) |((*(pIn+2)& 0b00000011) <<2) |(*(pIn)& 0b00000011) ;
         pIn = pIn + 8;
         pOut = pOut+5;
         // if need 16 byte
         //if(!((i + 4) % IMG_WIDTH))
         //pOut = pOut+8;       
      }
    return NULL;
}


int ss4c_raw_process1(unsigned short *pFcellImage, unsigned short *pOutImage, uint32_t input_width, uint32_t input_height){
  
///////////call raw_process example///////////
    struct mThreadParam iparams[4];
    pthread_t ths[4];
    uint32_t aFrameLen10 = ((input_width * 5 / 4) * input_height)/4;// ((input_width * 5 / 4 + 8) * input_height)/4;//if need 16 byte
    uint32_t aFrameLen16 = (input_width*input_height)/4;
  
    unsigned char *m_pInputBuffer = (unsigned char *)pFcellImage ;//(unsigned char *)pFcellImage;//src;
    uint16_t *inputRaw16 = (uint16 *)pOutImage;//malloc(input_width*input_height);
    for(int i=0; i<4; i++) {
        iparams[i].count = aFrameLen16;
        iparams[i].pOut = (uint8_t *)(m_pInputBuffer+aFrameLen10*i);
        iparams[i].pIn = (uint8_t *)(inputRaw16+aFrameLen16*i);
        pthread_create(&ths[i], NULL, raw_process1, (void *)&iparams[i]);
     }
    for(int i=0; i<4; i++) {
          pthread_join(ths[i], NULL);
     }
  
/*prcessed raw10 is : inputRaw16*/
     return 0;
}


static cmr_int s5kjn1_drv_4in1_process(cmr_handle handle, cmr_u32 *param) {
    //      #include "fcell.h"
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    struct frame_4in1_info *frame_4in1 = (struct frame_4in1_info *)param;
    cmr_uint imgsize = IMG_WIDTH * IMG_HEIGHT * 2;

    int32_t pFcellImagein_fd = frame_4in1->im_addr_in;//TODO need check fd?
    int32_t pFcellImageout_fd = frame_4in1->im_addr_out;
    uint16 * pFcellImage = (uint16 *)frame_4in1->im_addr_in;
    uint16 * pOutImage = (uint16 *)frame_4in1->im_addr_out;
//  uint16 *pOutImage;
//     pOutImage = malloc(imgsize);
#if 1
    SENSOR_LOGI("%s:enter\n", __func__);

    const char *sym = "ss4c_process";
    typedef int (*func_proc)(unsigned short * , unsigned short * , struct st_remosaic_param*, struct st_remosaic_gain* g_gain, uint32_t, uint32_t);
    func_proc proc_ss4c = (int *)dlsym(handlelib, sym);
    if (proc_ss4c == NULL) {
        SENSOR_LOGE("load: couldn't find symbol %s", sym);
        return SENSOR_FAIL;
    } else {
        SENSOR_LOGI("link symbol success");
    }
    struct st_remosaic_param p_param;
    p_param.wb_b_gain = frame_4in1->awb_gain.b_gain;
    p_param.wb_gb_gain = frame_4in1->awb_gain.g_gain;
    p_param.wb_gr_gain = frame_4in1->awb_gain.g_gain;
    p_param.wb_r_gain = frame_4in1->awb_gain.r_gain;

    cmr_u16 sensor_gain = 0x00;
    sensor_gain = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0204);
    SENSOR_LOGV("4IN1: sensor_gain = %x", sensor_gain);

    struct st_remosaic_gain g_gain;
    g_gain.total_gain = sensor_gain / SENSOR_BASE_GAIN;
    g_gain.analog_gain = sensor_gain / SENSOR_BASE_GAIN;
    g_gain.digital_gain = 1;
    SENSOR_LOGV("4IN1: sensor_again = %x", g_gain.total_gain);
    struct st_remosaic_gain remosaic_gain;
    remosaic_gain.analog_gain = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0204);
    remosaic_gain.digital_gain = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0210);
    remosaic_gain.total_gain = (remosaic_gain.analog_gain / 1024) * (remosaic_gain.digital_gain * 2 / 4096);
    SENSOR_LOGI("analog_gain: %d , digital_gain : %d , total_gain : %d", remosaic_gain.analog_gain, remosaic_gain.digital_gain, remosaic_gain.total_gain);

   // rtn = remosaic_process_param_set(&p_param);
    //rtn |= remosaic_process_gain_set(&remosaic_gain);
    if ( SENSOR_SUCCESS != rtn ){
        SENSOR_LOGE("failed , remosaic_process_param_set, return value is %d", rtn);
        rtn = SENSOR_FAIL;
    }
#if 1
    char value1[PROPERTY_VALUE_MAX];
    property_get("vendor.cam.sensor.dump", value1, "1");
    if (!strcmp(value1, "input")||!strcmp(value1, "4in1")) {
	    FILE *fp = fopen("/data/vendor/cameraserver/input.mipi_raw", "wb");
	    if (fp) {
	        fwrite(pFcellImage, imgsize*1.25/2, 1, fp);
	        fclose(fp);
	    }
    }
#endif

    SENSOR_LOGI("awb gain %x %x %x addr_in 0x%x out 0x%x", frame_4in1->awb_gain.r_gain,
		frame_4in1->awb_gain.b_gain, frame_4in1->awb_gain.g_gain,&pFcellImagein_fd,&pFcellImageout_fd);
    proc_ss4c(pFcellImage,pOutImage, &p_param, &remosaic_gain, IMG_WIDTH, IMG_HEIGHT);
//    proc_ss4c((unsigned short *)pFcellImage, (unsigned short *)pFcellImage, &p_param);
    SENSOR_LOGI("4in1_process ok %x %x", frame_4in1->im_addr_in_fd, pOutImage[1]);
#if 1
    property_get("vendor.cam.sensor.dump", value1, "1");
    if (!strcmp(value1, "output")||!strcmp(value1, "4in1")) {
	    FILE *fp = fopen("/data/vendor/cameraserver/output0.mipi_raw", "wb");
	    if (fp) {
	        fwrite(pOutImage, imgsize, 1, fp);
	        fclose(fp);
	    }
    }
#endif
   // ss4c_raw_process1(pFcellImage, pOutImage, IMG_WIDTH, IMG_HEIGHT);
    //frame_4in1->im_addr_out= frame_4in1->im_addr_in; //(cmr_int)(void *)pFcellImage;
#if 0
    property_get("vendor.cam.sensor.dump", value1, "1");
    if (!strcmp(value1, "output")||!strcmp(value1, "4in1")) {
	    FILE *fp = fopen("/data/vendor/cameraserver/output.mipi_raw", "wb");
	    if (fp) {
	        fwrite(pFcellImage, imgsize*5/8, 1, fp);
	        fclose(fp);
	    }
    }
#endif
//    frame_4in1->im_addr_in = (cmr_int)(void *)pFcellImage;//frame_4in1->im_addr_in;
#endif
    //    free(pOutImage);
    return rtn;
}
static cmr_int s5kjn1_drv_4in1_deinit(cmr_handle handle, cmr_u32 *param) {
    //      #include "fcell.h"
    cmr_int rtn = SENSOR_SUCCESS;
    // ov4c_release();
    /**
	void remosaic_deinit();

    */

    SENSOR_LOGE("%s:enter\n", __func__);

    const char *sym = "ss4c_release";
    typedef int (*func_release)();
    func_release release_ss4c = NULL;
	if (handlelib)
        release_ss4c = (int *)dlsym(handlelib, sym);
    if (release_ss4c == NULL) {
        SENSOR_LOGE("load: couldn't find symbol %s", sym);
		if (pOutImage) {
			free(pOutImage);
			pOutImage = NULL;
		}
        return SENSOR_FAIL;
    } else {
        SENSOR_LOGI("link symbol success");
    }
    release_ss4c();
    if (handlelib) {
        dlclose(handlelib);
        handlelib = NULL;
    }

    if (pOutImage) {
        free(pOutImage);
        pOutImage = NULL;
    }

    return rtn;
}


/*==============================================================================
 * Description:
 * cfg otp setting
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5kjn1_drv_access_val(cmr_handle handle, cmr_uint param) {

    cmr_int ret = SENSOR_FAIL;
    SENSOR_VAL_T *param_ptr = (SENSOR_VAL_T *)param;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param_ptr);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGV("sensor s5kjn1: param_ptr->type=%x", param_ptr->type);

    switch (param_ptr->type) {
    case SENSOR_VAL_TYPE_GET_STATIC_INFO:
        ret = s5kjn1_drv_get_static_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_FPS_INFO:
        ret = s5kjn1_drv_get_fps_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_SET_SENSOR_CLOSE_FLAG:
        ret = sns_drv_cxt->is_sensor_close = 1;
        break;
    case SENSOR_VAL_TYPE_GET_PDAF_INFO:
        ret = s5kjn1_drv_get_pdaf_info(handle, param_ptr->pval);
        break;
/*add the func of the reading of the awb info in the OTP,ping.luan@transsion.com,2021-05-12*/
#ifdef MIT_CAM_MODULE_INFO_TRANSSION
    case SENSOR_VAL_TYPE_GET_PART_OTP:
        ret = s5kjn1_readotp_awb_info(handle, param_ptr->pval);
        break;
#endif
/*add end. ping.luan@transsion.com, 2021-05-12*/
    case SENSOR_VAL_TYPE_GET_4IN1_INFO:
        ret = s5kjn1_drv_get_4in1_info(handle, param_ptr->pval);
        break;
	case SENSOR_VAL_TYPE_4IN1_INIT:
		ret = s5kjn1_drv_4in1_init(handle, param_ptr->pval);
		break;
	case SENSOR_VAL_TYPE_4IN1_PROC:
		ret = s5kjn1_drv_4in1_process(handle, param_ptr->pval);
		break;
	case SENSOR_VAL_TYPE_4IN1_DEINIT:
		ret = s5kjn1_drv_4in1_deinit(handle, param_ptr->pval);
		break;
    default:
        break;
    }
    ret = SENSOR_SUCCESS;

    return ret;
}

static cmr_u8 s5kjn1_snspid_is_init = 0;

static cmr_int s5kjn1_drv_save_snspid(cmr_handle handle) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u8 snspid[32] = {0};
    cmr_u16 snspid_size = 32; //sensor_drv_u.c sensor_snspid max-size is 32byte

    SENSOR_LOGI("E");

    if (sns_drv_cxt->ops_cb.set_snspid) {
        sns_drv_cxt->ops_cb.set_snspid(
            sns_drv_cxt->caller_handle, sns_drv_cxt->sensor_id, snspid, snspid_size);
    }

    s5kjn1_snspid_is_init = 1;
    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * identify sensor id
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5kjn1_drv_identify(cmr_handle handle, cmr_uint param) {

    cmr_u16 pid_value = 0x00;
    cmr_u16 ver_value = 0x00;
    cmr_int ret_value = SENSOR_FAIL;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("mipi raw identify");

    pid_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, s5kjn1_PID_ADDR);
    if (s5kjn1_PID_VALUE == pid_value) {
        ver_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, s5kjn1_VER_ADDR);
        SENSOR_LOGI("Identify: pid_value = %x, ver_value = %x", pid_value,
                    ver_value);
        if (s5kjn1_VER_VALUE == ver_value) {
            SENSOR_LOGI("this is s5kjn1 sensor");
/*add the func of the reading of the otp info in the OTP,ping.luan@transsion.com,2021-05-12*/
			SENSOR_LOGI("itelotp otp_read_once = %d",otp_read_once);
			if(otp_read_once == 0){
//[ITD] add GGC to sensor ,songcen.yao,2021-6-17,start
    s5kjn1_readotp_GGC_write(sns_drv_cxt->hw_handle);
//[ITD] add GGC to sensor ,songcen.yao,2021-6-17,end
				ret_value = s5kjn1_read_otp(sns_drv_cxt->hw_handle);
				if(ret_value != SENSOR_SUCCESS){
					SENSOR_LOGE("Error:s5kjn1_read_otp failed!!!");
				}

				otp_read_once++ ;
				SENSOR_LOGI("s5kjn1 otp_read_once= %d",otp_read_once);
			}
/*add end. ping.luan@transsion.com, 2021-05-12*/
            ret_value = SENSOR_SUCCESS;
        } else {
            SENSOR_LOGE("sensor identify fail, pid_value = %x, ver_value = %x",
                        pid_value, ver_value);
        }
    } else {
        SENSOR_LOGE("sensor identify fail, pid_value = %x, ver_value = %x",
                    pid_value, ver_value);
    }

    return ret_value;
}

/*==============================================================================
 * Description:
 * before snapshot
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int s5kjn1_drv_before_snapshot(cmr_handle handle, cmr_uint param) {

    cmr_u32 cap_shutter = 0;
    cmr_u32 cap_exptime = 0;
    cmr_u32 prv_shutter = 0;
    cmr_u32 prv_exptime = 0;
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
    SENSOR_LOGI("preview_shutter = %d, preview_exptime= %lld ns, preview_gain = %d",
                sns_drv_cxt->sensor_ev_info.preview_shutter,
                sns_drv_cxt->sensor_ev_info.preview_exptime,
                (unsigned int)sns_drv_cxt->sensor_ev_info.preview_gain);

    if (preview_mode == capture_mode) {
        cap_shutter = sns_drv_cxt->sensor_ev_info.preview_shutter;
        cap_exptime = sns_drv_cxt->sensor_ev_info.preview_exptime;
        cap_gain = sns_drv_cxt->sensor_ev_info.preview_gain;
        goto snapshot_info;
    }

    prv_shutter = sns_drv_cxt->sensor_ev_info.preview_shutter;
    prv_exptime = sns_drv_cxt->sensor_ev_info.preview_exptime;
    prv_gain = sns_drv_cxt->sensor_ev_info.preview_gain;

    if (sns_drv_cxt->ops_cb.set_mode)
        sns_drv_cxt->ops_cb.set_mode(sns_drv_cxt->caller_handle, capture_mode);
    if (sns_drv_cxt->ops_cb.set_mode_wait_done)
        sns_drv_cxt->ops_cb.set_mode_wait_done(sns_drv_cxt->caller_handle);

    cap_shutter = prv_shutter * prv_linetime / cap_linetime * BINNING_FACTOR;
    cap_exptime = prv_exptime;
    cap_gain = prv_gain;

    SENSOR_LOGI("capture_shutter = %d, capture_exptime = %d ns, capture_gain = %d",
		cap_shutter, cap_exptime, cap_gain);

    s5kjn1_drv_calc_exposure(handle, cap_shutter, 0, capture_mode, cap_exptime,
                             &s5kjn1_aec_info);
    s5kjn1_drv_write_reg2sensor(handle, s5kjn1_aec_info.frame_length);
    s5kjn1_drv_write_reg2sensor(handle, s5kjn1_aec_info.shutter);

    sns_drv_cxt->sensor_ev_info.preview_gain = cap_gain;
    s5kjn1_drv_write_gain(handle, &s5kjn1_aec_info, cap_gain);
    s5kjn1_drv_write_reg2sensor(handle, s5kjn1_aec_info.again);
    s5kjn1_drv_write_reg2sensor(handle, s5kjn1_aec_info.dgain);

snapshot_info:
    if (sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle,
                                          SENSOR_EXIF_CTRL_EXPOSURETIME_BYTIME,
                                          cap_exptime);
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
static cmr_int s5kjn1_drv_write_exposure(cmr_handle handle, cmr_uint param) {

    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 size_index = 0x00;
    cmr_u32 exp_time = 0x00;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ex_exposure *ex = (struct sensor_ex_exposure *)param;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    exposure_line = ex->exposure;
    dummy_line = ex->dummy;
    size_index = ex->size_index;
    exp_time = ex->exp_time;

    s5kjn1_drv_calc_exposure(handle, exposure_line, dummy_line, size_index, exp_time,
                             &s5kjn1_aec_info);
    s5kjn1_drv_write_reg2sensor(handle, s5kjn1_aec_info.frame_length);
    s5kjn1_drv_write_reg2sensor(handle, s5kjn1_aec_info.shutter);

    return ret_value;
}

/*==============================================================================
 * Description:
 * write gain value to sensor
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int s5kjn1_drv_write_gain_value(cmr_handle handle, cmr_uint param) {

    cmr_int ret_value = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    s5kjn1_drv_calc_gain(handle, param, &s5kjn1_aec_info);
    s5kjn1_drv_write_reg2sensor(handle, s5kjn1_aec_info.again);
    s5kjn1_drv_write_reg2sensor(handle, s5kjn1_aec_info.dgain);

    return ret_value;
}

/*==============================================================================
 * Description:
 * read ae control info
 * please don't change this function unless it's necessary
 *============================================================================*/
static cmr_int s5kjn1_drv_read_aec_info(cmr_handle handle, cmr_uint param) {

    cmr_int ret_value = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_aec_reg_info *info = (struct sensor_aec_reg_info *)param;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u32 exp_time = 0x00;
    cmr_u16 mode = 0x00;

    SENSOR_LOGI("E");

    info->aec_i2c_info_out = &s5kjn1_aec_info;
    exposure_line = info->exp.exposure;
    dummy_line = info->exp.dummy;
    mode = info->exp.size_index;
    exp_time = info->exp.exp_time;

    s5kjn1_drv_calc_exposure(handle, exposure_line, dummy_line, mode, exp_time,
                             &s5kjn1_aec_info);
    s5kjn1_drv_calc_gain(handle, info->gain, &s5kjn1_aec_info);

    return ret_value;
}
/*
static cmr_int s5kjn1_drv_set_master_FrameSync(cmr_handle handle,
                                               cmr_uint param) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("E");

    //TODO
    cmr_u16 val1 = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3c67);
    cmr_u16 val2 = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3c71);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3c67,
                        (0x10 << 8) | val1 & 0x00ff);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3c71,
                        (0x03 << 8) | val2 & 0x00ff);
    cmr_u16 val3 = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3c67);

    cmr_u16 val4 = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3c71);
    SENSOR_LOGI("val1:%04x val2:%04x  val3 :%04x val4:%04x ", val1, val2, val3,
                val4);
    //END

    return SENSOR_SUCCESS;
}
static cmr_int s5kjn1_drv_set_Slave_FrameSync(cmr_handle handle,
                                              cmr_uint param) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("E");

    //TODO
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3f00, 0x0500);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3c68, 0x0400);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3f05, 0x5A00);
    //END

    return SENSOR_SUCCESS;
}*/

/*==============================================================================
 * Description:
 * mipi stream on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5kjn1_drv_stream_on(cmr_handle handle, cmr_uint param) {

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    SENSOR_LOGI("E");

    char value1[PROPERTY_VALUE_MAX];
    property_get("vendor.cam.hw.framesync.on", value1, "1");
    if (!strcmp(value1, "1")) {
//#if defined(CONFIG_DUAL_MODULE)
//       s5kjn1_drv_set_master_FrameSync(handle, param);
//#endif
    }
    property_get("persist.vendor.cam.colorbar", value1, "0");
    if (!strcmp(value1, "1")) {
        SENSOR_LOGD("enable test mode");
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0600, 0x0002);
    }
    /*TODO*/
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0103);
    /*END*/

    SENSOR_LOGI("X");
    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * mipi stream off
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5kjn1_drv_stream_off(cmr_handle handle, cmr_uint param) {

    SENSOR_LOGI("E");
    cmr_u16 value = 0;
    cmr_u16 sleep_time = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0100);
    if (value != 0x0000) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0000);
        if (!sns_drv_cxt->is_sensor_close) {
            sleep_time = (sns_drv_cxt->sensor_ev_info.preview_framelength *
                          sns_drv_cxt->line_time_def / 1000000) +
                         10;
            usleep(sleep_time * 1000);
            SENSOR_LOGI("stream_off delay_ms %d", sleep_time);
        }
    } else {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0000);
    }
    sns_drv_cxt->is_sensor_close = 0;

    SENSOR_LOGI("X");
    return SENSOR_SUCCESS;
}

static cmr_int
s5kjn1_drv_handle_create(struct sensor_ic_drv_init_para *init_param,
                         cmr_handle *sns_ic_drv_handle) {

    cmr_int ret = SENSOR_SUCCESS;
    struct sensor_ic_drv_cxt *sns_drv_cxt = NULL;
    void *pri_data = NULL;

    ret = sensor_ic_drv_create(init_param, sns_ic_drv_handle);
    sns_drv_cxt = *sns_ic_drv_handle;
	s5kjn1_sns_drv_cxt = *sns_ic_drv_handle;

    sns_drv_cxt->sensor_ev_info.preview_shutter =
        PREVIEW_FRAME_LENGTH - FRAME_OFFSET;
    sns_drv_cxt->sensor_ev_info.preview_exptime =
        (PREVIEW_FRAME_LENGTH - FRAME_OFFSET) * PREVIEW_LINE_TIME;
    sns_drv_cxt->sensor_ev_info.preview_gain = SENSOR_BASE_GAIN;
    sns_drv_cxt->sensor_ev_info.preview_framelength = PREVIEW_FRAME_LENGTH;

    sns_drv_cxt->frame_length_def = PREVIEW_FRAME_LENGTH;
    sns_drv_cxt->line_time_def = PREVIEW_LINE_TIME;

    s5kjn1_drv_write_frame_length(
        sns_drv_cxt, &s5kjn1_aec_info,
        sns_drv_cxt->sensor_ev_info.preview_framelength);
    s5kjn1_drv_write_gain(sns_drv_cxt, &s5kjn1_aec_info,
                          sns_drv_cxt->sensor_ev_info.preview_gain);
    s5kjn1_drv_write_shutter(sns_drv_cxt, &s5kjn1_aec_info,
                             sns_drv_cxt->sensor_ev_info.preview_shutter);

    sensor_ic_set_match_module_info(sns_drv_cxt,
                                    ARRAY_SIZE(s_s5kjn1_module_info_tab),
                                    s_s5kjn1_module_info_tab);
    sensor_ic_set_match_resolution_info(sns_drv_cxt,
                                        ARRAY_SIZE(s_s5kjn1_resolution_tab_raw),
                                        s_s5kjn1_resolution_tab_raw);
    sensor_ic_set_match_trim_info(sns_drv_cxt,
                                  ARRAY_SIZE(s_s5kjn1_resolution_trim_tab),
                                  s_s5kjn1_resolution_trim_tab);
    sensor_ic_set_match_static_info(
        sns_drv_cxt, ARRAY_SIZE(s_s5kjn1_static_info), s_s5kjn1_static_info);
    sensor_ic_set_match_fps_info(sns_drv_cxt,
                                 ARRAY_SIZE(s_s5kjn1_mode_fps_info),
                                 s_s5kjn1_mode_fps_info);

    /*init exif info,this will be deleted in the future*/
    s5kjn1_drv_init_fps_info(sns_drv_cxt);

//    s5kjn1_drv_4in1_init(sns_drv_cxt, 0);

    /*add private here*/
    return ret;
}

static cmr_int s5kjn1_drv_handle_delete(cmr_handle handle, void *param) {

    cmr_int ret = SENSOR_SUCCESS;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

//    s5kjn1_drv_4in1_deinit(handle, param);

    ret = sensor_ic_drv_delete(handle, param);

    return ret;
}

static cmr_int s5kjn1_drv_get_private_data(cmr_handle handle, cmr_uint cmd,
                                           void **param) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);

    ret = sensor_ic_get_private_data(handle, cmd, param);

    return ret;
}

void *sensor_ic_open_lib(void)
{
     return &g_s5kjn1_mipi_raw_info;
}

/*==============================================================================
 * Description:
 * all ioctl functoins
 * you can add functions reference SENSOR_IOCTL_FUNC_TAB_T from sensor_drv_u.h
 *
 * add ioctl functions like this:
 * .power = s5kjn1_power_on,
 *============================================================================*/
static struct sensor_ic_ops s_s5kjn1_ops_tab = {
    .create_handle = s5kjn1_drv_handle_create,
    .delete_handle = s5kjn1_drv_handle_delete,
    /*get privage data*/
    .get_data = s5kjn1_drv_get_private_data,
    /*common interface*/
    .power = s5kjn1_drv_power_on,
    .identify = s5kjn1_drv_identify,
    .ex_write_exp = s5kjn1_drv_write_exposure,
    .write_gain_value = s5kjn1_drv_write_gain_value,

#if defined(CONFIG_DUAL_MODULE)
    .read_aec_info = s5kjn1_drv_read_aec_info,
#endif

    .ext_ops = {
            [SENSOR_IOCTL_BEFORE_SNAPSHOT].ops = s5kjn1_drv_before_snapshot,
            [SENSOR_IOCTL_STREAM_ON].ops = s5kjn1_drv_stream_on,
            [SENSOR_IOCTL_STREAM_OFF].ops = s5kjn1_drv_stream_off,
            /* expand interface,if you want to add your sub cmd ,
             *  you can add it in enum {@SENSOR_IOCTL_VAL_TYPE}
            */
            [SENSOR_IOCTL_ACCESS_VAL].ops = s5kjn1_drv_access_val,
    }};
