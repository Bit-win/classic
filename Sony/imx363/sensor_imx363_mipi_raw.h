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
#ifndef _SENSOR_imx363_MIPI_RAW_H_
#define _SENSOR_imx363_MIPI_RAW_H_

#define VENDOR_NUM 3

#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

//#include "parameters/sensor_imx363_otp_truly.h"

#define SENSOR_NAME "imx363_mipi_raw"
#ifdef CAMERA_SENSOR_BACK_I2C_SWITCH
#define I2C_SLAVE_ADDR 0x20
#else
#define I2C_SLAVE_ADDR 0x34
#endif

#define BINNING_FACTOR 2
#define imx363_PID_ADDR 0x0016
#define imx363_PID_VALUE 0x03
#define imx363_VER_ADDR 0x0017
#define imx363_VER_VALUE 0x63

/* sensor parameters begin */
/* effective sensor output image size */
#if 0                        // defined(CONFIG_CAMERA_SIZE_LIMIT_FOR_ANDROIDGO)
#define SNAPSHOT_WIDTH 2328  // 5344
#define SNAPSHOT_HEIGHT 1744 // 4016
#else
#define SNAPSHOT_WIDTH 4032  // 4656  // 5344
#define SNAPSHOT_HEIGHT 3024 // 3492 // 4016
#endif
#define PREVIEW_WIDTH 2016  // 2328  // 2672
#define PREVIEW_HEIGHT 1512 // 1744 // 2008

/*Mipi output*/
#define LANE_NUM 4
#define RAW_BITS 10

#define SNAPSHOT_MIPI_PER_LANE_BPS 1032 // 1316
#define PREVIEW_MIPI_PER_LANE_BPS 300   // 374

/*line time unit: 1ns*/
#define VIDEO_LINE_TIME 10291
#define PREVIEW_LINE_TIME 20697
#define SNAPSHOT_LINE_TIME 10687

/* frame length*/
#define VIDEO_FRAME_LENGTH 808
#define PREVIEW_FRAME_LENGTH 1610
#define SNAPSHOT_FRAME_LENGTH 3118

/* please ref your spec */
#define FRAME_OFFSET 14
#define SENSOR_MAX_GAIN 0xF0
#define SENSOR_BASE_GAIN 0x20
#define SENSOR_MIN_SHUTTER 4

/* isp parameters, please don't change it*/
#define ISP_BASE_GAIN 0x80

/* please don't change it */
#define EX_MCLK 24

/*==============================================================================
 * Description:
 * global variable
 *============================================================================*/
// static struct hdr_info_t s_hdr_info;
// static uint32_t s_current_default_frame_length;
// struct sensor_ev_info_t s_sensor_ev_info;

static struct sensor_ic_ops s_imx363_ops_tab;
struct sensor_raw_info *s_imx363_mipi_raw_info_ptr = PNULL;

static const SENSOR_REG_T imx363_init_setting[] = {
    {0x0136, 0x18}, {0x0137, 0x00}, {0x31A3, 0x00}, {0x64D4, 0x01},
    {0x64D5, 0xAA}, {0x64D6, 0x01}, {0x64D7, 0xA9}, {0x64D8, 0x01},
    {0x64D9, 0xA5}, {0x64DA, 0x01}, {0x64DB, 0xA1}, {0x720A, 0x24},
    {0x720B, 0x89}, {0x720C, 0x85}, {0x720D, 0xA1}, {0x720E, 0x6E},
    {0x729C, 0x59}, {0x817C, 0xFF}, {0x817D, 0x80}, {0x9348, 0x96},
    {0x934B, 0x8C}, {0x934C, 0x82}, {0x9353, 0xAA}, {0x9354, 0xAA},
};

static const SENSOR_REG_T imx363_1280x720_setting[] = {
    {0x0112, 0x0A}, {0x0113, 0x0A}, {0x0114, 0x03}, {0x0220, 0x00},
    {0x0221, 0x11}, {0x0340, 0x04}, {0x0341, 0x2C}, {0x0342, 0x11},
    {0x0343, 0x10}, {0x0381, 0x01}, {0x0383, 0x01}, {0x0385, 0x01},
    {0x0387, 0x01}, {0x0900, 0x01}, {0x0901, 0x22}, {0x30F4, 0x02},
    {0x30F5, 0xBC}, {0x30F6, 0x01}, {0x30F7, 0x18}, {0x31A0, 0x02},
    {0x31A5, 0x00}, {0x31A6, 0x01}, {0x560F, 0x14}, {0x5856, 0x04},
    {0x58D0, 0x0E}, {0x734A, 0x23}, {0x734F, 0x64}, {0x7441, 0x5A},
    {0x7914, 0x02}, {0x7928, 0x08}, {0x7929, 0x08}, {0x793F, 0x02},
    {0xBC7B, 0x2C}, {0x0344, 0x00}, {0x0345, 0x00}, {0x0346, 0x03},
    {0x0347, 0x18}, {0x0348, 0x0F}, {0x0349, 0xBF}, {0x034A, 0x08},
    {0x034B, 0xB7}, {0x034C, 0x05}, {0x034D, 0x00}, {0x034E, 0x02},
    {0x034F, 0xD0}, {0x0408, 0x01}, {0x0409, 0x70}, {0x040A, 0x00},
    {0x040B, 0x00}, {0x040C, 0x05}, {0x040D, 0x00}, {0x040E, 0x02},
    {0x040F, 0xD0}, {0x0301, 0x03}, {0x0303, 0x02}, {0x0305, 0x04},
    {0x0306, 0x00}, {0x0307, 0x46}, {0x0309, 0x0A}, {0x030B, 0x04},
    {0x030D, 0x03}, {0x030E, 0x00}, {0x030F, 0x97}, {0x0310, 0x01},
    {0x0202, 0x04}, {0x0203, 0x1E}, {0x0224, 0x01}, {0x0225, 0xF4},
    {0x0204, 0x00}, {0x0205, 0x00}, {0x0216, 0x00}, {0x0217, 0x00},
    {0x020E, 0x01}, {0x020F, 0x00}, {0x0226, 0x01}, {0x0227, 0x00},
};

static const SENSOR_REG_T imx363_2016x1512_setting[] = {
    {0x0112, 0x0A}, {0x0113, 0x0A}, {0x0114, 0x03}, {0x0220, 0x00},
    {0x0221, 0x11}, {0x0340, 0x08}, {0x0341, 0x58}, {0x0342, 0x11},
    {0x0343, 0x10}, {0x0381, 0x01}, {0x0383, 0x01}, {0x0385, 0x01},
    {0x0387, 0x01}, {0x0900, 0x01}, {0x0901, 0x22}, {0x30F4, 0x02},
    {0x30F5, 0xBC}, {0x30F6, 0x01}, {0x30F7, 0x18}, {0x31A0, 0x02},
    {0x31A5, 0x00}, {0x31A6, 0x01}, {0x560F, 0x14}, {0x5856, 0x04},
    {0x58D0, 0x0E}, {0x734A, 0x23}, {0x734F, 0x64}, {0x7441, 0x5A},
    {0x7914, 0x02}, {0x7928, 0x08}, {0x7929, 0x08}, {0x793F, 0x02},
    {0xBC7B, 0x2C}, {0x0344, 0x00}, {0x0345, 0x00}, {0x0346, 0x00},
    {0x0347, 0x00}, {0x0348, 0x0F}, {0x0349, 0xBF}, {0x034A, 0x0B},
    {0x034B, 0xCF}, {0x034C, 0x07}, {0x034D, 0xE0}, {0x034E, 0x05},
    {0x034F, 0xE8}, {0x0408, 0x00}, {0x0409, 0x00}, {0x040A, 0x00},
    {0x040B, 0x00}, {0x040C, 0x07}, {0x040D, 0xE0}, {0x040E, 0x05},
    {0x040F, 0xE8}, {0x0301, 0x03}, {0x0303, 0x02}, {0x0305, 0x04},
    {0x0306, 0x00}, {0x0307, 0x46}, {0x0309, 0x0A}, {0x030B, 0x04},
    {0x030D, 0x03}, {0x030E, 0x00}, {0x030F, 0xC2}, {0x0310, 0x01},
    {0x0202, 0x08}, {0x0203, 0x4A}, {0x0224, 0x01}, {0x0225, 0xF4},
    {0x0204, 0x00}, {0x0205, 0x00}, {0x0216, 0x00}, {0x0217, 0x00},
    {0x020E, 0x01}, {0x020F, 0x00}, {0x0226, 0x01}, {0x0227, 0x00},
};

static const SENSOR_REG_T imx363_4032x3024_setting[] = {
    {0x0112, 0x0A}, {0x0113, 0x0A}, {0x0114, 0x03}, {0x0220, 0x00},
    {0x0221, 0x11}, {0x0340, 0x0C}, {0x0341, 0x34}, {0x0342, 0x21},
    {0x0343, 0x28}, {0x0381, 0x01}, {0x0383, 0x01}, {0x0385, 0x01},
    {0x0387, 0x01}, {0x0900, 0x00}, {0x0901, 0x11}, {0x30F4, 0x00},
    {0x30F5, 0x14}, {0x30F6, 0x03}, {0x30F7, 0x20}, {0x31A0, 0x02},
    {0x31A5, 0x00}, {0x31A6, 0x00}, {0x560F, 0x05}, {0x5856, 0x04},
    {0x58D0, 0x0E}, {0x734A, 0x23}, {0x734F, 0x64}, {0x7441, 0x5A},
    {0x7914, 0x02}, {0x7928, 0x08}, {0x7929, 0x08}, {0x793F, 0x02},
    {0xBC7B, 0x2C}, {0x0344, 0x00}, {0x0345, 0x00}, {0x0346, 0x00},
    {0x0347, 0x00}, {0x0348, 0x0F}, {0x0349, 0xBF}, {0x034A, 0x0B},
    {0x034B, 0xCF}, {0x034C, 0x0F}, {0x034D, 0xC0}, {0x034E, 0x0B},
    {0x034F, 0xD0}, {0x0408, 0x00}, {0x0409, 0x00}, {0x040A, 0x00},
    {0x040B, 0x00}, {0x040C, 0x0F}, {0x040D, 0xC0}, {0x040E, 0x0B},
    {0x040F, 0xD0}, {0x0301, 0x03}, {0x0303, 0x02}, {0x0305, 0x04},
    {0x0306, 0x00}, {0x0307, 0xC7}, {0x0309, 0x0A}, {0x030B, 0x01},
    {0x030D, 0x04}, {0x030E, 0x00}, {0x030F, 0xAE}, {0x0310, 0x01},
    {0x0202, 0x0C}, {0x0203, 0x26}, {0x0224, 0x01}, {0x0225, 0xF4},
    {0x0204, 0x00}, {0x0205, 0x00}, {0x0216, 0x00}, {0x0217, 0x00},
    {0x020E, 0x01}, {0x020F, 0x00}, {0x0226, 0x01}, {0x0227, 0x00},
};

// dualPD mode 3
static const SENSOR_REG_T imx363_1280x720_setting3[] = {
    {0x0112, 0x0A}, {0x0113, 0x0A}, {0x0114, 0x03}, {0x0220, 0x00},
    {0x0221, 0x11}, {0x0340, 0x03}, {0x0341, 0x28}, {0x0342, 0x1C},
    {0x0343, 0xC8}, {0x0381, 0x01}, {0x0383, 0x01}, {0x0385, 0x01},
    {0x0387, 0x01}, {0x0900, 0x01}, {0x0901, 0x22}, {0x30F4, 0x01},
    {0x30F5, 0x40}, {0x30F6, 0x00}, {0x30F7, 0x14}, {0x31A0, 0x03},
    {0x3040, 0x01}, {0x4073, 0x30}, {0x31A5, 0x00}, {0x31A6, 0x01},
    {0x560F, 0x3D}, {0x5856, 0x04}, {0x58D0, 0x0E}, {0x734A, 0x23},
    {0x734F, 0x64}, {0x7441, 0x5A}, {0x7914, 0x02}, {0x7928, 0x08},
    {0x7929, 0x08}, {0x793F, 0x02}, {0xBC7B, 0x2C}, {0x0344, 0x00},
    {0x0345, 0x00}, {0x0346, 0x03}, {0x0347, 0x18}, {0x0348, 0x0F},
    {0x0349, 0xBF}, {0x034A, 0x08}, {0x034B, 0xB7}, {0x034C, 0x05},
    {0x034D, 0x00}, {0x034E, 0x02}, {0x034F, 0xD0}, {0x0408, 0x01},
    {0x0409, 0x70}, {0x040A, 0x00}, {0x040B, 0x00}, {0x040C, 0x05},
    {0x040D, 0x00}, {0x040E, 0x02}, {0x040F, 0xD0}, {0x0301, 0x03},
    {0x0303, 0x02}, {0x0305, 0x04}, {0x0306, 0x00}, {0x0307, 0xB3},
    {0x0309, 0x0A}, {0x030B, 0x02}, {0x030D, 0x04}, {0x030E, 0x00},
    {0x030F, 0xAA}, {0x0310, 0x01}, {0x0202, 0x03}, {0x0203, 0x1A},
    {0x0224, 0x01}, {0x0225, 0xF4}, {0x0204, 0x00}, {0x0205, 0x00},
    {0x0216, 0x00}, {0x0217, 0x00}, {0x020E, 0x01}, {0x020F, 0x00},
    {0x0226, 0x01}, {0x0227, 0x00},
};

// dualPD mode 3
static const SENSOR_REG_T imx363_2016x1512_setting3[] = {
    {0x0112, 0x0A}, {0x0113, 0x0A}, {0x0114, 0x03}, {0x0220, 0x00},
    {0x0221, 0x11}, {0x0340, 0x06}, {0x0341, 0x4A}, {0x0342, 0x1C},
    {0x0343, 0xC8}, {0x0381, 0x01}, {0x0383, 0x01}, {0x0385, 0x01},
    {0x0387, 0x01}, {0x0900, 0x01}, {0x0901, 0x22}, {0x30F4, 0x01},
    {0x30F5, 0x40}, {0x30F6, 0x00}, {0x30F7, 0x14}, {0x31A0, 0x03},
    {0x3040, 0x01}, {0x4073, 0x30}, {0x31A5, 0x00}, {0x31A6, 0x01},
    {0x560F, 0x3D}, {0x5856, 0x04}, {0x58D0, 0x0E}, {0x734A, 0x23},
    {0x734F, 0x64}, {0x7441, 0x5A}, {0x7914, 0x02}, {0x7928, 0x08},
    {0x7929, 0x08}, {0x793F, 0x02}, {0xBC7B, 0x2C}, {0x0344, 0x00},
    {0x0345, 0x00}, {0x0346, 0x00}, {0x0347, 0x00}, {0x0348, 0x0F},
    {0x0349, 0xBF}, {0x034A, 0x0B}, {0x034B, 0xCF}, {0x034C, 0x07},
    {0x034D, 0xE0}, {0x034E, 0x05}, {0x034F, 0xE8}, {0x0408, 0x00},
    {0x0409, 0x00}, {0x040A, 0x00}, {0x040B, 0x00}, {0x040C, 0x07},
    {0x040D, 0xE0}, {0x040E, 0x05}, {0x040F, 0xE8}, {0x0301, 0x03},
    {0x0303, 0x02}, {0x0305, 0x04}, {0x0306, 0x00}, {0x0307, 0x59},
    {0x0309, 0x0A}, {0x030B, 0x04}, {0x030D, 0x03}, {0x030E, 0x00},
    {0x030F, 0xBB}, {0x0310, 0x01}, {0x0202, 0x06}, {0x0203, 0x3C},
    {0x0224, 0x01}, {0x0225, 0xF4}, {0x0204, 0x00}, {0x0205, 0x00},
    {0x0216, 0x00}, {0x0217, 0x00}, {0x020E, 0x01}, {0x020F, 0x00},
    {0x0226, 0x01}, {0x0227, 0x00},
};

// dualPD mode 3
static const SENSOR_REG_T imx363_4032x3024_setting3[] = {
    {0x0112, 0x0A}, {0x0113, 0x0A}, {0x0114, 0x03}, {0x0220, 0x00},
    {0x0221, 0x11}, {0x0340, 0x0C}, {0x0341, 0x2E}, {0x0342, 0x23},
    {0x0343, 0x10}, {0x0381, 0x01}, {0x0383, 0x01}, {0x0385, 0x01},
    {0x0387, 0x01}, {0x0900, 0x00}, {0x0901, 0x11}, {0x30F4, 0x02},
    {0x30F5, 0x08}, {0x30F6, 0x00}, {0x30F7, 0x14}, {0x31A0, 0x03},
    {0x3040, 0x01}, {0x4073, 0x30}, {0x31A5, 0x00}, {0x31A6, 0x00},
    {0x560F, 0x6A}, {0x5856, 0x04}, {0x58D0, 0x0E}, {0x734A, 0x23},
    {0x734F, 0x64}, {0x7441, 0x5A}, {0x7914, 0x02}, {0x7928, 0x08},
    {0x7929, 0x08}, {0x793F, 0x02}, {0xBC7B, 0x2C}, {0x0344, 0x00},
    {0x0345, 0x00}, {0x0346, 0x00}, {0x0347, 0x00}, {0x0348, 0x0F},
    {0x0349, 0xBF}, {0x034A, 0x0B}, {0x034B, 0xCF}, {0x034C, 0x0F},
    {0x034D, 0xC0}, {0x034E, 0x0B}, {0x034F, 0xD0}, {0x0408, 0x00},
    {0x0409, 0x00}, {0x040A, 0x00}, {0x040B, 0x00}, {0x040C, 0x0F},
    {0x040D, 0xC0}, {0x040E, 0x0B}, {0x040F, 0xD0}, {0x0301, 0x03},
    {0x0303, 0x02}, {0x0305, 0x04}, {0x0306, 0x00}, {0x0307, 0xD2},
    {0x0309, 0x0A}, {0x030B, 0x01}, {0x030D, 0x04}, {0x030E, 0x00},
    {0x030F, 0xDA}, {0x0310, 0x01}, {0x0202, 0x0C}, {0x0203, 0x20},
    {0x0224, 0x01}, {0x0225, 0xF4}, {0x0204, 0x00}, {0x0205, 0x00},
    {0x0216, 0x00}, {0x0217, 0x00}, {0x020E, 0x01}, {0x020F, 0x00},
    {0x0226, 0x01}, {0x0227, 0x00},
};

static struct sensor_reg_tag imx363_shutter_reg[] = {
    {0x0202, 0}, {0x0203, 0},
};

static struct sensor_i2c_reg_tab imx363_shutter_tab = {
    .settings = imx363_shutter_reg, .size = ARRAY_SIZE(imx363_shutter_reg),
};

static struct sensor_reg_tag imx363_again_reg[] = {
    {0x0104, 0x01}, {0x0204, 0x00}, {0x0205, 0x00},
};

static struct sensor_i2c_reg_tab imx363_again_tab = {
    .settings = imx363_again_reg, .size = ARRAY_SIZE(imx363_again_reg),
};

static struct sensor_reg_tag imx363_dgain_reg[] = {
    {0x020e, 0x00}, {0x020f, 0x00}, {0x0104, 0x00},
};

struct sensor_i2c_reg_tab imx363_dgain_tab = {
    .settings = imx363_dgain_reg, .size = ARRAY_SIZE(imx363_dgain_reg),
};

static struct sensor_reg_tag imx363_frame_length_reg[] = {
    {0x0340, 0}, {0x0341, 0},
};

static struct sensor_i2c_reg_tab imx363_frame_length_tab = {
    .settings = imx363_frame_length_reg,
    .size = ARRAY_SIZE(imx363_frame_length_reg),
};

static SENSOR_REG_T imx363_grp_hold_start_reg[] = {

};

static struct sensor_i2c_reg_tab imx363_grp_hold_start_tab = {
    .settings = imx363_grp_hold_start_reg,
    .size = ARRAY_SIZE(imx363_grp_hold_start_reg),
};

static SENSOR_REG_T imx363_grp_hold_end_reg[] = {

};

static struct sensor_i2c_reg_tab imx363_grp_hold_end_tab = {
    .settings = imx363_grp_hold_end_reg,
    .size = ARRAY_SIZE(imx363_grp_hold_end_reg),
};

static struct sensor_aec_i2c_tag imx363_aec_info = {
    .slave_addr = (0x34 >> 1),
    .addr_bits_type = SENSOR_I2C_REG_16BIT,
    .data_bits_type = SENSOR_I2C_VAL_8BIT,
    .shutter = &imx363_shutter_tab,
    .again = &imx363_again_tab,
    .dgain = &imx363_dgain_tab,
    .frame_length = &imx363_frame_length_tab,
    .grp_hold_start = &imx363_grp_hold_start_tab,
    .grp_hold_end = &imx363_grp_hold_end_tab,
};

static SENSOR_STATIC_INFO_T s_imx363_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .static_info = {.f_num = 200,
                     .focal_length = 352,
                     .max_fps = 0,
                     .max_adgain = 16 * 16,
                     .ois_supported = 1,
                     .pdaf_supported = 4,
                     .exp_valid_frame_num = 1,
                     .clamp_level = 64,
                     .adgain_valid_frame_num = 2,
                     .fov_info = {{4.656f, 3.496f}, 3.698f}}},
};

/*==============================================================================
 * Description:
 * sensor fps info related to sensor mode need by isp
 * please modify this variable acording your spec
 *============================================================================*/
static SENSOR_MODE_FPS_INFO_T s_imx363_mode_fps_info[VENDOR_NUM] = {
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
       {SENSOR_MODE_SNAPSHOT_TWO_THIRD, 0, 1, 0, 0}}}},
};

/*==============================================================================
 * Description:
 * sensor all info
 * please modify this variable acording your spec
 *============================================================================*/
static struct sensor_res_tab_info s_imx363_resolution_tab_raw[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .reg_tab =
         {{ADDR_AND_LEN_OF_ARRAY(imx363_init_setting), PNULL, 0, .width = 0,
           .height = 0, .xclk_to_sensor = EX_MCLK,
           .image_format = SENSOR_IMAGE_FORMAT_RAW},
          /*{
             ADDR_AND_LEN_OF_ARRAY(imx363_1040x768_setting),1040,768,EX_MCLK,SENSOR_IMAGE_FORMAT_RAW
             },*/
          {ADDR_AND_LEN_OF_ARRAY(imx363_1280x720_setting3), PNULL, 0,
           .width = 1280, .height = 720, .xclk_to_sensor = EX_MCLK,
           .image_format = SENSOR_IMAGE_FORMAT_RAW},
          {ADDR_AND_LEN_OF_ARRAY(imx363_2016x1512_setting3), PNULL, 0,
           .width = 2016, .height = 1512, .xclk_to_sensor = EX_MCLK,
           .image_format = SENSOR_IMAGE_FORMAT_RAW},
          {ADDR_AND_LEN_OF_ARRAY(imx363_4032x3024_setting3), PNULL, 0,
           .width = 4032, .height = 3024, .xclk_to_sensor = EX_MCLK,
           .image_format = SENSOR_IMAGE_FORMAT_RAW}}},
};

static SENSOR_TRIM_T s_imx363_resolution_trim_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .trim_info = {{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
                   /*	{0,0, 1040, 768,10325,1296, 812, { 0,0,1040,768}},*/
                   {0, 0, 1280, 720, 10291, 510, 808, {0, 0, 1280, 720}},
                   {0, 0, 2016, 1512, 20697, 374, 1610, {0, 0, 2016, 1512}},
                   {0, 0, 4032, 3024, 10687, 1308, 3118, {0, 0, 4032, 3024}}}},
};

static const SENSOR_REG_T
    s_imx363_preview_size_video_tab[SENSOR_VIDEO_MODE_MAX][1] = {
        /*video mode 0: ?fps */
        {{0xffff, 0xff}},
        /* video mode 1:?fps */
        {{0xffff, 0xff}},
        /* video mode 2:?fps */
        {{0xffff, 0xff}},
        /* video mode 3:?fps */
        {{0xffff, 0xff}}};

static const SENSOR_REG_T
    s_imx363_capture_size_video_tab[SENSOR_VIDEO_MODE_MAX][1] = {
        /*video mode 0: ?fps */
        {{0xffff, 0xff}},
        /* video mode 1:?fps */
        {{0xffff, 0xff}},
        /* video mode 2:?fps */
        {{0xffff, 0xff}},
        /* video mode 3:?fps */
        {{0xffff, 0xff}}};

static SENSOR_VIDEO_INFO_T s_imx363_video_info[] = {
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},

    {.ae_info =
         {{.min_frate = 30, .max_frate = 30, .line_time = 270, .gain = 90},
          {0, 0, 0, 0},
          {0, 0, 0, 0},
          {0, 0, 0, 0}},
     .setting_ptr = (struct sensor_reg_tag **)s_imx363_preview_size_video_tab},

    {.ae_info =
         {{.min_frate = 2, .max_frate = 5, .line_time = 338, .gain = 1000},
          {0, 0, 0, 0},
          {0, 0, 0, 0},
          {0, 0, 0, 0}},
     .setting_ptr = (struct sensor_reg_tag **)s_imx363_capture_size_video_tab},
};

static struct sensor_module_info s_imx363_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {.major_i2c_addr = 0x34 >> 1,
                     .minor_i2c_addr = 0x20 >> 1,

                    .i2c_burst_mode = SENSOR_I2C_BURST_REG16_VAL8,

                     .reg_addr_value_bits = SENSOR_I2C_REG_16BIT |
                                            SENSOR_I2C_VAL_8BIT |
                                            SENSOR_I2C_FREQ_400,

                     .avdd_val = SENSOR_AVDD_2800MV,
                     .iovdd_val = SENSOR_AVDD_1800MV,
                     .dvdd_val = SENSOR_AVDD_1200MV,

                     .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_R,

                     .preview_skip_num = 1,
                     .capture_skip_num = 1,
                     .flash_capture_skip_num = 6,
                     .mipi_cap_skip_num = 0,
                     .preview_deci_num = 0,
                     .video_preview_deci_num = 0,

                     .sensor_interface =
                         {
                             .type = SENSOR_INTERFACE_TYPE_CSI2,
                             .bus_width = 4,
                             .pixel_width = 10,
                             #ifdef _SENSOR_RAW_SHARKL5PRO_H_,
                                 .is_loose = 2,
                             #else
                                 .is_loose = 0,
                             #endif
                         },

                     .change_setting_skip_num = 1,
                     .horizontal_view_angle = 35,
                     .vertical_view_angle = 35}},
};

SENSOR_INFO_T g_imx363_mipi_raw_info = {
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
    .identify_code = {{.reg_addr = imx363_PID_ADDR,
                       .reg_value = imx363_PID_VALUE},
                      {.reg_addr = imx363_VER_ADDR,
                       .reg_value = imx363_VER_VALUE}},

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_RAW,

    .resolution_tab_info_ptr = s_imx363_resolution_tab_raw,
    .sns_ops = &s_imx363_ops_tab,
    .raw_info_ptr = &s_imx363_mipi_raw_info_ptr,
    .module_info_tab = s_imx363_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_imx363_module_info_tab),
    .video_tab_info_ptr = s_imx363_video_info,
    .ext_info_ptr = NULL,

    .sensor_version_info = (cmr_s8 *)"imx363v1"};

#endif
