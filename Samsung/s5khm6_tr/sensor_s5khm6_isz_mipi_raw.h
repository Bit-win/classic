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

#ifndef _SENSOR_S5KHM6_MIPI_RAW_H_
#define _SENSOR_S5KHM6_MIPI_RAW_H_

#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"
#include "sensor_s5khm6_dphy_setting.h"
//#include "sprd_fcell_s5khm6.h" //remosaic include
#include "dlfcn.h"


#define VENDOR_NUM 1
#define SENSOR_NAME "s5khm6_mipi_raw"
#define I2C_SLAVE_ADDR 0x20 /* 8bit slave address*/
#define S5KHM6_PID_ADDR 0x0000
#define S5KHM6_PID_VALUE 0x1AD6

#define I2C_OTP_ADDR 0xa0 /* 8bit otp slave address*/
#define I2C_OTP_MODULEID_ADDR 0x01 /* 8bit otp reg address*/

/* sensor parameters begin */

/* effective sensor output image size */

#define PREVIEW_WIDTH 4000
#define PREVIEW_HEIGHT 3000
#define ISZ_PREVIEW_WIDTH 4000
#define ISZ_PREVIEW_HEIGHT 3000
#define SNAPSHOT_WIDTH 11968
#define SNAPSHOT_HEIGHT 8944
#define SLOWMOTION_WIDTH 2000
#define SLOWMOTION_HEIGHT 1500
#define VIDEO_4K_WIDTH 3840
#define VIDEO_4K_HEIGHT 2160

/*Raw Trim parameters*/
#define PREVIEW_TRIM_X 0
#define PREVIEW_TRIM_Y 0
#define PREVIEW_TRIM_W PREVIEW_WIDTH
#define PREVIEW_TRIM_H PREVIEW_HEIGHT
#define ISZ_PREVIEW_TRIM_X 0
#define ISZ_PREVIEW_TRIM_Y 0
#define ISZ_PREVIEW_TRIM_W ISZ_PREVIEW_WIDTH
#define ISZ_PREVIEW_TRIM_H ISZ_PREVIEW_HEIGHT
#define SNAPSHOT_TRIM_X 0
#define SNAPSHOT_TRIM_Y 0
#define SNAPSHOT_TRIM_W SNAPSHOT_WIDTH
#define SNAPSHOT_TRIM_H SNAPSHOT_HEIGHT
#define SLOWMOTION_TRIM_X 0
#define SLOWMOTION_TRIM_Y 0
#define SLOWMOTION_TRIM_W SLOWMOTION_WIDTH
#define SLOWMOTION_TRIM_H SLOWMOTION_HEIGHT
#define VIDEO_4K_TRIM_X 0
#define VIDEO_4K_TRIM_Y 0
#define VIDEO_4K_TRIM_W VIDEO_4K_WIDTH
#define VIDEO_4K_TRIM_H VIDEO_4K_HEIGHT

/*Mipi output*/
#define LANE_NUM 4
#define RAW_BITS 10

#define PREVIEW_MIPI_PER_LANE_BPS 2384   /* 4*Mipi clk */
#define ISZ_PREVIEW_MIPI_PER_LANE_BPS 2384   /* 4*Mipi clk */
#define SNAPSHOT_MIPI_PER_LANE_BPS 2384 /* 4*Mipi clk */
#define SLOWMOTION_MIPI_PER_LANE_BPS 2384     /* 4*Mipi clk */
#define VIDEO_4K_MIPI_PER_LANE_BPS 2384     /* 4*Mipi clk */

/*line time unit: 1ns*/
#define PREVIEW_LINE_TIME 6848
#define ISZ_PREVIEW_LINE_TIME 10273
#define SNAPSHOT_LINE_TIME 13536
#define SLOWMOTION_LINE_TIME 4709
#define VIDEO_4K_LINE_TIME 6850

/* frame length*/
#define PREVIEW_FRAME_LENGTH 4864
#define ISZ_PREVIEW_FRAME_LENGTH 3242
#define SNAPSHOT_FRAME_LENGTH 9228
#define SLOWMOTION_FRAME_LENGTH 5307
#define VIDEO_4K_FRAME_LENGTH 4866

/* please ref your spec */
#define FRAME_OFFSET 25
#define SENSOR_MAX_GAIN 0x800  // x64
#define SENSOR_MAX_GAIN_REMOSAIC 0x0200 // x16
#define SENSOR_BASE_GAIN 0x20
#define SENSOR_MIN_SHUTTER 12

#undef IMAGE_NORMAL_MIRROR
#undef IMAGE_H_MIRROR
#undef IMAGE_V_MIRROR
#undef IMAGE_HV_MIRROR

#define IMAGE_NORMAL_MIRROR

/* please ref your spec
 * 1 : average binning
 * 2 : sum-average binning
 * 4 : sum binning
 */
#define BINNING_FACTOR 1.3

/* please ref spec
 * 1: sensor auto caculate
 * 0: driver caculate
 */
/* sensor parameters end */

/* isp parameters, please don't change it*/
#define ISP_BASE_GAIN 0x80

/* please don't change it */
#define EX_MCLK 24

/* read otp xtc data cfg*/
#define OTP_DATA_START_REG 0x0CD5  //sensor specific isp cali data start reg in otp map
#define OTP_DATA_SIZE 0x1482 //0x2156 - 0x0CD5 + 1, sensor specific isp cali data size
#define OTP_OPC_START_REG (1+960+960+1280)
#define OTP_OPC_DATA_SIZE 2048
static cmr_u8 otp_data_buff[OTP_DATA_SIZE] = {0x00};

/*remosaic cfg*/

static const cmr_u32 sns_4in1_mode[] = {0, 0, 0, 1, 0};

static struct sensor_res_tab_info s_s5khm6_resolution_tab_raw[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
        .reg_tab = {
            {ADDR_AND_LEN_OF_ARRAY(s5khm6_init_setting), PNULL, 0, .width = 0,
            .height = 0, .xclk_to_sensor = EX_MCLK,
            .image_format = SENSOR_IMAGE_FORMAT_RAW,
            .fast_switch_mode = SENSOR_FAST_SWITCH_MODE_NONE,
            .binning_size_mode = SENSOR_BINNING_SIZE_MODE_NONE,
            .is_cropped = 0},
            {ADDR_AND_LEN_OF_ARRAY(s5khm6_preview_setting), PNULL, 0,
            .width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT,
            .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW,
            .fast_switch_mode = SENSOR_FAST_SWITCH_GROUP1_MODE0_S5KHM6_12M_9IN1_BINNING,
            .binning_size_mode = SENSOR_BINNING_SIZE_MODE_9IN1,
            .is_cropped = 0},
            {ADDR_AND_LEN_OF_ARRAY(s5khm6_isz_preview_setting),
            PNULL, 0, .width = ISZ_PREVIEW_WIDTH,
            .height = ISZ_PREVIEW_HEIGHT, .xclk_to_sensor = 24,
            .image_format = SENSOR_IMAGE_FORMAT_RAW,
            .fast_switch_mode = SENSOR_FAST_SWITCH_GROUP1_MODE1_S5KHM6_12M_CROP_FROM_108M,
            .binning_size_mode = SENSOR_BINNING_SIZE_MODE_FULLSIZE,
            .is_cropped = 1, .crop_start_x = 4000,
            .crop_start_y = 3000},
            {ADDR_AND_LEN_OF_ARRAY(s5khm6_snapshot_setting), PNULL, 0,
            .width = SNAPSHOT_WIDTH, .height = SNAPSHOT_HEIGHT,
            .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW,
            .fast_switch_mode = SENSOR_FAST_SWITCH_MODE_NONE,
            .binning_size_mode = SENSOR_BINNING_SIZE_MODE_FULLSIZE,
            .is_cropped = 1, .crop_start_x = 16, .crop_start_y = 28},
            {ADDR_AND_LEN_OF_ARRAY(s5khm6_slowmotion_setting), PNULL, 0,
            .width = SLOWMOTION_WIDTH, .height = SLOWMOTION_HEIGHT,
            .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW},
            {ADDR_AND_LEN_OF_ARRAY(s5khm6_VIDEO_4K_setting), PNULL, 0,
            .width = VIDEO_4K_WIDTH, .height = VIDEO_4K_HEIGHT,
            .xclk_to_sensor = EX_MCLK, .image_format = SENSOR_IMAGE_FORMAT_RAW,
            .fast_switch_mode = SENSOR_FAST_SWITCH_GROUP1_MODE0_S5KHM6_12M_9IN1_BINNING,
            .binning_size_mode = SENSOR_BINNING_SIZE_MODE_9IN1,
            .is_cropped = 0},
        }
    }
    /*If there are multiple modules,please add here*/
};

static SENSOR_TRIM_T s_s5khm6_resolution_trim_tab[VENDOR_NUM] = {
    {
        .module_id = MODULE_SUNNY,
        .trim_info = {
            {0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
            {
            .trim_start_x = PREVIEW_TRIM_X,
            .trim_start_y = PREVIEW_TRIM_Y,
            .trim_width = PREVIEW_TRIM_W,
            .trim_height = PREVIEW_TRIM_H,
            .line_time = PREVIEW_LINE_TIME,
            .bps_per_lane = PREVIEW_MIPI_PER_LANE_BPS,
            .frame_line = PREVIEW_FRAME_LENGTH,
            .scaler_trim = {.x = PREVIEW_TRIM_X,.y = PREVIEW_TRIM_Y,.w = PREVIEW_TRIM_W,.h = PREVIEW_TRIM_H}
            },
            {
            .trim_start_x = ISZ_PREVIEW_TRIM_X,
            .trim_start_y = ISZ_PREVIEW_TRIM_Y,
            .trim_width = ISZ_PREVIEW_TRIM_W,
            .trim_height = ISZ_PREVIEW_TRIM_H,
            .line_time = ISZ_PREVIEW_LINE_TIME,
            .bps_per_lane = ISZ_PREVIEW_MIPI_PER_LANE_BPS,
            .frame_line = ISZ_PREVIEW_FRAME_LENGTH,
            .scaler_trim = {.x = ISZ_PREVIEW_TRIM_X,.y = ISZ_PREVIEW_TRIM_Y,.w = ISZ_PREVIEW_TRIM_W,.h = ISZ_PREVIEW_TRIM_H}
            },
            {
            .trim_start_x = SNAPSHOT_TRIM_X,
            .trim_start_y = SNAPSHOT_TRIM_Y,
            .trim_width = SNAPSHOT_TRIM_W,
            .trim_height = SNAPSHOT_TRIM_H,
            .line_time = SNAPSHOT_LINE_TIME,
            .bps_per_lane = SNAPSHOT_MIPI_PER_LANE_BPS,
            .frame_line = SNAPSHOT_FRAME_LENGTH,
            .scaler_trim = {.x = SNAPSHOT_TRIM_X,.y = SNAPSHOT_TRIM_Y,.w = SNAPSHOT_TRIM_W,.h = SNAPSHOT_TRIM_H}
            },
            {
            .trim_start_x = SLOWMOTION_TRIM_X,
            .trim_start_y = SLOWMOTION_TRIM_Y,
            .trim_width = SLOWMOTION_TRIM_W,
            .trim_height = SLOWMOTION_TRIM_H,
            .line_time = SLOWMOTION_LINE_TIME,
            .bps_per_lane = SLOWMOTION_MIPI_PER_LANE_BPS,
            .frame_line = SLOWMOTION_FRAME_LENGTH,
            .scaler_trim = {.x = SLOWMOTION_TRIM_X,.y = SLOWMOTION_TRIM_Y,.w = SLOWMOTION_TRIM_W,.h = SLOWMOTION_TRIM_H}
            },
            {
            .trim_start_x = VIDEO_4K_TRIM_X,
            .trim_start_y = VIDEO_4K_TRIM_Y,
            .trim_width = VIDEO_4K_TRIM_W,
            .trim_height = VIDEO_4K_TRIM_H,
            .line_time = VIDEO_4K_LINE_TIME,
            .bps_per_lane = VIDEO_4K_MIPI_PER_LANE_BPS,
            .frame_line = VIDEO_4K_FRAME_LENGTH,
            .scaler_trim = {.x = VIDEO_4K_TRIM_X,.y = VIDEO_4K_TRIM_Y,.w = VIDEO_4K_TRIM_W,.h = VIDEO_4K_TRIM_H}
            },
        }
    }
    /*If there are multiple modules,please add here*/
};

static SENSOR_REG_T s5khm6_shutter_reg[] = {
    {0x0202, 0x0100},
    {0x0702, 0x0000},
    {0x0704, 0x0000},
};

static struct sensor_i2c_reg_tab s5khm6_shutter_tab = {
    .settings = s5khm6_shutter_reg, .size = ARRAY_SIZE(s5khm6_shutter_reg),
};

static SENSOR_REG_T s5khm6_again_reg[] = {
    {0x0204, 0x0020},
};

static struct sensor_i2c_reg_tab s5khm6_again_tab = {
    .settings = s5khm6_again_reg, .size = ARRAY_SIZE(s5khm6_again_reg),
};

static SENSOR_REG_T s5khm6_dgain_reg[] = {

};

static struct sensor_i2c_reg_tab s5khm6_dgain_tab = {
    .settings = s5khm6_dgain_reg, .size = ARRAY_SIZE(s5khm6_dgain_reg),
};

static SENSOR_REG_T s5khm6_frame_length_reg[] = {
    {0x0340, 0x23d8},
};

static struct sensor_i2c_reg_tab s5khm6_frame_length_tab = {
    .settings = s5khm6_frame_length_reg,
    .size = ARRAY_SIZE(s5khm6_frame_length_reg),
};

static SENSOR_REG_T s5khm6_grp_hold_start_reg[] = {
    {0x0104, 0x01},
};

static struct sensor_i2c_reg_tab s5khm6_grp_hold_start_tab = {
    .settings = s5khm6_grp_hold_start_reg,
    .size = ARRAY_SIZE(s5khm6_grp_hold_start_reg),
};

static SENSOR_REG_T s5khm6_grp_hold_end_reg[] = {
    {0x0104, 0x00},
};

static struct sensor_i2c_reg_tab s5khm6_grp_hold_end_tab = {
    .settings = s5khm6_grp_hold_end_reg,
    .size = ARRAY_SIZE(s5khm6_grp_hold_end_reg),
};
static struct sensor_aec_i2c_tag s5khm6_aec_info = {
    .slave_addr = (I2C_SLAVE_ADDR >> 1),
    .addr_bits_type = SENSOR_I2C_REG_16BIT,
    .data_bits_type = SENSOR_I2C_VAL_16BIT,
    .shutter = &s5khm6_shutter_tab,
    .again = &s5khm6_again_tab,
    .dgain = &s5khm6_dgain_tab,
    .frame_length = &s5khm6_frame_length_tab,
    .grp_hold_start = &s5khm6_grp_hold_start_tab,
    .grp_hold_end = &s5khm6_grp_hold_end_tab,
};

/* add pdaf info here */

static cmr_u32 s5khm6_pd_sns_mode[] = {0, 1, 1, 0, 0, 1};

static const cmr_u16 s5khm6_pd_is_right[] = {
0, 1
};

static const cmr_u16 s5khm6_pd_col[] = {
0, 1
};

static const cmr_u16 s5khm6_pd_row[] = {
0, 0
};

static const struct pd_pos_info s5khm6_pd_pos_l[] = {
{0, 0}
};

static const struct pd_pos_info s5khm6_pd_pos_r[] = {
{1, 0}
};


struct pdaf_block_descriptor s5khm6_pd_seprator_helper = {
    .block_width = 1,
    .block_height = 2,
    .coordinate_tab = {1, 0},
    .line_width = 1,
    .block_pattern = LINED_UP,
    .pd_line_coordinate = NULL,
};

//pdaf patter for isz pdaf
static const cmr_u16 s5khm6_isz_pd_is_right[] = {
0, 1, 0, 1
};

static const cmr_u16 s5khm6_isz_pd_col[] = {
0, 1, 3, 4
};

static const cmr_u16 s5khm6_isz_pd_row[] = {
1, 1, 4, 4
};

static const struct pd_pos_info s5khm6_isz_pd_pos_l[] = {
{0, 1}, {3, 4}
};

static const struct pd_pos_info s5khm6_isz_pd_pos_r[] = {
{1, 1}, {4, 4}
};


struct pdaf_block_descriptor s5khm6_isz_pd_seprator_helper = {
    .block_width = 1,
    .block_height = 4,
    .coordinate_tab = {1, 0, 1, 0},
    .line_width = 1,
    .block_pattern = LINED_UP,
    .pd_line_coordinate = NULL,
};


struct sensor_pdaf_iszfull_info s5khm6_pd_iszfull_info = {
    .cell_num = 3,
    .effective_pixel_w = 12000,
    .effective_pixel_h = 9000,
    .full_pd_offset_x = 0,
    .full_pd_offset_y = 0,
    .full_pd_area_w = 12000,
    .full_pd_area_h = 9000,
};

struct sensor_pdaf_info s5khm6_pdaf_info_array[] = {
	/* pd setting for sensor mode0 */
	{0},
	/* pd setting for sensor mode1 */
    {
	.pd_is_right = (cmr_u16 *)s5khm6_pd_is_right,
	.pd_pos_row = (cmr_u16 *)s5khm6_pd_row,
	.pd_pos_col = (cmr_u16 *)s5khm6_pd_col,
	.pd_pos_r = (struct pd_pos_info *)s5khm6_pd_pos_r,
	.pd_pos_l = (struct pd_pos_info *)s5khm6_pd_pos_l,
	.pd_pos_size = NUMBER_OF_ARRAY(s5khm6_pd_pos_r),
	.pd_offset_x = 0,
	.pd_offset_y = 0,
	.pd_end_x = 4000,
	.pd_end_y = 3000,
	.pd_block_w = 2,
	.pd_block_h = 4,
	.pd_block_num_x = 2000,
	.pd_block_num_y = 750,
	.pd_density_x = 2,
	.pd_density_y = 4,
	.pd_pitch_x = 8,
	.pd_pitch_y = 8,
	.vch2_info.bypass = 0,
	.vch2_info.vch2_vc = 1,
	.vch2_info.vch2_data_type = 0x30,
	.vch2_info.vch2_mode = 0x02,
	.sns_mode = s5khm6_pd_sns_mode,
	.descriptor = &s5khm6_pd_seprator_helper,
	.sns_orientation = 1, /*1: mirror+flip; 0: normal*/
	.pd_size_w = PREVIEW_WIDTH,
	.pd_size_h = PREVIEW_HEIGHT,
	.image_process_type = PDAF_MAP_DEFAULT, // 0:default; 1:only crop; 2:binning+crop;
	.crop_offset_x = 0,
	.crop_offset_y = 0,
	.pdaf_type = SENSOR_DUALPD_MODE3_ENABLE,
	.full_process_type = PDAF_MAP_BINNING,
	.full_crop_x = 0,
	.full_crop_y = 0,
	.bin_ratio_w = 3,
	.bin_ratio_h = 3,
    },

    {
    /* pd setting for sensor mode2 */
    .pd_is_right = (cmr_u16 *)s5khm6_isz_pd_is_right,  //TBD, full size pd pattern
	.pd_pos_row = (cmr_u16 *)s5khm6_isz_pd_row,  //TBD
	.pd_pos_col = (cmr_u16 *)s5khm6_isz_pd_col,  //TBD
	.pd_pos_r = (struct pd_pos_info *)s5khm6_isz_pd_pos_r,  //TBD
	.pd_pos_l = (struct pd_pos_info *)s5khm6_isz_pd_pos_l,  //TBD
	.pd_pos_size = NUMBER_OF_ARRAY(s5khm6_isz_pd_pos_r),  //TBD
	.pd_offset_x = 2,
	.pd_offset_y = 0,
	.pd_end_x = 3998,
	.pd_end_y = 3000,
	.pd_block_w = 6,
	.pd_block_h = 6,
	.pd_block_num_x = 666,
	.pd_block_num_y = 500,
	.pd_density_x = 6,
	.pd_density_y = 3,
	.pd_pitch_x = 8,
	.pd_pitch_y = 8,
	.vch2_info.bypass = 0,
	.vch2_info.vch2_vc = 1,
	.vch2_info.vch2_data_type = 0x30,
	.vch2_info.vch2_mode = 0x02,
	.sns_mode = s5khm6_pd_sns_mode,
	.descriptor = &s5khm6_isz_pd_seprator_helper,
	.sns_orientation = 1, /*1: mirror+flip; 0: normal*/
    .pd_size_w = PREVIEW_WIDTH,
	.pd_size_h = PREVIEW_HEIGHT,
	.image_process_type = 0, // 0:default; 1:only crop; 2:binning+crop;
	.crop_offset_x = 0,
	.crop_offset_y = 0,
    .pdaf_type = SENSOR_PDAF_TYPE2_ENABLE,  //TBD, isz pdaf type
    .full_process_type = PDAF_MAP_CROP,
    .full_crop_x = 3996,
    .full_crop_y = 3000,
    .bin_ratio_w = 1,
    .bin_ratio_h = 1,
    },
	
    /* pd setting for sensor mode3 */
	{0},

    
	/* pd setting for sensor mode4 */
	{0},
	/* pd setting for sensor mode5 */
    {
	.pd_is_right = (cmr_u16 *)s5khm6_pd_is_right,
	.pd_pos_row = (cmr_u16 *)s5khm6_pd_row,
	.pd_pos_col = (cmr_u16 *)s5khm6_pd_col,
	.pd_pos_r = (struct pd_pos_info *)s5khm6_pd_pos_r,
	.pd_pos_l = (struct pd_pos_info *)s5khm6_pd_pos_l,
	.pd_pos_size = NUMBER_OF_ARRAY(s5khm6_pd_pos_r),
	.pd_offset_x = 0,
	.pd_offset_y = 0,
	.pd_end_x = 3840,
	.pd_end_y = 2160,
	.pd_block_w = -2,
	.pd_block_h = -1,
	.pd_block_num_x = 1920,
	.pd_block_num_y = 540,
	.pd_density_x = 2,
	.pd_density_y = 4,
	.pd_pitch_x = 8,
	.pd_pitch_y = 8,
	.vch2_info.bypass = 0,
	.vch2_info.vch2_vc = 1,
	.vch2_info.vch2_data_type = 0x30,
	.vch2_info.vch2_mode = 0x02,
	.sns_mode = s5khm6_pd_sns_mode,
	.descriptor = &s5khm6_pd_seprator_helper,
	.sns_orientation = 1, /*1: mirror+flip; 0: normal*/
    .pd_size_w = PREVIEW_WIDTH,
	.pd_size_h = PREVIEW_HEIGHT,
	.image_process_type = PDAF_MAP_CROP, // 0:default; 1:only crop; 2:binning+crop;
	.crop_offset_x = 80,
	.crop_offset_y = 420,
    .pdaf_type = SENSOR_DUALPD_MODE3_ENABLE,
    .full_process_type = PDAF_MAP_BINNING_CROP,
    .full_crop_x = 80,
    .full_crop_y = 420,
    .bin_ratio_w = 3,
    .bin_ratio_h = 3,},
	/* add more pdaf setting here */
};

static struct sensor_pdaf_info_tab s5khm6_pdaf_info_tab = {
    .sns_mode_num = NUMBER_OF_ARRAY(s5khm6_pd_sns_mode),
	.pd_sns_mode = s5khm6_pd_sns_mode,
	.pdaf_init_index = 1,
	.pd_init_size_w = PREVIEW_WIDTH,
	.pd_init_size_h = PREVIEW_HEIGHT,
	.pd_info_tab = s5khm6_pdaf_info_array,
    .isz_pdaf_index = 2,
    .pd_iszfull_info = &s5khm6_pd_iszfull_info,
};
/*fov f_num cfg*/
static EXIF_SPEC_PIC_TAKING_COND_T s_s5khm6_exif_info;
/*static SENSOR_FOV_INFO_T s_s5khm6_fov_info[] = {

};*/

static SENSOR_STATIC_INFO_T s_s5khm6_static_info[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,  //s5khm6_1849
     .static_info = {.f_num = 165,
                     .focal_length = 533,
                     .max_fps = 30,
                     .max_adgain = 16,
                     .ois_supported = 0,
                     .pdaf_supported = SENSOR_DUALPD_MODE3_ENABLE,
                     .long_expose_supported = 1,    //Pro mode :long_expose funtion
                     .zsl_long_expose_supported = 1,
                     .exp_valid_frame_num = 1,
                     .clamp_level = 64,
                     .adgain_valid_frame_num = 1,
                     .fov_info = {{7.68f, 5.76f}, 5.33f}}
    },
};

static SENSOR_MODE_FPS_INFO_T s_s5khm6_mode_fps_info[VENDOR_NUM] = {
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

static struct sensor_module_info s_s5khm6_module_info_tab[VENDOR_NUM] = {
    {.module_id = MODULE_SUNNY,
     .module_info = {.major_i2c_addr = I2C_SLAVE_ADDR >> 1,
                     .minor_i2c_addr = I2C_SLAVE_ADDR >> 1,
                     .i2c_burst_mode = SENSOR_I2C_BURST_REG16_VAL16,

                     .reg_addr_value_bits = SENSOR_I2C_REG_16BIT |
                                            SENSOR_I2C_VAL_16BIT |
                                            SENSOR_I2C_FREQ_1000,

                     .avdd_val = SENSOR_AVDD_2200MV,
                     .iovdd_val = SENSOR_AVDD_1800MV,
                     .dvdd_val = SENSOR_AVDD_1000MV,

                     .image_pattern = SENSOR_IMAGE_PATTERN_RAWRGB_GB,

                     .preview_skip_num = 0,
                     .capture_skip_num = 0,
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

static struct sensor_ic_ops s_s5khm6_ops_tab;
struct sensor_raw_info *s_s5khm6_mipi_raw_info_ptr =
    PNULL; //&s_s5khm6_mipi_raw_info;

/*==============================================================================
 * Description:
 * sensor all info
 * please modify this variable acording your spec
 *============================================================================*/
SENSOR_INFO_T g_s5khm6_mipi_raw_info = {
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
    .identify_code = {{.reg_addr =S5KHM6_PID_ADDR,
                       .reg_value =S5KHM6_PID_VALUE},},

    .source_width_max = SNAPSHOT_WIDTH,
    .source_height_max = SNAPSHOT_HEIGHT,
    .true_fullsize_width = 12000,
    .true_fullsize_height = 9000,
    .fast_switch_zoom_ratio = 3,
    .name = (cmr_s8 *)SENSOR_NAME,
    .image_format = SENSOR_IMAGE_FORMAT_RAW,

    .module_info_tab = s_s5khm6_module_info_tab,
    .module_info_tab_size = ARRAY_SIZE(s_s5khm6_module_info_tab),

    .resolution_tab_info_ptr = s_s5khm6_resolution_tab_raw,
    .sns_ops = &s_s5khm6_ops_tab,
    .raw_info_ptr = &s_s5khm6_mipi_raw_info_ptr,

    .video_tab_info_ptr = NULL,
    .sensor_version_info = (cmr_s8 *)"s5khm6_v1",
};

#endif
