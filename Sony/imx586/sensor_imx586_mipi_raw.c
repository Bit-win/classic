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

//#define LOG_TAG "imx586_mipi_raw"
#ifdef _SENSOR_RAW_SHARKL5PRO_H_
#define IMX586_CPHY
#endif
#ifdef IMX586_CPHY
#include "sensor_imx586_mipi_raw_cphy.h"
#define LOG_TAG "imx586_cphy"
#else
#include "sensor_imx586_mipi_raw.h"
#define LOG_TAG "imx586_dphy"
#endif
/*==============================================================================
 * Description:
 * write register value to sensor
 * please modify this function acording your spec
 *============================================================================*/

static double imx586_longExp_mode[] = {0.5, 1, 2, 3, 4, 5, 6, 7, 8, 10, 13, 15, 20, 25, 30};
static cmr_u32 imx586_longExp_valid_frame_num = 2;
static cmr_u32 imx586_longExp_need_switch_setting = 0;
static cmr_u32 imx586_longExp_setting[] = {0, 0, 0, 0, 1, 0};
static cmr_u64 imx586_longExp_threshold = 80000000;
static struct sensor_ic_drv_cxt *imx586_sns_drv_cxt = NULL;

static void imx586_drv_write_reg2sensor(cmr_handle handle,
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
static void imx586_drv_write_gain(cmr_handle handle,
                                  struct sensor_aec_i2c_tag *aec_info,
                                  cmr_u32 gain) {
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if (aec_info->again->size) {
        /*TODO*/
        float temp_gain = (float)gain / 128;
        cmr_u16 sensor_again = 0;

        if (temp_gain < 1.13) {
            temp_gain = 1.13;
        } else if (temp_gain > 16.0) {
            temp_gain = 16.0;
        }

        sensor_again = (cmr_u16)(1024.0 - 1024.0 / temp_gain);
        SENSOR_LOGD("temp_gain %f, sensor_again 0x%x", temp_gain, sensor_again);
        aec_info->again->settings[0].reg_value = (sensor_again >> 8) & 0xff;
        aec_info->again->settings[1].reg_value = sensor_again & 0xff;

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
static void imx586_drv_write_frame_length(cmr_handle handle,
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
static void imx586_drv_write_shutter(cmr_handle handle,
                                     struct sensor_aec_i2c_tag *aec_info,
                                     cmr_u32 shutter, cmr_u16 shutter_shift) {
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if (aec_info->shutter->size) {

        aec_info->shutter->settings[0].reg_value = (shutter >> 8) & 0xff;
        aec_info->shutter->settings[1].reg_value = shutter & 0xff;
        aec_info->shutter->settings[2].reg_value = shutter_shift;

    }

}

/*==============================================================================
 * Description:
 * write exposure to sensor registers and get current shutter
 * please pay attention to the frame length
 * please don't change this function if it's necessary
 *============================================================================*/
static void imx586_drv_calc_exposure(cmr_handle handle, cmr_u32 shutter,
                                     cmr_u32 dummy_line, cmr_u16 mode,cmr_u64 exp_time,
                                     struct sensor_aec_i2c_tag *aec_info) {
    cmr_u32 dest_fr_len = 0;
    cmr_u32 cur_fr_len = 0;
    cmr_u32 fr_len = 0;
    float fps = 0.0;
    cmr_u16 frame_interval = 0x00;
    cmr_u64 longexp_value = 0x00;
    cmr_u16 longexp_value_shift = 0x00;

    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    sns_drv_cxt->frame_length_def = sns_drv_cxt->trim_tab_info[mode].frame_line;
    sns_drv_cxt->line_time_def = sns_drv_cxt->trim_tab_info[mode].line_time;
    cur_fr_len = sns_drv_cxt->sensor_ev_info.preview_framelength;
    fr_len = sns_drv_cxt->frame_length_def;


    if (exp_time < 500000000) {
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

        SENSOR_LOGD("mode = %d, exposure_line = %d, dummy_line= %d, "
                "frame_interval= %d ms, fps = %f",
                mode, shutter, dummy_line, frame_interval, fps);

        imx586_drv_write_frame_length(handle, aec_info, dest_fr_len);
        sns_drv_cxt->sensor_ev_info.preview_framelength = dest_fr_len;
        imx586_drv_write_shutter(handle, aec_info, shutter, longexp_value_shift);
        sns_drv_cxt->sensor_ev_info.preview_shutter = shutter;       
    } else {
        SENSOR_LOGI("long Exposure mode exp_time=%llu", exp_time);
	if (exp_time >= 500000000 && exp_time < 1000000000) {
            longexp_value = exp_time / 2 / PREVIEW_LINE_TIME;
            longexp_value_shift = 0x01;
            imx586_drv_write_shutter(handle, aec_info, longexp_value, longexp_value_shift);
        } else if (exp_time >= 1000000000 && exp_time < 2000000000) {
            longexp_value = exp_time / 4 / PREVIEW_LINE_TIME;
            longexp_value_shift = 0x02;
            imx586_drv_write_shutter(handle, aec_info, longexp_value, longexp_value_shift);
        } else if (exp_time >= 2000000000 && exp_time < 4000000000) {
            longexp_value = exp_time / 8 / PREVIEW_LINE_TIME;
            longexp_value_shift = 0x03;
            imx586_drv_write_shutter(handle, aec_info, longexp_value, longexp_value_shift);
        } else if (exp_time >= 4000000000 && exp_time < 8000000000) {
            longexp_value = exp_time / 16 / PREVIEW_LINE_TIME;
            longexp_value_shift = 0x04;
            imx586_drv_write_shutter(handle, aec_info, longexp_value, longexp_value_shift);
        } else if (exp_time >= 8000000000 && exp_time < 16000000000) {
            longexp_value = exp_time / 32 / PREVIEW_LINE_TIME;
            longexp_value_shift = 0x05;
            imx586_drv_write_shutter(handle, aec_info, longexp_value, longexp_value_shift);
        } else if (exp_time >= 16000000000 && exp_time < 32000000000) {
            longexp_value = exp_time / 64 / PREVIEW_LINE_TIME;
            longexp_value_shift = 0x06;
            imx586_drv_write_shutter(handle, aec_info, longexp_value, longexp_value_shift);
        } else if (exp_time >= 32000000000 && exp_time <= 64000000000) {
            longexp_value = exp_time / 128 / PREVIEW_LINE_TIME;
            longexp_value_shift = 0x07;
            imx586_drv_write_shutter(handle, aec_info, longexp_value, longexp_value_shift);
        } else {
            SENSOR_LOGE("longExp exp_time=%llu beyond the restrict", exp_time);
        }
    }
}

static void imx586_drv_calc_gain(cmr_handle handle, cmr_uint isp_gain,
                                 struct sensor_aec_i2c_tag *aec_info) {
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u32 sensor_gain = 0;

    sensor_gain = isp_gain < SENSOR_BASE_GAIN ? SENSOR_BASE_GAIN : (cmr_u32)isp_gain;

    if (SENSOR_MAX_GAIN < sensor_gain)
        sensor_gain = SENSOR_MAX_GAIN;

    SENSOR_LOGD("isp_gain = 0x%x, sensor_gain = 0x%x", (unsigned int)isp_gain,
                sensor_gain);

    sns_drv_cxt->sensor_ev_info.preview_gain = isp_gain;
    imx586_drv_write_gain(handle, aec_info, (cmr_u32)isp_gain);
}

/*==============================================================================
 * Description:
 * sensor power on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int imx586_drv_power_on(cmr_handle handle, cmr_uint power_on) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;

    SENSOR_AVDD_VAL_E dvdd_val = module_info->dvdd_val;
    SENSOR_AVDD_VAL_E avdd_val = module_info->avdd_val;
    SENSOR_AVDD_VAL_E iovdd_val = module_info->iovdd_val;
    BOOLEAN power_down = g_imx586_mipi_raw_info.power_down_level;
    BOOLEAN reset_level = g_imx586_mipi_raw_info.reset_pulse_level;

    if (SENSOR_TRUE == power_on) {
        hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        usleep(2 * 1000);

        hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, iovdd_val);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, avdd_val);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, dvdd_val);
        usleep(2 * 1000);

        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, EX_MCLK);
        usleep(2 * 1000);

        hw_sensor_power_down(sns_drv_cxt->hw_handle, !power_down);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, !reset_level);
        usleep(5 * 1000);
    } else {
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
        usleep(1 * 1000);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        usleep(1 * 1000);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
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
static cmr_int imx586_drv_init_fps_info(cmr_handle handle) {
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
            // max fps should be multiple of 30,it calulated from line_time and frame_line
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

static cmr_int imx586_drv_get_static_info(cmr_handle handle, cmr_u32 *param) {
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
    char value[128];

    // make sure we have get max fps of all settings.
    if (!fps_info->is_init) {
        imx586_drv_init_fps_info(handle);
    }
    property_get("persist.vendor.cam.sensor.pdaf.type1.on", value, "0");
    ex_info->f_num = static_info->f_num;
    ex_info->focal_length = static_info->focal_length;
    ex_info->min_focus_distance = static_info->min_focal_distance;
    ex_info->start_offset_time = static_info->start_offset_time;
    ex_info->max_fps = static_info->max_fps;
    ex_info->max_adgain = static_info->max_adgain;
    ex_info->ois_supported = static_info->ois_supported;
    if(atoi(value))
        ex_info->pdaf_supported = 1;
    else
        ex_info->pdaf_supported = static_info->pdaf_supported;
    ex_info->exp_valid_frame_num = static_info->exp_valid_frame_num;
    ex_info->clamp_level = static_info->clamp_level;
    ex_info->adgain_valid_frame_num = static_info->adgain_valid_frame_num;
    ex_info->preview_skip_num = module_info->preview_skip_num;
    ex_info->capture_skip_num = module_info->capture_skip_num;
    ex_info->name = (cmr_s8 *)g_imx586_mipi_raw_info.name;
    ex_info->sensor_version_info =
        (cmr_s8 *)g_imx586_mipi_raw_info.sensor_version_info;
    ex_info->fov_angle = static_info->fov_angle;
    memcpy(&ex_info->fov_info, &static_info->fov_info,
           sizeof(static_info->fov_info));
    ex_info->pos_dis.up2hori = up;
    ex_info->pos_dis.hori2down = down;
    ex_info->long_expose_supported = static_info->long_expose_supported;
    if(ex_info->long_expose_supported) {
        ex_info->long_expose_modes = imx586_longExp_mode;
        ex_info->long_expose_modes_size = ARRAY_SIZE(imx586_longExp_mode);
        ex_info->long_exposure_setting = imx586_longExp_setting;
        ex_info->long_exposure_setting_size = ARRAY_SIZE(imx586_longExp_setting);
        ex_info->longExp_valid_frame_num = imx586_longExp_valid_frame_num;
        ex_info->long_exposure_threshold = imx586_longExp_threshold;
        ex_info->longExp_need_switch_setting = imx586_longExp_need_switch_setting;
    }
    sensor_ic_print_static_info((cmr_s8 *)SENSOR_NAME, ex_info);

    return rtn;
}

static cmr_int imx586_drv_get_fps_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_MODE_FPS_T *fps_info = (SENSOR_MODE_FPS_T *)param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(fps_info);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_fps_info *fps_data = sns_drv_cxt->fps_info;

    // make sure have inited fps of every sensor mode.
    if (!fps_data->is_init) {
        imx586_drv_init_fps_info(handle);
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



cmr_int imx586_drv_pdaf_data_process(void *buffer_handle);

static cmr_int imx586_drv_get_pdaf_info(cmr_handle handle, void *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_pdaf_info_tab *pdaf_info_tab = NULL;
    struct sensor_pdaf_info *pdaf_info = NULL;
    cmr_int pd_info_num = 0;
    int i = 0;
    SENSOR_IC_CHECK_PTR(param);

    SENSOR_LOGV("E");

    pdaf_info_tab = (struct sensor_pdaf_info_tab *)param;
    
    pdaf_info_tab->sns_mode_num = imx586_pdaf_info_tab.sns_mode_num;
    pdaf_info_tab->pd_sns_mode = imx586_pdaf_info_tab.pd_sns_mode;
    pdaf_info_tab->pdaf_init_index = imx586_pdaf_info_tab.pdaf_init_index;
    pdaf_info_tab->pd_init_size_w = imx586_pdaf_info_tab.pd_init_size_w;
    pdaf_info_tab->pd_init_size_h = imx586_pdaf_info_tab.pd_init_size_h;
    pdaf_info_tab->pd_info_tab = imx586_pdaf_info_tab.pd_info_tab;
    pdaf_info_tab->pd_iszfull_info = imx586_pdaf_info_tab.pd_iszfull_info;

    pd_info_num = imx586_pdaf_info_tab.sns_mode_num;
    for(i = 0; i < pd_info_num; i++) {
        if(pdaf_info_tab->pd_sns_mode[i]){
            pdaf_info = &(pdaf_info_tab->pd_info_tab[i]);
            pdaf_info->pdaf_format_converter = imx586_drv_pdaf_data_process;
            pdaf_info->pd_data_size = pdaf_info->pd_block_num_x * pdaf_info->pd_block_num_y *
                                      pdaf_info->pd_pos_size * 2 * 5;
            SENSOR_LOGD("pd data size:%d", pdaf_info->pd_data_size);
        }
    }
    SENSOR_LOGE("sensor pdaf_init_index %d", pdaf_info_tab->pdaf_init_index);
    return rtn;
}

cmr_int imx586_drv_pdaf_data_process(void *buffer_handle) {
    if(!buffer_handle)
        return SENSOR_FAIL;
    cmr_u32 sensor_mode = 0;
    imx586_sns_drv_cxt->ops_cb.get_mode(imx586_sns_drv_cxt->caller_handle, &sensor_mode);
    if(!imx586_pd_sns_mode[sensor_mode]) {
        SENSOR_LOGE("sensor mode %d pdaf type2 unsupported!", sensor_mode);
        return SENSOR_FAIL;
    }
    sensor_pdaf_format_convertor(buffer_handle, s_imx586_static_info[0].static_info.pdaf_supported,
                                 (cmr_u32 *)(&imx586_pdaf_info_array[sensor_mode]));
    return SENSOR_SUCCESS;
}
/*
#include "parameters/param_manager.c"
static cmr_int imx586_drv_set_raw_info(cmr_handle handle, cmr_u8 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    cmr_u8 vendor_id = (cmr_u8)*param;
    SENSOR_LOGI("*param %x %x", *param, vendor_id);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    s_imx586_mipi_raw_info_ptr = imx586_drv_init_raw_info(sns_drv_cxt->sensor_id, vendor_id, 0, 0);

    return rtn;
}
*/
#if 1
static const cmr_u32 sns_4in1_mode[] = {0, 0, 0, 0, 0, 1};
static cmr_int imx586_drv_get_4in1_info(cmr_handle handle, cmr_u32 *param) {
	cmr_int rtn = SENSOR_SUCCESS;
	struct sensor_4in1_info *sn_4in1_info = NULL;
	SENSOR_IC_CHECK_PTR(param);

	SENSOR_LOGI("E\n");

	sn_4in1_info = (struct sensor_4in1_info *)param;
	sn_4in1_info->is_4in1_supported = 0;
	sn_4in1_info->limited_4in1_width = 4000;
	sn_4in1_info->limited_4in1_height = 3000;
	sn_4in1_info->sns_mode = sns_4in1_mode;

	return rtn;
}
#endif 

static cmr_s64 imx586_drv_get_shutter_skew(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    cmr_u16 height = 0;
    cmr_u32 line_time = 0;
    cmr_s64 shutter_skew = 0;
    SENSOR_IC_CHECK_PTR(param);
    SENSOR_LOGV("E");
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_shutter_skew_info *shutter_skew_info =
        (struct sensor_shutter_skew_info *)param;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    height = s_imx586_resolution_tab_raw[0].reg_tab[shutter_skew_info->sns_mode].height;
    line_time = s_imx586_resolution_trim_tab[0].trim_info[shutter_skew_info->sns_mode].line_time;
    shutter_skew = (height - 1) * line_time;
    shutter_skew_info->shutter_skew = shutter_skew;
    SENSOR_LOGD("sensor_mode:%d, height:%d, line_time:%d, shutter_skew:%d",
                shutter_skew_info->sns_mode, height, line_time, shutter_skew);
    return rtn;
}
static cmr_int imx586_drv_set_awb_gain(cmr_handle handle, void *awb_param) {
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_awb_gain_info *awb_info = (struct sensor_awb_gain_info *)awb_param;
    cmr_int ret = SENSOR_SUCCESS;
    sns_drv_cxt->awb_gain.b = awb_info->b;
    sns_drv_cxt->awb_gain.r = awb_info->r;
    sns_drv_cxt->awb_gain.g = awb_info->g;
    SENSOR_LOGV("AWB gain info from ispalg is [R: %d, G: %d, B: %d]",
                 awb_info->r, awb_info->g, awb_info->b);
    return ret;

}

static cmr_int imx586_drv_write_awb_gain(cmr_handle handle) {
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_int ret = SENSOR_SUCCESS;
    float R, G, B, awbBase;
    int r, g, b;
/*
    AWB algorithm of platform is 1024 - 16384 as 1X to 16X, and should be normalized as 0-255
 */
    awbBase = 1024;
    R = ((sns_drv_cxt->awb_gain.r / awbBase) * 256);
    G = ((sns_drv_cxt->awb_gain.g / awbBase) * 256);
    B = ((sns_drv_cxt->awb_gain.b / awbBase) * 256);
    r = (int)R;
    g = (int)G;
    b = (int)B;
    SENSOR_LOGD("Integer AWB Gain info is %d, %d, %d", r, g, b);
    ret |= hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0b8f, (g & 0xff));
    ret |= hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0b8e, (g >> 8) & 0xff);

    ret |= hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0b91, (r & 0xff));
    ret |= hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0b90, (r >> 8) & 0xff);

    ret |= hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0b93, (b & 0xff));
    ret |= hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0b92, (b >> 8) & 0xff);

    ret |= hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0b95, (g & 0xff));
    ret |= hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0b94, (g >> 8) & 0xff);

    return ret;

}

static cmr_int imx586_drv_dump_trace(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    cmr_u16 value = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0005);
    SENSOR_LOGI("FRM_CNT 0x0005:0x%x", value);

    return rtn;
}

/*==============================================================================
 * Description:
 * cfg otp setting
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int imx586_drv_access_val(cmr_handle handle, cmr_uint param) {
    cmr_int ret = SENSOR_FAIL;
    SENSOR_VAL_T *param_ptr = (SENSOR_VAL_T *)param;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param_ptr);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGV("sensor imx586: param_ptr->type = 0x%x", param_ptr->type);

    switch (param_ptr->type) {
    case SENSOR_VAL_TYPE_GET_STATIC_INFO:
        ret = imx586_drv_get_static_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_FPS_INFO:
        ret = imx586_drv_get_fps_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_SET_SENSOR_CLOSE_FLAG:
        ret = sns_drv_cxt->is_sensor_close = 1;
        break;
    case SENSOR_VAL_TYPE_GET_4IN1_INFO:
        ret = imx586_drv_get_4in1_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_SET_OTP_DATA:
        ret = 0;//imx586_drv_ov4c_init(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_PROC:
        ret = 0;//imx586_drv_ov4c_process(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_DEINIT:
        ret = 0;//imx586_drv_ov4c_deinit(handle, param_ptr->pval);
        break;
//    case SENSOR_VAL_TYPE_SET_RAW_INFOR:
//        ret = imx586_drv_set_raw_info(handle, param_ptr->pval);
//        break;
    case SENSOR_VAL_TYPE_GET_PDAF_INFO:
        ret = imx586_drv_get_pdaf_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_SHUTTER_SKEW_DATA:
        ret = imx586_drv_get_shutter_skew(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_WRITE_AWB_GAIN:
        ret = imx586_drv_set_awb_gain(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_DUMP_TRACE:
        ret = imx586_drv_dump_trace(handle, param_ptr->pval);
        break;
    default:
        break;
    }
    ret = SENSOR_SUCCESS;

    return ret;
}

static cmr_u8 imx586_snspid_is_init = 0;

static cmr_int imx586_drv_save_snspid(cmr_handle handle) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u8 snspid[32] = {0};
    cmr_u16 snspid_size = 32; //sensor_drv_u.c sensor_snspid max-size is 32byte

    SENSOR_LOGI("E");

    if (sns_drv_cxt->ops_cb.set_snspid) {
        sns_drv_cxt->ops_cb.set_snspid(
            sns_drv_cxt->caller_handle, sns_drv_cxt->sensor_id, snspid, snspid_size);
    }

    imx586_snspid_is_init = 1;
    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * identify sensor id
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int imx586_drv_identify(cmr_handle handle, cmr_uint param) {
    cmr_u16 pid_value = 0x00;
    cmr_u16 ver_value = 0x00;
    cmr_int ret_value = SENSOR_FAIL;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("mipi raw identify");

    pid_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, imx586_PID_ADDR);

    if (imx586_PID_VALUE == pid_value) {
        ver_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, imx586_VER_ADDR);
        SENSOR_LOGI("Identify: pid_value = %x, ver_value = %x", pid_value,
                    ver_value);
        if (imx586_VER_VALUE == ver_value) {
            SENSOR_LOGI("this is imx586 sensor");
            ret_value = SENSOR_SUCCESS;
            if (!imx586_snspid_is_init) {
                imx586_drv_save_snspid(handle);
            }
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
static cmr_int imx586_drv_before_snapshot(cmr_handle handle, cmr_uint param) {
    cmr_u32 cap_shutter = 0;
    cmr_u32 prv_shutter = 0;
    cmr_u32 prv_gain = 0;
    cmr_u32 cap_gain = 0;
    cmr_u32 capture_mode = param & 0xffff;
    cmr_u32 preview_mode = (param >> 0x10) & 0xffff;
    cmr_u64 cap_exptime = 0;
    cmr_u64 prv_exptime = 0;

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
        cap_exptime = sns_drv_cxt->sensor_ev_info.preview_exptime;
        cap_gain = sns_drv_cxt->sensor_ev_info.preview_gain;
        goto snapshot_info;
    }

    if (sns_drv_cxt->ops_cb.set_mode)
        sns_drv_cxt->ops_cb.set_mode(sns_drv_cxt->caller_handle, capture_mode);
    if (sns_drv_cxt->ops_cb.set_mode_wait_done)
        sns_drv_cxt->ops_cb.set_mode_wait_done(sns_drv_cxt->caller_handle);

    prv_shutter = sns_drv_cxt->sensor_ev_info.preview_shutter;
    prv_exptime = sns_drv_cxt->sensor_ev_info.preview_exptime;
    prv_gain = sns_drv_cxt->sensor_ev_info.preview_gain;

    if(sns_4in1_mode[capture_mode]) {
        cap_shutter = prv_shutter * prv_linetime / cap_linetime * BINNING_FACTOR;
        cap_shutter = cap_shutter < 38700 ? cap_shutter : 38700;
        cap_exptime = prv_exptime  * BINNING_FACTOR;
        cap_exptime = cap_exptime < 600000000 ? cap_exptime : 600000000;
    } else {
        cap_shutter = prv_shutter * prv_linetime / cap_linetime;
        cap_exptime = prv_exptime;
    }
    cap_gain = prv_gain;

    SENSOR_LOGI("capture_shutter = 0x%x, capture_gain = 0x%x", cap_shutter,
                cap_gain);

    imx586_drv_calc_exposure(handle, cap_shutter, 0, capture_mode,cap_exptime,
                             &imx586_aec_info);
    imx586_drv_write_reg2sensor(handle, imx586_aec_info.frame_length);
    imx586_drv_write_reg2sensor(handle, imx586_aec_info.shutter);

    sns_drv_cxt->sensor_ev_info.preview_gain = cap_gain;
    imx586_drv_write_gain(handle, &imx586_aec_info, cap_gain);
    imx586_drv_write_reg2sensor(handle, imx586_aec_info.again);
    imx586_drv_write_reg2sensor(handle, imx586_aec_info.dgain);
    imx586_drv_write_awb_gain(handle);

snapshot_info:
    if (sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle,
                                          SENSOR_EXIF_CTRL_EXPOSURETIME_BYTIME, cap_exptime);
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
static cmr_int imx586_drv_write_exposure(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 size_index = 0x00;
    cmr_u64 exp_time = 0x00;
    cmr_u64 longexp_value = 0x00;

    struct sensor_ex_exposure *ex = (struct sensor_ex_exposure *)param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_HANDLE(ex);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    exposure_line = ex->exposure;
    dummy_line = ex->dummy;
    size_index = ex->size_index;
    exp_time = ex->exp_time;

    if (exp_time < 500000000) {
        imx586_drv_calc_exposure(handle, exposure_line, dummy_line, size_index, exp_time,
                                &imx586_aec_info);
        imx586_drv_write_reg2sensor(handle, imx586_aec_info.frame_length);
        imx586_drv_write_reg2sensor(handle, imx586_aec_info.shutter);
    } else {
        SENSOR_LOGI("long Exposure mode exp_time=%llu", exp_time);

        longexp_value = 500000000 / PREVIEW_LINE_TIME;
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0202, (longexp_value >> 8) & 0xff);
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0203, longexp_value & 0xff);
        if (exp_time == 500000000) {
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3100, 0x00);//*1
        } else if (exp_time == 1000000000) {
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3100, 0x01);//*2
        } else if (exp_time == 2000000000) {
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3100, 0x02);//*4
        } else if (exp_time == 3000000000 || exp_time == 4000000000 || exp_time == 5000000000) {
            longexp_value = exp_time / 8 / PREVIEW_LINE_TIME;
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0202, (longexp_value >> 8) & 0xff);
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0203, longexp_value & 0xff);
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3100, 0x03);//*8
        } else if (exp_time >= 6000000000 && exp_time < 10000000000) {
            longexp_value = exp_time / 16 / PREVIEW_LINE_TIME;
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0202, (longexp_value >> 8) & 0xff);
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0203, longexp_value & 0xff);
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3100, 0x04);//*16
        } else if (exp_time >= 10000000000 && exp_time <= 30000000000) {
            longexp_value = exp_time / 64 / PREVIEW_LINE_TIME;
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0202, (longexp_value >> 8) & 0xff);
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0203, longexp_value & 0xff);
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3100, 0x06);//*64
        } else {
            SENSOR_LOGE("longExp exp_time=%llu beyond the restrict", exp_time);
        }
    }

    return ret_value;
}

/*==============================================================================
 * Description:
 * write gain value to sensor
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int imx586_drv_write_gain_value(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    SENSOR_LOGD("GAIN IS %d", param);
    imx586_drv_calc_gain(handle, param, &imx586_aec_info);
    imx586_drv_write_reg2sensor(handle, imx586_aec_info.again);
    imx586_drv_write_reg2sensor(handle, imx586_aec_info.dgain);

    return ret_value;
}

/*==============================================================================
 * Description:
 * read ae control info
 * please don't change this function unless it's necessary
 *============================================================================*/
static cmr_int imx586_drv_read_aec_info(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 mode = 0x00;
    cmr_u64 exp_time = 0x00;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    SENSOR_LOGD("E");

    struct sensor_aec_reg_info *info = (struct sensor_aec_reg_info *)param;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    info->aec_i2c_info_out = &imx586_aec_info;
    exposure_line = info->exp.exposure;
    dummy_line = info->exp.dummy;
    mode = info->exp.size_index;
    exp_time = info->exp.exp_time;

    imx586_drv_calc_exposure(handle, exposure_line, dummy_line, mode, exp_time,
                                &imx586_aec_info);
    imx586_drv_calc_gain(handle, info->gain, &imx586_aec_info);

    return ret_value;
}

static cmr_int imx586_drv_set_master_FrameSync(cmr_handle handle,
                                               cmr_uint param) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGD("E");

    /*TODO*/

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3040, 0x01);

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3F71, 0x01);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4B82, 0x03);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4B83, 0x01);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4B85, 0x01);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x423D, 0xff);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4BD7, 0x16);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4225, 0x00);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3041, 0x01);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x424A, 0x00);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x424B, 0xf0);
    /*END*/
    SENSOR_LOGD("0x424A read out is 0x%x", hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x424A));
    SENSOR_LOGD("0x424B read out is 0x%x", hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x424B));
    return SENSOR_SUCCESS;
}

static cmr_int imx586_drv_set_pdaf_mode(cmr_handle handle, cmr_uint param) {
    UNUSED(param);

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    if (param == 0x02) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0315d, 0x02);
    } else if (param == 0x01) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0315d,
                            0x01); // 8x6 504x501 //4032x3008
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x03104, 0x01); // 0x02
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x03105, 0xf8); // 0x30
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x03106, 0x01); // 0x0d
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x03107, 0xf5); // 0xa8
    } else if (param == 0x00) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0315d,
                            0x00); // 16x12 252x250
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x03104, 0x00); // 0x02
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x03105, 0xfc); // 0x30
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x03106, 0x00); // 0x0d
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x03107, 0xfa); // 0xa8
    }
#if 0
        SENSOR_LOGI("E %x",hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x38a3));
        SENSOR_LOGI("E %x",hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3e37));

        for(int i = 0x38a3; i < 0x38f3; i++){
            SENSOR_LOGI("E1  0x%04x 0x%02x",  i, hw_sensor_read_reg(sns_drv_cxt->hw_handle, i));
         }
#endif
    return 0;
}

static cmr_int imx586_drv_set_qsc_data(cmr_handle handle, cmr_uint param) {
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u8 *param_ptr = NULL;
    struct sensor_drv_context *sensor_cxt = (struct sensor_drv_context *)(sns_drv_cxt->caller_handle);
    SENSOR_IC_CHECK_HANDLE(sns_drv_cxt->caller_handle);
    otp_drv_cxt_t *otp_cxt = (otp_drv_cxt_t *)(sensor_cxt->otp_drv_handle);
    char value[128];
    property_get("persist.vendor.cam.skip.qsc", value, "0");
    if(atoi(value))
        return 0;
    if(!otp_cxt || !otp_cxt->otp_raw_data.buffer) {
        SENSOR_LOGD("otp not configured");
        return 0;
    }

    param_ptr = otp_cxt->otp_raw_data.buffer;
    SENSOR_REG_T imx586_qsc_setting[2304];
    memset(imx586_qsc_setting, 0, 2304 * sizeof(SENSOR_REG_T));
    for (int i = 0; i < 2304; i++) {
        imx586_qsc_setting[i].reg_addr = 0x7f00 + i;
        imx586_qsc_setting[i].reg_value = *(param_ptr + 0xdde + i);
    }

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3621, 0x01);
    hw_sensor_dev_WriteRegTab(sns_drv_cxt->hw_handle,
            imx586_qsc_setting, 2304);

    return 0;

}

static cmr_int imx586_drv_set_LRC_data(cmr_handle handle, cmr_uint param) {
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_drv_context *sensor_cxt = (struct sensor_drv_context *)(sns_drv_cxt->caller_handle);
    cmr_u8 *param_ptr = NULL;
    SENSOR_IC_CHECK_HANDLE(sns_drv_cxt->caller_handle);
    otp_drv_cxt_t *otp_cxt = (otp_drv_cxt_t *)(sensor_cxt->otp_drv_handle);

    char value[128];
    property_get("persist.vendor.cam.skip.LRC.area.control", value, "0");
    if(atoi(value)) {
	SENSOR_LOGD("START TO SKIP");
	return 0;
    }

    if(!otp_cxt || !otp_cxt->otp_raw_data.buffer) {
        SENSOR_LOGD("otp not configured");
        return 0;
    }

    param_ptr = otp_cxt->otp_raw_data.buffer;

    SENSOR_REG_T imx586_LRC_setting1[192];
    memset(imx586_LRC_setting1, 0, 192 * sizeof(SENSOR_REG_T));
    for (int i = 0; i < 192; i++) {
        imx586_LRC_setting1[i].reg_addr = 0x7510 + i;
        imx586_LRC_setting1[i].reg_value = *(param_ptr + 0x0C5D + i);
    }
    hw_sensor_dev_WriteRegTab(sns_drv_cxt->hw_handle,
            imx586_LRC_setting1, 192);

    SENSOR_REG_T imx586_LRC_setting2[192];
    memset(imx586_LRC_setting2, 0, 192 * sizeof(SENSOR_REG_T));
    for (int i = 0; i < 192; i++) {
        imx586_LRC_setting2[i].reg_addr = 0x7600 + i;
        imx586_LRC_setting2[i].reg_value = *(param_ptr + 0x0D1D + i);
    }
    hw_sensor_dev_WriteRegTab(sns_drv_cxt->hw_handle,
            imx586_LRC_setting2, 192);

    return 0;
}

/*==============================================================================
 * Description:
 * mipi stream on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int imx586_drv_stream_on(cmr_handle handle, cmr_uint param) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("E");
    char value1[PROPERTY_VALUE_MAX], value2[PROPERTY_VALUE_MAX], value3[PROPERTY_VALUE_MAX];
    property_get("persist.vendor.cam.colorbar", value1, "0");

    cmr_u32 sensor_mode = 0;
    sns_drv_cxt->ops_cb.get_mode(sns_drv_cxt->caller_handle, &sensor_mode);
    if(sensor_mode == 4)
	imx586_drv_set_LRC_data(handle, param);
    if (sensor_mode > 4)
        imx586_drv_set_qsc_data(handle, param);
    else
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3621, 0x00);

    if (sns_4in1_mode[sensor_mode]) {
        imx586_drv_write_awb_gain(handle);
    }

    if (!strcmp(value1, "1")) {
        SENSOR_LOGI("enable test mode");
     //   hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x780F, 0x00);
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0600, 0x00);
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0601, 0x02);
    }

#if 0//def IMX586_CPHY
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0808, 0x02); //default 00
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0854, 0x03); //default 00
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0855, 0xf4);//defaul 14
#endif
//	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0856, 0x01); //default 00

   // imx586_drv_set_pdaf_mode(handle, 1);
/*    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0315d, 0x02);

    SENSOR_LOGI("E %x",hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3047));
    SENSOR_LOGI("E %x",hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3049));
    SENSOR_LOGI("E %x",hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x315d));*/

    property_get("vendor.cam.hw.framesync.on", value2, "1");
    if (!strcmp(value2, "1") && sns_drv_cxt->is_multi_mode) {
        imx586_drv_set_master_FrameSync(handle, param);
        // imx586_drv_set_slave_FrameSync(handle, param);
    }

    /*TODO*/
    property_get("persist.vendor.cam.sensor.pdaf.type1.on", value3, "0");
    if(atoi(value3)) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3e20, 0x02);
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3e37, 0x00);
    }
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x01);

    /*END*/

    /*delay*/
    // usleep(1 * 1000);

    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * mipi stream off
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int imx586_drv_stream_off(cmr_handle handle, cmr_uint param) {
    SENSOR_LOGI("E");
    cmr_u16 value = 0;
    cmr_s32 ret = SENSOR_SUCCESS;
    cmr_u16 status = 0;
    clock_t start_clk, end_clk;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0100);
    if (value != 0x00) {
        ret = hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x00);
        start_clk = end_clk = clock();
        while(1) {
            status = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0005);
            if(status == 0xff) {
                goto exit;
            }
            end_clk = clock();
            if((end_clk - start_clk) * 1000 / CLOCKS_PER_SEC > 100) {
                SENSOR_LOGI("sensor stream off out of time");
                goto exit;
            }
            usleep(1*1000);
        }
    } else {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x00);
    }

exit:
    sns_drv_cxt->is_sensor_close = 0;
    SENSOR_LOGI("X");

    return ret;
}

static cmr_int imx586_drv_handle_create(struct sensor_ic_drv_init_para *init_param,
                         cmr_handle *sns_ic_drv_handle) {
    cmr_int ret = SENSOR_SUCCESS;
    cmr_u16 shutter_shift = 0x00;
    struct sensor_ic_drv_cxt *sns_drv_cxt = NULL;
    void *pri_data = NULL;

    ret = sensor_ic_drv_create(init_param, sns_ic_drv_handle);
    sns_drv_cxt = *sns_ic_drv_handle;
    imx586_sns_drv_cxt = *sns_ic_drv_handle;

    sns_drv_cxt->line_time_def = PREVIEW_LINE_TIME;

    sns_drv_cxt->sensor_ev_info.preview_shutter =
        PREVIEW_FRAME_LENGTH - FRAME_OFFSET;
    sns_drv_cxt->sensor_ev_info.preview_gain = SENSOR_BASE_GAIN;
    sns_drv_cxt->sensor_ev_info.preview_framelength = PREVIEW_FRAME_LENGTH;

    sns_drv_cxt->frame_length_def = PREVIEW_FRAME_LENGTH;
    sns_drv_cxt->sensor_ev_info.preview_exptime =(PREVIEW_FRAME_LENGTH - FRAME_OFFSET) * PREVIEW_LINE_TIME;

    imx586_drv_write_frame_length(
        sns_drv_cxt, &imx586_aec_info,
        sns_drv_cxt->sensor_ev_info.preview_framelength);
    imx586_drv_write_gain(sns_drv_cxt, &imx586_aec_info,
                          sns_drv_cxt->sensor_ev_info.preview_gain);
    imx586_drv_write_shutter(sns_drv_cxt, &imx586_aec_info,
                             sns_drv_cxt->sensor_ev_info.preview_shutter, shutter_shift);

    sensor_ic_set_match_module_info(sns_drv_cxt,
                                    ARRAY_SIZE(s_imx586_module_info_tab),
                                    s_imx586_module_info_tab);
    sensor_ic_set_match_resolution_info(sns_drv_cxt,
                                        ARRAY_SIZE(s_imx586_resolution_tab_raw),
                                        s_imx586_resolution_tab_raw);
    sensor_ic_set_match_trim_info(sns_drv_cxt,
                                  ARRAY_SIZE(s_imx586_resolution_trim_tab),
                                  s_imx586_resolution_trim_tab);
    sensor_ic_set_match_static_info(
        sns_drv_cxt, ARRAY_SIZE(s_imx586_static_info), s_imx586_static_info);
    sensor_ic_set_match_fps_info(sns_drv_cxt,
                                 ARRAY_SIZE(s_imx586_mode_fps_info),
                                 s_imx586_mode_fps_info);

    /*init exif info,this will be deleted in the future*/
    imx586_drv_init_fps_info(sns_drv_cxt);

    /*add private here*/
    return ret;
}

static cmr_int imx586_drv_handle_delete(cmr_handle handle, void *param) {
    cmr_int ret = SENSOR_SUCCESS;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    ret = sensor_ic_drv_delete(handle, param);
    return ret;
}

static cmr_int imx586_drv_get_private_data(cmr_handle handle, cmr_uint cmd,
                                           void **param) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);

    ret = sensor_ic_get_private_data(handle, cmd, param);
    return ret;
}

void *sensor_ic_open_lib(void)
{
     return &g_imx586_mipi_raw_info;
}

/*==============================================================================
 * Description:
 * all ioctl functoins
 * you can add functions reference SENSOR_IOCTL_FUNC_TAB_T from sensor_drv_u.h
 *
 * add ioctl functions like this:
 * .power = imx586_power_on,
 *============================================================================*/
static struct sensor_ic_ops s_imx586_ops_tab = {
    .create_handle = imx586_drv_handle_create,
    .delete_handle = imx586_drv_handle_delete,
    /*get privage data*/
    .get_data = imx586_drv_get_private_data,
    /*common interface*/
    .power = imx586_drv_power_on,
    .identify = imx586_drv_identify,
    .ex_write_exp = imx586_drv_write_exposure,
    .write_gain_value = imx586_drv_write_gain_value,
#if defined(CONFIG_DUAL_MODULE)
    .read_aec_info = imx586_drv_read_aec_info,
#endif
    .ext_ops = {
            [SENSOR_IOCTL_BEFORE_SNAPSHOT].ops = imx586_drv_before_snapshot,
            [SENSOR_IOCTL_STREAM_ON].ops = imx586_drv_stream_on,
            [SENSOR_IOCTL_STREAM_OFF].ops = imx586_drv_stream_off,
            /* expand interface,if you want to add your sub cmd ,
             *  you can add it in enum {@SENSOR_IOCTL_VAL_TYPE}
             */
            [SENSOR_IOCTL_ACCESS_VAL].ops = imx586_drv_access_val,
    }};
