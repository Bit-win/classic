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

#ifndef _SENSOR_ov13853_MIPI_RAW_H_
#define _SENSOR_ov13853_MIPI_RAW_H_


#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

#include "parameters/sensor_ov13853_raw_param_main.c"

//#define FEATURE_OTP

#define VENDOR_NUM 1
#define SENSOR_NAME				"ov13853_s1_2lane"
#define I2C_SLAVE_ADDR			0x6C //0x6c 		/* 8bit slave address*/

#define ov13853_PID_ADDR			0x300A
#define ov13853_PID_VALUE			0xD8
#define ov13853_VER_ADDR			0x300B 
#define ov13853_VER_VALUE			0x53

/* sensor parameters begin */

/* effective sensor output image size */
#define VIDEO_WIDTH				1280
#define VIDEO_HEIGHT			720
#define PREVIEW_WIDTH			2096
#define PREVIEW_HEIGHT			1552
#define SNAPSHOT_WIDTH			4192 
#define SNAPSHOT_HEIGHT			3104

/*Raw Trim parameters*/
#define VIDEO_TRIM_X			0
#define VIDEO_TRIM_Y			0
#define VIDEO_TRIM_W			VIDEO_WIDTH
#define VIDEO_TRIM_H			VIDEO_HEIGHT
#define PREVIEW_TRIM_X			0
#define PREVIEW_TRIM_Y			0
#define PREVIEW_TRIM_W			PREVIEW_WIDTH
#define PREVIEW_TRIM_H			PREVIEW_HEIGHT
#define SNAPSHOT_TRIM_X			0
#define SNAPSHOT_TRIM_Y			0
#define SNAPSHOT_TRIM_W			SNAPSHOT_WIDTH
#define SNAPSHOT_TRIM_H			SNAPSHOT_HEIGHT

/*Mipi output*/
#define LANE_NUM			2
#define RAW_BITS			10

#define VIDEO_MIPI_PER_LANE_BPS	  	  600  /* 2*Mipi clk */
#define PREVIEW_MIPI_PER_LANE_BPS	  600  /* 2*Mipi clk */
#define SNAPSHOT_MIPI_PER_LANE_BPS	  1200  /* 2*Mipi clk */

/*line time unit: 1ns*/
#define VIDEO_LINE_TIME		  	  10000   
#define PREVIEW_LINE_TIME		  10000
#define SNAPSHOT_LINE_TIME		  10000

/* frame length*/
#define VIDEO_FRAME_LENGTH			3328
#define PREVIEW_FRAME_LENGTH		3328
#define SNAPSHOT_FRAME_LENGTH		3328

/* please ref your spec */
#define FRAME_OFFSET			8
cmr_u32 SENSOR_MAX_GAIN=0xf8;
cmr_u32  SENSOR_BASE_GAIN = 0x10;
#define SENSOR_MIN_SHUTTER		4

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
/* sensor parameters end */

/* isp parameters, please don't change it*/
#define ISP_BASE_GAIN			0x80

/* please don't change it */
#define EX_MCLK				24

/*==============================================================================
 * Description:
 * register setting
 *============================================================================*/

static const SENSOR_REG_T ov13853_init_setting[] = {
    /*mipi 1080Mbps,4224,3136*/
    // Address   value
{0x0103, 0x01},
{0x0102, 0x01},//
{0x0300, 0x01},//	;for 640Mbps
{0x0301, 0x00},//
{0x0302, 0x28},//
{0x0303, 0x00},//
{0x030a, 0x00},
{0x300f, 0x11},
{0x3010, 0x01},
{0x3011, 0x76},
{0x3012, 0x21},//lane num
{0x3013, 0x12},
{0x3014, 0x11},
{0x301f, 0x03},
{0x3106, 0x00},
{0x3210, 0x47},
{0x3500, 0x00},
{0x3501, 0x60},
{0x3502, 0x00},
{0x3506, 0x00},
{0x3507, 0x02},
{0x3508, 0x00},
{0x350a, 0x00},
{0x350b, 0x80},
{0x350e, 0x00},
{0x350f, 0x10},
{0x351a, 0x00},
{0x351b, 0x10},
{0x351c, 0x00},
{0x351d, 0x20},
{0x351e, 0x00},
{0x351f, 0x40},
{0x3520, 0x00},
{0x3521, 0x80},
{0x3600, 0xc0},
{0x3601, 0xfc},
{0x3602, 0x02},
{0x3603, 0x78},
{0x3604, 0xb1},
{0x3605, 0x95},
{0x3606, 0x73},
{0x3607, 0x07},
{0x3609, 0x40},
{0x360a, 0x30},
{0x360b, 0x91},
{0x360c, 0x09},
{0x360f, 0x02},
{0x3611, 0x10},
{0x3612, 0x27},
{0x3613, 0x33},
{0x3615, 0x0c},
{0x3616, 0x0e},
{0x3641, 0x02},
{0x3660, 0x82},
{0x3668, 0x54},
{0x3669, 0x00},
{0x366a, 0x3f},
{0x3667, 0xa0},
{0x3702, 0x40},
{0x3703, 0x44},
{0x3704, 0x2c},
{0x3705, 0x01},
{0x3706, 0x15},
{0x3707, 0x44},
{0x3708, 0x3c},
{0x3709, 0x1f},
{0x370a, 0x27},
{0x370b, 0x3c},
{0x3720, 0x55},
{0x3722, 0x84},
{0x3729, 0x00},
{0x3728, 0x40},
{0x372a, 0x00},
{0x372b, 0x02},
{0x372e, 0x22},
{0x372f, 0xa0},
{0x3730, 0x00},
{0x3731, 0x00},
{0x3732, 0x00},
{0x3733, 0x00},
{0x3710, 0x28},
{0x3716, 0x03},
{0x3718, 0x1c},
{0x3719, 0x0c},
{0x371a, 0x08},
{0x371c, 0xfc},
{0x3748, 0x00},
{0x3760, 0x13},
{0x3761, 0x33},
{0x3762, 0x86},
{0x3763, 0x16},
{0x3767, 0x24},
{0x3768, 0x06},
{0x3769, 0x45},
{0x376c, 0x23},
{0x376f, 0x80},
{0x3773, 0x06},
{0x3d84, 0x00},
{0x3d85, 0x17},
{0x3d8c, 0x73},
{0x3d8d, 0xbf},
{0x3800, 0x00},
{0x3801, 0x00},
{0x3802, 0x00},
{0x3803, 0x08},
{0x3804, 0x10},
{0x3805, 0x9f},
{0x3806, 0x0c},
{0x3807, 0x4f},
{0x3808, 0x08},
{0x3809, 0x40},
{0x380a, 0x06},
{0x380b, 0x20},
{0x380c, 0x09},
{0x380d, 0x60},
{0x380e, 0x0d},
{0x380f, 0x00},
{0x3810, 0x00},
{0x3811, 0x08},
{0x3812, 0x00},
{0x3813, 0x00},
{0x3814, 0x31},
{0x3815, 0x31},
{0x3820, 0x01},
{0x3821, 0x06},
{0x3823, 0x00},
{0x3826, 0x00},
{0x3827, 0x02},
{0x3834, 0x00},
{0x3835, 0x1c},
{0x3836, 0x08},
{0x3837, 0x02},
{0x4000, 0xf1},
{0x4001, 0x00},
{0x4005, 0x40},// add obtarget=0x40 
{0x400b, 0x0c},
{0x4011, 0x00},
{0x401a, 0x00},
{0x401b, 0x00},
{0x401c, 0x00},
{0x401d, 0x00},
{0x4020, 0x00},
{0x4021, 0xe4},
{0x4022, 0x04},
{0x4023, 0xd7},
{0x4024, 0x05},
{0x4025, 0xbc},
{0x4026, 0x05},
{0x4027, 0xbf},
{0x4028, 0x00},
{0x4029, 0x02},
{0x402a, 0x04},
{0x402b, 0x08},
{0x402c, 0x02},
{0x402d, 0x02},
{0x402e, 0x0c},
{0x402f, 0x08},
{0x403d, 0x2c},
{0x403f, 0x7f},
{0x4041, 0x07},
{0x4500, 0x82},
{0x4501, 0x78},//	;H digital skip for PD pixel; 3C for H digital bin
{0x458b, 0x00},
{0x459c, 0x00},
{0x459d, 0x00},
{0x459e, 0x00},
{0x4601, 0x83},
{0x4602, 0x22},
{0x4603, 0x01},
{0x4837, 0x19},
{0x4d00, 0x04},
{0x4d01, 0x42},
{0x4d02, 0xd1},
{0x4d03, 0x90},
{0x4d04, 0x66},
{0x4d05, 0x65},
{0x4d0b, 0x00},

{0x5001, 0x01},
{0x5002, 0x07},
{0x5003, 0x4f},
{0x5013, 0x40},
{0x501c, 0x00},
{0x501d, 0x10},
{0x510f, 0xfc},
{0x5110, 0xf0},
{0x5111, 0x10},
{0x5115, 0x3c},
{0x5116, 0x30},
{0x5117, 0x10},
{0x536d, 0x02},
{0x536e, 0x67},
{0x536f, 0x01},
{0x5370, 0x4c},
{0x5400, 0x00},
{0x5400, 0x00},
{0x5401, 0x61},
{0x5402, 0x00},
{0x5403, 0x00},
{0x5404, 0x00},
{0x5405, 0x40},
{0x540c, 0x05},
{0x5501, 0x00},
{0x5b00, 0x00},
{0x5b01, 0x00},
{0x5b02, 0x01},
{0x5b03, 0xff},
{0x5b04, 0x02},
{0x5b05, 0x6c},
{0x5b09, 0x02},
{0x5e00, 0x00},
{0x5e10, 0x1c},
{0x0100, 0x01},
{0x5b20, 0x02},
{0x5b21, 0x37},
{0x5b22, 0x0c},
{0x5b23, 0x37},
{0x5101, 0x00},
{0x5102, 0x00},
{0x5103, 0x00},
{0x5104, 0x00},
{0x5105, 0x00},
{0x5106, 0x00},

{0x510f, 0xfd},
{0x5110, 0xf0},
{0x5111, 0x10},

{0x5000, 0x0e},
{0x5100, 0x30},
{0x5107, 0x00},
{0x5108, 0x00},
{0x5109, 0x00},
{0x510e, 0x00},
//{0x0100, 0x0},
};

static const SENSOR_REG_T ov13853_preview_setting[] = {
   // {0x0100, 0x00}, //
	{0x0300, 0x01}, //	;for 640Mbps
	{0x0301, 0x00}, //
	{0x0302, 0x28}, //
	{0x0303, 0x00}, //
	{0x3501, 0x60}, // 
	{0x3612, 0x27}, // 
	{0x370a, 0x27}, // 
	{0x3729, 0x00}, // 
	{0x372a, 0x00}, // 
	{0x3718, 0x1c}, // 
	{0x372f, 0xa0}, // 
	{0x3801, 0x00}, // 
	{0x3802, 0x00}, //
	{0x3803, 0x08}, // 
	{0x3805, 0x9f}, // 
	{0x3806, 0x0c}, //
	{0x3807, 0x4f}, // 
	{0x3808, 0x08}, // 
	{0x3809, 0x40}, // 
	{0x380a, 0x06}, // 
	{0x380b, 0x20}, // 
	{0x380c, 0x09}, // 
	{0x380d, 0x60}, // 
	{0x380e, 0x0d}, // 
	{0x380f, 0x00}, // 
	//{0x380c, ((2400 >> 8) & 0xFF)}, // hts = 2688
    //{0x380d, (2400 & 0xFF)},        // hts
    //{0x380e, ((3328 >> 8) & 0xFF)},  // vts = 1984
    //{0x380f, (3328 & 0xFF)},         // vts
	{0x3810, 0x00}, // 
	{0x3811, 0x08}, // 
	{0x3813, 0x00}, // 0x02
	{0x3814, 0x31}, // 
	{0x3815, 0x31}, // 
	{0x3820, 0x01}, // 
	{0x3821, 0x06}, // 
	{0x3836, 0x08}, // 
	{0x3837, 0x02}, // 
	{0x4020, 0x00}, // 
	{0x4021, 0xe4}, // 
	{0x4022, 0x04}, // 
	{0x4023, 0xd7}, // 
	{0x4024, 0x05}, // 
	{0x4025, 0xbc}, // 
	{0x4026, 0x05}, // 
	{0x4027, 0xbf}, // 
	{0x4501, 0x78}, //	;H digital skip for PD pixel; 3C for H digital bin
	{0x4601, 0x83}, // 
	{0x4603, 0x01}, // 
	{0x4837, 0x19}, // 
	{0x5401, 0x61}, // 
	{0x5405, 0x40}, // 
	{0x5b21, 0x37}, // 
	{0x5b22, 0x0c}, // 
	{0x5b23, 0x37}, // 
	//{0x0100, 0x01}, //
};

static const SENSOR_REG_T ov13853_snapshot_setting[] = {
    /*4Lane
    Full (4:3) 29.95fps
        line time 10.38
		bps 1080Mbps/lan
    H: 4224
    V: 3136
    Output format Setting
        Address value*/
    //  {0x0100,0x00},
{0x0300, 0x00},
{0x0301, 0x00},
{0x0302, 0x32},
{0x0303, 0x00},
{0x3501, 0xc0},
{0x3612, 0x07},
{0x370a, 0x24},
{0x3729, 0x00},
{0x372a, 0x04},
{0x3718, 0x10},
{0x372f, 0xa0},
{0x3801, 0x0C},
{0x3802, 0x00},
{0x3803, 0x00},
{0x3805, 0x93},
{0x3806, 0x0c},
{0x3807, 0x4F},
{0x3808, 0x10},
{0x3809, 0x80},
{0x380a, 0x0c},
{0x380b, 0x40},
{0x380c, 0x12},//4800
{0x380d, 0xc0},
{0x380e, 0x0d},
{0x380f, 0x00},
{0x3810, 0x00},
{0x3811, 0x04},
{0x3813, 0x08},
{0x3814, 0x11},
{0x3815, 0x11},
{0x3820, 0x00},
{0x3821, 0x04},
{0x3836, 0x04},
{0x3837, 0x01},
{0x4020, 0x02},
{0x4021, 0x4C},
{0x4022, 0x0E},
{0x4023, 0x37},
{0x4024, 0x0F},
{0x4025, 0x1C},
{0x4026, 0x0F},
{0x4027, 0x1F},
{0x4501, 0x38},
{0x4601, 0x04},
{0x4603, 0x00},
{0x4837, 0x0d},
{0x5401, 0x71},
{0x5405, 0x80},
{0x5b21, 0x91},
{0x5b22, 0x02},
{0x5b23, 0x91},
    //    {0x0100,0x01},

};

static const SENSOR_REG_T ov13853_video_setting[] = {
    /*4Lane
    HV1/4 (4:3) 90fps
        line time 10.38
        bps 540Mpbs/lane
    H: 1280
    V: 720
    Output format Setting
        Address value*/
    //   {0x0100,0x00},
};

static struct sensor_res_tab_info s_ov13853_resolution_tab_raw[VENDOR_NUM] = {
	{
      .module_id = MODULE_SUNNY,
      .reg_tab = {
        {ADDR_AND_LEN_OF_ARRAY(ov13853_init_setting), PNULL, 0,
        .width = 0, .height = 0,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},
		
        {ADDR_AND_LEN_OF_ARRAY(ov13853_preview_setting), PNULL, 0,
        .width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

        {ADDR_AND_LEN_OF_ARRAY(ov13853_snapshot_setting), PNULL, 0,
        .width = SNAPSHOT_WIDTH, .height = SNAPSHOT_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW}
		}
	}

	/*If there are multiple modules,please add here*/
};

static SENSOR_TRIM_T s_ov13853_resolution_trim_tab[VENDOR_NUM] = {
	{
     .module_id = MODULE_SUNNY,
     .trim_info = {
       {0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0},0},
	   
	   {.trim_start_x = VIDEO_TRIM_X, .trim_start_y = VIDEO_TRIM_Y,
        .trim_width = VIDEO_TRIM_W,   .trim_height = VIDEO_TRIM_H,
        .line_time = VIDEO_LINE_TIME, .bps_per_lane = VIDEO_MIPI_PER_LANE_BPS,
        .frame_line = VIDEO_FRAME_LENGTH,
        .scaler_trim = {.x = VIDEO_TRIM_X, .y = VIDEO_TRIM_Y, .w = VIDEO_TRIM_W, .h = VIDEO_TRIM_H},0},
	   
	   {.trim_start_x = PREVIEW_TRIM_X, .trim_start_y = PREVIEW_TRIM_Y,
        .trim_width = PREVIEW_TRIM_W,   .trim_height = PREVIEW_TRIM_H,
        .line_time = PREVIEW_LINE_TIME, .bps_per_lane = PREVIEW_MIPI_PER_LANE_BPS,
        .frame_line = PREVIEW_FRAME_LENGTH,
        .scaler_trim = {.x = PREVIEW_TRIM_X, .y = PREVIEW_TRIM_Y, .w = PREVIEW_TRIM_W, .h = PREVIEW_TRIM_H},0},
       
	   {
        .trim_start_x = SNAPSHOT_TRIM_X, .trim_start_y = SNAPSHOT_TRIM_Y,
        .trim_width = SNAPSHOT_TRIM_W,   .trim_height = SNAPSHOT_TRIM_H,
        .line_time = SNAPSHOT_LINE_TIME, .bps_per_lane = SNAPSHOT_MIPI_PER_LANE_BPS,
        .frame_line = SNAPSHOT_FRAME_LENGTH,
        .scaler_trim = {.x = SNAPSHOT_TRIM_X, .y = SNAPSHOT_TRIM_Y, .w = SNAPSHOT_TRIM_W, .h = SNAPSHOT_TRIM_H},0},
		}
	}

    /*If there are multiple modules,please add here*/

};

static SENSOR_REG_T ov13853_shutter_reg[] = {
    {0x3502, 0x00}, 
	{0x3501, 0x00}, 
	{0x3500, 0x00},
};

static struct sensor_i2c_reg_tab ov13853_shutter_tab = {
    .settings = ov13853_shutter_reg, 
	.size = ARRAY_SIZE(ov13853_shutter_reg),
};

static SENSOR_REG_T ov13853_again_reg[] = {
	{0x350a, 0x00},
	{0x350b, 0x00},	
};

static struct sensor_i2c_reg_tab ov13853_again_tab = {
    .settings = ov13853_again_reg, 
	.size = ARRAY_SIZE(ov13853_again_reg),
};

static SENSOR_REG_T ov13853_dgain_reg[] = {
   
};

static struct sensor_i2c_reg_tab ov13853_dgain_tab = {
    .settings = ov13853_dgain_reg, 
	.size = ARRAY_SIZE(ov13853_dgain_reg),
};

static SENSOR_REG_T ov13853_frame_length_reg[] = {
    {0x380e, 0x0d}, 
	{0x380f, 0x00},
};

static struct sensor_i2c_reg_tab ov13853_frame_length_tab = {
    .settings = ov13853_frame_length_reg,
    .size = ARRAY_SIZE(ov13853_frame_length_reg),
};

static SENSOR_REG_T ov13853_grp_hold_start_reg[] = {

};

static struct sensor_i2c_reg_tab ov13853_grp_hold_start_tab = {
    .settings = ov13853_grp_hold_start_reg,
    .size = ARRAY_SIZE(ov13853_grp_hold_start_reg),
};

static SENSOR_REG_T ov13853_grp_hold_end_reg[] = {

};

static struct sensor_i2c_reg_tab ov13853_grp_hold_end_tab = {
    .settings = ov13853_grp_hold_end_reg,
    .size = ARRAY_SIZE(ov13853_grp_hold_end_reg),
};

static struct sensor_aec_i2c_tag ov13853_aec_info = {
    .slave_addr = (I2C_SLAVE_ADDR >> 1),
    .addr_bits_type = SENSOR_I2C_REG_16BIT,
    .data_bits_type = SENSOR_I2C_VAL_8BIT,
    .shutter = &ov13853_shutter_tab,
    .again = &ov13853_again_tab,
    .dgain = &ov13853_dgain_tab,
    .frame_length = &ov13853_frame_length_tab,
    .grp_hold_start = &ov13853_grp_hold_start_tab,
    .grp_hold_end = &ov13853_grp_hold_end_tab,
};


static SENSOR_STATIC_INFO_T s_ov13853_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
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
        .fov_info = {{4.614f, 3.444f}, 4.222f}}
    }
    /*If there are multiple modules,please add here*/
};

static SENSOR_MODE_FPS_INFO_T s_ov13853_mode_fps_info[VENDOR_NUM] = {
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

static struct sensor_module_info s_ov13853_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {
         .major_i2c_addr = I2C_SLAVE_ADDR >> 1,
         .minor_i2c_addr = I2C_SLAVE_ADDR >> 1,

         .reg_addr_value_bits = SENSOR_I2C_REG_16BIT | SENSOR_I2C_VAL_8BIT |
                                SENSOR_I2C_FREQ_400,

         .avdd_val = SENSOR_AVDD_2800MV,
         .iovdd_val = SENSOR_AVDD_1800MV,
         .dvdd_val = SENSOR_AVDD_1200MV,

         .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_B,

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

static struct sensor_ic_ops s_ov13853_ops_tab;
struct sensor_raw_info *s_ov13853_mipi_raw_info_ptr = &s_ov13853_mipi_raw_info;


/*==============================================================================
 * Description:
 * sensor all info
 * please modify this variable acording your spec
 *============================================================================*/
SENSOR_INFO_T g_ov13853_mipi_raw_info = {
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
        {{ .reg_addr = ov13853_PID_ADDR, .reg_value = ov13853_PID_VALUE},
         { .reg_addr = ov13853_VER_ADDR, .reg_value = ov13853_VER_VALUE}},

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_RAW,

    .module_info_tab = s_ov13853_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_ov13853_module_info_tab),

    .resolution_tab_info_ptr = s_ov13853_resolution_tab_raw,
    .sns_ops = &s_ov13853_ops_tab,
    .raw_info_ptr = &s_ov13853_mipi_raw_info_ptr,

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"ov13853_s1",
};

#endif
