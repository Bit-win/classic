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

#ifndef _SENSOR_BF30A2_MIPI_RAW_H_
#define _SENSOR_BF30A2_MIPI_RAW_H_

#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

//#include "parameters/sensor_bf30a2_raw_param_main.c"

//#define FEATURE_OTP

#define VENDOR_NUM 1
#define SENSOR_NAME				"c3z02_bydbf30a2_depthjk"
#define I2C_SLAVE_ADDR			0xdc   /* 8bit slave address*/

#define bf30a2_PID_ADDR			0xfc
#define bf30a2_PID_VALUE		0x3b
#define bf30a2_VER_ADDR			0xfd
#define bf30a2_VER_VALUE		0x02

/* sensor parameters begin */
/* effective sensor output image size */
#define PREVIEW_WIDTH			240
#define PREVIEW_HEIGHT			320
#define SNAPSHOT_WIDTH			240 
#define SNAPSHOT_HEIGHT			320

/*Raw Trim parameters*/
#define PREVIEW_TRIM_X			0
#define PREVIEW_TRIM_Y			0
#define PREVIEW_TRIM_W			240
#define PREVIEW_TRIM_H			320
#define SNAPSHOT_TRIM_X			0
#define SNAPSHOT_TRIM_Y			0
#define SNAPSHOT_TRIM_W			240
#define SNAPSHOT_TRIM_H			320

/*Mipi output*/
#define LANE_NUM				1
#define RAW_BITS				8

#define PREVIEW_MIPI_PER_LANE_BPS	  120  /* 2*Mipi clk */
#define SNAPSHOT_MIPI_PER_LANE_BPS	  120  /* 2*Mipi clk */

/*line time unit: 1ns*/
#define PREVIEW_LINE_TIME		  65333
#define SNAPSHOT_LINE_TIME		65333

/* frame length*/
#define PREVIEW_FRAME_LENGTH		500
#define SNAPSHOT_FRAME_LENGTH		500

/* please ref your spec */
#define FRAME_OFFSET			1
#define SENSOR_MAX_GAIN			0x100
#define SENSOR_BASE_GAIN		0x10
#define SENSOR_MIN_SHUTTER		1

/* please ref your spec
 * 1 : average binning
 * 2 : sum-average binning
 * 4 : sum binning
 */
#define BINNING_FACTOR			1

/* please ref spec
 * 1: sensor auto caculate
 * 0: driver caculate
 */
#define SUPPORT_AUTO_FRAME_LENGTH	0

/* sensor parameters end */

/* isp parameters, please don't change it*/
#define ISP_BASE_GAIN			0x80

/* please don't change it */
#define EX_MCLK				24

/*==============================================================================
 * Description:
 * register setting
 *============================================================================*/

static const SENSOR_REG_T bf30a2_init_setting[] = {
	//BF3A02 SPI ZX V1 20200319
//XCLK:24M; SPICLK: 24M; MCLK:3M
//Width:240; Height: 320
//行长：294  帧长：340
//Max fps: 15fps  
{0xf2, 0x01},
{0x12, 0x10},
{0x15, 0x10},
{0x6b, 0x71},
{0x04, 0x00},
{0x06, 0x26},
{0x08, 0x07},
{0x1c, 0x12},
{0x1e, 0x26},
{0x1f, 0x01},
{0x20, 0x20},
{0x21, 0x20},
{0x34, 0x00},
{0x35, 0x00},
{0x36, 0x21},
{0x37, 0x13},
{0xca, 0x03},
{0xcb, 0x22},
{0xcc, 0x89},
{0xcd, 0x6c},
{0xce, 0x6b},
{0xcf, 0xf0},
{0xa0, 0x8e},
{0x01, 0x1b},
{0x02, 0x1d},
{0x13, 0x48},
{0x87, 0x20},
{0x8a, 0x33},
{0x8b, 0x70},
{0x70, 0x1f},
{0x71, 0x40},
{0x72, 0x0a},
{0x73, 0x62},
{0x74, 0xa2},
{0x75, 0xbf},
{0x76, 0x02},
{0x77, 0xcc},
{0x40, 0x32},
{0x41, 0x28},
{0x42, 0x26},
{0x43, 0x1d},
{0x44, 0x1a},
{0x45, 0x14},
{0x46, 0x11},
{0x47, 0x0f},
{0x48, 0x0e},
{0x49, 0x0d},
{0x4b, 0x0c},
{0x4c, 0x0b},
{0x4e, 0x0a},
{0x4f, 0x09},
{0x50, 0x09},
{0x24, 0x50},
{0x25, 0x36},
{0x80, 0x00},
{0x81, 0x20},
{0x82, 0x40},
{0x83, 0x30},
{0x84, 0x50},
{0x85, 0x30},
{0x86, 0xd8},
{0x89, 0x45},
{0x8f, 0x81},
{0x91, 0xff},
{0x92, 0x08},
{0x94, 0x82},
{0x95, 0xfd},
{0x9a, 0x20},
{0x9e, 0xbc},
{0xf0, 0x8f},
{0xf1, 0x02},
{0x51, 0x06},
{0x52, 0x25},
{0x53, 0x2b},
{0x54, 0x0f},
{0x57, 0x2a},
{0x58, 0x22},
{0x59, 0x2c},
{0x23, 0x33},
{0xa0, 0x8f},
{0xa1, 0x13},
{0xa2, 0x0f},
{0xa3, 0x2a},
{0xa4, 0x08},
{0xa5, 0x26},
{0xa7, 0x80},
{0xa8, 0x80},
{0xa9, 0x1e},
{0xaa, 0x19},
{0xab, 0x18},
{0xae, 0x50},
{0xaf, 0x04},
{0xc8, 0x10},
{0xc9, 0x15},
{0xd3, 0x0c},
{0xd4, 0x16},
{0xee, 0x06},
{0xef, 0x04},
{0x55, 0x30},
{0x56, 0x9c},
{0xb1, 0x98},
{0xb2, 0x98},
{0xb3, 0xc4},
{0xb4, 0x0c},
{0x00, 0x40},
{0x13, 0x48},
};

static const SENSOR_REG_T bf30a2_preview_setting[] = {
};

static const SENSOR_REG_T bf30a2_snapshot_setting[] = {
};

static struct sensor_res_tab_info s_bf30a2_resolution_tab_raw[VENDOR_NUM] = {
	{
      .module_id = MODULE_SUNNY,
      .reg_tab = {
        {ADDR_AND_LEN_OF_ARRAY(bf30a2_init_setting), PNULL, 0,
        .width = 0, .height = 0,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},
		
        {ADDR_AND_LEN_OF_ARRAY(bf30a2_preview_setting), PNULL, 0,
        .width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

        {ADDR_AND_LEN_OF_ARRAY(bf30a2_snapshot_setting), PNULL, 0,
        .width = SNAPSHOT_WIDTH, .height = SNAPSHOT_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW}
		}
	}
/*If there are multiple modules,please add here*/
};

static SENSOR_TRIM_T s_bf30a2_resolution_trim_tab[VENDOR_NUM] = {
{
     .module_id = MODULE_SUNNY,
     .trim_info = {
       {0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	   
	   {.trim_start_x = PREVIEW_TRIM_X, .trim_start_y = PREVIEW_TRIM_Y,
        .trim_width = PREVIEW_TRIM_W,   .trim_height = PREVIEW_TRIM_H,
        .line_time = PREVIEW_LINE_TIME, .bps_per_lane = PREVIEW_MIPI_PER_LANE_BPS,
        .frame_line = PREVIEW_FRAME_LENGTH,
        .scaler_trim = {.x = PREVIEW_TRIM_X, .y = PREVIEW_TRIM_Y, .w = PREVIEW_TRIM_W, .h = PREVIEW_TRIM_H}},
       
	   {
        .trim_start_x = SNAPSHOT_TRIM_X, .trim_start_y = SNAPSHOT_TRIM_Y,
        .trim_width = SNAPSHOT_TRIM_W,   .trim_height = SNAPSHOT_TRIM_H,
        .line_time = SNAPSHOT_LINE_TIME, .bps_per_lane = SNAPSHOT_MIPI_PER_LANE_BPS,
        .frame_line = SNAPSHOT_FRAME_LENGTH,
        .scaler_trim = {.x = SNAPSHOT_TRIM_X, .y = SNAPSHOT_TRIM_Y, .w = SNAPSHOT_TRIM_W, .h = SNAPSHOT_TRIM_H}},
       }
	}

    /*If there are multiple modules,please add here*/

};

static SENSOR_REG_T bf30a2_shutter_reg[] = {
};

static struct sensor_i2c_reg_tab bf30a2_shutter_tab = {
    .settings = bf30a2_shutter_reg, 
	.size = ARRAY_SIZE(bf30a2_shutter_reg),
};

static SENSOR_REG_T bf30a2_again_reg[] = {
};

static struct sensor_i2c_reg_tab bf30a2_again_tab = {
    .settings = bf30a2_again_reg, 
	.size = ARRAY_SIZE(bf30a2_again_reg),
};

static SENSOR_REG_T bf30a2_dgain_reg[] = {
   
};

static struct sensor_i2c_reg_tab bf30a2_dgain_tab = {
    .settings = bf30a2_dgain_reg, 
	.size = ARRAY_SIZE(bf30a2_dgain_reg),
};

static SENSOR_REG_T bf30a2_frame_length_reg[] = {
};

static struct sensor_i2c_reg_tab bf30a2_frame_length_tab = {
    .settings = bf30a2_frame_length_reg,
    .size = ARRAY_SIZE(bf30a2_frame_length_reg),
};

static struct sensor_aec_i2c_tag bf30a2_aec_info = {
    .slave_addr = (I2C_SLAVE_ADDR >> 1),
    .addr_bits_type = SENSOR_I2C_REG_8BIT,
    .data_bits_type = SENSOR_I2C_VAL_8BIT,
    .shutter = &bf30a2_shutter_tab,
    .again = &bf30a2_again_tab,
    .dgain = &bf30a2_dgain_tab,
    .frame_length = &bf30a2_frame_length_tab,
};


static SENSOR_STATIC_INFO_T s_bf30a2_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .static_info = {
        .f_num = 300,
        .focal_length = 354,
        .max_fps = 15,
        .max_adgain = 4,
        .ois_supported = 0,
        .pdaf_supported = 0,
        .exp_valid_frame_num = 1,
        .clamp_level = 0,
        .adgain_valid_frame_num = 1,
        .fov_info = {{4.614f, 3.444f}, 4.222f}}
    }
    /*If there are multiple modules,please add here*/
};

static SENSOR_MODE_FPS_INFO_T s_bf30a2_mode_fps_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
       {.is_init = 0,
         {{SENSOR_MODE_COMMON_INIT, 0, 1, 0, 0},
         {SENSOR_MODE_PREVIEW_ONE, 0, 1, 0, 0},
         {SENSOR_MODE_SNAPSHOT_ONE_FIRST, 0, 1, 0, 0},
         {SENSOR_MODE_SNAPSHOT_ONE_SECOND, 0, 1, 0, 0},
         {SENSOR_MODE_SNAPSHOT_ONE_THIRD, 0, 1, 0, 0},
         {SENSOR_MODE_PREVIEW_TWO, 0, 1, 0, 0},
         {SENSOR_MODE_SNAPSHOT_TWO_FIRST, 0, 1, 0, 0},
         {SENSOR_MODE_SNAPSHOT_TWO_SECOND, 0, 1, 0, 0},
         {SENSOR_MODE_SNAPSHOT_TWO_THIRD, 0, 1, 0, 0}}}
    }
    /*If there are multiple modules,please add here*/
};

static struct sensor_module_info s_bf30a2_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {
         .major_i2c_addr = I2C_SLAVE_ADDR >> 1,
         .minor_i2c_addr = I2C_SLAVE_ADDR >> 1,

         .reg_addr_value_bits = SENSOR_I2C_REG_8BIT | SENSOR_I2C_VAL_8BIT |
                                SENSOR_I2C_FREQ_400,

         .avdd_val = SENSOR_AVDD_2800MV,
         .iovdd_val = SENSOR_AVDD_1800MV,
         .dvdd_val = SENSOR_AVDD_1800MV,

         .image_pattern = SENSOR_IMAGE_PATTERN_YUV422_YUYV,

         .preview_skip_num = 1,
         .capture_skip_num = 1,
         .flash_capture_skip_num = 6,
         .mipi_cap_skip_num = 0,
         .preview_deci_num = 0,
         .video_preview_deci_num = 0,

         .threshold_eb = 0,
         .threshold_mode = 0,
         .threshold_start = 0,
         .threshold_end = 0,

         .sensor_interface = {
              .type = SENSOR_INTERFACE_TYPE_CSI2,
              .bus_width = LANE_NUM,
              .pixel_width = RAW_BITS,
              .is_loose = 0,
          },
         .change_setting_skip_num = 1,
         .horizontal_view_angle = 65,
         .vertical_view_angle = 60
      }
    }

/*If there are multiple modules,please add here*/
};

static struct sensor_ic_ops s_bf30a2_ops_tab;
struct sensor_raw_info *s_bf30a2_mipi_raw_info_ptr = NULL;

SENSOR_INFO_T g_bf30a2_mipi_yuv_info = {
    .hw_signal_polarity = SENSOR_HW_SIGNAL_PCLK_P | SENSOR_HW_SIGNAL_VSYNC_P |
                          SENSOR_HW_SIGNAL_HSYNC_P,
    .environment_mode = SENSOR_ENVIROMENT_NORMAL | SENSOR_ENVIROMENT_NIGHT,
    .image_effect = SENSOR_IMAGE_EFFECT_NORMAL |
                    SENSOR_IMAGE_EFFECT_BLACKWHITE | SENSOR_IMAGE_EFFECT_RED |
                    SENSOR_IMAGE_EFFECT_GREEN | SENSOR_IMAGE_EFFECT_BLUE |
                    SENSOR_IMAGE_EFFECT_YELLOW | SENSOR_IMAGE_EFFECT_NEGATIVE |
                    SENSOR_IMAGE_EFFECT_CANVAS,

    .wb_mode = 0,
    .step_count = 7,
    .reset_pulse_level = SENSOR_LOW_PULSE_RESET,
    .reset_pulse_width = 100,
    .power_down_level = SENSOR_HIGH_LEVEL_PWDN,
    .identify_count = 1,
    .identify_code =
        {{ .reg_addr = bf30a2_PID_ADDR, .reg_value = bf30a2_PID_VALUE},
         { .reg_addr = bf30a2_VER_ADDR, .reg_value = bf30a2_VER_VALUE}},

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_YUV422,

    .module_info_tab = s_bf30a2_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_bf30a2_module_info_tab),

    .resolution_tab_info_ptr = s_bf30a2_resolution_tab_raw,
    .sns_ops = &s_bf30a2_ops_tab,
    .raw_info_ptr = &s_bf30a2_mipi_raw_info_ptr,

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"BF30A2_C&T",
};
#endif
