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

/*
 * V1.0
 */
/*History
*Date                  Modification                                 Reason
*
*/

#define LOG_TAG "s5k3p9sp04"

//#define MIPI_NUM_2LANE

#ifdef MIPI_NUM_2LANE
#include "sensor_s5k3p9sp04_mipi_raw_2lane.h"
#else
#include "sensor_s5k3p9sp04_mipi_raw_4lane.h"
#endif

static cmr_s64 s5k3p9sp04ExpTimeRange[] = {46000/* 46us */, 200000000/* default 200ms*/};

static int64_t mStreamOnTime = 0;
static int64_t mStreamOffTime = 0;
//#define CONFIG_FLIP

/*==============================================================================
 * Description:
 * write register value to sensor
 * please modify this function acording your spec
 *============================================================================*/

static void
s5k3p9sp04_drv_write_reg2sensor(cmr_handle handle,
                                 struct sensor_i2c_reg_tab *reg_info) {
  SENSOR_IC_CHECK_PTR_VOID(reg_info);
  SENSOR_IC_CHECK_HANDLE_VOID(handle);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
  cmr_int i = 0;

  for (i = 0; i < reg_info->size; i++) {

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, reg_info->settings[i].reg_addr,
                        reg_info->settings[i].reg_value);
  }
}

/*==============================================================================
 * Description:
 * write gain to sensor registers buffer
 * please modify this function acording your spec
 *============================================================================*/
static void s5k3p9sp04_drv_write_gain(struct sensor_aec_i2c_tag *aec_info,
                                       cmr_u32 gain) {
  SENSOR_IC_CHECK_PTR_VOID(aec_info);
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
static void
s5k3p9sp04_drv_write_frame_length(struct sensor_aec_i2c_tag *aec_info,
                                   cmr_u32 frame_len) {
  SENSOR_IC_CHECK_PTR_VOID(aec_info);

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
static void s5k3p9sp04_drv_write_shutter(struct sensor_aec_i2c_tag *aec_info,
                                          cmr_u32 shutter) {
  SENSOR_IC_CHECK_PTR_VOID(aec_info);
  cmr_u32 value = 0x00;

  if (aec_info->shutter->size) {
    /*TODO*/

    aec_info->shutter->settings[0].reg_value = shutter;

    /*END*/
  }
}

static void s5k3p9sp04_drv_calc_exposure(cmr_handle handle, cmr_u32 shutter,
                                          cmr_u32 dummy_line, cmr_u16 mode,
                                          struct sensor_aec_i2c_tag *aec_info) {
  cmr_u32 dest_fr_len = 0;
  cmr_u32 cur_fr_len = 0;
  cmr_u32 fr_len = 0;
  cmr_u32 value = 0x00;
  float fps = 0.0;
  float line_time = 0.0;
  cmr_u16 frame_interval = 0x00;

  SENSOR_IC_CHECK_PTR_VOID(aec_info);
  SENSOR_IC_CHECK_HANDLE_VOID(handle);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  do {
      char value[PROPERTY_VALUE_MAX];

      property_get("persist.vendor.cam.raw.mode", value, "jpeg");
      if ((!strcmp(value, "raw")) && mode == 3) {
          property_get("persist.vendor.cam.raw.bin.factor", value, "400");
          shutter *= (atoi(value));
          shutter /= 100;
      }
  } while (0);

  sns_drv_cxt->frame_length_def = sns_drv_cxt->trim_tab_info[mode].frame_line;
  sns_drv_cxt->line_time_def = sns_drv_cxt->trim_tab_info[mode].line_time;
  cur_fr_len = sns_drv_cxt->sensor_ev_info.preview_framelength;
  fr_len = sns_drv_cxt->frame_length_def;
  value = dummy_line > FRAME_OFFSET ? dummy_line : FRAME_OFFSET;
  dest_fr_len = ((shutter + value) > fr_len) ? (shutter + value) : fr_len;
  sns_drv_cxt->frame_length = dest_fr_len;

  if (shutter < SENSOR_MIN_SHUTTER)
    shutter = SENSOR_MIN_SHUTTER;

  line_time = sns_drv_cxt->trim_tab_info[mode].line_time;
  if (cur_fr_len > shutter) {
    fps = 1000000000.0 / (cur_fr_len * line_time);
  } else {
    fps = 1000000000.0 / ((shutter + dummy_line) * line_time);
  }
  SENSOR_LOGD("fps = %f", fps);

  frame_interval = (uint16_t)(
      ((shutter + dummy_line) * sns_drv_cxt->line_time_def) / 1000000);
  SENSOR_LOGD(
      "mode = %d, exposure_line = %d, dummy_line= %d, frame_interval= %d ms",
      mode, shutter, dummy_line, frame_interval);

  if (dest_fr_len != cur_fr_len) {
    sns_drv_cxt->sensor_ev_info.preview_framelength = dest_fr_len;
    s5k3p9sp04_drv_write_frame_length(aec_info, dest_fr_len);
  }
  sns_drv_cxt->sensor_ev_info.preview_shutter = shutter;
  s5k3p9sp04_drv_write_shutter(aec_info, shutter);
}

static void s5k3p9sp04_drv_calc_gain(cmr_handle handle, cmr_uint isp_gain,
                                      struct sensor_aec_i2c_tag *aec_info) {
  SENSOR_IC_CHECK_HANDLE_VOID(handle);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
  cmr_u32 sensor_gain = 0;

  sensor_gain = isp_gain < SENSOR_BASE_GAIN ? SENSOR_BASE_GAIN : isp_gain;
  sensor_gain = sensor_gain * SENSOR_BASE_GAIN / ISP_BASE_GAIN;
  if (SENSOR_MAX_GAIN < sensor_gain)
    sensor_gain = SENSOR_MAX_GAIN;

  SENSOR_LOGD("isp_gain = 0x%x,sensor_gain=0x%x", isp_gain, sensor_gain);

  sns_drv_cxt->sensor_ev_info.preview_gain = sensor_gain;
  s5k3p9sp04_drv_write_gain(aec_info, sensor_gain);
}

/*==============================================================================
 * Description:
 * sensor power on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5k3p9sp04_drv_power_on(cmr_handle handle, cmr_uint power_on) {
  SENSOR_IC_CHECK_HANDLE(handle);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
  struct module_cfg_info *module_info = sns_drv_cxt->module_info;

  SENSOR_AVDD_VAL_E dvdd_val = module_info->dvdd_val;
  SENSOR_AVDD_VAL_E avdd_val = module_info->avdd_val;
  SENSOR_AVDD_VAL_E iovdd_val = module_info->iovdd_val;
  BOOLEAN power_down = g_s5k3p9sp04_mipi_raw_info.power_down_level;
  BOOLEAN reset_level = g_s5k3p9sp04_mipi_raw_info.reset_pulse_level;

  if (SENSOR_TRUE == power_on) {
    hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
    hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
    hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
    hw_sensor_set_voltage(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED,
                          SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED);
    usleep(1 * 1000);
    hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, dvdd_val);
    hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, avdd_val);
    // hw_sensor_set_avdd_en_level(sns_drv_cxt->hw_handle,1);
    hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, iovdd_val);
    hw_sensor_power_down(sns_drv_cxt->hw_handle, !power_down);
    hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, !reset_level);
    usleep(10);
    hw_sensor_set_mclk(sns_drv_cxt->hw_handle, EX_MCLK);
    usleep(1 * 1000);
    hw_sensor_set_mipi_level(sns_drv_cxt->hw_handle, 0);

  } else {
    hw_sensor_set_mipi_level(sns_drv_cxt->hw_handle, 1);
    hw_sensor_set_mclk(sns_drv_cxt->hw_handle, SENSOR_DISABLE_MCLK);
    usleep(500);
    hw_sensor_set_reset_level(sns_drv_cxt->hw_handle, reset_level);
    hw_sensor_power_down(sns_drv_cxt->hw_handle, power_down);
    usleep(200);
    // hw_sensor_set_avdd_en_level(sns_drv_cxt->hw_handle,0);
    hw_sensor_set_iovdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
    hw_sensor_set_avdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
    hw_sensor_set_dvdd_val(sns_drv_cxt->hw_handle, SENSOR_AVDD_CLOSED);
  }

  SENSOR_LOGI("(1:on, 0:off): %lu", power_on);
  return SENSOR_SUCCESS;
}

static cmr_int s5k3p9sp04_drv_get_exp_range(struct sensor_ex_info *ex_info) {
    cmr_int rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_PTR(ex_info);
    ex_info->sensor_min_exp = s5k3p9sp04ExpTimeRange[0];
    ex_info->sensor_max_exp = s5k3p9sp04ExpTimeRange[1];
    SENSOR_LOGI("ExpTimeRange[%lld,%lld]", ex_info->sensor_min_exp, ex_info->sensor_max_exp);

    return rtn;
}

/*==============================================================================
 * Description:
 * calculate fps for every sensor mode according to frame_line and line_time
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5k3p9sp04_drv_init_fps_info(cmr_handle handle) {
  cmr_int rtn = SENSOR_SUCCESS;
  SENSOR_IC_CHECK_HANDLE(handle);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  struct sensor_fps_info *fps_info = sns_drv_cxt->fps_info;
  struct sensor_trim_tag *trim_info = sns_drv_cxt->trim_tab_info;
  struct sensor_static_info *static_info = sns_drv_cxt->static_info;

  SENSOR_LOGD("E");
  if (!fps_info->is_init) {
    cmr_u32 i, modn, tempfps = 0;
    SENSOR_LOGD("start init");
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
      SENSOR_LOGD("mode %d,tempfps %d,frame_len %d,line_time: %d ", i, tempfps,
                  trim_info[i].frame_line, trim_info[i].line_time);
      SENSOR_LOGD("mode %d,max_fps: %d ", i,
                  fps_info->sensor_mode_fps[i].max_fps);
      SENSOR_LOGI("is_high_fps: %d,highfps_skip_num %d",
                  fps_info->sensor_mode_fps[i].is_high_fps,
                  fps_info->sensor_mode_fps[i].high_fps_skip_num);
    }
    fps_info->is_init = 1;
  }
  SENSOR_LOGD("X");
  return rtn;
}

/*==============================================================================
 * Description:
 * Get PDAF info for every sensor with SIN_MODULE or DUAL_MODULE
 * please modify this function acording your sensor spec && pdaf map
 *============================================================================*/
static const cmr_u16 s5k3p9sp04_pd_is_right[] = {
    0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0};

static const cmr_u16 s5k3p9sp04_pd_col[] = {
    4,  56, 4, 20, 40, 56, 20, 40, 8,  52, 8, 24, 36, 52, 24, 36,
    24, 36, 8, 24, 36, 52, 8,  52, 20, 40, 4, 20, 40, 56, 4,  56};

static const cmr_u16 s5k3p9sp04_pd_row[] = {
    7,  7,  11, 11, 11, 11, 15, 15, 23, 23, 27, 27, 27, 27, 31, 31,
    39, 39, 43, 43, 43, 43, 47, 47, 55, 55, 59, 59, 59, 59, 63, 63};

/* static cmr_int s5k3p9sp04_drv_get_pdaf_info(cmr_handle handle,
                                             cmr_u32 *param) {
  cmr_int rtn = SENSOR_SUCCESS;
  struct sensor_pdaf_info *pdaf_info = NULL;
  cmr_u16 i = 0;
  cmr_u16 pd_pos_row_size = 0;
  cmr_u16 pd_pos_col_size = 0;
  cmr_u16 pd_pos_is_right_size = 0;
  SENSOR_IC_CHECK_PTR(param);
  pdaf_info = (struct sensor_pdaf_info *)param;

  pd_pos_is_right_size = NUMBER_OF_ARRAY(s5k3p9sp04_pd_is_right);
  pd_pos_row_size = NUMBER_OF_ARRAY(s5k3p9sp04_pd_row);
  pd_pos_col_size = NUMBER_OF_ARRAY(s5k3p9sp04_pd_col);
  if ((pd_pos_row_size != pd_pos_col_size) ||
      (pd_pos_row_size != pd_pos_is_right_size) ||
      (pd_pos_is_right_size != pd_pos_col_size)) {
    SENSOR_LOGE("pd_pos_row size and pd_pos_is_right size are not match");
    return -1;
  }

  pdaf_info->pd_offset_x = 24;
  pdaf_info->pd_offset_y = 24;
  pdaf_info->pd_end_x = 4184;
  pdaf_info->pd_end_y = 3096;
  pdaf_info->pd_size_w = 4640;
  pdaf_info->pd_size_h = 3488;
  pdaf_info->pd_block_w = 3;
  pdaf_info->pd_block_h = 3;
  pdaf_info->pd_block_num_x = 65;
  pdaf_info->pd_block_num_y = 48;
  pdaf_info->pd_is_right = (cmr_u16 *)s5k3p9sp04_pd_is_right;
  pdaf_info->pd_pos_row = (cmr_u16 *)s5k3p9sp04_pd_row;
  pdaf_info->pd_pos_col = (cmr_u16 *)s5k3p9sp04_pd_col;
  pdaf_info->vendor_type = SENSOR_VENDOR_S5K3L8XXM3;
  pdaf_info->pd_pos_size = (pd_pos_is_right_size / 2);
  pdaf_info->sns_orientation = 0; //Normal

  return rtn;
} */

static cmr_int s5k3p9sp04_drv_get_static_info(cmr_handle handle,
                                               uint32_t *param) {
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
    s5k3p9sp04_drv_init_fps_info(handle);
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
  ex_info->name = (cmr_s8 *)g_s5k3p9sp04_mipi_raw_info.name;
  ex_info->sensor_version_info =
      (cmr_s8 *)g_s5k3p9sp04_mipi_raw_info.sensor_version_info;
  memcpy(&ex_info->fov_info, &static_info->fov_info,
           sizeof(static_info->fov_info));

  ex_info->pos_dis.up2hori = up;
  ex_info->pos_dis.hori2down = down;
  s5k3p9sp04_drv_get_exp_range(ex_info);
  sensor_ic_print_static_info((cmr_s8 *)SENSOR_NAME, ex_info);

  return rtn;
}

static cmr_int s5k3p9sp04_drv_get_fps_info(cmr_handle handle, cmr_u32 *param) {
  cmr_int rtn = SENSOR_SUCCESS;
  SENSOR_MODE_FPS_T *fps_info = (SENSOR_MODE_FPS_T *)param;
  SENSOR_IC_CHECK_HANDLE(handle);
  SENSOR_IC_CHECK_PTR(fps_info);
  SENSOR_IC_CHECK_PTR(param);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
  struct sensor_fps_info *fps_data = sns_drv_cxt->fps_info;

  // make sure have inited fps of every sensor mode.
  if (!fps_data->is_init) {
    s5k3p9sp04_drv_init_fps_info(handle);
  }
  cmr_u32 sensor_mode = fps_info->mode;
  fps_info->max_fps = fps_data->sensor_mode_fps[sensor_mode].max_fps;
  fps_info->min_fps = fps_data->sensor_mode_fps[sensor_mode].min_fps;
  fps_info->is_high_fps = fps_data->sensor_mode_fps[sensor_mode].is_high_fps;
  fps_info->high_fps_skip_num =
      fps_data->sensor_mode_fps[sensor_mode].high_fps_skip_num;
  SENSOR_LOGI("mode %d, min_fps: %d,max_fps: %d", fps_info->mode, fps_info->min_fps, fps_info->max_fps);
  SENSOR_LOGD("is_high_fps: %d", fps_info->is_high_fps);
  SENSOR_LOGI("high_fps_skip_num: %d", fps_info->high_fps_skip_num);

  return rtn;
}
#if 1
static const cmr_u32 sns_4in1_mode[] = {0, 0, 0, 1};
static cmr_int s5k3p9sp04_drv_get_4in1_info(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    struct sensor_4in1_info *sn_4in1_info = NULL;
    SENSOR_IC_CHECK_PTR(param);

    SENSOR_LOGI("E\n");

    sn_4in1_info = (struct sensor_4in1_info *)param;
    sn_4in1_info->is_4in1_supported = 1;
    sn_4in1_info->limited_4in1_width = 2304;
    sn_4in1_info->limited_4in1_height = 1744;
    sn_4in1_info->sns_mode = sns_4in1_mode;
    sn_4in1_info->input_format = DATA_RAW10;//for remosaic input
    sn_4in1_info->output_format = DATA_BYTE2;//for remosaic output

    return rtn;
}

#include "sprd_fcell_ss.h"
#include "dlfcn.h"
#include <fcntl.h>
#ifdef CONFIG_ISP_2_7
#define IMG_WIDTH 4608 //4672 //4608 //5120
#else
#define IMG_WIDTH 4640 //4672 //4608 //5120
#endif
//remosaic output need 128 align
#define IMG_HEIGHT    3456 //3504 //3456 //3840

void *handlelib;
//uint16 *pOutImage; // for mipi_raw to raw16
#define OTPDPC_MIRROR 0
#define OTPDPC_FLIP 0
static cmr_int s5k3p9sp04_drv_4in1_init(cmr_handle handle, cmr_u32 *param) {
    //      #include "fcell.h"
    cmr_int rtn = SENSOR_SUCCESS;
//    ov4c_init();//unsigned short *xtalk_data, unsigned short *otp_data, int
//    otpdpc_len)
#if 1
    int ret = RET_OK;
    int imgsize = IMG_WIDTH * IMG_HEIGHT * 2;
    ssfcell_init init;
    init.width = IMG_WIDTH;
    init.height = IMG_HEIGHT;
    // Step 1: Init Fcell Library
    init.xtalk_len = XTALK_LEN;
    init.pedestal = XTALK_BLC;
#ifndef SENSOR_s5k3p9sp04_MIRROR_FLIP
    init.bayer_order = BAYER_GRBG;
#else
    init.bayer_order = BAYER_GBRG;
#endif
#if 0
   const char *fcellXtalkFile = XTALK_DATA;
   char pFcellXtalk[XTALK_LEN];
   FILE *fp1 = fopen(fcellXtalkFile, "rb");
   if(fp1){
        fread(pFcellXtalk, XTALK_LEN, 1, fp1);
        fclose(fp1);
   }
   init.xtalk = pFcellXtalk;//xtalk_data;//pFcellXtalk;//TODO
#else
#if 1
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u8 param_ptr[0x8ba + 2048] = {0x00, 0x00};
    hw_sensor_read_i2c(sns_drv_cxt->hw_handle, 0xa2 >> 1, param_ptr,
                       0x8ba + 2048 << 16 | SENSOR_I2C_REG_16BIT);
        SENSOR_LOGI("xtalk: pid_value = %x, ver_value = %x",
        param_ptr[0x8ba], param_ptr[0x8ba+1]);
    init.xtalk = param_ptr + 0x8ba; // xtalk_data;//pFcellXtalk;//TODO
#else
    cmr_u8 *param_ptr = (cmr_u8 *)param;
    init.xtalk = param_ptr + 0x08ba;
#endif
#endif
    //pOutImage = malloc(imgsize);
#endif
    handlelib = dlopen("libsprd_fcell_ss.so", RTLD_NOW);
    if (handlelib == NULL) {
        char const *err_str = dlerror();
        SENSOR_LOGE("dlopen error %s", err_str ? err_str : "unknown");
    }
#if 1
    /* Get the address of the struct hal_module_info. */
    const char *sym = "ss4c_init";
    typedef int (*func_init)(ssfcell_init); // unsigned char *, unsigned char
                                            // *);
    func_init init_ss4c = (int *)dlsym(handlelib, sym);
    if (init_ss4c == NULL) {
        SENSOR_LOGI("load: couldn't find symbol %s", sym);
        return SENSOR_FAIL;
    } else {
        SENSOR_LOGI("link symbol success");
    }
    init_ss4c(init); //(unsigned char *)param, (unsigned char *)param);
#endif
    return rtn;
}

static cmr_int s5k3p9sp04_drv_4in1_process(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;

    struct frame_4in1_info *frame_4in1 = (struct frame_4in1_info *)param;
    cmr_uint imgsize = IMG_WIDTH * IMG_HEIGHT * 2;

/*    uint16 *pFcellImage = (uint16 *)frame_4in1->im_addr_in;
    uint16 *pOutImage;
    pOutImage = malloc(imgsize);*/
    int32_t pFcellImagein_fd = frame_4in1->im_addr_in;//TODO need check fd?
    int32_t pFcellImageout_fd = frame_4in1->im_addr_out;
    uint16 * pFcellImage = (uint16 *)frame_4in1->im_addr_in;
    uint16 * pOutImage = (uint16 *)frame_4in1->im_addr_out;

#if 0
    const char *fcellImgFile = "/data/vendor/cameraserver/input.mipi_raw";
    const char *outImgFile = "/data/vendor/cameraserver/output1.raw";

         FILE *fp0;
#if 1
     // pFcellImage = (uint16 *)malloc(imgsize*1.25/2);
      fp0= fopen(fcellImgFile, "wb");
         if (fp0) {
           fwrite(pFcellImage, imgsize*1.25/2, 1, fp0);
            fclose(fp0);
            SENSOR_LOGI("write fcell image file %s pFcellImage %p\n", fcellImgFile,pFcellImage);
         } else {
            SENSOR_LOGI("Cannot open fcell image file %s\n", fcellImgFile);
            goto exit;
         }
#else
        pFcellImage = (uint16 *)malloc(imgsize*1.25/2);
        fp0= fopen(fcellImgFile, "rb");
         if (fp0) {
//           fwrite(pFcellImage, imgsize*1.25/2, 1, fp0);
           fread(pFcellImage, imgsize*1.25/2, 1, fp0);
            fclose(fp0);
            SENSOR_LOGI("read fcell image file %s pFcellImage %p\n", fcellImgFile,pFcellImage);
         } else {
            SENSOR_LOGI("Cannot open fcell image file %s\n", fcellImgFile);
            goto exit;
         }
#endif
#endif
#if 1
    const char *sym = "ss4c_process";
    typedef int (*func_proc)(unsigned short *, unsigned short *, struct st_remosaic_param*, uint32_t, uint32_t);
    func_proc proc_ss4c = (int *)dlsym(handlelib, sym);
    if (proc_ss4c == NULL) {
        SENSOR_LOGI("load: couldn't find symbol %s", sym);
        return SENSOR_FAIL;
    } else {
        SENSOR_LOGI("link symbol success");
    }
    struct st_remosaic_param p_param;
    p_param.wb_b_gain = frame_4in1->awb_gain.b_gain;//0x800;//
    p_param.wb_gb_gain =  frame_4in1->awb_gain.g_gain;
    p_param.wb_gr_gain =  frame_4in1->awb_gain.g_gain;
    p_param.wb_r_gain =  frame_4in1->awb_gain.r_gain;

    SENSOR_LOGD("awb gain %x %x %x", frame_4in1->awb_gain.r_gain,
    frame_4in1->awb_gain.b_gain, frame_4in1->awb_gain.g_gain);
    proc_ss4c((unsigned short *)pFcellImage, (unsigned short *)pOutImage, &p_param, IMG_WIDTH, IMG_HEIGHT);
    SENSOR_LOGI("X %x %x", frame_4in1->im_addr_in, pOutImage[1]);
/*    frame_4in1->im_addr_out = frame_4in1->im_addr_in; //(cmr_int)(void *)pFcellImage;
      frame_4in1->im_addr_in = (cmr_int)(void *)pFcellImage;//frame_4in1->im_addr_in;*/
#endif
#if 0
    fp0 = fopen(outImgFile, "wb");
    if (fp0) {
        fwrite(pOutImage, imgsize* 2, 1, fp0);
        fclose(fp0);
    }
#if 1
    fp0 = fopen("/data/vendor/cameraserver/input-new.mipi_raw", "wb");
    if (fp0) {
        fwrite(pFcellImage, 1,imgsize * 5 /8, fp0);
        fclose(fp0);
    }
#endif
    param = (cmr_u32 *)pFcellImage;
exit:
#endif
    return rtn;
}
static cmr_int s5k3p9sp04_drv_4in1_deinit(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    /**
	void remosaic_deinit();

    */
    const char *sym = "ss4c_release";
    typedef int (*func_release)();
    func_release release_ss4c = NULL;
    if (handlelib)
        release_ss4c = (int *)dlsym(handlelib, sym);
    if (release_ss4c == NULL) {
        SENSOR_LOGI("load: couldn't find symbol %s", sym);
        return SENSOR_FAIL;
    } else {
        SENSOR_LOGI("link symbol success");
    }
    release_ss4c();
    if (handlelib) {
        dlclose(handlelib);
        handlelib = NULL;
    }
    return rtn;
}

#endif

static cmr_s64 s5k3p9sp04_drv_get_shutter_skew(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    cmr_u16 height = 0;
    cmr_s64 line_time = 0;
    cmr_s64 shutter_skew = 0;
    SENSOR_IC_CHECK_PTR(param);
    SENSOR_LOGD("E\n");
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_shutter_skew_info *shutter_skew_info =
        (struct sensor_shutter_skew_info *)param;
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    height = s_s5k3p9sp04_resolution_tab_raw[0].reg_tab[shutter_skew_info->sns_mode].height;
    line_time = s_s5k3p9sp04_resolution_trim_tab[0].trim_info[shutter_skew_info->sns_mode].line_time;
    shutter_skew = (height - 1) * line_time;
    shutter_skew_info->shutter_skew = shutter_skew;
    SENSOR_LOGD("sensor_mode:%d, height:%d, line_time:%lld, shutter_skew:%lld",
                shutter_skew_info->sns_mode, height, line_time, shutter_skew);
    return rtn;
}

static cmr_int s5k3p9sp04_drv_dump_trace(cmr_handle handle, cmr_u32 *param) {
    cmr_int rtn = SENSOR_SUCCESS;
    cmr_u16 value = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x40000005);
    SENSOR_LOGD("FRM_CNT 0x40000005:0x%x", value);

    return rtn;
}

/*==============================================================================
 * Description:
 * cfg otp setting
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5k3p9sp04_drv_access_val(cmr_handle handle, cmr_uint param) {
  cmr_int ret = SENSOR_FAIL;
  SENSOR_VAL_T *param_ptr = (SENSOR_VAL_T *)param;

  SENSOR_IC_CHECK_HANDLE(handle);
  SENSOR_IC_CHECK_PTR(param_ptr);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  SENSOR_LOGD("sensor s5k3p9sp04: param_ptr->type=%x", param_ptr->type);

  switch (param_ptr->type) {
  case SENSOR_VAL_TYPE_GET_STATIC_INFO:
    ret = s5k3p9sp04_drv_get_static_info(handle, param_ptr->pval);
    break;
  case SENSOR_VAL_TYPE_GET_FPS_INFO:
    ret = s5k3p9sp04_drv_get_fps_info(handle, param_ptr->pval);
    break;
  case SENSOR_VAL_TYPE_SET_SENSOR_CLOSE_FLAG:
    ret = sns_drv_cxt->is_sensor_close = 1;
    break;
  case SENSOR_VAL_TYPE_GET_PDAF_INFO:
    //ret = s5k3p9sp04_drv_get_pdaf_info(handle, param_ptr->pval);
    break;
#if 1
    case SENSOR_VAL_TYPE_GET_4IN1_INFO:
        ret = s5k3p9sp04_drv_get_4in1_info(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_INIT:
        ret = s5k3p9sp04_drv_4in1_init(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_PROC:
        ret = s5k3p9sp04_drv_4in1_process(handle, param_ptr->pval);
        break;
    case SENSOR_VAL_TYPE_4IN1_DEINIT:
        ret = s5k3p9sp04_drv_4in1_deinit(handle, param_ptr->pval);
        break;
#endif
    case SENSOR_VAL_TYPE_GET_SHUTTER_SKEW_DATA:
      ret = s5k3p9sp04_drv_get_shutter_skew(handle, param_ptr->pval);
      break;
  case SENSOR_VAL_TYPE_DUMP_TRACE:
        ret = s5k3p9sp04_drv_dump_trace(handle, param_ptr->pval);
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
static cmr_int s5k3p9sp04_drv_identify(cmr_handle handle, cmr_uint param) {
  cmr_u16 pid_value = 0x00;
  cmr_u16 ver_value = 0x00;
  cmr_int ret_value = SENSOR_FAIL;

  SENSOR_IC_CHECK_HANDLE(handle);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  pid_value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, s5k3p9sp04_PID_ADDR);

  if (s5k3p9sp04_PID_VALUE == pid_value) {
    ver_value =
        hw_sensor_read_reg(sns_drv_cxt->hw_handle, s5k3p9sp04_VER_ADDR);
    if (s5k3p9sp04_VER_VALUE == ver_value) {
      SENSOR_LOGI("this is s5k3p9sp04 sensor");
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
static cmr_int s5k3p9sp04_drv_before_snapshot(cmr_handle handle,
                                               cmr_uint param) {
  cmr_u32 prv_shutter = 0;
  cmr_u32 prv_gain = 0;
  cmr_u32 cap_shutter = 0;
  cmr_u32 cap_gain = 0;
  cmr_u32 capture_mode = param & 0xffff;
  cmr_u32 preview_mode = (param >> 0x10) & 0xffff;

  SENSOR_IC_CHECK_HANDLE(handle);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  cmr_u32 prv_linetime = sns_drv_cxt->trim_tab_info[preview_mode].line_time;
  cmr_u32 cap_linetime = sns_drv_cxt->trim_tab_info[capture_mode].line_time;

  SENSOR_LOGD("preview_mode=%d,capture_mode = %d", preview_mode, capture_mode);
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

  s5k3p9sp04_drv_calc_exposure(handle, cap_shutter, 0, capture_mode,
                                &s5k3p9sp04_aec_info);
  s5k3p9sp04_drv_write_reg2sensor(handle, s5k3p9sp04_aec_info.frame_length);
  s5k3p9sp04_drv_write_reg2sensor(handle, s5k3p9sp04_aec_info.shutter);

  sns_drv_cxt->sensor_ev_info.preview_gain = cap_gain;
  s5k3p9sp04_drv_write_gain(&s5k3p9sp04_aec_info, cap_gain);
  s5k3p9sp04_drv_write_reg2sensor(handle, s5k3p9sp04_aec_info.again);
  s5k3p9sp04_drv_write_reg2sensor(handle, s5k3p9sp04_aec_info.dgain);

snapshot_info:
  if (!sns_drv_cxt->ops_cb.set_exif_info) {
    sns_drv_cxt->exif_info.exposure_line = cap_shutter;
  }

  SENSOR_LOGI("capture_shutter = 0x%x, capture_gain = 0x%x", cap_shutter,
              cap_gain);

  return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * get the shutter from isp
 * please don't change this function unless it's necessary
 *============================================================================*/
static cmr_int s5k3p9sp04_drv_write_exposure(cmr_handle handle,
                                              cmr_uint param) {
  cmr_int ret_value = SENSOR_SUCCESS;
  cmr_u16 exposure_line = 0x00;
  cmr_u16 dummy_line = 0x00;
  cmr_u16 size_index = 0x00;
  // cmr_u16 frame_interval = 0x00;

  struct sensor_ex_exposure *ex = (struct sensor_ex_exposure *)param;
  SENSOR_IC_CHECK_HANDLE(handle);
  SENSOR_IC_CHECK_HANDLE(ex);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  exposure_line = ex->exposure;
  dummy_line = ex->dummy;
  size_index = ex->size_index;

  s5k3p9sp04_drv_calc_exposure(handle, exposure_line, dummy_line, size_index,
                                &s5k3p9sp04_aec_info);
  s5k3p9sp04_drv_write_reg2sensor(handle, s5k3p9sp04_aec_info.frame_length);
  s5k3p9sp04_drv_write_reg2sensor(handle, s5k3p9sp04_aec_info.shutter);

  return ret_value;
}

/*==============================================================================
 * Description:
 * write gain value to sensor
 * you can change this function if it's necessary
 *============================================================================*/
static cmr_int s5k3p9sp04_drv_write_gain_value(cmr_handle handle,
                                                cmr_uint param) {
  cmr_int ret_value = SENSOR_SUCCESS;

  SENSOR_IC_CHECK_HANDLE(handle);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  s5k3p9sp04_drv_calc_gain(handle, param, &s5k3p9sp04_aec_info);
  s5k3p9sp04_drv_write_reg2sensor(handle, s5k3p9sp04_aec_info.again);
  s5k3p9sp04_drv_write_reg2sensor(handle, s5k3p9sp04_aec_info.dgain);

  return ret_value;
}

static cmr_int s5k3p9sp04_drv_read_aec_info(cmr_handle handle,
                                             cmr_uint param) {
  cmr_int ret_value = SENSOR_SUCCESS;
  struct sensor_aec_reg_info *info = (struct sensor_aec_reg_info *)param;
  cmr_u16 exposure_line = 0x00;
  cmr_u16 dummy_line = 0x00;
  cmr_u16 mode = 0x00;
  SENSOR_IC_CHECK_HANDLE(handle);
  SENSOR_IC_CHECK_PTR(info);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  SENSOR_LOGD("E");

  info->aec_i2c_info_out = &s5k3p9sp04_aec_info;
  exposure_line = info->exp.exposure;
  dummy_line = info->exp.dummy;
  mode = info->exp.size_index;

  s5k3p9sp04_drv_calc_exposure(handle, exposure_line, dummy_line, mode,
                                &s5k3p9sp04_aec_info);
  s5k3p9sp04_drv_calc_gain(handle, info->gain, &s5k3p9sp04_aec_info);

  return ret_value;
}

static cmr_int s5k3p9sp04_drv_set_master_FrameSync(cmr_handle handle,
                                                    cmr_uint param) {
  SENSOR_IC_CHECK_HANDLE(handle);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  SENSOR_LOGI("E");

  /*TODO*/

  hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x3002, 0x40);

  /*END*/

  return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * mipi stream on
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5k3p9sp04_drv_stream_on(cmr_handle handle, cmr_uint param) {
  SENSOR_IC_CHECK_HANDLE(handle);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  SENSOR_LOGI("E");

    char value2[PROPERTY_VALUE_MAX];
    property_get("persist.vendor.cam.colorbar", value2, "0");
    if (!strcmp(value2, "1")) {
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0600, 0x02);
    }

    char value1[PROPERTY_VALUE_MAX];
    property_get("vendor.cam.hw.framesync.on", value1, "1");
    if (!strcmp(value1, "1")) {
#if 0//defined(CONFIG_DUAL_MODULE)
        s5k3p9sp04_drv_set_master_FrameSync(handle, param);
#endif
    }

#ifdef SENSOR_s5k3p9sp04_MIRROR_FLIP
  hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0103);
#else
  hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0100);
#endif
  // add for cts
  mStreamOnTime = systemTime(SYSTEM_TIME_BOOTTIME);
  return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * mipi stream off
 * please modify this function acording your spec
 *============================================================================*/
static cmr_int s5k3p9sp04_drv_stream_off(cmr_handle handle, cmr_uint param) {
    SENSOR_LOGI("E");

    cmr_u16 value = 0;
    cmr_u16 sleep_time = 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
#define STREAM_ON_TO_OFF_DELAYMS (20)
    mStreamOffTime = systemTime(SYSTEM_TIME_BOOTTIME);
    if ((mStreamOffTime - mStreamOnTime) < STREAM_ON_TO_OFF_DELAYMS * 1000000) {
        usleep(STREAM_ON_TO_OFF_DELAYMS * 1000);
    }
    value = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0100);
    if (((value >> 8) & 0xFF) != 0x00) {
#ifdef SENSOR_s5k3p9sp04_MIRROR_FLIP
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0003);
#else
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0000);
#endif
        if (!sns_drv_cxt->is_sensor_close) {
            sleep_time = (sns_drv_cxt->sensor_ev_info.preview_framelength *
                        sns_drv_cxt->line_time_def / 1000000) + 10;
            usleep(sleep_time * 1000);
            SENSOR_LOGI("stream_off delay_ms %d", sleep_time);
        }
    } else {
#ifdef SENSOR_s5k3p9sp04_MIRROR_FLIP
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0003);
#else
        hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0100, 0x0000);
#endif
    }

    sns_drv_cxt->is_sensor_close = 0;

    SENSOR_LOGI("X");
    return SENSOR_SUCCESS;
}

static int s5k3p9_4in1_is_init = 0;

static cmr_int
s5k3p9sp04_drv_handle_create(struct sensor_ic_drv_init_para *init_param,
                              cmr_handle *sns_ic_drv_handle) {
  cmr_int ret = SENSOR_SUCCESS;
  struct sensor_ic_drv_cxt *sns_drv_cxt = NULL;
  void *pri_data = NULL;

  ret = sensor_ic_drv_create(init_param, sns_ic_drv_handle);
  sns_drv_cxt = *sns_ic_drv_handle;

  sns_drv_cxt->sensor_ev_info.preview_shutter =
      PREVIEW_FRAME_LENGTH - FRAME_OFFSET;
  sns_drv_cxt->sensor_ev_info.preview_gain = SENSOR_BASE_GAIN;
  sns_drv_cxt->sensor_ev_info.preview_framelength = PREVIEW_FRAME_LENGTH;

  sns_drv_cxt->frame_length_def = PREVIEW_FRAME_LENGTH;
  sns_drv_cxt->line_time_def = PREVIEW_LINE_TIME;

  sensor_ic_set_match_module_info(sns_drv_cxt,
                                  ARRAY_SIZE(s_s5k3p9sp04_module_info_tab),
                                  s_s5k3p9sp04_module_info_tab);
  sensor_ic_set_match_resolution_info(
      sns_drv_cxt, ARRAY_SIZE(s_s5k3p9sp04_resolution_tab_raw),
      s_s5k3p9sp04_resolution_tab_raw);
  sensor_ic_set_match_trim_info(sns_drv_cxt,
                                ARRAY_SIZE(s_s5k3p9sp04_resolution_trim_tab),
                                s_s5k3p9sp04_resolution_trim_tab);
  sensor_ic_set_match_static_info(sns_drv_cxt,
                                  ARRAY_SIZE(s_s5k3p9sp04_static_info),
                                  s_s5k3p9sp04_static_info);
  sensor_ic_set_match_fps_info(sns_drv_cxt,
                               ARRAY_SIZE(s_s5k3p9sp04_mode_fps_info),
                               s_s5k3p9sp04_mode_fps_info);

  /*init exif info,this will be deleted in the future*/
  s5k3p9sp04_drv_init_fps_info(sns_drv_cxt);
  /*add private here*/
  return ret;
}

static cmr_int s5k3p9sp04_drv_handle_delete(cmr_handle handle, void *param) {
  cmr_int ret = SENSOR_SUCCESS;

  SENSOR_IC_CHECK_HANDLE(handle);
  struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

  ret = sensor_ic_drv_delete(handle, param);
  return ret;
}

static cmr_int s5k3p9sp04_drv_get_private_data(cmr_handle handle, cmr_uint cmd,
                                                void **param) {
  cmr_int ret = SENSOR_SUCCESS;
  SENSOR_IC_CHECK_HANDLE(handle);
  SENSOR_IC_CHECK_PTR(param);

  ret = sensor_ic_get_private_data(handle, cmd, param);
  return ret;
}

void *sensor_ic_open_lib(void)
{
     return &g_s5k3p9sp04_mipi_raw_info;
}

/*==============================================================================
 * Description:
 * all ioctl functoins
 * you can add functions reference SENSOR_IOCTL_FUNC_TAB_T from sensor_drv_u.h
 *
 * add ioctl functions like this:
 * .power = s5k3p9sp04_power_on,
 *============================================================================*/
static struct sensor_ic_ops s_s5k3p9sp04_ops_tab = {
    .create_handle = s5k3p9sp04_drv_handle_create,
    .delete_handle = s5k3p9sp04_drv_handle_delete,
    .get_data = s5k3p9sp04_drv_get_private_data,
    .power = s5k3p9sp04_drv_power_on,
    .identify = s5k3p9sp04_drv_identify,
    .ex_write_exp = s5k3p9sp04_drv_write_exposure,
    .write_gain_value = s5k3p9sp04_drv_write_gain_value,
#if defined(CONFIG_DUAL_MODULE)
    .read_aec_info = s5k3p9sp04_drv_read_aec_info,
#endif

    .ext_ops = {
            [SENSOR_IOCTL_BEFORE_SNAPSHOT].ops =
                s5k3p9sp04_drv_before_snapshot,
            [SENSOR_IOCTL_STREAM_ON].ops = s5k3p9sp04_drv_stream_on,
            [SENSOR_IOCTL_STREAM_OFF].ops = s5k3p9sp04_drv_stream_off,
            /* expand interface,if you want to add your sub cmd ,
             *  you can add it in enum {@SENSOR_IOCTL_VAL_TYPE}
             */
            [SENSOR_IOCTL_ACCESS_VAL].ops = s5k3p9sp04_drv_access_val,
    }};
