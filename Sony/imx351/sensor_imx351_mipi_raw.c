/*
 * SPDX-FileCopyrightText: 2022-2023 Unisoc (Shanghai) Technologies Co., Ltd
 * SPDX-License-Identifier: LicenseRef-Unisoc-General-1.0
 *
 * Copyright 2022-2023 Unisoc (Shanghai) Technologies Co., Ltd.
 * Licensed under the Unisoc General Software License, version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * https://www.unisoc.com/en_us/license/UNISOC_GENERAL_LICENSE_V1.0-EN_US
 * Software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 * See the Unisoc General Software License, version 1.0 for more details.
 */

#define LOG_TAG "sensor_imx351"
#include "sensor_imx351_mipi_raw.h"

#define VIDEO_INFO s_imx351_video_info
#define STATIC_INFO s_imx351_static_info
#define FPS_INFO s_imx351_mode_fps_info
#define RES_TAB_RAW s_imx351_resolution_tab_raw
#define RES_TRIM_TAB s_imx351_resolution_trim_tab
#define MIPI_RAW_INFO g_imx351_mipi_raw_info
#define MODULE_INFO s_imx351_module_info_tab

/*==============================================================================
 * Description:
 * set video mode
 *
 *============================================================================*/
static cmr_int imx351_drv_set_video_mode(cmr_handle handle, cmr_u32 param) {
    cmr_int ret = SENSOR_SUCCESS;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_REG_T_PTR sensor_reg_ptr;
    cmr_u16 i = 0x00;
    cmr_u32 mode = 0;

    if (param >= SENSOR_VIDEO_MODE_MAX)
        return 0;

    if (sns_drv_cxt->ops_cb.get_mode) {
        ret = sns_drv_cxt->ops_cb.get_mode(handle, &mode);
        if (SENSOR_SUCCESS != ret) {
            SENSOR_LOGI("fail.");
            return SENSOR_FAIL;
        }
    }
    if (PNULL == VIDEO_INFO[mode].setting_ptr) {
        SENSOR_LOGI("fail.");
        return SENSOR_FAIL;
    }

    sensor_reg_ptr = (SENSOR_REG_T_PTR)&VIDEO_INFO[mode].setting_ptr[param];
    if (PNULL == sensor_reg_ptr) {
        SENSOR_LOGI("fail.");
        return SENSOR_FAIL;
    }

    for (i = 0x00; (0xffff != sensor_reg_ptr[i].reg_addr) ||
                   (0xff != sensor_reg_ptr[i].reg_value);
         i++) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, sensor_reg_ptr[i].reg_addr,
                            sensor_reg_ptr[i].reg_value);
    }

    return 0;
}
#if 0
/*==============================================================================
 * Description:
 * get default frame length
 *
 *============================================================================*/
static cmr_u32 imx351_drv_get_default_frame_length(cmr_handle handle,
                                                cmr_u32 mode) {
    return RES_TRIM_TAB[mode].frame_line;
}
#endif

/*==============================================================================
 * Description:
 * read gain from sensor registers
 * please modify this function acording your spec
 *============================================================================*/
static cmr_u16 imx351_drv_read_gain(cmr_handle handle) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    cmr_u16 gain_a = 0;
    cmr_u16 gain_d = 0;

    gain_a = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0205);
    gain_a = gain_a << 8 | hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0204);

    gain_d = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0210);
    gain_d = gain_d << 8 | hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0211);

    return gain_a * gain_d;
}

/*==============================================================================
 * Description:
 * write gain to sensor registers
 * please modify this function acording your spec
 *============================================================================*/
static void imx351_drv_write_gain(cmr_handle handle, double gain) {

    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u16 sensor_again = 0;

    if (gain > 16) {
        SENSOR_LOGD("real_gain:%f, over max_sensor_again 16.0", gain);
        gain = 16.0;
    } else {
        SENSOR_LOGD("real_gain:%f", gain);
    }

    sensor_again = (cmr_u16)(1024.0 - 1024.0 / gain);

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0104, 0x01);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0204,
                        (sensor_again >> 8) & 0xFF);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0205, sensor_again & 0xFF);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0104, 0x00);
}

/*==============================================================================
 * Description:
 * write frame length to sensor registers
 * please modify this function acording your spec
 *============================================================================*/
static void imx351_drv_write_frame_length(cmr_handle handle,
                                          cmr_u32 frame_len) {
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0340,
                        (frame_len >> 8) & 0xff);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0341, frame_len & 0xff);
}

/*==============================================================================
 * Description:
 * read shutter from sensor registers
 * please modify this function acording your spec
 *============================================================================*/
static cmr_u32 imx351_drv_read_shutter(cmr_handle handle) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    cmr_u16 shutter_h = 0;
    cmr_u16 shutter_l = 0;

    shutter_h = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0202) & 0xff;
    shutter_l = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0203) & 0xff;

    return (shutter_h << 8) | shutter_l;
}

/*==============================================================================
 * Description:
 * write shutter to sensor registers
 * please pay attention to the frame length
 * please modify this function acording your spec
 *============================================================================*/
static void imx351_drv_write_shutter(cmr_handle handle, cmr_u32 shutter) {
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0202, (shutter >> 8) & 0xff);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0203, shutter & 0xff);
}

/*==============================================================================
 * Description:
 * write exposure to sensor registers and get current shutter
 * please pay attention to the frame length
 * please don't change this function if it's necessary
 *============================================================================*/
static cmr_u16 imx351_drv_update_exposure(cmr_handle handle, cmr_u32 shutter,
                                          cmr_u32 dummy_line) {
    cmr_u32 dest_fr_len = 0;
    cmr_u32 fr_len = 0;
    cmr_u32 offset = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    fr_len = sns_drv_cxt->frame_length_def;

    if (dummy_line > FRAME_OFFSET)
        offset = dummy_line;
    else
        offset = FRAME_OFFSET;
    dest_fr_len = ((shutter + offset) > fr_len) ? (shutter + offset) : fr_len;

    if (shutter < SENSOR_MIN_SHUTTER)
        shutter = SENSOR_MIN_SHUTTER;

    imx351_drv_write_frame_length(handle, dest_fr_len);

    sns_drv_cxt->sensor_ev_info.preview_framelength = dest_fr_len;
write_sensor_shutter:
    /* write shutter to sensor registers */
    imx351_drv_write_shutter(handle, shutter);
    return shutter;
}

/*==============================================================================
 * Description:
 * sensor power on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int imx351_drv_power_on(cmr_handle handle, cmr_uint power_on) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;
    SENSOR_IC_CHECK_PTR(module_info);
    cmr_uint ret = 0;
    SENSOR_AVDD_VAL_E dvdd_val = module_info->dvdd_val;
    SENSOR_AVDD_VAL_E avdd_val = module_info->avdd_val;
    SENSOR_AVDD_VAL_E iovdd_val = module_info->iovdd_val;
    BOOLEAN power_down = MIPI_RAW_INFO.power_down_level;
    BOOLEAN reset_level = MIPI_RAW_INFO.reset_pulse_level;
    SENSOR_LOGI("(1:on, 0:off): %ld", power_on);

    if (SENSOR_TRUE == power_on) {
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        hw_sensor_set_voltage(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED,
                              SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED);
        ret = usleep(1 * 1000);
        hw_sensor_set_voltage(sns_drv_cxt->hw_handle, dvdd_val, avdd_val,
                              iovdd_val);
        ret = usleep(1 * 1000);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DEFALUT_MCLK);
        ret = usleep(1 * 1000);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, !reset_level);
        ret = usleep(1 * 1000);
    } else {
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        hw_sensor_set_voltage(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED,
                              SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED);
    }
    SENSOR_LOGI("(1:on, 0:off): %ld %d", power_on, ret);
    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * calculate fps for every sensor mode according to frame_line and line_time
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int imx351_drv_init_fps_info(cmr_handle handle) {
    cmr_int ret = SENSOR_SUCCESS;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
    struct sensor_trim_tag *trim_info = sns_drv_cxt->trim_tab_info;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;

    if (!fps_info->is_init) {
        cmr_u32 i, modn, tempfps = 0;
        SENSOR_LOGI("imx351_init_mode_fps_info:start init");
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
            SENSOR_LOGV("mode %d,tempfps %d,frame_len %d,line_time: %d ", i,
                        tempfps, trim_info[i].frame_line,
                        trim_info[i].line_time);
            SENSOR_LOGV("mode %d,max_fps: %d ", i,
                        fps_info->sensor_mode_fps[i].max_fps);
            SENSOR_LOGV("is_high_fps: %d,highfps_skip_num %d",
                        fps_info->sensor_mode_fps[i].is_high_fps,
                        fps_info->sensor_mode_fps[i].high_fps_skip_num);
        }
        fps_info->is_init = 1;
    }
    SENSOR_LOGV("X:");
    return ret;
}

static cmr_u8 imx351_snspid_is_init = 0;

static cmr_int imx351_drv_save_snspid(cmr_handle handle) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u8 snspid[32] = {0};
    cmr_u16 snspid_size = 32; //sensor_drv_u.c sensor_snspid max-size is 32byte

    SENSOR_LOGI("E");

    if (sns_drv_cxt->ops_cb.set_snspid) {
        sns_drv_cxt->ops_cb.set_snspid(
            sns_drv_cxt->caller_handle, sns_drv_cxt->sensor_id, snspid, snspid_size);
    }

    imx351_snspid_is_init = 1;
    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * identify sensor id
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int imx351_drv_identify(cmr_handle handle, cmr_uint param) {
    cmr_u8 pid_value = 0x00;
    cmr_u8 ver_value = 0x00;
    cmr_int ret_value = SENSOR_FAIL;
    cmr_u8 i = 0x00;
    UNUSED(param);

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("imx351 mipi raw identify");

    pid_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, imx351_PID_ADDR);

    if (imx351_PID_VALUE == pid_value) {
        ver_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, imx351_VER_ADDR);
        SENSOR_LOGI("Identify: PID = %x, VER = %x", pid_value, ver_value);
        if (imx351_VER_VALUE == ver_value) {
            ret_value = SENSOR_SUCCESS;
            SENSOR_LOGI("this is imx351 sensor");
            imx351_drv_init_fps_info(handle);
        if (!imx351_snspid_is_init) {
                imx351_drv_save_snspid(handle);
            }
        } else {
            SENSOR_LOGI("Identify this is %x%x sensor", pid_value, ver_value);
        }
    } else {
        SENSOR_LOGI("identify fail, pid_value = %x", pid_value);
    }

    return ret_value;
}

/*==============================================================================
 * Description:
 * get resolution trim
 *
 *============================================================================*/
static cmr_uint imx351_drv_get_trim_tab(cmr_handle handle, cmr_uint param) {
    UNUSED(param);
    UNUSED(handle);
    return (cmr_uint)RES_TRIM_TAB;
}

/*==============================================================================
 * Description:
 * before snapshot
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int imx351_drv_before_snapshot(cmr_handle handle, cmr_uint param) {
    cmr_u32 cap_shutter = 0;
    cmr_u64 cap_exptime = 0;
    cmr_u32 prv_shutter = 0;
    cmr_u64 prv_exptime = 0;
    float gain = 0;
    float cap_gain = 0;
    cmr_u32 capture_mode = param & 0xffff;
    cmr_u32 preview_mode = (param >> 0x10) & 0xffff;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u32 prv_linetime = sns_drv_cxt->trim_tab_info[preview_mode].line_time;
    cmr_u32 cap_linetime = sns_drv_cxt->trim_tab_info[capture_mode].line_time;

    sns_drv_cxt->frame_length_def =
        sns_drv_cxt->trim_tab_info[capture_mode].frame_line;
    SENSOR_LOGI("capture_mode = %d,preview_mode=%d\n", capture_mode,
                preview_mode);

    if (sns_drv_cxt->ops_cb.set_mode)
        sns_drv_cxt->ops_cb.set_mode(sns_drv_cxt->caller_handle, capture_mode);

    if (sns_drv_cxt->ops_cb.set_mode_wait_done)
        sns_drv_cxt->ops_cb.set_mode_wait_done(sns_drv_cxt->caller_handle);

    if (preview_mode == capture_mode) {
        cap_shutter = sns_drv_cxt->sensor_ev_info.preview_shutter;
        cap_exptime = sns_drv_cxt->sensor_ev_info.preview_exptime;
        cap_gain = sns_drv_cxt->sensor_ev_info.preview_gain;
        goto snapshot_info;
    }

    prv_shutter = sns_drv_cxt->sensor_ev_info.preview_shutter;
    prv_exptime = sns_drv_cxt->sensor_ev_info.preview_exptime;
    gain = sns_drv_cxt->sensor_ev_info.preview_gain;

    cap_shutter = prv_shutter * prv_linetime / cap_linetime;

    cap_shutter = imx351_drv_update_exposure(handle, cap_shutter, 0);
    cap_exptime = prv_exptime;
    cap_gain = gain;
    imx351_drv_write_gain(handle, cap_gain);
    SENSOR_LOGI("preview_shutter = %d, preview_exptime= %d, preview_gain = %f",
                sns_drv_cxt->sensor_ev_info.preview_shutter,
                sns_drv_cxt->sensor_ev_info.preview_exptime,
                sns_drv_cxt->sensor_ev_info.preview_gain);

    SENSOR_LOGI("capture_shutter = %d, capture_exptime = %llu, capture_gain = %f",
                cap_shutter, cap_exptime, cap_gain);
snapshot_info:
    sns_drv_cxt->hdr_info.capture_shutter = cap_shutter;
    sns_drv_cxt->hdr_info.capture_gain = cap_gain;
    /* limit HDR capture min fps to 10;
     * MaxFrameTime = 1000000*0.1us;
     */
    sns_drv_cxt->hdr_info.capture_max_shutter = 1000000 / cap_linetime;

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
 * get the shutter from isp
 * please don't change this function unless it's necessary
 *============================================================================*/
static cmr_int imx351_drv_write_exposure(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 mode = 0x00;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    exposure_line = param & 0xffff;
    dummy_line = (param >> 0x10) & 0xfff;
    mode = (param >> 0x1c) & 0x0f;

    SENSOR_LOGI("current mode = %d, exposure_line = %d, dummy_line=%d", mode,
                exposure_line, dummy_line);

    sns_drv_cxt->frame_length_def = sns_drv_cxt->trim_tab_info[mode].frame_line;

    sns_drv_cxt->sensor_ev_info.preview_shutter =
        imx351_drv_update_exposure(handle, exposure_line, dummy_line);

    return ret_value;
}

static cmr_int imx351_drv_ex_write_exposure(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 mode = 0x00;
    cmr_u32 exp_time = 0x00;
    struct sensor_ex_exposure *ex = (struct sensor_ex_exposure *)param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(ex);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    exposure_line = ex->exposure;
    dummy_line = ex->dummy;
    mode = ex->size_index;
    exp_time = ex->exp_time;

    SENSOR_LOGD("current mode = %d, exposure_line = %d, dummy_line=%d", mode,
                exposure_line, dummy_line);

    sns_drv_cxt->frame_length_def = sns_drv_cxt->trim_tab_info[mode].frame_line;
    sns_drv_cxt->line_time_def = sns_drv_cxt->trim_tab_info[mode].line_time;
    sns_drv_cxt->sensor_ev_info.preview_shutter =
        imx351_drv_update_exposure(handle, exposure_line, dummy_line);

    sns_drv_cxt->sensor_ev_info.preview_exptime= exp_time;
    if (!sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->exif_info.exposure_line = exposure_line;
    }

    return ret_value;
}

/*==============================================================================
 * Description:
 * get the parameter from isp to real gain
 * you mustn't change the funcion !
 *============================================================================*/
static cmr_u32 isp_to_real_gain(cmr_handle handle, cmr_u32 param) {
    cmr_u32 real_gain = 0;
    real_gain = param;

    return real_gain;
}

/*==============================================================================
 * Description:
 * write gain value to sensor
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int imx351_drv_write_gain_value(cmr_handle handle, cmr_uint param) {
    double real_gain = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    real_gain = (double)param/ ISP_BASE_GAIN * 1.0;

    SENSOR_LOGV("real_gain = %f", real_gain);

    sns_drv_cxt->sensor_ev_info.preview_gain = real_gain;
    imx351_drv_write_gain(handle, real_gain);

    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * increase gain or shutter for hdr
 *
 *============================================================================*/
static void imx351_drv_increase_hdr_exposure(cmr_handle handle,
                                             cmr_u8 ev_multiplier) {
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    cmr_u32 shutter_multiply = sns_drv_cxt->hdr_info.capture_max_shutter /
                               sns_drv_cxt->hdr_info.capture_shutter;
    cmr_u32 gain = 0;
    struct hdr_info_t *hdr_info = &sns_drv_cxt->hdr_info;

    if (0 == shutter_multiply)
        shutter_multiply = 1;

    if (shutter_multiply >= ev_multiplier) {
        imx351_drv_update_exposure(
            handle, hdr_info->capture_shutter * ev_multiplier, 0);
        imx351_drv_write_gain(handle, hdr_info->capture_gain);
    } else {
        gain = hdr_info->capture_gain * ev_multiplier / shutter_multiply;
        imx351_drv_update_exposure(
            handle, hdr_info->capture_shutter * shutter_multiply, 0);
        imx351_drv_write_gain(handle, gain);
    }
}

/*==============================================================================
 * Description:
 * decrease gain or shutter for hdr
 *
 *============================================================================*/
static void imx351_drv_decrease_hdr_exposure(cmr_handle handle,
                                             cmr_u8 ev_divisor) {
    cmr_u16 gain_multiply = 0;
    cmr_u32 shutter = 0;
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct hdr_info_t *hdr_info = &sns_drv_cxt->hdr_info;

    gain_multiply = hdr_info->capture_gain / SENSOR_BASE_GAIN;

    if (gain_multiply >= ev_divisor) {
        imx351_drv_update_exposure(handle, hdr_info->capture_shutter, 0);
        imx351_drv_write_gain(handle, hdr_info->capture_gain / ev_divisor);

    } else {
        shutter = hdr_info->capture_shutter * gain_multiply / ev_divisor;
        imx351_drv_update_exposure(handle, shutter, 0);
        imx351_drv_write_gain(handle, hdr_info->capture_gain / gain_multiply);
    }
}

/*==============================================================================
 * Description:
 * set hdr ev
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int imx351_drv_set_hdr_ev(cmr_handle handle, cmr_uint param) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_EXT_FUN_PARAM_T_PTR ext_ptr = (SENSOR_EXT_FUN_PARAM_T_PTR)param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(ext_ptr);

    cmr_u32 ev = ext_ptr->param;
    cmr_u8 ev_divisor, ev_multiplier;

    switch (ev) {
    case SENSOR_HDR_EV_LEVE_0:
        ev_divisor = 2;
        imx351_drv_decrease_hdr_exposure(handle, ev_divisor);
        break;
    case SENSOR_HDR_EV_LEVE_1:
        ev_multiplier = 2;
        imx351_drv_increase_hdr_exposure(handle, ev_multiplier);
        break;
    case SENSOR_HDR_EV_LEVE_2:
        ev_multiplier = 1;
        imx351_drv_increase_hdr_exposure(handle, ev_multiplier);
        break;
    default:
        break;
    }
    return ret;
}

/*==============================================================================
 * Description:
 * extra functoin
 * you can add functions reference SENSOR_EXT_FUNC_CMD_E which from
 *sensor_drv_u.h
 *============================================================================*/
static cmr_int imx351_drv_ext_func(cmr_handle handle, cmr_uint param) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_EXT_FUN_PARAM_T_PTR ext_ptr = (SENSOR_EXT_FUN_PARAM_T_PTR)param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(ext_ptr);

    SENSOR_LOGI("ext_ptr->cmd: %d", ext_ptr->cmd);
    switch (ext_ptr->cmd) {
    case SENSOR_EXT_EV:
        rtn = imx351_drv_set_hdr_ev(handle, param);
        break;
    default:
        break;
    }

    return rtn;
}

cmr_int imx351_drv_write_spcdata(cmr_handle handle, cmr_u8 *param) {
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_LOGI("E");
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u8 *param_ptr = (cmr_u8 *)param;
    // cmr_u8 param_ptr[8192] = {0x00,0x00};
    // hw_sensor_read_i2c(sns_drv_cxt->hw_handle, 0xA0 >> 1, param_ptr,8192 <<
    // 16 |SENSOR_I2C_REG_16BIT);
    // SENSOR_LOGI("otp_raw_data:%p",param_ptr);
    for (int i = 0; i < 70; i++) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x7500 + i,
                            param_ptr[0x08ba + i] & 0xFF);
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x7548 + i,
                            param_ptr[0x0900 + i] & 0xFF);
        // SENSOR_LOGV("E1  0x%02x 0x%02x",
        // param_ptr[0x08ba+i],hw_sensor_grc_read_i2c(sns_drv_cxt->hw_handle,
        //             0x34 >> 1, 0x7500 + i, BITS_ADDR16_REG8) );
    }
    return 0;
}
static cmr_int imx351_drv_set_pdaf_mode(cmr_handle handle, cmr_uint param) {
    UNUSED(param);
    SENSOR_LOGI("E 0x%x", param);

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    if (param == 0x02) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038a3, 0x02);
    } else if (param == 0x31) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038a3,
                            0x01); // 8x6 582x582
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038a8, 0x02); // 0x02
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038a9, 0x44); // 0x30
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038aa, 0x02); // 0x0d
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038ab, 0x40); // 0xa8
    } else if (param == 0x30 || param == 0x21) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038a3,
                            0x00); // 16x12 582x582
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038a8, 0x01); // 0x02
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038a9, 0x22); // 0x30
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038aa, 0x01); // 0x0d
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038ab, 0x20); // 0xa8
    } else if (param == 0x20) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038a3,
                            0x01); // 8x6 582x582
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038a8, 0x00); // 0x02
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038a9, 0x91); // 0x30
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038aa, 0x00); // 0x0d
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x038ab, 0x90); // 0xa8
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
/*==============================================================================
 * Description:
 * mipi stream on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int imx351_drv_stream_on(cmr_handle handle, cmr_uint param) {
    UNUSED(param);

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_LOGI("E");
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_uint sensor_mode = 0;
    sns_drv_cxt->ops_cb.get_mode(sns_drv_cxt->caller_handle, &sensor_mode);
    if (sensor_mode == 3)
        imx351_drv_set_pdaf_mode(handle, 0x31);
    else if (sensor_mode == 2)
        imx351_drv_set_pdaf_mode(handle, 0x21);
// imx351_drv_set_pdaf_mode(handle, 1);

#if 1
    char value1[PROPERTY_VALUE_MAX];
    property_get("debug.camera.test.mode", value1, "0");
    if (!strcmp(value1, "1")) {
        SENSOR_LOGI("enable test mode");
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0600, 0x00);
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0601, 0x02);
    }
#endif

#if 1
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0101, 0x03);
#endif
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x01);

    return 0;
}

/*==============================================================================
 * Description:
 * mipi stream off
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int imx351_drv_stream_off(cmr_handle handle, cmr_uint param) {
    SENSOR_LOGI("E");
    UNUSED(param);
    unsigned char value = 0;
    cmr_u16 sleep_time = 0, ret = 0;;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0100);
    if (value != 0x00) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x00);
        if (!sns_drv_cxt->is_sensor_close) {
            sleep_time = (sns_drv_cxt->sensor_ev_info.preview_framelength *
                        sns_drv_cxt->line_time_def / 1000000) + 10;
            ret = usleep(sleep_time * 1000);
            SENSOR_LOGI("stream_off delay_ms %d %d", sleep_time, ret);
        }
    } else {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x00);
    }
    sns_drv_cxt->is_sensor_close = 0;

    SENSOR_LOGI("X");
    return SENSOR_SUCCESS;
}

static cmr_int imx351_drv_get_static_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_ex_info *ex_info = PNULL;
    cmr_u32 up = 0, down = 0, i = 0;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    struct sensor_static_info *static_info = sns_drv_cxt->static_info;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;
    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;

    if (!static_info || !module_info) {
        SENSOR_LOGI("error:static_info:%p,module_info:%p", static_info,
                    module_info);
        rtn = SENSOR_FAIL;
        goto exit;
    }

    // make sure we have get max fps of all settings.
    if (!fps_info->is_init) {
        imx351_drv_init_fps_info(handle);
    }
    ex_info = (struct sensor_ex_info *)param;
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
    ex_info->name = (cmr_s8 *)MIPI_RAW_INFO.name;
    ex_info->sensor_version_info = (cmr_s8 *)MIPI_RAW_INFO.sensor_version_info;
    ex_info->pos_dis.up2hori = up;
    ex_info->pos_dis.hori2down = down;
    ex_info->embedded_line_enable = static_info->embedded_line_enable;
    memcpy(&ex_info->fov_info, &static_info->fov_info,
           sizeof(static_info->fov_info));
    ex_info->mono_sensor = static_info->mono_sensor;

exit:
    sensor_ic_print_static_info((cmr_s8 *)SENSOR_NAME, ex_info);
    return rtn;
}

static cmr_int imx351_drv_get_fps_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_MODE_FPS_T *fps_info;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_fps_info *fps_data = sns_drv_cxt->fps_info;
    SENSOR_IC_CHECK_PTR(fps_data);

    // make sure have inited fps of every sensor mode.
    if (!fps_data->is_init) {
        imx351_drv_init_fps_info(handle);
    }

    fps_info = (SENSOR_MODE_FPS_T *)param;
    cmr_u32 sensor_mode = fps_info->mode;
    fps_info->max_fps = fps_data->sensor_mode_fps[sensor_mode].max_fps;
    fps_info->min_fps = fps_data->sensor_mode_fps[sensor_mode].min_fps;
    fps_info->is_high_fps = fps_data->sensor_mode_fps[sensor_mode].is_high_fps;
    fps_info->high_fps_skip_num =
        fps_data->sensor_mode_fps[sensor_mode].high_fps_skip_num;
    SENSOR_LOGV("mode %d, max_fps: %d", fps_info->mode, fps_info->max_fps);
    SENSOR_LOGV("min_fps: %d", fps_info->min_fps);
    SENSOR_LOGV("is_high_fps: %d", fps_info->is_high_fps);
    SENSOR_LOGV("high_fps_skip_num: %d", fps_info->high_fps_skip_num);

    return rtn;
}
/*
static const cmr_u16 imx351_pd_is_right[] = {0, 0, 1, 1, 1, 1, 0, 0};

static const cmr_u16 imx351_pd_row[] = {5, 5, 8, 8, 21, 21, 24, 24};

static const cmr_u16 imx351_pd_col[] = {2, 18, 1, 17, 10, 26, 9, 25};
static const struct pd_pos_info _imx351_pd_pos_l[] = {
    {2, 5}, {18, 5}, {9, 24}, {25, 24},
};

static const struct pd_pos_info _imx351_pd_pos_r[] = {
    {1, 8}, {17, 8}, {10, 21}, {26, 21},
};*/
static const cmr_u32 imx351_pd_sns_mode[] = {
    SENSOR_PDAF_MODE_DISABLE, SENSOR_PDAF_MODE_DISABLE,
    SENSOR_PDAF_MODE_ENABLE, SENSOR_PDAF_MODE_ENABLE
};

struct sensor_pdaf_info imx351_pdaf_info_array[] = {
	/* pd setting for sensor mode0 */
	{0},
	/* pd setting for sensor mode1 */
	{0},
	/* pd setting for sensor mode2 */
    {
	.vch2_info.bypass = 0,
	.vch2_info.vch2_vc = 0,
	.vch2_info.vch2_data_type = 0x36,
	.vch2_info.vch2_mode = 0x01,
	.sns_mode = imx351_pd_sns_mode,
	.pd_size_w = SNAPSHOT_WIDTH,
	.pd_size_h = SNAPSHOT_HEIGHT,
	.image_process_type = 0, // 0:default; 1:only crop; 2:binning+crop;
	.crop_offset_x = 0,
	.crop_offset_y = 0,
    },
	
    /* pd setting for sensor mode3 */
    {
	.vch2_info.bypass = 0,
	.vch2_info.vch2_vc = 0,
	.vch2_info.vch2_data_type = 0x36,
	.vch2_info.vch2_mode = 0x01,
	.sns_mode = imx351_pd_sns_mode,
	.pd_size_w = SNAPSHOT_WIDTH,
	.pd_size_h = SNAPSHOT_HEIGHT,
	.image_process_type = 0, // 0:default; 1:only crop; 2:binning+crop;
	.crop_offset_x = 0,
	.crop_offset_y = 0,
	},

	/* add more pdaf setting here */
};

static struct sensor_pdaf_info_tab imx351_pdaf_info_tab = {
    .sns_mode_num = NUMBER_OF_ARRAY(imx351_pd_sns_mode),
	.pd_sns_mode = imx351_pd_sns_mode,
#if defined(CONFIG_CAMERA_SUPPORT_8M)
	.pdaf_init_index = 2,
#else
    .pdaf_init_index = 3,
#endif
	.pd_init_size_w = SNAPSHOT_WIDTH,
	.pd_init_size_h = SNAPSHOT_HEIGHT,
	.pd_info_tab = imx351_pdaf_info_array,
};

static cmr_int imx351_drv_get_pdaf_info(cmr_handle handle, void *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_pdaf_info_tab *pdaf_info_tab = NULL;
    struct sensor_pdaf_info *pdaf_info = NULL;
    cmr_int pd_info_num = 0;
    int i = 0;
    SENSOR_IC_CHECK_PTR(param);

    SENSOR_LOGV("E");

    pdaf_info_tab = (struct sensor_pdaf_info_tab *)param;
    
    pdaf_info_tab->sns_mode_num = imx351_pdaf_info_tab.sns_mode_num;
    pdaf_info_tab->pd_sns_mode = imx351_pdaf_info_tab.pd_sns_mode;
    pdaf_info_tab->pdaf_init_index = imx351_pdaf_info_tab.pdaf_init_index;
    pdaf_info_tab->pd_init_size_w = imx351_pdaf_info_tab.pd_init_size_w;
    pdaf_info_tab->pd_init_size_h = imx351_pdaf_info_tab.pd_init_size_h;
    pdaf_info_tab->pd_info_tab = imx351_pdaf_info_tab.pd_info_tab;

    pd_info_num = imx351_pdaf_info_tab.sns_mode_num;
    SENSOR_LOGE("sensor pdaf_init_index %d", pdaf_info_tab->pdaf_init_index);
    return rtn;
}


static cmr_int imx351_drv_set_spc_data(cmr_handle handle, cmr_u8 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    cmr_u8 *spc_data = (cmr_u8 *)param;
    SENSOR_LOGI("*param 0x%x 0x%x", *param, *spc_data);

    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    imx351_drv_write_spcdata(handle, spc_data);

    return rtn;
}
static  const cmr_u32 ebd_sns_mode[] = {0,1,1,1};

static cmr_int imx351_drv_get_ebdline_info(cmr_handle handle,
                                         cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_embedded_info *embedded_info = NULL;
    /*TODO*/
    if (param == NULL) {
        SENSOR_LOGE("null input");
        return -1;
    }

    embedded_info = (struct sensor_embedded_info *)param;
    embedded_info->vc_info.bypass = 0;
    embedded_info->vc_info.vch_id = 0;
    embedded_info->vc_info.vch_data_type = 0x12;
    embedded_info->vc_info.vch_mode = 0x01;
    embedded_info->sns_mode = ebd_sns_mode;
    SENSOR_LOGV("X");

    return rtn;
}

static cmr_int imx351_drv_parse_ebd_data(cmr_handle handle,
                                         cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_embedded_info *embedded_info = NULL;
    struct ebd_parse_data *parse_data = NULL;
    /*TODO*/
    if (param == NULL) {
        SENSOR_LOGE("null input");
        return -1;
    }

    embedded_info = (struct sensor_embedded_info *)param;
   if (embedded_info->embedded_data == NULL) {
        SENSOR_LOGE("embedded_data err");
        return -1;
    }
    parse_data = &embedded_info->parse_data;
    cmr_u8 *embedded_data = (cmr_u8 *)embedded_info->embedded_data;
    embedded_info->frame_count_valid = 1;
    embedded_info->shutter_valid= 1;
    embedded_info->again_valid=  1;
    embedded_info->dgain_valid = 1;
    parse_data->frame_count= *(embedded_data+6+ 6/4);
    parse_data->shutter = *(embedded_data+46+46/4) << 8 |*(embedded_data+48+48/4);
    parse_data->again =  *(embedded_data+50+50/4) << 8 | *(embedded_data+52+52/4);
    parse_data->dgain_gr =*(embedded_data+56+56/4) <<8 |*(embedded_data+58+58/4);
    parse_data->dgain_r = *(embedded_data+60+60/4) <<8 | *(embedded_data+62+62/4);
    parse_data->dgain_b =  *(embedded_data+64+64/4) << 8 |*(embedded_data+66+66/4);
    parse_data->dgain_gb =  *(embedded_data+68+68/4) << 8 | *(embedded_data+70+70/4);
    parse_data->gain =  (double)(1024.0/(1024.0-parse_data->again))*(double)(parse_data->dgain_r/256.0)*ISP_BASE_GAIN;
    SENSOR_LOGV("X frame_count %x shutter: %d again %x %x %x %x %x %d", parse_data->frame_count,
        parse_data->shutter, parse_data->again,parse_data->dgain_gr,parse_data->dgain_r,
        parse_data->dgain_b,parse_data->dgain_gb,parse_data->gain);

    return rtn;

}

static cmr_int imx351_drv_get_3dnr_threshold(cmr_handle handle, cmr_u8 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct threshold_3dnr *tdnr_thr = (struct threshold_3dnr *)param;
    tdnr_thr->threshold_3dnr_down = 410;
    tdnr_thr->threshold_3dnr_up = 510;
    SENSOR_LOGV("*param 0x%x 0x%x", *param, tdnr_thr->threshold_3dnr_down);

    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    return rtn;
}

static cmr_int imx351_drv_dump_trace(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    cmr_u16 value = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0005);
    SENSOR_LOGI("FRM_CNT 0x0005:0x%x", value);

    return rtn;
}


static cmr_int imx351_drv_access_val(cmr_handle handle, cmr_uint param) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_VAL_T *param_ptr = (SENSOR_VAL_T *)param;
    cmr_u16 tmp;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param_ptr);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGV("ptr:%p,type:0x%x", param_ptr, param_ptr->type);

    switch (param_ptr->type) {
    case SENSOR_VAL_TYPE_GET_STATIC_INFO:
        rtn = imx351_drv_get_static_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_FPS_INFO:
        rtn = imx351_drv_get_fps_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_SET_SENSOR_CLOSE_FLAG:
        sns_drv_cxt->is_sensor_close = 1;
        break;
    case SENSOR_VAL_TYPE_GET_PDAF_INFO:
        rtn = imx351_drv_get_pdaf_info(handle, param_ptr->pval);
        break;
     case SENSOR_VAL_TYPE_SET_OTP_DATA:
         rtn = imx351_drv_set_spc_data(handle, param_ptr->pval);
         break;
    case SENSOR_VAL_TYPE_GET_EBDLINE_INFO:
        rtn = imx351_drv_get_ebdline_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_3DNR_THRESHOLD:
        rtn = imx351_drv_get_3dnr_threshold(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_PARSE_EBD_DATA:
        rtn = imx351_drv_parse_ebd_data(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_DUMP_TRACE:
        rtn = imx351_drv_dump_trace(handle, param_ptr->pval);
        break;
    default:
        break;
    }

    SENSOR_LOGV("X");

    return rtn;
}

static cmr_u16 imx351_drv_calc_exposure(cmr_handle handle, cmr_u32 shutter,
                                        cmr_u32 dummy_line,
                                        struct sensor_aec_i2c_tag *aec_info) {
    cmr_u32 dest_fr_len = 0;
    cmr_u32 fr_len = 0;
    cmr_int offset = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(aec_info);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    fr_len = sns_drv_cxt->frame_length_def;

    if (dummy_line > FRAME_OFFSET)
        offset = dummy_line;
    else
        offset = FRAME_OFFSET;
    dest_fr_len = ((shutter + offset) > fr_len) ? (shutter + offset) : fr_len;

    sns_drv_cxt->frame_length = dest_fr_len;

    if (shutter < SENSOR_MIN_SHUTTER)
        shutter = SENSOR_MIN_SHUTTER;

    cmr_u16 frame_interval = 0x00;
    frame_interval = (cmr_u16)(
        ((shutter + dummy_line) * sns_drv_cxt->line_time_def) / 1000000);
    SENSOR_LOGD("exposure_line = %d, dummy_line= %d, frame_interval= %d ms",
                shutter, dummy_line, frame_interval);

    aec_info->frame_length->settings[0].reg_value = (dest_fr_len >> 8) & 0xff;
    aec_info->frame_length->settings[1].reg_value = dest_fr_len & 0xff;
    sns_drv_cxt->sensor_ev_info.preview_framelength = dest_fr_len;

    aec_info->shutter->settings[0].reg_value = (shutter >> 8) & 0xff;
    aec_info->shutter->settings[1].reg_value = shutter & 0xff;
    return shutter;
}

static void imx351_drv_calc_gain(double gain,
                                 struct sensor_aec_i2c_tag *aec_info) {
    cmr_u16 sensor_again = 0;
    SENSOR_IC_CHECK_PTR_VOID(aec_info);

    gain = gain / 32.0;

    if (gain > 16) {
        SENSOR_LOGD("real_gain:%f, over max_sensor_again 16.0", gain);
        gain = 16.0;
    } else {
        SENSOR_LOGD("real_gain:%f", gain);
    }

    sensor_again = (cmr_u16)(1024.0 - 1024.0 / gain);

    aec_info->again->settings[0].reg_value = (sensor_again >> 8) & 0xFF;
    aec_info->again->settings[1].reg_value = sensor_again & 0xFF;

}

static unsigned long imx351_drv_read_aec_info(cmr_handle handle,
                                              cmr_uint param) {
    unsigned long ret_value = SENSOR_SUCCESS;
    struct sensor_aec_reg_info *info = (struct sensor_aec_reg_info *)param;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 mode = 0x00;
    cmr_u32 exp_time = 0x00;
    double real_gain = 0;
    cmr_u32 gain = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(info);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    info->aec_i2c_info_out = &imx351_aec_info;

    exposure_line = info->exp.exposure;
    dummy_line = info->exp.dummy;
    mode = info->exp.size_index;
    exp_time = info->exp.exp_time;
    sns_drv_cxt->frame_length_def = sns_drv_cxt->trim_tab_info[mode].frame_line;

#if 1
    sns_drv_cxt->line_time_def = sns_drv_cxt->trim_tab_info[mode].line_time;
    sns_drv_cxt->sensor_ev_info.preview_shutter = imx351_drv_calc_exposure(
        handle, exposure_line, dummy_line, &imx351_aec_info);

    gain = info->gain < SENSOR_BASE_GAIN ? SENSOR_BASE_GAIN : info->gain;
    real_gain = (double)info->gain * SENSOR_BASE_GAIN / ISP_BASE_GAIN * 1.0;
    imx351_drv_calc_gain(real_gain, &imx351_aec_info);
#endif
    return ret_value;
}

static cmr_int
imx351_drv_handle_create(struct sensor_ic_drv_init_para *init_param,
                         cmr_handle *sns_ic_drv_handle) {
    cmr_int ret = SENSOR_SUCCESS;
    struct sensor_ic_drv_cxt *sns_drv_cxt = NULL;

    ret = sensor_ic_drv_create(init_param, sns_ic_drv_handle);
    if (ret != SENSOR_SUCCESS)
        return SENSOR_FAIL;

    sns_drv_cxt = (struct sensor_ic_drv_cxt *)*sns_ic_drv_handle;

    sns_drv_cxt->sensor_ev_info.preview_framelength = PREVIEW_FRAME_LENGTH;
    sns_drv_cxt->line_time_def = PREVIEW_LINE_TIME;

    sns_drv_cxt->sensor_ev_info.preview_exptime =
        (PREVIEW_FRAME_LENGTH - FRAME_OFFSET) * PREVIEW_LINE_TIME;

    sensor_ic_set_match_module_info(sns_drv_cxt, ARRAY_SIZE(MODULE_INFO),
                                    MODULE_INFO);
    sensor_ic_set_match_resolution_info(sns_drv_cxt, ARRAY_SIZE(RES_TAB_RAW),
                                        RES_TAB_RAW);
    sensor_ic_set_match_trim_info(sns_drv_cxt, ARRAY_SIZE(RES_TRIM_TAB),
                                  RES_TRIM_TAB);
    sensor_ic_set_match_static_info(sns_drv_cxt, ARRAY_SIZE(STATIC_INFO),
                                    STATIC_INFO);
    sensor_ic_set_match_fps_info(sns_drv_cxt, ARRAY_SIZE(FPS_INFO), FPS_INFO);

    /*add private here*/

    return ret;
}

static cmr_int imx351_drv_handle_delete(cmr_handle handle, void *param) {
    cmr_int ret = SENSOR_SUCCESS;
    /*if has private data,you must release it here*/

    ret = sensor_ic_drv_delete(handle, param);
    return ret;
}

static cmr_int imx351_drv_get_private_data(cmr_handle handle, cmr_uint cmd,
                                           void **param) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);

    ret = sensor_ic_get_private_data(handle, cmd, param);
    return ret;
}

void *sensor_ic_open_lib(void)
{
     return &g_imx351_mipi_raw_info;
}

/*==============================================================================
 * Description:
 * all ioctl functoins
 * you can add functions reference SENSOR_IOCTL_FUNC_TAB_T from sensor_drv_u.h
 *
 * add ioctl functions like this:
 *============================================================================*/
static struct sensor_ic_ops s_imx351_ops_tab = {
    .create_handle = imx351_drv_handle_create,
    .delete_handle = imx351_drv_handle_delete,
    /*get privage data*/
    .get_data = imx351_drv_get_private_data,
    /*common interface*/
    .power = imx351_drv_power_on,
    .identify = imx351_drv_identify,
    .write_gain_value = imx351_drv_write_gain_value,
    .read_aec_info = imx351_drv_read_aec_info,
    .ex_write_exp = imx351_drv_ex_write_exposure,
    .ext_ops = {
            [SENSOR_IOCTL_BEFORE_SNAPSHOT].ops = imx351_drv_before_snapshot,
            [SENSOR_IOCTL_STREAM_ON].ops = imx351_drv_stream_on,
            [SENSOR_IOCTL_STREAM_OFF].ops = imx351_drv_stream_off,
            [SENSOR_IOCTL_ACCESS_VAL].ops = imx351_drv_access_val,
    }};
