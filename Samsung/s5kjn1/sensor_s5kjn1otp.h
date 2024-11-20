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

#include <utils/Log.h>
#include "sensor.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

#define OTP_AUTO_LOAD_LSC_S5KJN1_SHINE
#define MODULE_NAME       		"Shine"    //module vendor name

#define S5KJN1_EEPROM_I2C_ADDR   0xa0    // EEPROM  address

#define OTP_MODULE_OFFSET  0x00
#define OTP_MODULE_INFO_SIZE  0x10

#define OTP_AF_OFFSET  0x6A
#define OTP_AF_INFO_SIZE  11

#define OTP_AWB_OFFSET  0x75
#define OTP_AWB_INFO_SIZE  14

struct otp_info_t {
    uint16_t module_info_valide;
	uint16_t module_intergrator_id;
	uint16_t sensor_id;

    uint16_t awb_info_valide;
	uint16_t r;
	uint16_t g;
	uint16_t b;
	uint16_t r_golden;
	uint16_t g_golden;
	uint16_t b_golden;
    uint16_t rg_ratio_golden;
    uint16_t bg_ratio_golden;

    uint16_t af_info_valide;
	uint16_t af_MAC;
	uint16_t af_MIDDLE;
	uint16_t af_INFI;
};

struct otp_info_t s_s5kjn1_otp_info={0x00};

typedef enum{
	otp_close=0,
	otp_open,
}otp_state;

static cmr_int s5kjn1_i2c_read_16bit(cmr_handle handle, uint16_t slave_addr,
                                    uint16_t memory_addr, uint8_t *memory_data){
    cmr_int ret = SENSOR_SUCCESS;
    CHECK_PTR(handle);

    uint8_t cmd_val[5] = { 0x00 };
    uint16_t cmd_len = 0;

    cmd_val[0] = (memory_addr>>8);
    cmd_val[1] = (memory_addr & 0xff);
    cmd_len = 2;

    ret = hw_sensor_read_i2c(handle, slave_addr, (uint8_t*)&cmd_val[0], cmd_len);
    if (SENSOR_SUCCESS == ret) {
        *memory_data  = cmd_val[0];
    }
    return ret;
}

static cmr_int s5kjn1_i2c_write_16bit(cmr_handle handle, uint16_t slave_addr,
                                    uint16_t memory_addr, uint16_t memory_data){
    cmr_int ret = SENSOR_SUCCESS;
    CHECK_PTR(handle);

    uint8_t cmd_val[5] = { 0x00 };
    uint16_t cmd_len = 0;

    cmd_val[0] = memory_addr & 0xff;
    cmd_val[1] = memory_data;
    cmd_len = 2;
    ret = hw_sensor_write_i2c(handle, slave_addr, (uint8_t *)&cmd_val[0], cmd_len);

    return ret;
}

static cmr_int s5kjn1_common_section_checksum(cmr_u8 *cmd_buffer, unsigned char *buf,
                                           unsigned int start_addr, unsigned int data_count,
                                           unsigned int checkSum_addr) {
    cmr_int ret = SENSOR_SUCCESS;
    uint32_t i = 0, sum = 0;
    OTP_LOGI("in");

    for (i = 0; i < data_count; i++) {
        buf[i] = cmd_buffer[start_addr + i];
        sum += buf[i];
    }
    uint32_t checkSum = cmd_buffer[checkSum_addr];
    if ((sum % 256) == checkSum) {
        ret = SENSOR_SUCCESS;
    } else {
        ret = SENSOR_FAIL;
    }
    OTP_LOGI("_s5kjn1_otp_section_checks: data_count:%d, sum:0x%x, checksum:0x%x,ret =%d", data_count, sum,checkSum,ret);
    return ret;
}


static uint16_t s5kjn1_read_otp(cmr_handle handle)
{
	cmr_int ret_value = SENSOR_FAIL;
    cmr_u16 index = 0;
    cmr_u8 *module_info_buff = NULL;
    cmr_u8 *af_read_info_buff = NULL;
    cmr_u8 *awb_read_info_buff = NULL;
    cmr_u8 r,g,b,r_golden,b_golden,g_golden;
    module_info_buff = (cmr_u8 *)malloc(OTP_MODULE_INFO_SIZE*sizeof(cmr_u8));
    af_read_info_buff = (cmr_u8 *)malloc(OTP_AF_INFO_SIZE*sizeof(cmr_u8));
    awb_read_info_buff = (cmr_u8 *)malloc(OTP_AWB_INFO_SIZE*sizeof(cmr_u8));

    if(module_info_buff == NULL || af_read_info_buff == NULL || awb_read_info_buff == NULL)
    {
        if(module_info_buff != NULL) {
            free(module_info_buff);
        }
        if(af_read_info_buff != NULL) {
            free(af_read_info_buff);
        }
        if(awb_read_info_buff != NULL) {
            free(awb_read_info_buff);
        }
        CMR_LOGE("s5kjn1_read_otp  malloc fail, OTP read fail!!!");
        return  ret_value;
    }
    memset(module_info_buff, 0, OTP_MODULE_INFO_SIZE*sizeof(cmr_u8));
    memset(af_read_info_buff, 0, OTP_AF_INFO_SIZE*sizeof(cmr_u8));
    memset(awb_read_info_buff, 0, OTP_AWB_INFO_SIZE*sizeof(cmr_u8));

    /*�̨���????����?��?����*/
    cmr_int ret1 = hw_sensor_read_i2c(handle, S5KJN1_EEPROM_I2C_ADDR >> 1,
                    module_info_buff, SENSOR_I2C_REG_16BIT | SENSOR_I2C_VAL_8BIT | OTP_MODULE_INFO_SIZE << 16);
    if(ret1 == CMR_CAMERA_FAIL)
    {
        CMR_LOGE("s5kjn1_read_otp  hw_sensor_read_i2c fail, OTP read fail!!!");
        if (module_info_buff != NULL) {
            free(module_info_buff);
            module_info_buff = NULL;
        }
        if(af_read_info_buff != NULL) {
            free(af_read_info_buff);
        }
        if(awb_read_info_buff != NULL) {
            free(awb_read_info_buff);
        }
        return  ret_value;
    }

    // read OTP into buffer
    s_s5kjn1_otp_info.module_info_valide = module_info_buff[0];

    //Module Information
    if(0x01 == s_s5kjn1_otp_info.module_info_valide){
        ret_value = SENSOR_SUCCESS;
        unsigned char module_info_buf[16] = {0};
        ret_value = s5kjn1_common_section_checksum(module_info_buff,module_info_buf,0x0001,14,0x000f);
        if(SENSOR_SUCCESS == ret_value){
            s_s5kjn1_otp_info.module_intergrator_id = (uint16_t)module_info_buf[0];
            s_s5kjn1_otp_info.sensor_id = (uint16_t)module_info_buf[5];
        }
        else{
            s_s5kjn1_otp_info.module_info_valide = 0;
        }
        CMR_LOGI("s5kjn1_read_otp  module_info   module_intergrator_id =%d,  sensor_id=%d,  ret_value = %d",
                            s_s5kjn1_otp_info.module_intergrator_id, s_s5kjn1_otp_info.sensor_id, ret_value);
    }
    CMR_LOGI("s5kjn1_read_otp  module_info_valide = %d",s_s5kjn1_otp_info.module_info_valide);

    /*�̨�?t???����?��?����*/
    for(index = 0; index < OTP_AWB_INFO_SIZE; index++){
        s5kjn1_i2c_read_16bit(handle, S5KJN1_EEPROM_I2C_ADDR >> 1, OTP_AWB_OFFSET + index, &(awb_read_info_buff[index]));
        CMR_LOGI("AWB addr = 0x%x,  value =0x%x ",OTP_AWB_OFFSET + index,awb_read_info_buff[index]);
    }
    //AWB Information
    s_s5kjn1_otp_info.awb_info_valide = awb_read_info_buff[0];
    if(SENSOR_SUCCESS == ret_value){
        unsigned char awb_info_buf[14] = {0};
        ret_value = s5kjn1_common_section_checksum(awb_read_info_buff,awb_info_buf,0x0,13,13);
        if(SENSOR_SUCCESS == ret_value){
            s_s5kjn1_otp_info.awb_info_valide = 0x1;
            s_s5kjn1_otp_info.r = (uint16_t)((awb_info_buf[2] << 8) + awb_info_buf[1]);
            s_s5kjn1_otp_info.g = (uint16_t)((awb_info_buf[4] << 8) + awb_info_buf[3]);
            s_s5kjn1_otp_info.b = (uint16_t)((awb_info_buf[6] << 8) + awb_info_buf[5]);
            s_s5kjn1_otp_info.r_golden= (uint16_t)((awb_info_buf[8] << 8) + awb_info_buf[7]);
            s_s5kjn1_otp_info.g_golden= (uint16_t)((awb_info_buf[10] << 8) + awb_info_buf[9]);
            s_s5kjn1_otp_info.b_golden= (uint16_t)((awb_info_buf[12] << 8) + awb_info_buf[11]);
        }
        else{
            s_s5kjn1_otp_info.awb_info_valide = 0;
        }
        CMR_LOGI("s5kjn1_read_otp  AWB info r= %d,g= %d,b= %d, r_golden= %d,g_golden=%d,b_golden=%d,ret_value = %d",
            s_s5kjn1_otp_info.r,s_s5kjn1_otp_info.g,s_s5kjn1_otp_info.b,
            s_s5kjn1_otp_info.r_golden,s_s5kjn1_otp_info.g_golden,s_s5kjn1_otp_info.b_golden,ret_value);
    }
    CMR_LOGI("s5kjn1_read_otp  awb_info_valide = %d",s_s5kjn1_otp_info.awb_info_valide);

    for(index = 0; index < OTP_AF_INFO_SIZE; index++){
        s5kjn1_i2c_read_16bit(handle, S5KJN1_EEPROM_I2C_ADDR >> 1, OTP_AF_OFFSET + index, &(af_read_info_buff[index]));
        CMR_LOGI("af addr = 0x%x,  value =0x%x ",OTP_AF_OFFSET + index,af_read_info_buff[index]);
    }

    //AF Information
    s_s5kjn1_otp_info.af_info_valide = af_read_info_buff[0];
    unsigned char af_info_buf[11] = {0};
    ret_value = s5kjn1_common_section_checksum(af_read_info_buff,af_info_buf,0,10,10);
    if(SENSOR_SUCCESS == ret_value){
        s_s5kjn1_otp_info.af_info_valide = af_info_buf[0];
        s_s5kjn1_otp_info.af_MAC = (uint16_t)((af_info_buf[4] << 8) + af_info_buf[3]);
        s_s5kjn1_otp_info.af_MIDDLE= (uint16_t)((af_info_buf[7] << 8) + af_info_buf[6]);
        s_s5kjn1_otp_info.af_INFI = (uint16_t)((af_info_buf[2] << 8) + af_info_buf[1]);
        CMR_LOGI("s_s5kjn1_otp_info.af_MAC =0x%x,  s_s5kjn1_otp_info.af_INFI=0x%x",
                                        s_s5kjn1_otp_info.af_MAC,s_s5kjn1_otp_info.af_INFI);
    }
    CMR_LOGI("af info   ret_value = %d ",ret_value);

    if (module_info_buff != NULL) {
        free(module_info_buff);
        module_info_buff = NULL;
    }
    if (af_read_info_buff != NULL) {
        free(af_read_info_buff);
        af_read_info_buff = NULL;
    }
    if (awb_read_info_buff != NULL) {
        free(awb_read_info_buff);
        awb_read_info_buff = NULL;
    }
    return  ret_value;
}

static cmr_int s5kjn1_readotp_awb_info(cmr_handle handle, void *param_ptr){
    cmr_int ret_value = SENSOR_FAIL;


    uint32_t *otp_ptr = (uint32_t *)param_ptr;
	  uint16_t r_gain = 0x100, g_gain = 0x100, b_gain = 0x100;
	  uint16_t r_gain_current = 0, g_gain_current = 0, b_gain_current = 0, base_gain = 0;
	  uint16_t RG_GAIN, BG_GAIN, GG_GAIN, RG_GOLDEN_GAIN, BG_GOLDEN_GAIN, GG_GOLDEN_GAIN;
/*del the useful of the calculation of the awb,gain ping.luan@transsion.com,2021-05-12*/
#if 0
    // apply OTP WB Calibration
    if ((0x01 == s_s5kjn1_otp_info.module_info_valide) && (0x01 == s_s5kjn1_otp_info.awb_info_valide)) {
        //calculate G gain
        s_s5kjn1_otp_info.rg_ratio_golden = (s_s5kjn1_otp_info.r_golden*1000) / (s_s5kjn1_otp_info.g_golden);
        s_s5kjn1_otp_info.bg_ratio_golden = (s_s5kjn1_otp_info.b_golden*1000) / (s_s5kjn1_otp_info.g_golden);
    }
    else
    {
        CMR_LOGE("s5kjn1_readotp_awb_info   module_info_valide = %d",s_s5kjn1_otp_info.module_info_valide);
        return ret_value;
    }
#endif
/*del end. ping.luan@transsion.com,2021-05-12*/
    	  RG_GOLDEN_GAIN = s_s5kjn1_otp_info.r_golden;
    	  BG_GOLDEN_GAIN = s_s5kjn1_otp_info.b_golden;
		  GG_GOLDEN_GAIN = s_s5kjn1_otp_info.g_golden;

    	  RG_GAIN = s_s5kjn1_otp_info.r;
    	  BG_GAIN = s_s5kjn1_otp_info.b;
      	  GG_GAIN = s_s5kjn1_otp_info.g;

        r_gain_current = 1024 * RG_GOLDEN_GAIN / RG_GAIN;
        b_gain_current = 1024 * BG_GOLDEN_GAIN / BG_GAIN;
        g_gain_current = 1024 * GG_GOLDEN_GAIN / GG_GAIN;

        base_gain = (r_gain_current < b_gain_current) ? r_gain_current : b_gain_current;
        base_gain = (base_gain < g_gain_current) ? base_gain : g_gain_current;

        r_gain = 0x400 * r_gain_current / base_gain;//0x100,modify the coefficient of the gain gang.zhang@transsion.com,2021-03-22
        g_gain = 0x400 * g_gain_current / base_gain;//0x100,modify the coefficient of the gain gang.zhang@transsion.com,2021-03-22
        b_gain = 0x400 * b_gain_current / base_gain;//0x100,modify the coefficient of the gain gang.zhang@transsion.com,2021-03-22
	CMR_LOGI("S5KJN1_OTP_UPDATE_AWB: RG_GAIN = 0x%x, BG_GAIN = 0x%x,r_gain = 0x%x, g_gain = 0x%x, b_gain = 0x%x\n", RG_GAIN, BG_GAIN, r_gain, g_gain, b_gain);
    //static int32_t engeneer_otp[2][7];
    *otp_ptr++ = RG_GAIN;
    *otp_ptr++ = BG_GAIN;
    *otp_ptr++ = r_gain;
    *otp_ptr++ = b_gain;
    *otp_ptr++ = g_gain;
    *otp_ptr++ = s_s5kjn1_otp_info.af_INFI;
    *otp_ptr++ = s_s5kjn1_otp_info.af_MAC;

    return SENSOR_SUCCESS;
}

#define XTALK_START_OFFSET 0x1A40
#define OTP_XTC_SIZE 8354
#define OTP_GGC_OFFSET 0x3987
#define OTP_GGC_SIZE 346
cmr_u8 xtc_data_buf[OTP_XTC_SIZE] = {0x00};

static cmr_int s5kjn1_readotp_GGC_write(cmr_handle handle){
    cmr_int rtn = SENSOR_SUCCESS;
    int sum,i;
    uint16_t gcc ;
   
   //read XTC from EPPROM
	xtc_data_buf[0] = (XTALK_START_OFFSET & 0xff00 ) >> 8;
	xtc_data_buf[1] = XTALK_START_OFFSET & 0x00ff;
	rtn = hw_sensor_read_i2c(handle, 0xa0 >> 1, xtc_data_buf,
                       OTP_XTC_SIZE << 16 | SENSOR_I2C_REG_16BIT);
	SENSOR_LOGI("read xtalk data return %d", rtn);
    if (rtn == HW_SUCCESS) {
    SENSOR_LOGI("read xtalk data  success return %d", rtn);
    }else{
    SENSOR_LOGI("read xtalk data  fail return %d", rtn); 
    return SENSOR_FAIL;
    }	
    if (xtc_data_buf[0] == 0x01) {
    CMR_LOGD("XTC flag is vaild!    addr = 0x%x,  XTC_FLAG =0x%x ",XTALK_START_OFFSET ,xtc_data_buf[0]);
    }else{
    CMR_LOGE("XTC flag is invaild!    addr = 0x%x,  XTC_FLAG =0x%x ",XTALK_START_OFFSET ,xtc_data_buf[0]); 
    return SENSOR_FAIL;
    }	
        sum = 0;
		 for(i = 1; i < (OTP_XTC_SIZE - 1); i++) {
			sum += xtc_data_buf[i];
		}
		if ((sum % 256) == xtc_data_buf[OTP_XTC_SIZE - 1]) {
		   SENSOR_LOGE("XTC data checksum is right!   sum = 0x%x,  checksum =0x%x ",sum ,xtc_data_buf[OTP_XTC_SIZE - 1]);
		} else {
            SENSOR_LOGE("XTC data checksum is flase!   sum = 0x%x,  checksum =0x%x ",sum ,xtc_data_buf[OTP_XTC_SIZE - 1]);
			return SENSOR_FAIL;
		}

            // write HW_GGC to sensor 
    hw_sensor_write_reg(handle, 0x6028, 0x2400);
    hw_sensor_write_reg(handle, 0x602A, 0x0CFC);
    for(cmr_u16 index = 0; index < OTP_GGC_SIZE; index += 2){
        gcc= (uint16_t)((xtc_data_buf[OTP_GGC_OFFSET - XTALK_START_OFFSET+index] << 8) + xtc_data_buf[OTP_GGC_OFFSET - XTALK_START_OFFSET+index+1]);
             //CMR_LOGD("gcc = 0x%x ", gcc);
       hw_sensor_write_reg(handle, 0x6F12, gcc);
    }
    /* if need read to check
    hw_sensor_write_reg(handle, 0x602c, 0x2400);
    hw_sensor_write_reg(handle, 0x602e, 0x0CFC);
        for(cmr_u16 index = 0; index < OTP_GGC_SIZE; index += 2){
        gcc= (uint16_t)((GGC_info_buff[index] << 8) + GGC_info_buff[index+1]);
        value = hw_sensor_read_reg(handle, 0x6F12);
        CMR_LOGI("write = 0x%x, addr = 0x%x,  value =0x%x ",gcc,0x0CFC+index,value);
    }
    */
    return SENSOR_SUCCESS;
}