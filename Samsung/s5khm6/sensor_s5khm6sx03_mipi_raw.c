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

#include "sensor_s5khm6sx03_mipi_raw.h"
#define LOG_TAG "s5khm6sx03_qt_hulk"

#define RES_TAB_RAW s_s5khm6sx_resolution_Tab_RAW
#define RES_TRIM_TAB s_s5khm6sx_Resolution_Trim_Tab
#define STATIC_INFO s_s5khm6sx_static_info
#define FPS_INFO s_s5khm6sx_mode_fps_info
#define MIPI_RAW_INFO g_s5khm6sx_mipi_raw_info
#define MODULE_INFO s_s5khm6sx_module_info_tab

static struct sensor_ic_drv_cxt *s5khm6sx_sns_drv_cxt = NULL;

struct fast_switch_ae_info {
    cmr_u32 sensor_mode;
    cmr_u32 fast_switch_mode;
    cmr_u32 line_time_ns;
    cmr_u32 exposure_line;
    cmr_u32 frame_line;
    cmr_u32 again;
    cmr_u32 dgain;
    cmr_u64 exposure_time_ns;
};

struct hw_drv_cxt {
    /*sensor device file descriptor*/
    cmr_s32 fd_sensor;
    /*sensor_id will be used mipi init*/
    cmr_u32 sensor_id;
    /*0-bit:reg value width,1-bit:reg addre width*/
    /*5,6,7-bit:i2c frequency*/
    cmr_u8 i2c_bus_config;
    /*sensor module handle,will be deleted later*/
    cmr_handle caller_handle; /**/
    /**/
    cmr_u8 i2c_burst_mode;
};

struct fsm_ae_info {
    struct fast_switch_ae_info current;
    struct fast_switch_ae_info target;
};

static struct fsm_ae_info s5khm6sx_fsm_ae_info = {0};

static cmr_int s5khm6sx_drv_init_fps_info(cmr_handle handle) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("E");
    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
    struct sensor_trim_tag *trim_info = sns_drv_cxt->trim_tab_info;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;

    if (!fps_info->is_init) {
        uint32_t i, modn, tempfps = 0;
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

/*
    UNFLIPPED
 */

static cmr_u8 s5khm6sx_snspid_is_init = 0;

static cmr_int s5khm6sx_drv_save_snspid(cmr_handle handle) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u8 snspid[32] = {0};
    cmr_u16 snspid_size = 32; // sensor_drv_u.c sensor_snspid max-size is 32byte
    struct hw_drv_cxt *hw_drv_cxt =
        (struct hw_drv_cxt *)(sns_drv_cxt->hw_handle);
    struct module_cfg_info *sensor_module_info_ptr =
        &(s_s5khm6sx_module_info_tab[0].module_info);
    SENSOR_LOGI("E");

    // switch hw to otp module
    hw_drv_cxt->i2c_bus_config =
        SENSOR_I2C_REG_16BIT | SENSOR_I2C_VAL_8BIT | SENSOR_I2C_FREQ_400;
    hw_sensor_i2c_set_addr(hw_drv_cxt, 0x50);
    for (int i = 0; i < 23; i++) {
        snspid[i] = hw_sensor_read_reg(hw_drv_cxt, 0x0C6F + i);
    }
    SENSOR_LOGI("SN code is %s", snspid);

    // reset hw to sensor module
    hw_drv_cxt->i2c_bus_config = sensor_module_info_ptr->reg_addr_value_bits;
    hw_sensor_i2c_set_addr(hw_drv_cxt, sensor_module_info_ptr->major_i2c_addr);

    if (sns_drv_cxt->ops_cb.set_snspid) {
        sns_drv_cxt->ops_cb.set_snspid(sns_drv_cxt->caller_handle,
                                       sns_drv_cxt->sensor_id, snspid,
                                       snspid_size);
    }

    s5khm6sx_snspid_is_init = 1;
    return SENSOR_SUCCESS;
}

static cmr_int s5khm6sx_drv_identify(cmr_handle handle, cmr_int param) {
    cmr_u16 pid_value, short_setting = 0x00;
    cmr_int ret_value = SENSOR_FAIL;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    pid_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, S5KHM6SX_PID_ADDR);

    if (S5KHM6SX_PID_VALUE == pid_value) {
        SENSOR_LOGI("this is s5khm6sx sensor!");
        ret_value = SENSOR_SUCCESS;
        s5khm6sx_drv_init_fps_info(handle);
        if (!s5khm6sx_snspid_is_init) {
            s5khm6sx_drv_save_snspid(handle);
        }
    } else {
        SENSOR_LOGI("sensor identify fail, pid_value=0x%x", pid_value);
    }
    short_setting = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0X0002);
    SENSOR_LOGI("X:short_setting reg_value 0x%x", short_setting);

    return ret_value;
}

static cmr_int s5khm6sx_drv_write_exp_dummy(cmr_handle handle,
                                            cmr_u16 expsure_line,
                                            cmr_u16 dummy_line,
                                            cmr_u16 size_index) {
    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u32 frame_len_cur = 0x00;
    cmr_u32 frame_len = 0x00;
    cmr_u32 max_frame_len = 0x00;
    cmr_u32 linetime = 0;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("exposure_line:%d, dummy:%d, size_index:%d", expsure_line,
                dummy_line, size_index);
    max_frame_len = sns_drv_cxt->trim_tab_info[size_index].frame_line;
    if (expsure_line < 3) {
        expsure_line = 3;
    }

    frame_len = expsure_line + dummy_line;
    frame_len = (frame_len > (uint32_t)(expsure_line + 5))
                    ? frame_len
                    : (uint32_t)(expsure_line + 5);
    frame_len = (frame_len > max_frame_len) ? frame_len : max_frame_len;
    if (0x00 != (0x01 & frame_len)) {
        frame_len += 0x01;
    }

    frame_len_cur = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0340);

    if (frame_len_cur != frame_len) {
        ret_value =
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0340, frame_len);
    }
    sns_drv_cxt->sensor_ev_info.preview_framelength = frame_len;

    ret_value =
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x202, expsure_line);

    sns_drv_cxt->exp_line = expsure_line;
    linetime = sns_drv_cxt->trim_tab_info[size_index].line_time;
    if (sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle,
                                          SENSOR_EXIF_CTRL_EXPOSURETIME,
                                          expsure_line);
    } else {
        /*sns_drv_cxt->exif_info.exposure_time = expsure_line;*/
        sns_drv_cxt->exif_info.exposure_time = expsure_line * linetime / 1000;
    }

    return ret_value;
}
static cmr_int s5khm6sx_drv_write_exposure(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u32 expsure_line = 0x00;
    cmr_u32 dummy_line = 0x00;
    cmr_u32 size_index = 0x00;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    expsure_line = param & 0xffff;
    dummy_line = (param >> 0x10) & 0x0fff;
    size_index = (param >> 0x1c) & 0x0f;

    SENSOR_LOGI("write_exposure line:%d, dummy:%d, size_index:%d", expsure_line,
                dummy_line, size_index);

    ret_value = s5khm6sx_drv_write_exp_dummy(handle, expsure_line, dummy_line,
                                             size_index);

    sns_drv_cxt->sensor_ev_info.preview_shutter = sns_drv_cxt->exp_line;

    return ret_value;
}

static cmr_int s5khm6sx_drv_ex_write_exposure(cmr_handle handle,
                                              cmr_int param) {
    cmr_int ret = SENSOR_SUCCESS;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 size_index = 0x00;
    struct sensor_ex_exposure *ex = (struct sensor_ex_exposure *)param;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_trim_tag *trim_info = sns_drv_cxt->trim_tab_info;

    exposure_line = ex->exposure;
    dummy_line = ex->dummy;
    size_index = ex->size_index;

    sns_drv_cxt->frame_length_def = trim_info[size_index].frame_line;
    sns_drv_cxt->line_time_def = trim_info[size_index].line_time;

    ret = s5khm6sx_drv_write_exp_dummy(handle, exposure_line, dummy_line,
                                       size_index);

    sns_drv_cxt->sensor_ev_info.preview_shutter = sns_drv_cxt->exp_line;

    return ret;
}

static cmr_int s5khm6sx_drv_update_gain(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;
    uint32_t real_gain = 0;
    uint32_t a_gain = 0;
    uint32_t d_gain = 0;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    real_gain = param;

    SENSOR_LOGI("real_gain:%d, param: %ld", real_gain, param);

    if (real_gain <= 16 * 32) {
        a_gain = real_gain;
        d_gain = 256;
    } else {
        a_gain = 16 * 32;
        d_gain = 256.0 * real_gain / a_gain;
        SENSOR_LOGI("real_gain:0x%x, a_gain: 0x%x, d_gain: 0x%x",
                    (uint32_t)real_gain, (uint32_t)a_gain, (uint32_t)d_gain);
        if ((uint32_t)d_gain > 256 * 256)
            d_gain = 256 * 256; // d_gain < 256x
    }

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x204, a_gain);

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x20e, d_gain);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x210, d_gain);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x212, d_gain);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x214, d_gain);

    SENSOR_LOGI("a_gain:0x%x, d_gain: 0x%x", a_gain, d_gain);

    return ret_value;
}

static cmr_int s5khm6sx_drv_write_gain(cmr_handle handle, cmr_uint param) {
    cmr_int ret = SENSOR_SUCCESS;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    SENSOR_LOGI("param: %ld", param);
    cmr_uint gain = param < SENSOR_BASE_GAIN ? SENSOR_BASE_GAIN : param;
    cmr_uint real_gain = param * SENSOR_BASE_GAIN / ISP_BASE_GAIN * 1.0;

    ret = s5khm6sx_drv_update_gain(handle, real_gain);
    sns_drv_cxt->sensor_ev_info.preview_gain = real_gain;

    return ret;
}

static cmr_int s5khm6sx_drv_read_gain(cmr_handle handle, cmr_u32 param) {
    cmr_int rtn = SENSOR_SUCCESS;
    cmr_u32 again = 0;
    cmr_u32 dgain = 0;
    cmr_u32 gain = 0;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    again = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0204);
    dgain = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0210);
    gain = again * dgain;

    SENSOR_LOGI("gain: 0x%x", gain);
    return rtn;
}

static uint16_t s5khm6sx_drv_get_shutter(cmr_handle handle) {
    uint16_t shutter;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    shutter = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0202) & 0xffff;

    return shutter;
}

static cmr_int s5khm6sx_drv_set_awb_gain(cmr_handle handle, void *awb_param) {
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_awb_gain_info *awb_info =
        (struct sensor_awb_gain_info *)awb_param;
    cmr_int ret = SENSOR_SUCCESS;
    sns_drv_cxt->awb_gain.b = awb_info->b;
    sns_drv_cxt->awb_gain.r = awb_info->r;
    sns_drv_cxt->awb_gain.g = awb_info->g;
    SENSOR_LOGV("Ispalg AWB Gain is [R: %d, G: %d, B: %d]", awb_info->r,
                awb_info->g, awb_info->b);
    return ret;
}

static cmr_int s5khm6sx_drv_write_awb_gain(cmr_handle handle) {
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_int ret = SENSOR_SUCCESS;
    cmr_u32 R, G, B;
    float AWBGainRed, AWBGainGreen, AWBGainBlue;
    float wbMaxGain = 3.984375;
    float awbBase_isp = 1024.0;
    float awbBase_sensor = 1024.0;

    /* AWB algorithm of platform is 1024 - 16384 as 1X to 16X */
    AWBGainRed = sns_drv_cxt->awb_gain.r / awbBase_isp;
    AWBGainGreen = sns_drv_cxt->awb_gain.g / awbBase_isp;
    AWBGainBlue = sns_drv_cxt->awb_gain.b / awbBase_isp;
    if (AWBGainRed > wbMaxGain) {
        AWBGainRed = wbMaxGain;
    }
    if (AWBGainGreen > wbMaxGain) {
        AWBGainGreen = wbMaxGain;
    }
    if (AWBGainBlue > wbMaxGain) {
        AWBGainBlue = wbMaxGain;
    }

    R = (cmr_u32)(AWBGainRed * awbBase_sensor);
    G = (cmr_u32)(AWBGainGreen * awbBase_sensor);
    B = (cmr_u32)(AWBGainBlue * awbBase_sensor);
    SENSOR_LOGD("Sensor AWB Gain is [R: %d, G: %d, B: %d]", R, G, B);

    if (R > 0 && G > 0 && B > 0) {
        ret |= hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0D82, R);
        ret |= hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0D84, G);
        ret |= hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0D86, B);
    } else {
        SENSOR_LOGE("Invalid Sensor AWB Gain!");
    }
    return ret;
}

static cmr_int s5khm6sx_drv_before_snapshot(cmr_handle handle, cmr_int param) {
    cmr_u8 ret_l, ret_m, ret_h;
    cmr_u32 capture_exposure, preview_maxline;
    cmr_u32 capture_maxline, preview_exposure;
    cmr_u32 capture_mode = param & 0xffff;
    cmr_u32 preview_mode = (param >> 0x10) & 0xffff;
    cmr_u16 exposure_line = 0;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    cmr_u32 prv_linetime = sns_drv_cxt->trim_tab_info[preview_mode].line_time;
    cmr_u32 cap_linetime = sns_drv_cxt->trim_tab_info[capture_mode].line_time;
    cmr_u32 frame_len = 0x00;
    cmr_u32 gain = 0;

    SENSOR_LOGI("mode: 0x%08lx,capture_mode:%d", param, capture_mode);

    if (preview_mode == capture_mode) {
        SENSOR_LOGI("prv mode equal to capmode");
        goto CFG_INFO;
    }

    if (sns_drv_cxt->ops_cb.set_mode)
        sns_drv_cxt->ops_cb.set_mode(sns_drv_cxt->caller_handle, capture_mode);
    if (sns_drv_cxt->ops_cb.set_mode_wait_done)
        sns_drv_cxt->ops_cb.set_mode_wait_done(sns_drv_cxt->caller_handle);

    preview_exposure = sns_drv_cxt->sensor_ev_info.preview_shutter;
    gain = sns_drv_cxt->sensor_ev_info.preview_gain;

    capture_exposure =
        preview_exposure * prv_linetime / cap_linetime * BINNING_FACTOR;

    SENSOR_LOGI("prev_exp=%d,cap_exp=%d,gain=%d", preview_exposure,
                capture_exposure, gain);

    s5khm6sx_drv_write_exp_dummy(handle, capture_exposure, 0, capture_mode);
    s5khm6sx_drv_update_gain(handle, gain);

CFG_INFO:
    exposure_line = s5khm6sx_drv_get_shutter(handle);

    if (sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle,
                                          SENSOR_EXIF_CTRL_EXPOSURETIME,
                                          exposure_line);
        sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle,
                                          SENSOR_EXIF_CTRL_APERTUREVALUE, 20);
        sns_drv_cxt->ops_cb.set_exif_info(
            sns_drv_cxt->caller_handle, SENSOR_EXIF_CTRL_MAXAPERTUREVALUE, 20);
        sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle,
                                          SENSOR_EXIF_CTRL_FNUMBER, 20);
    } else {
        sns_drv_cxt->exif_info.exposure_line = exposure_line;
        sns_drv_cxt->exif_info.aperture_value = 29;
        sns_drv_cxt->exif_info.max_aperture_value = 20;
        sns_drv_cxt->exif_info.numerator = 20;
    }
    sns_drv_cxt->exp_line = exposure_line;
    sns_drv_cxt->exp_time = exposure_line * cap_linetime / 1000;

    return SENSOR_SUCCESS;
}

static cmr_int s5khm6sx_drv_init_exif_info(cmr_handle handle,
                                           void **exif_info_in /*in*/) {
    cmr_int ret = SENSOR_FAIL;
    EXIF_SPEC_PIC_TAKING_COND_T *exif_ptr = NULL;
    *exif_info_in = NULL;
    SENSOR_IC_CHECK_HANDLE(handle);

    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    ret = sensor_ic_get_init_exif_info(sns_drv_cxt, &exif_ptr);
    SENSOR_IC_CHECK_PTR(exif_ptr);
    *exif_info_in = exif_ptr;

    SENSOR_LOGI("Start");
    /*aperture = numerator/denominator */
    /*fnumber = numerator/denominator */
    exif_ptr->valid.FNumber = 1;
    exif_ptr->FNumber.numerator = 14;
    exif_ptr->FNumber.denominator = 5;

    exif_ptr->valid.ApertureValue = 1;
    exif_ptr->ApertureValue.numerator = 14;
    exif_ptr->ApertureValue.denominator = 5;
    exif_ptr->valid.MaxApertureValue = 1;
    exif_ptr->MaxApertureValue.numerator = 14;
    exif_ptr->MaxApertureValue.denominator = 5;
    exif_ptr->valid.FocalLength = 1;
    exif_ptr->FocalLength.numerator = 289;
    exif_ptr->FocalLength.denominator = 100;

    exif_ptr->ExposureTime.denominator = 1000000;

    return ret;
}

static void s5khm6sx_calc_gain(float gain,
                               struct sensor_aec_i2c_tag *aec_info) {
    cmr_u32 ret_value = SENSOR_SUCCESS;
    cmr_u16 value = 0x00;
    float real_gain = gain;
    float a_gain = 0;
    float d_gain = 0;
    uint16_t i = 0;

    if ((cmr_u32)real_gain <= 16 * 32) {
        a_gain = real_gain;
        d_gain = 256;
    } else {
        a_gain = 16 * 32;
        d_gain = 256.0 * real_gain / a_gain;

        if ((cmr_u32)d_gain > 256 * 256)
            d_gain = 256 * 256; // d_gain < 256x
    }
    SENSOR_LOGI("real_gain:0x%x, a_gain: 0x%x, d_gain: 0x%x",
                (cmr_u32)real_gain, (cmr_u32)a_gain, (cmr_u32)d_gain);
    SENSOR_LOGD("real_gain:%d, a_gain: %d, d_gain: %d", (cmr_u32)real_gain,
                (cmr_u32)a_gain, (cmr_u32)d_gain);

    aec_info->again->settings[0].reg_value = (cmr_u16)a_gain;
    for (i = 0; i < aec_info->dgain->size; i++)
        aec_info->dgain->settings[i].reg_value = (cmr_u16)d_gain;

    s5khm6sx_fsm_ae_info.current.again = (cmr_u32)a_gain;
    s5khm6sx_fsm_ae_info.current.dgain = (cmr_u32)d_gain;
}

static cmr_u16 s5khm6sx_read_frame_length(cmr_handle handle) {
    cmr_u16 frame_length = 0;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    frame_length = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0340);
    sns_drv_cxt->sensor_ev_info.preview_framelength = frame_length;

    return frame_length;
}

static cmr_u16 s5khm6sx_calc_exposure(cmr_handle handle, cmr_u32 shutter,
                                      cmr_u32 dummy_line, cmr_u16 mode,
                                      struct sensor_aec_i2c_tag *aec_info) {
    cmr_u32 dest_fr_len = 0;
    cmr_u32 cur_fr_len = 0;
    cmr_u32 fr_len = 0;
    cmr_int offset = 0;
    float fps = 0.0;
    float line_time = 0.0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    fr_len = sns_drv_cxt->frame_length_def;

    dummy_line = dummy_line > FRAME_OFFSET ? dummy_line : FRAME_OFFSET;
    dest_fr_len =
        ((shutter + dummy_line) > fr_len) ? (shutter + dummy_line) : fr_len;
    sns_drv_cxt->frame_length = dest_fr_len;

    cur_fr_len = s5khm6sx_read_frame_length(handle);

    if (shutter < SENSOR_MIN_SHUTTER)
        shutter = SENSOR_MIN_SHUTTER;
    line_time = sns_drv_cxt->trim_tab_info[mode].line_time;
    if (cur_fr_len > shutter) {
        fps = 1000000000.0 / (cur_fr_len * line_time);
    } else {
        fps = 1000000000.0 / ((shutter + dummy_line) * line_time);
    }
    SENSOR_LOGI("fps = %f", fps);

    aec_info->frame_length->settings[0].reg_value = dest_fr_len;
    aec_info->shutter->settings[0].reg_value = shutter;

    s5khm6sx_fsm_ae_info.current.line_time_ns = line_time;
    s5khm6sx_fsm_ae_info.current.exposure_line = shutter;
    s5khm6sx_fsm_ae_info.current.frame_line = dest_fr_len;
    s5khm6sx_fsm_ae_info.current.exposure_time_ns = line_time * shutter;

    return shutter;
}

static cmr_int s5khm6sx_read_aec_info(cmr_handle handle, void *param) {
    cmr_int ret = SENSOR_SUCCESS;
    struct sensor_aec_reg_info *info = (struct sensor_aec_reg_info *)param;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 mode = 0x00;
    float real_gain = 0;
    cmr_u32 gain = 0;
    cmr_u16 frame_interval = 0x00;
    cmr_u64 exposure_time = 0;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_trim_tag *trim_info = sns_drv_cxt->trim_tab_info;

    info->aec_i2c_info_out = &s5khm6sx_aec_info;
    exposure_line = info->exp.exposure;
    dummy_line = info->exp.dummy;
    mode = info->exp.size_index;
    exposure_time = info->exp.exp_time;

    frame_interval = (cmr_u16)(
        ((exposure_line + dummy_line) * (trim_info[mode].line_time)) / 1000000);
    exposure_time = exposure_time ? exposure_time
                                  : exposure_line * (trim_info[mode].line_time);
    exposure_time = exposure_time / 1000000;
    SENSOR_LOGI("mode = %d, exposure_line = %d, dummy_line = %d, exposure_time "
                "= %lld ms, frame_interval = %d ms",
                mode, exposure_line, dummy_line, exposure_time, frame_interval);

    sns_drv_cxt->frame_length_def = trim_info[mode].frame_line;
    sns_drv_cxt->line_time_def = trim_info[mode].line_time;

    sns_drv_cxt->sensor_ev_info.preview_shutter = s5khm6sx_calc_exposure(
        handle, exposure_line, dummy_line, mode, &s5khm6sx_aec_info);

    SENSOR_LOGI("isp aec gain_in %d", info->gain);
    gain = info->gain < SENSOR_BASE_GAIN ? SENSOR_BASE_GAIN : info->gain;
    real_gain = (float)info->gain * SENSOR_BASE_GAIN / ISP_BASE_GAIN * 1.0;
    sns_drv_cxt->sensor_ev_info.preview_gain = real_gain;
    s5khm6sx_calc_gain(real_gain, &s5khm6sx_aec_info);
    return ret;
}

static cmr_int s5khm6sx_drv_fast_switch_ae(cmr_handle handle,
                                           enum sensor_mode mode_after) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_reg_tab_info_tag *res_info = sns_drv_cxt->res_tab_info;
    struct sensor_trim_tag *trim_info = sns_drv_cxt->trim_tab_info;

    char ae_ratio[PROPERTY_VALUE_MAX] = {0};
    float again_before, dgain_before, again_after, dgain_after = 0.0;

    s5khm6sx_fsm_ae_info.target.sensor_mode = mode_after;
    s5khm6sx_fsm_ae_info.target.fast_switch_mode =
        res_info[mode_after].fast_switch_mode;
    s5khm6sx_fsm_ae_info.target.line_time_ns = trim_info[mode_after].line_time;
    again_before = (float)s5khm6sx_fsm_ae_info.current.again;
    dgain_before = (float)s5khm6sx_fsm_ae_info.current.dgain;

    SENSOR_LOGI("before:mode %d, linetime %d ns, exposure_line %d, "
                "frame_length %d, exposure_time %lld ns, again %d, dgain %d",
                s5khm6sx_fsm_ae_info.current.sensor_mode,
                s5khm6sx_fsm_ae_info.current.line_time_ns,
                s5khm6sx_fsm_ae_info.current.exposure_line,
                s5khm6sx_fsm_ae_info.current.frame_line,
                s5khm6sx_fsm_ae_info.current.exposure_time_ns,
                s5khm6sx_fsm_ae_info.current.again,
                s5khm6sx_fsm_ae_info.current.dgain);
    property_get("persist.vendor.cam.fast.switch.ae", ae_ratio, "gain");
    if (!strcmp(ae_ratio, "gain")) {
        s5khm6sx_fsm_ae_info.target.exposure_time_ns =
            s5khm6sx_fsm_ae_info.current.exposure_time_ns;
        s5khm6sx_fsm_ae_info.target.exposure_line =
            s5khm6sx_fsm_ae_info.target.exposure_time_ns /
            s5khm6sx_fsm_ae_info.target.line_time_ns;
        s5khm6sx_fsm_ae_info.target.frame_line =
            s5khm6sx_fsm_ae_info.target.exposure_line + FRAME_OFFSET;
        if (s5khm6sx_fsm_ae_info.target.fast_switch_mode ==
            SENSOR_FAST_SWITCH_GROUP1_MODE0_S5KHM6_12M_9IN1_BINNING) {
            // target mode0, from mode1
            again_after = again_before / BINNING_FACTOR;
        } else if (s5khm6sx_fsm_ae_info.target.fast_switch_mode ==
                   SENSOR_FAST_SWITCH_GROUP1_MODE1_S5KHM6_12M_CROP_FROM_108M) {
            // target mode1, from mode0
            again_after = again_before * BINNING_FACTOR;
        } else {
            SENSOR_LOGE("wrong fast switch mode %d",
                        s5khm6sx_fsm_ae_info.target.fast_switch_mode);
        }
        if ((cmr_u32)again_after <= 16 * SENSOR_BASE_GAIN) {
            s5khm6sx_fsm_ae_info.target.again = (cmr_u32)again_after;
            dgain_after = 256;
            s5khm6sx_fsm_ae_info.target.dgain = (cmr_u32)dgain_after;
        } else {
            s5khm6sx_fsm_ae_info.target.again = 16 * SENSOR_BASE_GAIN;
            dgain_after =
                256.0 * again_after / s5khm6sx_fsm_ae_info.target.again;

            if ((cmr_u32)dgain_after > 256 * 256) {
                dgain_after = 256 * 256; // d_gain < 256x
            }
            s5khm6sx_fsm_ae_info.target.dgain = (cmr_u32)dgain_after;
        }
    } else if (!strcmp(ae_ratio, "exposure")) {
        if (s5khm6sx_fsm_ae_info.target.fast_switch_mode ==
            SENSOR_FAST_SWITCH_GROUP1_MODE0_S5KHM6_12M_9IN1_BINNING) {
            // target mode0, from mode1
            s5khm6sx_fsm_ae_info.target.exposure_time_ns =
                s5khm6sx_fsm_ae_info.current.exposure_time_ns / BINNING_FACTOR;
        } else if (s5khm6sx_fsm_ae_info.target.fast_switch_mode ==
                   SENSOR_FAST_SWITCH_GROUP1_MODE1_S5KHM6_12M_CROP_FROM_108M) {
            // target mode1, from mode0
            s5khm6sx_fsm_ae_info.target.exposure_time_ns =
                s5khm6sx_fsm_ae_info.current.exposure_time_ns * BINNING_FACTOR;
        } else {
            SENSOR_LOGE("wrong fast switch mode %d",
                        s5khm6sx_fsm_ae_info.target.fast_switch_mode);
        }
        s5khm6sx_fsm_ae_info.target.exposure_line =
            s5khm6sx_fsm_ae_info.target.exposure_time_ns /
            s5khm6sx_fsm_ae_info.target.line_time_ns;
        s5khm6sx_fsm_ae_info.target.frame_line =
            s5khm6sx_fsm_ae_info.target.exposure_line + FRAME_OFFSET;
        s5khm6sx_fsm_ae_info.target.again = s5khm6sx_fsm_ae_info.current.again;
        s5khm6sx_fsm_ae_info.target.dgain = s5khm6sx_fsm_ae_info.current.dgain;
    } else {
        SENSOR_LOGE("wrong fast switch ae ratio base");
    }
    SENSOR_LOGI("after:mode %d, linetime %d ns, exposure_line %d, frame_length "
                "%d, exposure_time %lld ns, again %d, dgain %d",
                s5khm6sx_fsm_ae_info.target.sensor_mode,
                s5khm6sx_fsm_ae_info.target.line_time_ns,
                s5khm6sx_fsm_ae_info.target.exposure_line,
                s5khm6sx_fsm_ae_info.target.frame_line,
                s5khm6sx_fsm_ae_info.target.exposure_time_ns,
                s5khm6sx_fsm_ae_info.target.again,
                s5khm6sx_fsm_ae_info.target.dgain);
    return ret;
}

static cmr_int s5khm6sx_drv_fast_switch(cmr_handle handle, cmr_uint param) {

    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_reg_tab_info_tag *res_info = sns_drv_cxt->res_tab_info;

    SENSOR_FAST_SWITCH_E fsm = (SENSOR_FAST_SWITCH_E)param;
    enum sensor_mode mode_before = sns_drv_cxt->sensor_mode;
    enum sensor_mode mode_after = SENSOR_MODE_COMMON_INIT;

    if (fsm != SENSOR_FAST_SWITCH_GROUP1_MODE0_S5KHM6_12M_9IN1_BINNING &&
        fsm != SENSOR_FAST_SWITCH_GROUP1_MODE1_S5KHM6_12M_CROP_FROM_108M) {
        SENSOR_LOGE("wrong fast switch mode %d", fsm);
        ret = -1;
        return ret;
    }

    if (fsm == res_info[mode_before].fast_switch_mode) {
        mode_after = mode_before;
        SENSOR_LOGI("sensor current mode is target mode %d, don't need switch",
                    mode_before);
        ret = 0;
        return ret;
    } else {
        for (int i = SENSOR_MODE_PREVIEW_ONE; i < SENSOR_MODE_MAX; i++) {
            if (fsm == res_info[i].fast_switch_mode) {
                mode_after = i;
                break;
            }
        }
        if (mode_after == SENSOR_MODE_COMMON_INIT) {
            SENSOR_LOGE("can't find target sensor mode, fsm %d", fsm);
            ret = -1;
            return ret;
        }
    }
    s5khm6sx_fsm_ae_info.current.sensor_mode = mode_before;
    s5khm6sx_fsm_ae_info.current.fast_switch_mode =
        res_info[mode_before].fast_switch_mode;
    // s5khm6sx_drv_fast_switch_ae(handle, mode_after);

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0104,
                        0x0101); // group_hold_on

    if (fsm == SENSOR_FAST_SWITCH_GROUP1_MODE0_S5KHM6_12M_9IN1_BINNING) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0b30, 0x0100); // mode0
    } else if (fsm ==
               SENSOR_FAST_SWITCH_GROUP1_MODE1_S5KHM6_12M_CROP_FROM_108M) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0b30, 0x0101); // mode1
    } else {
        SENSOR_LOGE("wrong fast switch mode %d", fsm);
        ret = -1;
        return ret;
    }
#if 0
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0340, s5khm6sx_fsm_ae_info.target.frame_line); // frame_length
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0202, s5khm6sx_fsm_ae_info.target.exposure_line); // exposure_line
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0204, s5khm6sx_fsm_ae_info.target.again); // again

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x020e, s5khm6sx_fsm_ae_info.target.dgain); // dgain
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0210, s5khm6sx_fsm_ae_info.target.dgain); // dgain
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0212, s5khm6sx_fsm_ae_info.target.dgain); // dgain
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0214, s5khm6sx_fsm_ae_info.target.dgain); // dgain
#endif
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0104,
                        0x0001); // group_hold_off

    if (fsm == SENSOR_FAST_SWITCH_GROUP1_MODE1_S5KHM6_12M_CROP_FROM_108M) {
        s5khm6sx_drv_write_awb_gain(handle);
    }

    sns_drv_cxt->sensor_mode = mode_after;
    SENSOR_LOGI("X:target sensor_mode %d, fast_switch_mode %d", mode_after,
                fsm);
    return ret;
}

static cmr_int s5khm6sx_drv_set_mode_to_cxt(cmr_handle handle) {

    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_drv_context *sensor_cxt =
        (struct sensor_drv_context *)sns_drv_cxt->caller_handle;

    sns_drv_cxt->sensor_mode = sensor_cxt->sensor_mode;
    SENSOR_LOGI("X:sensor_mode %d", sns_drv_cxt->sensor_mode);

    return ret;
}

#if 1
static const cmr_u32 sns_9in1_mode[] = {0, 0, 0, 0, 1, 1, 0};
static cmr_int s5khm6sx_drv_get_4in1_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_4in1_info *sn_4in1_info = NULL;
    SENSOR_IC_CHECK_PTR(param);
    SENSOR_LOGI("E");

    sn_4in1_info = (struct sensor_4in1_info *)param;
    sn_4in1_info->is_4in1_supported = 0;
    sn_4in1_info->limited_4in1_width = PREVIEW_WIDTH;
    sn_4in1_info->limited_4in1_height = PREVIEW_HEIGHT;
    sn_4in1_info->sns_mode = sns_9in1_mode;

    return rtn;
}

static cmr_int s5khm6sx_drv_4in1_init(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;

    SENSOR_LOGV("E");

    return rtn;
}

static cmr_int s5khm6sx_drv_4in1_process(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;

    SENSOR_LOGV("E");

    return rtn;
}

static cmr_int s5khm6sx_drv_4in1_deinit(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;

    SENSOR_LOGV("E");

    return rtn;
}
#endif

static cmr_int s5khm6sx_drv_stream_on(cmr_handle handle, cmr_uint param) {
    cmr_s32 ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_drv_context *sensor_cxt =
        (struct sensor_drv_context *)sns_drv_cxt->caller_handle;
    char xtalk_dump[PROPERTY_VALUE_MAX] = {0};
    cmr_u32 sensor_mode = 0;

    sns_drv_cxt->ops_cb.get_mode(sns_drv_cxt->caller_handle, &sensor_mode);
    if (sns_9in1_mode[sensor_mode]) {
        s5khm6sx_drv_write_awb_gain(handle);
    }

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xFCFC, 0x4000);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0100);

    usleep(5 * 1000);

    property_get("persist.vendor.cam.xtalk.otp.dump", xtalk_dump, "0");
    if (!strcmp(xtalk_dump, "1")) {
        cmr_u16 val = 0;
        cmr_u16 v = 0xffff;
        cmr_u32 i = 0;
        cmr_u32 page = 0;
        cmr_u32 high8, low8 = 0;
        FILE *fp = NULL;
        cmr_u8 s5khm6_xtalk[12500] = {0};

        i = 0x0B10;
        val = hw_sensor_read_reg(sns_drv_cxt->hw_handle, i);
        SENSOR_LOGI("xtalk cali on (0x%x, 0x%x)", i, val);

        for (page = 388; page <= /*581*/ 484; page++) {
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0A02, page);
            hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0A00, 0x0100);
            while (v != 0) {
                v = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0A00);
                // SENSOR_LOGI("xtalk (0x0A00, 0x%04x)", v);
            }
            // usleep(1 * 1000);
            for (i = 0x0A04; i <= 0x0A42; i = i + 2) {
                val = hw_sensor_read_reg(sns_drv_cxt->hw_handle, i);
                high8 = (page - 388) * 64 + (i - 0x0A04);
                low8 = (page - 388) * 64 + (i - 0x0A03);
                s5khm6_xtalk[high8] = (val >> 8) & 0xff;
                s5khm6_xtalk[low8] = val & 0xff;
                if (0 /*page == 392*/) {
                    SENSOR_LOGI(
                        "page %d (0x%04x, 0x%04x), (0x%04x, 0x%02x%02x)", page,
                        i, val, high8, s5khm6_xtalk[high8], s5khm6_xtalk[low8]);
                }
            }
        }

        fp = fopen("/data/vendor/cameraserver/s5khm6_xtalk_otp.bin", "wb");
        fwrite(s5khm6_xtalk, 1, /*12416*/ 6208, fp);
        fclose(fp);
    }
    SENSOR_LOGI("X");

    return 0;
}

static cmr_int s5khm6sx_drv_stream_off(cmr_handle handle, cmr_uint param) {
    UNUSED(param);
    cmr_s32 ret = SENSOR_SUCCESS;
    uint16_t value;
    cmr_u16 status = 0;
    clock_t start_clk, end_clk;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    SENSOR_LOGI("StreamOff:E");

    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0100);

    if (value != 0x0000) {
        ret = hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x00);
        start_clk = end_clk = clock();
        while (1) {
            status = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0004);
            if ((status & 0xff) == 0xff) {
                goto exit;
            }
            end_clk = clock();
            if ((end_clk - start_clk) * 1000 / CLOCKS_PER_SEC > 100) {
                SENSOR_LOGI("sensor stream off out of time");
                goto exit;
            }
            usleep(1000);
        }
    }

exit:
    SENSOR_LOGI("X");

    return ret;
}

static cmr_int s5khm6sx_drv_power_on(cmr_handle handle, cmr_uint power_on) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;

    SENSOR_IC_CHECK_PTR(module_info);

    SENSOR_AVDD_VAL_E dvdd_val = module_info->dvdd_val;
    SENSOR_AVDD_VAL_E avdd_val = module_info->avdd_val;
    SENSOR_AVDD_VAL_E iovdd_val = module_info->iovdd_val;
    BOOLEAN power_down = MIPI_RAW_INFO.power_down_level;
    BOOLEAN reset_level = MIPI_RAW_INFO.reset_pulse_level;

    if (SENSOR_TRUE == power_on) {
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        usleep(1 * 1000);
        hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, iovdd_val);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, avdd_val);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, dvdd_val);
        usleep(1 * 1000);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, !reset_level);
        usleep(1 * 1000);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DEFALUT_MCLK);
        usleep(11 * 1000);
    } else {
        usleep(1 * 1000);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        usleep(1 * 1000);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        usleep(1 * 1000);
        hw_sensor_set_voltage(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED,
                              SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED);
    }

    SENSOR_LOGI("s5khm6sx_drv_power_on(1:on, 0:off): %ld, "
                "reset_level %d, dvdd_val %d",
                power_on, reset_level, dvdd_val);
    return SENSOR_SUCCESS;
}

static cmr_int s5khm6sx_drv_get_static_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int ret = SENSOR_SUCCESS;
    struct sensor_ex_info *ex_info;
    uint32_t up = 0;
    uint32_t down = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;
    if (!(fps_info && static_info && module_info)) {
        SENSOR_LOGE("error:null pointer checked.return");
        return SENSOR_FAIL;
    }

    if (!fps_info->is_init) {
        s5khm6sx_drv_init_fps_info(handle);
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

    ex_info->fov_angle = static_info->fov_angle;
    memcpy(&ex_info->fov_info, &static_info->fov_info,
           sizeof(static_info->fov_info));

    ex_info->pos_dis.up2hori = up;
    ex_info->pos_dis.hori2down = down;
    sensor_ic_print_static_info("s5khm6sx", ex_info);

    return ret;
}

static cmr_int s5khm6sx_drv_get_fps_info(cmr_handle handle, void *param) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_MODE_FPS_T *fps_info;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_fps_info *fps_data = sns_drv_cxt->fps_info;

    if (!fps_data->is_init) {
        s5khm6sx_drv_init_fps_info(handle);
    }
    fps_info = (SENSOR_MODE_FPS_T *)param;
    uint32_t sensor_mode = fps_info->mode;
    fps_info->max_fps = fps_data->sensor_mode_fps[sensor_mode].max_fps;
    fps_info->min_fps = fps_data->sensor_mode_fps[sensor_mode].min_fps;
    fps_info->is_high_fps = fps_data->sensor_mode_fps[sensor_mode].is_high_fps;
    fps_info->high_fps_skip_num =
        fps_data->sensor_mode_fps[sensor_mode].high_fps_skip_num;
    SENSOR_LOGI("mode %d, max_fps: %d", fps_info->mode, fps_info->max_fps);
    SENSOR_LOGI("min_fps: %d", fps_info->min_fps);
    SENSOR_LOGI("is_high_fps: %d", fps_info->is_high_fps);
    SENSOR_LOGI("high_fps_skip_num: %d", fps_info->high_fps_skip_num);

    return ret;
}

static const cmr_u16 s5khm6sx_pd_is_right[] = {1, 0};

static const cmr_u16 s5khm6sx_pd_col[] = {0, 1};

static const cmr_u16 s5khm6sx_pd_row[] = {0, 0};

static const struct pd_pos_info s5khm6sx_pd_pos_r[] = {{0, 0}};
static const struct pd_pos_info s5khm6sx_pd_pos_l[] = {{1, 0}};

struct pdaf_block_descriptor s5khm6sx_pd_seprator_helper = {
    .block_width = 1,
    .block_height = 2,
    .coordinate_tab = {1, 0},
    .line_width = 1,
    .block_pattern = LINED_UP,
    .pd_line_coordinate = NULL,
};

static const cmr_u32 s5khm6sx_pd_sns_mode[] = {0, 0, 1, 1, 0, 0};

struct sensor_pdaf_info s5khm6sx_pdaf_info_array[] = {
    /* pd setting for sensor mode0 */
    {0},

    /* pd setting for sensor mode1 */
    {0},

    /* pd setting for sensor mode2 */
    {
        .pd_is_right = (cmr_u16 *)s5khm6sx_pd_is_right,
        .pd_pos_row = (cmr_u16 *)s5khm6sx_pd_row,
        .pd_pos_col = (cmr_u16 *)s5khm6sx_pd_col,
        .pd_pos_r = (struct pd_pos_info *)s5khm6sx_pd_pos_r,
        .pd_pos_l = (struct pd_pos_info *)s5khm6sx_pd_pos_l,
        .pd_pos_size = NUMBER_OF_ARRAY(s5khm6sx_pd_pos_r),
        .pd_offset_x = 0,
        .pd_offset_y = 0,
        .pd_end_x = 3840,
        .pd_end_y = 2160,
        .pd_block_w = -2, // pd_unit_w = 8 >> 2
        .pd_block_h = -1, // pd_unit_h = 8 >> 1
        .pd_block_num_x = 1920,
        .pd_block_num_y = 540,
        .pd_density_x = 2,
        .pd_density_y = 4,
        .pd_pitch_x = 8,
        .pd_pitch_y = 8,
        .vch2_info.bypass = 0,
        .vch2_info.vch2_vc = 1,
        .vch2_info.vch2_data_type = 0x30,
        .vch2_info.vch2_mode = 0x02,
        .sns_mode = s5khm6sx_pd_sns_mode,
        .descriptor = &s5khm6sx_pd_seprator_helper,
        .sns_orientation = 0,    /*1: mirror+flip; 0: normal*/
        .pd_size_w = 3840,       // actual raw size
        .pd_size_h = 2160,       // actual raw size
        .image_process_type = 1, // 0:default; 1:only crop; 2:binning+crop;
        .crop_offset_x = 80,
        .crop_offset_y = 420,
    },

    /* pd setting for sensor mode3 */
    {
        .pd_is_right = (cmr_u16 *)s5khm6sx_pd_is_right,
        .pd_pos_row = (cmr_u16 *)s5khm6sx_pd_row,
        .pd_pos_col = (cmr_u16 *)s5khm6sx_pd_col,
        .pd_pos_r = (struct pd_pos_info *)s5khm6sx_pd_pos_r,
        .pd_pos_l = (struct pd_pos_info *)s5khm6sx_pd_pos_l,
        .pd_pos_size = NUMBER_OF_ARRAY(s5khm6sx_pd_pos_r),
        .pd_offset_x = 0,
        .pd_offset_y = 0,
        .pd_end_x = 4000,
        .pd_end_y = 3000,
        .pd_block_w = -2, // pd_unit_w = 8 >> 2
        .pd_block_h = -1, // pd_unit_h = 8 >> 1
        .pd_block_num_x = 2000,
        .pd_block_num_y = 750,
        .pd_density_x = 2,
        .pd_density_y = 4,
        .pd_pitch_x = 8,
        .pd_pitch_y = 8,
        .vch2_info.bypass = 0,
        .vch2_info.vch2_vc = 1,
        .vch2_info.vch2_data_type = 0x30,
        .vch2_info.vch2_mode = 0x02,
        .sns_mode = s5khm6sx_pd_sns_mode,
        .descriptor = &s5khm6sx_pd_seprator_helper,
        .sns_orientation = 0,    /*1: mirror+flip; 0: normal*/
        .pd_size_w = 4000,       // actual raw size
        .pd_size_h = 3000,       // actual raw size
        .image_process_type = 0, // 0:default; 1:only crop; 2:binning+crop;
        .crop_offset_x = 0,
        .crop_offset_y = 0,
    },

    /* pd setting for sensor mode4 */
    {0},

    /* pd setting for sensor mode5 */
    {0},
    /* add more pdaf setting here */
};

static struct sensor_pdaf_info_tab s5khm6sx_pdaf_info_tab = {
    .sns_mode_num = NUMBER_OF_ARRAY(s5khm6sx_pd_sns_mode),
    .pd_sns_mode = s5khm6sx_pd_sns_mode,
    .pdaf_init_index = 3,   // origin size of otp.
    .pd_init_size_w = 4000, // origin size of otp
    .pd_init_size_h = 3000, // origin size of otp
    .pd_info_tab = s5khm6sx_pdaf_info_array,
};

cmr_int s5khm6sx_drv_pdaf_data_process(void *buffer_handle);

static cmr_int s5khm6sx_drv_get_pdaf_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_pdaf_info_tab *pdaf_info_tab = NULL;
    struct sensor_pdaf_info *pdaf_info = NULL;
    cmr_int pd_info_num = 0;
    int i = 0;
    SENSOR_IC_CHECK_PTR(param);

    pdaf_info_tab = (struct sensor_pdaf_info_tab *)param;

    pdaf_info_tab->sns_mode_num = s5khm6sx_pdaf_info_tab.sns_mode_num;
    pdaf_info_tab->pd_sns_mode = s5khm6sx_pdaf_info_tab.pd_sns_mode;
    pdaf_info_tab->pdaf_init_index = s5khm6sx_pdaf_info_tab.pdaf_init_index;
    pdaf_info_tab->pd_init_size_w = s5khm6sx_pdaf_info_tab.pd_init_size_w;
    pdaf_info_tab->pd_init_size_h = s5khm6sx_pdaf_info_tab.pd_init_size_h;
    pdaf_info_tab->pd_info_tab = s5khm6sx_pdaf_info_tab.pd_info_tab;

    pd_info_num = s5khm6sx_pdaf_info_tab.sns_mode_num;
    for (i = 0; i < pd_info_num; i++) {
        if (pdaf_info_tab->pd_sns_mode[i]) {
            pdaf_info = &(pdaf_info_tab->pd_info_tab[i]);
            pdaf_info->pdaf_format_converter = s5khm6sx_drv_pdaf_data_process;
            pdaf_info->pd_data_size = pdaf_info->pd_block_num_x *
                                      pdaf_info->pd_block_num_y *
                                      pdaf_info->pd_pos_size * 2 * 5;
            SENSOR_LOGD("pd data size:%d", pdaf_info->pd_data_size);
        }
    }
    SENSOR_LOGE("sensor pdaf_init_index %d", pdaf_info_tab->pdaf_init_index);
    SENSOR_LOGI("X\n");
    return rtn;
}
cmr_int s5khm6sx_drv_pdaf_data_process(void *buffer_handle) {
    if (!buffer_handle)
        return SENSOR_FAIL;
    cmr_u32 sensor_mode = 0;
    s5khm6sx_sns_drv_cxt->ops_cb.get_mode(s5khm6sx_sns_drv_cxt->caller_handle,
                                          &sensor_mode);
    if (!s5khm6sx_pd_sns_mode[sensor_mode]) {
        SENSOR_LOGE("sensor mode %d pdaf type2 unsupported!", sensor_mode);
        return SENSOR_FAIL;
    }
    sensor_pdaf_format_convertor(
        buffer_handle, s_s5khm6sx_static_info[0].static_info.pdaf_supported,
        (cmr_u32 *)(&s5khm6sx_pdaf_info_array[sensor_mode]));
    return SENSOR_SUCCESS;
}

static cmr_int s5khm6sx_drv_access_val(cmr_handle handle, cmr_int param) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_VAL_T *param_ptr = (SENSOR_VAL_T *)param;
    uint16_t tmp;

    SENSOR_LOGI("param_ptr = %p", param_ptr);
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param_ptr);

    SENSOR_LOGI("param_ptr->type=%x", param_ptr->type);
    switch (param_ptr->type) {
    case SENSOR_VAL_TYPE_SHUTTER:
        *((uint32_t *)param_ptr->pval) = s5khm6sx_drv_get_shutter(handle);
        break;
    case SENSOR_VAL_TYPE_GET_AFPOSITION:
        *(uint32_t *)param_ptr->pval = 0; // cur_af_pos;
        break;
    case SENSOR_VAL_TYPE_GET_STATIC_INFO:
        rtn = s5khm6sx_drv_get_static_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_FPS_INFO:
        rtn = s5khm6sx_drv_get_fps_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_4IN1_INFO:
        rtn = s5khm6sx_drv_get_4in1_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_PDAF_INFO:
        rtn = s5khm6sx_drv_get_pdaf_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_WRITE_AWB_GAIN:
        rtn = s5khm6sx_drv_set_awb_gain(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_SET_MODE_TO_CXT:
        rtn = s5khm6sx_drv_set_mode_to_cxt(handle);
        break;
    default:
        break;
    }

    SENSOR_LOGI("X");

    return rtn;
}

static cmr_int
s5khm6sx_drv_handle_create(struct sensor_ic_drv_init_para *init_param,
                           cmr_handle *sns_ic_drv_handle) {
    cmr_int ret = SENSOR_SUCCESS;
    struct sensor_ic_drv_cxt *sns_drv_cxt = NULL;

    SENSOR_LOGI("E");
    ret = sensor_ic_drv_create(init_param, sns_ic_drv_handle);
    if (SENSOR_IC_FAILED == ret) {
        SENSOR_LOGE("sensor instance create failed!");
        return ret;
    }
    sns_drv_cxt = *sns_ic_drv_handle;
    s5khm6sx_sns_drv_cxt = *sns_ic_drv_handle;
    sns_drv_cxt->sensor_ev_info.preview_shutter =
        PREVIEW_FRAME_LENGTH - FRAME_OFFSET;
    sns_drv_cxt->sensor_ev_info.preview_gain = SENSOR_BASE_GAIN;
    sns_drv_cxt->sensor_ev_info.preview_framelength = PREVIEW_FRAME_LENGTH;
    sns_drv_cxt->frame_length_def = PREVIEW_FRAME_LENGTH;
    sns_drv_cxt->line_time_def = PREVIEW_LINE_TIME;

    sensor_ic_set_match_module_info(sns_drv_cxt, ARRAY_SIZE(MODULE_INFO),
                                    MODULE_INFO);
    sensor_ic_set_match_resolution_info(sns_drv_cxt, ARRAY_SIZE(RES_TAB_RAW),
                                        RES_TAB_RAW);
    sensor_ic_set_match_trim_info(sns_drv_cxt, ARRAY_SIZE(RES_TRIM_TAB),
                                  RES_TRIM_TAB);
    sensor_ic_set_match_static_info(sns_drv_cxt, ARRAY_SIZE(STATIC_INFO),
                                    STATIC_INFO);
    sensor_ic_set_match_fps_info(sns_drv_cxt, ARRAY_SIZE(FPS_INFO), FPS_INFO);

    s5khm6sx_drv_init_exif_info(sns_drv_cxt, &sns_drv_cxt->exif_ptr);

    SENSOR_LOGI("X");

    return ret;
}

static cmr_s32 s5khm6sx_drv_handle_delete(cmr_handle handle, uint32_t *param) {
    cmr_s32 ret = SENSOR_SUCCESS;
    /*if has private data,you must release it here*/
    SENSOR_LOGI("E:");
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    ret = sensor_ic_drv_delete(handle, param);
    SENSOR_LOGI("X:");

    return ret;
}

static cmr_int s5khm6sx_drv_get_private_data(cmr_handle handle, cmr_uint cmd,
                                             void **param) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);

    ret = sensor_ic_get_private_data(handle, cmd, param);
    return ret;
}
void *sensor_ic_open_lib(void) { return &g_s5khm6sx_mipi_raw_info; }

static struct sensor_ic_ops s5khm6sx_ops_tab = {
    .create_handle = s5khm6sx_drv_handle_create,
    .delete_handle = s5khm6sx_drv_handle_delete,
    .get_data = s5khm6sx_drv_get_private_data,
    .power = s5khm6sx_drv_power_on,
    .identify = s5khm6sx_drv_identify,

    .write_exp = s5khm6sx_drv_write_exposure,
    .write_gain_value = s5khm6sx_drv_write_gain,
    .ex_write_exp = s5khm6sx_drv_ex_write_exposure,
    .read_aec_info = s5khm6sx_read_aec_info,
    .ext_ops = {
            [SENSOR_IOCTL_BEFORE_SNAPSHOT].ops = s5khm6sx_drv_before_snapshot,
            [SENSOR_IOCTL_STREAM_ON].ops = s5khm6sx_drv_stream_on,
            [SENSOR_IOCTL_STREAM_OFF].ops = s5khm6sx_drv_stream_off,
            [SENSOR_IOCTL_FAST_SWITCH].ops = s5khm6sx_drv_fast_switch,
            [SENSOR_IOCTL_ACCESS_VAL].ops = s5khm6sx_drv_access_val,
    }};
