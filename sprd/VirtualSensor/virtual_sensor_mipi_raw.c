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

#include "virtual_sensor_mipi_raw.h"
#include "virtual_sensor_wrapper.h"
#include "SimulationWrapper.h"
#undef LOG_TAG
#define LOG_TAG "virtual_sensor_mipi_raw"

#define SENSOR_MCLK 24

#define STATIC_INFO s_virtual_sensor_static_info
#define FPS_INFO s_virtual_sensor_mode_fps_info
#define MIPI_RAW_INFO g_virtual_sensor_mipi_raw_info
#define RES_TRIM_TAB s_virtual_sensor_resolution_trim_tab
#define MODULE_INFO s_virtual_sensor_module_info_tab
#define RES_TAB_RAW s_virtual_sensor_resolution_tab_raw

static int sensor_id;
void *VirtualSensorHandler[ISS_VITUAL_SENSOR_INSTANCE_MAX];
struct sensor_remosaic_info s_virtual_sensor_remosaic_info[ISS_VITUAL_SENSOR_INSTANCE_MAX];

cmr_int virtual_sensor_drv_fullfill_remosaic_info(void *cxt);

static cmr_int virtual_sensor_drv_power_on(cmr_handle handle, cmr_uint power_on) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    if (!VirtualSensorHandler[sensor_id]) {
        VirtualSensorHandler[sensor_id] = CreateOneVirtualSensor(sensor_id);
    }
    if(power_on)
        PowerOnVirtualSensor(VirtualSensorHandler[sensor_id], sns_drv_cxt->VSMode);
    else
        PowerOffVirtualSensor(VirtualSensorHandler[sensor_id]);
    return SENSOR_SUCCESS;
}

static cmr_int virtual_sensor_drv_identify(cmr_handle handle, cmr_uint param) {

     SENSOR_LOGI("identify ok");

    return SENSOR_SUCCESS;
}

static cmr_int virtual_sensor_drv_write_exposure(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u16 expsure_line = 0x00;
    cmr_u16 dummy_line = 0x00;
    cmr_u16 value = 0x00;
    cmr_u16 value0 = 0x00;
    cmr_u16 value1 = 0x00;
    cmr_u16 value2 = 0x00;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    expsure_line = param & 0xffff;
    dummy_line = (param >> 0x10) & 0xffff;

    SENSOR_LOGI("write_exposure line:%d, dummy:%d", expsure_line, dummy_line);

    value = (expsure_line << 0x04) & 0xff;
    ret_value = hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3502, value);
    value = (expsure_line >> 0x04) & 0xff;
    ret_value = hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3501, value);
    value = (expsure_line >> 0x0c) & 0x0f;
    ret_value = hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3500, value);

    return ret_value;
}

static cmr_int virtual_sensor_drv_write_gain(cmr_handle handle, cmr_uint param) {
    cmr_int ret_value = SENSOR_SUCCESS;
    cmr_u16 value = 0x00;
    cmr_uint real_gain = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    real_gain = ((param & 0xf) + 16) * (((param >> 4) & 0x01) + 1) *
                (((param >> 5) & 0x01) + 1);
    real_gain = real_gain * (((param >> 6) & 0x01) + 1) *
                (((param >> 7) * 0x01) + 1) * (((param >> 8) * 0x01) + 1);

    SENSOR_LOGI("real_gain:0x%x, param: 0x%x", real_gain, param);

    value = real_gain & 0xff;
    ret_value =
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x350b, value); /*0-7*/
    value = (real_gain >> 0x08) & 0x03;
    ret_value =
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x350a, value); /*8*/

    return ret_value;
}

static cmr_int virtual_sensor_drv_read_gain(cmr_handle handle, cmr_u32 param) {
    cmr_u32 rtn = SENSOR_SUCCESS;
    cmr_u16 value = 0x00;
    cmr_u32 gain = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x350b); /*0-7*/
    gain = value & 0xff;
    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x350a); /*8*/
    gain |= (value << 0x08) & 0x300;

    sns_drv_cxt->sensor_gain.gain = gain;

    SENSOR_LOGI("gain: 0x%x", gain);

    return rtn;
}

static cmr_int virtual_sensor_drv_set_ev(cmr_handle handle, cmr_uint param) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_EXT_FUN_T_PTR ext_ptr = (SENSOR_EXT_FUN_T_PTR)param;
    cmr_u16 value = 0x00;
    cmr_u32 gain = 0;
    cmr_u32 ev = ext_ptr->param;

    SENSOR_LOGI("param: 0x%x", ev);
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    gain = sns_drv_cxt->sensor_gain.gain;
    gain = (gain * ext_ptr->param) >> 0x06;

    value = gain & 0xff;
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x350b, value); /*0-7*/
    value = (gain >> 0x08) & 0x03;
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x350a, value); /*8*/

    return rtn;
}

/*==============================================================================
 * Description:
 * calculate fps for every sensor mode according to frame_line and line_time
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int virtual_sensor_drv_init_fps_info(cmr_handle handle) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
    struct sensor_trim_tag *trim_info = sns_drv_cxt->trim_tab_info;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;

    SENSOR_LOGI("E");
    SENSOR_LOGI("trim_info %p, static_info %p, fps_info %p, fps is_init %d", trim_info,
        static_info, fps_info, fps_info->is_init);
    if (trim_info && static_info && fps_info && !fps_info->is_init) {
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

static cmr_int virtual_sensor_drv_get_static_info(cmr_handle handle, cmr_uint *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ex_info *ex_info = (struct sensor_ex_info *)param;
    cmr_u32 up = 0;
    cmr_u32 down = 0;

    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;

    // make sure we have get max fps of all settings.
    if (!fps_info || !static_info || !module_info || !ex_info) {
        if (!fps_info)
            SENSOR_LOGD("fps is not complete");
        if (!static_info)
            SENSOR_LOGD("static_info is not complete");
        if (!module_info)
            SENSOR_LOGD("module_info is not complete");
        if (!ex_info)
            SENSOR_LOGD("ex_info is not complete");
        SENSOR_LOGD("Context is not complete with %p", sns_drv_cxt);
        sensor_ic_set_match_module_info(sns_drv_cxt, ARRAY_SIZE(s_virtual_sensor_module_info_tab),
                                        s_virtual_sensor_module_info_tab);
        sensor_ic_set_match_resolution_info(sns_drv_cxt, ARRAY_SIZE(s_virtual_sensor_resolution_tab_raw),
                                            s_virtual_sensor_resolution_tab_raw);
        sensor_ic_set_match_trim_info(sns_drv_cxt, ARRAY_SIZE(s_virtual_sensor_resolution_trim_tab),
                                    s_virtual_sensor_resolution_trim_tab);
        sensor_ic_set_match_static_info(sns_drv_cxt, ARRAY_SIZE(s_virtual_sensor_static_info),
                                        s_virtual_sensor_static_info);
        sensor_ic_set_match_fps_info(sns_drv_cxt, ARRAY_SIZE(s_virtual_sensor_mode_fps_info), s_virtual_sensor_mode_fps_info);
        fps_info = sns_drv_cxt->fps_info;
        static_info = sns_drv_cxt->static_info;
        module_info = sns_drv_cxt->module_info;
    }

    if (fps_info) {
        if (!fps_info->is_init) {
            virtual_sensor_drv_init_fps_info(handle);
        }
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
    ex_info->name = (cmr_s8 *)MIPI_RAW_INFO.name;
    ex_info->sensor_version_info = (cmr_s8 *)MIPI_RAW_INFO.sensor_version_info;

    ex_info->pos_dis.up2hori = up;
    ex_info->pos_dis.hori2down = down;

    sensor_ic_print_static_info(ex_info->name, ex_info);

    return rtn;
}

static cmr_int virtual_sensor_drv_get_fps_info(cmr_handle handle, cmr_uint *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_MODE_FPS_T *fps_info = (SENSOR_MODE_FPS_T *)param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(fps_info);
    SENSOR_IC_CHECK_PTR(param);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    struct sensor_fps_info *fps_data = sns_drv_cxt->fps_info;

    // make sure have inited fps of every sensor mode.
    if (!fps_data->is_init) {
        virtual_sensor_drv_init_fps_info(handle);
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

static cmr_int virtual_sensor_drv_ext_func(cmr_handle handle, cmr_uint ctl_param) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_EXT_FUN_PARAM_T_PTR ext_ptr = (SENSOR_EXT_FUN_PARAM_T_PTR)ctl_param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(ext_ptr);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    switch (ext_ptr->cmd) {
    case 10:
        rtn = virtual_sensor_drv_set_ev(handle, ctl_param);
        break;
    default:
        break;
    }

    return rtn;
}

static cmr_int virtual_sensor_drv_before_snapshot(cmr_handle handle, cmr_uint param) {
    cmr_uint cap_mode = (param >> CAP_MODE_BITS);
    cmr_u8 ret_l, ret_m, ret_h;
    cmr_u32 capture_exposure, preview_maxline;
    cmr_u32 capture_maxline, preview_exposure;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    SENSOR_PRINT("%d,%d.", cap_mode, param);
    param = param & 0xffff;
    cmr_u32 prv_linetime =
        sns_drv_cxt->trim_tab_info[SENSOR_MODE_PREVIEW_ONE].line_time;
    cmr_u32 cap_linetime = sns_drv_cxt->trim_tab_info[param].line_time;

    if (SENSOR_MODE_PREVIEW_ONE >= param) {
        virtual_sensor_drv_read_gain(handle, 0x00);
        SENSOR_PRINT("prvmode equal to capmode");
        return SENSOR_SUCCESS;
    }

    ret_h = (cmr_u8)hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3500);
    ret_m = (cmr_u8)hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3501);
    ret_l = (cmr_u8)hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x3502);
    preview_exposure = (ret_h << 12) + (ret_m << 4) + (ret_l >> 4);

    ret_h = (cmr_u8)hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x380e);
    ret_l = (cmr_u8)hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x380f);
    preview_maxline = (ret_h << 8) + ret_l;

    if (sns_drv_cxt->ops_cb.set_mode)
        sns_drv_cxt->ops_cb.set_mode(sns_drv_cxt->caller_handle, param);

    if (prv_linetime == cap_linetime) {
        SENSOR_PRINT("prvline equal to capline");
        return SENSOR_SUCCESS;
    }

    ret_h = (cmr_u8)hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x380e);
    ret_l = (cmr_u8)hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x380f);
    capture_maxline = (ret_h << 8) + ret_l;
    capture_exposure = preview_exposure * prv_linetime / cap_linetime;

    if (0 == capture_exposure) {
        capture_exposure = 1;
    }

    ret_l = ((unsigned char)capture_exposure) << 4;
    ret_m = (unsigned char)(capture_exposure >> 4) & 0xff;
    ret_h = (unsigned char)(capture_exposure >> 12);

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3502, ret_l);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3501, ret_m);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3500, ret_h);

    virtual_sensor_drv_read_gain(handle, 0x00);

    if (sns_drv_cxt->ops_cb.set_exif_info) {
        sns_drv_cxt->ops_cb.set_exif_info(sns_drv_cxt->caller_handle,
                                          SENSOR_EXIF_CTRL_EXPOSURETIME,
                                          capture_exposure);
    }

    return SENSOR_SUCCESS;
}

static cmr_int virtual_sensor_drv_after_snapshot(cmr_handle handle, cmr_uint param) {
    SENSOR_PRINT("after_snapshot mode:%d", param);
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if (sns_drv_cxt->ops_cb.set_mode)
        sns_drv_cxt->ops_cb.set_mode(sns_drv_cxt->caller_handle,
                                     (cmr_u32)param);

    return SENSOR_SUCCESS;
}

static cmr_int virtual_sensor_drv_stream_on(cmr_handle handle, cmr_uint param) {
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_PRINT("StreamOn with %p -> id %d", VirtualSensorHandler[sensor_id], sensor_id);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    //StreamOnOneVirtualSensor(VirtualSensorHandler[sensor_id]);
    return 0;
}

static cmr_int virtual_sensor_drv_stream_off(cmr_handle handle, cmr_uint param) {
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_PRINT("StreamOff");
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    StreamOffOneVirtualSensor(VirtualSensorHandler[sensor_id]);
    return 0;
}

static cmr_u32 sns_4in1_mode[ISS_VITUAL_SENSOR_INSTANCE_MAX] = {0};

static cmr_int virtual_senosr_drv_get_4in1_info(cmr_handle handle, cmr_u32 *param) {
	cmr_int rtn = SENSOR_SUCCESS;
	struct sensor_4in1_info *sn_4in1_info = NULL;
	SENSOR_INFO_T temp_mipi_raw_info;
	SENSOR_IC_CHECK_PTR(param);

	SENSOR_LOGI("E\n");
    virtual_sensor_drv_fullfill_remosaic_info(VirtualSensorHandler[sensor_id]);
    if (s_virtual_sensor_remosaic_info[sensor_id].limited_width &&
        s_virtual_sensor_remosaic_info[sensor_id].limited_height) {

        sn_4in1_info = (struct sensor_4in1_info *)param;
        sn_4in1_info->is_4in1_supported = 0;
        sn_4in1_info->limited_4in1_width = s_virtual_sensor_remosaic_info[sensor_id].limited_width;
        sn_4in1_info->limited_4in1_height = s_virtual_sensor_remosaic_info[sensor_id].limited_height;
        for (int i = 0; i < ISS_VITUAL_SENSOR_INSTANCE_MAX; i++) {
            sns_4in1_mode[i] = s_virtual_sensor_remosaic_info[sensor_id].remosaic_mode_info[i];
            if (s_virtual_sensor_remosaic_info[sensor_id].remosaic_mode_info[i]) {
                SENSOR_LOGD("BREAK");
                break;
            }
        }
        sn_4in1_info->sns_mode = sns_4in1_mode;
        GetVirtualSensorMipiRawInfo((void *)&temp_mipi_raw_info, VirtualSensorHandler[sensor_id]);
        sn_4in1_info->binningfactor = temp_mipi_raw_info.source_width_max / sn_4in1_info->limited_4in1_width;
    }
	return rtn;
}

/*==============================================================================
 * Description:
 * cfg otp setting
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int virtual_sensor_drv_access_val(cmr_handle handle, cmr_uint param) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_VAL_T *param_ptr = (SENSOR_VAL_T *)param;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param_ptr);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    int isStop;
    SENSOR_LOGI("param_ptr->type1=%x", param_ptr->type);
    switch (param_ptr->type) {
    case SENSOR_VAL_TYPE_GET_STATIC_INFO:
        if (!param_ptr->pval || !handle)
            SENSOR_LOGD("Could not get param");
        ret = virtual_sensor_drv_get_static_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_GET_FPS_INFO:
        ret = virtual_sensor_drv_get_fps_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_SET_VIRTUAL_SENSOR_MODE:
        break;
    case SENSOR_VAL_TYPE_SET_VIRTUAL_SENSOR_BUFFER:
        SENSOR_LOGD("Set One Frame to VirtualSensor");
        StreamControlVirtualSensor(param_ptr->pval, VirtualSensorHandler[sensor_id]);
        break;
    case SENSOR_VAL_TYPE_SET_VIRTUAL_SENSOR_RUN:
        StreamControlRunOneFrame(VirtualSensorHandler[sensor_id]);
        break;
    case SENSOR_VAL_TYPE_SET_VIRTUAL_SENSOR_EXTERNAL_CXT:
        SetVirtualSensorExternalCxt(param_ptr->pval, VirtualSensorHandler[sensor_id]);
        break;
    case SENSOR_VAL_TYPE_PAUSE_VIRTUAL_SENSOR:
        isStop = *((int *)param_ptr->pval);
        if (isStop)
            StreamOffOneVirtualSensor(VirtualSensorHandler[sensor_id]);
        else
            StreamOnOneVirtualSensor(VirtualSensorHandler[sensor_id]);
        SENSOR_LOGD("Control VirtualSensor streaming with %d", isStop);
        break;
    case SENSOR_VAL_TYPE_GET_4IN1_INFO:
        ret = virtual_senosr_drv_get_4in1_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_SET_FPS_INFO:
        SetFpsInfoVirtualSensor(param_ptr->pval, VirtualSensorHandler[sensor_id]);
        break;
    default:
        break;
    }

    return ret;
}

cmr_int virtual_sensor_drv_fullfill_static_info(void *cxt) {
    if(!cxt) {
        SENSOR_LOGE("Not valid VirtualSensor instance, return");
        return SENSOR_FAIL;
    }
    struct sensor_static_info static_info = {0};
    GetVirtualSensorStaticInfo(&s_virtual_sensor_static_info[0].static_info, cxt);
    s_virtual_sensor_static_info[0].module_id = MODULE_SUNNY;
    s_virtual_sensor_static_info[0].static_info.exp_valid_frame_num = 1;
    return SENSOR_SUCCESS;
}

cmr_int virtual_sensor_drv_fullfill_remosaic_info(void *cxt) {
    if(!cxt) {
        SENSOR_LOGE("Not valid VirtualSensor instance, return");
        return SENSOR_FAIL;
    }
    GetVirtualSensorRemosaicInfo((void *)&s_virtual_sensor_remosaic_info[sensor_id], cxt);
    return SENSOR_SUCCESS;
}

cmr_int virtual_sensor_drv_fullfill_module_info(void *cxt) {
    if(!cxt) {
        SENSOR_LOGE("Not valid VirtualSensor instance, return");
        return SENSOR_FAIL;
    }
    struct module_cfg_info temp_module_info;
    GetVirtualSensorModuleCfgInfo((void *)&temp_module_info, cxt);
    s_virtual_sensor_module_info_tab[0].module_info.image_pattern = temp_module_info.image_pattern;
    s_virtual_sensor_module_info_tab[0].module_info.sensor_interface.is_cphy = temp_module_info.sensor_interface.is_cphy;
    // SENSOR_LOGD("Image pattern is %d", temp_module_info.image_pattern);
    return SENSOR_SUCCESS;
}

cmr_int virtual_sensor_drv_fullfill_trim_info(void *cxt) {
    if (!cxt) {
        SENSOR_LOGE("Not valid VirtualSensor instance, return");
        return SENSOR_FAIL;
    }
    SENSOR_TRIM_T temp_trimInfo;
    GetVirtualSensorResource((void *)&temp_trimInfo, cxt);
    for (int i = 1; i < SENSOR_MODE_MAX; i++) {
        if (temp_trimInfo.trim_info[i].trim_width && temp_trimInfo.trim_info[i].trim_height) {
            s_virtual_sensor_resolution_trim_tab[0].trim_info[i].trim_start_x = temp_trimInfo.trim_info[i].trim_start_x;
            s_virtual_sensor_resolution_trim_tab[0].trim_info[i].trim_start_y = temp_trimInfo.trim_info[i].trim_start_y;
            s_virtual_sensor_resolution_trim_tab[0].trim_info[i].trim_width = temp_trimInfo.trim_info[i].trim_width;
            s_virtual_sensor_resolution_trim_tab[0].trim_info[i].trim_height = temp_trimInfo.trim_info[i].trim_height;
            s_virtual_sensor_resolution_trim_tab[0].trim_info[i].line_time = temp_trimInfo.trim_info[i].line_time;
            s_virtual_sensor_resolution_trim_tab[0].trim_info[i].bps_per_lane = temp_trimInfo.trim_info[i].bps_per_lane;
            s_virtual_sensor_resolution_trim_tab[0].trim_info[i].frame_line = temp_trimInfo.trim_info[i].frame_line;
            s_virtual_sensor_resolution_trim_tab[0].trim_info[i].scaler_trim.x = temp_trimInfo.trim_info[i].scaler_trim.x;
            s_virtual_sensor_resolution_trim_tab[0].trim_info[i].scaler_trim.y = temp_trimInfo.trim_info[i].scaler_trim.y;
            s_virtual_sensor_resolution_trim_tab[0].trim_info[i].scaler_trim.w = temp_trimInfo.trim_info[i].scaler_trim.w;
            s_virtual_sensor_resolution_trim_tab[0].trim_info[i].scaler_trim.h = temp_trimInfo.trim_info[i].scaler_trim.h;
            s_virtual_sensor_resolution_tab_raw[0].reg_tab[i].width = temp_trimInfo.trim_info[i].trim_width;
            s_virtual_sensor_resolution_tab_raw[0].reg_tab[i].height = temp_trimInfo.trim_info[i].trim_height;
        }
    }
    return SENSOR_SUCCESS;
}

cmr_int virtual_sensor_drv_fullfill_mipiraw_info(void *cxt) {
    if(!cxt) {
        SENSOR_LOGE("Not valid VirtualSensor instance, return");
        return SENSOR_FAIL;
    }
    SENSOR_INFO_T temp_mipi_raw_info;
    GetVirtualSensorMipiRawInfo((void *)&temp_mipi_raw_info, cxt);
    g_virtual_sensor_mipi_raw_info.image_format = SENSOR_IMAGE_FORMAT_RAW;
    g_virtual_sensor_mipi_raw_info.source_width_max = temp_mipi_raw_info.source_width_max;
    g_virtual_sensor_mipi_raw_info.source_height_max = temp_mipi_raw_info.source_height_max;
    g_virtual_sensor_mipi_raw_info.name = temp_mipi_raw_info.name;
    SENSOR_LOGD("VirtualSensor ImageFormat is %d, Name %s", g_virtual_sensor_mipi_raw_info.image_format, g_virtual_sensor_mipi_raw_info.name);
    return SENSOR_SUCCESS;
}

cmr_int virtual_sensor_drv_fullfill_4in1_info(void *cxt) {
    if(!cxt) {
        SENSOR_LOGE("Not valid VirtualSensor instance, return");
        return SENSOR_FAIL;
    }
    return SENSOR_FAIL;
}

static cmr_int
virtual_sensor_drv_handle_create(struct sensor_ic_drv_init_para *init_param,
                         cmr_handle *sns_ic_drv_handle) {

    struct sensor_ic_drv_cxt *sns_drv_cxt = NULL;
    
    sensor_ic_drv_create(init_param, sns_ic_drv_handle);
    SENSOR_LOGD("VirtualSensor handle created with %p", sns_ic_drv_handle);
    sns_drv_cxt = (struct sensor_ic_drv_cxt *)sns_ic_drv_handle;
    sns_drv_cxt->module_id = MODULE_SUNNY;
    virtual_sensor_drv_fullfill_trim_info(VirtualSensorHandler[sensor_id]);
    // virtual_sensor_drv_fullfill_static_info(VirtualSensorHandler[sensor_id]);
    // virtual_sensor_drv_fullfill_module_info(VirtualSensorHandler[sensor_id]);
    virtual_sensor_drv_fullfill_mipiraw_info(VirtualSensorHandler[sensor_id]);


    /*init exif info,this will be deleted in the future*/

    sensor_ic_set_match_module_info(sns_drv_cxt, ARRAY_SIZE(s_virtual_sensor_module_info_tab),
                                        s_virtual_sensor_module_info_tab);
    sensor_ic_set_match_resolution_info(sns_drv_cxt, ARRAY_SIZE(s_virtual_sensor_resolution_tab_raw),
                                            s_virtual_sensor_resolution_tab_raw);
    sensor_ic_set_match_trim_info(sns_drv_cxt, ARRAY_SIZE(s_virtual_sensor_resolution_trim_tab),
                                    s_virtual_sensor_resolution_trim_tab);
    sensor_ic_set_match_static_info(sns_drv_cxt, ARRAY_SIZE(s_virtual_sensor_static_info),
                                        s_virtual_sensor_static_info);
    sensor_ic_set_match_fps_info(sns_drv_cxt, ARRAY_SIZE(s_virtual_sensor_mode_fps_info), s_virtual_sensor_mode_fps_info);
    /*add private here*/
    virtual_sensor_drv_init_fps_info(sns_drv_cxt);
    struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
    struct sensor_static_info *static_info = sns_drv_cxt->static_info;
    struct module_cfg_info *module_info = sns_drv_cxt->module_info;
    struct sensor_trim_tag *trim_info = sns_drv_cxt->trim_tab_info;
    // make sure we have get max fps of all settings.
    if (!fps_info || !static_info || !module_info) {
        if (!fps_info) {
            sns_drv_cxt->fps_info = &s_virtual_sensor_mode_fps_info[0].fps_info;
            SENSOR_LOGD("fps is not complete");
        }
        if (!static_info) {
            sns_drv_cxt->static_info = &s_virtual_sensor_static_info[0].static_info;
            SENSOR_LOGD("static_info is not complete");
        }
        if (!module_info) {
            sns_drv_cxt->module_info = &s_virtual_sensor_module_info_tab[0].module_info;
            SENSOR_LOGD("module_info is not complete");
        }
        if (!trim_info) {
            SENSOR_LOGD("module_info is not complete");
            sns_drv_cxt->trim_tab_info = &s_virtual_sensor_resolution_trim_tab[0].trim_info;
        }
        SENSOR_LOGD("Context is not complete");
    }
    SENSOR_LOGD("handle create sucess");
    return SENSOR_SUCCESS;
}

static cmr_int virtual_sensor_drv_get_private_data(cmr_handle handle, cmr_uint cmd,
                                           void **param) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    SENSOR_IC_CHECK_PTR(param);

    ret = sensor_ic_get_private_data(handle, cmd, param);
    return ret;
}

static cmr_int virtual_sensor_drv_handle_delete(cmr_handle handle, void *param) {
    cmr_int ret = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);

    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    /*if has private data,you must release it here*/
    char value[128];
    property_get("persist.vendor.cam.id", value, "0");
    // DeleteOneVirtualSensor(VirtualSensorHandler[atoi(value)]);
    ret = sensor_ic_drv_delete(handle, param);
    return ret;
}
void *sensor_ic_open_lib(void)
{
    char value[128];
    property_get("persist.vendor.cam.id", value, "0");
    sensor_id = atoi(value);
    VirtualSensorHandler[sensor_id] = CreateOneVirtualSensor(sensor_id);
    SENSOR_LOGD("VirtualSensorHandle is %p", VirtualSensorHandler[sensor_id]);
    virtual_sensor_drv_fullfill_trim_info(VirtualSensorHandler[sensor_id]);
    // virtual_sensor_drv_fullfill_static_info(VirtualSensorHandler[sensor_id]);
    virtual_sensor_drv_fullfill_module_info(VirtualSensorHandler[sensor_id]);
    virtual_sensor_drv_fullfill_mipiraw_info(VirtualSensorHandler[sensor_id]);

    // g_virtual_sensor_mipi_raw_info.sns_ops = &s_virtual_sensor_ops_tab;
    // g_virtual_sensor_mipi_raw_info.resolution_tab_info_ptr = s_virtual_sensor_resolution_tab_raw;
    // g_virtual_sensor_mipi_raw_info.module_info_tab = s_virtual_sensor_module_info_tab;

    return &g_virtual_sensor_mipi_raw_info;
}

static struct sensor_ic_ops s_virtual_sensor_ops_tab = {
    .create_handle = virtual_sensor_drv_handle_create,
    .delete_handle = virtual_sensor_drv_handle_delete,
    .get_data = virtual_sensor_drv_get_private_data,
    .power = virtual_sensor_drv_power_on,
    .identify = virtual_sensor_drv_identify,

    .write_exp = virtual_sensor_drv_write_exposure,
    .write_gain_value = virtual_sensor_drv_write_gain,

    /*the following interface called by oem layer*/
    .ext_ops = {
            [SENSOR_IOCTL_BEFORE_SNAPSHOT].ops = virtual_sensor_drv_before_snapshot,
            [SENSOR_IOCTL_STREAM_ON].ops = virtual_sensor_drv_stream_on,
            [SENSOR_IOCTL_STREAM_OFF].ops = virtual_sensor_drv_stream_off,
            [SENSOR_IOCTL_EXT_FUNC].ops = virtual_sensor_drv_ext_func,
            [SENSOR_IOCTL_ACCESS_VAL].ops = virtual_sensor_drv_access_val,
    }};

