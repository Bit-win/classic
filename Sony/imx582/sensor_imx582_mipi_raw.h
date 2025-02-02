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

#ifndef _SENSOR_imx582_MIPI_RAW_H_
#define _SENSOR_imx582_MIPI_RAW_H_

#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

//#define FEATURE_OTP

#define VENDOR_NUM 1
#define SENSOR_NAME "imx582_mipi_raw"
#define I2C_SLAVE_ADDR 0x20 /* 8bit slave address*/

#define imx582_PID_ADDR 0x0016
#define imx582_PID_VALUE 0x05
#define imx582_VER_ADDR 0x0017
#define imx582_VER_VALUE 0x82

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

#define VIDEO_MIPI_PER_LANE_BPS 717   /* 2*Mipi clk */
#define PREVIEW_MIPI_PER_LANE_BPS 1146  /* 2*Mipi clk */
#define SNAPSHOT_MIPI_PER_LANE_BPS 2058 /* 2*Mipi clk */

/*line time unit: 1ns*/
#define VIDEO_LINE_TIME 7240
#define PREVIEW_LINE_TIME 10893
#define SNAPSHOT_LINE_TIME 10990

/* frame length*/
#define VIDEO_FRAME_LENGTH 1150
#define PREVIEW_FRAME_LENGTH 3058
#define SNAPSHOT_FRAME_LENGTH 6062

/* please ref your spec */
#define FRAME_OFFSET 48
#define SENSOR_MAX_GAIN 0x2c0
#define SENSOR_BASE_GAIN 0x0020
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

/*==============================================================================
 * Description:
 * register setting
 *============================================================================*/
static const SENSOR_REG_T imx582_init_setting[] = {
    {0x0136, 0x18},
    {0x0137, 0x00},
    {0x3C7E, 0x01},
    {0x3C7F, 0x06},
    {0x3C00, 0x10},
    {0x3C01, 0x10},
    {0x3C02, 0x10},
    {0x3C03, 0x10},
    {0x3C04, 0x10},
    {0x3C05, 0x01},
    {0x3C06, 0x00},
    {0x3C07, 0x00},
    {0x3C08, 0x03},
    {0x3C09, 0xFF},
    {0x3C0A, 0x01},
    {0x3C0B, 0x00},
    {0x3C0C, 0x00},
    {0x3C0D, 0x03},
    {0x3C0E, 0xFF},
    {0x3C0F, 0x20},
    {0x6E1D, 0x00},
    {0x6E25, 0x00},
    {0x6E38, 0x03},
    {0x6E3B, 0x01},///
    {0x9004, 0x2C},
    {0x9200, 0xF4},
    {0x9201, 0xA7},
    {0x9202, 0xF4},
    {0x9203, 0xAA},
    {0x9204, 0xF4},
    {0x9205, 0xAD},
    {0x9206, 0xF4},
    {0x9207, 0xB0},
    {0x9208, 0xF4},
    {0x9209, 0xB3},
    {0x920A, 0xB7},
    {0x920B, 0x34},
    {0x920C, 0xB7},
    {0x920D, 0x36},
    {0x920E, 0xB7},
    {0x920F, 0x37},
    {0x9210, 0xB7},
    {0x9211, 0x38},
    {0x9212, 0xB7},
    {0x9213, 0x39},
    {0x9214, 0xB7},
    {0x9215, 0x3A},
    {0x9216, 0xB7},
    {0x9217, 0x3C},
    {0x9218, 0xB7},
    {0x9219, 0x3D},
    {0x921A, 0xB7},
    {0x921B, 0x3E},
    {0x921C, 0xB7},
    {0x921D, 0x3F},
    {0x921E, 0x85},
    {0x921F, 0x77},
    {0x9226, 0x42},
    {0x9227, 0x52},
    {0x9228, 0x60},
    {0x9229, 0xB9},
    {0x922A, 0x60},
    {0x922B, 0xBF},
    {0x922C, 0x60},
    {0x922D, 0xC5},
    {0x922E, 0x60},
    {0x922F, 0xCB},
    {0x9230, 0x60},
    {0x9231, 0xD1},
    {0x9232, 0x60},
    {0x9233, 0xD7},
    {0x9234, 0x60},
    {0x9235, 0xDD},
    {0x9236, 0x60},
    {0x9237, 0xE3},
    {0x9238, 0x60},
    {0x9239, 0xE9},
    {0x923A, 0x60},
    {0x923B, 0xEF},///
    {0x923C, 0x60},
    {0x923D, 0xF5},
    {0x923E, 0x60},
    {0x923F, 0xF9},
    {0x9240, 0x60},
    {0x9241, 0xFD},
    {0x9242, 0x61},
    {0x9243, 0x01},
    {0x9244, 0x61},
    {0x9245, 0x05},
    {0x924A, 0x61},
    {0x924B, 0x6B},
    {0x924C, 0x61},
    {0x924D, 0x7F},
    {0x924E, 0x61},
    {0x924F, 0x92},
    {0x9250, 0x61},
    {0x9251, 0x9C},
    {0x9252, 0x61},
    {0x9253, 0xAB},
    {0x9254, 0x61},
    {0x9255, 0xC4},
    {0x9256, 0x61},
    {0x9257, 0xCE},
    {0x9810, 0x14},
    {0x9814, 0x14},
    {0xC449, 0x04},
    {0xC44A, 0x01},
    {0xE286, 0x31},
    {0xE2A6, 0x32},
    {0xE2C6, 0x33},
    {0x88D6, 0x60},
    {0x9852, 0x00},
    {0xAE09, 0xFF},
    {0xAE0A, 0xFF},
    {0xAE12, 0x58},
    {0xAE13, 0x58},
    {0xAE15, 0x10},
    {0xAE16, 0x10},
    {0xB071, 0x00},
};

static const SENSOR_REG_T imx582_video_setting[] = {
    {0x0112, 0x0A},
    {0x0113, 0x0A},
    {0x0114, 0x03},
    {0x0342, 0x0B},
    {0x0343, 0x60},
    {0x0340, 0x04},
    {0x0341, 0x7E},
    {0x0344, 0x00},
    {0x0345, 0x00},
    {0x0346, 0x03},
    {0x0347, 0x40},
    {0x0348, 0x1F},
    {0x0349, 0x3F},
    {0x034A, 0x14},
    {0x034B, 0x1F},
    {0x0900, 0x01},
    {0x0901, 0x44},
    {0x0902, 0x08},
    {0x3246, 0x89},
    {0x3247, 0x89},
    {0x0401, 0x00},
    {0x0404, 0x00},
    {0x0405, 0x10},
    {0x0408, 0x00},
    {0x0409, 0x28},
    {0x040A, 0x00},
    {0x040B, 0x00},
    {0x040C, 0x07},
    {0x040D, 0x80},
    {0x040E, 0x04},
    {0x040F, 0x38},
    {0x034C, 0x07},
    {0x034D, 0x80},
    {0x034E, 0x04},
    {0x034F, 0x38},
    {0x0301, 0x05},
    {0x0303, 0x04},
    {0x0305, 0x04},
    {0x0306, 0x01},
    {0x0307, 0x4F},
    {0x030B, 0x02},
    {0x030D, 0x04},
    {0x030E, 0x00},
    {0x030F, 0xEF},
    {0x0310, 0x01},
    {0x3620, 0x00},
    {0x3621, 0x00},
    {0x380C, 0x80},
    {0x3C13, 0x00},
    {0x3C14, 0x28},
    {0x3C15, 0x28},
    {0x3C16, 0x32},
    {0x3C17, 0x46},
    {0x3C18, 0x67},
    {0x3C19, 0x8F},
    {0x3C1A, 0x8F},
    {0x3C1B, 0x99},
    {0x3C1C, 0xAD},
    {0x3C1D, 0xCE},
    {0x3C1E, 0x8F},
    {0x3C1F, 0x8F},
    {0x3C20, 0x99},
    {0x3C21, 0xAD},
    {0x3C22, 0xCE},
    {0x3C25, 0x22},
    {0x3C26, 0x23},
    {0x3C27, 0xE6},
    {0x3C28, 0xE6},
    {0x3C29, 0x08},
    {0x3C2A, 0x0F},
    {0x3C2B, 0x14},
    {0x3F0C, 0x00},
    {0x3F14, 0x00},
    {0x3F80, 0x00},
    {0x3F81, 0x00},
    {0x3F82, 0x00},
    {0x3F83, 0x00},
    {0x3F8C, 0x00},
    {0x3F8D, 0x00},
    {0x3FF4, 0x00},
    {0x3FF5, 0x4C},
    {0x3FFC, 0x00},
    {0x3FFD, 0x00},
    {0x0202, 0x04},
    {0x0203, 0x4E},
    {0x0224, 0x01},
    {0x0225, 0xF4},
    {0x3FE0, 0x01},
    {0x3FE1, 0xF4},
    {0x0204, 0x00},
    {0x0205, 0x70},
    {0x0216, 0x00},
    {0x0217, 0x70},
    {0x0218, 0x01},
    {0x0219, 0x00},
    {0x020E, 0x01},
    {0x020F, 0x00},
    {0x0210, 0x01},
    {0x0211, 0x00},
    {0x0212, 0x01},
    {0x0213, 0x00},
    {0x0214, 0x01},
    {0x0215, 0x00},
    {0x3FE2, 0x00},
    {0x3FE3, 0x70},
    {0x3FE4, 0x01},
    {0x3FE5, 0x00},
    {0x3E20, 0x01},
    {0x3E3B, 0x00},
    {0x4034, 0x01},
    {0x4035, 0xF0},
};
//#if 0
static const SENSOR_REG_T imx582_preview_setting[] = {
    {0x0112, 0x0A},
    {0x0113, 0x0A},
    {0x0114, 0x03},
    {0x0342, 0x1E},
    {0x0343, 0xC0},
    {0x0340, 0x0B},
    {0x0341, 0xF2},
    {0x0344, 0x00},
    {0x0345, 0x00},
    {0x0346, 0x00},
    {0x0347, 0x00},
    {0x0348, 0x1F},
    {0x0349, 0x3F},
    {0x034A, 0x17},
    {0x034B, 0x6F},
    {0x0900, 0x01},
    {0x0901, 0x22},
    {0x0902, 0x08},
    {0x3246, 0x81},
    {0x3247, 0x81},
    {0x0401, 0x00},
    {0x0404, 0x00},
    {0x0405, 0x10},
    {0x0408, 0x00},
    {0x0409, 0x00},
    {0x040A, 0x00},
    {0x040B, 0x00},
    {0x040C, 0x0F},
    {0x040D, 0xA0},
    {0x040E, 0x0B},
    {0x040F, 0xB8},
    {0x034C, 0x0F},
    {0x034D, 0xA0},
    {0x034E, 0x0B},
    {0x034F, 0xB8},
    {0x0301, 0x05},
    {0x0303, 0x02},
    {0x0305, 0x04},
    {0x0306, 0x01},
    {0x0307, 0x2D},
    {0x030B, 0x01},
    {0x030D, 0x04},
    {0x030E, 0x00},
    {0x030F, 0xBF},
    {0x0310, 0x01},
    {0x3620, 0x00},
    {0x3621, 0x00},
    {0x380C, 0x80},
    {0x3C13, 0x00},
    {0x3C14, 0x28},
    {0x3C15, 0x28},
    {0x3C16, 0x32},
    {0x3C17, 0x46},
    {0x3C18, 0x67},
    {0x3C19, 0x8F},
    {0x3C1A, 0x8F},
    {0x3C1B, 0x99},
    {0x3C1C, 0xAD},
    {0x3C1D, 0xCE},
    {0x3C1E, 0x8F},
    {0x3C1F, 0x8F},
    {0x3C20, 0x99},
    {0x3C21, 0xAD},
    {0x3C22, 0xCE},
    {0x3C25, 0x22},
    {0x3C26, 0x23},
    {0x3C27, 0xE6},
    {0x3C28, 0xE6},
    {0x3C29, 0x08},
    {0x3C2A, 0x0F},
    {0x3C2B, 0x14},
    {0x3F0C, 0x01},
    {0x3F14, 0x00},
    {0x3F80, 0x00},
    {0x3F81, 0x00},
    {0x3F82, 0x00},
    {0x3F83, 0x00},
    {0x3F8C, 0x07},
    {0x3F8D, 0xD0},
    {0x3FF4, 0x00},
    {0x3FF5, 0x00},
    {0x3FFC, 0x04},
    {0x3FFD, 0xB0},
    {0x0202, 0x0B},
    {0x0203, 0xC2},
    {0x0224, 0x01},
    {0x0225, 0xF4},
    {0x3FE0, 0x01},
    {0x3FE1, 0xF4},
    {0x0204, 0x00},
    {0x0205, 0x70},
    {0x0216, 0x00},
    {0x0217, 0x70},
    {0x0218, 0x01},
    {0x0219, 0x00},
    {0x020E, 0x01},
    {0x020F, 0x00},
    {0x0210, 0x01},
    {0x0211, 0x00},
    {0x0212, 0x01},
    {0x0213, 0x00},
    {0x0214, 0x01},
    {0x0215, 0x00},
    {0x3FE2, 0x00},
    {0x3FE3, 0x70},
    {0x3FE4, 0x01},
    {0x3FE5, 0x00},
    {0x3E20, 0x02},
    {0x3E3B, 0x01},
    {0x4034, 0x01},
    {0x4035, 0xF0},
};

static const SENSOR_REG_T imx582_snapshot_setting[] = {
    {0x0112, 0x0A},
    {0x0113, 0x0A},
    {0x0114, 0x03},
    {0x0342, 0x23},
    {0x0343, 0xE0},
    {0x0340, 0x17},
    {0x0341, 0xAE},
    {0x0344, 0x00},
    {0x0345, 0x00},
    {0x0346, 0x00},
    {0x0347, 0x00},
    {0x0348, 0x1F},
    {0x0349, 0x3F},
    {0x034A, 0x17},
    {0x034B, 0x6F},
    {0x0900, 0x00},
    {0x0901, 0x11},
    {0x0902, 0x0A},
    {0x3246, 0x01},
    {0x3247, 0x01},
    {0x0401, 0x00},
    {0x0404, 0x00},
    {0x0405, 0x10},
    {0x0408, 0x00},
    {0x0409, 0x00},
    {0x040A, 0x00},
    {0x040B, 0x00},
    {0x040C, 0x1F},
    {0x040D, 0x40},
    {0x040E, 0x17},
    {0x040F, 0x70},
    {0x034C, 0x1F},
    {0x034D, 0x40},
    {0x034E, 0x17},
    {0x034F, 0x70},
    {0x0301, 0x05},
    {0x0303, 0x02},
    {0x0305, 0x04},
    {0x0306, 0x01},
    {0x0307, 0x5C},
    {0x030B, 0x01},
    {0x030D, 0x04},
    {0x030E, 0x01},
    {0x030F, 0x57},
    {0x0310, 0x01},
    {0x3620, 0x01},
    {0x3621, 0x01},
    {0x380C, 0x80},
    {0x3C13, 0x00},
    {0x3C14, 0x28},
    {0x3C15, 0x28},
    {0x3C16, 0x32},
    {0x3C17, 0x46},
    {0x3C18, 0x67},
    {0x3C19, 0x8F},
    {0x3C1A, 0x8F},
    {0x3C1B, 0x99},
    {0x3C1C, 0xAD},
    {0x3C1D, 0xCE},
    {0x3C1E, 0x8F},
    {0x3C1F, 0x8F},
    {0x3C20, 0x99},
    {0x3C21, 0xAD},
    {0x3C22, 0xCE},
    {0x3C25, 0x22},
    {0x3C26, 0x23},
    {0x3C27, 0xE6},
    {0x3C28, 0xE6},
    {0x3C29, 0x08},
    {0x3C2A, 0x0F},
    {0x3C2B, 0x14},
    {0x3F0C, 0x00},
    {0x3F14, 0x01},
    {0x3F80, 0x04},
    {0x3F81, 0xB0},
    {0x3F82, 0x00},
    {0x3F83, 0x00},
    {0x3F8C, 0x03},
    {0x3F8D, 0x5C},
    {0x3FF4, 0x00},
    {0x3FF5, 0x00},
    {0x3FFC, 0x00},
    {0x3FFD, 0x00},
    {0x0202, 0x17},
    {0x0203, 0x7E},
    {0x0224, 0x01},
    {0x0225, 0xF4},
    {0x3FE0, 0x01},
    {0x3FE1, 0xF4},
    {0x0204, 0x00},
    {0x0205, 0x70},
    {0x0216, 0x00},
    {0x0217, 0x70},
    {0x0218, 0x01},
    {0x0219, 0x00},
    {0x020E, 0x01},
    {0x020F, 0x00},
    {0x0210, 0x01},
    {0x0211, 0x00},
    {0x0212, 0x01},
    {0x0213, 0x00},
    {0x0214, 0x01},
    {0x0215, 0x00},
    {0x3FE2, 0x00},
    {0x3FE3, 0x70},
    {0x3FE4, 0x01},
    {0x3FE5, 0x00},
    {0x3E20, 0x02},
    {0x3E3B, 0x01},
    {0x4034, 0x01},
    {0x4035, 0xF0},
};
//#endif
static struct sensor_res_tab_info s_imx582_resolution_tab_raw[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .reg_tab =
         {{ADDR_AND_LEN_OF_ARRAY(imx582_init_setting), PNULL, 0, .width = 0,
           .height = 0, .xclk_to_sensor = EX_MCLK,
           .image_format = SENSOR_IMAGE_FORMAT_RAW},

          {ADDR_AND_LEN_OF_ARRAY(imx582_video_setting), PNULL, 0,
           .width = VIDEO_WIDTH, .height = VIDEO_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},
//#if 0
          {ADDR_AND_LEN_OF_ARRAY(imx582_preview_setting), PNULL, 0,
           .width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

          {ADDR_AND_LEN_OF_ARRAY(imx582_snapshot_setting), PNULL, 0,
           .width = SNAPSHOT_WIDTH, .height = SNAPSHOT_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW}
//#endif
}}

    /*If there are multiple modules,please add here*/
};

static SENSOR_TRIM_T s_imx582_resolution_trim_tab[VENDOR_NUM] = {
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
//#if 0
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
//#endif
	}}

    /*If there are multiple modules,please add here*/

};

static SENSOR_REG_T imx582_shutter_reg[] = {
    {0x0202, 0x0000}, {0x0203, 0x0000},
};

static struct sensor_i2c_reg_tab imx582_shutter_tab = {
    .settings = imx582_shutter_reg, .size = ARRAY_SIZE(imx582_shutter_reg),
};

static SENSOR_REG_T imx582_again_reg[] = {
    {0x0104, 0x0001}, {0x0204, 0x0000}, {0x0205, 0x0000}, {0x0104, 0x0000},
};

static struct sensor_i2c_reg_tab imx582_again_tab = {
    .settings = imx582_again_reg, .size = ARRAY_SIZE(imx582_again_reg),
};

static SENSOR_REG_T imx582_dgain_reg[] = {

};

static struct sensor_i2c_reg_tab imx582_dgain_tab = {
    .settings = imx582_dgain_reg, .size = ARRAY_SIZE(imx582_dgain_reg),
};

static SENSOR_REG_T imx582_frame_length_reg[] = {
    {0x0340, 0x0000}, {0x0341, 0x0000},
};

static struct sensor_i2c_reg_tab imx582_frame_length_tab = {
    .settings = imx582_frame_length_reg,
    .size = ARRAY_SIZE(imx582_frame_length_reg),
};

static SENSOR_REG_T imx582_grp_hold_start_reg[] = {

};

static struct sensor_i2c_reg_tab imx582_grp_hold_start_tab = {
    .settings = imx582_grp_hold_start_reg,
    .size = ARRAY_SIZE(imx582_grp_hold_start_reg),
};

static SENSOR_REG_T imx582_grp_hold_end_reg[] = {

};

static struct sensor_i2c_reg_tab imx582_grp_hold_end_tab = {
    .settings = imx582_grp_hold_end_reg,
    .size = ARRAY_SIZE(imx582_grp_hold_end_reg),
};


static struct sensor_aec_i2c_tag imx582_aec_info = {
    .slave_addr = (I2C_SLAVE_ADDR >> 1),
    .addr_bits_type = SENSOR_I2C_REG_16BIT,
    .data_bits_type = SENSOR_I2C_VAL_8BIT,
    .shutter = &imx582_shutter_tab,
    .again = &imx582_again_tab,
    .dgain = &imx582_dgain_tab,
    .frame_length = &imx582_frame_length_tab,
    .grp_hold_start = &imx582_grp_hold_start_tab,
    .grp_hold_end = &imx582_grp_hold_end_tab,
};
/*
static const cmr_u16 imx582_pd_is_right[] = {
    0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
    0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
};

static const cmr_u16 imx582_pd_row[] = {
    7,  7,  23, 23, 43, 43, 59, 59, 11, 11, 27, 27, 39, 39, 55, 55,
    11, 11, 27, 27, 39, 39, 55, 55, 7,  7,  23, 23, 43, 43, 59, 59};

static const cmr_u16 imx582_pd_col[] = {
    0,  4,  4,  8,  4,  8,  0,  4,  20, 16, 24, 20, 24, 20, 20, 16,
    36, 40, 32, 36, 32, 36, 36, 40, 56, 52, 52, 48, 52, 48, 56, 52};

 */
static SENSOR_STATIC_INFO_T s_imx582_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .static_info = {.f_num = 200,
                     .focal_length = 354,
                     .max_fps = 30,
                     .max_adgain = 8,
                     .ois_supported = 0,
                     .pdaf_supported = 2,
                     .exp_valid_frame_num = 1,
                     .clamp_level = 64,
                     .adgain_valid_frame_num = 0,
                     .fov_info = {{4.614f, 3.444f}, 4.222f}}}
    /*If there are multiple modules,please add here*/
};

static SENSOR_MODE_FPS_INFO_T s_imx582_mode_fps_info[VENDOR_NUM] = {
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

static struct sensor_module_info s_imx582_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {.major_i2c_addr = 0x20 >> 1,
                     .minor_i2c_addr = 0x34 >> 1,

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

static struct sensor_ic_ops s_imx582_ops_tab;
struct sensor_raw_info *s_imx582_mipi_raw_info_ptr =
    PNULL; //&s_imx582_mipi_raw_info;

/*==============================================================================
 * Description:
 * sensor all info
 * please modify this variable acording your spec
 *============================================================================*/
SENSOR_INFO_T g_imx582_mipi_raw_info = {
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
    .identify_code = {{.reg_addr = imx582_PID_ADDR,
                       .reg_value = imx582_PID_VALUE},
                      {.reg_addr = imx582_VER_ADDR,
                       .reg_value = imx582_VER_VALUE}},

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_RAW,

    .module_info_tab = s_imx582_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_imx582_module_info_tab),

    .resolution_tab_info_ptr = s_imx582_resolution_tab_raw,
    .sns_ops = &s_imx582_ops_tab,
    .raw_info_ptr = &s_imx582_mipi_raw_info_ptr,

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"imx582_v1",
};

#endif