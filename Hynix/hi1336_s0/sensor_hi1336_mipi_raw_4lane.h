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

#ifndef _SENSOR_hi1336_MIPI_RAW_H_
#define _SENSOR_hi1336_MIPI_RAW_H_


#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"


//#include "parameters/sensor_hi1336_raw_param_main.c"


 //#define FEATURE_OTP



#define VENDOR_NUM 1
#define SENSOR_NAME "hi1336_s0"


#define I2C_SLAVE_ADDR 0x40 /* 8bit slave address*/


#define hi1336_PID_ADDR 0x0717
#define hi1336_PID_VALUE 0x36
#define hi1336_VER_ADDR 0x0716
#define hi1336_VER_VALUE 0x13

/* sensor parameters begin */
/* effective sensor output image size */
#define VIDEO_WIDTH 2104
#define VIDEO_HEIGHT 1560
#define PREVIEW_WIDTH 2104
#define PREVIEW_HEIGHT 1560
#define SNAPSHOT_WIDTH 4208
#define SNAPSHOT_HEIGHT 3120

/*Raw Trim parameters*/
#define VIDEO_TRIM_X 0
#define VIDEO_TRIM_Y 0
#define VIDEO_TRIM_W 2104
#define VIDEO_TRIM_H 1560
#define PREVIEW_TRIM_X 0
#define PREVIEW_TRIM_Y 0
#define PREVIEW_TRIM_W 2104
#define PREVIEW_TRIM_H 1560
#define SNAPSHOT_TRIM_X 0
#define SNAPSHOT_TRIM_Y 0
#define SNAPSHOT_TRIM_W 4208
#define SNAPSHOT_TRIM_H 3120

/*Mipi output*/
#define LANE_NUM 4
#define RAW_BITS 10

#define VIDEO_MIPI_PER_LANE_BPS 576    /* 2*Mipi clk */
#define PREVIEW_MIPI_PER_LANE_BPS 576  /* 2*Mipi clk */
#define SNAPSHOT_MIPI_PER_LANE_BPS 1152 /* 2*Mipi clk */

/*line time unit: 1ns*/
#define VIDEO_LINE_TIME 10006
#define PREVIEW_LINE_TIME 10006
#define SNAPSHOT_LINE_TIME 10006

/* frame length*/
#define VIDEO_FRAME_LENGTH 3328
#define PREVIEW_FRAME_LENGTH 3328
#define SNAPSHOT_FRAME_LENGTH 3328

/* please ref your spec */
#define FRAME_OFFSET 4
#define SENSOR_MAX_GAIN 0x100    //x16
#define SENSOR_BASE_GAIN 0x10    //x1
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

static const SENSOR_REG_T hi1336_init_setting[] =
    {
        // Sensor Information////////////////////////////
        // Sensor	  : Hi-1336
        // Date		  : 2019-04-22
        // Customer        : SPRD_validation
        // MCLK	          : 24MHz
        // MIPI            : 4 Lane
        // Pixel order 	  : B 1st
        // BLC offset	  : 64code
        // Firmware Ver.   : v15
        ////////////////////////////////////////////////

	{0x2000, 0x0021}, 
	{0x2002, 0x04a5}, 
	{0x2004, 0xb124}, 
	{0x2006, 0xc09c}, 
	{0x2008, 0x0064}, 
	{0x200a, 0x088e}, 
	{0x200c, 0x01c2}, 
	{0x200e, 0x00b4}, 
	{0x2010, 0x4020}, 
	{0x2012, 0x4292}, 
	{0x2014, 0xf00a}, 
	{0x2016, 0x0310}, 
	{0x2018, 0x12b0}, 
	{0x201a, 0xc3f2}, 
	{0x201c, 0x425f}, 
	{0x201e, 0x0282}, 
	{0x2020, 0xf35f}, 
	{0x2022, 0xf37f}, 
	{0x2024, 0x5f0f}, 
	{0x2026, 0x4f92}, 
	{0x2028, 0xf692}, 
	{0x202a, 0x0402}, 
	{0x202c, 0x93c2}, 
	{0x202e, 0x82cc}, 
	{0x2030, 0x2403}, 
	{0x2032, 0xf0f2}, 
	{0x2034, 0xffe7}, 
	{0x2036, 0x0254}, 
	{0x2038, 0x4130}, 
	{0x203a, 0x120b}, 
	{0x203c, 0x120a}, 
	{0x203e, 0x1209}, 
	{0x2040, 0x425f}, 
	{0x2042, 0x0600}, 
	{0x2044, 0xf35f}, 
	{0x2046, 0x4f4b}, 
	{0x2048, 0x12b0}, 
	{0x204a, 0xc6ae}, 
	{0x204c, 0x403d}, 
	{0x204e, 0x0100}, 
	{0x2050, 0x403e}, 
	{0x2052, 0x2bfc}, 
	{0x2054, 0x403f}, 
	{0x2056, 0x8020}, 
	{0x2058, 0x12b0}, 
	{0x205a, 0xc476}, 
	{0x205c, 0x930b}, 
	{0x205e, 0x2009}, 
	{0x2060, 0x93c2}, 
	{0x2062, 0x0c0a}, 
	{0x2064, 0x2403}, 
	{0x2066, 0x43d2}, 
	{0x2068, 0x0e1f}, 
	{0x206a, 0x3c13}, 
	{0x206c, 0x43c2}, 
	{0x206e, 0x0e1f}, 
	{0x2070, 0x3c10}, 
	{0x2072, 0x4039}, 
	{0x2074, 0x0e08}, 
	{0x2076, 0x492a}, 
	{0x2078, 0x421c}, 
	{0x207a, 0xf010}, 
	{0x207c, 0x430b}, 
	{0x207e, 0x430d}, 
	{0x2080, 0x12b0}, 
	{0x2082, 0xdfb6}, 
	{0x2084, 0x403d}, 
	{0x2086, 0x000e}, 
	{0x2088, 0x12b0}, 
	{0x208a, 0xc62c}, 
	{0x208c, 0x4e89}, 
	{0x208e, 0x0000}, 
	{0x2090, 0x3fe7}, 
	{0x2092, 0x4139}, 
	{0x2094, 0x413a}, 
	{0x2096, 0x413b}, 
	{0x2098, 0x4130}, 
	{0x209a, 0xb0b2}, 
	{0x209c, 0x0020}, 
	{0x209e, 0xf002}, 
	{0x20a0, 0x2429}, 
	{0x20a2, 0x421e}, 
	{0x20a4, 0x0256}, 
	{0x20a6, 0x532e}, 
	{0x20a8, 0x421f}, 
	{0x20aa, 0xf008}, 
	{0x20ac, 0x9e0f}, 
	{0x20ae, 0x2c01}, 
	{0x20b0, 0x4e0f}, 
	{0x20b2, 0x4f0c}, 
	{0x20b4, 0x430d}, 
	{0x20b6, 0x421e}, 
	{0x20b8, 0x7300}, 
	{0x20ba, 0x421f}, 
	{0x20bc, 0x7302}, 
	{0x20be, 0x5c0e}, 
	{0x20c0, 0x6d0f}, 
	{0x20c2, 0x821e}, 
	{0x20c4, 0x830c}, 
	{0x20c6, 0x721f}, 
	{0x20c8, 0x830e}, 
	{0x20ca, 0x2c0d}, 
	{0x20cc, 0x0900}, 
	{0x20ce, 0x7312}, 
	{0x20d0, 0x421e}, 
	{0x20d2, 0x7300}, 
	{0x20d4, 0x421f}, 
	{0x20d6, 0x7302}, 
	{0x20d8, 0x5c0e}, 
	{0x20da, 0x6d0f}, 
	{0x20dc, 0x821e}, 
	{0x20de, 0x830c}, 
	{0x20e0, 0x721f}, 
	{0x20e2, 0x830e}, 
	{0x20e4, 0x2bf3}, 
	{0x20e6, 0x4292}, 
	{0x20e8, 0x8248}, 
	{0x20ea, 0x0a08}, 
	{0x20ec, 0x0c10}, 
	{0x20ee, 0x4292}, 
	{0x20f0, 0x8252}, 
	{0x20f2, 0x0a12}, 
	{0x20f4, 0x12b0}, 
	{0x20f6, 0xdc9c}, 
	{0x20f8, 0xd0f2}, 
	{0x20fa, 0x0018}, 
	{0x20fc, 0x0254}, 
	{0x20fe, 0x4130}, 
	{0x2100, 0x120b}, 
	{0x2102, 0x12b0}, 
	{0x2104, 0xcfc8}, 
	{0x2106, 0x4f4b}, 
	{0x2108, 0x12b0}, 
	{0x210a, 0xcfc8}, 
	{0x210c, 0xf37f}, 
	{0x210e, 0x108f}, 
	{0x2110, 0xdb0f}, 
	{0x2112, 0x413b}, 
	{0x2114, 0x4130}, 
	{0x2116, 0x120b}, 
	{0x2118, 0x12b0}, 
	{0x211a, 0xcfc8}, 
	{0x211c, 0x4f4b}, 
	{0x211e, 0x108b}, 
	{0x2120, 0x12b0}, 
	{0x2122, 0xcfc8}, 
	{0x2124, 0xf37f}, 
	{0x2126, 0xdb0f}, 
	{0x2128, 0x413b}, 
	{0x212a, 0x4130}, 
	{0x212c, 0x120b}, 
	{0x212e, 0x120a}, 
	{0x2130, 0x1209}, 
	{0x2132, 0x1208}, 
	{0x2134, 0x4338}, 
	{0x2136, 0x40b2}, 
	{0x2138, 0x17fb}, 
	{0x213a, 0x83be}, 
	{0x213c, 0x12b0}, 
	{0x213e, 0xcfc8}, 
	{0x2140, 0xf37f}, 
	{0x2142, 0x903f}, 
	{0x2144, 0x0013}, 
	{0x2146, 0x244c}, 
	{0x2148, 0x12b0}, 
	{0x214a, 0xf100}, 
	{0x214c, 0x4f82}, 
	{0x214e, 0x82a4}, 
	{0x2150, 0xb3e2}, 
	{0x2152, 0x0282}, 
	{0x2154, 0x240a}, 
	{0x2156, 0x5f0f}, 
	{0x2158, 0x5f0f}, 
	{0x215a, 0x521f}, 
	{0x215c, 0x83be}, 
	{0x215e, 0x533f}, 
	{0x2160, 0x4f82}, 
	{0x2162, 0x83be}, 
	{0x2164, 0x43f2}, 
	{0x2166, 0x83c0}, 
	{0x2168, 0x4308}, 
	{0x216a, 0x4309}, 
	{0x216c, 0x9219}, 
	{0x216e, 0x82a4}, 
	{0x2170, 0x2c34}, 
	{0x2172, 0xb3e2}, 
	{0x2174, 0x0282}, 
	{0x2176, 0x242a}, 
	{0x2178, 0x12b0}, 
	{0x217a, 0xf116}, 
	{0x217c, 0x4f0b}, 
	{0x217e, 0x12b0}, 
	{0x2180, 0xf116}, 
	{0x2182, 0x4f0a}, 
	{0x2184, 0x490f}, 
	{0x2186, 0x5f0f}, 
	{0x2188, 0x5f0f}, 
	{0x218a, 0x4b8f}, 
	{0x218c, 0x2bfc}, 
	{0x218e, 0x4a8f}, 
	{0x2190, 0x2bfe}, 
	{0x2192, 0x5319}, 
	{0x2194, 0x9039}, 
	{0x2196, 0x0100}, 
	{0x2198, 0x2be9}, 
	{0x219a, 0x43d2}, 
	{0x219c, 0x83c0}, 
	{0x219e, 0x421e}, 
	{0x21a0, 0x82a4}, 
	{0x21a2, 0x903e}, 
	{0x21a4, 0x0080}, 
	{0x21a6, 0x2810}, 
	{0x21a8, 0x421f}, 
	{0x21aa, 0x2d28}, 
	{0x21ac, 0x503f}, 
	{0x21ae, 0x0014}, 
	{0x21b0, 0x4f82}, 
	{0x21b2, 0x82a0}, 
	{0x21b4, 0x903e}, 
	{0x21b6, 0x00c0}, 
	{0x21b8, 0x2805}, 
	{0x21ba, 0x421f}, 
	{0x21bc, 0x2e28}, 
	{0x21be, 0x503f}, 
	{0x21c0, 0x0014}, 
	{0x21c2, 0x3c12}, 
	{0x21c4, 0x480f}, 
	{0x21c6, 0x3c10}, 
	{0x21c8, 0x480f}, 
	{0x21ca, 0x3ff2}, 
	{0x21cc, 0x12b0}, 
	{0x21ce, 0xf100}, 
	{0x21d0, 0x4f0a}, 
	{0x21d2, 0x12b0}, 
	{0x21d4, 0xf100}, 
	{0x21d6, 0x4f0b}, 
	{0x21d8, 0x3fd5}, 
	{0x21da, 0x430a}, 
	{0x21dc, 0x430b}, 
	{0x21de, 0x3fd2}, 
	{0x21e0, 0x40b2}, 
	{0x21e2, 0x1bfe}, 
	{0x21e4, 0x83be}, 
	{0x21e6, 0x3fb0}, 
	{0x21e8, 0x4f82}, 
	{0x21ea, 0x82a2}, 
	{0x21ec, 0x4138}, 
	{0x21ee, 0x4139}, 
	{0x21f0, 0x413a}, 
	{0x21f2, 0x413b}, 
	{0x21f4, 0x4130}, 
	{0x21f6, 0x43d2}, 
	{0x21f8, 0x0300}, 
	{0x21fa, 0x12b0}, 
	{0x21fc, 0xcf6a}, 
	{0x21fe, 0x12b0}, 
	{0x2200, 0xcf0a}, 
	{0x2202, 0xb3d2}, 
	{0x2204, 0x0267}, 
	{0x2206, 0x2404}, 
	{0x2208, 0x12b0}, 
	{0x220a, 0xf12c}, 
	{0x220c, 0xc3d2}, 
	{0x220e, 0x0267}, 
	{0x2210, 0x12b0}, 
	{0x2212, 0xd0d4}, 
	{0x2214, 0x0261}, 
	{0x2216, 0x0000}, 
	{0x2218, 0x43c2}, 
	{0x221a, 0x0300}, 
	{0x221c, 0x4392}, 
	{0x221e, 0x732a}, 
	{0x2220, 0x4130}, 
	{0x2222, 0x90f2}, 
	{0x2224, 0x0010}, 
	{0x2226, 0x0260}, 
	{0x2228, 0x2002}, 
	{0x222a, 0x12b0}, 
	{0x222c, 0xd4aa}, 
	{0x222e, 0x12b0}, 
	{0x2230, 0xd5fa}, 
	{0x2232, 0x4392}, 
	{0x2234, 0x732a}, 
	{0x2236, 0x12b0}, 
	{0x2238, 0xf1f6}, 
	{0x223a, 0x4130}, 
	{0x223c, 0x120b}, 
	{0x223e, 0x120a}, 
	{0x2240, 0x1209}, 
	{0x2242, 0x1208}, 
	{0x2244, 0x1207}, 
	{0x2246, 0x1206}, 
	{0x2248, 0x1205}, 
	{0x224a, 0x1204}, 
	{0x224c, 0x8031}, 
	{0x224e, 0x000a}, 
	{0x2250, 0x4291}, 
	{0x2252, 0x82d8}, 
	{0x2254, 0x0004}, 
	{0x2256, 0x411f}, 
	{0x2258, 0x0004}, 
	{0x225a, 0x4fa1}, 
	{0x225c, 0x0006}, 
	{0x225e, 0x4257}, 
	{0x2260, 0x82e5}, 
	{0x2262, 0x4708}, 
	{0x2264, 0xd038}, 
	{0x2266, 0xff00}, 
	{0x2268, 0x4349}, 
	{0x226a, 0x4346}, 
	{0x226c, 0x90b2}, 
	{0x226e, 0x07d1}, 
	{0x2270, 0x0b94}, 
	{0x2272, 0x2806}, 
	{0x2274, 0x40b2}, 
	{0x2276, 0x0246}, 
	{0x2278, 0x0228}, 
	{0x227a, 0x40b2}, 
	{0x227c, 0x09fb}, 
	{0x227e, 0x0232}, 
	{0x2280, 0x4291}, 
	{0x2282, 0x0422}, 
	{0x2284, 0x0000}, 
	{0x2286, 0x421f}, 
	{0x2288, 0x0424}, 
	{0x228a, 0x812f}, 
	{0x228c, 0x4f81}, 
	{0x228e, 0x0002}, 
	{0x2290, 0x4291}, 
	{0x2292, 0x8248}, 
	{0x2294, 0x0008}, 
	{0x2296, 0x4214}, 
	{0x2298, 0x0310}, 
	{0x229a, 0x421a}, 
	{0x229c, 0x82a0}, 
	{0x229e, 0xf80a}, 
	{0x22a0, 0x421b}, 
	{0x22a2, 0x82a2}, 
	{0x22a4, 0xf80b}, 
	{0x22a6, 0x4382}, 
	{0x22a8, 0x7334}, 
	{0x22aa, 0x0f00}, 
	{0x22ac, 0x7304}, 
	{0x22ae, 0x4192}, 
	{0x22b0, 0x0008}, 
	{0x22b2, 0x0a08}, 
	{0x22b4, 0x4382}, 
	{0x22b6, 0x040c}, 
	{0x22b8, 0x4305}, 
	{0x22ba, 0x9382}, 
	{0x22bc, 0x7112}, 
	{0x22be, 0x2001}, 
	{0x22c0, 0x4315}, 
	{0x22c2, 0x421e}, 
	{0x22c4, 0x7100}, 
	{0x22c6, 0xb2f2}, 
	{0x22c8, 0x0261}, 
	{0x22ca, 0x2406}, 
	{0x22cc, 0xb3d2}, 
	{0x22ce, 0x0b02}, 
	{0x22d0, 0x2403}, 
	{0x22d2, 0x42d2}, 
	{0x22d4, 0x0809}, 
	{0x22d6, 0x0b00}, 
	{0x22d8, 0x40b2}, 
	{0x22da, 0x00b6}, 
	{0x22dc, 0x7334}, 
	{0x22de, 0x0f00}, 
	{0x22e0, 0x7304}, 
	{0x22e2, 0x4482}, 
	{0x22e4, 0x0a08}, 
	{0x22e6, 0xb2e2}, 
	{0x22e8, 0x0b05}, 
	{0x22ea, 0x2404}, 
	{0x22ec, 0x4392}, 
	{0x22ee, 0x7a0e}, 
	{0x22f0, 0x0800}, 
	{0x22f2, 0x7a10}, 
	{0x22f4, 0xf80e}, 
	{0x22f6, 0x93c2}, 
	{0x22f8, 0x82de}, 
	{0x22fa, 0x2468}, 
	{0x22fc, 0x9e0a}, 
	{0x22fe, 0x2803}, 
	{0x2300, 0x9349}, 
	{0x2302, 0x2001}, 
	{0x2304, 0x4359}, 
	{0x2306, 0x9e0b}, 
	{0x2308, 0x2802}, 
	{0x230a, 0x9369}, 
	{0x230c, 0x245c}, 
	{0x230e, 0x421f}, 
	{0x2310, 0x731a}, 
	{0x2312, 0xc312}, 
	{0x2314, 0x100f}, 
	{0x2316, 0x4f82}, 
	{0x2318, 0x7334}, 
	{0x231a, 0x0f00}, 
	{0x231c, 0x7304}, 
	{0x231e, 0x4192}, 
	{0x2320, 0x0008}, 
	{0x2322, 0x0a08}, 
	{0x2324, 0x421e}, 
	{0x2326, 0x7100}, 
	{0x2328, 0x812e}, 
	{0x232a, 0x425c}, 
	{0x232c, 0x0419}, 
	{0x232e, 0x537c}, 
	{0x2330, 0xfe4c}, 
	{0x2332, 0x9305}, 
	{0x2334, 0x2003}, 
	{0x2336, 0x40b2}, 
	{0x2338, 0x0c78}, 
	{0x233a, 0x7100}, 
	{0x233c, 0x421f}, 
	{0x233e, 0x731a}, 
	{0x2340, 0xc312}, 
	{0x2342, 0x100f}, 
	{0x2344, 0x503f}, 
	{0x2346, 0x00b6}, 
	{0x2348, 0x4f82}, 
	{0x234a, 0x7334}, 
	{0x234c, 0x0f00}, 
	{0x234e, 0x7304}, 
	{0x2350, 0x4482}, 
	{0x2352, 0x0a08}, 
	{0x2354, 0x9e81}, 
	{0x2356, 0x0002}, 
	{0x2358, 0x2814}, 
	{0x235a, 0xf74c}, 
	{0x235c, 0x434d}, 
	{0x235e, 0x411f}, 
	{0x2360, 0x0004}, 
	{0x2362, 0x4f1e}, 
	{0x2364, 0x0002}, 
	{0x2366, 0x9381}, 
	{0x2368, 0x0006}, 
	{0x236a, 0x240b}, 
	{0x236c, 0x4e6f}, 
	{0x236e, 0xf74f}, 
	{0x2370, 0x9c4f}, 
	{0x2372, 0x2423}, 
	{0x2374, 0x535d}, 
	{0x2376, 0x503e}, 
	{0x2378, 0x0006}, 
	{0x237a, 0x4d4f}, 
	{0x237c, 0x911f}, 
	{0x237e, 0x0006}, 
	{0x2380, 0x2bf5}, 
	{0x2382, 0x9359}, 
	{0x2384, 0x2403}, 
	{0x2386, 0x9079}, 
	{0x2388, 0x0003}, 
	{0x238a, 0x2028}, 
	{0x238c, 0x434d}, 
	{0x238e, 0x464f}, 
	{0x2390, 0x5f0f}, 
	{0x2392, 0x5f0f}, 
	{0x2394, 0x4f9f}, 
	{0x2396, 0x2dfc}, 
	{0x2398, 0x8020}, 
	{0x239a, 0x4f9f}, 
	{0x239c, 0x2dfe}, 
	{0x239e, 0x8022}, 
	{0x23a0, 0x5356}, 
	{0x23a2, 0x9076}, 
	{0x23a4, 0x0040}, 
	{0x23a6, 0x2407}, 
	{0x23a8, 0x9076}, 
	{0x23aa, 0xff80}, 
	{0x23ac, 0x2404}, 
	{0x23ae, 0x535d}, 
	{0x23b0, 0x926d}, 
	{0x23b2, 0x2bed}, 
	{0x23b4, 0x3c13}, 
	{0x23b6, 0x5359}, 
	{0x23b8, 0x3c11}, 
	{0x23ba, 0x4ea2}, 
	{0x23bc, 0x040c}, 
	{0x23be, 0x4e92}, 
	{0x23c0, 0x0002}, 
	{0x23c2, 0x040e}, 
	{0x23c4, 0x3fde}, 
	{0x23c6, 0x4079}, 
	{0x23c8, 0x0003}, 
	{0x23ca, 0x3fa1}, 
	{0x23cc, 0x9a0e}, 
	{0x23ce, 0x2803}, 
	{0x23d0, 0x9349}, 
	{0x23d2, 0x2001}, 
	{0x23d4, 0x4359}, 
	{0x23d6, 0x9b0e}, 
	{0x23d8, 0x2b9a}, 
	{0x23da, 0x3f97}, 
	{0x23dc, 0x9305}, 
	{0x23de, 0x2363}, 
	{0x23e0, 0x5031}, 
	{0x23e2, 0x000a}, 
	{0x23e4, 0x4134}, 
	{0x23e6, 0x4135}, 
	{0x23e8, 0x4136}, 
	{0x23ea, 0x4137}, 
	{0x23ec, 0x4138}, 
	{0x23ee, 0x4139}, 
	{0x23f0, 0x413a}, 
	{0x23f2, 0x413b}, 
	{0x23f4, 0x4130}, 
	{0x23f6, 0x120b}, 
	{0x23f8, 0x120a}, 
	{0x23fa, 0x1209}, 
	{0x23fc, 0x1208}, 
	{0x23fe, 0x1207}, 
	{0x2400, 0x1206}, 
	{0x2402, 0x1205}, 
	{0x2404, 0x1204}, 
	{0x2406, 0x8221}, 
	{0x2408, 0x425f}, 
	{0x240a, 0x0600}, 
	{0x240c, 0xf35f}, 
	{0x240e, 0x4fc1}, 
	{0x2410, 0x0002}, 
	{0x2412, 0x43c1}, 
	{0x2414, 0x0003}, 
	{0x2416, 0x403f}, 
	{0x2418, 0x0603}, 
	{0x241a, 0x4fe1}, 
	{0x241c, 0x0000}, 
	{0x241e, 0xb3ef}, 
	{0x2420, 0x0000}, 
	{0x2422, 0x2431}, 
	{0x2424, 0x4344}, 
	{0x2426, 0x4445}, 
	{0x2428, 0x450f}, 
	{0x242a, 0x5f0f}, 
	{0x242c, 0x5f0f}, 
	{0x242e, 0x403d}, 
	{0x2430, 0x000e}, 
	{0x2432, 0x4f1e}, 
	{0x2434, 0x0632}, 
	{0x2436, 0x4f1f}, 
	{0x2438, 0x0634}, 
	{0x243a, 0x12b0}, 
	{0x243c, 0xc62c}, 
	{0x243e, 0x4e08}, 
	{0x2440, 0x4f09}, 
	{0x2442, 0x421e}, 
	{0x2444, 0xf00c}, 
	{0x2446, 0x430f}, 
	{0x2448, 0x480a}, 
	{0x244a, 0x490b}, 
	{0x244c, 0x4e0c}, 
	{0x244e, 0x4f0d}, 
	{0x2450, 0x12b0}, 
	{0x2452, 0xdf96}, 
	{0x2454, 0x421a}, 
	{0x2456, 0xf00e}, 
	{0x2458, 0x430b}, 
	{0x245a, 0x403d}, 
	{0x245c, 0x0009}, 
	{0x245e, 0x12b0}, 
	{0x2460, 0xc62c}, 
	{0x2462, 0x4e06}, 
	{0x2464, 0x4f07}, 
	{0x2466, 0x5a06}, 
	{0x2468, 0x6b07}, 
	{0x246a, 0x425f}, 
	{0x246c, 0x0668}, 
	{0x246e, 0xf37f}, 
	{0x2470, 0x9f08}, 
	{0x2472, 0x2c6b}, 
	{0x2474, 0x4216}, 
	{0x2476, 0x06ca}, 
	{0x2478, 0x4307}, 
	{0x247a, 0x5505}, 
	{0x247c, 0x4685}, 
	{0x247e, 0x065e}, 
	{0x2480, 0x5354}, 
	{0x2482, 0x9264}, 
	{0x2484, 0x2bd0}, 
	{0x2486, 0x403b}, 
	{0x2488, 0x0603}, 
	{0x248a, 0x416f}, 
	{0x248c, 0xc36f}, 
	{0x248e, 0x4fcb}, 
	{0x2490, 0x0000}, 
	{0x2492, 0x12b0}, 
	{0x2494, 0xcd42}, 
	{0x2496, 0x41eb}, 
	{0x2498, 0x0000}, 
	{0x249a, 0x421f}, 
	{0x249c, 0x0256}, 
	{0x249e, 0x522f}, 
	{0x24a0, 0x421b}, 
	{0x24a2, 0xf008}, 
	{0x24a4, 0x532b}, 
	{0x24a6, 0x9f0b}, 
	{0x24a8, 0x2c01}, 
	{0x24aa, 0x4f0b}, 
	{0x24ac, 0x9381}, 
	{0x24ae, 0x0002}, 
	{0x24b0, 0x2409}, 
	{0x24b2, 0x430a}, 
	{0x24b4, 0x421e}, 
	{0x24b6, 0x0614}, 
	{0x24b8, 0x503e}, 
	{0x24ba, 0x000a}, 
	{0x24bc, 0x421f}, 
	{0x24be, 0x0680}, 
	{0x24c0, 0x9f0e}, 
	{0x24c2, 0x2801}, 
	{0x24c4, 0x431a}, 
	{0x24c6, 0xb0b2}, 
	{0x24c8, 0x0020}, 
	{0x24ca, 0xf002}, 
	{0x24cc, 0x241f}, 
	{0x24ce, 0x93c2}, 
	{0x24d0, 0x82cc}, 
	{0x24d2, 0x201c}, 
	{0x24d4, 0x4b0e}, 
	{0x24d6, 0x430f}, 
	{0x24d8, 0x521e}, 
	{0x24da, 0x7300}, 
	{0x24dc, 0x621f}, 
	{0x24de, 0x7302}, 
	{0x24e0, 0x421c}, 
	{0x24e2, 0x7316}, 
	{0x24e4, 0x421d}, 
	{0x24e6, 0x7318}, 
	{0x24e8, 0x8c0e}, 
	{0x24ea, 0x7d0f}, 
	{0x24ec, 0x2c0f}, 
	{0x24ee, 0x930a}, 
	{0x24f0, 0x240d}, 
	{0x24f2, 0x421f}, 
	{0x24f4, 0x8248}, 
	{0x24f6, 0xf03f}, 
	{0x24f8, 0xf7ff}, 
	{0x24fa, 0x4f82}, 
	{0x24fc, 0x0a08}, 
	{0x24fe, 0x0c10}, 
	{0x2500, 0x421f}, 
	{0x2502, 0x8252}, 
	{0x2504, 0xd03f}, 
	{0x2506, 0x00c0}, 
	{0x2508, 0x4f82}, 
	{0x250a, 0x0a12}, 
	{0x250c, 0x4b0a}, 
	{0x250e, 0x430b}, 
	{0x2510, 0x421e}, 
	{0x2512, 0x7300}, 
	{0x2514, 0x421f}, 
	{0x2516, 0x7302}, 
	{0x2518, 0x5a0e}, 
	{0x251a, 0x6b0f}, 
	{0x251c, 0x421c}, 
	{0x251e, 0x7316}, 
	{0x2520, 0x421d}, 
	{0x2522, 0x7318}, 
	{0x2524, 0x8c0e}, 
	{0x2526, 0x7d0f}, 
	{0x2528, 0x2c1a}, 
	{0x252a, 0x0900}, 
	{0x252c, 0x7312}, 
	{0x252e, 0x421e}, 
	{0x2530, 0x7300}, 
	{0x2532, 0x421f}, 
	{0x2534, 0x7302}, 
	{0x2536, 0x5a0e}, 
	{0x2538, 0x6b0f}, 
	{0x253a, 0x421c}, 
	{0x253c, 0x7316}, 
	{0x253e, 0x421d}, 
	{0x2540, 0x7318}, 
	{0x2542, 0x8c0e}, 
	{0x2544, 0x7d0f}, 
	{0x2546, 0x2bf1}, 
	{0x2548, 0x3c0a}, 
	{0x254a, 0x460e}, 
	{0x254c, 0x470f}, 
	{0x254e, 0x803e}, 
	{0x2550, 0x0800}, 
	{0x2552, 0x730f}, 
	{0x2554, 0x2b92}, 
	{0x2556, 0x4036}, 
	{0x2558, 0x07ff}, 
	{0x255a, 0x4307}, 
	{0x255c, 0x3f8e}, 
	{0x255e, 0x5221}, 
	{0x2560, 0x4134}, 
	{0x2562, 0x4135}, 
	{0x2564, 0x4136}, 
	{0x2566, 0x4137}, 
	{0x2568, 0x4138}, 
	{0x256a, 0x4139}, 
	{0x256c, 0x413a}, 
	{0x256e, 0x413b}, 
	{0x2570, 0x4130}, 
	{0x2572, 0x7400}, 
	{0x2574, 0x2003}, 
	{0x2576, 0x72a1}, 
	{0x2578, 0x2f00}, 
	{0x257a, 0x7020}, 
	{0x257c, 0x2f21}, 
	{0x257e, 0x7800}, 
	{0x2580, 0x0040}, 
	{0x2582, 0x7400}, 
	{0x2584, 0x2005}, 
	{0x2586, 0x72a1}, 
	{0x2588, 0x2f00}, 
	{0x258a, 0x7020}, 
	{0x258c, 0x2f22}, 
	{0x258e, 0x7800}, 
	{0x2590, 0x7400}, 
	{0x2592, 0x2011}, 
	{0x2594, 0x72a1}, 
	{0x2596, 0x2f00}, 
	{0x2598, 0x7020}, 
	{0x259a, 0x2f21}, 
	{0x259c, 0x7800}, 
	{0x259e, 0x7400}, 
	{0x25a0, 0x2009}, 
	{0x25a2, 0x72a1}, 
	{0x25a4, 0x2f1f}, 
	{0x25a6, 0x7021}, 
	{0x25a8, 0x3f40}, 
	{0x25aa, 0x7800}, 
	{0x25ac, 0x7400}, 
	{0x25ae, 0x2005}, 
	{0x25b0, 0x72a1}, 
	{0x25b2, 0x2f1f}, 
	{0x25b4, 0x7021}, 
	{0x25b6, 0x3f40}, 
	{0x25b8, 0x7800}, 
	{0x25ba, 0x7400}, 
	{0x25bc, 0x2009}, 
	{0x25be, 0x72a1}, 
	{0x25c0, 0x2f00}, 
	{0x25c2, 0x7020}, 
	{0x25c4, 0x2f22}, 
	{0x25c6, 0x7800}, 
	{0x25c8, 0x0009}, 
	{0x25ca, 0xf572}, 
	{0x25cc, 0x0009}, 
	{0x25ce, 0xf582}, 
	{0x25d0, 0x0009}, 
	{0x25d2, 0xf590}, 
	{0x25d4, 0x0009}, 
	{0x25d6, 0xf59e}, 
	{0x25d8, 0xf580}, 
	{0x25da, 0x0004}, 
	{0x25dc, 0x0009}, 
	{0x25de, 0xf590}, 
	{0x25e0, 0x0009}, 
	{0x25e2, 0xf5ba}, 
	{0x25e4, 0x0009}, 
	{0x25e6, 0xf572}, 
	{0x25e8, 0x0009}, 
	{0x25ea, 0xf5ac}, 
	{0x25ec, 0xf580}, 
	{0x25ee, 0x0004}, 
	{0x25f0, 0x0009}, 
	{0x25f2, 0xf572}, 
	{0x25f4, 0x0009}, 
	{0x25f6, 0xf5ac}, 
	{0x25f8, 0x0009}, 
	{0x25fa, 0xf590}, 
	{0x25fc, 0x0009}, 
	{0x25fe, 0xf59e}, 
	{0x2600, 0xf580}, 
	{0x2602, 0x0004}, 
	{0x2604, 0x0009}, 
	{0x2606, 0xf590}, 
	{0x2608, 0x0009}, 
	{0x260a, 0xf59e}, 
	{0x260c, 0x0009}, 
	{0x260e, 0xf572}, 
	{0x2610, 0x0009}, 
	{0x2612, 0xf5ac}, 
	{0x2614, 0xf580}, 
	{0x2616, 0x0004}, 
	{0x2618, 0x0212}, 
	{0x261a, 0x0217}, 
	{0x261c, 0x041f}, 
	{0x261e, 0x1017}, 
	{0x2620, 0x0413}, 
	{0x2622, 0x0103}, 
	{0x2624, 0x010b}, 
	{0x2626, 0x1c0a}, 
	{0x2628, 0x0202}, 
	{0x262a, 0x0407}, 
	{0x262c, 0x0205}, 
	{0x262e, 0x0204}, 
	{0x2630, 0x0114}, 
	{0x2632, 0x0110}, 
	{0x2634, 0xffff}, 
	{0x2636, 0x0048}, 
	{0x2638, 0x0090}, 
	{0x263a, 0x0000}, 
	{0x263c, 0x0000}, 
	{0x263e, 0xf618}, 
	{0x2640, 0x0000}, 
	{0x2642, 0x0000}, 
	{0x2644, 0x0060}, 
	{0x2646, 0x0078}, 
	{0x2648, 0x0060}, 
	{0x264a, 0x0078}, 
	{0x264c, 0x004f}, 
	{0x264e, 0x0037}, 
	{0x2650, 0x0048}, 
	{0x2652, 0x0090}, 
	{0x2654, 0x0000}, 
	{0x2656, 0x0000}, 
	{0x2658, 0xf618}, 
	{0x265a, 0x0000}, 
	{0x265c, 0x0000}, 
	{0x265e, 0x0180}, 
	{0x2660, 0x0780}, 
	{0x2662, 0x0180}, 
	{0x2664, 0x0780}, 
	{0x2666, 0x04cf}, 
	{0x2668, 0x0337}, 
	{0x266a, 0xf636}, 
	{0x266c, 0xf650}, 
	{0x266e, 0xf5c8}, 
	{0x2670, 0xf5dc}, 
	{0x2672, 0xf5f0}, 
	{0x2674, 0xf604}, 
	{0x2676, 0x0100}, 
	{0x2678, 0xff8a}, 
	{0x267a, 0xffff}, 
	{0x267c, 0x0104}, 
	{0x267e, 0xff0a}, 
	{0x2680, 0xffff}, 
	{0x2682, 0x0108}, 
	{0x2684, 0xff02}, 
	{0x2686, 0xffff}, 
	{0x2688, 0x010c}, 
	{0x268a, 0xff82}, 
	{0x268c, 0xffff}, 
	{0x268e, 0x0004}, 
	{0x2690, 0xf676}, 
	{0x2692, 0xe4e4}, 
	{0x2694, 0x4e4e}, 
	{0x2ffe, 0xc114}, 
	{0x3224, 0xf222}, 
	{0x322a, 0xf23c}, 
	{0x3230, 0xf03a}, 
	{0x3238, 0xf09a}, 
	{0x323a, 0xf012}, 
	{0x323e, 0xf3f6}, 
	{0x32a0, 0x0000}, 
	{0x32a2, 0x0000}, 
	{0x32a4, 0x0000}, 
	{0x32b0, 0x0000}, 
	{0x32c0, 0xf66a}, 
	{0x32c2, 0xf66e}, 
	{0x32c4, 0x0000}, 
	{0x32c6, 0xf66e}, 
	{0x32c8, 0x0000}, 
	{0x32ca, 0xf68e}, 
	{0x0a7e, 0x219c}, 
	{0x3244, 0x8400}, 
	{0x3246, 0xe400}, 
	{0x3248, 0xc88e}, 
	{0x324e, 0xfcd8}, 
	{0x3250, 0xa060}, 
	{0x325a, 0x7a37}, 
	{0x0734, 0x4b0b}, 
	{0x0736, 0xd8b0}, 
	{0x0600, 0x1190}, 
	{0x0602, 0x0052}, 
	{0x0604, 0x1008}, 
	{0x0606, 0x0200}, 
	{0x0616, 0x0040}, 
	{0x0614, 0x0040}, 
	{0x0612, 0x0040}, 
	{0x0610, 0x0040}, 
	{0x06b2, 0x0500}, 
	{0x06b4, 0x3ff0}, 
	{0x0618, 0x0a80}, 
	{0x0668, 0x4303}, 
	{0x06ca, 0x02cc}, 
	{0x066e, 0x0050}, 
	{0x0670, 0x0050}, 
	{0x113c, 0x0001}, 
	{0x11c4, 0x1080}, 
	{0x11c6, 0x0c34}, 
	{0x1104, 0x0160}, 
	{0x1106, 0x0138}, 
	{0x110a, 0x010e}, 
	{0x110c, 0x021d}, 
	{0x110e, 0xba2e}, 
	{0x1110, 0x0056}, 
	{0x1112, 0x00ac}, 
	{0x1114, 0x6907}, 
	{0x1122, 0x0011}, 
	{0x1124, 0x0022}, 
	{0x1126, 0x2e8c}, 
	{0x1128, 0x0016}, 
	{0x112a, 0x002b}, 
	{0x112c, 0x3483}, 
	{0x1130, 0x0200}, 
	{0x1132, 0x0200}, 
	{0x1102, 0x0028}, 
	{0x113e, 0x0200}, 
	{0x0d00, 0x4000}, 
	{0x0d02, 0x8004}, 
	{0x120a, 0x0a00}, 
	{0x0214, 0x0200}, 
	{0x0216, 0x0200}, 
	{0x0218, 0x0200}, 
	{0x021a, 0x0200}, 
	{0x1000, 0x0300}, 
	{0x1002, 0xc319}, 
	{0x1004, 0x2b30}, 
	{0x105a, 0x0091}, 
	{0x105c, 0x0f08}, 
	{0x105e, 0x0000}, 
	{0x1060, 0x3408}, 
	{0x1062, 0x0000}, 
	{0x0202, 0x0100}, 
	{0x0b10, 0x400c}, 
	{0x0212, 0x0000}, 
	{0x035e, 0x0701}, 
	{0x040a, 0x0000}, 
	{0x0420, 0x0003}, 
	{0x0424, 0x0c47}, 
	{0x0418, 0x1010}, 
	{0x0740, 0x004f}, 
	{0x0354, 0x1000}, 
	{0x035c, 0x0303}, 
	{0x050e, 0x0000}, 
	{0x0510, 0x0058}, 
	{0x0512, 0x0058}, 
	{0x0514, 0x0050}, 
	{0x0516, 0x0050}, 
	{0x0260, 0x0003}, 
	{0x0262, 0x0700}, 
	{0x0266, 0x0007}, 
	{0x0250, 0x0000}, 
	{0x0258, 0x0002}, 
	{0x025c, 0x0002}, 
	{0x025a, 0x03e8}, 
	{0x0256, 0x0100}, 
	{0x0254, 0x0001}, 
	{0x0440, 0x000c}, 
	{0x0908, 0x0003}, 
	{0x0708, 0x2f00}, 

};

static const SENSOR_REG_T hi1336_preview_setting[] = {
    // Sensor Information////////////////////////////
    // Sensor	  : Hi-1336
    // Date		  : 2019-04-22
    // Customer        : SPRD_validation
    // Image size	  : 2104x1560(BIN2)
    // MCLK/PCLK	  : 24MHz /288Mhz
    // MIPI speed(Mbps): 576Mbps x 4Lane
    // Frame Length	  : 3328
    // Line Length 	  : 6004
    // line Time       :13194
    // Max Fps 	  : 30.00fps
    // Pixel order 	  : Green 1st (=GB)
    // X/Y-flip        : X-flip
    // BLC offset	    : 64code
    // Firmware Ver.   : v15
    ////////////////////////////////////////////////

	{0x3250, 0xa060}, 
	{0x0730, 0x600f}, 
	{0x0732, 0xe1b0}, 
	{0x1118, 0x0004}, 
	{0x1200, 0x011f}, 
	{0x1204, 0x1c01}, 
	{0x1240, 0x0100}, 
	{0x0b20, 0x8200}, 
	{0x0f00, 0x0400}, 
	{0x103e, 0x0100}, 
	{0x1020, 0xc106}, 
	{0x1022, 0x0617}, 
	{0x1024, 0x0306}, 
	{0x1026, 0x0609}, 
	{0x1028, 0x1207}, 
	{0x102a, 0x090a}, 
	{0x102c, 0x1400}, 
	{0x1010, 0x1340}, 
	{0x1012, 0x0010}, 
	{0x1014, 0x001b}, 
	{0x1016, 0x001b}, 
	{0x101a, 0x001b}, 
	{0x0404, 0x0008}, 
	{0x0406, 0x1087}, 
	{0x0220, 0x0008}, 
	{0x022a, 0x0017}, 
	{0x0222, 0x0c80}, 
	{0x022c, 0x0c89}, 
	{0x0224, 0x002c}, 
	{0x022e, 0x0c61}, 
	{0x0f04, 0x0004}, 
	{0x0f06, 0x0000}, 
	{0x023a, 0x1122}, 
	{0x0234, 0x1111}, 
	{0x0238, 0x3311}, 
	{0x0246, 0x0020}, 
	{0x020a, 0x0cfb}, 
	{0x021c, 0x0008}, 
	{0x0206, 0x05dd}, 
	{0x020e, 0x0d00}, 
	{0x0b12, 0x0838}, 
	{0x0b14, 0x0618}, 
	{0x0204, 0x0000}, 
	{0x041c, 0x0048}, 
	{0x041e, 0x1047}, 
	{0x0b04, 0x037e}, 
	{0x027e, 0x0100}, 

};

static const SENSOR_REG_T hi1336_snapshot_setting[] = {
    // Sensor Information////////////////////////////
    // Sensor	  : Hi-1336
    // Date		  : 2019-04-22
    // Customer        : SPRD_validation
    // Image size	  : 4208x3120
    // MCLK/PCLK	  : 24MHz /288Mhz
    // MIPI speed(Mbps): 1152Mbps x 4Lane
    // Frame Length	  : 2492
    // Line Length 	  : 3800
    // line Time       :13194
    // Max Fps 	  : 30.00fps
    // Pixel order 	  : Green 1st (=GB)
    // X/Y-flip        : X-flip
    // BLC offset	    : 64code
    // Firmware Ver.   : v15
    ////////////////////////////////////////////////

	{0x3250, 0xa060},
	{0x0730, 0x600f},
	{0x0732, 0xe0b0},
	{0x1118, 0x0006},
	{0x1200, 0x0d1f},
	{0x1204, 0x1c01},
	{0x1240, 0x0100},
	{0x0b20, 0x8100},
	{0x0f00, 0x0000},
	{0x103e, 0x0000},
	{0x1020, 0xc10b},
	{0x1022, 0x0a31},
	{0x1024, 0x030b},
	{0x1026, 0x0d0f},
	{0x1028, 0x1a0e},
	{0x102a, 0x1311},
	{0x102c, 0x2400},
	{0x1010, 0x26a0},
	{0x1012, 0x0024},
	{0x1014, 0x006a},
	{0x1016, 0x006a},
	{0x101a, 0x006a},
	{0x1038, 0x0000},
	{0x1042, 0x0008},
	{0x0404, 0x0008},
	{0x0406, 0x1087},
	{0x0220, 0x0008},
	{0x022a, 0x0017},
	{0x0222, 0x0c80},
	{0x022c, 0x0c89},
	{0x0224, 0x002e},
	{0x022e, 0x0c61},
	{0x0f04, 0x0008},
	{0x0f06, 0x0000},
	{0x023a, 0x1111},
	{0x0234, 0x1111},
	{0x0238, 0x1111},
	{0x0246, 0x0020},
	{0x020a, 0x0cfb},
	{0x021c, 0x0008},
	{0x0206, 0x05dd},
	{0x020e, 0x0d00},
	{0x0b12, 0x1070},
	{0x0b14, 0x0c30},
	{0x0204, 0x0000},
	{0x041c, 0x0048},
	{0x041e, 0x1047},
	{0x0b04, 0x037e},
	{0x027e, 0x0100},

};

static const SENSOR_REG_T hi1336_video_setting[] = {
    // Sensor Information////////////////////////////
    // Sensor	  : Hi-1336
    // Date		  : 2017-04-22
    // Customer        : SPRD_validation
    // Image size	  : 1280x720
    // MCLK/PCLK	  : 24MHz /288Mhz
    // MIPI speed(Mbps): 360Mbps x 4Lane
    // Frame Length	  :  842
    // Line Length 	  : 3800
    // line Time       :13194
    // Max Fps 	  : 90.00fps
    // Pixel order 	  : Green 1st (=GB)
    // X/Y-flip        : X-flip
    // BLC offset	    : 64code
    // Firmware Ver.   : v15
    ////////////////////////////////////////////////

	{0x3250, 0xa060}, 
	{0x0730, 0x600f}, 
	{0x0732, 0xe1b0}, 
	{0x1118, 0x0004}, 
	{0x1200, 0x011f}, 
	{0x1204, 0x1c01}, 
	{0x1240, 0x0100}, 
	{0x0b20, 0x8200}, 
	{0x0f00, 0x0400}, 
	{0x103e, 0x0100}, 
	{0x1020, 0xc106}, 
	{0x1022, 0x0617}, 
	{0x1024, 0x0306}, 
	{0x1026, 0x0609}, 
	{0x1028, 0x1207}, 
	{0x102a, 0x090a}, 
	{0x102c, 0x1400}, 
	{0x1010, 0x1340}, 
	{0x1012, 0x0010}, 
	{0x1014, 0x001b}, 
	{0x1016, 0x001b}, 
	{0x101a, 0x001b}, 
	{0x0404, 0x0008}, 
	{0x0406, 0x1087}, 
	{0x0220, 0x0008}, 
	{0x022a, 0x0017}, 
	{0x0222, 0x0c80}, 
	{0x022c, 0x0c89}, 
	{0x0224, 0x002c}, 
	{0x022e, 0x0c61}, 
	{0x0f04, 0x0004}, 
	{0x0f06, 0x0000}, 
	{0x023a, 0x1122}, 
	{0x0234, 0x1111}, 
	{0x0238, 0x3311}, 
	{0x0246, 0x0020}, 
	{0x020a, 0x0cfb}, 
	{0x021c, 0x0008}, 
	{0x0206, 0x05dd}, 
	{0x020e, 0x0d00}, 
	{0x0b12, 0x0838}, 
	{0x0b14, 0x0618}, 
	{0x0204, 0x0000}, 
	{0x041c, 0x0048}, 
	{0x041e, 0x1047}, 
	{0x0b04, 0x037e}, 
	{0x027e, 0x0100}, 

};

static struct sensor_res_tab_info s_hi1336_resolution_tab_raw[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .reg_tab =
         {{ADDR_AND_LEN_OF_ARRAY(hi1336_init_setting), PNULL, 0, .width = 0,
           .height = 0, .xclk_to_sensor = EX_MCLK,
           .image_format = SENSOR_IMAGE_FORMAT_RAW},

          /*{ADDR_AND_LEN_OF_ARRAY(hi1336_video_setting), PNULL, 0,
           .width = VIDEO_WIDTH, .height = VIDEO_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},*/

          {ADDR_AND_LEN_OF_ARRAY(hi1336_preview_setting), PNULL, 0,
           .width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},

          {ADDR_AND_LEN_OF_ARRAY(hi1336_snapshot_setting), PNULL, 0,
           .width = SNAPSHOT_WIDTH, .height = SNAPSHOT_HEIGHT,
           .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW}}}

    /*If there are multiple modules,please add here*/
};

static SENSOR_TRIM_T s_hi1336_resolution_trim_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .trim_info =
         {
             {0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},

             /*{.trim_start_x = VIDEO_TRIM_X,
              .trim_start_y = VIDEO_TRIM_Y,
              .trim_width = VIDEO_TRIM_W,
              .trim_height = VIDEO_TRIM_H,
              .line_time = VIDEO_LINE_TIME,
              .bps_per_lane = VIDEO_MIPI_PER_LANE_BPS,
              .frame_line = VIDEO_FRAME_LENGTH,
              .scaler_trim = {.x = VIDEO_TRIM_X,
                              .y = VIDEO_TRIM_Y,
                              .w = VIDEO_TRIM_W,
                              .h = VIDEO_TRIM_H}},*/

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

static SENSOR_REG_T hi1336_shutter_reg[] = {
    {0x020A, 0x0000},
};

static struct sensor_i2c_reg_tab hi1336_shutter_tab = {
    .settings = hi1336_shutter_reg, .size = ARRAY_SIZE(hi1336_shutter_reg),
};

static SENSOR_REG_T hi1336_again_reg[] = {
    {0x0213, 0x0000},
};

static struct sensor_i2c_reg_tab hi1336_again_tab = {
    .settings = hi1336_again_reg, .size = ARRAY_SIZE(hi1336_again_reg),
};

static SENSOR_REG_T hi1336_dgain_reg[] = {

};

static struct sensor_i2c_reg_tab hi1336_dgain_tab = {
    .settings = hi1336_dgain_reg, .size = ARRAY_SIZE(hi1336_dgain_reg),
};

static SENSOR_REG_T hi1336_frame_length_reg[] = {
    {0x020E, 0x0000},
};

static struct sensor_i2c_reg_tab hi1336_frame_length_tab = {
    .settings = hi1336_frame_length_reg,
    .size = ARRAY_SIZE(hi1336_frame_length_reg),
};

static struct sensor_aec_i2c_tag hi1336_aec_info = {
    .slave_addr = (I2C_SLAVE_ADDR >> 1),
    .addr_bits_type = SENSOR_I2C_REG_16BIT,
    .data_bits_type = SENSOR_I2C_VAL_16BIT,
    .shutter = &hi1336_shutter_tab,
    .again = &hi1336_again_tab,
    .dgain = &hi1336_dgain_tab,
    .frame_length = &hi1336_frame_length_tab,
};

static SENSOR_STATIC_INFO_T s_hi1336_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .static_info = {.f_num = 200,
                     .focal_length = 354,
                     .max_fps = 30,
                     .max_adgain = 15 * 2,
                     .ois_supported = 0,
                     .pdaf_supported = 0,
                     .exp_valid_frame_num = 1,
                     .clamp_level = 64,
                     .adgain_valid_frame_num = 1,
                     .fov_info = {{4.614f, 3.444f}, 3.802f}}}
    /*If there are multiple modules,please add here*/
};

static SENSOR_MODE_FPS_INFO_T s_hi1336_mode_fps_info[VENDOR_NUM] = {
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

static struct sensor_module_info s_hi1336_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {.major_i2c_addr = I2C_SLAVE_ADDR >> 1,
                     .minor_i2c_addr = I2C_SLAVE_ADDR >> 1,

                     .i2c_burst_mode = SENSOR_I2C_BURST_REG16_VAL16,

                     .reg_addr_value_bits = SENSOR_I2C_REG_16BIT |
                                            SENSOR_I2C_VAL_16BIT |
                                            SENSOR_I2C_FREQ_400,

                     .avdd_val = SENSOR_AVDD_2800MV,
                     .iovdd_val = SENSOR_AVDD_1800MV,
                     .dvdd_val = SENSOR_AVDD_1200MV,

                     .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_GB,

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

static struct sensor_ic_ops s_hi1336_ops_tab;
struct sensor_raw_info *s_hi1336_mipi_raw_info_ptr = PNULL;

/*==============================================================================
 * Description:
 * sensor all info
 * please modify this variable acording your spec
 *============================================================================*/
SENSOR_INFO_T g_hi1336_mipi_raw_info = {
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
    .identify_code = {{.reg_addr = hi1336_PID_ADDR,
                       .reg_value = hi1336_PID_VALUE},
                      {.reg_addr = hi1336_VER_ADDR,
                       .reg_value = hi1336_VER_VALUE}},

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_RAW,

    .module_info_tab = s_hi1336_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_hi1336_module_info_tab),

    .resolution_tab_info_ptr = s_hi1336_resolution_tab_raw,
    .sns_ops = &s_hi1336_ops_tab,
    .raw_info_ptr = &s_hi1336_mipi_raw_info_ptr,

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"hi1336_s0",
};

#endif
