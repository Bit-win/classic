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

#ifndef _SENSOR_ov08f10_MIPI_RAW_H_
#define _SENSOR_ov08f10_MIPI_RAW_H_

#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

//#include "parameters/parameters_sharkl5/sensor_ov08f10_raw_param_main.c"

#define VENDOR_NUM 1
#define SENSOR_NAME "c3z01_ov08f10_frontsyx"

#define MAJOR_I2C_SLAVE_ADDR 0x20
#define MINOR_I2C_SLAVE_ADDR 0x6C

#define ov08f10_PID_ADDR 0x02
#define ov08f10_PID_VALUE 0x46
#define ov08f10_VER_ADDR 0x03
#define ov08f10_VER_VALUE 0x10

/* sensor parameters begin */

/* effective sensor output image size */
#define VIDEO_WIDTH 3264
#define VIDEO_HEIGHT 1836
#define PREVIEW_WIDTH 3264
#define PREVIEW_HEIGHT 2448
#define SNAPSHOT_WIDTH 3264
#define SNAPSHOT_HEIGHT 2448

/*Raw Trim parameters*/
#define VIDEO_TRIM_X 0
#define VIDEO_TRIM_Y 0
#define VIDEO_TRIM_W 3264
#define VIDEO_TRIM_H 1836
#define PREVIEW_TRIM_X 0
#define PREVIEW_TRIM_Y 0
#define PREVIEW_TRIM_W 3264
#define PREVIEW_TRIM_H 2448
#define SNAPSHOT_TRIM_X 0
#define SNAPSHOT_TRIM_Y 0
#define SNAPSHOT_TRIM_W 3264
#define SNAPSHOT_TRIM_H 2448

/*Mipi output*/
#define LANE_NUM 2
#define RAW_BITS 10

#define VIDEO_MIPI_PER_LANE_BPS 1440
#define PREVIEW_MIPI_PER_LANE_BPS 1440
#define SNAPSHOT_MIPI_PER_LANE_BPS 1440

/*line time unit: 1ns*/
#define VIDEO_LINE_TIME 13278
#define PREVIEW_LINE_TIME 13278
#define SNAPSHOT_LINE_TIME 13278

/* frame length*/
#define VIDEO_FRAME_LENGTH 2501
#define PREVIEW_FRAME_LENGTH 2501
#define SNAPSHOT_FRAME_LENGTH 2501

/* please ref your spec */
#define FRAME_OFFSET 20
#define SENSOR_MAX_GAIN 0xf8
#define SENSOR_BASE_GAIN 0x10
#define SENSOR_MIN_SHUTTER 4

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
//#define IMAGE_V_MIRROR

//#define IMAGE_V_MIRROR //IMAGE_NORMAL_MIRROR; IMAGE_H_MIRROR; IMAGE_V_MIRROR;
// IMAGE_HV_MIRROR

static const SENSOR_REG_T ov08f10_init_setting[] = {
// Initial setting 
{0xfd, 0x00},
{0x20, 0x0e},
{0xffff, 0x03},//delay 3ms by FAE
{0x20, 0x0b},
{0xfd, 0x00},
{0x11, 0x4e},
{0x12, 0x01},
{0x14, 0x90},
{0x1e, 0x10},
{0xfd, 0x01},
{0x12, 0x00},
{0x03, 0x05},
{0x04, 0x08},
{0x07, 0x05},
{0x24, 0xff},
{0x33, 0x03},
{0x01, 0x03},
{0x19, 0x40},
{0xc0, 0x00},
{0x42, 0x5d},
{0x44, 0x81},
{0x46, 0x3f},
{0x47, 0x0f},
{0x48, 0x0c},
{0x4c, 0x18},
{0xb2, 0x1e},
{0xb3, 0x1b},
{0xc3, 0x2c},
{0xd2, 0xfc},
{0xfd, 0x01},
{0x50, 0x1c},
{0x53, 0x1c},
{0x54, 0x04},
{0x55, 0x04},
{0x57, 0x87},
{0x5e, 0x10},
{0x67, 0x05},
{0x7a, 0x02},
{0x7c, 0x0a},
{0x90, 0x3a},
{0x91, 0x12},
{0x92, 0x16},
{0x94, 0x12},
{0x95, 0x40},
{0x97, 0x03},
{0x98, 0x48},
{0x9a, 0x02},
{0x9c, 0x0c},
{0x9e, 0x2d},
{0xca, 0x0d},
{0xce, 0x0d},
{0xfd, 0x07},
{0x2c, 0x02},
{0x2d, 0x00},
{0x00, 0xf8},
{0x01, 0x2b},
{0x05, 0x40},
{0x08, 0x06},
{0x09, 0x11},
{0x28, 0x6f},
{0x2a, 0x20},
{0x2b, 0x05},
{0x5e, 0x10},
{0x52, 0x00},
{0x53, 0x78},
{0x54, 0x00},
{0x55, 0x78},
{0x56, 0x00},
{0x57, 0x78},
{0x58, 0x00},
{0x59, 0x78},
{0x5c, 0x3f},
{0xfd, 0x0f},
{0x00, 0x23},
{0x01, 0x01},
{0x02, 0x48},
{0x1a, 0x29},
{0x1b, 0x88},
{0x1c, 0x4c},
{0x1d, 0x9c},
{0x1f, 0x00},
{0x20, 0xcc},
{0xfd, 0x0f},
{0x03, 0x07},
{0x05, 0x0a},
{0x12, 0x00},
{0x13, 0x00},
{0x14, 0x13},
{0x08, 0x28},
{0x16, 0x0c},
{0x17, 0x0c},
{0x18, 0x0c},
{0x19, 0x0c},
{0xfd, 0x02},
{0x9a, 0x30},
{0xfd, 0x04},
{0x12, 0x01},
{0x0a, 0x10},
{0x0b, 0x11},
{0x0c, 0x12},
{0xfd, 0x03},
{0x9d, 0x0f},
{0x9f, 0x20},
{0xfd, 0x05},
{0x00, 0x00},
{0x01, 0x1c},
{0x02, 0x01},
{0x03, 0xff},
{0x1a, 0x20},
{0xfd, 0x02},
{0xa1, 0x08},
{0xa2, 0x09},
{0xa3, 0x90},
{0xa5, 0x08},
{0xa6, 0x0c},
{0xa7, 0xc0},
{0xfd, 0x00},
{0x24, 0x01},
{0xc0, 0x17},
{0xc1, 0x08},
{0xc2, 0x30},
{0x8e, 0x0c},
{0x8f, 0xc0},
{0x90, 0x09},
{0x91, 0x90},
{0xb7, 0x02},
{0xfd, 0x00},
{0xe7, 0x03},
{0xe7, 0x00},
{0x20, 0x0f},

};

static const SENSOR_REG_T ov08f10_video_setting[] = {
{0xfd, 0x00},
{0x20, 0x0e},
{0xffff, 0x03},//delay 3ms by FAE
{0x20, 0x0b},
{0xfd, 0x00},
{0x11, 0x4e},
{0x12, 0x01},
{0x14, 0x90},
{0x1e, 0x10},
{0xfd, 0x01},
{0x12, 0x00},
{0x03, 0x05},
{0x04, 0x08},
{0x07, 0x05},
{0x24, 0xff},
{0x32, 0x03},
{0x33, 0x03},
{0x01, 0x03},
{0x19, 0x40},
{0xc0, 0x00},
{0x42, 0x5d},
{0x44, 0x81},
{0x46, 0x3f},
{0x47, 0x0f},
{0x48, 0x0c},
{0x4c, 0x18},
{0xb2, 0x1e},
{0xb3, 0x1b},
{0xc3, 0x2c},
{0xd2, 0xfc},
{0xfd, 0x01},
{0x50, 0x1c},
{0x53, 0x1c},
{0x54, 0x04},
{0x55, 0x04},
{0x57, 0x87},
{0x5e, 0x10},
{0x67, 0x05},
{0x7a, 0x02},
{0x7c, 0x0a},
{0x90, 0x3a},
{0x91, 0x12},
{0x92, 0x16},
{0x94, 0x12},
{0x95, 0x40},
{0x97, 0x03},
{0x98, 0x48},
{0x9a, 0x02},
{0x9c, 0x0c},
{0x9e, 0x2d},
{0xca, 0x0d},
{0xce, 0x0d},
{0xfd, 0x07},
{0x2c, 0x02},
{0x2d, 0x00},
{0x00, 0xf8},
{0x01, 0x2b},
{0x05, 0x40},
{0x08, 0x06},
{0x09, 0x11},
{0x28, 0x6f},
{0x2a, 0x20},
{0x2b, 0x05},
{0x5e, 0x10},
{0x52, 0x00},
{0x53, 0x78},
{0x54, 0x00},
{0x55, 0x78},
{0x56, 0x00},
{0x57, 0x78},
{0x58, 0x00},
{0x59, 0x78},
{0x5c, 0x3f},
{0xfd, 0x0f},
{0x00, 0x23},
{0x01, 0x01},
{0x02, 0x48},
{0x1a, 0x29},
{0x1b, 0x88},
{0x1c, 0x4c},
{0x1d, 0x9c},
{0x1f, 0x00},
{0x20, 0xcc},
{0xfd, 0x0f},
{0x03, 0x07},
{0x05, 0x0a},
{0x12, 0x00},
{0x13, 0x00},
{0x14, 0x13},
{0x08, 0x28},
{0x16, 0x0c},
{0x17, 0x0c},
{0x18, 0x0c},
{0x19, 0x0c},
{0xfd, 0x02},
{0x9a, 0x30},
{0xfd, 0x04},
{0x12, 0x01},
{0x0a, 0x10},
{0x0b, 0x11},
{0x0c, 0x12},
{0xfd, 0x03},
{0x9d, 0x0f},
{0x9f, 0x20},
{0xfd, 0x05},
{0x00, 0x00},
{0x01, 0x1c},
{0x02, 0x01},
{0x03, 0xff},
{0x1a, 0x20},
{0xfd, 0x02},
{0xa0, 0x01},
{0xa1, 0x3a},
{0xa2, 0x07},
{0xa3, 0x2c},
{0xa5, 0x08},
{0xa6, 0x0c},
{0xa7, 0xc0},
{0xfd, 0x00},
{0x24, 0x01},
{0xc0, 0x17},
{0xc1, 0x08},
{0xc2, 0x30},
{0x8e, 0x0c},
{0x8f, 0xc0},
{0x90, 0x07},
{0x91, 0x2c},
{0xb7, 0x02},
{0xfd, 0x00},
{0xe7, 0x03},
{0xe7, 0x00},
{0x20, 0x0f},

};

static const SENSOR_REG_T ov08f10_preview_setting[] = {
{0xfd, 0x00},
{0x20, 0x0e},
{0xffff, 0x03},//delay 3ms by FAE
{0x20, 0x0b},
{0xfd, 0x00},
{0x11, 0x4e},
{0x12, 0x01},
{0x14, 0x90},
{0x1e, 0x10},
{0xfd, 0x01},
{0x12, 0x00},
{0x03, 0x05},
{0x04, 0x08},
{0x07, 0x05},
{0x24, 0xff},
{0x32, 0x03},
{0x33, 0x03},
{0x01, 0x03},
{0x19, 0x40},
{0xc0, 0x00},
{0x42, 0x5d},
{0x44, 0x81},
{0x46, 0x3f},
{0x47, 0x0f},
{0x48, 0x0c},
{0x4c, 0x18},
{0xb2, 0x1e},
{0xb3, 0x1b},
{0xc3, 0x2c},
{0xd2, 0xfc},
{0xfd, 0x01},
{0x50, 0x1c},
{0x53, 0x1c},
{0x54, 0x04},
{0x55, 0x04},
{0x57, 0x87},
{0x5e, 0x10},
{0x67, 0x05},
{0x7a, 0x02},
{0x7c, 0x0a},
{0x90, 0x3a},
{0x91, 0x12},
{0x92, 0x16},
{0x94, 0x12},
{0x95, 0x40},
{0x97, 0x03},
{0x98, 0x48},
{0x9a, 0x02},
{0x9c, 0x0c},
{0x9e, 0x2d},
{0xca, 0x0d},
{0xce, 0x0d},
{0xfd, 0x07},
{0x2c, 0x02},
{0x2d, 0x00},
{0x00, 0xf8},
{0x01, 0x2b},
{0x05, 0x40},
{0x08, 0x06},
{0x09, 0x11},
{0x28, 0x6f},
{0x2a, 0x20},
{0x2b, 0x05},
{0x5e, 0x10},
{0x52, 0x00},
{0x53, 0x78},
{0x54, 0x00},
{0x55, 0x78},
{0x56, 0x00},
{0x57, 0x78},
{0x58, 0x00},
{0x59, 0x78},
{0x5c, 0x3f},
{0xfd, 0x0f},
{0x00, 0x23},
{0x01, 0x01},
{0x02, 0x48},
{0x1a, 0x29},
{0x1b, 0x88},
{0x1c, 0x4c},
{0x1d, 0x9c},
{0x1f, 0x00},
{0x20, 0xcc},
{0xfd, 0x0f},
{0x03, 0x07},
{0x05, 0x0a},
{0x12, 0x00},
{0x13, 0x00},
{0x14, 0x13},
{0x08, 0x28},
{0x16, 0x0c},
{0x17, 0x0c},
{0x18, 0x0c},
{0x19, 0x0c},
{0xfd, 0x02},
{0x9a, 0x30},
{0xfd, 0x04},
{0x12, 0x01},
{0x0a, 0x10},
{0x0b, 0x11},
{0x0c, 0x12},
{0xfd, 0x03},
{0x9d, 0x0f},
{0x9f, 0x20},
{0xfd, 0x05},
{0x00, 0x00},
{0x01, 0x1c},
{0x02, 0x01},
{0x03, 0xff},
{0x1a, 0x20},
{0xfd, 0x02},
{0xa1, 0x08},
{0xa2, 0x09},
{0xa3, 0x90},
{0xa5, 0x08},
{0xa6, 0x0c},
{0xa7, 0xc0},
{0xfd, 0x00},
{0x24, 0x01},
{0xc0, 0x17},
{0xc1, 0x08},
{0xc2, 0x30},
{0x8e, 0x0c},
{0x8f, 0xc0},
{0x90, 0x09},
{0x91, 0x90},
{0xb7, 0x02},
{0xfd, 0x00},
{0xe7, 0x03},
{0xe7, 0x00},
{0x20, 0x0f},

};

static const SENSOR_REG_T ov08f10_snapshot_setting[] = {
{0xfd, 0x00},
{0x20, 0x0e},
{0xffff, 0x03},//delay 3ms by FAE
{0x20, 0x0b},
{0xfd, 0x00},
{0x11, 0x4e},
{0x12, 0x01},
{0x14, 0x90},
{0x1e, 0x10},
{0xfd, 0x01},
{0x12, 0x00},
{0x03, 0x05},
{0x04, 0x08},
{0x07, 0x05},
{0x24, 0xff},
{0x32, 0x03},
{0x33, 0x03},
{0x01, 0x03},
{0x19, 0x40},
{0xc0, 0x00},
{0x42, 0x5d},
{0x44, 0x81},
{0x46, 0x3f},
{0x47, 0x0f},
{0x48, 0x0c},
{0x4c, 0x18},
{0xb2, 0x1e},
{0xb3, 0x1b},
{0xc3, 0x2c},
{0xd2, 0xfc},
{0xfd, 0x01},
{0x50, 0x1c},
{0x53, 0x1c},
{0x54, 0x04},
{0x55, 0x04},
{0x57, 0x87},
{0x5e, 0x10},
{0x67, 0x05},
{0x7a, 0x02},
{0x7c, 0x0a},
{0x90, 0x3a},
{0x91, 0x12},
{0x92, 0x16},
{0x94, 0x12},
{0x95, 0x40},
{0x97, 0x03},
{0x98, 0x48},
{0x9a, 0x02},
{0x9c, 0x0c},
{0x9e, 0x2d},
{0xca, 0x0d},
{0xce, 0x0d},
{0xfd, 0x07},
{0x2c, 0x02},
{0x2d, 0x00},
{0x00, 0xf8},
{0x01, 0x2b},
{0x05, 0x40},
{0x08, 0x06},
{0x09, 0x11},
{0x28, 0x6f},
{0x2a, 0x20},
{0x2b, 0x05},
{0x5e, 0x10},
{0x52, 0x00},
{0x53, 0x78},
{0x54, 0x00},
{0x55, 0x78},
{0x56, 0x00},
{0x57, 0x78},
{0x58, 0x00},
{0x59, 0x78},
{0x5c, 0x3f},
{0xfd, 0x0f},
{0x00, 0x23},
{0x01, 0x01},
{0x02, 0x48},
{0x1a, 0x29},
{0x1b, 0x88},
{0x1c, 0x4c},
{0x1d, 0x9c},
{0x1f, 0x00},
{0x20, 0xcc},
{0xfd, 0x0f},
{0x03, 0x07},
{0x05, 0x0a},
{0x12, 0x00},
{0x13, 0x00},
{0x14, 0x13},
{0x08, 0x28},
{0x16, 0x0c},
{0x17, 0x0c},
{0x18, 0x0c},
{0x19, 0x0c},
{0xfd, 0x02},
{0x9a, 0x30},
{0xfd, 0x04},
{0x12, 0x01},
{0x0a, 0x10},
{0x0b, 0x11},
{0x0c, 0x12},
{0xfd, 0x03},
{0x9d, 0x0f},
{0x9f, 0x20},
{0xfd, 0x05},
{0x00, 0x00},
{0x01, 0x1c},
{0x02, 0x01},
{0x03, 0xff},
{0x1a, 0x20},
{0xfd, 0x02},
{0xa1, 0x08},
{0xa2, 0x09},
{0xa3, 0x90},
{0xa5, 0x08},
{0xa6, 0x0c},
{0xa7, 0xc0},
{0xfd, 0x00},
{0x24, 0x01},
{0xc0, 0x17},
{0xc1, 0x08},
{0xc2, 0x30},
{0x8e, 0x0c},
{0x8f, 0xc0},
{0x90, 0x09},
{0x91, 0x90},
{0xb7, 0x02},
{0xfd, 0x00},
{0xe7, 0x03},
{0xe7, 0x00},
{0x20, 0x0f},

	};

static struct sensor_res_tab_info s_ov08f10_resolution_tab_raw[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .reg_tab =
         {{ADDR_AND_LEN_OF_ARRAY(ov08f10_init_setting), PNULL, 0, .width = 0,
           .height = 0, .xclk_to_sensor = EX_MCLK,
           .image_format = SENSOR_IMAGE_FORMAT_RAW},

          {ADDR_AND_LEN_OF_ARRAY(ov08f10_video_setting), PNULL, 0,
           .width = VIDEO_WIDTH, .height = VIDEO_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

          {ADDR_AND_LEN_OF_ARRAY(ov08f10_preview_setting), PNULL, 0,
           .width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

          {ADDR_AND_LEN_OF_ARRAY(ov08f10_snapshot_setting), PNULL, 0,
           .width = SNAPSHOT_WIDTH, .height = SNAPSHOT_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW}}}

    /*If there are multiple modules,please add here*/
};

static SENSOR_TRIM_T s_ov08f10_resolution_trim_tab[VENDOR_NUM] = {
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

static SENSOR_REG_T ov08f10_shutter_reg[] = {
    {0xfd, 0x01}, //page1
	{0x02, 0x00}, 
	{0x03, 0x05},
	{0x04, 0x08},
	{0x01, 0x01}, //trigger the new exposure
};

static struct sensor_i2c_reg_tab ov08f10_shutter_tab = {
    .settings = ov08f10_shutter_reg, .size = ARRAY_SIZE(ov08f10_shutter_reg),
};

static SENSOR_REG_T ov08f10_again_reg[] = {
    {0xfd, 0x01}, //page1
    {0x24, 0x10},
    {0x01, 0x01}, //trigger the new gain
};

static struct sensor_i2c_reg_tab ov08f10_again_tab = {
    .settings = ov08f10_again_reg, .size = ARRAY_SIZE(ov08f10_again_reg),
};

static SENSOR_REG_T ov08f10_dgain_reg[] = {

};

static struct sensor_i2c_reg_tab ov08f10_dgain_tab = {
    .settings = ov08f10_dgain_reg, .size = ARRAY_SIZE(ov08f10_dgain_reg),
};

static SENSOR_REG_T ov08f10_frame_length_reg[] = {
    {0xfd, 0x01},//page1
	{0x05, 0x00},
    {0x06, 0x00},
    {0x01, 0x01},//trigger the new vblank
};

static struct sensor_i2c_reg_tab ov08f10_frame_length_tab = {
    .settings = ov08f10_frame_length_reg,
    .size = ARRAY_SIZE(ov08f10_frame_length_reg),
};

static SENSOR_REG_T ov08f10_grp_hold_start_reg[] = {

};

static struct sensor_i2c_reg_tab ov08f10_grp_hold_start_tab = {
    .settings = ov08f10_grp_hold_start_reg,
    .size = ARRAY_SIZE(ov08f10_grp_hold_start_reg),
};

static SENSOR_REG_T ov08f10_grp_hold_end_reg[] = {

};

static struct sensor_i2c_reg_tab ov08f10_grp_hold_end_tab = {
    .settings = ov08f10_grp_hold_end_reg,
    .size = ARRAY_SIZE(ov08f10_grp_hold_end_reg),
};

static struct sensor_aec_i2c_tag ov08f10_aec_info = {
    .slave_addr = (MAJOR_I2C_SLAVE_ADDR >> 1),
    .addr_bits_type = SENSOR_I2C_REG_8BIT,
    .data_bits_type = SENSOR_I2C_VAL_8BIT,
    .shutter = &ov08f10_shutter_tab,
    .again = &ov08f10_again_tab,
    .dgain = &ov08f10_dgain_tab,
    .frame_length = &ov08f10_frame_length_tab,
    .grp_hold_start = &ov08f10_grp_hold_start_tab,
    .grp_hold_end = &ov08f10_grp_hold_end_tab,
};

static SENSOR_STATIC_INFO_T s_ov08f10_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .static_info = {.f_num = 200,
                     .focal_length = 354,
                     .max_fps = 30,
                     .max_adgain = 15.5,
                     .ois_supported = 0,
                     .pdaf_supported = 0, // ov08f not support PD
                     .exp_valid_frame_num = 1,
                     .clamp_level = 64,
                     .adgain_valid_frame_num = 1,
                     .fov_info = {{3.673f, 2.759f}, 2.780f}}}
    /*If there are multiple modules,please add here*/
};

static SENSOR_MODE_FPS_INFO_T s_ov08f10_mode_fps_info[VENDOR_NUM] = {
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

static struct sensor_module_info s_ov08f10_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {.major_i2c_addr = MAJOR_I2C_SLAVE_ADDR >> 1,
                     .minor_i2c_addr = MINOR_I2C_SLAVE_ADDR >> 1,

                     .reg_addr_value_bits = SENSOR_I2C_REG_8BIT |
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
                             .is_loose = 0,
                         },
                     .change_setting_skip_num = 1,
                     .horizontal_view_angle = 65,
                     .vertical_view_angle = 60}}

    /*If there are multiple modules,please add here*/
};

static struct sensor_ic_ops s_ov08f10_ops_tab;
struct sensor_raw_info *s_ov08f10_mipi_raw_info_ptr = PNULL; //&s_ov08f10_mipi_raw_info;

/*==============================================================================
 * Description:
 * sensor all info
 * please modify this variable acording your spec
 *============================================================================*/
SENSOR_INFO_T g_ov08f10_mipi_raw_info = {
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
    .identify_code = {{.reg_addr = ov08f10_PID_ADDR,
                       .reg_value = ov08f10_PID_VALUE},
                      {.reg_addr = ov08f10_VER_ADDR,
                       .reg_value = ov08f10_VER_VALUE}},

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_RAW,

    .module_info_tab = s_ov08f10_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_ov08f10_module_info_tab),

    .resolution_tab_info_ptr = s_ov08f10_resolution_tab_raw,
    .sns_ops = &s_ov08f10_ops_tab,
    .raw_info_ptr = &s_ov08f10_mipi_raw_info_ptr,

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"ov08f10_v1",
};

#endif
