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
#ifndef _SENSOR_OV9282_dual_MIPI_RAW_H_
#define _SENSOR_OV9282_dual_MIPI_RAW_H_

#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

#include "parameters_sharkl3/sensor_ov9282_dual_raw_param_main.c"

#define SENSOR_NAME "ov9282_dual_mipi_raw"
#define I2C_SLAVE_ADDR 0x66

#define VENDOR_NUM 1
#define ov9282_dual_CHIP_ID_H_ADDR 0x300A
#define ov9282_dual_CHIP_ID_H_VALUE 0x92
#define ov9282_dual_CHIP_ID_L_ADDR 0x300B
#define ov9282_dual_CHIP_ID_L_VALUE 0x81

/* sensor parameters begin */
/* effective sensor output image size */
#define SNAPSHOT_WIDTH 1280
#define SNAPSHOT_HEIGHT 800
#define FRAME_LINE 910
/* frame length*/
#define PREVIEW_FRAME_LENGTH 910

/* Mipi output */
#define LANE_NUM 2
#define RAW_BITS 10

/* please ref your spec */
#define FRAME_OFFSET 25
#define SENSOR_BASE_GAIN 0x10
#define ISP_BASE_GAIN			0x80
#define SENSOR_MIN_SHUTTER		1

/* please don't change it */
#define EX_MCLK 24

static struct sensor_module_info s_ov9282_dual_module_info_tab[VENDOR_NUM] = {
    {
        .module_id = MODULE_SUNNY,
        .module_info = {
            .major_i2c_addr = 0x20 >> 1,//0xe0 >> 1, //0xc0---main2
            .minor_i2c_addr = 0x20 >> 1,//I2C_SLAVE_ADDR >> 1,

            .reg_addr_value_bits = SENSOR_I2C_REG_16BIT |
                                   SENSOR_I2C_VAL_8BIT |
                                   SENSOR_I2C_FREQ_100,

            .avdd_val = SENSOR_AVDD_2800MV,
            .iovdd_val = SENSOR_AVDD_1800MV,
            .dvdd_val = SENSOR_AVDD_1200MV,

            .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_B,

            .preview_skip_num = 1,// TODO:
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
            .change_setting_skip_num = 1,// TODO:
            .horizontal_view_angle = 35,
            .vertical_view_angle = 35,
        },
    },
/*If there are multiple modules,please add here*/
};

/* 640x480 RAW10 100fps */
static const SENSOR_REG_T ov9282_dual_init_setting[] = {
{0x0103, 0x01},
{0x0302, 0x32},
{0x030d, 0x50},
{0x030e, 0x02},
{0x3001, 0x00},
{0x3004, 0x00},
{0x3005, 0x00},
{0x3006, 0x04},
{0x3011, 0x0a},
{0x3013, 0x18},
{0x301c, 0xf0},
{0x3022, 0x01},
{0x3030, 0x10},
{0x3039, 0x32},
{0x303a, 0x00},
{0x3500, 0x00},
{0x3501, 0x2a},
{0x3502, 0x90},
{0x3503, 0x08},
{0x3505, 0x8c},
{0x3507, 0x03},
{0x3508, 0x00},
{0x3509, 0x10},
{0x3610, 0x80},
{0x3611, 0xa0},
{0x3620, 0x6e},
{0x3632, 0x56},
{0x3633, 0x78},
{0x3662, 0x05},
{0x3666, 0x00},
{0x366f, 0x5a},
{0x3680, 0x84},
{0x3712, 0x80},
{0x372d, 0x22},
{0x3731, 0x80},
{0x3732, 0x30},
{0x3778, 0x00},
{0x377d, 0x22},
{0x3788, 0x02},
{0x3789, 0xa4},
{0x378a, 0x00},
{0x378b, 0x4a},
{0x3799, 0x20},
{0x3800, 0x00},
{0x3801, 0x00},
{0x3802, 0x00},
{0x3803, 0x00},
{0x3804, 0x05},
{0x3805, 0x0f},
{0x3806, 0x03},
{0x3807, 0x2f},
{0x3808, 0x05},
{0x3809, 0x00},
{0x380a, 0x03},
{0x380b, 0x20},
{0x380c, 0x02},
{0x380d, 0xd8},
{0x380e, 0x03},
{0x380f, 0x8e},
{0x3810, 0x00},
{0x3811, 0x08},
{0x3812, 0x00},
{0x3813, 0x08},
{0x3814, 0x11},
{0x3815, 0x11},
{0x3820, 0x40},
{0x3821, 0x00},
{0x382c, 0x05},
{0x382d, 0xb0},
{0x389d, 0x00},
{0x3881, 0x42},
{0x3882, 0x01},
{0x3883, 0x00},
{0x3885, 0x02},
{0x38a8, 0x02},
{0x38a9, 0x80},
{0x38b1, 0x00},
{0x38b3, 0x02},
{0x38c4, 0x00},
{0x38c5, 0xc0},
{0x38c6, 0x04},
{0x38c7, 0x80},
{0x3920, 0xff},
{0x4003, 0x40},
{0x4008, 0x04},
{0x4009, 0x0b},
{0x400c, 0x00},
{0x400d, 0x07},
{0x4010, 0x40},
{0x4043, 0x40},
{0x4307, 0x30},
{0x4317, 0x00},
{0x4501, 0x00},
{0x4507, 0x00},
{0x4509, 0x00},
{0x450a, 0x08},
{0x4601, 0x04},
{0x470f, 0x00},
{0x4f07, 0x00},
{0x4800, 0x00},
{0x4837, 0x14},
{0x5000, 0x9f},
{0x5001, 0x00},
{0x5e00, 0x00},
{0x5d00, 0x07},
{0x5d01, 0x00},
{0x4f00, 0x04},
{0x4f10, 0x00},
{0x4f11, 0x98},
{0x4f12, 0x0f},
{0x4f13, 0xc4},

};

static const SENSOR_REG_T ov9282_dual_640x480_raw10_60fps_setting[] = {
/*Full size 1280x800 setting 	
60fps,line time 4.55us, 800Mbps/lane	
register	value*/
{0x0103, 0x01},
{0x0302, 0x32},
{0x030d, 0x50},
{0x030e, 0x02},
{0x3501, 0x2a},
{0x3502, 0x90},
{0x3503, 0x08},
{0x3508, 0x00},
{0x3509, 0x10},
{0x3620, 0x6e},
{0x3662, 0x05},
{0x3712, 0x80},
{0x3800, 0x00},
{0x3801, 0x00},
{0x3802, 0x00},
{0x3803, 0x00},
{0x3804, 0x05},
{0x3805, 0x0f},
{0x3806, 0x03},
{0x3807, 0x2f},
{0x3808, 0x05},
{0x3809, 0x00},
{0x380a, 0x03},
{0x380b, 0x20},
{0x380c, 0x05},
{0x380d, 0xb0},
{0x380e, 0x07},//3},
{0x380f, 0x1c},//8e},
{0x3811, 0x08},
{0x3813, 0x08},
{0x3814, 0x11},
{0x3815, 0x11},
{0x3820, 0x40},
{0x3821, 0x00},
{0x4003, 0x40},
{0x4008, 0x04},
{0x4009, 0x0b},
{0x400c, 0x00},
{0x400d, 0x07},
{0x4501, 0x00},
{0x4507, 0x00},
{0x4509, 0x00},
{0x4800, 0x00},
{0x4837, 0x14},
{0x5000, 0x9f},
{0x5001, 0x00},
{0x5e00, 0x00},
{0x5d00, 0x07},
{0x5d01, 0x00},
};


static SENSOR_STATIC_INFO_T s_ov9282_dual_static_info[VENDOR_NUM] = {
    {
        .module_id = MODULE_SUNNY,
        .static_info = {
            .f_num = 200,
            .focal_length = 354,
            .max_fps = 0,
            .max_adgain = 15 * 2,
            .ois_supported = 0,
            .pdaf_supported = 0,
            .exp_valid_frame_num = 1,
            .clamp_level = 64,
            .adgain_valid_frame_num = 1,
            .fov_info = {{4.614f, 3.444f}, 4.222f},
        },
    },
    /*If there are multiple modules,please add here*/
};

static SENSOR_MODE_FPS_INFO_T s_ov9282_dual_mode_fps_info[VENDOR_NUM] = {
    {
        .module_id = MODULE_SUNNY,
        .fps_info = {
            .is_init = 0,
            .sensor_mode_fps = {
                {SENSOR_MODE_COMMON_INIT, 0, 1, 0, 0},
                {SENSOR_MODE_PREVIEW_ONE, 0, 1, 0, 0},
                {SENSOR_MODE_SNAPSHOT_ONE_FIRST, 0, 1, 0, 0},
                {SENSOR_MODE_SNAPSHOT_ONE_SECOND, 0, 1, 0, 0},
                {SENSOR_MODE_SNAPSHOT_ONE_THIRD, 0, 1, 0, 0},
                {SENSOR_MODE_PREVIEW_TWO, 0, 1, 0, 0},
                {SENSOR_MODE_SNAPSHOT_TWO_FIRST, 0, 1, 0, 0},
                {SENSOR_MODE_SNAPSHOT_TWO_SECOND, 0, 1, 0, 0},
                {SENSOR_MODE_SNAPSHOT_TWO_THIRD, 0, 1, 0, 0},
            },
        },
    },
    /*If there are multiple modules,please add here*/
};

static struct sensor_res_tab_info s_ov9282_dual_resolution_tab_raw[VENDOR_NUM] = {
    {
        .module_id = MODULE_SUNNY,
        .reg_tab = {
            {
                ADDR_AND_LEN_OF_ARRAY(ov9282_dual_init_setting),
                PNULL,
                0,
                .width = 0,
                .height = 0,
                .xclk_to_sensor = EX_MCLK,
                .image_format = SENSOR_IMAGE_FORMAT_RAW,
            },
            {
                ADDR_AND_LEN_OF_ARRAY(ov9282_dual_640x480_raw10_60fps_setting),
                PNULL,
                0,
                .width = 1280,
                .height = 800,
                .xclk_to_sensor = EX_MCLK,
                .image_format = SENSOR_IMAGE_FORMAT_RAW,
            },
        },
    },
/*If there are multiple modules,please add here*/
};

static SENSOR_TRIM_T s_ov9282_dual_resolution_trim_tab[VENDOR_NUM] = {
    {
        .module_id = MODULE_SUNNY,
        .trim_info = {
            {0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
            {
                .trim_start_x = 0,
                .trim_start_y = 0,
                .trim_width = 1280,
                .trim_height = 800,
                .line_time = 18315,
                .bps_per_lane = 800,
                .frame_line = 1820,
                .scaler_trim = {
                    .x = 0,
                    .y = 0,
                    .w = 1280,
                    .h = 800,
                },
            },
        },
    },
    /*If there are multiple modules,please add here*/
};

static struct sensor_ic_ops s_ov9282_dual_ops_tab;
static struct sensor_raw_info *s_ov9282_dual_mipi_raw_info_ptr = &s_ov9282_dual_mipi_raw_info;

SENSOR_INFO_T g_ov9282_dual_mipi_raw_info = {
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
 //   .reset_pulse_level = SENSOR_LOW_PULSE_RESET,
    .reset_pulse_level = SENSOR_HIGH_PULSE_RESET,
    .reset_pulse_width = 50,
    .power_down_level = SENSOR_LOW_LEVEL_PWDN,
    .identify_count = 1,
    .identify_code = {
        {
            .reg_addr = ov9282_dual_CHIP_ID_H_ADDR,
            .reg_value = ov9282_dual_CHIP_ID_H_VALUE,
        },
        {
            .reg_addr = ov9282_dual_CHIP_ID_L_ADDR,
            .reg_value = ov9282_dual_CHIP_ID_L_VALUE,
        },
    },

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_RAW,

    .module_info_tab = s_ov9282_dual_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_ov9282_dual_module_info_tab),

    .resolution_tab_info_ptr = s_ov9282_dual_resolution_tab_raw,
    .sns_ops = &s_ov9282_dual_ops_tab,
    .raw_info_ptr = &s_ov9282_dual_mipi_raw_info_ptr,

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"ov9282_dualv1",
};
#endif
