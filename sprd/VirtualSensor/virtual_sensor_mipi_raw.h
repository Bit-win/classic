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

#ifndef _SENSOR_virtual_sensor_MIPI_RAW_H_
#define _SENSOR_virtual_sensor_MIPI_RAW_H_

#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"
#if defined(CONFIG_CAMERA_ISP_VERSION_V3) ||                                   \
    defined(CONFIG_CAMERA_ISP_VERSION_V4)
#include "parameters/sensor_virtual_sensor_raw_param_main.c"
#else
#endif

#define virtual_sensor_I2C_ADDR_W (0xee >> 1)
#define virtual_sensor_I2C_ADDR_R (0xee >> 1)
#define SENSOR_NAME "virtual_sensor_mipi_raw"
#define VENDOR_NUM 1
#define SENSOR_NAME "virtual_sensor_mipi_raw"
#define I2C_SLAVE_ADDR 0x34 /* 8bit slave address*/

#define virtual_sensor_PID_ADDR 0x0016
#define virtual_sensor_PID_VALUE 0x05
#define virtual_sensor_VER_ADDR 0x0017
#define virtual_sensor_VER_VALUE 0x86

/* sensor parameters begin */

/* effective sensor output image size */
#define VIDEO_WIDTH 1920
#define VIDEO_HEIGHT 1080
#define PREVIEW_WIDTH 4000
#define PREVIEW_HEIGHT 3000
#define SNAPSHOT_WIDTH 8000
#define SNAPSHOT_HEIGHT 6000

/*Raw Trim parameters*/
#define VIDEO_TRIM_X 0
#define VIDEO_TRIM_Y 0
#define VIDEO_TRIM_W VIDEO_WIDTH
#define VIDEO_TRIM_H VIDEO_HEIGHT
#define PREVIEW_TRIM_X 0
#define PREVIEW_TRIM_Y 0
#define PREVIEW_TRIM_W PREVIEW_WIDTH
#define PREVIEW_TRIM_H PREVIEW_HEIGHT
#define SNAPSHOT_TRIM_X 0
#define SNAPSHOT_TRIM_Y 0
#define SNAPSHOT_TRIM_W SNAPSHOT_WIDTH
#define SNAPSHOT_TRIM_H SNAPSHOT_HEIGHT

/*Mipi output*/
#define LANE_NUM 4
#define RAW_BITS 10

#define VIDEO_MIPI_PER_LANE_BPS 726    /* 2*Mipi clk */
#define PREVIEW_MIPI_PER_LANE_BPS 1148  /* 2*Mipi clk */
#define SNAPSHOT_MIPI_PER_LANE_BPS 940 /* 2*Mipi clk */

/*line time unit: 1ns*/
#define VIDEO_LINE_TIME 7297
#define PREVIEW_LINE_TIME 10893
#define SNAPSHOT_LINE_TIME 16480

/* frame length*/
#define VIDEO_FRAME_LENGTH 1142
#define PREVIEW_FRAME_LENGTH 3064
#define SNAPSHOT_FRAME_LENGTH 6068

/* please ref your spec */
#define FRAME_OFFSET 5

#define SENSOR_MAX_GAIN                0x200
#define SENSOR_BASE_GAIN               0x20
#define SENSOR_MIN_SHUTTER 6

/* please ref your spec
 * 1 : average binning
 * 2 : sum-average binning
 * 4 : sum binning
 */
#define BINNING_FACTOR 1

/* please ref spec
 * 1: sensor auto caculate
 * 0: driver caculate
 */
/* sensor parameters end */

/* isp parameters, please don't change it*/
#define ISP_BASE_GAIN 0x80

/* please don't change it */
#define EX_MCLK 24
LOCAL const SENSOR_REG_T virtual_sensor_init_setting[] = {};

LOCAL const SENSOR_REG_T virtual_sensor_video_setting[] = {};

LOCAL const SENSOR_REG_T virtual_sensor_preview_setting[] = {};

LOCAL const SENSOR_REG_T virtual_sensor_snapshot_setting[] = {};

struct sensor_res_tab_info s_virtual_sensor_resolution_tab_raw[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .reg_tab =
         {{ADDR_AND_LEN_OF_ARRAY(virtual_sensor_init_setting), PNULL, 0, .width = 0,
           .height = 0, .xclk_to_sensor = EX_MCLK,
           .image_format = SENSOR_IMAGE_FORMAT_RAW},

          {ADDR_AND_LEN_OF_ARRAY(virtual_sensor_video_setting), PNULL, 0,
           .width = VIDEO_WIDTH, .height = VIDEO_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

          {ADDR_AND_LEN_OF_ARRAY(virtual_sensor_preview_setting), PNULL, 0,
           .width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

          {ADDR_AND_LEN_OF_ARRAY(virtual_sensor_snapshot_setting), PNULL, 0,
           .width = SNAPSHOT_WIDTH, .height = SNAPSHOT_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW}}}

    /*If there are multiple modules,please add here*/
};

SENSOR_TRIM_T s_virtual_sensor_resolution_trim_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .trim_info =
         {
             {0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},

             {.trim_start_x = VIDEO_TRIM_X,
              .trim_start_y = VIDEO_TRIM_Y,
              .trim_width = VIDEO_TRIM_W,
              .trim_height = VIDEO_TRIM_H,
              .line_time = VIDEO_LINE_TIME,
              .bps_per_lane = VIDEO_MIPI_PER_LANE_BPS,
              .frame_line = VIDEO_FRAME_LENGTH,
              .scaler_trim = {.x = VIDEO_TRIM_X,
                              .y = VIDEO_TRIM_Y,
                              .w = VIDEO_TRIM_W,
                              .h = VIDEO_TRIM_H}},

             {.trim_start_x = PREVIEW_TRIM_X,
              .trim_start_y = PREVIEW_TRIM_Y,
              .trim_width = PREVIEW_TRIM_W,
              .trim_height = PREVIEW_TRIM_H,
              .line_time = PREVIEW_LINE_TIME,
              .bps_per_lane = PREVIEW_MIPI_PER_LANE_BPS,
              .frame_line = PREVIEW_FRAME_LENGTH,
              .scaler_trim = {.x = PREVIEW_TRIM_X,
                              .y = PREVIEW_TRIM_Y,
                              .w = PREVIEW_TRIM_W,
                              .h = PREVIEW_TRIM_H}},

             {.trim_start_x = SNAPSHOT_TRIM_X,
              .trim_start_y = SNAPSHOT_TRIM_Y,
              .trim_width = SNAPSHOT_TRIM_W,
              .trim_height = SNAPSHOT_TRIM_H,
              .line_time = SNAPSHOT_LINE_TIME,
              .bps_per_lane = SNAPSHOT_MIPI_PER_LANE_BPS,
              .frame_line = SNAPSHOT_FRAME_LENGTH,
              .scaler_trim = {.x = SNAPSHOT_TRIM_X,
                              .y = SNAPSHOT_TRIM_Y,
                              .w = SNAPSHOT_TRIM_W,
                              .h = SNAPSHOT_TRIM_H}},
         }}

    /*If there are multiple modules,please add here*/

};

static SENSOR_REG_T virtual_sensor_shutter_reg[] = {
    {0x0202, 0x0000}, {0x0203, 0x0000}, {0x3100, 0x0000},
};

static struct sensor_i2c_reg_tab virtual_sensor_shutter_tab = {
    .settings = virtual_sensor_shutter_reg, .size = ARRAY_SIZE(virtual_sensor_shutter_reg),
};

static SENSOR_REG_T virtual_sensor_again_reg[] = {
    {0x0104, 0x0001}, {0x0204, 0x0000}, {0x0205, 0x0000}, {0x0104, 0x0000},
};

static struct sensor_i2c_reg_tab virtual_sensor_again_tab = {
    .settings = virtual_sensor_again_reg, .size = ARRAY_SIZE(virtual_sensor_again_reg),
};

static SENSOR_REG_T virtual_sensor_dgain_reg[] = {

};

static struct sensor_i2c_reg_tab virtual_sensor_dgain_tab = {
    .settings = virtual_sensor_dgain_reg, .size = ARRAY_SIZE(virtual_sensor_dgain_reg),
};

static SENSOR_REG_T virtual_sensor_frame_length_reg[] = {
    {0x0340, 0x0000}, {0x0341, 0x0000},
};

static struct sensor_i2c_reg_tab virtual_sensor_frame_length_tab = {
    .settings = virtual_sensor_frame_length_reg,
    .size = ARRAY_SIZE(virtual_sensor_frame_length_reg),
};

static struct sensor_aec_i2c_tag virtual_sensor_aec_info = {
    .slave_addr = (I2C_SLAVE_ADDR >> 1),
    .addr_bits_type = SENSOR_I2C_REG_16BIT,
    .data_bits_type = SENSOR_I2C_VAL_8BIT,
    .shutter = &virtual_sensor_shutter_tab,
    .again = &virtual_sensor_again_tab,
    .dgain = &virtual_sensor_dgain_tab,
    .frame_length = &virtual_sensor_frame_length_tab,
};
/*
static const cmr_u16 virtual_sensor_pd_is_right[] = {
    0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
    0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
};

static const cmr_u16 virtual_sensor_pd_row[] = {
    7,  7,  23, 23, 43, 43, 59, 59, 11, 11, 27, 27, 39, 39, 55, 55,
    11, 11, 27, 27, 39, 39, 55, 55, 7,  7,  23, 23, 43, 43, 59, 59};

static const cmr_u16 virtual_sensor_pd_col[] = {
    0,  4,  4,  8,  4,  8,  0,  4,  20, 16, 24, 20, 24, 20, 20, 16,
    36, 40, 32, 36, 32, 36, 36, 40, 56, 52, 52, 48, 52, 48, 56, 52};

 */
SENSOR_STATIC_INFO_T s_virtual_sensor_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .static_info = {.f_num = 200,
                     .focal_length = 354,
                     .min_focal_distance = 100,
                     .start_offset_time = -5500000,
                     .max_fps = 30,
                     .max_adgain = 8,
                     .ois_supported = 0,
                     .pdaf_supported = 0,
                     .long_expose_supported = 1,
                     .zsl_long_expose_supported = 1,
                     .exp_valid_frame_num = 1,
                     .clamp_level = 64,
                     .adgain_valid_frame_num = 0,
                     .fov_angle = 78.6,
                     .fov_info = {{4.614f, 3.444f}, 3.546f}}}
    /*If there are multiple modules,please add here*/
};

SENSOR_MODE_FPS_INFO_T s_virtual_sensor_mode_fps_info[VENDOR_NUM] = {
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
       {SENSOR_MODE_SNAPSHOT_TWO_THIRD, 0, 1, 0, 0}}}}
    /*If there are multiple modules,please add here*/
};

struct sensor_module_info s_virtual_sensor_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {.major_i2c_addr = 0xee >> 1,
                     .minor_i2c_addr = 0xee >> 1,

                     .i2c_burst_mode = SENSOR_I2C_BURST_REG16_VAL8,

                     .reg_addr_value_bits = SENSOR_I2C_REG_16BIT |
                                            SENSOR_I2C_VAL_8BIT |
                                            SENSOR_I2C_FREQ_400,

                     .avdd_val = SENSOR_AVDD_2800MV,
                     .iovdd_val = SENSOR_AVDD_1800MV,
                     .dvdd_val = SENSOR_AVDD_1200MV,

                     .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_R,

                     .preview_skip_num = 1,
                     .capture_skip_num = 2,
                     .flash_capture_skip_num = 6,
                     .mipi_cap_skip_num = 0,
                     .preview_deci_num = 0,
                     .video_preview_deci_num = 0,

                     .threshold_eb = 0,
                     .threshold_mode = 0,
                     .threshold_start = 0,
                     .threshold_end = 0,

                     .sensor_interface =
                         {
                             .type = SENSOR_INTERFACE_TYPE_CSI2,
                             .bus_width = LANE_NUM,
                             .pixel_width = RAW_BITS,
#if defined  _SENSOR_RAW_SHARKL5PRO_H_ || defined _SENSOR_RAW_SHARKL6_H_
                             .is_loose = 2,
#else
                             .is_loose = 0,
#endif


                         },
                     .change_setting_skip_num = 1,
                     .horizontal_view_angle = 65,
                     .vertical_view_angle = 60}}

    /*If there are multiple modules,please add here*/
};

static struct sensor_ic_ops s_virtual_sensor_ops_tab;
struct sensor_raw_info *s_virtual_sensor_mipi_raw_info_ptr =
    PNULL; //&s_virtual_sensor_mipi_raw_info;

/*==============================================================================
 * Description:
 * sensor all info
 * please modify this variable acording your spec
 *============================================================================*/
SENSOR_INFO_T g_virtual_sensor_mipi_raw_info = {
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
    .identify_code = {{0x0A, 0x56}, {0x0B, 0x40}},

    .source_width_max = PREVIEW_WIDTH,
    .source_height_max = PREVIEW_HEIGHT,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_RAW,

    .module_info_tab = s_virtual_sensor_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_virtual_sensor_module_info_tab),

    .resolution_tab_info_ptr = s_virtual_sensor_resolution_tab_raw,
    .sns_ops = &s_virtual_sensor_ops_tab,
    .raw_info_ptr = &s_virtual_sensor_mipi_raw_info_ptr,

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"virtual_sensor_v1",
};
#endif

