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

#ifndef _SENSOR_S5KHM6SX_MIPI_RAW_
#define _SENSOR_S5KHM6SX_MIPI_RAW_

#define PDAF_OFF
#ifdef PDAF_OFF
#include "sensor_s5khm6sx03_dphy_setting.h"
#else
#include "sensor_s5khm6sx03_dphy_setting_hulk.h"
#endif

#define BINNING_FACTOR 1.25
#define VENDOR_NUM 1
#define S5KHM6SX_I2C_ADDR_W 0x20
#define S5KHM6SX_I2C_ADDR_R 0x21

#define S5KHM6SX_PID_VALUE 0x1AD6
#define S5KHM6SX_PID_ADDR 0x0000
#define S5KHM6SX_VER_VALUE 0xb000
#define S5KHM6SX_VER_ADDR 0x0002

/* effective sensor output image size */
#define PREVIEW_WIDTH 4000
#define PREVIEW_HEIGHT 3000
#define CROP_PREVIEW_WIDTH 4000
#define CROP_PREVIEW_HEIGHT 3000
#define SNAPSHOT_WIDTH 11968
#define SNAPSHOT_HEIGHT 8944
#define SLOWVIDEO_WIDTH 1280
#define SLOWVIDEO_HEIGHT 720
#define VIDEO_WIDTH 3840
#define VIDEO_HEIGHT 2160

/*Raw Trim parameters*/
#define PREVIEW_TRIM_X 0
#define PREVIEW_TRIM_Y 0
#define PREVIEW_TRIM_W PREVIEW_WIDTH
#define PREVIEW_TRIM_H PREVIEW_HEIGHT
#define CROP_PREVIEW_TRIM_X 0
#define CROP_PREVIEW_TRIM_Y 0
#define CROP_PREVIEW_TRIM_W CROP_PREVIEW_WIDTH
#define CROP_PREVIEW_TRIM_H CROP_PREVIEW_HEIGHT
#define SNAPSHOT_TRIM_X 0
#define SNAPSHOT_TRIM_Y 0
#define SNAPSHOT_TRIM_W SNAPSHOT_WIDTH
#define SNAPSHOT_TRIM_H SNAPSHOT_HEIGHT
#define DUALCAMERA_PREVIEW_TRIM_X 0
#define DUALCAMERA_PREVIEW_TRIM_Y 0
#define DUALCAMERA_PREVIEW_TRIM_W PREVIEW_WIDTH
#define DUALCAMERA_PREVIEW_TRIM_H PREVIEW_HEIGHT
#define SLOWVIDEO_TRIM_X 0
#define SLOWVIDEO_TRIM_Y 0
#define SLOWVIDEO_TRIM_W SLOWVIDEO_WIDTH
#define SLOWVIDEO_TRIM_H SLOWVIDEO_HEIGHT
#define VIDEO_TRIM_X 0
#define VIDEO_TRIM_Y 0
#define VIDEO_TRIM_W VIDEO_WIDTH
#define VIDEO_TRIM_H VIDEO_HEIGHT

/*Mipi output*/
#define LANE_NUM 4
#define RAW_BITS 10

/* 2*Mipi clk */
#define PREVIEW_MIPI_PER_LANE_BPS 2340
#define CROP_PREVIEW_MIPI_PER_LANE_BPS 2340
#define SNAPSHOT_MIPI_PER_LANE_BPS 2340
#define DUALCAMERA_PREVIEW_MIPI_PER_LANE_BPS 1984
#define SLOWVIDEO_MIPI_PER_LANE_BPS 2692
#define VIDEO_MIPI_PER_LANE_BPS 2340

/*line time unit: 1ns*/
#define PREVIEW_LINE_TIME 6848
#define CROP_PREVIEW_LINE_TIME 10302
#define SNAPSHOT_LINE_TIME 13536
#define DUALCAMERA_PREVIEW_LINE_TIME 7468
#define SLOWVIDEO_LINE_TIME 5273
#define VIDEO_LINE_TIME 6868

/* frame length*/
#define PREVIEW_FRAME_LENGTH 4864
#define CROP_PREVIEW_FRAME_LENGTH 3234
#define SNAPSHOT_FRAME_LENGTH 10560
#define DUALCAMERA_PREVIEW_FRAME_LENGTH 5568
#define SLOWVIDEO_FRAME_LENGTH 1580
#define VIDEO_FRAME_LENGTH 4853

/* please ref your spec */

#define SENSOR_BASE_GAIN 0x20
#define ISP_BASE_GAIN 0x80
#define SENSOR_MIN_SHUTTER 6
#define FRAME_OFFSET 6

static struct sensor_res_tab_info
    s_s5khm6sx_resolution_Tab_RAW[VENDOR_NUM] =
        {
            {.module_id = MODULE_SUNNY,
             .reg_tab =
                 {
                     {ADDR_AND_LEN_OF_ARRAY(s5khm6sx_fast_switch_init), PNULL,
                      0, .width = 0, .height = 0, .xclk_to_sensor = 24,
                      .image_format = SENSOR_IMAGE_FORMAT_RAW,
                      .fast_switch_mode = SENSOR_FAST_SWITCH_MODE_NONE,
                      .binning_size_mode = SENSOR_BINNING_SIZE_MODE_NONE,
                      .is_cropped = 0},

                     {ADDR_AND_LEN_OF_ARRAY(
                          s5khm6sx_slowvideo_720P_120fps_setting),
                      PNULL, 0, .width = SLOWVIDEO_WIDTH,
                      .height = SLOWVIDEO_HEIGHT, .xclk_to_sensor = 24,
                      .image_format = SENSOR_IMAGE_FORMAT_RAW,
                      .is_cropped = 1,
                      .crop_start_x = 40,
                      .crop_start_y = 210,
                      .binning = 6},

                     {ADDR_AND_LEN_OF_ARRAY(s5khm6sx_video_4lane_setting),
                      PNULL, 0, .width = VIDEO_WIDTH, .height = VIDEO_HEIGHT,
                      .xclk_to_sensor = 24,
                      .image_format = SENSOR_IMAGE_FORMAT_RAW,
                      .is_cropped = 1,
                      .crop_start_x = 80,
                      .crop_start_y = 220,
                      .binning = 3},

                     {ADDR_AND_LEN_OF_ARRAY(s5khm6sx_binning_12M_30fps_setting),
                      PNULL, 0, .width = PREVIEW_WIDTH,
                      .height = PREVIEW_HEIGHT, .xclk_to_sensor = 24,
                      .image_format = SENSOR_IMAGE_FORMAT_RAW,
                      .fast_switch_mode =
                          SENSOR_FAST_SWITCH_GROUP1_MODE0_S5KHM6_12M_9IN1_BINNING,
                      .binning_size_mode = SENSOR_BINNING_SIZE_MODE_9IN1,
                      .is_cropped = 0,
                      .crop_start_x = 0,
                      .crop_start_y = 0,
                      .binning = 3},

                     {ADDR_AND_LEN_OF_ARRAY(s5khm6sx_crop_12M_30fps_setting),
                      PNULL, 0, .width = CROP_PREVIEW_WIDTH,
                      .height = CROP_PREVIEW_HEIGHT, .xclk_to_sensor = 24,
                      .image_format = SENSOR_IMAGE_FORMAT_RAW,
                      .fast_switch_mode =
                          SENSOR_FAST_SWITCH_GROUP1_MODE1_S5KHM6_12M_CROP_FROM_108M,
                      .binning_size_mode = SENSOR_BINNING_SIZE_MODE_FULLSIZE,
                      .is_cropped = 1,
                      .crop_start_x = 4000,
                      .crop_start_y = 3000,
                      .binning = 1},

                     {ADDR_AND_LEN_OF_ARRAY(s5khm6sx_snapshot_fullSize_setting),
                      PNULL, 0, .width = SNAPSHOT_WIDTH,
                      .height = SNAPSHOT_HEIGHT, .xclk_to_sensor = 24,
                      .image_format = SENSOR_IMAGE_FORMAT_RAW,
                      .fast_switch_mode = SENSOR_FAST_SWITCH_MODE_NONE,
                      .binning_size_mode = SENSOR_BINNING_SIZE_MODE_FULLSIZE,
                      .is_cropped = 1,
                      .crop_start_x = 16,
                      .crop_start_y = 28,
                      .binning = 1},

                     /*{ADDR_AND_LEN_OF_ARRAY(s5khm6sx_dualcamera_12M_24fps_setting),
                      PNULL,
                      0, .width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT,
                      .xclk_to_sensor = 24, .image_format =
                      SENSOR_IMAGE_FORMAT_RAW}*/
                 }}
            /*If there are multiple modules,please add here*/
};

static SENSOR_TRIM_T
    s_s5khm6sx_Resolution_Trim_Tab[VENDOR_NUM] =
        {
            {.module_id = MODULE_SUNNY,
             .trim_info =
                 {
                     {0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},

                     {.trim_start_x = SLOWVIDEO_TRIM_X,
                      .trim_start_y = SLOWVIDEO_TRIM_Y,
                      .trim_width = SLOWVIDEO_TRIM_W,
                      .trim_height = SLOWVIDEO_TRIM_H,
                      .line_time = SLOWVIDEO_LINE_TIME,
                      .bps_per_lane = SLOWVIDEO_MIPI_PER_LANE_BPS,
                      .frame_line = SLOWVIDEO_FRAME_LENGTH,
                      .scaler_trim = {.x = SLOWVIDEO_TRIM_X,
                                      .y = SLOWVIDEO_TRIM_Y,
                                      .w = SLOWVIDEO_TRIM_W,
                                      .h = SLOWVIDEO_TRIM_H}},

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

                     {.trim_start_x = CROP_PREVIEW_TRIM_X,
                      .trim_start_y = CROP_PREVIEW_TRIM_Y,
                      .trim_width = CROP_PREVIEW_TRIM_W,
                      .trim_height = CROP_PREVIEW_TRIM_H,
                      .line_time = CROP_PREVIEW_LINE_TIME,
                      .bps_per_lane = CROP_PREVIEW_MIPI_PER_LANE_BPS,
                      .frame_line = CROP_PREVIEW_FRAME_LENGTH,
                      .scaler_trim = {.x = CROP_PREVIEW_TRIM_X,
                                      .y = CROP_PREVIEW_TRIM_Y,
                                      .w = CROP_PREVIEW_TRIM_W,
                                      .h = CROP_PREVIEW_TRIM_H}},

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

                     /*{.trim_start_x = DUALCAMERA_PREVIEW_TRIM_X,
                      .trim_start_y = DUALCAMERA_PREVIEW_TRIM_Y,
                      .trim_width = DUALCAMERA_PREVIEW_TRIM_W,
                      .trim_height = DUALCAMERA_PREVIEW_TRIM_H,
                      .line_time = DUALCAMERA_PREVIEW_LINE_TIME,
                      .bps_per_lane = DUALCAMERA_PREVIEW_MIPI_PER_LANE_BPS,
                      .frame_line = DUALCAMERA_PREVIEW_FRAME_LENGTH,
                      .scaler_trim = {.x = DUALCAMERA_PREVIEW_TRIM_X,
                                      .y = DUALCAMERA_PREVIEW_TRIM_Y,
                                      .w = DUALCAMERA_PREVIEW_TRIM_W,
                                      .h = DUALCAMERA_PREVIEW_TRIM_H}},*/

                 }}

            /*If there are multiple modules,please add here*/
};

static struct sensor_reg_tag s5khm6sx_shutter_reg[] = {
    {0x0202, 0},
};

static struct sensor_i2c_reg_tab s5khm6sx_shutter_tab = {
    .settings = s5khm6sx_shutter_reg, .size = ARRAY_SIZE(s5khm6sx_shutter_reg),
};

static struct sensor_reg_tag s5khm6sx_again_reg[] = {
    {0x0204, 0},
};

static struct sensor_i2c_reg_tab s5khm6sx_again_tab = {
    .settings = s5khm6sx_again_reg, .size = ARRAY_SIZE(s5khm6sx_again_reg),
};

static struct sensor_reg_tag s5khm6sx_dgain_reg[] = {
    {0x020e, 0}, {0x0210, 0}, {0x0212, 0}, {0x0214, 0},
};

struct sensor_i2c_reg_tab s5khm6sx_dgain_tab = {
    .settings = s5khm6sx_dgain_reg, .size = ARRAY_SIZE(s5khm6sx_dgain_reg),
};

static struct sensor_reg_tag s5khm6sx_frame_length_reg[] = {
    {0x0340, 0},
};

static struct sensor_i2c_reg_tab s5khm6sx_frame_length_tab = {
    .settings = s5khm6sx_frame_length_reg,
    .size = ARRAY_SIZE(s5khm6sx_frame_length_reg),
};

static SENSOR_REG_T s5khm6sx_grp_hold_start_reg[] = {
    {0x0104, 0x01},
};

static struct sensor_i2c_reg_tab s5khm6sx_grp_hold_start_tab = {
    .settings = s5khm6sx_grp_hold_start_reg,
    .size = ARRAY_SIZE(s5khm6sx_grp_hold_start_reg),
};

static SENSOR_REG_T s5khm6sx_grp_hold_end_reg[] = {
    {0x0104, 0x00},
};

static struct sensor_i2c_reg_tab s5khm6sx_grp_hold_end_tab = {
    .settings = s5khm6sx_grp_hold_end_reg,
    .size = ARRAY_SIZE(s5khm6sx_grp_hold_end_reg),
};

static struct sensor_aec_i2c_tag s5khm6sx_aec_info = {
    .slave_addr = (S5KHM6SX_I2C_ADDR_W >> 1),
    .addr_bits_type = SENSOR_I2C_REG_16BIT,
    .data_bits_type = SENSOR_I2C_VAL_16BIT,
    .shutter = &s5khm6sx_shutter_tab,
    .again = &s5khm6sx_again_tab,
    .dgain = &s5khm6sx_dgain_tab,
    .frame_length = &s5khm6sx_frame_length_tab,
    .grp_hold_start = &s5khm6sx_grp_hold_start_tab,
    .grp_hold_end = &s5khm6sx_grp_hold_end_tab,
};

static const SENSOR_REG_T s_s5khm6sx_video_tab[SENSOR_VIDEO_MODE_MAX][1] = {
    /*video mode 0: ?fps*/
    {{0xffff, 0xff}},
    /* video mode 1:?fps*/
    {{0xffff, 0xff}},
    /* video mode 2:?fps*/
    {{0xffff, 0xff}},
    /* video mode 3:?fps*/
    {{0xffff, 0xff}}};

static SENSOR_VIDEO_INFO_T s_s5khm6sx_video_info[] = {
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{30, 30, 164, 100}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     (SENSOR_REG_T **)s_s5khm6sx_video_tab},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL}};

static SENSOR_STATIC_INFO_T s_s5khm6sx_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .static_info = {.f_num = 175,
                     .focal_length = 524,
                     .max_fps = 30,
                     .max_adgain = 16,
                     .ois_supported = 0,
#ifdef PDAF_OFF
                     .pdaf_supported = 0,
#else
                     .pdaf_supported =
                         SENSOR_DUALPD_MODE3_ENABLE, // SENSOR_DUAL_PDAF_ENABLE,
#endif
                     .exp_valid_frame_num = 1,
                     .clamp_level = 64,
                     .adgain_valid_frame_num = 1,
                     .fov_angle = 78.6,
                     .fov_info = {{4.032f, 3.024f}, 5.24f}}}
    /*If there are multiple modules,please add here*/
};

static SENSOR_MODE_FPS_INFO_T s_s5khm6sx_mode_fps_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     {.is_init = 0,
      /*1:mode,2:max_fps,3:min_fps,4:is_high_fps,5:high_fps_skip_num*/
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

static struct sensor_module_info s_s5khm6sx_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {.major_i2c_addr = S5KHM6SX_I2C_ADDR_W >> 1,
                     .minor_i2c_addr = 0x20 >> 1,
                     .i2c_burst_mode = 3,
                     .reg_addr_value_bits = SENSOR_I2C_REG_16BIT |
                                            SENSOR_I2C_VAL_16BIT |
                                            SENSOR_I2C_FREQ_1000,

                     .avdd_val = SENSOR_AVDD_2200MV,
                     .iovdd_val = SENSOR_AVDD_1800MV,
                     .dvdd_val = SENSOR_AVDD_1000MV,

                     .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_GR,

                     .preview_skip_num = 0,
                     .capture_skip_num = 0,
                     .flash_capture_skip_num = 6,
                     .mipi_cap_skip_num = 0,
                     .preview_deci_num = 0,
                     .video_preview_deci_num = 0,

                     .sensor_interface =
                         {
                             .type = SENSOR_INTERFACE_TYPE_CSI2,
                             .bus_width = 4,
                             .pixel_width = 10,
#if defined _SENSOR_RAW_SHARKL5PRO_H_ || defined _SENSOR_RAW_SHARKL6_H_
                             .is_loose = 2,
#else
                             .is_loose = 0,
#endif
                         },

                     .change_setting_skip_num = 3,
                     .horizontal_view_angle = 65,
                     .vertical_view_angle = 60}}};

static struct sensor_ic_ops s5khm6sx_ops_tab;
static struct sensor_raw_info *s_s5khm6sx_mipi_raw_info_ptr = PNULL;
//    &s_s5khm6sx_mipi_raw_info;

SENSOR_INFO_T g_s5khm6sx_mipi_raw_info = {
    .hw_signal_polarity = SENSOR_HW_SIGNAL_PCLK_N | SENSOR_HW_SIGNAL_VSYNC_N |
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
    .reset_pulse_width = 5,
    .power_down_level = SENSOR_LOW_LEVEL_PWDN,

    .identify_count = 1,
    .identify_code = {{.reg_addr = S5KHM6SX_PID_ADDR,
                       .reg_value = S5KHM6SX_PID_VALUE},
                      {.reg_addr = S5KHM6SX_VER_ADDR,
                       .reg_value = S5KHM6SX_VER_VALUE}},

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .true_fullsize_width = 12000,
    .true_fullsize_height = 9000,
    .fast_switch_zoom_ratio = 3,
    .name = (cmr_s8 *)"s5khm6sx03_qt",

    .image_format = SENSOR_IMAGE_FORMAT_RAW,

    .resolution_tab_info_ptr = s_s5khm6sx_resolution_Tab_RAW,
    .sns_ops = &s5khm6sx_ops_tab,
    .raw_info_ptr = &s_s5khm6sx_mipi_raw_info_ptr,
    .module_info_tab = s_s5khm6sx_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_s5khm6sx_module_info_tab),
    .ext_info_ptr = NULL,

    .video_tab_info_ptr = s_s5khm6sx_video_info,
    .sensor_version_info = (cmr_s8 *)"s5khm6sx03_qt",
};
#endif
