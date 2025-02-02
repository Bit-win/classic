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

#ifndef _SENSOR_GC08A8_MIPI_RAW_H_
#define _SENSOR_GC08A8_MIPI_RAW_H_


#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

// #include "parameters/sensor_gc08a8_raw_param_main.c"

#define VENDOR_NUM 1
#define SENSOR_NAME				"c3z03_gc08a8_frontcxt"

/* please check layout
* Pin IDSEL1 = 0, IDSEL0 = 0, I2C_SLAVE_ADDR = 0x62;
* Pin IDSEL1 = 0, IDSEL0 = 1, I2C_SLAVE_ADDR = 0x20;
* Pin IDSEL1 = 1, IDSEL0 = 0, I2C_SLAVE_ADDR = 0x22;
* Pin IDSEL1 = 1, IDSEL0 = 1, I2C_SLAVE_ADDR = 0x24;
*/

#define I2C_SLAVE_ADDR			0x62 		/* 8bit slave address*/

#define GC08A8_PID_ADDR			0x03f0
#define GC08A8_PID_VALUE		0x08
#define GC08A8_VER_ADDR			0x03f1
#define GC08A8_VER_VALUE		0xa8

/* sensor parameters begin */

/* effective sensor output image size */
#define PREVIEW_WIDTH			3264	
#define PREVIEW_HEIGHT			2448
#define VIDEO_WIDTH			    3264	
#define VIDEO_HEIGHT			1836
#define SNAPSHOT_WIDTH			3264	
#define SNAPSHOT_HEIGHT			2448
#define CUSTOM1_WIDTH			1632	
#define CUSTOM1_HEIGHT			1224

/*Raw Trim parameters*/
#define PREVIEW_TRIM_X			0
#define PREVIEW_TRIM_Y			0
#define PREVIEW_TRIM_W			3264
#define PREVIEW_TRIM_H			2448
#define VIDEO_TRIM_X			0
#define VIDEO_TRIM_Y			0
#define VIDEO_TRIM_W			3264
#define VIDEO_TRIM_H			1836
#define SNAPSHOT_TRIM_X			0
#define SNAPSHOT_TRIM_Y			0
#define SNAPSHOT_TRIM_W			3264	
#define SNAPSHOT_TRIM_H			2448
#define CUSTOM1_TRIM_X			0
#define CUSTOM1_TRIM_Y			0
#define CUSTOM1_TRIM_W			1632
#define CUSTOM1_TRIM_H			1224

/*Mipi output*/
#define LANE_NUM			4
#define RAW_BITS			10

#define PREVIEW_MIPI_PER_LANE_BPS	  672  /* 2*Mipi clk */
#define VIDEO_MIPI_PER_LANE_BPS	      672  /* 2*Mipi clk */
#define SNAPSHOT_MIPI_PER_LANE_BPS	  672  /* 2*Mipi clk */
#define CUSTOM1_MIPI_PER_LANE_BPS	  342  /* 2*Mipi clk */

/*line time unit: 1ns*/
#define PREVIEW_LINE_TIME		  13000
#define VIDEO_LINE_TIME		      13000
#define SNAPSHOT_LINE_TIME		  13000
#define CUSTOM1_LINE_TIME		  13000

/* frame length*/
#define PREVIEW_FRAME_LENGTH		2548
#define VIDEO_FRAME_LENGTH		    2548
#define SNAPSHOT_FRAME_LENGTH		2548
#define CUSTOM1_FRAME_LENGTH		2552

/* please ref your spec */
#define FRAME_OFFSET			16
#define SENSOR_MAX_GAIN			0x4000 //16x gain
#define SENSOR_BASE_GAIN		0x400
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

/* SENSOR MIRROR FLIP INFO */
#define GC08A8_MIRROR_NORMAL    0
#define GC08A8_MIRROR_H         0
#define GC08A8_MIRROR_V         0
#define GC08A8_MIRROR_HV        1

#if GC08A8_MIRROR_NORMAL
#define GC08A8_MIRROR	0x00
#elif GC08A8_MIRROR_H
#define GC08A8_MIRROR	0x01
#elif GC08A8_MIRROR_V
#define GC08A8_MIRROR	0x02
#elif GC08A8_MIRROR_HV
#define GC08A8_MIRROR	0x03
#else
#define GC08A8_MIRROR	0x00
#endif

/*==============================================================================
 * Description:
 * register setting
 *============================================================================*/

static const SENSOR_REG_T gc08a8_init_setting[] = {
    /*system*/
    {0x031c, 0x60},
    {0x0337, 0x04},
    {0x0335, 0x51},
    {0x0336, 0x70},
    {0x0383, 0xbb},
    {0x031a, 0x00},
    {0x0321, 0x10},
    {0x0327, 0x03},
    {0x0325, 0x40},
    {0x0326, 0x23},
    {0x0314, 0x11},
    {0x0315, 0xd6},
    {0x0316, 0x01},
    {0x0334, 0x40},
    {0x0324, 0x42},
    {0x031c, 0x00},
    {0x031c, 0x9f},
    {0x039a, 0x43},
    {0x0084, 0x30},
    {0x02b3, 0x08},
    {0x0057, 0x0c},
    {0x05c3, 0x50},
    {0x0311, 0x90},
    {0x05a0, 0x02},
    {0x0074, 0x0a},
    {0x0059, 0x11},
    {0x0070, 0x05},
    {0x0101, GC08A8_MIRROR},
    {0x0344, 0x00},
    {0x0345, 0x06},
    {0x0346, 0x00},
    {0x0347, 0x04},
    {0x0348, 0x0c},
    {0x0349, 0xd0},
    {0x034a, 0x09},
    {0x034b, 0x9c},
    {0x0202, 0x09},
    {0x0203, 0x04},
    {0x0340, 0x09},
    {0x0341, 0xf4},
    {0x0342, 0x07},
    {0x0343, 0x1c},
    {0x0219, 0x05},
    {0x0226, 0x00},
    {0x0227, 0x28},
    {0x0e0a, 0x00},
    {0x0e0b, 0x00},
    {0x0e24, 0x04},
    {0x0e25, 0x04},
    {0x0e26, 0x00},
    {0x0e27, 0x10},
    {0x0e01, 0x74},
    {0x0e03, 0x47},
    {0x0e04, 0x33},
    {0x0e05, 0x44},
    {0x0e06, 0x44},
    {0x0e0c, 0x1e},
    {0x0e17, 0x3a},
    {0x0e18, 0x3c},
    {0x0e19, 0x40},
    {0x0e1a, 0x42},
    {0x0e28, 0x21},
    {0x0e2b, 0x68},
    {0x0e2c, 0x0d},
    {0x0e2d, 0x08},
    {0x0e34, 0xf4},
    {0x0e35, 0x44},
    {0x0e36, 0x07},
    {0x0e38, 0x39},
    {0x0210, 0x13},
    {0x0218, 0x00},
    {0x0241, 0x88},
    {0x0e32, 0x00},
    {0x0e33, 0x18},
    {0x0e42, 0x03},
    {0x0e43, 0x80},
    {0x0e44, 0x04},
    {0x0e45, 0x00},
    {0x0e4f, 0x04},
    {0x057a, 0x20},
    {0x0381, 0x7c},
    {0x0382, 0x9b},
    {0x0384, 0xfb},
    {0x0389, 0x38},
    {0x038a, 0x03},
    {0x0390, 0x6a},
    {0x0391, 0x0f},
    {0x0392, 0x60},
    {0x0393, 0xc1},
    {0x0396, 0x3f},
    {0x0398, 0x22},
    {0x031c, 0x80},
    {0x03fe, 0x10},
    {0x03fe, 0x00},
    {0x031c, 0x9f},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x031c, 0x80},
    {0x03fe, 0x10},
    {0x03fe, 0x00},
    {0x031c, 0x9f},
    {0x0360, 0x01},
    {0x0360, 0x00},
    {0x0316, 0x09},
    {0x0a67, 0x80},
    {0x0313, 0x00},
    {0x0a53, 0x0e},
    {0x0a65, 0x17},
    {0x0a68, 0xa1},
    {0x0a58, 0x00},
    {0x0ace, 0x0c},
    {0x00a4, 0x00},
    {0x00a5, 0x01},
    {0x00a7, 0x09},
    {0x00a8, 0x9c},
    {0x00a9, 0x0c},
    {0x00aa, 0xd0},
    {0x0a8a, 0x00},
    {0x0a8b, 0xe0},
    {0x0a8c, 0x13},
    {0x0a8d, 0xe8},
    {0x0a90, 0x0a},
    {0x0a91, 0x10},
    {0x0a92, 0xf8},
    {0x0a71, 0xf2},
    {0x0a72, 0x12},
    {0x0a73, 0x64},
    {0x0a75, 0x41},
    {0x0a70, 0x07},
    {0x0313, 0x80},
    {0x00a0, 0x01},
    {0x0080, 0xd2},
    {0x0081, 0x3f},
    {0x0087, 0x51},
    {0x0089, 0x03},
    {0x009b, 0x40},
    {0x0096, 0x81},
    {0x0097, 0x08},
    {0x05a0, 0x82},
    {0x05ac, 0x00},
    {0x05ad, 0x01},
    {0x05ae, 0x00},
    {0x0800, 0x0a},
    {0x0801, 0x14},
    {0x0802, 0x28},
    {0x0803, 0x34},
    {0x0804, 0x0e},
    {0x0805, 0x33},
    {0x0806, 0x03},
    {0x0807, 0x8a},
    {0x0808, 0x3e},
    {0x0809, 0x00},
    {0x080a, 0x28},
    {0x080b, 0x03},
    {0x080c, 0x1d},
    {0x080d, 0x03},
    {0x080e, 0x16},
    {0x080f, 0x03},
    {0x0810, 0x10},
    {0x0811, 0x03},
    {0x0812, 0x00},
    {0x0813, 0x00},
    {0x0814, 0x01},
    {0x0815, 0x00},
    {0x0816, 0x01},
    {0x0817, 0x00},
    {0x0818, 0x00},
    {0x0819, 0x0a},
    {0x081a, 0x01},
    {0x081b, 0x6c},
    {0x081c, 0x00},
    {0x081d, 0x0b},
    {0x081e, 0x02},
    {0x081f, 0x00},
    {0x0820, 0x00},
    {0x0821, 0x0c},
    {0x0822, 0x02},
    {0x0823, 0xd9},
    {0x0824, 0x00},
    {0x0825, 0x0d},
    {0x0826, 0x03},
    {0x0827, 0xf0},
    {0x0828, 0x00},
    {0x0829, 0x0e},
    {0x082a, 0x05},
    {0x082b, 0x94},
    {0x082c, 0x09},
    {0x082d, 0x6e},
    {0x082e, 0x07},
    {0x082f, 0xe6},
    {0x0830, 0x10},
    {0x0831, 0x0e},
    {0x0832, 0x0b},
    {0x0833, 0x2c},
    {0x0834, 0x14},
    {0x0835, 0xae},
    {0x0836, 0x0f},
    {0x0837, 0xc4},
    {0x0838, 0x18},
    {0x0839, 0x0e},
    {0x05ac, 0x01},
    {0x059a, 0x00},
    {0x059b, 0x00},
    {0x059c, 0x01},
    {0x0598, 0x00},
    {0x0597, 0x14},
    {0x05ab, 0x09},
    {0x05a4, 0x02},
    {0x05a3, 0x05},
    {0x05a0, 0xc2},
    {0x0207, 0xc4},
    {0x0208, 0x01},
    {0x0209, 0x78},
    {0x0204, 0x04},
    {0x0205, 0x00},
    {0x0040, 0x22},
    {0x0041, 0x20},
    {0x0043, 0x10},
    {0x0044, 0x00},
    {0x0046, 0x08},
    {0x0047, 0xf0},
    {0x0048, 0x0f},
    {0x004b, 0x0f},
    {0x004c, 0x00},
    {0x0050, 0x5c},
    {0x0051, 0x44},
    {0x005b, 0x03},
    {0x00c0, 0x00},
    {0x00c1, 0x80},
    {0x00c2, 0x31},
    {0x00c3, 0x00},
    {0x0460, 0x04},
    {0x0462, 0x08},
    {0x0464, 0x0e},
    {0x0466, 0x0a},
    {0x0468, 0x12},
    {0x046a, 0x12},
    {0x046c, 0x10},
    {0x046e, 0x0c},
    {0x0461, 0x03},
    {0x0463, 0x03},
    {0x0465, 0x03},
    {0x0467, 0x03},
    {0x0469, 0x04},
    {0x046b, 0x04},
    {0x046d, 0x04},
    {0x046f, 0x04},
    {0x0470, 0x04},
    {0x0472, 0x10},
    {0x0474, 0x26},
    {0x0476, 0x38},
    {0x0478, 0x20},
    {0x047a, 0x30},
    {0x047c, 0x38},
    {0x047e, 0x60},
    {0x0471, 0x05},
    {0x0473, 0x05},
    {0x0475, 0x05},
    {0x0477, 0x05},
    {0x0479, 0x04},
    {0x047b, 0x04},
    {0x047d, 0x04},
    {0x047f, 0x04},


};

static const SENSOR_REG_T gc08a8_1632x1224_setting[] = {
	   /*system*/
    {0x031c, 0x60},
    {0x0337, 0x04},
    {0x0335, 0x55},
    {0x0336, 0x72},
    {0x0383, 0xbb},
    {0x031a, 0x00},
    {0x0321, 0x10},
    {0x0327, 0x03},
    {0x0325, 0x40},
    {0x0326, 0x23},
    {0x0314, 0x11},
    {0x0315, 0xd6},
    {0x0316, 0x01},
    {0x0334, 0x40},
    {0x0324, 0x42},
    {0x031c, 0x00},
    {0x031c, 0x9f},
    {0x0344, 0x00},
    {0x0345, 0x06},
    {0x0346, 0x00},
    {0x0347, 0x04},
    {0x0348, 0x0c},
    {0x0349, 0xd0},
    {0x034a, 0x09},
    {0x034b, 0x9c},
    {0x0202, 0x03},
    {0x0203, 0x00},
    {0x0340, 0x09},
    {0x0341, 0xf8},  
    {0x0342, 0x07},
    {0x0343, 0x1c},
    {0x0226, 0x00},
    {0x0227, 0x06},
    {0x0e38, 0x39},
    {0x0210, 0x53},
    {0x0218, 0x80},
    {0x0241, 0x8c},
    {0x0392, 0x3b},
    {0x031c, 0x80},
    {0x03fe, 0x10},
    {0x03fe, 0x00},
    {0x031c, 0x9f},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x031c, 0x80},
    {0x03fe, 0x10},
    {0x03fe, 0x00},
    {0x031c, 0x9f},
    {0x00a2, 0x00},
    {0x00a3, 0x00},
    {0x00ab, 0x00},
    {0x00ac, 0x00},
    {0x05a0, 0x82},
    {0x05ac, 0x00},
    {0x05ad, 0x01},
    {0x05ae, 0x00},
    {0x0800, 0x0a},
    {0x0801, 0x14},
    {0x0802, 0x28},
    {0x0803, 0x34},
    {0x0804, 0x0e},
    {0x0805, 0x33},
    {0x0806, 0x03},
    {0x0807, 0x8a},
    {0x0808, 0x3e},
    {0x0809, 0x00},
    {0x080a, 0x28},
    {0x080b, 0x03},
    {0x080c, 0x1d},
    {0x080d, 0x03},
    {0x080e, 0x16},
    {0x080f, 0x03},
    {0x0810, 0x10},
    {0x0811, 0x03},
    {0x0812, 0x00},
    {0x0813, 0x00},
    {0x0814, 0x01},
    {0x0815, 0x00},
    {0x0816, 0x01},
    {0x0817, 0x00},
    {0x0818, 0x00},
    {0x0819, 0x0a},
    {0x081a, 0x01},
    {0x081b, 0x6c},
    {0x081c, 0x00},
    {0x081d, 0x0b},
    {0x081e, 0x02},
    {0x081f, 0x00},
    {0x0820, 0x00},
    {0x0821, 0x0c},
    {0x0822, 0x02},
    {0x0823, 0xd9},
    {0x0824, 0x00},
    {0x0825, 0x0d},
    {0x0826, 0x03},
    {0x0827, 0xf0},
    {0x0828, 0x00},
    {0x0829, 0x0e},
    {0x082a, 0x05},
    {0x082b, 0x94},
    {0x082c, 0x09},
    {0x082d, 0x6e},
    {0x082e, 0x07},
    {0x082f, 0xe6},
    {0x0830, 0x10},
    {0x0831, 0x0e},
    {0x0832, 0x0b},
    {0x0833, 0x2c},
    {0x0834, 0x14},
    {0x0835, 0xae},
    {0x0836, 0x0f},
    {0x0837, 0xc4},
    {0x0838, 0x18},
    {0x0839, 0x0e},
    {0x05ac, 0x01},
    {0x059a, 0x00},
    {0x059b, 0x00},
    {0x059c, 0x01},
    {0x0598, 0x00},
    {0x0597, 0x14},
    {0x05ab, 0x09},
    {0x05a4, 0x02},
    {0x05a3, 0x05},
    {0x05a0, 0xc2},
    {0x0207, 0xc4},
    {0x0204, 0x04},
    {0x0205, 0x00},
    {0x0050, 0x5c},
    {0x0051, 0x44},
    {0x009a, 0x00},
    {0x0351, 0x00},
    {0x0352, 0x04},
    {0x0353, 0x00},
    {0x0354, 0x04},
    {0x034c, 0x06},
    {0x034d, 0x60},
    {0x034e, 0x04},
    {0x034f, 0xc8},
    {0x0114, 0x03},
    {0x0180, 0x67},
    {0x0181, 0xf0},
    {0x0185, 0x01},
    {0x0115, 0x30},
    {0x011b, 0x12},
    {0x011c, 0x12},
    {0x0121, 0x02},
    {0x0122, 0x03},
    {0x0123, 0x09},
    {0x0124, 0x00},
    {0x0125, 0x16},
    {0x0126, 0x05},
    {0x0129, 0x03},
    {0x012a, 0x02},
    {0x012b, 0x05},
    {0x0a73, 0x60},
    {0x0a70, 0x11},
    {0x0313, 0x80},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0a70, 0x00},
    {0x00a4, 0x80},
    {0x0316, 0x01},
    {0x0a67, 0x00},
    {0x0084, 0x10},
    {0x0102, 0x09},

};

static const SENSOR_REG_T gc08a8_3264x2448_setting[] = {
    {0x031c, 0x60},
    {0x0337, 0x04},
    {0x0335, 0x51},
    {0x0336, 0x70},
    {0x0383, 0xbb},
    {0x031a, 0x00},
    {0x0321, 0x10},
    {0x0327, 0x03},
    {0x0325, 0x40},
    {0x0326, 0x23},
    {0x0314, 0x11},
    {0x0315, 0xd6},
    {0x0316, 0x01},
    {0x0334, 0x40},
    {0x0324, 0x42},
    {0x031c, 0x00},
    {0x031c, 0x9f},
    {0x0344, 0x00},
    {0x0345, 0x06},
    {0x0346, 0x00},
    {0x0347, 0x04},
    {0x0348, 0x0c},
    {0x0349, 0xd0},
    {0x034a, 0x09},
    {0x034b, 0x9c},
    {0x0202, 0x09},
    {0x0203, 0x04},
    {0x0340, 0x09},
    {0x0341, 0xf4},
    {0x0342, 0x07},
    {0x0343, 0x1c},
    {0x0226, 0x00},
    {0x0227, 0x28},
    {0x0e38, 0x39},
    {0x0210, 0x13},
    {0x0218, 0x00},
    {0x0241, 0x88},
    {0x0392, 0x60},
    {0x031c, 0x80},
    {0x03fe, 0x10},
    {0x03fe, 0x00},
    {0x031c, 0x9f},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x031c, 0x80},
    {0x03fe, 0x10},
    {0x03fe, 0x00},
    {0x031c, 0x9f},
    {0x00a2, 0x00},
    {0x00a3, 0x00},
    {0x00ab, 0x00},
    {0x00ac, 0x00},
    {0x05a0, 0x82},
    {0x05ac, 0x00},
    {0x05ad, 0x01},
    {0x05ae, 0x00},
    {0x0800, 0x0a},
    {0x0801, 0x14},
    {0x0802, 0x28},
    {0x0803, 0x34},
    {0x0804, 0x0e},
    {0x0805, 0x33},
    {0x0806, 0x03},
    {0x0807, 0x8a},
    {0x0808, 0x3e},
    {0x0809, 0x00},
    {0x080a, 0x28},
    {0x080b, 0x03},
    {0x080c, 0x1d},
    {0x080d, 0x03},
    {0x080e, 0x16},
    {0x080f, 0x03},
    {0x0810, 0x10},
    {0x0811, 0x03},
    {0x0812, 0x00},
    {0x0813, 0x00},
    {0x0814, 0x01},
    {0x0815, 0x00},
    {0x0816, 0x01},
    {0x0817, 0x00},
    {0x0818, 0x00},
    {0x0819, 0x0a},
    {0x081a, 0x01},
    {0x081b, 0x6c},
    {0x081c, 0x00},
    {0x081d, 0x0b},
    {0x081e, 0x02},
    {0x081f, 0x00},
    {0x0820, 0x00},
    {0x0821, 0x0c},
    {0x0822, 0x02},
    {0x0823, 0xd9},
    {0x0824, 0x00},
    {0x0825, 0x0d},
    {0x0826, 0x03},
    {0x0827, 0xf0},
    {0x0828, 0x00},
    {0x0829, 0x0e},
    {0x082a, 0x05},
    {0x082b, 0x94},
    {0x082c, 0x09},
    {0x082d, 0x6e},
    {0x082e, 0x07},
    {0x082f, 0xe6},
    {0x0830, 0x10},
    {0x0831, 0x0e},
    {0x0832, 0x0b},
    {0x0833, 0x2c},
    {0x0834, 0x14},
    {0x0835, 0xae},
    {0x0836, 0x0f},
    {0x0837, 0xc4},
    {0x0838, 0x18},
    {0x0839, 0x0e},
    {0x05ac, 0x01},
    {0x059a, 0x00},
    {0x059b, 0x00},
    {0x059c, 0x01},
    {0x0598, 0x00},
    {0x0597, 0x14},
    {0x05ab, 0x09},
    {0x05a4, 0x02},
    {0x05a3, 0x05},
    {0x05a0, 0xc2},
    {0x0207, 0xc4},
    {0x0204, 0x04},
    {0x0205, 0x00},
    {0x0050, 0x5c},
    {0x0051, 0x44},
    {0x009a, 0x00},
    {0x0351, 0x00},
    {0x0352, 0x06},
    {0x0353, 0x00},
    {0x0354, 0x08},
    {0x034c, 0x0c},
    {0x034d, 0xc0},
    {0x034e, 0x09},
    {0x034f, 0x90},
    {0x0114, 0x03},
    {0x0180, 0x67},
    {0x0181, 0xf0},
    {0x0185, 0x01},
    {0x0115, 0x30},
    {0x011b, 0x12},
    {0x011c, 0x12},
    {0x0121, 0x06},
    {0x0122, 0x06},
    {0x0123, 0x15},
    {0x0124, 0x01},
    {0x0125, 0x13},
    {0x0126, 0x08},
    {0x0129, 0x06},
    {0x012a, 0x08},
    {0x012b, 0x08},
    {0x0a73, 0x60},
    {0x0a70, 0x11},
    {0x0313, 0x80},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0a70, 0x00},
    {0x00a4, 0x80},
    {0x0316, 0x01},
    {0x0a67, 0x00},
    {0x0084, 0x10},
    {0x0102, 0x09},

};

static const SENSOR_REG_T gc08a8_3264x1836_setting[] = {
    {0x031c, 0x60},
    {0x0337, 0x04},
    {0x0335, 0x51},
    {0x0336, 0x70},
    {0x0383, 0xbb},
    {0x031a, 0x00},
    {0x0321, 0x10},
    {0x0327, 0x03},
    {0x0325, 0x40},
    {0x0326, 0x23},
    {0x0314, 0x11},
    {0x0315, 0xd6},
    {0x0316, 0x01},
    {0x0334, 0x40},
    {0x0324, 0x42},
    {0x031c, 0x00},
    {0x031c, 0x9f},
    {0x0344, 0x00},
    {0x0345, 0x06},
    {0x0346, 0x01},
    {0x0347, 0x36},
    {0x0348, 0x0c},
    {0x0349, 0xd0},
    {0x034a, 0x07},
    {0x034b, 0x38},
    {0x0202, 0x09},
    {0x0203, 0x04},
    {0x0340, 0x09},
    {0x0341, 0xf4},
    {0x0342, 0x07},
    {0x0343, 0x1c},
    {0x0226, 0x02},
    {0x0227, 0x8c},
    {0x0e38, 0x39},
    {0x0210, 0x13},
    {0x0218, 0x00},
    {0x0241, 0x88},
    {0x0392, 0x60},
    {0x031c, 0x80},
    {0x03fe, 0x10},
    {0x03fe, 0x00},
    {0x031c, 0x9f},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x03fe, 0x00},
    {0x031c, 0x80},
    {0x03fe, 0x10},
    {0x03fe, 0x00},
    {0x031c, 0x9f},
    {0x00a2, 0x32},
    {0x00a3, 0x01},
    {0x00ab, 0x00},
    {0x00ac, 0x00},
    {0x05a0, 0x82},
    {0x05ac, 0x00},
    {0x05ad, 0x01},
    {0x05ae, 0x00},
    {0x0800, 0x0a},
    {0x0801, 0x14},
    {0x0802, 0x28},
    {0x0803, 0x34},
    {0x0804, 0x0e},
    {0x0805, 0x33},
    {0x0806, 0x03},
    {0x0807, 0x8a},
    {0x0808, 0x3e},
    {0x0809, 0x00},
    {0x080a, 0x28},
    {0x080b, 0x03},
    {0x080c, 0x1d},
    {0x080d, 0x03},
    {0x080e, 0x16},
    {0x080f, 0x03},
    {0x0810, 0x10},
    {0x0811, 0x03},
    {0x0812, 0x00},
    {0x0813, 0x00},
    {0x0814, 0x01},
    {0x0815, 0x00},
    {0x0816, 0x01},
    {0x0817, 0x00},
    {0x0818, 0x00},
    {0x0819, 0x0a},
    {0x081a, 0x01},
    {0x081b, 0x6c},
    {0x081c, 0x00},
    {0x081d, 0x0b},
    {0x081e, 0x02},
    {0x081f, 0x00},
    {0x0820, 0x00},
    {0x0821, 0x0c},
    {0x0822, 0x02},
    {0x0823, 0xd9},
    {0x0824, 0x00},
    {0x0825, 0x0d},
    {0x0826, 0x03},
    {0x0827, 0xf0},
    {0x0828, 0x00},
    {0x0829, 0x0e},
    {0x082a, 0x05},
    {0x082b, 0x94},
    {0x082c, 0x09},
    {0x082d, 0x6e},
    {0x082e, 0x07},
    {0x082f, 0xe6},
    {0x0830, 0x10},
    {0x0831, 0x0e},
    {0x0832, 0x0b},
    {0x0833, 0x2c},
    {0x0834, 0x14},
    {0x0835, 0xae},
    {0x0836, 0x0f},
    {0x0837, 0xc4},
    {0x0838, 0x18},
    {0x0839, 0x0e},
    {0x05ac, 0x01},
    {0x059a, 0x00},
    {0x059b, 0x00},
    {0x059c, 0x01},
    {0x0598, 0x00},
    {0x0597, 0x14},
    {0x05ab, 0x09},
    {0x05a4, 0x02},
    {0x05a3, 0x05},
    {0x05a0, 0xc2},
    {0x0207, 0xc4},
    {0x0204, 0x04},
    {0x0205, 0x00},
    {0x0050, 0x5c},
    {0x0051, 0x44},
    {0x009a, 0x00},
    {0x0351, 0x00},
    {0x0352, 0x06},
    {0x0353, 0x00},
    {0x0354, 0x08},
    {0x034c, 0x0c},
    {0x034d, 0xc0},
    {0x034e, 0x07},
    {0x034f, 0x2c},
    {0x0114, 0x03},
    {0x0180, 0x67},
    {0x0181, 0xf0},
    {0x0185, 0x01},
    {0x0115, 0x30},
    {0x011b, 0x12},
    {0x011c, 0x12},
    {0x0121, 0x06},
    {0x0122, 0x06},
    {0x0123, 0x15},
    {0x0124, 0x01},
    {0x0125, 0x13},
    {0x0126, 0x08},
    {0x0129, 0x06},
    {0x012a, 0x08},
    {0x012b, 0x08},
    {0x0a73, 0x60},
    {0x0a70, 0x11},
    {0x0313, 0x80},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0aff, 0x00},
    {0x0a70, 0x00},
    {0x00a4, 0x80},
    {0x0316, 0x01},
    {0x0a67, 0x00},
    {0x0084, 0x10},
    {0x0102, 0x09},



};

static struct sensor_res_tab_info s_gc08a8_resolution_tab_raw[VENDOR_NUM] = {
	{
      .module_id = MODULE_SUNNY,
      .reg_tab = {
        {ADDR_AND_LEN_OF_ARRAY(gc08a8_init_setting), PNULL, 0,
        .width = 0, .height = 0,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

        {ADDR_AND_LEN_OF_ARRAY(gc08a8_3264x1836_setting), PNULL, 0,
        .width = VIDEO_WIDTH, .height = VIDEO_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

        {ADDR_AND_LEN_OF_ARRAY(gc08a8_3264x2448_setting), PNULL, 0,
        .width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},
        
        {ADDR_AND_LEN_OF_ARRAY(gc08a8_3264x2448_setting), PNULL, 0,
        .width = SNAPSHOT_WIDTH, .height = SNAPSHOT_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

        {ADDR_AND_LEN_OF_ARRAY(gc08a8_1632x1224_setting), PNULL, 0,
        .width = CUSTOM1_WIDTH, .height = CUSTOM1_HEIGHT,
        .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW}
		}
	}

	/*If there are multiple modules,please add here*/
};

static SENSOR_TRIM_T s_gc08a8_resolution_trim_tab[VENDOR_NUM] = {
	{
     .module_id = MODULE_SUNNY,
     .trim_info = {
       {0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},

       {.trim_start_x = VIDEO_TRIM_X, .trim_start_y = VIDEO_TRIM_Y,
        .trim_width = VIDEO_TRIM_W,   .trim_height = VIDEO_TRIM_H,
        .line_time = VIDEO_LINE_TIME, .bps_per_lane = VIDEO_MIPI_PER_LANE_BPS,
        .frame_line = VIDEO_FRAME_LENGTH,
        .scaler_trim = {.x = VIDEO_TRIM_X, .y = VIDEO_TRIM_Y, .w = VIDEO_TRIM_W, .h = VIDEO_TRIM_H}},
	      
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

        {.trim_start_x = CUSTOM1_TRIM_X, .trim_start_y = CUSTOM1_TRIM_Y,
        .trim_width = CUSTOM1_TRIM_W,   .trim_height = CUSTOM1_TRIM_H,
        .line_time = CUSTOM1_LINE_TIME, .bps_per_lane = CUSTOM1_MIPI_PER_LANE_BPS,
        .frame_line = CUSTOM1_FRAME_LENGTH,
        .scaler_trim = {.x = CUSTOM1_TRIM_X, .y = CUSTOM1_TRIM_Y, .w = CUSTOM1_TRIM_W, .h = CUSTOM1_TRIM_H}},
       }
	}

    /*If there are multiple modules,please add here*/

};

static SENSOR_REG_T gc08a8_shutter_reg[] = {
	{0x0202, 0x08}, 
	{0x0203, 0xfa},
};

static struct sensor_i2c_reg_tab gc08a8_shutter_tab = {
    .settings = gc08a8_shutter_reg, 
	.size = ARRAY_SIZE(gc08a8_shutter_reg),
};

static SENSOR_REG_T gc08a8_again_reg[] = {
    {0x0204, 0x04}, 
	{0x0205, 0x00}, 
};

static struct sensor_i2c_reg_tab gc08a8_again_tab = {
    .settings = gc08a8_again_reg, 
	.size = ARRAY_SIZE(gc08a8_again_reg),
};

static SENSOR_REG_T gc08a8_dgain_reg[] = {
   
};

static struct sensor_i2c_reg_tab gc08a8_dgain_tab = {
    .settings = gc08a8_dgain_reg, 
	.size = ARRAY_SIZE(gc08a8_dgain_reg),
};

static SENSOR_REG_T gc08a8_frame_length_reg[] = {
	{0x0340, 0x09}, 
    {0x0341, 0xde}, 
};

static struct sensor_i2c_reg_tab gc08a8_frame_length_tab = {
    .settings = gc08a8_frame_length_reg,
    .size = ARRAY_SIZE(gc08a8_frame_length_reg),
};

static struct sensor_aec_i2c_tag gc08a8_aec_info = {
    .slave_addr = (I2C_SLAVE_ADDR >> 1),
    .addr_bits_type = SENSOR_I2C_REG_16BIT,
    .data_bits_type = SENSOR_I2C_VAL_8BIT,
    .shutter = &gc08a8_shutter_tab,
    .again = &gc08a8_again_tab,
    .dgain = &gc08a8_dgain_tab,
    .frame_length = &gc08a8_frame_length_tab,
};


static SENSOR_STATIC_INFO_T s_gc08a8_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .static_info = {
        .f_num = 200,
        .focal_length = 354,
        .max_fps = 30,
        .max_adgain = 16,
        .ois_supported = 0,
        .pdaf_supported = 0,
        .exp_valid_frame_num = 1,
        .clamp_level = 64,
        .adgain_valid_frame_num = 1,
        .fov_info = {{4.614f, 3.444f}, 4.222f}}
    }
    /*If there are multiple modules,please add here*/
};


static SENSOR_MODE_FPS_INFO_T s_gc08a8_mode_fps_info[VENDOR_NUM] = {
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


static struct sensor_module_info s_gc08a8_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {
         .major_i2c_addr = I2C_SLAVE_ADDR >> 1,
         .minor_i2c_addr = I2C_SLAVE_ADDR >> 1,

         .reg_addr_value_bits = SENSOR_I2C_REG_16BIT | SENSOR_I2C_VAL_8BIT |
                                SENSOR_I2C_FREQ_400,

         .avdd_val = SENSOR_AVDD_2800MV,
         .iovdd_val = SENSOR_AVDD_1800MV,
         .dvdd_val = SENSOR_AVDD_1200MV,

#if GC08A8_MIRROR_NORMAL
		 .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_R,
#elif GC08A8_MIRROR_H
		 .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_GR,
#elif GC08A8_MIRROR_V
		 .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_GB,
#elif GC08A8_MIRROR_HV
		 .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_B,
#else 
		 .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_R,
#endif
         .preview_skip_num = 1,
         .capture_skip_num = 1,
         .flash_capture_skip_num = 3,
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
			  .is_loose = 2,
          },
         .change_setting_skip_num = 1,
         .horizontal_view_angle = 65,
         .vertical_view_angle = 60
      }
    }

/*If there are multiple modules,please add here*/
};

static struct sensor_ic_ops s_gc08a8_ops_tab;
struct sensor_raw_info *s_gc08a8_mipi_raw_info_ptr = NULL;

/*==============================================================================
 * Description:
 * sensor all info
 * please modify this variable acording your spec
 *============================================================================*/
SENSOR_INFO_T g_gc08a8_mipi_raw_info = {
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
        {{ .reg_addr = GC08A8_PID_ADDR, .reg_value = GC08A8_PID_VALUE},
         { .reg_addr = GC08A8_VER_ADDR, .reg_value = GC08A8_VER_VALUE}},

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_RAW,

    .module_info_tab = s_gc08a8_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_gc08a8_module_info_tab),

    .resolution_tab_info_ptr = s_gc08a8_resolution_tab_raw,
    .sns_ops = &s_gc08a8_ops_tab,
    .raw_info_ptr = &s_gc08a8_mipi_raw_info_ptr,

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"gc08a8_v1",
};

#endif
