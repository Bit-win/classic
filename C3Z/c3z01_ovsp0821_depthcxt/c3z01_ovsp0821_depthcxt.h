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

#ifndef _SENSOR_sp0821_MIPI_RAW_H_
#define _SENSOR_sp0821_MIPI_RAW_H_


#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"


//#define FEATURE_OTP

#define VENDOR_NUM 1
#define SENSOR_NAME                "c3z01_ovsp0821_depthcxt"
#define I2C_SLAVE_ADDR            0x86    /* 8bit slave address*/

#define sp0821_PID_ADDR                0x01
#define sp0821_PID_VALUE            0x00
#define sp0821_VER_ADDR                0x02
#define sp0821_VER_VALUE            0x9C

/* sensor parameters begin */

/* effective sensor output image size */
#define PREVIEW_WIDTH            320
#define PREVIEW_HEIGHT            240
#define SNAPSHOT_WIDTH            320
#define SNAPSHOT_HEIGHT            240

/*Raw Trim parameters*/
#define PREVIEW_TRIM_X            0
#define PREVIEW_TRIM_Y            0
#define PREVIEW_TRIM_W            PREVIEW_WIDTH
#define PREVIEW_TRIM_H            PREVIEW_HEIGHT
#define SNAPSHOT_TRIM_X            0
#define SNAPSHOT_TRIM_Y            0
#define SNAPSHOT_TRIM_W            SNAPSHOT_WIDTH
#define SNAPSHOT_TRIM_H            SNAPSHOT_HEIGHT

/*Mipi output*/
#define LANE_NUM            1
#define RAW_BITS            8


#define PREVIEW_MIPI_PER_LANE_BPS      240  /* 2*Mipi clk */
#define SNAPSHOT_MIPI_PER_LANE_BPS      240  /* 2*Mipi clk */

/*line time unit: 1ns*/
#define PREVIEW_LINE_TIME          67200
#define SNAPSHOT_LINE_TIME          67200

/* frame length*/
#define PREVIEW_FRAME_LENGTH        499
#define SNAPSHOT_FRAME_LENGTH        499

/* please ref your spec */
#define FRAME_OFFSET            0
#define SENSOR_MAX_GAIN            0xf8
#define SENSOR_BASE_GAIN        0x28
#define SENSOR_MIN_SHUTTER        1

/* please ref your spec
 * 1 : average binning
 * 2 : sum-average binning
 * 4 : sum binning
 */
#define BINNING_FACTOR            1

/* please ref spec
 * 1: sensor auto caculate
 * 0: driver caculate
 */
/* sensor parameters end */

/* isp parameters, please don't change it*/
#define ISP_BASE_GAIN            0x80

/* please don't change it */
#define EX_MCLK                24

/*==============================================================================
 * Description:
 * register setting
 *============================================================================*/

static const SENSOR_REG_T sp0821_init_setting[] = {
    {0x30,0x01},
    {0x32,0x00},
    {0x03,0x00},
    {0x04,0x96},
    {0x24,0x13},
    {0x9b,0x32},
    {0xd7,0x00},
    {0xc5,0xc7},
    {0xc6,0xe2},
    {0xe7,0x03},
    {0x32,0x00},
    {0x32,0x01},
    {0x32,0x00},
    {0xbf,0x0f},
    {0xba,0x5a},
    {0xbb,0x69},
    {0xe7,0x00},
    {0x32,0x07},
    {0x31,0x03},
    {0x19,0x04},
    {0x2c,0x0f},
    {0x2e,0x3c},
    {0x30,0x01},
    {0x28,0x2e},
    {0x29,0x1f},
    {0x0f,0x30},
    {0x14,0xb0},
    {0x38,0x50},
    {0x39,0x52},
    {0x3a,0x60},
    {0x3b,0x10},
    {0x3c,0xe0},
    {0x85,0x01},
    {0xe0,0x02},
    {0xe5,0x60},
    {0xf5,0x02},
    {0xf1,0x03},
    {0xf3,0x40},
    {0x41,0x00},
    {0x05,0x00},
    {0x06,0x00},
    {0x07,0x00},
    {0x08,0x00},
    {0x09,0x00},
    {0x0a,0x34},
    {0x0D,0x01},
    {0xc8,0x10},
    {0x29,0x1e},
    {0xa2,0x4c},
    {0xa3,0x04},
    {0xa4,0x32},
    {0xa5,0x00},
    {0xa6,0xa0},
    {0xa8,0x32},
    {0xa9,0x00},
    {0xaa,0x01},
    {0xab,0x00},
    {0x4c,0x80},
    {0x4d,0x80},
    {0xa6,0xf0},
    {0xa7,0x20},
    {0xac,0xf0},
    {0xad,0x20},
    {0x8a,0x3e},
    {0x8b,0x30},
    {0x8c,0x2a},
    {0x8d,0x26},
    {0x8e,0x26},
    {0x8f,0x24},
    {0x90,0x24},
    {0x91,0x22},
    {0x92,0x22},
    {0x93,0x22},
    {0x94,0x20},
    {0x95,0x20},
    {0x96,0x20},
    {0x17,0x88},
    {0x18,0x80},
    {0x4e,0x78},
    {0x4f,0x78},
    {0x58,0x8a},
    {0x59,0xa8},
    {0x5a,0x80},
    {0xca,0x00},
    {0x86,0x08},
    {0x87,0x0f},
    {0x88,0x30},
    {0x89,0x45},
    {0x9e,0x94},
    {0x9f,0x88},
    {0x97,0x84},
    {0x98,0x88},
    {0x99,0x74},
    {0x9a,0x84},
    {0xa0,0x7c},
    {0xa1,0x78},
    {0x9d,0x09},
    {0xB1,0x04},
    {0xb3,0x00},
    {0x47,0x40},
    {0xb8,0x04},
    {0xb9,0x28},
    {0x3f,0x18},
    {0xc1,0xff},
    {0xc2,0x40},
    {0xc3,0xff},
    {0xc4,0x40},
    {0xc5,0xc7},
    {0xc6,0xe2},
    {0xc7,0xef},
    {0xc8,0x10},
    {0x50,0x2a},
    {0x51,0x2a},
    {0x52,0x2f},
    {0x53,0xcf},
    {0x54,0xd0},
    {0x5c,0x1e},
    {0x5d,0x21},
    {0x5e,0x1a},
    {0x5f,0xe9},
    {0x60,0x98},
    {0xcb,0x3f},
    {0xcc,0x3f},
    {0xcd,0x3f},
    {0xce,0x85},
    {0xcf,0xff},
    {0x79,0x5a},
    {0x7a,0xDC},
    {0x7b,0x0A},
    {0x7c,0xFD},
    {0x7d,0x46},
    {0x7e,0xFD},
    {0x7f,0xFD},
    {0x80,0xEF},
    {0x81,0x54},
    {0x1b,0x0a},
    {0x1c,0x0f},
    {0x1d,0x15},
    {0x1e,0x15},
    {0x1f,0x15},
    {0x20,0x1f},
    {0x21,0x2a},
    {0x22,0x2a},
    {0x56,0x49},
    {0x1a,0x14},
    {0x34,0x1f},
    {0x82,0x10},
    {0x83,0x00},
    {0x84,0xff},
    {0xd7,0x50},
    {0xd8,0x1a},
    {0xd9,0x20},
    {0xc9,0x1f},
    {0xbf,0x33},
    {0xba,0x37},
    {0xbb,0x38},
    {0x32,0x00},
    {0x03,0x00},
    {0x04,0x01},
    {0x24,0x13},
    {0xe7,0x03},
    {0x9d,0x40},
    {0x32,0x05},
    {0xa2,0x01},
    {0xa4,0x01},
    {0xa6,0x10},
    {0xa7,0x10},
    {0xa8,0x01},
    {0xaa,0x01},
    {0xac,0x10},
    {0xad,0x10}
};

static const SENSOR_REG_T sp0821_preview_setting[] = {

};

static const SENSOR_REG_T sp0821_snapshot_setting[] = {

};



static struct sensor_res_tab_info s_sp0821_resolution_tab_raw[VENDOR_NUM] = {
    {
      .module_id = MODULE_SUNNY,
      .reg_tab = {
        {ADDR_AND_LEN_OF_ARRAY(sp0821_init_setting), PNULL, 0,
        .width = 0, .height = 0,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},
        /*
        {ADDR_AND_LEN_OF_ARRAY(sp0821_preview_setting), PNULL, 0,
        .width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},*/

        {ADDR_AND_LEN_OF_ARRAY(sp0821_snapshot_setting), PNULL, 0,
        .width = SNAPSHOT_WIDTH, .height = SNAPSHOT_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW}
        }
    }

    /*If there are multiple modules,please add here*/
};

static SENSOR_TRIM_T s_sp0821_resolution_trim_tab[VENDOR_NUM] = {
    {
     .module_id = MODULE_SUNNY,
     .trim_info = {
       {0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
       /*
       {.trim_start_x = PREVIEW_TRIM_X, .trim_start_y = PREVIEW_TRIM_Y,
        .trim_width = PREVIEW_TRIM_W,   .trim_height = PREVIEW_TRIM_H,
        .line_time = PREVIEW_LINE_TIME, .bps_per_lane = PREVIEW_MIPI_PER_LANE_BPS,
        .frame_line = PREVIEW_FRAME_LENGTH,
        .scaler_trim = {.x = PREVIEW_TRIM_X, .y = PREVIEW_TRIM_Y, .w = PREVIEW_TRIM_W, .h = PREVIEW_TRIM_H}},*/

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

static SENSOR_REG_T sp0821_shutter_reg[] = {
};

static struct sensor_i2c_reg_tab sp0821_shutter_tab = {
    .settings = sp0821_shutter_reg,
    .size = ARRAY_SIZE(sp0821_shutter_reg),
};

static SENSOR_REG_T sp0821_again_reg[] = {
};

static struct sensor_i2c_reg_tab sp0821_again_tab = {
    .settings = sp0821_again_reg,
    .size = ARRAY_SIZE(sp0821_again_reg),
};

static SENSOR_REG_T sp0821_dgain_reg[] = {

};

static struct sensor_i2c_reg_tab sp0821_dgain_tab = {
    .settings = sp0821_dgain_reg,
    .size = ARRAY_SIZE(sp0821_dgain_reg),
};

static SENSOR_REG_T sp0821_frame_length_reg[] = {

};

static struct sensor_i2c_reg_tab sp0821_frame_length_tab = {
    .settings = sp0821_frame_length_reg,
    .size = ARRAY_SIZE(sp0821_frame_length_reg),
};

static struct sensor_aec_i2c_tag sp0821_aec_info = {
    .slave_addr = (I2C_SLAVE_ADDR >> 1),
    .addr_bits_type = SENSOR_I2C_REG_8BIT,
    .data_bits_type = SENSOR_I2C_VAL_8BIT,
    .shutter = &sp0821_shutter_tab,
    .again = &sp0821_again_tab,
    .dgain = &sp0821_dgain_tab,
    .frame_length = &sp0821_frame_length_tab,
};


static SENSOR_STATIC_INFO_T s_sp0821_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .static_info = {
        .f_num = 300,
        .focal_length = 139,
        .max_fps = 30,
        .max_adgain = 8,
        .ois_supported = 0,
        .pdaf_supported = 0,
        .exp_valid_frame_num = 1,
        .clamp_level = 64,
        .adgain_valid_frame_num = 1,
        .fov_info = {{4.614f, 3.444f}, 4.222f}}
    }
    /*If there are multiple modules,please add here*/
};


static SENSOR_MODE_FPS_INFO_T s_sp0821_mode_fps_info[VENDOR_NUM] = {
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


static struct sensor_module_info s_sp0821_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {
         .major_i2c_addr = I2C_SLAVE_ADDR >> 1,
         .minor_i2c_addr = I2C_SLAVE_ADDR >> 1,

         .reg_addr_value_bits = SENSOR_I2C_REG_8BIT | SENSOR_I2C_VAL_8BIT |
                                SENSOR_I2C_FREQ_100,

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

static struct sensor_ic_ops s_sp0821_ops_tab;
struct sensor_raw_info *s_sp0821_mipi_raw_info_ptr = PNULL;


/*==============================================================================
 * Description:
 * sensor all info
 * please modify this variable acording your spec
 *============================================================================*/
SENSOR_INFO_T g_sp0821_mipi_yuv_info = {
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
    .reset_pulse_width = 50,
    .power_down_level = SENSOR_LOW_LEVEL_PWDN,
    .identify_count = 1,
    .identify_code =
        {{ .reg_addr = sp0821_PID_ADDR, .reg_value = sp0821_PID_VALUE},
         { .reg_addr = sp0821_VER_ADDR, .reg_value = sp0821_VER_VALUE}},

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_YUV422,

    .module_info_tab = s_sp0821_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_sp0821_module_info_tab),

    .resolution_tab_info_ptr = s_sp0821_resolution_tab_raw,
    .sns_ops = &s_sp0821_ops_tab,
    .raw_info_ptr = &s_sp0821_mipi_raw_info_ptr,

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"sp0821_v1",
};

#endif