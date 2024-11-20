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

#ifndef _SENSOR_SC080CS_MIPI_RAW_H_
#define _SENSOR_SC080CS_MIPI_RAW_H_

#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

//#include "parameters/sensor_sc080cs_raw_param_main.c"

//#define FEATURE_OTP

#define VENDOR_NUM 1
#define SENSOR_NAME				"c3z03_sc080cs_depthcxt"
#define I2C_SLAVE_ADDR			0x6C   /* 8bit slave address*/

#define sc080cs_PID_ADDR		0x3107
#define sc080cs_PID_VALUE		0x3a
#define sc080cs_VER_ADDR		0x3108
#define sc080cs_VER_VALUE		0x6C

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
#define RAW_BITS				10

#define PREVIEW_MIPI_PER_LANE_BPS	  240   /* 2*Mipi clk */
#define SNAPSHOT_MIPI_PER_LANE_BPS	  240  /* 2*Mipi clk */

/*line time unit: 1ns*/
#define PREVIEW_LINE_TIME		  94000
#define SNAPSHOT_LINE_TIME		  94000

/* frame length*/
#define PREVIEW_FRAME_LENGTH		442
#define SNAPSHOT_FRAME_LENGTH		442

/* please ref your spec */
#define FRAME_OFFSET			6
#define SENSOR_MAX_GAIN			0xabcd
#define SENSOR_BASE_GAIN		0x20
#define SENSOR_MIN_SHUTTER		6

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

static const SENSOR_REG_T sc080cs_init_setting[] = {
    {0x0103,0x01},
	{0x0100,0x00},
	{0x8001,0x18},
	{0x5001,0x01},
	{0x30b0,0x0a},
	{0x470b,0x0a},
	{0x470d,0x06},
	{0x309b,0xf1},
	{0x0100,0x01},
	{0x5170,0x80},
	{0x5172,0xc1},      
	{0x518b,0x00},
	{0x518c,0x20},
	{0x518d,0x02},
	{0x518e,0x00},
	{0x519e,0x10},
	{0x518f,0x01},                       
	{0x518f,0x00},             
	{0x309b,0xf0},             
	{0x5170,0x2c},             
	{0x3908,0x41},            
	{0x5000,0xc2},             
	{0x3637,0x48},             
	{0x330b,0x40},             
	{0x3908,0x00},      
	{0x3948,0x00},             
	{0x320c,0x01},                                                                    
	{0x320d,0x6a},             
	{0x320e,0x01},             
	{0x320f,0xba},                                             
	{0x3301,0x04},                                             
	{0x3630,0x45},                                             
	{0x363a,0x90},                                             
	{0x330a,0x00},                                             
	{0x330b,0x50},                                             
	{0x3305,0x00},                                             
	{0x3306,0x18},
	{0x4509,0x30},
	{0x5000,0xc0},
	{0x518d,0x03},
	{0x518e,0x6a},
	{0x3308,0x04},
	{0x3904,0x0f},
	{0x3948,0x00},
	{0x3307,0x02},
	{0x3310,0x01},
	{0x334c,0x01},
	{0x330f,0x01},
	{0x3310,0x01},
	{0x330e,0x01},
	{0x391f,0x41},
	{0x37f9,0x29},
	{0x3306,0x0c},
	{0x330b,0x28},
	{0x3304,0x0c},
	{0x331e,0x09},
	{0x3637,0x47},
	{0x3900,0x91},
	{0x391b,0x07},
	{0x391c,0x0a},
	{0x391d,0x15},
	{0x391e,0x28},
	{0x4509,0x20},
	{0x4509,0x10},
	{0x3306,0x10},
	{0x330b,0x2c},
	{0x3630,0x65},
	{0x330b,0x30},
	{0x3333,0x10}, 
	{0x30b8,0x21},
	{0x5000,0xc2},
	{0x3301,0x04},
	{0x330a,0x00},
	{0x3305,0x00},
	{0x3306,0x10},
	{0x3630,0x65},
	{0x330b,0x30},
	{0x4509,0x10},
	{0x363a,0xe0},
	{0x3670,0x01}, 
	{0x3694,0x03}, 
	{0x3695,0x07},
	{0x3696,0x07},
	{0x3690,0xe0},
	{0x3691,0xe1},
	{0x3692,0xe1},
	{0x3693,0xe1},
	{0x300a,0x00},
};

static const SENSOR_REG_T sc080cs_preview_setting[] = {
};

static const SENSOR_REG_T sc080cs_snapshot_setting[] = {
};

static struct sensor_res_tab_info s_sc080cs_resolution_tab_raw[VENDOR_NUM] = {
	{
      .module_id = MODULE_SUNNY,
      .reg_tab = {
        {ADDR_AND_LEN_OF_ARRAY(sc080cs_init_setting), PNULL, 0,
        .width = 0, .height = 0,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},
		
        {ADDR_AND_LEN_OF_ARRAY(sc080cs_preview_setting), PNULL, 0,
        .width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

        {ADDR_AND_LEN_OF_ARRAY(sc080cs_snapshot_setting), PNULL, 0,
        .width = SNAPSHOT_WIDTH, .height = SNAPSHOT_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW}
		}
	}
/*If there are multiple modules,please add here*/
};

static SENSOR_TRIM_T s_sc080cs_resolution_trim_tab[VENDOR_NUM] = {
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

static SENSOR_REG_T sc080cs_shutter_reg[] = {
};

static struct sensor_i2c_reg_tab sc080cs_shutter_tab = {
    .settings = sc080cs_shutter_reg, 
	.size = ARRAY_SIZE(sc080cs_shutter_reg),
};

static SENSOR_REG_T sc080cs_again_reg[] = {
};

static struct sensor_i2c_reg_tab sc080cs_again_tab = {
    .settings = sc080cs_again_reg, 
	.size = ARRAY_SIZE(sc080cs_again_reg),
};

static SENSOR_REG_T sc080cs_dgain_reg[] = {
   
};

static struct sensor_i2c_reg_tab sc080cs_dgain_tab = {
    .settings = sc080cs_dgain_reg, 
	.size = ARRAY_SIZE(sc080cs_dgain_reg),
};

static SENSOR_REG_T sc080cs_frame_length_reg[] = {
};

static struct sensor_i2c_reg_tab sc080cs_frame_length_tab = {
    .settings = sc080cs_frame_length_reg,
    .size = ARRAY_SIZE(sc080cs_frame_length_reg),
};

static struct sensor_aec_i2c_tag sc080cs_aec_info = {
    .slave_addr = (I2C_SLAVE_ADDR >> 1),
    .addr_bits_type = SENSOR_I2C_REG_16BIT,
    .data_bits_type = SENSOR_I2C_VAL_8BIT,
    .shutter = &sc080cs_shutter_tab,
    .again = &sc080cs_again_tab,
    .dgain = &sc080cs_dgain_tab,
    .frame_length = &sc080cs_frame_length_tab,
};


static SENSOR_STATIC_INFO_T s_sc080cs_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .static_info = {
        .f_num = 200,
        .focal_length = 354,
        .max_fps = 30,
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

static SENSOR_MODE_FPS_INFO_T s_sc080cs_mode_fps_info[VENDOR_NUM] = {
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

static struct sensor_module_info s_sc080cs_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {
         .major_i2c_addr = I2C_SLAVE_ADDR >> 1,
         .minor_i2c_addr = I2C_SLAVE_ADDR >> 1,

         .reg_addr_value_bits = SENSOR_I2C_REG_16BIT | SENSOR_I2C_VAL_8BIT |
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

static struct sensor_ic_ops s_sc080cs_ops_tab;
struct sensor_raw_info *s_sc080cs_mipi_raw_info_ptr = PNULL;

SENSOR_INFO_T g_sc080cs_mipi_yuv_info = {
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
        {{ .reg_addr = sc080cs_PID_ADDR, .reg_value = sc080cs_PID_VALUE},
         { .reg_addr = sc080cs_VER_ADDR, .reg_value = sc080cs_VER_VALUE}},

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_YUV422,

    .module_info_tab = s_sc080cs_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_sc080cs_module_info_tab),

    .resolution_tab_info_ptr = s_sc080cs_resolution_tab_raw,
    .sns_ops = &s_sc080cs_ops_tab,
    .raw_info_ptr = &s_sc080cs_mipi_raw_info_ptr,

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"sc080cs_v1",
};
#endif

