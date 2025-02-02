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

#define LOG_TAG "c3z03_gc08a8_frontcxt"  

#include "c3z03_gc08a8_frontcxt.h"

pthread_mutex_t gc08a8_sensor_mutex;

/*==============================================================================
 * Description:
 * write register value to sensor
 * please modify this function acording your spec
 *============================================================================*/

static void gc08a8_drv_write_reg2sensor(cmr_handle handle,struct sensor_i2c_reg_tab *reg_info)
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
static void gc08a8_drv_write_gain(cmr_handle handle, struct sensor_aec_i2c_tag *aec_info, cmr_u32 gain)
{
	SENSOR_IC_CHECK_PTR_VOID(aec_info);
	SENSOR_IC_CHECK_HANDLE_VOID(handle);
	struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

	
	if (aec_info->again->size) {
		gain = (gain < SENSOR_BASE_GAIN) ? SENSOR_BASE_GAIN : gain;
		gain = (gain > SENSOR_MAX_GAIN) ? SENSOR_MAX_GAIN : gain;
		aec_info->again->settings[0].reg_value = (gain >> 8) & 0xff;
		aec_info->again->settings[1].reg_value = gain & 0xff;
	}

	SENSOR_LOGI("sensor gain = 0x%x , 0x0204 = 0x%x", gain , aec_info->again->settings[0].reg_value);

}

static cmr_u16 gc08a8_drv_read_frame_length(cmr_handle handle)
{
	SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	cmr_u16 frame_len_h=0;
	cmr_u16	frame_len_l=0;

	frame_len_h = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0340)&0xff;
	frame_len_l = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0341)&0xff;

    return ((frame_len_h << 8) | frame_len_l);

}
/*==============================================================================
 * Description:
 * write frame length to sensor registers buffer
 * please modify this function acording your spec
 *============================================================================*/
static void gc08a8_drv_write_frame_length(cmr_handle handle, struct sensor_aec_i2c_tag *aec_info, cmr_u32 frame_len)
{
    SENSOR_IC_CHECK_PTR_VOID(aec_info);
	SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

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
static void gc08a8_drv_write_shutter(cmr_handle handle, struct sensor_aec_i2c_tag *aec_info, cmr_u32 shutter)
{
	SENSOR_IC_CHECK_PTR_VOID(aec_info);
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

	if(shutter < 2) shutter = 2;
	if(shutter > 65519) shutter = 65519;

	if(aec_info->shutter->size){
		/*TODO*/
		aec_info->shutter->settings[0].reg_value = (shutter >> 8) & 0xff;
		aec_info->shutter->settings[1].reg_value = shutter & 0xff;
		
		/*END*/
	}
}

/*==============================================================================
 * Description:
 * write exposure to sensor registers and get current shutter
 * please pay attention to the frame length
 * please don't change this function if it's necessary
 *============================================================================*/
static void gc08a8_drv_calc_exposure(cmr_handle handle, cmr_u32 shutter,cmr_u32 dummy_line, 
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
    dest_fr_len = (dest_fr_len > 65535) ? 65535 : dest_fr_len;
    sns_drv_cxt->frame_length = dest_fr_len;

	shutter = shutter < SENSOR_MIN_SHUTTER ? SENSOR_MIN_SHUTTER : shutter;

	if (cur_fr_len > shutter) {
	 fps = 1000000000.0 / (cur_fr_len * sns_drv_cxt->trim_tab_info[mode].line_time);
	} else {
	 fps = 1000000000.0 / ((shutter + dummy_line) * sns_drv_cxt->trim_tab_info[mode].line_time);
	}
	SENSOR_LOGI("fps = %f", fps);

    frame_interval = (cmr_u16)(((shutter + dummy_line) *
               sns_drv_cxt->line_time_def) / 1000000);
	SENSOR_LOGI("mode = %d, exposure_line = %d, dummy_line= %d, frame_interval= %d ms",
					mode, shutter, dummy_line, frame_interval);

	if (dest_fr_len != cur_fr_len){
		sns_drv_cxt->sensor_ev_info.preview_framelength = dest_fr_len;
		gc08a8_drv_write_frame_length(handle, aec_info, dest_fr_len);
	}
    sns_drv_cxt->sensor_ev_info.preview_shutter = shutter;
	//if (dest_fr_len > 10675)
		//shutter = shutter + dummy_line;
	gc08a8_drv_write_shutter(handle, aec_info, shutter);

    if(sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle,
                          SENSOR_EXIF_CTRL_EXPOSURETIME, shutter);
    }
}


static void gc08a8_drv_calc_gain(cmr_handle handle,cmr_uint isp_gain, struct sensor_aec_i2c_tag *aec_info)
{
	SENSOR_IC_CHECK_HANDLE_VOID(handle);
	struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	cmr_u32 sensor_gain = 0;

	sensor_gain = isp_gain < ISP_BASE_GAIN ? ISP_BASE_GAIN : isp_gain;
	sensor_gain = sensor_gain * SENSOR_BASE_GAIN / ISP_BASE_GAIN;
	
	if (SENSOR_MAX_GAIN < sensor_gain)
			sensor_gain = SENSOR_MAX_GAIN;

	SENSOR_LOGI("isp_gain = 0x%x,sensor_gain=0x%x", (unsigned int)isp_gain,sensor_gain);

	sns_drv_cxt->sensor_ev_info.preview_gain=sensor_gain;
	gc08a8_drv_write_gain(handle, aec_info,sensor_gain);
	
}

/*==============================================================================
 * Description:
 * sensor power on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int gc08a8_drv_power_on(cmr_handle handle, cmr_uint power_on)
{
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;

    SENSOR_AVDD_VAL_E dvdd_val = module_info->dvdd_val;
    SENSOR_AVDD_VAL_E avdd_val = module_info->avdd_val;
    SENSOR_AVDD_VAL_E iovdd_val = module_info->iovdd_val;
    BOOLEAN power_down = g_gc08a8_mipi_raw_info.power_down_level;
    BOOLEAN reset_level = g_gc08a8_mipi_raw_info.reset_pulse_level;	
	
    if (SENSOR_TRUE == power_on) 
	{
        hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
        hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
		
        usleep(12 * 1000);
		
		hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, iovdd_val);
		usleep(1 * 1000);
		
        hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, dvdd_val);
        usleep(1 * 1000);
		
        hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, avdd_val);
        usleep(1 * 1000);
		
        hw_sensor_set_mclk(sns_drv_cxt->hw_handle, EX_MCLK);
        usleep(10 * 1000);
		
        hw_sensor_power_down(sns_drv_cxt->hw_handle, !power_down);
        usleep(1 * 1000);
		
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, !reset_level);
        usleep(6 * 1000);
    }
	else 
	{
        hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
        usleep(200);
		
        hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
		
		hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
        usleep(500);
		
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
static cmr_int gc08a8_drv_init_fps_info(cmr_handle handle) 
{
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

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
    SENSOR_LOGI("X");
    return rtn;
}

static cmr_int gc08a8_drv_get_static_info(cmr_handle handle, cmr_u32 *param) 
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
        gc08a8_drv_init_fps_info(handle);
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
    ex_info->name = (cmr_s8 *)g_gc08a8_mipi_raw_info.name;
    ex_info->sensor_version_info = (cmr_s8 *)g_gc08a8_mipi_raw_info.sensor_version_info;
    memcpy(&ex_info->fov_info, &static_info->fov_info, sizeof(static_info->fov_info));
    ex_info->pos_dis.up2hori = up;
    ex_info->pos_dis.hori2down = down;
    sensor_ic_print_static_info((cmr_s8 *)SENSOR_NAME, ex_info);

    return rtn;
}

static cmr_int gc08a8_drv_get_fps_info(cmr_handle handle, cmr_u32 *param) 
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
        gc08a8_drv_init_fps_info(handle);
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

static cmr_int gc08a8_drv_access_val(cmr_handle handle, cmr_uint param)
{
	cmr_int ret = SENSOR_FAIL;
    SENSOR_VAL_T *param_ptr = (SENSOR_VAL_T *)param;
    
	SENSOR_IC_CHECK_HANDLE(handle);
	SENSOR_IC_CHECK_PTR(param_ptr);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

	SENSOR_LOGI("sensor gc08a8: param_ptr->type=%x", param_ptr->type);
	
	switch(param_ptr->type)
	{
		case SENSOR_VAL_TYPE_GET_STATIC_INFO:
			ret = gc08a8_drv_get_static_info(handle, param_ptr->pval);
			break;
		case SENSOR_VAL_TYPE_GET_FPS_INFO:
			ret = gc08a8_drv_get_fps_info(handle, param_ptr->pval);
			break;
		case SENSOR_VAL_TYPE_SET_SENSOR_CLOSE_FLAG:
			ret = sns_drv_cxt->is_sensor_close = 1;
			break;
		case SENSOR_VAL_TYPE_GET_PDAF_INFO:
			//ret = gc08a8_drv_get_pdaf_info(handle, param_ptr->pval);
			break;
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
static cmr_int gc08a8_drv_identify(cmr_handle handle, cmr_uint param)
{
	cmr_u16 pid_value = 0x00;
	cmr_u16 ver_value = 0x00;
	cmr_int ret_value = SENSOR_FAIL;
	
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	
	SENSOR_LOGI("mipi raw identify");

	pid_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, GC08A8_PID_ADDR);

	if (GC08A8_PID_VALUE == pid_value) {
		ver_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, GC08A8_VER_ADDR);
		SENSOR_LOGI("Identify: pid_value = %x, ver_value = %x", pid_value, ver_value);
		if (GC08A8_VER_VALUE == ver_value) {
			SENSOR_LOGI("this is gc08a8 sensor");
			
			ret_value = SENSOR_SUCCESS;
            
		} else {
			SENSOR_LOGE("sensor identify fail, pid_value = %x, ver_value = %x", pid_value, ver_value);
		}
	} else {
		SENSOR_LOGE("sensor identify fail, pid_value = %x, ver_value = %x", pid_value, ver_value);
	}

	return ret_value;
}


/*==============================================================================
 * Description:
 * before snapshot
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int gc08a8_drv_before_snapshot(cmr_handle handle, cmr_uint param)
{
	cmr_u32 prv_shutter = 0;
	cmr_u32 prv_gain = 0;
	cmr_u32 cap_shutter = 0;
	cmr_u32 cap_gain = 0;
	cmr_u32 capture_mode = param & 0xffff;
	cmr_u32 preview_mode = (param >> 0x10) & 0xffff;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	
	cmr_u32 prv_linetime = sns_drv_cxt->trim_tab_info[preview_mode].line_time;
	cmr_u32 cap_linetime = sns_drv_cxt->trim_tab_info[capture_mode].line_time;

	SENSOR_LOGI("preview_mode=%d,capture_mode = %d", preview_mode,capture_mode);
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
	
    gc08a8_drv_calc_exposure(handle,cap_shutter, 0 , capture_mode,&gc08a8_aec_info);
	gc08a8_drv_write_reg2sensor(handle, gc08a8_aec_info.frame_length);
	gc08a8_drv_write_reg2sensor(handle, gc08a8_aec_info.shutter);

	sns_drv_cxt->sensor_ev_info.preview_gain=cap_gain;
	gc08a8_drv_write_gain(handle, &gc08a8_aec_info, cap_gain);
	gc08a8_drv_write_reg2sensor(handle, gc08a8_aec_info.again);
	gc08a8_drv_write_reg2sensor(handle, gc08a8_aec_info.dgain);

snapshot_info:
    if(sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle, SENSOR_EXIF_CTRL_EXPOSURETIME, cap_shutter);
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
static cmr_int gc08a8_drv_write_exposure(cmr_handle handle, cmr_uint param)
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

	gc08a8_drv_calc_exposure(handle,exposure_line, dummy_line, size_index, &gc08a8_aec_info);
	gc08a8_drv_write_reg2sensor(handle, gc08a8_aec_info.frame_length);
	gc08a8_drv_write_reg2sensor(handle, gc08a8_aec_info.shutter);

    return ret_value;
}

/*==============================================================================
 * Description:
 * write gain value to sensor
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int gc08a8_drv_write_gain_value(cmr_handle handle, cmr_uint param)
{
	cmr_int ret_value = SENSOR_SUCCESS;
	
	pthread_mutex_lock(&gc08a8_sensor_mutex);
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

	gc08a8_drv_calc_gain(handle,param,&gc08a8_aec_info);
	gc08a8_drv_write_reg2sensor(handle,gc08a8_aec_info.again);
	gc08a8_drv_write_reg2sensor(handle,gc08a8_aec_info.dgain);
	pthread_mutex_unlock(&gc08a8_sensor_mutex);

	return ret_value;
}

/*==============================================================================
 * Description:
 * read ae control info
 * please don't change this function unless it's necessary
 *============================================================================*/
static cmr_int gc08a8_drv_read_aec_info(cmr_handle handle, cmr_uint param) 
{
    cmr_int ret_value = SENSOR_SUCCESS;
    struct sensor_aec_reg_info *info = (struct sensor_aec_reg_info *)param;
    cmr_u16 exposure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 mode = 0x00;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(info);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_LOGI("E");

    info->aec_i2c_info_out = &gc08a8_aec_info;
    exposure_line = info->exp.exposure;
    dummy_line = info->exp.dummy;
    mode = info->exp.size_index;

    gc08a8_drv_calc_exposure(handle, exposure_line, dummy_line, mode, &gc08a8_aec_info);
    gc08a8_drv_calc_gain(handle,info->gain, &gc08a8_aec_info);

    return ret_value;
}

static cmr_int gc08a8_drv_set_master_FrameSync(cmr_handle handle, cmr_uint param) 
{
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	
	SENSOR_LOGI("E");

	/*TODO*/

	//hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3002, 0x40);
	
	/*END*/

    return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * mipi stream on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int gc08a8_drv_stream_on(cmr_handle handle, cmr_uint param)
{
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	
	pthread_mutex_lock(&gc08a8_sensor_mutex);
	SENSOR_LOGI("E");
	
#if defined(CONFIG_DUAL_MODULE)
	//gc08a8_drv_set_master_FrameSync(handle,param);
#endif   
	/*TODO*/
	usleep(100*1000);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x01);	
    /*END*/
	
	/*delay*/
	usleep(20*1000);
	pthread_mutex_unlock(&gc08a8_sensor_mutex);
	
	return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * mipi stream off
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int gc08a8_drv_stream_off(cmr_handle handle, cmr_uint param)
{
	pthread_mutex_lock(&gc08a8_sensor_mutex);
	SENSOR_LOGI("E");
	
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if (!sns_drv_cxt->is_sensor_close) {
        usleep(5 * 1000);
    }
   	/*TODO*/
   
	usleep(20*1000);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x00);
	/*END*/
	
	/*delay*/
	usleep(100*1000);
    sns_drv_cxt->is_sensor_close = 0;
    SENSOR_LOGI("X");
	pthread_mutex_unlock(&gc08a8_sensor_mutex);
    return SENSOR_SUCCESS;
}

static cmr_int gc08a8_drv_handle_create(struct sensor_ic_drv_init_para *init_param, cmr_handle* sns_ic_drv_handle) 
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
	
	gc08a8_drv_write_frame_length(sns_drv_cxt, &gc08a8_aec_info, sns_drv_cxt->sensor_ev_info.preview_framelength);
	gc08a8_drv_write_gain(sns_drv_cxt, &gc08a8_aec_info, sns_drv_cxt->sensor_ev_info.preview_gain);
	gc08a8_drv_write_shutter(sns_drv_cxt, &gc08a8_aec_info, sns_drv_cxt->sensor_ev_info.preview_shutter);

    sensor_ic_set_match_module_info(sns_drv_cxt, ARRAY_SIZE(s_gc08a8_module_info_tab), s_gc08a8_module_info_tab);
    sensor_ic_set_match_resolution_info(sns_drv_cxt, ARRAY_SIZE(s_gc08a8_resolution_tab_raw), s_gc08a8_resolution_tab_raw);
    sensor_ic_set_match_trim_info(sns_drv_cxt, ARRAY_SIZE(s_gc08a8_resolution_trim_tab), s_gc08a8_resolution_trim_tab);
    sensor_ic_set_match_static_info(sns_drv_cxt, ARRAY_SIZE(s_gc08a8_static_info), s_gc08a8_static_info);
    sensor_ic_set_match_fps_info(sns_drv_cxt, ARRAY_SIZE(s_gc08a8_mode_fps_info), s_gc08a8_mode_fps_info);

    /*init exif info,this will be deleted in the future*/
    gc08a8_drv_init_fps_info(sns_drv_cxt);
    pthread_mutex_init(&gc08a8_sensor_mutex, NULL);

    /*add private here*/
    return ret;
}

static cmr_int gc08a8_drv_handle_delete(cmr_handle handle, void *param) 
{
    cmr_int ret = SENSOR_SUCCESS;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    pthread_mutex_destroy(&gc08a8_sensor_mutex);
    ret = sensor_ic_drv_delete(handle,param);
    return ret;
}

static cmr_int gc08a8_drv_get_private_data(cmr_handle handle, cmr_uint cmd, void**param)
{
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);

    ret = sensor_ic_get_private_data(handle,cmd, param);
    return ret;
}

void *sensor_ic_open_lib(void)
{
     return &g_gc08a8_mipi_raw_info;
}

/*==============================================================================
 * Description:
 * all ioctl functoins
 * you can add functions reference SENSOR_IOCTL_FUNC_TAB_T from sensor_drv_u.h
 *
 * add ioctl functions like this:
 * .power = gc08a8_power_on,
 *============================================================================*/
static struct sensor_ic_ops s_gc08a8_ops_tab = {
    .create_handle = gc08a8_drv_handle_create,
    .delete_handle = gc08a8_drv_handle_delete,
    .get_data = gc08a8_drv_get_private_data,
/*---------------------------------------*/
	.power = gc08a8_drv_power_on,
	.identify = gc08a8_drv_identify,
	.ex_write_exp = gc08a8_drv_write_exposure,
	.write_gain_value = gc08a8_drv_write_gain_value,

	.read_aec_info = gc08a8_drv_read_aec_info,//android13及以上平台需要调用此处

    .ext_ops = {
        [SENSOR_IOCTL_BEFORE_SNAPSHOT].ops = gc08a8_drv_before_snapshot,
        [SENSOR_IOCTL_STREAM_ON].ops = gc08a8_drv_stream_on,
        [SENSOR_IOCTL_STREAM_OFF].ops = gc08a8_drv_stream_off,
        /* expand interface,if you want to add your sub cmd ,
         *  you can add it in enum {@SENSOR_IOCTL_VAL_TYPE}
         */
        [SENSOR_IOCTL_ACCESS_VAL].ops = gc08a8_drv_access_val,
    }
};

