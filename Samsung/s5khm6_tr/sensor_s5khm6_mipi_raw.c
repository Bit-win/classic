/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * V1.0
 */
/*History
*Date                  Modification                                 Reason
*
*/

#define LOG_TAG "s5khm6"

#include "sensor_s5khm6_isz_mipi_raw.h"
#include "otp_common.h"
#include <string.h>

/*==============================================================================
 * Description:
 * write register value to sensor
 * please modify this function acording your spec
 *============================================================================*/

static void s5khm6_drv_write_reg2sensor(cmr_handle handle,
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
static void s5khm6_drv_write_gain(cmr_handle handle,
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
static void s5khm6_drv_write_frame_length(cmr_handle handle,
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
static void s5khm6_drv_write_shutter(cmr_handle handle,
                                     struct sensor_aec_i2c_tag *aec_info,
                                     cmr_u64 shutter) {
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if (aec_info->shutter->size) {
        /*TODO*/
        aec_info->shutter->settings[0].reg_value = shutter & 0xffff;
        aec_info->shutter->settings[1].reg_value = (shutter>>16 & 0xf) << 8;
        aec_info->shutter->settings[2].reg_value = (shutter>>16 & 0xf) << 8;
        /*END*/
    }
}

/*==============================================================================
 * Description:
 * write exposure to sensor registers and get current shutter
 * please pay attention to the frame length
 * please don't change this function if it's necessary
 *============================================================================*/
static void s5khm6_drv_calc_exposure(cmr_handle handle, cmr_u32 shutter,
                                     cmr_u32 dummy_line, cmr_u16 mode, cmr_u64 exp_time,
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
        ((double)(shutter + dummy_line) * sns_drv_cxt->line_time_def) / 1000000);

    if (frame_interval >= 500) {// 1s enter long_exp, 2^7 =128, 0x0700
        shutter = (double)1000000000/(PREVIEW_LINE_TIME*128*1000)*frame_interval + (7 << 16);
        dest_fr_len = (shutter + 25)&0xffff;
    }

    SENSOR_LOGI("mode = %d, fps =%f, framelength = %d, exposure_line = %d, dummy_line= %d, frame_interval= %d ms",
                    mode, fps, dest_fr_len, shutter, dummy_line, frame_interval);

    if (dest_fr_len != cur_fr_len) {
        sns_drv_cxt->sensor_ev_info.preview_framelength = dest_fr_len;
        s5khm6_drv_write_frame_length(handle, aec_info, dest_fr_len);
    }

    sns_drv_cxt->sensor_ev_info.preview_exptime= exp_time;
    sns_drv_cxt->sensor_ev_info.preview_shutter = shutter;
    s5khm6_drv_write_shutter(handle, aec_info, shutter);

    if (sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->ops_cb.set_exif_info(
            sns_drv_cxt->caller_handle, SENSOR_EXIF_CTRL_EXPOSURETIME_BYTIME, exp_time);
    }
}

static void s5khm6_drv_calc_gain(cmr_handle handle, cmr_uint isp_gain,
                                 struct sensor_aec_i2c_tag *aec_info) {
    cmr_u32 sensor_gain = 0;
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_drv_context *sensor_cxt =
            (struct sensor_drv_context *)sns_drv_cxt->caller_handle;

    sensor_gain = isp_gain < ISP_BASE_GAIN ? ISP_BASE_GAIN : isp_gain;
    sensor_gain = sensor_gain * SENSOR_BASE_GAIN / ISP_BASE_GAIN;

    if (SENSOR_MAX_GAIN < sensor_gain)
        sensor_gain = SENSOR_MAX_GAIN;

    if (sns_4in1_mode[sensor_cxt->sensor_mode])
    {
        if (SENSOR_MAX_GAIN_REMOSAIC < sensor_gain)
            sensor_gain = SENSOR_MAX_GAIN_REMOSAIC;
    }

    SENSOR_LOGI("isp_gain = 0x%x,sensor_gain=0x%x", (unsigned int)isp_gain,
                sensor_gain);

    sns_drv_cxt->sensor_ev_info.preview_gain = sensor_gain;
    s5khm6_drv_write_gain(handle, aec_info, sensor_gain);
}

/*==============================================================================
 * Description:
 * sensor power on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5khm6_drv_power_on(cmr_handle handle, cmr_uint power_on) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;

    SENSOR_AVDD_VAL_E dvdd_val = module_info->dvdd_val;
    SENSOR_AVDD_VAL_E avdd_val = module_info->avdd_val;
    SENSOR_AVDD_VAL_E iovdd_val = module_info->iovdd_val;
    BOOLEAN power_down = g_s5khm6_mipi_raw_info.power_down_level;
    BOOLEAN reset_level = g_s5khm6_mipi_raw_info.reset_pulse_level;

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
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, !reset_level);
        usleep(1 * 1000);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, EX_MCLK);
        usleep(5 * 1000);
    } else {
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        usleep(1 * 1000);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        usleep(1 * 1000);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        usleep(1*1000);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
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
static cmr_int s5khm6_drv_init_fps_info(cmr_handle handle) {

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

static double s5khm6_longExp_mode[] = {0.5, 1, 2, 3, 4, 5, 6, 7, 8, 10, 13, 15, 20, 25, 30};
static cmr_u32 s5khm6_longExp_valid_frame_num = 2;   //The first frame takes effect by sensor
static cmr_u32 s5khm6_longExp_need_switch_setting = 0;
static cmr_u32 s5khm6_longExp_setting_mode[] = {0, 1, 0, 0};
static cmr_u64 s5khm6_longExp_threshold = 59000000;// Depends on sensor, MIN longExp or MAX normal exposure to change longEXP_mode
static cmr_int s5khm6_drv_get_static_info(cmr_handle handle, cmr_u32 *param) {

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
        s5khm6_drv_init_fps_info(handle);
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
    ex_info->name = (cmr_s8 *)g_s5khm6_mipi_raw_info.name;
    ex_info->sensor_version_info =
        (cmr_s8 *)g_s5khm6_mipi_raw_info.sensor_version_info;
    memcpy(&ex_info->fov_info, &static_info->fov_info,
           sizeof(static_info->fov_info));
    ex_info->pos_dis.up2hori = up;
    ex_info->pos_dis.hori2down = down;
    ex_info->long_expose_supported = static_info->long_expose_supported;
    if(ex_info->long_expose_supported) {
        ex_info->long_expose_modes = s5khm6_longExp_mode;
        ex_info->long_expose_modes_size = ARRAY_SIZE(s5khm6_longExp_mode);
        ex_info->long_exposure_setting = s5khm6_longExp_setting_mode;
        ex_info->long_exposure_setting_size = ARRAY_SIZE(s5khm6_longExp_setting_mode);
        ex_info->longExp_valid_frame_num = s5khm6_longExp_valid_frame_num;
        ex_info->long_exposure_threshold = s5khm6_longExp_threshold;
        ex_info->longExp_need_switch_setting = s5khm6_longExp_need_switch_setting;
    }
    sensor_ic_print_static_info((cmr_s8 *)SENSOR_NAME, ex_info);

    return rtn;
}

static cmr_int s5khm6_drv_get_fps_info(cmr_handle handle, cmr_u32 *param) {

    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    SENSOR_MODE_FPS_T *fps_info = (SENSOR_MODE_FPS_T *)param;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_fps_info *fps_data = sns_drv_cxt->fps_info;

    // make sure have inited fps of every sensor mode.
    if (!fps_data->is_init) {
        s5khm6_drv_init_fps_info(handle);
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



cmr_int s5khm6_drv_pdaf_data_process(void *buffer_handle);

static cmr_int s5khm6_drv_get_pdaf_info(cmr_handle handle, void *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_pdaf_info_tab *pdaf_info_tab = NULL;
    struct sensor_pdaf_info *pdaf_info = NULL;
    cmr_int pd_info_num = 0;
    int i = 0;
    SENSOR_IC_CHECK_PTR(param);

    SENSOR_LOGV("E");

    pdaf_info_tab = (struct sensor_pdaf_info_tab *)param;
    
    pdaf_info_tab->sns_mode_num = s5khm6_pdaf_info_tab.sns_mode_num;
    pdaf_info_tab->pd_sns_mode = s5khm6_pdaf_info_tab.pd_sns_mode;
    pdaf_info_tab->pdaf_init_index = s5khm6_pdaf_info_tab.pdaf_init_index;
    pdaf_info_tab->pd_init_size_w = s5khm6_pdaf_info_tab.pd_init_size_w;
    pdaf_info_tab->pd_init_size_h = s5khm6_pdaf_info_tab.pd_init_size_h;
    pdaf_info_tab->isz_pdaf_index = s5khm6_pdaf_info_tab.isz_pdaf_index;
    pdaf_info_tab->pd_info_tab = s5khm6_pdaf_info_tab.pd_info_tab;
    pdaf_info_tab->pd_iszfull_info = s5khm6_pdaf_info_tab.pd_iszfull_info;

    pd_info_num = s5khm6_pdaf_info_tab.sns_mode_num;
    for(i = 0; i < pd_info_num; i++) {
        if(pdaf_info_tab->pd_sns_mode[i]){
            pdaf_info = &(pdaf_info_tab->pd_info_tab[i]);
            pdaf_info->pdaf_format_converter = s5khm6_drv_pdaf_data_process;
            pdaf_info->pd_data_size = pdaf_info->pd_block_num_x * pdaf_info->pd_block_num_y *
                                      pdaf_info->pd_pos_size * 2 * 5;
            SENSOR_LOGD("pd data size:%d", pdaf_info->pd_data_size);
        }
    }
    SENSOR_LOGI("sensor pdaf_init_index %d, isz_pdaf_index %d",
        pdaf_info_tab->pdaf_init_index, pdaf_info_tab->isz_pdaf_index);
    return rtn;
}

static struct sensor_ic_drv_cxt *s5khm6_sns_drv_cxt = NULL;

cmr_int s5khm6_drv_pdaf_data_process(void *buffer_handle) {
    if(!buffer_handle)
        return SENSOR_FAIL;
    cmr_u32 sensor_mode = 0;
    s5khm6_sns_drv_cxt->ops_cb.get_mode(s5khm6_sns_drv_cxt->caller_handle, &sensor_mode);
    if(!s5khm6_pd_sns_mode[sensor_mode]) {
        SENSOR_LOGE("sensor mode %d pdaf type2 unsupported!", sensor_mode);
        return SENSOR_FAIL;
    }
    sensor_pdaf_format_convertor(buffer_handle, s_s5khm6_static_info[0].static_info.pdaf_supported,
                                 (cmr_u32 *)(&s5khm6_pdaf_info_array[sensor_mode]));
    return SENSOR_SUCCESS;
}
/*==============================================================================
 * Description:
 * GGC read from otp. write to sensor
 * please modify this function acording your spec
 *============================================================================*/
#if 0
static cmr_int s5khm6_readotp_section_check(cmr_u16 flag_reg, cmr_u16 checksum_reg, cmr_u16 data_size){
    cmr_u16 flag_reg_addr = flag_reg - OTP_DATA_START_REG;
    cmr_u16 checksum_reg_addr = checksum_reg - OTP_DATA_START_REG;
    int sum = 0, i = 0;
    cmr_int rtn = SENSOR_SUCCESS;

    CMR_LOGD("flag_reg_addr %x, checksum_reg_addr = 0x%x, OTP_DATA_START_REG = 0x%x ,data_size = %d",
                flag_reg_addr, checksum_reg_addr, OTP_DATA_START_REG,  data_size);

    //section flag check, default(0x01)
    if (otp_data_buff[flag_reg_addr] != 0x01) {
        rtn = SENSOR_FAIL;
    }
    CMR_LOGD("flag rtn %d, addr = 0x%x, otp section FLAG =0x%x ", rtn, flag_reg, otp_data_buff[flag_reg_addr]);

    // section check_sum check
    for(i = 1; i < data_size; i++) {
        sum += otp_data_buff[flag_reg_addr + i];
    }
    sum = sum % 256;
    if (sum != otp_data_buff[checksum_reg_addr]) {
        rtn = SENSOR_FAIL;
    }

    SENSOR_LOGD("checksum ret =%d,calculate_sum = 0x%x,otp_sum =0x%x ", rtn, sum, otp_data_buff[checksum_reg_addr]);

    return rtn;

}

static cmr_int s5khm6_readotp(cmr_handle handle){
    cmr_int rtn = SENSOR_SUCCESS;

   //read otp data from EPPROM
    otp_data_buff[0] = (OTP_DATA_START_REG & 0xff00 ) >> 8;
    otp_data_buff[1] = OTP_DATA_START_REG & 0x00ff;
    rtn = hw_sensor_read_i2c(handle, 0xa0 >> 1, otp_data_buff,
                       OTP_DATA_SIZE << 16 | SENSOR_I2C_REG_16BIT);
    if (rtn != HW_SUCCESS) {
        SENSOR_LOGE("read xtalk data  fail return %d", rtn);
    return SENSOR_FAIL;
    }

    //step.1 check XGC flag
    if (otp_data_buff[0x00] != 0x01)
        SENSOR_LOGE("read xtalk flag  fail, flag value 0x%x", otp_data_buff[0x00]);

	//step.2 check XGC sum
    rtn = sensor_otp_section_checksum(otp_data_buff,0x01,OTP_DATA_SIZE -1);
    if (rtn) {
        CMR_LOGE("XTC section check sum fail %d", rtn);
        return SENSOR_FAIL;
    }
    CMR_LOGD("XTC section read success %d", rtn);

    return SENSOR_SUCCESS;
}

static cmr_int s5khm6_writeotp(cmr_handle handle){
    cmr_u16 XGC_Flag;
    cmr_u16 QBGC_Flag;
    cmr_u8 *pdata = NULL;
    cmr_u16 TEMP1;
    cmr_u16 temp_value;
    cmr_u16 i = 0;

    //1. XGC_calibration_write
    hw_sensor_write_reg(handle, 0x0B00, 0X0000);//stearm off
    TEMP1 = hw_sensor_read_reg(handle, 0x0318);
    hw_sensor_write_reg(handle, 0x0318, TEMP1 & 0xFFEF); //XGC data loading disable B[4]
    XGC_Flag = hw_sensor_read_reg(handle, 0x0318);
    SENSOR_LOGD("XGC_Flag = %d", XGC_Flag);

    pdata = &otp_data_buff[0x01];// xgc_gb first data index,basc offset OTP_DATA_START_REG
    SENSOR_LOGI("[Start]: XGC_Gb Line Start first data =0x%x", *pdata);

    //2. XGC(Gb line) SRAM memory access enalbe
    hw_sensor_write_reg(handle, 0xffff, 0x0A40); //XGC(Gb line) enable

    for (i = 0x0000; i < 960; i += 2) {
        temp_value = (*pdata) & 0x00ff;
        pdata++;
        temp_value |= ((*pdata) & 0x00ff) << 8;
        pdata++;
		//SENSOR_LOGI("XGC_Gb data =0x%x", temp_value);
        hw_sensor_write_reg(handle, i, temp_value);
    }

    //2.1 XGC(Gb line) SRAM memory access disable
    hw_sensor_write_reg(handle, 0xffff, 0x0000); //XGC(Gb line) disable
    SENSOR_LOGI("[End]: XGC_Gb Line End ");

    SENSOR_LOGI("[Start]: XGC_Gr Line Start first data =0x%x", *pdata);
    //3. XGC(Gr line) SRAM memory access enalbe
    hw_sensor_write_reg(handle, 0xffff, 0x0B40); //XGC(Gr line) enable
    for (i = 0x0000; i < 960; i += 2) {
        temp_value = (*pdata) & 0x00ff;
        pdata++;
        temp_value |= ((*pdata) & 0x00ff) << 8;
        pdata++;
		//SENSOR_LOGI("XGC_GR data =0x%x", temp_value);
        hw_sensor_write_reg(handle, i, temp_value);
    }
    //3.1 XGC(Gr line) SRAM memory access disable
    hw_sensor_write_reg(handle, 0xffff, 0x0000); //XGC(Gr line) disable
    hw_sensor_write_reg(handle, 0x0b32, 0xAAAA); //F/W run and data copy
    SENSOR_LOGI("[End]: XGC_Gr Line End ");

    //4. QBGC_calibration_write
    SENSOR_LOGI("[Start]: QBGC Start first data =0x%x", *pdata);
    TEMP1 = hw_sensor_read_reg(handle, 0x0318);
    hw_sensor_write_reg(handle, 0x0318, TEMP1 & 0xFFF7); //QBGC data loading disable B[3]
    QBGC_Flag = hw_sensor_read_reg(handle, 0x0318);
    SENSOR_LOGI("QBGC_Flag = %d", QBGC_Flag);

    //4.1 QBGC SRAM memory access enalbe
    hw_sensor_write_reg(handle, 0xffff, 0x0E40); //QBGC enable
    for (i = 0x0000; i < 1280; i += 2) {
        temp_value = (*pdata) & 0x00ff;
        pdata++;
        temp_value |= ((*pdata) & 0x00ff) << 8;
        pdata++;
		//SENSOR_LOGI("QBGC data =0x%x", temp_value);
        hw_sensor_write_reg(handle, i, temp_value);
    }
    //QBGC SRAM memory access disable
    hw_sensor_write_reg(handle, 0xffff, 0x0000); //QBGC disable
    hw_sensor_write_reg(handle, 0x0b32, 0xAAAA); //F/W run and QBGC data copy
    SENSOR_LOGI("[End]: QBGC End ");

    return SENSOR_SUCCESS;
}
#endif
/*==============================================================================
 * Description:
 * add the interface of remosaic
 * please modify this function acording your sensor spec && remosaic guide
 *============================================================================*/
static cmr_int s5khm6_drv_set_mode_to_cxt(cmr_handle handle) {

    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_drv_context *sensor_cxt =
        (struct sensor_drv_context *)sns_drv_cxt->caller_handle;

    sns_drv_cxt->sensor_mode = sensor_cxt->sensor_mode;
    SENSOR_LOGI("X:sensor_mode %d", sns_drv_cxt->sensor_mode);

    return ret;
}

static cmr_int s5khm6_drv_set_awb_gain(cmr_handle handle, void *awb_param) {
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_awb_gain_info *awb_info =
        (struct sensor_awb_gain_info *)awb_param;
    cmr_int ret = SENSOR_SUCCESS;
    sns_drv_cxt->awb_gain.b = awb_info->b;
    sns_drv_cxt->awb_gain.r = awb_info->r;
    sns_drv_cxt->awb_gain.g = awb_info->g;
    SENSOR_LOGI("Ispalg AWB Gain is [R: %d, G: %d, B: %d]", awb_info->r,
                awb_info->g, awb_info->b);
    return ret;
}

static cmr_int s5khm6_drv_get_4in1_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_4in1_info *sn_4in1_info = NULL;
    SENSOR_IC_CHECK_PTR(param);

    SENSOR_LOGI("E\n");

    sn_4in1_info = (struct sensor_4in1_info *)param;
    sn_4in1_info->is_4in1_supported = 0;
    sn_4in1_info->limited_4in1_width = PREVIEW_WIDTH;
    sn_4in1_info->limited_4in1_height = PREVIEW_HEIGHT;
    sn_4in1_info->sns_mode = sns_4in1_mode;
    //sn_4in1_info->input_format = DATA_RAW10;//for remosaic_lib process input data format
    //sn_4in1_info->output_format = DATA_BYTE2;//for remosaic_lib process output data format

    return rtn;
}

static cmr_int s5khm6_drv_4in1_init(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;

    SENSOR_LOGV("E");

    return rtn;
}


static cmr_int s5khm6_drv_4in1_process(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;

    SENSOR_LOGV("E");

    return rtn;
}

static cmr_int s5khm6_drv_4in1_deinit(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;

    SENSOR_LOGV("E");

    return rtn;
}


static cmr_int s5khm6_drv_write_awb_gain(cmr_handle handle) {
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


static cmr_int s5khm6_drv_fast_switch_calc_exposure(cmr_handle handle,
                                                    cmr_uint mode_before,
                                                    cmr_uint mode_after,
                                                    cmr_uint fsm) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u16 convert_shutter = sns_drv_cxt->sensor_ev_info.preview_shutter;
    cmr_u32 convert_exptime = sns_drv_cxt->sensor_ev_info.preview_exptime;
    cmr_u32 ref_linetime = sns_drv_cxt->trim_tab_info[mode_after].line_time;

    if (fsm == SENSOR_FAST_SWITCH_GROUP1_MODE0_S5KHM6_12M_9IN1_BINNING) {
        convert_exptime = convert_exptime / BINNING_FACTOR;
        convert_shutter = convert_exptime / ref_linetime;
    } else if (fsm == SENSOR_FAST_SWITCH_GROUP1_MODE1_S5KHM6_12M_CROP_FROM_108M){
        convert_exptime = convert_exptime * BINNING_FACTOR;
        convert_shutter = convert_exptime / ref_linetime;
    } else {
        SENSOR_LOGE("not fast switch mode, do not conver exposure");
        return SENSOR_FAIL;
    }

    sns_drv_cxt->sensor_ev_info.preview_framelength =
        sns_drv_cxt->trim_tab_info[mode_after].frame_line;
    s5khm6_drv_calc_exposure(handle, convert_shutter, 0, mode_after, convert_exptime,
                             &s5khm6_aec_info);
    s5khm6_drv_write_reg2sensor(handle, s5khm6_aec_info.frame_length);
    s5khm6_drv_write_reg2sensor(handle, s5khm6_aec_info.shutter);
    return ret;
}


static cmr_int s5khm6_drv_fast_switch(cmr_handle handle, cmr_uint param) {

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

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0104,0x0101); // group_hold_on

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

    //fast switch convert exposure
    ret = s5khm6_drv_fast_switch_calc_exposure(handle, mode_before, mode_after, fsm);
    if(ret) {
        SENSOR_LOGE("fast switch convert exposure failed");
    }

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0104,0x0001); // group_hold_off

    if (fsm == SENSOR_FAST_SWITCH_GROUP1_MODE1_S5KHM6_12M_CROP_FROM_108M) {
        s5khm6_drv_write_awb_gain(handle);
    }

    sns_drv_cxt->sensor_mode = mode_after;
    SENSOR_LOGI("X:target sensor_mode %d, fast_switch_mode %d", mode_after,
                fsm);
    return ret;
}

/*==============================================================================
 * Description:
 * cfg otp setting
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5khm6_drv_access_val(cmr_handle handle, cmr_uint param) {

    cmr_int ret = SENSOR_FAIL;
    SENSOR_VAL_T *param_ptr = (SENSOR_VAL_T *)param;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param_ptr);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGV("sensor s5khm6: param_ptr->type = 0x%x", param_ptr->type);

    switch (param_ptr->type) {
    case SENSOR_VAL_TYPE_GET_STATIC_INFO:
        ret = s5khm6_drv_get_static_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_FPS_INFO:
        ret = s5khm6_drv_get_fps_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_SET_SENSOR_CLOSE_FLAG:
        ret = sns_drv_cxt->is_sensor_close = 1;
        break;
    case SENSOR_VAL_TYPE_GET_PDAF_INFO:
        ret = s5khm6_drv_get_pdaf_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_4IN1_INFO:
        ret = s5khm6_drv_get_4in1_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_INIT:
        ret = s5khm6_drv_4in1_init(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_PROC:
        ret = s5khm6_drv_4in1_process(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_DEINIT:
        ret = s5khm6_drv_4in1_deinit(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_WRITE_AWB_GAIN:
        ret = s5khm6_drv_set_awb_gain(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_SET_MODE_TO_CXT:
        ret = s5khm6_drv_set_mode_to_cxt(handle);
        break;
    case SENSOR_VAL_TYPE_SET_OTP_DATA:
        //ret = s5khm6_writeotp(sns_drv_cxt->hw_handle);

    default:
        break;
    }
    ret = SENSOR_SUCCESS;

    return ret;
}

/*==============================================================================
 * Description:
 * identify sensor id
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5khm6_drv_identify(cmr_handle handle, cmr_uint param) {

    cmr_u16 pid_value = 0x00;
    //static cmr_int otp_read_once = 0;
    cmr_u16 module_id = 0;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("enter");

    pid_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle,S5KHM6_PID_ADDR);

    if (S5KHM6_PID_VALUE != pid_value)
    {
        SENSOR_LOGE("sensor identify fail, pid_value = %x", pid_value);
        return SENSOR_FAIL;
    }

    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * before snapshot
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int s5khm6_drv_before_snapshot(cmr_handle handle, cmr_uint param) {

    cmr_u32 cap_shutter = 0;
    cmr_u64 cap_exptime = 0;
    cmr_u32 prv_shutter = 0;
    cmr_u64 prv_exptime = 0;
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
    SENSOR_LOGI("preview_shutter = %d, preview_exptime= %d ns, preview_gain = %d",
                sns_drv_cxt->sensor_ev_info.preview_shutter,
                sns_drv_cxt->sensor_ev_info.preview_exptime,
                (unsigned int)sns_drv_cxt->sensor_ev_info.preview_gain);

    if (preview_mode == capture_mode || s5khm6_longExp_setting_mode[capture_mode]) {
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

    SENSOR_LOGI("capture_shutter = %d, capture_exptime = %llu ns, capture_gain = %d",
                    cap_shutter, cap_exptime, cap_gain);

    s5khm6_drv_calc_exposure(handle, cap_shutter, 0, capture_mode, cap_exptime,
                             &s5khm6_aec_info);
    s5khm6_drv_write_reg2sensor(handle, s5khm6_aec_info.frame_length);
    s5khm6_drv_write_reg2sensor(handle, s5khm6_aec_info.shutter);

    sns_drv_cxt->sensor_ev_info.preview_gain = cap_gain;
    s5khm6_drv_write_gain(handle, &s5khm6_aec_info, cap_gain);
    s5khm6_drv_write_reg2sensor(handle, s5khm6_aec_info.again);
    s5khm6_drv_write_reg2sensor(handle, s5khm6_aec_info.dgain);

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
static cmr_int s5khm6_drv_write_exposure(cmr_handle handle, cmr_uint param) {

    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u32 exposure_line = 0x00;
    cmr_u32 dummy_line = 0x00;
    cmr_u32 size_index = 0x00;
    cmr_u64 exp_time = 0x00;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ex_exposure *ex = (struct sensor_ex_exposure *)param;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    exposure_line = ex->exposure;
    dummy_line = ex->dummy;
    size_index = ex->size_index;
    exp_time = ex->exp_time;

    s5khm6_drv_calc_exposure(handle, exposure_line, dummy_line, size_index, exp_time,
                             &s5khm6_aec_info);
    s5khm6_drv_write_reg2sensor(handle, s5khm6_aec_info.frame_length);
    s5khm6_drv_write_reg2sensor(handle, s5khm6_aec_info.shutter);

    return ret_value;
}

/*==============================================================================
 * Description:
 * write gain value to sensor
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int s5khm6_drv_write_gain_value(cmr_handle handle, cmr_uint param) {

    cmr_int ret_value = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    s5khm6_drv_calc_gain(handle, param, &s5khm6_aec_info);
    s5khm6_drv_write_reg2sensor(handle, s5khm6_aec_info.again);
    s5khm6_drv_write_reg2sensor(handle, s5khm6_aec_info.dgain);

    return ret_value;
}

/*==============================================================================
 * Description:
 * read ae control info
 * please don't change this function unless it's necessary
 *============================================================================*/
static cmr_int s5khm6_drv_read_aec_info(cmr_handle handle, cmr_uint param) {

    cmr_int ret_value = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_aec_reg_info *info = (struct sensor_aec_reg_info *)param;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u64 exposure_line = 0x00;
    cmr_u32 dummy_line = 0x00;
    cmr_u64 exp_time = 0x00;
    cmr_u16 mode = 0x00;

    SENSOR_LOGI("E");

    info->aec_i2c_info_out = &s5khm6_aec_info;
    exposure_line = info->exp.exposure;
    dummy_line = info->exp.dummy;
    mode = info->exp.size_index;
    exp_time = info->exp.exp_time;

    s5khm6_drv_calc_exposure(handle, exposure_line, dummy_line, mode, exp_time,
                             &s5khm6_aec_info);
    s5khm6_drv_calc_gain(handle, info->gain, &s5khm6_aec_info);

    return ret_value;
}

/*==============================================================================
 * Description:
 * mipi stream on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5khm6_drv_stream_on(cmr_handle handle, cmr_uint param) {

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_drv_context *sensor_cxt =
                                    (struct sensor_drv_context *)sns_drv_cxt->caller_handle;

    SENSOR_LOGI("E");

    if (sns_4in1_mode[sensor_cxt->sensor_mode]) {
        s5khm6_drv_write_awb_gain(handle);
    }

    /*TODO*/
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xFCFC, 0x4000);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0103);
    /*END*/
    SENSOR_LOGI("X");
    return SENSOR_SUCCESS;
}

#define STREAMOFF_TIMEOUT 40
static void check_streamoff(cmr_handle handle)
{
    unsigned int i = 0;
    cmr_u16 stream_status = 0;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    usleep(3*1000);
    for (i = 0; i < STREAMOFF_TIMEOUT; i++) {
        stream_status = hw_sensor_grc_read_i2c(sns_drv_cxt->hw_handle, I2C_SLAVE_ADDR >> 1, 0x0005, BITS_ADDR16_REG8);
        if (stream_status == 0xFF)
            break;
        usleep(1*1000);
    }
    SENSOR_LOGI("delay_ms %d", i);
}


/*==============================================================================
 * Description:
 * mipi stream off
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5khm6_drv_stream_off(cmr_handle handle, cmr_uint param) {

    SENSOR_LOGI("E");
    cmr_u16 value = 0;
    cmr_u16 sleep_time = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0000);
    check_streamoff(handle);

    SENSOR_LOGI("X");
    return SENSOR_SUCCESS;
}

static cmr_int
s5khm6_drv_handle_create(struct sensor_ic_drv_init_para *init_param,
                         cmr_handle *sns_ic_drv_handle) {

    cmr_int ret = SENSOR_SUCCESS;
    struct sensor_ic_drv_cxt *sns_drv_cxt = NULL;
    void *pri_data = NULL;

    ret = sensor_ic_drv_create(init_param, sns_ic_drv_handle);
    sns_drv_cxt = *sns_ic_drv_handle;
    s5khm6_sns_drv_cxt = *sns_ic_drv_handle;

    sns_drv_cxt->sensor_ev_info.preview_shutter =
        PREVIEW_FRAME_LENGTH - FRAME_OFFSET;
    sns_drv_cxt->sensor_ev_info.preview_exptime =
        (PREVIEW_FRAME_LENGTH - FRAME_OFFSET) * PREVIEW_LINE_TIME;
    sns_drv_cxt->sensor_ev_info.preview_gain = SENSOR_BASE_GAIN;
    sns_drv_cxt->sensor_ev_info.preview_framelength = PREVIEW_FRAME_LENGTH;

    sns_drv_cxt->frame_length_def = PREVIEW_FRAME_LENGTH;
    sns_drv_cxt->line_time_def = PREVIEW_LINE_TIME;

    s5khm6_drv_write_frame_length(
        sns_drv_cxt, &s5khm6_aec_info,
        sns_drv_cxt->sensor_ev_info.preview_framelength);
    s5khm6_drv_write_gain(sns_drv_cxt, &s5khm6_aec_info,
                          sns_drv_cxt->sensor_ev_info.preview_gain);
    s5khm6_drv_write_shutter(sns_drv_cxt, &s5khm6_aec_info,
                             sns_drv_cxt->sensor_ev_info.preview_shutter);

    sensor_ic_set_match_module_info(sns_drv_cxt,
                                    ARRAY_SIZE(s_s5khm6_module_info_tab),
                                    s_s5khm6_module_info_tab);
    sensor_ic_set_match_resolution_info(sns_drv_cxt,
                                        ARRAY_SIZE(s_s5khm6_resolution_tab_raw),
                                        s_s5khm6_resolution_tab_raw);
    sensor_ic_set_match_trim_info(sns_drv_cxt,
                                  ARRAY_SIZE(s_s5khm6_resolution_trim_tab),
                                  s_s5khm6_resolution_trim_tab);
    sensor_ic_set_match_static_info(
        sns_drv_cxt, ARRAY_SIZE(s_s5khm6_static_info), s_s5khm6_static_info);
/*    sensor_ic_set_match_fov_info(
        sns_drv_cxt, ARRAY_SIZE(s_s5khm6_fov_info), s_s5khm6_fov_info, (void*)&s_s5khm6_exif_info);*/
    sensor_ic_set_match_fps_info(sns_drv_cxt,
                                 ARRAY_SIZE(s_s5khm6_mode_fps_info),
                                 s_s5khm6_mode_fps_info);

    /*init exif info,this will be deleted in the future*/
    s5khm6_drv_init_fps_info(sns_drv_cxt);

    /*add private here*/
    return ret;
}

static cmr_int s5khm6_drv_handle_delete(cmr_handle handle, void *param) {

    cmr_int ret = SENSOR_SUCCESS;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    ret = sensor_ic_drv_delete(handle, param);

    return ret;
}

static cmr_int s5khm6_drv_get_private_data(cmr_handle handle, cmr_uint cmd,
                                           void **param) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);

    ret = sensor_ic_get_private_data(handle, cmd, param);

    return ret;
}

void *sensor_ic_open_lib(void)
{
     return &g_s5khm6_mipi_raw_info;
}

/*==============================================================================
 * Description:
 * all ioctl functoins
 * you can add functions reference SENSOR_IOCTL_FUNC_TAB_T from sensor_drv_u.h
 *
 * add ioctl functions like this:
 * .power = s5khm6_power_on,
 *============================================================================*/
static struct sensor_ic_ops s_s5khm6_ops_tab = {
    .create_handle = s5khm6_drv_handle_create,
    .delete_handle = s5khm6_drv_handle_delete,
    /*get privage data*/
    .get_data = s5khm6_drv_get_private_data,
    /*common interface*/
    .power = s5khm6_drv_power_on,
    .identify = s5khm6_drv_identify,
    .ex_write_exp = s5khm6_drv_write_exposure,
    .write_gain_value = s5khm6_drv_write_gain_value,
    .read_aec_info = s5khm6_drv_read_aec_info,
    .ext_ops = {
            [SENSOR_IOCTL_BEFORE_SNAPSHOT].ops = s5khm6_drv_before_snapshot,
            [SENSOR_IOCTL_STREAM_ON].ops = s5khm6_drv_stream_on,
            [SENSOR_IOCTL_STREAM_OFF].ops = s5khm6_drv_stream_off,
            /* expand interface,if you want to add your sub cmd ,
             *  you can add it in enum {@SENSOR_IOCTL_VAL_TYPE}
            */
            [SENSOR_IOCTL_FAST_SWITCH].ops = s5khm6_drv_fast_switch,
            [SENSOR_IOCTL_ACCESS_VAL].ops = s5khm6_drv_access_val,
    }};
