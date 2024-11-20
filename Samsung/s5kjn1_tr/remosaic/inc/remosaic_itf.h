 /*
 * Copyright (c) 2016-2021 Samsung Electronics Ltd.
 * All Rights Reserved.
 * Confidential and Proprietary - Samsung Electronics Ltd.
 */

/**
 * @file remosaic_itf.h
 */

#pragma once
#include <stdio.h>

#ifndef REMOSAIC_ITF_H_
#define REMOSAIC_ITF_H_

#ifdef __cplusplus
extern "C" {
#endif

enum e_remosaic_bayer_order {
    BAYER_BGGR = 0,
    BAYER_GBRG = 1,
    BAYER_GRBG = 2,
    BAYER_RGGB = 3,
    BAYER_MAX
};

struct st_remosaic_param {
    int16_t wb_r_gain;         /**< capture WB R Gain */
    int16_t wb_gr_gain;        /**< capture WB GR Gain */
    int16_t wb_gb_gain;        /**< capture WB GB Gain */
    int16_t wb_b_gain;         /**< capture WB B Gain */
};

struct st_remosaic_gain {
    float total_gain;          /**< capture total Gain */
    int16_t analog_gain;       /**< capture analog Gain */
    int16_t digital_gain;      /**< capture digital Gain */
};

enum {
    RET_OK = 0,
    RET_NG = -1,
};

/**
 * @brief Set Initial value for Remosaic
 * @return int32_t result OK, NG
 */
int32_t remosaic_init(int32_t img_w, int32_t img_h,
                enum e_remosaic_bayer_order bayer_order, int32_t pedestal);


/**
 * @brief Make gain map for remosaic algorithm, call just 1 time
 * @return int32_t result OK, NG
 */
int32_t remosaic_gainmap_gen(void* eep_buf_addr, size_t eep_buf_size);

/**
 * @brief Set parameter for 4pixel remosaic algorithm
 * @return int32_t result OK, NG
 */
int32_t remosaic_process_param_set(struct st_remosaic_param* p_param);

/**
 * @brief Set AD gain for 4pixel remosaic algorithm
 * @return int32_t result OK, NG
 */
int32_t remosaic_process_gain_set(struct st_remosaic_gain* p_param);

/**
 * @brief run 4pixel remosaic algorithm
 * @return int32_t result OK, NG
 */
int32_t remosaic_process(uint8_t* src_buf_addr, size_t src_buf_size,
                uint8_t* dst_buf_addr, size_t dst_buf_size);

/**
 * @brief free and destroy all resource
 */
void remosaic_deinit();

/**
 * @brief debug interface
 * @param opt display remosaic daemon state
 */
void remosaic_debug(int opt);

#ifdef __cplusplus
}
#endif

#endif // REMOSAIC_ITF_H_

