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
#ifndef _GC0310_MIPI_YUV_
#define _GC0310_MIPI_YUV_

#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"

#define GC0310_MIPI_I2C_ADDR_W 0x21
#define GC0310_MIPI_I2C_ADDR_R 0x21
#define SENSOR_GAIN_SCALE 16
#define VENDOR_NUM 1

typedef enum { FLICKER_50HZ = 0, FLICKER_60HZ, FLICKER_MAX } FLICKER_E;

SENSOR_REG_T GC0310_MIPI_YUV_COMMON1[] = {};

SENSOR_REG_T GC0310_MIPI_YUV_COMMON2[] = {
    {0xfe, 0xf0},
    {0xfe, 0xf0},
    {0xfe, 0x00},
    {0xfc, 0x16},
    {0xfc, 0x16},
    {0xf2, 0x07},
    {0xf3, 0xa1},
    {0xf5, 0x07},
    {0xf7, 0x88},
    {0xf8, 0x00},
    {0xf9, 0x4f},
    /* mipi */
    {0xfa, 0x74},
    {0xfc, 0xce},
    {0xfd, 0x00},

    {0x00, 0x2f},
    {0x01, 0x0f},
    {0x02, 0x04},
    {0x03, 0x02},
    {0x04, 0x12},
    {0x09, 0x00},
    {0x0a, 0x00},
    {0x0b, 0x00},
    {0x0c, 0x04},
    {0x0d, 0x01},
    {0x0e, 0xe8},
    {0x0f, 0x02},
    {0x10, 0x88},
    {0x16, 0x00},
    {0x17, 0x14},
    {0x18, 0x1a},
    {0x19, 0x14},
    {0x1b, 0x48},
    {0x1c, 0x6c},
    {0x1e, 0x6b},
    {0x1f, 0x28},
    {0x20, 0x8b}, // 0x89
    {0x21, 0x49},
    {0x22, 0xd0},
    {0x23, 0x04},
    {0x24, 0xff},
    {0x34, 0x20},
    // BLK//
    {0x26, 0x23},
    {0x28, 0xff},
    {0x29, 0x00},
    {0x32, 0x04},
    {0x33, 0x10},
    {0x37, 0x20},
    {0x38, 0x10},
    {0x47, 0x80},
    {0x4e, 0x66},
    {0xa8, 0x02},
    {0xa9, 0x80},
    // ISP reg//
    {0x40, 0xff},
    {0x41, 0x21},
    {0x42, 0xcf},
    {0x44, 0x02},
    {0x45, 0xa0},
    {0x46, 0x02},
    {0x4a, 0x11},
    {0x4b, 0x01},
    {0x4c, 0x20},
    {0x4d, 0x05},
    {0x4f, 0x01},
    {0x50, 0x01},
    {0x55, 0x01},
    {0x56, 0xe0},
    {0x57, 0x02},
    {0x58, 0x80},
    // GAIN//
    {0x70, 0x70},
    {0x5a, 0x84},
    {0x5b, 0xc9},
    {0x5c, 0xed},
    {0x77, 0x74},
    {0x78, 0x40},
    {0x79, 0x5f},
    // DNDD//
    {0x82, 0x14},
    {0x83, 0x0b},
    {0x89, 0xf0},
    // EEINTP//
    {0x8f, 0xaa},
    {0x90, 0x8c},
    {0x91, 0x90},
    {0x92, 0x03},
    {0x93, 0x03},
    {0x94, 0x05},
    {0x95, 0x65},
    {0x96, 0xf0},
    // ASDE//
    {0xfe, 0x00},
    {0x9a, 0x20},
    {0x9b, 0x80},
    {0x9c, 0x40},
    {0x9d, 0x80},
    {0xa1, 0x30},
    {0xa2, 0x32},
    {0xa4, 0x80},
    {0xa5, 0x28},
    {0xaa, 0x30},
    {0xac, 0x22},
    // GAMMA//
    {0xfe, 0x00}, // big gamma
    {0xbf, 0x08},
    {0xc0, 0x16},
    {0xc1, 0x28},
    {0xc2, 0x41},
    {0xc3, 0x5a},
    {0xc4, 0x6c},
    {0xc5, 0x7a},
    {0xc6, 0x96},
    {0xc7, 0xac},
    {0xc8, 0xbc},
    {0xc9, 0xc9},
    {0xca, 0xd3},
    {0xcb, 0xdd},
    {0xcc, 0xe5},
    {0xcd, 0xf1},
    {0xce, 0xfa},
    {0xcf, 0xff},

    // YCP//
    {0xd0, 0x40},
    {0xd1, 0x34},
    {0xd2, 0x34},
    {0xd3, 0x40},
    {0xd6, 0xf2},
    {0xd7, 0x1b},
    {0xd8, 0x18},
    {0xdd, 0x03},
    // AEC//
    {0xfe, 0x01},
    {0x05, 0x30},
    {0x06, 0x75},
    {0x07, 0x40},
    {0x08, 0xb0},
    {0x0a, 0xc5},
    {0x0b, 0x11},
    {0x0c, 0x00},
    {0x12, 0x52},
    {0x13, 0x38},
    {0x18, 0x95},
    {0x19, 0x96},
    {0x1f, 0x20},
    {0x20, 0xc0}, // 80

    {0x3e, 0x40},
    {0x3f, 0x57},
    {0x40, 0x7d},
    {0x03, 0x60},

    {0x44, 0x02},
    // AWB//
    {0x1c, 0x91},
    {0x21, 0x15},
    {0x50, 0x80},
    {0x56, 0x04},
    {0x59, 0x08},
    {0x5b, 0x02},
    {0x61, 0x8d},
    {0x62, 0xa7},
    {0x63, 0xd0},
    {0x65, 0x06},
    {0x66, 0x06},
    {0x67, 0x84},
    {0x69, 0x08},
    {0x6a, 0x25}, // 50
    {0x6b, 0x01},
    {0x6c, 0x00},
    {0x6d, 0x02},
    {0x6e, 0xf0},
    {0x6f, 0x80},
    {0x76, 0x80},

    {0x78, 0xaf},
    {0x79, 0x75},
    {0x7a, 0x40},
    {0x7b, 0x50},
    {0x7c, 0x0c},

    {0xa4, 0xb9},
    {0xa5, 0xa0},
    {0x90, 0xc9},
    {0x91, 0xbe},

    {0xa6, 0xb8},
    {0xa7, 0x95},
    {0x92, 0xe6},
    {0x93, 0xca},

    {0xa9, 0xbc},
    {0xaa, 0x95},
    {0x95, 0x23},
    {0x96, 0xe7},

    {0xab, 0x9d},
    {0xac, 0x80},
    {0x97, 0x43},
    {0x98, 0x24},

    {0xae, 0xb7},
    {0xaf, 0x9e},
    {0x9a, 0x43},
    {0x9b, 0x24},

    {0xb0, 0xc8},
    {0xb1, 0x97},
    {0x9c, 0xc4},
    {0x9d, 0x44},

    {0xb3, 0xb7},
    {0xb4, 0x7f},
    {0x9f, 0xc7},
    {0xa0, 0xc8},

    {0xb5, 0x00},
    {0xb6, 0x00},
    {0xa1, 0x00},
    {0xa2, 0x00},

    {0x86, 0x60},
    {0x87, 0x08},
    {0x88, 0x00},
    {0x89, 0x00},
    {0x8b, 0xde},
    {0x8c, 0x80},
    {0x8d, 0x00},
    {0x8e, 0x00},

    {0x94, 0x55},
    {0x99, 0xa6},
    {0x9e, 0xaa},
    {0xa3, 0x0a},
    {0x8a, 0x0a},
    {0xa8, 0x55},
    {0xad, 0x55},
    {0xb2, 0x55},
    {0xb7, 0x05},
    {0x8f, 0x05},

    {0xb8, 0xcc},
    {0xb9, 0x9a},
    // CC//
    {0xfe, 0x01},
    {0xd0, 0x38},
    {0xd1, 0x00},
    {0xd2, 0x02}, // 0a
    {0xd3, 0x04},
    {0xd4, 0x38},
    {0xd5, 0x12}, // f0
    {0xd6, 0x30},
    {0xd7, 0x00},
    {0xd8, 0x0a},
    {0xd9, 0x16},
    {0xda, 0x39},
    {0xdb, 0xf8},
    // LSC//
    {0xfe, 0x01},
    {0xc1, 0x3c},
    {0xc2, 0x50},
    {0xc3, 0x00},
    {0xc4, 0x40},
    {0xc5, 0x30},
    {0xc6, 0x30},
    {0xc7, 0x10},
    {0xc8, 0x00},
    {0xc9, 0x00},
    {0xdc, 0x20},
    {0xdd, 0x10},
    {0xdf, 0x00},
    {0xde, 0x00},
    // Histogram//
    {0x01, 0x10},
    {0x0b, 0x31},
    {0x0e, 0x50},
    {0x0f, 0x0f},
    {0x10, 0x6e},
    {0x12, 0xa0},
    {0x15, 0x60},
    {0x16, 0x60},
    {0x17, 0xe0},
    // Measure Window//
    {0xcc, 0x0c},
    {0xcd, 0x10},
    {0xce, 0xa0},
    {0xcf, 0xe6},
    // dark sun//
    {0x45, 0xf7},
    {0x46, 0xff},
    {0x47, 0x15},
    {0x48, 0x03},
    {0x4f, 0x60},
    // banding//
    {0xfe, 0x00},
    {0x05, 0x01},
    {0x06, 0xb6}, // HB
    {0x07, 0x00},
    {0x08, 0x2a}, // VB
    {0xfe, 0x01},
    {0x25, 0x00}, // step
    {0x26, 0x6a},
    {0x27, 0x02}, // 20fps
    {0x28, 0x12},
    {0x29, 0x03}, // 12.5fps
    {0x2a, 0x50},
    {0x2b, 0x04}, // 7.14fps
    {0x2c, 0xf8},
    {0x2d, 0x07}, // 5.55fps
    {0x2e, 0x74},
    {0x3c, 0x20},
    {0xfe, 0x03},
    {0x01, 0x00},
    {0x02, 0x00},
    {0x10, 0x00},
    {0x15, 0x00},
    {0x17, 0x00},
    {0x04, 0x10},
    {0x05, 0x00},
    {0x40, 0x00},
    {0x52, 0x02},
    {0x53, 0x00},
    {0x54, 0x20},
    {0x55, 0x20},
    // dark sun//
    {0x5a, 0x00},
    {0x5b, 0x00},
    {0x5c, 0x02},
    {0x5d, 0xe0},
    {0x5a, 0x03},
    // banding//
    {0x60, 0xb6},
    {0x61, 0x80},
    {0x62, 0x9d}, // HB
    {0x63, 0xb6},
    {0x64, 0x05}, // VB
    {0x65, 0xff},
    {0x66, 0x00}, // step
    {0x67, 0x00},
    {0xfe, 0x00},
};

SENSOR_REG_T GC0310_MIPI_YUV_COMMON[] = {
    {0xfe, 0xf0},
    {0xfe, 0xf0},
    {0xfe, 0x00},
    {0xfc, 0x0e},
    {0xfc, 0x0e},
    {0xf2, 0x80},
    {0xf3, 0x00},
    {0xf7, 0x1b},
    {0xf8, 0x04},
    {0xf9, 0x8e},
    {0xfa, 0x11},
    /* mipi */
    {0xfe, 0x03},
    {0x40, 0x08},
    {0x42, 0x00},
    {0x43, 0x00},
    {0x01, 0x03},
    {0x10, 0x84},

    {0x01, 0x03},
    {0x02, 0x11},
    {0x03, 0x94},
    {0x04, 0x01},
    {0x05, 0x00},
    {0x06, 0x80},
    {0x11, 0x1e},
    {0x12, 0x00},
    {0x13, 0x05},
    {0x15, 0x10},
    {0x21, 0x10},
    {0x22, 0x01},
    {0x23, 0x10},
    {0x24, 0x02},
    {0x25, 0x10},
    {0x26, 0x03},
    {0x29, 0x02},
    {0x2a, 0x0a},
    {0x2b, 0x04},
    {0xfe, 0x00},

    {0x00, 0x2f},
    {0x01, 0x0f},
    {0x02, 0x04},
    {0x03, 0x03},
    {0x04, 0x50},
    {0x09, 0x00},
    {0x0a, 0x00},
    {0x0b, 0x00},
    {0x0c, 0x04},
    {0x0d, 0x01},
    {0x0e, 0xe8},
    {0x0f, 0x02},
    {0x10, 0x88},
    {0x16, 0x00},
    {0x17, 0x14},
    {0x18, 0x1a},
    {0x19, 0x14},
    {0x1b, 0x48},
    {0x1e, 0x6b},
    {0x1f, 0x28},
    {0x20, 0x8b}, // 0x89
    {0x21, 0x49},
    {0x22, 0xb0},
    {0x23, 0x04},
    {0x24, 0x16},
    {0x34, 0x20},
    // BLK//
    {0x26, 0x23},
    {0x28, 0xff},
    {0x29, 0x00},
    {0x33, 0x10},
    {0x37, 0x20},
    {0x38, 0x10},
    {0x47, 0x80},
    {0x4e, 0x66},
    {0xa8, 0x02},
    {0xa9, 0x80},
    // ISP reg//
    {0x40, 0xff},
    {0x41, 0x21},
    {0x42, 0xcf},
    {0x44, 0x02},
    {0x45, 0xa0},
    {0x46, 0x02},
    {0x4a, 0x11},
    {0x4b, 0x01},
    {0x4c, 0x20},
    {0x4d, 0x05},
    {0x4f, 0x01},
    {0x50, 0x01},
    {0x55, 0x01},
    {0x56, 0xe0},
    {0x57, 0x02},
    {0x58, 0x80},
    // GAIN//
    {0x70, 0x70},
    {0x5a, 0x84},
    {0x5b, 0xc9},
    {0x5c, 0xed},
    {0x77, 0x74},
    {0x78, 0x40},
    {0x79, 0x5f},
    // DNDD//
    {0x82, 0x14},
    {0x83, 0x0b},
    {0x89, 0xf0},
    // EEINTP//
    {0x8f, 0xaa},
    {0x90, 0x8c},
    {0x91, 0x90},
    {0x92, 0x03},
    {0x93, 0x03},
    {0x94, 0x05},
    {0x95, 0x65},
    {0x96, 0xf0},
    // ASDE//
    {0xfe, 0x00},
    {0x9a, 0x20},
    {0x9b, 0x80},
    {0x9c, 0x40},
    {0x9d, 0x80},
    {0xa1, 0x30},
    {0xa2, 0x32},
    {0xa4, 0x30},
    {0xa5, 0x30},
    {0xaa, 0x10},
    {0xac, 0x22},
    // GAMMA//
    {0xfe, 0x00}, // big gamma
    {0xbf, 0x08},
    {0xc0, 0x1d},
    {0xc1, 0x34},
    {0xc2, 0x4b},
    {0xc3, 0x60},
    {0xc4, 0x73},
    {0xc5, 0x85},
    {0xc6, 0x9f},
    {0xc7, 0xb5},
    {0xc8, 0xc7},
    {0xc9, 0xd5},
    {0xca, 0xe0},
    {0xcb, 0xe7},
    {0xcc, 0xec},
    {0xcd, 0xf4},
    {0xce, 0xfa},
    {0xcf, 0xff},

    // YCP//
    {0xd0, 0x40},
    {0xd1, 0x28},
    {0xd2, 0x28},
    {0xd3, 0x40},
    {0xd6, 0xf2},
    {0xd7, 0x1b},
    {0xd8, 0x18},
    {0xdd, 0x03},
    // AEC//
    {0xfe, 0x01},
    {0x05, 0x30},
    {0x06, 0x75},
    {0x07, 0x40},
    {0x08, 0xb0},
    {0x0a, 0xc5},
    {0x0b, 0x11},
    {0x0c, 0x00},
    {0x12, 0x52},
    {0x13, 0x38},
    {0x18, 0x95},
    {0x19, 0x96},
    {0x1f, 0x20},
    {0x20, 0xc0}, // 80

    {0x3e, 0x40},
    {0x3f, 0x57},
    {0x40, 0x7d},
    {0x03, 0x60},

    {0x44, 0x03},
    // AWB//
    {0x1c, 0x91},
    {0x21, 0x15},
    {0x50, 0x80},
    {0x56, 0x04},
    {0x59, 0x08},
    {0x5b, 0x02},
    {0x61, 0x8d},
    {0x62, 0xa7},
    {0x63, 0xd0},
    {0x65, 0x06},
    {0x66, 0x06},
    {0x67, 0x84},
    {0x69, 0x08},
    {0x6a, 0x25}, // 50
    {0x6b, 0x01},
    {0x6c, 0x00},
    {0x6d, 0x02},
    {0x6e, 0xf0},
    {0x6f, 0x80},
    {0x76, 0x80},

    {0x78, 0xaf},
    {0x79, 0x75},
    {0x7a, 0x40},
    {0x7b, 0x50},
    {0x7c, 0x0c},

    {0xa4, 0xb9},
    {0xa5, 0xa0},
    {0x90, 0xc9},
    {0x91, 0xbe},

    {0xa6, 0xb8},
    {0xa7, 0x95},
    {0x92, 0xe6},
    {0x93, 0xca},

    {0xa9, 0xb6},
    {0xaa, 0x89},
    {0x95, 0x23},
    {0x96, 0xe7},

    {0xab, 0x9d},
    {0xac, 0x80},
    {0x97, 0x43},
    {0x98, 0x24},

    {0xae, 0xb7},
    {0xaf, 0x9e},
    {0x9a, 0x43},
    {0x9b, 0x24},

    {0xb0, 0xc8},
    {0xb1, 0x97},
    {0x9c, 0xc4},
    {0x9d, 0x44},

    {0xb3, 0xb7},
    {0xb4, 0x7f},
    {0x9f, 0xc7},
    {0xa0, 0xc8},

    {0xb5, 0x00},
    {0xb6, 0x00},
    {0xa1, 0x00},
    {0xa2, 0x00},

    {0x86, 0x60},
    {0x87, 0x08},
    {0x88, 0x00},
    {0x89, 0x00},
    {0x8b, 0xde},
    {0x8c, 0x80},
    {0x8d, 0x00},
    {0x8e, 0x00},

    {0x94, 0x55},
    {0x99, 0xa6},
    {0x9e, 0xaa},
    {0xa3, 0x0a},
    {0x8a, 0x0a},
    {0xa8, 0x55},
    {0xad, 0x55},
    {0xb2, 0x55},
    {0xb7, 0x05},
    {0x8f, 0x05},

    {0xb8, 0xcc},
    {0xb9, 0x9a},
    // CC//
    {0xfe, 0x01},
    {0xd0, 0x38},
    {0xd1, 0x00},
    {0xd2, 0x06}, // 0a
    {0xd3, 0xf8},
    {0xd4, 0x3c},
    {0xd5, 0xf0}, // f0
    {0xd6, 0x30},
    {0xd7, 0x00},
    {0xd8, 0x0a},
    {0xd9, 0x16},
    {0xda, 0x39},
    {0xdb, 0xf8},
    // LSC//
    {0xfe, 0x01},
    {0xc1, 0x3c},
    {0xc2, 0x50},
    {0xc3, 0x00},
    {0xc4, 0x40},
    {0xc5, 0x30},
    {0xc6, 0x30},
    {0xc7, 0x10},
    {0xc8, 0x00},
    {0xc9, 0x00},
    {0xdc, 0x20},
    {0xdd, 0x10},
    {0xdf, 0x00},
    {0xde, 0x00},
    // Histogram//
    {0x01, 0x10},
    {0x0b, 0x31},
    {0x0e, 0x50},
    {0x0f, 0x0f},
    {0x10, 0x6e},
    {0x12, 0xa0},
    {0x15, 0x60},
    {0x16, 0x60},
    {0x17, 0xe0},
    // Measure Window//
    {0xcc, 0x0c},
    {0xcd, 0x10},
    {0xce, 0xa0},
    {0xcf, 0xe6},
    // dark sun//
    {0x45, 0xf7},
    {0x46, 0xff},
    {0x47, 0x15},
    {0x48, 0x03},
    {0x4f, 0x60},
    // banding//
    {0xfe, 0x00},
    {0x05, 0x02},
    {0x06, 0xd1}, // HB
    {0x07, 0x00},
    {0x08, 0x22}, // VB
    {0xfe, 0x01},
    {0x25, 0x00}, // step
    {0x26, 0x6a},
    {0x27, 0x02}, // 20fps
    {0x28, 0x12},
    {0x29, 0x03}, // 12.5fps
    {0x2a, 0x50},
    {0x2b, 0x05}, // 7.14fps
    {0x2c, 0xcc},
    {0x2d, 0x07}, // 5.55fps
    {0x2e, 0x74},
    {0x3c, 0x20},
    {0xfe, 0x00},
    {0x7a, 0x80},
    {0x7b, 0x80},
    {0x7c, 0x86},
};

static struct sensor_res_tab_info s_gc0310_resolution_tab_yuv[VENDOR_NUM] = {
    {
      .module_id = MODULE_SUNNY,
      .reg_tab = {
        {ADDR_AND_LEN_OF_ARRAY(GC0310_MIPI_YUV_COMMON1), PNULL, 0,
        .width = 0, .height = 0,
        .xclk_to_sensor = 24, .image_format = SENSOR_IMAGE_FORMAT_YUV422},

        {ADDR_AND_LEN_OF_ARRAY(GC0310_MIPI_YUV_COMMON1), PNULL, 0,
        .width = 640,.height = 480,
        .xclk_to_sensor = 24, .image_format = SENSOR_IMAGE_FORMAT_YUV422}}
    }
/*If there are multiple modules,please add here*/
};


static SENSOR_TRIM_T s_gc0310_resolution_trim_tab[VENDOR_NUM] = {
    {
     .module_id = MODULE_SUNNY,
     .trim_info = {
        {
         .trim_start_x = 0,  .trim_start_y = 0,
         .trim_width = 640, .trim_height = 480,
         .line_time = 0, .bps_per_lane = 0,
         .frame_line = 0,
         .scaler_trim = { .x = 0, .y = 0, .w = 640, .h = 480}},
        {
         .trim_start_x = 0,.trim_start_y = 0,
         .trim_width = 640,.trim_height = 480,
         .line_time = 0,.bps_per_lane = 240,
         .frame_line = 0x03b8,
         .scaler_trim = { .x = 0, .y = 0, .w = 640, .h = 480}},
        }}

    /*If there are multiple modules,please add here*/
};


LOCAL SENSOR_VIDEO_INFO_T s_GC0310_video_info[] = {
    {{{0, 30, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 15, 0, 0}, {0, 30, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL}};



SENSOR_REG_T GC0310_MIPI_brightness_tab[][2] = {
    {{0xd5, 0xd0}, {0xff, 0xff}}, {{0xd5, 0xe0}, {0xff, 0xff}},
    {{0xd5, 0xf0}, {0xff, 0xff}}, {{0xd5, 0x00}, {0xff, 0xff}},
    {{0xd5, 0x20}, {0xff, 0xff}}, {{0xd5, 0x30}, {0xff, 0xff}},
    {{0xd5, 0x40}, {0xff, 0xff}},
};
SENSOR_REG_T * brightness_tab[MODULE_MAX] = {
    [MODULE_SUNNY] = GC0310_MIPI_brightness_tab,
};

LOCAL const SENSOR_REG_T gc0310_video_mode_tab[][18] = {
    /* normal preview mode*/
    {{0xff, 0xff}},
    /* video mode: 15fps */
    {{0xfe, 0x00},
     {0x05, 0x02},
     {0x06, 0xd1},
     {0x07, 0x00},
     {0x08, 0x22},
     {0xfe, 0x01},
     {0x25, 0x00},
     {0x26, 0x6a},
     {0x27, 0x02},
     {0x28, 0x7c},
     {0x29, 0x02},
     {0x2a, 0x7c},
     {0x2b, 0x02},
     {0x2c, 0x7c},
     {0x2d, 0x02},
     {0x2e, 0x7c},
     {0xfe, 0x00},
     {0xff, 0xff}}};

static SENSOR_REG_T * video_mode_tab[MODULE_MAX] = {
    [MODULE_SUNNY] = gc0310_video_mode_tab,
};


SENSOR_REG_T GC0310_MIPI_ev_tab[][4] = {
    {{0xfe, 0x01}, {0x13, 0x18}, {0xfe, 0x00}, {0xff, 0xff}},
    {{0xfe, 0x01}, {0x13, 0x20}, {0xfe, 0x00}, {0xff, 0xff}},
    {{0xfe, 0x01}, {0x13, 0x28}, {0xfe, 0x00}, {0xff, 0xff}},
    {{0xfe, 0x01}, {0x13, 0x38}, {0xfe, 0x00}, {0xff, 0xff}},
    {{0xfe, 0x01}, {0x13, 0x40}, {0xfe, 0x00}, {0xff, 0xff}},
    {{0xfe, 0x01}, {0x13, 0x48}, {0xfe, 0x00}, {0xff, 0xff}},
    {{0xfe, 0x01}, {0x13, 0x50}, {0xfe, 0x00}, {0xff, 0xff}},
};
static SENSOR_REG_T * ev_tab[MODULE_MAX] = {
    [MODULE_SUNNY] = GC0310_MIPI_ev_tab,
};

SENSOR_REG_T GC0310_MIPI_awb_tab[][6] = {
    // Auto
    {{0x77, 0x57}, // r gain
     {0x78, 0x4d}, // g gain
     {0x79, 0x45}, // b gain
     {0x42, 0xcf}, // awb_enable[1]
     {0xff, 0xff}},
    // INCANDESCENCE:
    {{0x42, 0xcd}, // Disable AWB
     {0x77, 0x4c},
     {0x78, 0x40},
     {0x79, 0xbb}, // 7b
     {0xff, 0xff}},
    // U30
    {{0x42, 0xcd}, // Disable AWB
     {0x77, 0x40},
     {0x78, 0x54},
     {0x79, 0x70},
     {0xff, 0xff}},
    // CWF
    {{0x42, 0xcd}, // Disable AWB
     {0x77, 0x40},
     {0x78, 0x54},
     {0x79, 0x70},
     {0xff, 0xff}},
    // FLUORESCENT
    {{0x42, 0xcd}, // Disable AWB
     {0x77, 0x60}, // 40
     {0x78, 0x48},
     {0x79, 0x88}, // 50
     {0xff, 0xff}},
    // SUN
    {{0x42, 0xcd}, // Disable AWB
     {0x77, 0x80}, // 50
     {0x78, 0x40},
     {0x79, 0x48}, // 40
     {0xff, 0xff}},
    // CLOUD
    {{0x42, 0xcd}, // Disable AWB
     {0x77, 0xb0},
     {0x78, 0x40}, // 42
     {0x79, 0x40},
     {0xff, 0xff}},
};
static SENSOR_REG_T * awb_tab[MODULE_MAX] = {
    [MODULE_SUNNY] = GC0310_MIPI_awb_tab,
};

SENSOR_REG_T GC0310_MIPI_contrast_tab[][2] = {
    {
        {0xd3, 0x28}, {0xff, 0xff},
    },

    {
        {0xd3, 0x30}, {0xff, 0xff},
    },

    {
        {0xd3, 0x34}, {0xff, 0xff},
    },

    {
        {0xd3, 0x3c}, {0xff, 0xff},
    },

    {
        {0xd3, 0x44}, {0xff, 0xff},
    },

    {
        {0xd3, 0x48}, {0xff, 0xff},
    },

    {
        {0xd3, 0x50}, {0xff, 0xff},
    },
};
static SENSOR_REG_T * contrast_tab[MODULE_MAX] = {
    [MODULE_SUNNY] = GC0310_MIPI_contrast_tab,
};

SENSOR_REG_T GC0310_MIPI_saturation_tab[][3] = {
    {
        {0xd1, 0x10}, {0xd2, 0x10}, {0xff, 0xff},
    },

    {
        {0xd1, 0x18}, {0xd2, 0x18}, {0xff, 0xff},
    },

    {
        {0xd1, 0x20}, {0xd2, 0x20}, {0xff, 0xff},
    },

    {
        {0xd1, 0x28}, {0xd2, 0x28}, {0xff, 0xff},
    },

    {
        {0xd1, 0x40}, {0xd2, 0x40}, {0xff, 0xff},
    },

    {
        {0xd1, 0x48}, {0xd2, 0x48}, {0xff, 0xff},
    },

    {
        {0xd1, 0x50}, {0xd2, 0x50}, {0xff, 0xff},
    },
};
static SENSOR_REG_T * saturation_tab[MODULE_MAX] = {
    [MODULE_SUNNY] = GC0310_MIPI_saturation_tab,
};

SENSOR_REG_T GC0310_MIPI_image_effect_tab[][4] = {
    // effect normal
    {{0x43, 0x00}, {0xda, 0x00}, {0xdb, 0x00}, {0xff, 0xff}},
    // effect BLACKWHITE
    {{0x43, 0x02}, {0xda, 0x00}, {0xdb, 0x00}, {0xff, 0xff}},
    // effect RED pink
    {
        {0x43, 0x02}, {0xda, 0x10}, {0xdb, 0x50}, {0xff, 0xff},
    },
    // effect GREEN
    {
        {0x43, 0x02}, {0xda, 0xc0}, {0xdb, 0xc0}, {0xff, 0xff},
    },
    // effect  BLUE
    {{0x43, 0x02}, {0xda, 0x50}, {0xdb, 0xe0}, {0xff, 0xff}},
    // effect  YELLOW
    {{0x43, 0x02}, {0xda, 0x80}, {0xdb, 0x20}, {0xff, 0xff}},
    // effect NEGATIVE
    {{0x43, 0x01}, {0xda, 0x00}, {0xdb, 0x00}, {0xff, 0xff}},
    // effect ANTIQUE
    {{0x43, 0x02}, {0xda, 0xd2}, {0xdb, 0x28}, {0xff, 0xff}},
};
static SENSOR_REG_T * image_effect_tab[MODULE_MAX] = {
    [MODULE_SUNNY] = GC0310_MIPI_image_effect_tab,
};

static struct sensor_module_info s_GC0310_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {
         .major_i2c_addr = GC0310_MIPI_I2C_ADDR_W >> 1,
         .minor_i2c_addr = GC0310_MIPI_I2C_ADDR_W >> 1,

         .reg_addr_value_bits = SENSOR_I2C_REG_8BIT | SENSOR_I2C_VAL_8BIT |
                                SENSOR_I2C_FREQ_100,

         .avdd_val =  SENSOR_AVDD_2800MV,
         .iovdd_val = SENSOR_AVDD_1800MV,
         .dvdd_val =  SENSOR_AVDD_1800MV,

         .image_pattern = SENSOR_IMAGE_PATTERN_YUV422_YUYV,

         .preview_skip_num = 2,
         .capture_skip_num = 1,
         .flash_capture_skip_num = 0,
         .mipi_cap_skip_num = 0,
         .preview_deci_num = 0,
         .video_preview_deci_num = 0,

         .sensor_interface =
         {
            .type = SENSOR_INTERFACE_TYPE_CSI2,
            .bus_width = 1,
            .pixel_width = 8,
            .is_loose = 1,
         },

         .change_setting_skip_num = 3,
         .horizontal_view_angle = 48,
         .vertical_view_angle = 48
      }
    }

/*If there are multiple modules,please add here*/
};

static struct sensor_ic_ops s_gc0310_ops_tab;

SENSOR_INFO_T g_GC0310_MIPI_yuv_info = {
    .hw_signal_polarity = SENSOR_HW_SIGNAL_PCLK_P | SENSOR_HW_SIGNAL_VSYNC_N |
                          SENSOR_HW_SIGNAL_HSYNC_P,
    .environment_mode = SENSOR_ENVIROMENT_NORMAL | SENSOR_ENVIROMENT_NIGHT |
                        SENSOR_ENVIROMENT_SUNNY,
    .image_effect = SENSOR_IMAGE_EFFECT_NORMAL |
                    SENSOR_IMAGE_EFFECT_BLACKWHITE | SENSOR_IMAGE_EFFECT_RED |
                    SENSOR_IMAGE_EFFECT_GREEN | SENSOR_IMAGE_EFFECT_BLUE |
                    SENSOR_IMAGE_EFFECT_YELLOW | SENSOR_IMAGE_EFFECT_NEGATIVE |
                    SENSOR_IMAGE_EFFECT_CANVAS,
    .wb_mode = 0,
    .step_count = 7,
    .reset_pulse_level = SENSOR_LOW_PULSE_RESET,
    .reset_pulse_width = 100,
    .power_down_level = SENSOR_HIGH_LEVEL_PWDN,
    .identify_count = 2,
    .identify_code =
        { { .reg_addr = 0xf0, .reg_value = 0xa3},
          { .reg_addr = 0xf1, .reg_value = 0x10},
        },

    .source_width_max = 640,
    .source_height_max = 480,
    .name = (cmr_s8 *)"GC0310_MIPI",

    .image_format = SENSOR_IMAGE_FORMAT_YUV422,

    .resolution_tab_info_ptr = s_gc0310_resolution_tab_yuv,
    .sns_ops = &s_gc0310_ops_tab,
    .module_info_tab = s_GC0310_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_GC0310_module_info_tab),

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"gc0310_v1",
};

#endif
