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

#define LOG_TAG "ov16885_normal"

#include "sensor_ov16885_normal_mipi_raw.h"

/*==============================================================================
 * Description:
 * write register value to sensor
 * please modify this function acording your spec
 *============================================================================*/

static void ov16885_normal_drv_write_reg2sensor(cmr_handle handle, struct sensor_i2c_reg_tab *reg_info)
{
    SENSOR_IC_CHECK_PTR_VOID(reg_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);

    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_int i=0;

    for(i=0;i<reg_info->size;i++){
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, reg_info->settings[i].reg_addr, reg_info->settings[i].reg_value);
    }
}


/*==============================================================================
 * Description:
 * write gain to sensor registers buffer
 * please modify this function acording your spec
 *============================================================================*/
static void ov16885_normal_drv_write_gain(cmr_handle handle, struct sensor_aec_i2c_tag *aec_info, cmr_u32 gain)
{
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if(aec_info->again->size)
    {
        /*TODO*/
        aec_info->again->settings[0].reg_value = (gain >> 8) & 0x1f;
        aec_info->again->settings[1].reg_value = gain & 0xff;
        /*END*/
    }

    if(aec_info->dgain->size){

        /*TODO*/

        /*END*/
    }
}


/*==============================================================================
 * Description:
 * write frame length to sensor registers buffer
 * please modify this function acording your spec
 *============================================================================*/
static void ov16885_normal_drv_write_frame_length(cmr_handle handle, struct sensor_aec_i2c_tag *aec_info, cmr_u32 frame_len)
{
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if(aec_info->frame_length->size){
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
static void ov16885_normal_drv_write_shutter(cmr_handle handle, struct sensor_aec_i2c_tag *aec_info , cmr_u32 shutter)
{
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if(aec_info->shutter->size){
        /*TODO*/
        aec_info->shutter->settings[0].reg_value = (shutter >> 0x0c) & 0x0f;
        aec_info->shutter->settings[1].reg_value = (shutter >> 0x04) & 0xff;
        aec_info->shutter->settings[2].reg_value = (shutter << 0x04) & 0xff;

        /*END*/
    }
}

/*==============================================================================
 * Description:
 * write exposure to sensor registers and get current shutter
 * please pay attention to the frame length
 * please don't change this function if it's necessary
 *============================================================================*/
static void ov16885_normal_drv_calc_exposure(cmr_handle handle, cmr_u32 shutter, cmr_u32 dummy_line, 
                                                  cmr_u16 mode, struct sensor_aec_i2c_tag *aec_info)
{
    cmr_u32 dest_fr_len = 0;
    cmr_u32 cur_fr_len = 0;
    cmr_u32 fr_len = 0;
    float fps = 0.0;
    cmr_u16 frame_interval = 0x00;

    SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    sns_drv_cxt->frame_length_def = sns_drv_cxt->trim_tab_info[mode].frame_line;
    sns_drv_cxt->line_time_def = sns_drv_cxt->trim_tab_info[mode].line_time;
    cur_fr_len = sns_drv_cxt->sensor_ev_info.preview_framelength;
    fr_len = sns_drv_cxt->frame_length_def;

    dummy_line = dummy_line > FRAME_OFFSET ? dummy_line : FRAME_OFFSET;
    dest_fr_len = ((shutter + dummy_line) > fr_len) ? (shutter +dummy_line) : fr_len;
    sns_drv_cxt->frame_length = dest_fr_len;

    if (shutter < SENSOR_MIN_SHUTTER)
        shutter = SENSOR_MIN_SHUTTER;

    if (cur_fr_len > shutter) {
     fps = 1000000000.0 / (cur_fr_len * sns_drv_cxt->trim_tab_info[mode].line_time);
    } else {
     fps = 1000000000.0 / ((shutter + dummy_line) * sns_drv_cxt->trim_tab_info[mode].line_time);
    }
   // SENSOR_LOGI("fps = %f", fps);

    frame_interval = (cmr_u16)(((shutter + dummy_line) *
               sns_drv_cxt->line_time_def) / 1000000);
    SENSOR_LOGI("mode = %d, exposure_line = %d, dummy_line= %d, frame_interval= %d ms",
                    mode, shutter, dummy_line, frame_interval);

    if (dest_fr_len != cur_fr_len){
        sns_drv_cxt->sensor_ev_info.preview_framelength = dest_fr_len;
        ov16885_normal_drv_write_frame_length(handle, aec_info, dest_fr_len);
    }
    sns_drv_cxt->sensor_ev_info.preview_shutter = shutter;
    ov16885_normal_drv_write_shutter(handle, aec_info, shutter);
}


static void ov16885_normal_drv_calc_gain(cmr_handle handle,cmr_uint isp_gain, struct sensor_aec_i2c_tag *aec_info)
{
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u32 sensor_gain = 0;

    sensor_gain = isp_gain < ISP_BASE_GAIN ? ISP_BASE_GAIN : isp_gain;
    sensor_gain = sensor_gain * SENSOR_BASE_GAIN / ISP_BASE_GAIN;

    if (SENSOR_MAX_GAIN < sensor_gain)
            sensor_gain = SENSOR_MAX_GAIN;

    SENSOR_LOGI("isp_gain = 0x%x,sensor_gain=0x%x", (unsigned int)isp_gain,sensor_gain);

    sns_drv_cxt->sensor_ev_info.preview_gain = sensor_gain;
    ov16885_normal_drv_write_gain(handle, aec_info, sensor_gain);
}

/*==============================================================================
 * Description:
 * sensor power on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int ov16885_normal_drv_power_on(cmr_handle handle, cmr_uint power_on)
{
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;

    SENSOR_AVDD_VAL_E dvdd_val = module_info->dvdd_val;
    SENSOR_AVDD_VAL_E avdd_val = module_info->avdd_val;
    SENSOR_AVDD_VAL_E iovdd_val = module_info->iovdd_val;
    BOOLEAN power_down = g_ov16885_normal_mipi_raw_info.power_down_level;
    BOOLEAN reset_level = g_ov16885_normal_mipi_raw_info.reset_pulse_level;    

    if (SENSOR_TRUE == power_on)
    {
        hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        //hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);

        usleep(1 * 1000);
        hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, iovdd_val);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, avdd_val);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, dvdd_val);
        hw_sensor_power_down(sns_drv_cxt->hw_handle, !power_down);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, !reset_level);

        usleep(5 * 1000);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, EX_MCLK);
        hw_sensor_set_mipi_level(sns_drv_cxt->hw_handle, 1);
        usleep(1 * 1000);
    } else
    {    
        hw_sensor_set_mipi_level(sns_drv_cxt->hw_handle, 0);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
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
static cmr_int ov16885_normal_drv_init_fps_info(cmr_handle handle) 
{
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
    struct sensor_trim_tag *trim_info = sns_drv_cxt->trim_tab_info;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;

  //  SENSOR_LOGI("E");
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
                if (fps_info->sensor_mode_fps[i].max_fps > static_info->max_fps) {
                    static_info->max_fps = fps_info->sensor_mode_fps[i].max_fps;
                }
            }
            SENSOR_LOGI("mode %d,tempfps %d,frame_len %d,line_time: %d ", i, tempfps, 
                       trim_info[i].frame_line, trim_info[i].line_time);
            SENSOR_LOGI("mode %d,max_fps: %d ", i,
                         fps_info->sensor_mode_fps[i].max_fps);
            SENSOR_LOGI("is_high_fps: %d,highfps_skip_num %d",
                fps_info->sensor_mode_fps[i].is_high_fps,
                fps_info->sensor_mode_fps[i].high_fps_skip_num);
        }
        fps_info->is_init = 1;
    }
  //  SENSOR_LOGI("X");
    return rtn;
}


static cmr_int ov16885_normal_drv_init_exif_info(cmr_handle handle, void **exif_info_in /*in*/) 
{
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    *exif_info_in = NULL;
    EXIF_SPEC_PIC_TAKING_COND_T *exif_ptr = NULL;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;

    rtn = sensor_ic_get_init_exif_info(sns_drv_cxt, &exif_ptr);
    SENSOR_IC_CHECK_PTR(exif_ptr);
    *exif_info_in = exif_ptr;

   // SENSOR_LOGI("Start");
    exif_ptr->valid.FNumber = 1;
    exif_ptr->FNumber.numerator = static_info->f_num;
    exif_ptr->FNumber.denominator = 100;
    exif_ptr->valid.ApertureValue = 1;
    exif_ptr->ApertureValue.numerator = static_info->f_num;
    exif_ptr->ApertureValue.denominator = 100;
    exif_ptr->valid.MaxApertureValue = 1;
    exif_ptr->MaxApertureValue.numerator = static_info->f_num;
    exif_ptr->MaxApertureValue.denominator = 100;
    exif_ptr->valid.FocalLength = 1;
    exif_ptr->FocalLength.numerator = static_info->focal_length;
    exif_ptr->FocalLength.denominator = 100;
    
    return rtn;
}


static cmr_int ov16885_normal_drv_get_static_info(cmr_handle handle, cmr_u32 *param) 
{
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_ex_info *ex_info = (struct sensor_ex_info *)param;
    cmr_u32 up = 0;
    cmr_u32 down = 0;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(ex_info);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;

    // make sure we have get max fps of all settings.
    if (!fps_info->is_init) {
        ov16885_normal_drv_init_fps_info(handle);
    }
    ex_info->f_num = static_info->f_num;
    ex_info->focal_length = static_info->focal_length;
    ex_info->min_focus_distance = static_info->min_focal_distance;
    ex_info->start_offset_time = static_info->start_offset_time;
    ex_info->max_fps = static_info->max_fps;
    ex_info->max_adgain = static_info->max_adgain;
    ex_info->ois_supported = static_info->ois_supported;
    ex_info->pdaf_supported = static_info->pdaf_supported;
    ex_info->exp_valid_frame_num = static_info->exp_valid_frame_num;
    ex_info->clamp_level = static_info->clamp_level;
    ex_info->adgain_valid_frame_num = static_info->adgain_valid_frame_num;
    ex_info->preview_skip_num = module_info->preview_skip_num;
    ex_info->capture_skip_num = module_info->capture_skip_num;
    ex_info->name = (cmr_s8 *)g_ov16885_normal_mipi_raw_info.name;
    ex_info->sensor_version_info = (cmr_s8 *)g_ov16885_normal_mipi_raw_info.sensor_version_info;
    ex_info->fov_angle = static_info->fov_angle;
    memcpy(&ex_info->fov_info, &static_info->fov_info, sizeof(static_info->fov_info));
    ex_info->pos_dis.up2hori = up;
    ex_info->pos_dis.hori2down = down;
    sensor_ic_print_static_info((cmr_s8 *)SENSOR_NAME, ex_info);
    ex_info->sns_binning_factor = sns_binning_fact;

    return rtn;
}


static cmr_int ov16885_normal_drv_get_fps_info(cmr_handle handle, cmr_u32 *param) 
{
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_MODE_FPS_T *fps_info = (SENSOR_MODE_FPS_T *)param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(fps_info);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_fps_info *fps_data = sns_drv_cxt->fps_info;

    // make sure have inited fps of every sensor mode.
    if (!fps_data->is_init) {
        ov16885_normal_drv_init_fps_info(handle);
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

static const cmr_u16 ov16885_normal_pd_is_right[] = {1, 0, 0, 1};

static const cmr_u16 ov16885_normal_pd_col[] = {14, 14, 6, 6};

static const cmr_u16 ov16885_normal_pd_row[] = {2, 6, 10, 14};

static const struct pd_pos_info ov16885_normal_pd_pos_l[] = {
    {14, 6}, {6, 10},
};

static const struct pd_pos_info ov16885_normal_pd_pos_r[] = {
    {14, 2}, {6, 14},
};
static const cmr_u32 ov16885_normal_pd_sns_mode[] = {
    SENSOR_PDAF_MODE_DISABLE, SENSOR_PDAF_MODE_DISABLE,
    SENSOR_PDAF_MODE_DISABLE, SENSOR_PDAF_MODE_ENABLE
};

struct sensor_pdaf_info ov16885_normal_pdaf_info_array[] = {
	/* pd setting for sensor mode0 */
	{0},
	/* pd setting for sensor mode1 */
	{0},
	/* pd setting for sensor mode2 */
	{0},
    /* pd setting for sensor mode3 */
    {
	.pd_is_right = (cmr_u16 *)ov16885_normal_pd_is_right,
	.pd_pos_row = (cmr_u16 *)ov16885_normal_pd_row,
	.pd_pos_col = (cmr_u16 *)ov16885_normal_pd_col,
	.pd_pos_r = (struct pd_pos_info *)ov16885_normal_pd_pos_r,
	.pd_pos_l = (struct pd_pos_info *)ov16885_normal_pd_pos_l,
	.pd_pos_size = NUMBER_OF_ARRAY(ov16885_normal_pd_pos_r),
	.pd_offset_x = 0,
	.pd_offset_y = 0,
	.pd_end_x = 4672,
	.pd_end_y = 3504,
	.pd_block_w = 1,
	.pd_block_h = 1,
	.pd_block_num_x = 292,
	.pd_block_num_y = 219,
	.pd_density_x = 16,
	.pd_density_y = 8,
	.pd_pitch_x = 16,
	.pd_pitch_y = 16,
	.vch2_info.bypass = 0,
	.vch2_info.vch2_vc = 0,
	.vch2_info.vch2_data_type = 0x2b,
	.vch2_info.vch2_mode = 0x03,
	.sns_mode = ov16885_normal_pd_sns_mode,
	.sns_orientation = 0, /*1: mirror+flip; 0: normal*/
	.pd_size_w = SNAPSHOT_WIDTH,
	.pd_size_h = SNAPSHOT_HEIGHT,
	.image_process_type = 0, // 0:default; 1:only crop; 2:binning+crop;
	.crop_offset_x = 0,
	.crop_offset_y = 0,
	},
    
	/* add more pdaf setting here */
};

static struct sensor_pdaf_info_tab ov16885_normal_pdaf_info_tab = {
    .sns_mode_num = NUMBER_OF_ARRAY(ov16885_normal_pd_sns_mode),
	.pd_sns_mode = ov16885_normal_pd_sns_mode,
	.pdaf_init_index = 3,
	.pd_init_size_w = SNAPSHOT_WIDTH,
	.pd_init_size_h = SNAPSHOT_HEIGHT,
	.pd_info_tab = ov16885_normal_pdaf_info_array,
};

static cmr_int ov16885_normal_drv_get_pdaf_info(cmr_handle handle, void *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_pdaf_info_tab *pdaf_info_tab = NULL;
    struct sensor_pdaf_info *pdaf_info = NULL;
    cmr_int pd_info_num = 0;
    int i = 0;
    SENSOR_IC_CHECK_PTR(param);

    SENSOR_LOGV("E");

    pdaf_info_tab = (struct sensor_pdaf_info_tab *)param;
    
    pdaf_info_tab->sns_mode_num = ov16885_normal_pdaf_info_tab.sns_mode_num;
    pdaf_info_tab->pd_sns_mode = ov16885_normal_pdaf_info_tab.pd_sns_mode;
    pdaf_info_tab->pdaf_init_index = ov16885_normal_pdaf_info_tab.pdaf_init_index;
    pdaf_info_tab->pd_init_size_w = ov16885_normal_pdaf_info_tab.pd_init_size_w;
    pdaf_info_tab->pd_init_size_h = ov16885_normal_pdaf_info_tab.pd_init_size_h;
    pdaf_info_tab->pd_info_tab = ov16885_normal_pdaf_info_tab.pd_info_tab;

    pd_info_num = ov16885_normal_pdaf_info_tab.sns_mode_num;
    for(i = 0; i < pd_info_num; i++) {
        if(pdaf_info_tab->pd_sns_mode[i]){
            pdaf_info = &(pdaf_info_tab->pd_info_tab[i]);
            pdaf_info->pd_data_size = pdaf_info->pd_block_num_x * pdaf_info->pd_block_num_y *
                                      pdaf_info->pd_pos_size * 2 * 5;
            SENSOR_LOGD("pd data size:%d", pdaf_info->pd_data_size);
        }
    }
    SENSOR_LOGD("sensor pdaf_init_index %d", pdaf_info_tab->pdaf_init_index);
    return rtn;
}


static cmr_int ov16885_normal_drv_get_brightness(cmr_handle handle, cmr_u32 *param) {
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    *param = 20;// this value 20 is only for test
    SENSOR_LOGI("SENSOR: get_brightness: lumma = 0x%x\n", *param);

    return 0;
}

static cmr_s64 ov16885_normal_drv_get_shutter_skew(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    cmr_u16 height = 0;
    cmr_u32 line_time = 0;
    cmr_s64 shutter_skew = 0;
    SENSOR_IC_CHECK_PTR(param);
    SENSOR_LOGI("E\n");
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_shutter_skew_info *shutter_skew_info =
        (struct sensor_shutter_skew_info *)param;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    height = s_ov16885_normal_resolution_tab_raw[0].reg_tab[shutter_skew_info->sns_mode].height;
    line_time = s_ov16885_normal_resolution_trim_tab[0].trim_info[shutter_skew_info->sns_mode].line_time;
    shutter_skew = (height - 1) * line_time;
    shutter_skew_info->shutter_skew = shutter_skew;
    SENSOR_LOGI("sensor_mode:%d, height:%d, line_time:%d, shutter_skew:%d",
                shutter_skew_info->sns_mode, height, line_time, shutter_skew);
    return rtn;
}

static cmr_int ov16885_normal_drv_dump_trace(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    cmr_u16 value1 = 0, value2 = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    value1 = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3862);
    value2 = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3863);
    SENSOR_LOGI("FRM_CNT 0x3862:0x%x 0x3863:0x%x", value1, value2);

    return rtn;
}

/*==============================================================================
 * Description:
 * cfg otp setting
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int ov16885_normal_drv_access_val(cmr_handle handle, cmr_uint param)
{
    cmr_int ret = SENSOR_FAIL;
    SENSOR_VAL_T *param_ptr = (SENSOR_VAL_T *)param;

    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param_ptr);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGV("ov16885 sensor: param_ptr->type=%x", param_ptr->type);

    switch(param_ptr->type)
    {
        case SENSOR_VAL_TYPE_GET_STATIC_INFO:
            ret = ov16885_normal_drv_get_static_info(handle, param_ptr->pval);
            break;
        case SENSOR_VAL_TYPE_GET_FPS_INFO:
            ret = ov16885_normal_drv_get_fps_info(handle, param_ptr->pval);
            break;
        case SENSOR_VAL_TYPE_SET_SENSOR_CLOSE_FLAG:
            ret = sns_drv_cxt->is_sensor_close = 1;
            break;
        case SENSOR_VAL_TYPE_GET_PDAF_INFO:
            ret = ov16885_normal_drv_get_pdaf_info(handle, param_ptr->pval);
            break;
        case SENSOR_VAL_TYPE_GET_BV:
            //ret = ov16885_normal_drv_get_brightness(handle, param_ptr->pval);
            break;
        case SENSOR_VAL_TYPE_GET_SHUTTER_SKEW_DATA:
            ret = ov16885_normal_drv_get_shutter_skew(handle, param_ptr->pval);
            break;
        case SENSOR_VAL_TYPE_DUMP_TRACE:
            ret = ov16885_normal_drv_dump_trace(handle, param_ptr->pval);
            break;
        default:
            break;
    }

    return ret;
}

static cmr_u8 ov16885_normal_snspid_is_init = 0;

static cmr_int ov16885_normal_drv_save_snspid(cmr_handle handle) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u8 snspid_size = 32;
    cmr_u8 snspid[32] = {0};

    SENSOR_LOGI("E");

    for (int i = 0; i < 32; i++) {
        snspid[i] = 0;
    }
    if (sns_drv_cxt->ops_cb.set_snspid) {
        sns_drv_cxt->ops_cb.set_snspid(
            sns_drv_cxt->caller_handle, sns_drv_cxt->sensor_id, snspid, snspid_size);
    }

    ov16885_normal_snspid_is_init = 1;
    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * identify sensor id
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int ov16885_normal_drv_identify(cmr_handle handle, cmr_uint param)
{
    cmr_u16 pid_value = 0x00;
    cmr_u16 ver_value = 0x00;
    cmr_int ret_value = SENSOR_FAIL;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("mipi raw identify");

    pid_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, ov16885_normal_PID_ADDR);

    if (ov16885_normal_PID_VALUE == pid_value) {
        ver_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, ov16885_normal_VER_ADDR);
        SENSOR_LOGI("Identify: pid_value = %x, ver_value = %x", pid_value, ver_value);
        if (ov16885_normal_VER_VALUE == ver_value) {
            SENSOR_LOGI("this is ov16885 sensor");
            ret_value = SENSOR_SUCCESS;
        if (!ov16885_normal_snspid_is_init) {
             ov16885_normal_drv_save_snspid(handle);
            }
        } else {
            SENSOR_LOGI("sensor identify fail, pid_value = %x, ver_value = %x", pid_value, ver_value);
        }
    } else {
        SENSOR_LOGI("sensor identify fail, pid_value = %x, ver_value = %x", pid_value, ver_value);
    }

    return ret_value;
}


/*==============================================================================
 * Description:
 * before snapshot
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int ov16885_normal_drv_before_snapshot(cmr_handle handle, cmr_uint param)
{
    cmr_u32 cap_shutter = 0;
    cmr_u32 prv_shutter = 0;
    cmr_u32 prv_gain = 0;
    cmr_u32 cap_gain = 0;
    cmr_u32 capture_mode = param & 0xffff;
    cmr_u32 preview_mode = (param >> 0x10) & 0xffff;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    cmr_u32 prv_linetime = sns_drv_cxt->trim_tab_info[preview_mode].line_time;
    cmr_u32 cap_linetime = sns_drv_cxt->trim_tab_info[capture_mode].line_time;

    SENSOR_LOGI("preview_mode=%d,capture_mode = %d", preview_mode, capture_mode);
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

    if(sns_drv_cxt->ops_cb.set_mode)
        sns_drv_cxt->ops_cb.set_mode(sns_drv_cxt->caller_handle, capture_mode);
    if(sns_drv_cxt->ops_cb.set_mode_wait_done)
        sns_drv_cxt->ops_cb.set_mode_wait_done(sns_drv_cxt->caller_handle);

    cap_shutter = prv_shutter * prv_linetime / cap_linetime * BINNING_FACTOR;
    cap_gain = prv_gain;

    SENSOR_LOGI("capture_shutter = 0x%x, capture_gain = 0x%x", cap_shutter, cap_gain);

    ov16885_normal_drv_calc_exposure(handle,cap_shutter, 0 , capture_mode, &ov16885_normal_aec_info);
    ov16885_normal_drv_write_reg2sensor(handle, ov16885_normal_aec_info.frame_length);
    ov16885_normal_drv_write_reg2sensor(handle, ov16885_normal_aec_info.shutter);

    sns_drv_cxt->sensor_ev_info.preview_gain=cap_gain;
    ov16885_normal_drv_write_gain(handle, &ov16885_normal_aec_info, cap_gain);
    ov16885_normal_drv_write_reg2sensor(handle, ov16885_normal_aec_info.again);
    ov16885_normal_drv_write_reg2sensor(handle, ov16885_normal_aec_info.dgain);

snapshot_info:
    if(sns_drv_cxt->ops_cb.set_exif_info) {
       // sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle, SENSOR_EXIF_CTRL_EXPOSURETIME, cap_shutter);
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
static cmr_int ov16885_normal_drv_write_exposure(cmr_handle handle, cmr_uint param)
{
    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 size_index = 0x00;

    struct sensor_ex_exposure *ex = (struct sensor_ex_exposure *)param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_HANDLE(ex);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    exposure_line = ex->exposure;
    dummy_line = ex->dummy;
    size_index = ex->size_index;

    ov16885_normal_drv_calc_exposure(handle,exposure_line, dummy_line, size_index, &ov16885_normal_aec_info);
    ov16885_normal_drv_write_reg2sensor(handle, ov16885_normal_aec_info.frame_length);
    ov16885_normal_drv_write_reg2sensor(handle, ov16885_normal_aec_info.shutter);

    return ret_value;
}

/*==============================================================================
 * Description:
 * write gain value to sensor
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int ov16885_normal_drv_write_gain_value(cmr_handle handle, cmr_uint param)
{
    cmr_int ret_value = SENSOR_SUCCESS;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    ov16885_normal_drv_calc_gain(handle,param, &ov16885_normal_aec_info);
    ov16885_normal_drv_write_reg2sensor(handle, ov16885_normal_aec_info.again);
    ov16885_normal_drv_write_reg2sensor(handle, ov16885_normal_aec_info.dgain);

    return ret_value;
}


/*==============================================================================
 * Description:
 * read ae control info
 * please don't change this function unless it's necessary
 *============================================================================*/
static cmr_int ov16885_normal_drv_read_aec_info(cmr_handle handle, cmr_uint param)
{
    cmr_int ret_value = SENSOR_SUCCESS;
    struct sensor_aec_reg_info *info = (struct sensor_aec_reg_info *)param;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 mode = 0x00;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(info);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  //  SENSOR_LOGI("E");

    info->aec_i2c_info_out = &ov16885_normal_aec_info;
    exposure_line = info->exp.exposure;
    dummy_line = info->exp.dummy;
    mode = info->exp.size_index;

    ov16885_normal_drv_calc_exposure(handle, exposure_line, dummy_line, mode, &ov16885_normal_aec_info);
    ov16885_normal_drv_calc_gain(handle,info->gain, &ov16885_normal_aec_info);

    return ret_value;
}

static cmr_int ov16885_normal_drv_set_master_FrameSync(cmr_handle handle, cmr_uint param)
{
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("E");

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3002, 0x88);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3008, 0x00);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x382F, 0x84);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3830, 0x17);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3831, 0x4b);

    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * mipi stream on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int ov16885_normal_drv_stream_on(cmr_handle handle, cmr_uint param)
{
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("E");

    char value1[PROPERTY_VALUE_MAX];
    property_get("persist.vendor.cam.colorbar", value1, "0");
    if (!strcmp(value1, "1")) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x5081, 0x01);
    }

    char value2[PROPERTY_VALUE_MAX];
    property_get("vendor.cam.hw.framesync.on", value2, "1");
    if (!strcmp(value2, "1")) {
#if defined(CONFIG_DUAL_MODULE)
        if (sns_drv_cxt->is_multi_mode == MODE_BOKEH || MODE_PORTRAIT == sns_drv_cxt->is_multi_mode) {
            ov16885_normal_drv_set_master_FrameSync(handle, param);
        }
#endif
    }

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x01);

    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * mipi stream off
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int ov16885_normal_drv_stream_off(cmr_handle handle, cmr_uint param)
{
    SENSOR_LOGI("E");
    unsigned char value = 0;
    cmr_s32 ret = SENSOR_SUCCESS;
    cmr_u16 status_l = 0;
    cmr_u16 status_h = 0;
    cmr_u16 status = 0;
    clock_t start_clk, end_clk;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0100);
    if (value != 0x00) {
        ret = hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x00);
        start_clk = end_clk = clock();
        while(1) {
            status_h = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x485E);
            status_l = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x485F);
            status = (status_h << 8) | (status_l & 0xff);
            if(status == 0) {
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
        ret = hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x00);
    }
exit:
    sns_drv_cxt->is_sensor_close = 0;

    SENSOR_LOGI("X");
    return ret;
}

static cmr_int ov16885_normal_drv_handle_create(struct sensor_ic_drv_init_para *init_param, cmr_handle* sns_ic_drv_handle)
{
    cmr_int ret = SENSOR_SUCCESS;
    struct sensor_ic_drv_cxt * sns_drv_cxt = NULL;
    void *pri_data = NULL;

    ret = sensor_ic_drv_create(init_param,sns_ic_drv_handle);
    sns_drv_cxt = *sns_ic_drv_handle;

    sns_drv_cxt->sensor_ev_info.preview_shutter = PREVIEW_FRAME_LENGTH - FRAME_OFFSET;
    sns_drv_cxt->sensor_ev_info.preview_gain = SENSOR_BASE_GAIN;
    sns_drv_cxt->sensor_ev_info.preview_framelength = PREVIEW_FRAME_LENGTH;

    sns_drv_cxt->frame_length_def = PREVIEW_FRAME_LENGTH;
    sns_drv_cxt->line_time_def = PREVIEW_LINE_TIME;

    ov16885_normal_drv_write_frame_length(sns_drv_cxt, &ov16885_normal_aec_info, sns_drv_cxt->sensor_ev_info.preview_framelength);
    ov16885_normal_drv_write_gain(sns_drv_cxt, &ov16885_normal_aec_info, sns_drv_cxt->sensor_ev_info.preview_gain);
    ov16885_normal_drv_write_shutter(sns_drv_cxt, &ov16885_normal_aec_info, sns_drv_cxt->sensor_ev_info.preview_shutter);

    sensor_ic_set_match_module_info(sns_drv_cxt, ARRAY_SIZE(s_ov16885_normal_module_info_tab), s_ov16885_normal_module_info_tab);
    sensor_ic_set_match_resolution_info(sns_drv_cxt, ARRAY_SIZE(s_ov16885_normal_resolution_tab_raw), s_ov16885_normal_resolution_tab_raw);
    sensor_ic_set_match_trim_info(sns_drv_cxt, ARRAY_SIZE(s_ov16885_normal_resolution_trim_tab), s_ov16885_normal_resolution_trim_tab);
    sensor_ic_set_match_static_info(sns_drv_cxt, ARRAY_SIZE(s_ov16885_normal_static_info), s_ov16885_normal_static_info);
    sensor_ic_set_match_fps_info(sns_drv_cxt, ARRAY_SIZE(s_ov16885_normal_mode_fps_info), s_ov16885_normal_mode_fps_info);

    /*init exif info,this will be deleted in the future*/
    ov16885_normal_drv_init_fps_info(sns_drv_cxt);
    /*get exif pointer*/    
	sns_drv_cxt->exif_ptr = (void*)&ov16885_normal_drv_init_exif_info;
    ov16885_normal_drv_init_exif_info(sns_drv_cxt, &sns_drv_cxt->exif_ptr);

    /*add private here*/
    return ret;
}


static cmr_int ov16885_normal_drv_handle_delete(cmr_handle handle, void *param)
{
    cmr_int ret = SENSOR_SUCCESS;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    ret = sensor_ic_drv_delete(handle,param);
    return ret;
}


static cmr_int ov16885_normal_drv_get_private_data(cmr_handle handle, cmr_uint cmd, void**param)
{
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);

    ret = sensor_ic_get_private_data(handle,cmd, param);
    return ret;
}

void *sensor_ic_open_lib(void)
{
     return &g_ov16885_normal_mipi_raw_info;
}
/*==============================================================================
 * Description:
 * all ioctl functoins
 * you can add functions reference SENSOR_IOCTL_FUNC_TAB_T from sensor_drv_u.h
 *
 * add ioctl functions like this:
 * .power = ov16885_normal_power_on,
 *============================================================================*/
static struct sensor_ic_ops s_ov16885_normal_ops_tab = {
    .create_handle = ov16885_normal_drv_handle_create,
    .delete_handle = ov16885_normal_drv_handle_delete,
    /*get privage data*/
    .get_data = ov16885_normal_drv_get_private_data,
    /*common interface*/
    .power = ov16885_normal_drv_power_on,
    .identify = ov16885_normal_drv_identify,
    .ex_write_exp = ov16885_normal_drv_write_exposure,
    .write_gain_value = ov16885_normal_drv_write_gain_value,
#if defined(CONFIG_DUAL_MODULE)
    .read_aec_info = ov16885_normal_drv_read_aec_info,
#endif
    .ext_ops = {
        [SENSOR_IOCTL_BEFORE_SNAPSHOT].ops = ov16885_normal_drv_before_snapshot,
        [SENSOR_IOCTL_STREAM_ON].ops = ov16885_normal_drv_stream_on,
        [SENSOR_IOCTL_STREAM_OFF].ops = ov16885_normal_drv_stream_off,
        /* expand interface,if you want to add your sub cmd ,
         *  you can add it in enum {@SENSOR_IOCTL_VAL_TYPE}
         */
        [SENSOR_IOCTL_ACCESS_VAL].ops = ov16885_normal_drv_access_val,
    }
};
