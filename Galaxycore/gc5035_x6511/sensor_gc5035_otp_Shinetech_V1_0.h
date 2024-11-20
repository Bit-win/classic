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

#ifndef _SENSOR_GC5035_OTP_SHINETECH_V1_0_H_
#define _SENSOR_GC5035_OTP_SHINETECH_V1_0_H_

#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

#define LOG_TAG "gc5035_OTP"  

#define OTP_LEN   30

struct otp_info_t {
	uint16_t module_id;
	uint16_t lens_id;
	uint16_t vcm_id;
	uint16_t vcm_driver_id;
	uint16_t year;
	uint16_t month;
	uint16_t day;
	uint16_t rg_ratio_current;
	uint16_t bg_ratio_current;
	uint16_t rg_ratio_typical;
	uint16_t bg_ratio_typical;
	uint16_t wb_flag;// 0:Empty 1:Success 2:Invalid
	uint16_t golden_flag;
	uint16_t golden_rg;
	uint16_t golden_bg;
	uint16_t vcm_dac_medium;
	uint16_t vcm_dac_inifity;
	uint16_t vcm_dac_macro;
	uint16_t w_rgain;
	uint16_t w_bgain;
	uint16_t w_gain;
};

static struct otp_info_t s_gc5035_otp_info={0x00};

static cmr_u8 gc5035_otp_read_byte(cmr_handle handle, cmr_u16 addr)
{
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x02);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x69, (addr >> 8) & 0x1f);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x6a, addr & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf3, 0x20);

	return hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x6c);
}

static void gc5035_otp_read_group(cmr_handle handle, cmr_u16 addr, cmr_u8 *data, cmr_u16 length)
{
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	cmr_u16 i = 0;

	if ((((addr & 0x1fff) >> 3) + length) > GC5035_OTP_DATA_LENGTH) {
		SENSOR_PRINT("out of range, start addr: 0x%.4x, length = %d\n", addr & 0x1fff, length);
		return;
	}

	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x02);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x69, (addr >> 8) & 0x1f);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x6a, addr & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf3, 0x20);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf3, 0x12);

	for (i = 0; i < length; i++)
		data[i] = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x6c);

	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf3, 0x00);
}

static void gc5035_gcore_read_dpc(cmr_handle handle)
{
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	cmr_u32 dpcFlag = 0;
	struct gc5035_dpc_t *pDPC = &gc5035_otp_data.dpc;

	dpcFlag = gc5035_otp_read_byte(handle, GC5035_OTP_DPC_FLAG_OFFSET);
	SENSOR_PRINT("dpc flag = 0x%x\n", dpcFlag);
	switch (GC5035_OTP_GET_2BIT_FLAG(dpcFlag, 0)) {
	case GC5035_OTP_FLAG_EMPTY: {
		SENSOR_PRINT("dpc info is empty!\n");
		pDPC->flag = GC5035_OTP_FLAG_EMPTY;
		break;
	}
	case GC5035_OTP_FLAG_VALID: {
		SENSOR_PRINT("dpc info is valid!\n");
		pDPC->total_num = gc5035_otp_read_byte(handle, GC5035_OTP_DPC_TOTAL_NUMBER_OFFSET)
			+ gc5035_otp_read_byte(handle, GC5035_OTP_DPC_ERROR_NUMBER_OFFSET);
		pDPC->flag = GC5035_OTP_FLAG_VALID;
		SENSOR_PRINT("total_num = %d\n", pDPC->total_num);
		break;
	}
	default:
		pDPC->flag = GC5035_OTP_FLAG_INVALID;
		break;
	}
}

static void gc5035_gcore_read_reg(cmr_handle handle)
{
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	cmr_u8 i = 0;
	cmr_u8 j = 0;
	cmr_u16 base_group = 0;
	cmr_u8 reg[GC5035_OTP_REG_DATA_SIZE];
	struct gc5035_reg_update_t *pRegs = &gc5035_otp_data.regs;

	memset(&reg, 0, GC5035_OTP_REG_DATA_SIZE);
	pRegs->flag = gc5035_otp_read_byte(handle, GC5035_OTP_REG_FLAG_OFFSET);
	SENSOR_PRINT("register update flag = 0x%x\n", pRegs->flag);
	if (pRegs->flag == GC5035_OTP_FLAG_VALID) {
		gc5035_otp_read_group(handle, GC5035_OTP_REG_DATA_OFFSET, &reg[0], GC5035_OTP_REG_DATA_SIZE);

		for (i = 0; i < GC5035_OTP_REG_MAX_GROUP; i++) {
			base_group = i * GC5035_OTP_REG_BYTE_PER_GROUP;
			for (j = 0; j < GC5035_OTP_REG_REG_PER_GROUP; j++)
				if (GC5035_OTP_CHECK_1BIT_FLAG(reg[base_group], (4 * j + 3))) {
					pRegs->reg[pRegs->cnt].page =(reg[base_group] >> (4 * j)) & 0x07;
					pRegs->reg[pRegs->cnt].addr = reg[base_group + j * GC5035_OTP_REG_BYTE_PER_REG + 1];
					pRegs->reg[pRegs->cnt].value =reg[base_group + j * GC5035_OTP_REG_BYTE_PER_REG + 2];
					SENSOR_PRINT("register[%d] P%d:0x%x->0x%x\n",pRegs->cnt, pRegs->reg[pRegs->cnt].page,
						pRegs->reg[pRegs->cnt].addr, pRegs->reg[pRegs->cnt].value);
					pRegs->cnt++;
				}
		}

	}
}

static void gc5035_otp_read_sensor_info(cmr_handle handle)
{
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

#if GC5035_OTP_DEBUG
	cmr_u16 i = 0;
	cmr_u8 debug[GC5035_OTP_DATA_LENGTH];
#endif

	gc5035_gcore_read_dpc(handle);
	gc5035_gcore_read_reg(handle);

#if GC5035_OTP_DEBUG
	memset(&debug[0], 0, GC5035_OTP_DATA_LENGTH);
	gc5035_otp_read_group(handle, GC5035_OTP_START_ADDR, &debug[0], GC5035_OTP_DATA_LENGTH);
	for (i = 0; i < GC5035_OTP_DATA_LENGTH; i++)
		SENSOR_PRINT("addr = 0x%x, data = 0x%x\n", GC5035_OTP_START_ADDR + i * 8, debug[i]);
#endif
}

static void gc5035_otp_update_dd(cmr_handle handle)
{
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	cmr_u8 state = 0;
	cmr_u8 n = 0;
	struct gc5035_dpc_t *pDPC = &gc5035_otp_data.dpc;

	if (GC5035_OTP_FLAG_VALID == pDPC->flag) {
		SENSOR_PRINT("DD auto load start!\n");
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x17, GC5035_MIRROR);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x88);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x10);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x8e);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x88);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x10);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x8e);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x02);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xbe, 0x00);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xa9, 0x01);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x09, 0x33);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x01, (pDPC->total_num >> 8) & 0x07);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x02, pDPC->total_num & 0xff);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x03, 0x00);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x04, 0x80);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x95, 0x0a);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x96, 0x30);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x97, 0x0a);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x98, 0x32);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x99, 0x07);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x9a, 0xa9);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf3, 0x80);
		while (n < 3) {
			state = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x06);
			if ((state | 0xfe) == 0xff)
				usleep(10 *1000);
			else
				n = 3;
			n++;
		}
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xbe, 0x01);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x09, 0x00);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x01);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x80, 0x02);
		hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
	}
}

static void gc5035_otp_update_reg(cmr_handle handle)
{
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	cmr_u8 i = 0;

	SENSOR_PRINT("reg count = %d\n", gc5035_otp_data.regs.cnt);

	if (GC5035_OTP_CHECK_1BIT_FLAG(gc5035_otp_data.regs.flag, 0))
		for (i = 0; i < gc5035_otp_data.regs.cnt; i++) {
			hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, gc5035_otp_data.regs.reg[i].page);
			hw_sensor_write_reg(sns_drv_cxt->hw_handle, gc5035_otp_data.regs.reg[i].addr, gc5035_otp_data.regs.reg[i].value);
			SENSOR_PRINT("reg[%d] P%d:0x%x -> 0x%x\n", i, gc5035_otp_data.regs.reg[i].page,
				gc5035_otp_data.regs.reg[i].addr, gc5035_otp_data.regs.reg[i].value);
		}
}

static void gc5035_otp_update(cmr_handle handle)
{
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

	SENSOR_PRINT("in\n");
	gc5035_otp_update_dd(handle);
	gc5035_otp_update_reg(handle);
}

static void gc5035_gcore_load_data(cmr_handle handle)
{
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	cmr_u8 i = 0;
	cmr_u8 n = 0;
	cmr_u8 state = 0;

	SENSOR_PRINT("in\n");

	/*TODO*/
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x01);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf4, 0x40);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf5, 0xe9);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf6, 0x14);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf8, 0x49);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf9, 0x82);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfa, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x81);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x36, 0x01);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xd3, 0x87);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x36, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x33, 0x00);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x03);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x01, 0xe7);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf7, 0x01);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x8f);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x8f);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x8e);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xee, 0x30);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x87, 0x18);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x01);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x8c, 0x90);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfa, 0x10);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf5, 0xe9);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x02);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x67, 0xc0);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x59, 0x3f);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x55, 0x80);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x65, 0x80);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x66, 0x03);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);

	gc5035_otp_read_sensor_info(handle);
	gc5035_otp_update(handle);

	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x02);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x67, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfa, 0x00);
}

static cmr_u32 gc5035_get_otp_info(cmr_handle handle)
{
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    SENSOR_LOGI("E!\n");
    int i = 0;
    cmr_u8 group1_module_info_valid = 0,group2_module_info_valid = 0;
    cmr_u8 group1_awb_info_valid = 0,group2_awb_info_valid = 0;
    cmr_u8 group1_golden_awb_valid = 0,group2_golden_awb_valid = 0;
    int Checksum = 0;
    cmr_u8 otp_info_buff[OTP_LEN] = {0};
    cmr_u8 module_base_info_buff[6] = {0};
    cmr_u8 awb_info_buff[4] = {0};
    cmr_u8 awb_golden_buff[4] = {0};
    cmr_u8 module_info_valid_flag = 0;
    cmr_u8 awb_valid_group_flag = 0;

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x02);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x69, (0x1f10 >> 8) & 0x1f);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x6a, 0x1f10 & 0xff);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf3, 0x20);
    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf3, 0x12);

	for (i = 0; i < OTP_LEN; i++){
		otp_info_buff[i] = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x6c);
	}

	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf3, 0x00);
#if 1
    for(int i = 13; i < OTP_LEN; i++)
    {
        OTP_LOGI("otp address, value: p1:[0x%x,0x%x]",0x1f10+i*8,otp_info_buff[i]);
    }
#endif
    module_info_valid_flag = otp_info_buff[0]; /* 0x1f10 Module flag */
    if((module_info_valid_flag & 0x3) == 0x01){      /*  0x1f48 WB calibration of group2*/
        for(i = 0; i < 6 ; i++){
            module_base_info_buff[i] = otp_info_buff[7 + i];
        }
        for(i = 0; i < 5 ; i++){
            Checksum += module_base_info_buff[i];
        }
        if(Checksum % 255 == module_base_info_buff[5] - 1){
            group2_module_info_valid = 1;
        }
    }
    if(group2_module_info_valid != 1){
        if(((module_info_valid_flag>>2) & 0x3) == 0x01){ /* 0x1f18  WB calibration of group1*/
            for(i = 0; i < 6 ; i++){
                module_base_info_buff[i] = otp_info_buff[1 + i];
            }
            Checksum = 0;
            for(i = 0; i < 5 ; i++){
                Checksum += module_base_info_buff[i];
            }
            if(Checksum % 255 == module_base_info_buff[5] - 1){
                group1_module_info_valid = 1;
            }
        }
    }
    if((group1_module_info_valid != 1)&&(group2_module_info_valid != 1)){
        OTP_LOGE("Module info otp info invalied Error!!!");
        return  SENSOR_FAIL;
    }

    awb_valid_group_flag = otp_info_buff[13];          /* 0x1f78 WB flag */

    if((awb_valid_group_flag & 0x3) == 0x01){      /*  0x1fa0 WB calibration of group2*/
        for(i = 0; i < 4 ; i++){
            awb_info_buff[i] = otp_info_buff[18 + i];
        }
        Checksum = 0;
        for(i = 0; i < 3 ; i++){
            Checksum += awb_info_buff[i];
        }
        if(Checksum % 255 == awb_info_buff[3] - 1){
            group2_awb_info_valid = 1;
        }
    }
    if(group2_awb_info_valid != 1){
        if(((awb_valid_group_flag>>2) & 0x3) == 0x01){ /* 0x1f80  WB calibration of group1*/
            for(i = 0; i < 4 ; i++){
                awb_info_buff[i] = otp_info_buff[14 + i];
            }
            Checksum = 0;
            for(i = 0; i < 3 ; i++){
                Checksum += awb_info_buff[i];
            }
            if(Checksum % 255 == awb_info_buff[3] - 1){
                group1_awb_info_valid = 1;
            }
        }
    }
    if((1 != group1_awb_info_valid)&&(1 != group2_awb_info_valid)){
        OTP_LOGE("current module awb otp info invalied Error!!!");
        return  SENSOR_FAIL;
    }
    if(((awb_valid_group_flag>>4) & 0x3) == 0x01){   /* 0x1fe0  WB Golden flag group2*/
        for(i = 0; i < 4 ; i++){
            awb_golden_buff[i] = otp_info_buff[26 + i];
        }
        Checksum = 0;
        for(i = 0; i < 3 ; i++){
            Checksum += awb_golden_buff[i];
        }
        if(Checksum % 255 == awb_golden_buff[3] - 1){
            group2_golden_awb_valid = 1;
        }
    }
    if(group2_golden_awb_valid != 1){
        if(((awb_valid_group_flag>>6) & 0x3) == 0x01){ /*0x1fc0  WB Golden flag group1*/
            for(i = 0; i < 4 ; i++){
                awb_golden_buff[i] = otp_info_buff[22 + i];
            }
            Checksum = 0;
            for(i = 0; i < 3 ; i++){
                Checksum += awb_golden_buff[i];
            }
            if(Checksum % 255 == awb_golden_buff[3] - 1){
                group1_golden_awb_valid = 1;
            }
        }
    }
    if((1 != group1_golden_awb_valid)&&(1 != group2_golden_awb_valid)){
        OTP_LOGE("golden module awb otp info invalied Error!!!");
        return  SENSOR_FAIL;
    }

    cmr_u16 rg_ratio_typical = (awb_golden_buff[0] | (awb_golden_buff[1] & 0xf0) << 4);
    cmr_u16 bg_ratio_typical = ((awb_golden_buff[1] & 0x0f) << 8) | (awb_golden_buff[2]);

    cmr_u16 rg_ratio_current = (awb_info_buff[0] | (awb_info_buff[1] & 0xf0) << 4);
    cmr_u16 bg_ratio_current = ((awb_info_buff[1] & 0x0f) << 8) | (awb_info_buff[2]);
    cmr_u16 r_gain_current = 0, g_gain_current = 0, b_gain_current = 0, base_gain = 0;
    cmr_u16 r_gain = 1024, g_gain = 1024, b_gain = 1024;

    rg_ratio_typical = rg_ratio_typical > 0 ? rg_ratio_typical : 0x400;
    bg_ratio_typical = bg_ratio_typical > 0 ? bg_ratio_typical : 0x400;

    rg_ratio_current = rg_ratio_current > 0 ? rg_ratio_current : 0x400;
    bg_ratio_current = bg_ratio_current > 0 ? bg_ratio_current : 0x400;
    OTP_LOGI("rg_ratio_typical = 0x%x, bg_ratio_typical = 0x%x\n", rg_ratio_typical, bg_ratio_typical);
    OTP_LOGI("rg_ratio_current = 0x%x, bg_ratio_current = 0x%x\n", rg_ratio_current, bg_ratio_current);

    r_gain_current = 0x400 * rg_ratio_typical / rg_ratio_current;
    b_gain_current = 0x400 * bg_ratio_typical / bg_ratio_current;
    g_gain_current = 0x400;

    base_gain = r_gain_current < b_gain_current ? r_gain_current : b_gain_current;
    base_gain = base_gain < g_gain_current ? base_gain : g_gain_current;
    OTP_LOGI("r_gain_current = 0x%x, b_gain_current = 0x%x, base_gain = 0x%x\n",
        r_gain_current, b_gain_current, base_gain);

    r_gain = 0x400 * r_gain_current / base_gain;
    g_gain = 0x400 * g_gain_current / base_gain;
    b_gain = 0x400 * b_gain_current / base_gain;
    OTP_LOGI("r_gain = 0x%x, g_gain = 0x%x, b_gain = 0x%x\n", r_gain, g_gain, b_gain);

    s_gc5035_otp_info.module_id = module_base_info_buff[0];
    s_gc5035_otp_info.lens_id = module_base_info_buff[1];
    s_gc5035_otp_info.year = module_base_info_buff[2];
    s_gc5035_otp_info.month = module_base_info_buff[3];
    s_gc5035_otp_info.day = module_base_info_buff[4];
    s_gc5035_otp_info.w_rgain = r_gain;
    s_gc5035_otp_info.w_bgain = b_gain;
    s_gc5035_otp_info.w_gain = g_gain;
    s_gc5035_otp_info.golden_bg = bg_ratio_typical;
    s_gc5035_otp_info.golden_rg = rg_ratio_typical;


    SENSOR_LOGD("X!\n");
    return  SENSOR_SUCCESS;
}

static cmr_int gc5035_get_part_otp(cmr_handle handle, void *param_ptr)
{
    cmr_int ret_value = SENSOR_FAIL;
    uint32_t *otp_ptr = (uint32_t *)param_ptr;

    //static int32_t engeneer_otp[2][7];
    *otp_ptr++ = s_gc5035_otp_info.golden_rg;
    *otp_ptr++ = s_gc5035_otp_info.golden_bg;
    *otp_ptr++ = s_gc5035_otp_info.w_rgain;
    *otp_ptr++ = s_gc5035_otp_info.w_bgain;
    *otp_ptr++ = s_gc5035_otp_info.w_gain;
    *otp_ptr++ = 0;
    *otp_ptr++ = 0;

    return SENSOR_SUCCESS;
}


static cmr_int gc5035_apply_otp(cmr_handle handle)
{
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    if(s_gc5035_otp_info.w_gain == 0){
        return SENSOR_FAIL;
    }

	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x04);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x40, s_gc5035_otp_info.w_gain & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x41, s_gc5035_otp_info.w_rgain & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x42, s_gc5035_otp_info.w_bgain & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x43, s_gc5035_otp_info.w_gain & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x44, s_gc5035_otp_info.w_gain & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x45, s_gc5035_otp_info.w_rgain & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x46, s_gc5035_otp_info.w_bgain & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x47, s_gc5035_otp_info.w_gain & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x48, (s_gc5035_otp_info.w_gain >> 8) & 0x07);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x49, (s_gc5035_otp_info.w_rgain >> 8) & 0x07);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4a, (s_gc5035_otp_info.w_bgain >> 8) & 0x07);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4b, (s_gc5035_otp_info.w_gain >> 8) & 0x07);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4c, (s_gc5035_otp_info.w_gain >> 8) & 0x07);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4d, (s_gc5035_otp_info.w_rgain >> 8) & 0x07);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4e, (s_gc5035_otp_info.w_bgain >> 8) & 0x07);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x4f, (s_gc5035_otp_info.w_gain >> 8) & 0x07);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
    return 0;
}

static uint32_t gc5035_gcore_identify_otp(cmr_handle handle)
{
	uint32_t rtn = SENSOR_SUCCESS;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;

    OTP_LOGI("Enable OTP!"); 
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x01);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf4, 0x40);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf5, 0xe9);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf6, 0x14);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf8, 0x49);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf9, 0x82);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfa, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x81);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x36, 0x01);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xd3, 0x87);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x36, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x33, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf7, 0x01);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfc, 0x8e);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xee, 0x30);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfa, 0x10);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xf5, 0xe9);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x02);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x67, 0xc0);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x59, 0x3f);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x55, 0x80);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x65, 0x80);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x66, 0x03);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);

	rtn = gc5035_get_otp_info(handle);

    OTP_LOGI("Disable OTP!");
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x02);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x67, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfe, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0xfa, 0x00);

	if(rtn!=SENSOR_SUCCESS){
		SENSOR_PRINT_ERR("read otp information failed\n!");
		return rtn;
	} else {
		SENSOR_PRINT("identify otp success!");
	}

	return rtn;
}

#ifdef SENSOR_MATCH_OTPMODULE
static cmr_u8 gc5035_gcore_match_otp_data(cmr_handle handle)
{
    int i = 0;
    int otp_module_info_match_num = 0;

    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    //vendor_1
    otp_module_info_match_num = sizeof(gc5035_otpmodule_ptrarray)/sizeof(gc5035_otpmodule_ptrarray[0]);
    for(i = 0;i < otp_module_info_match_num;i ++){

        if(gc5035_otpmodule_ptrarray[i].module_id== s_gc5035_otp_info.module_id){
            SENSOR_PRINT("gc5035_gcore_match_otp_data Success!   module_id = %d\n", s_gc5035_otp_info.module_id);
            s_gc5035_otp_info_tab[0].otp_info.module_id= s_gc5035_otp_info.module_id;
            s_gc5035_otp_info_tab[0].otp_info.module_vendor_id= 0;
            /*
                s_gc5035_otp_info_tab[0].otp_info.lens_id = s_gc5035_otp_info.lens_id;
                s_gc5035_otp_info_tab[0].otp_info.sensor_id = s_gc5035_otp_info.sensor_id;
                s_gc5035_otp_info_tab[0].otp_info.vcm_id = s_gc5035_otp_info.vcm_id;
                s_gc5035_otp_info_tab[0].otp_info.year = s_gc5035_otp_info.year;
                s_gc5035_otp_info_tab[0].otp_info.month = s_gc5035_otp_info.month;
                s_gc5035_otp_info_tab[0].otp_info.day = s_gc5035_otp_info.day;
                */
            sensor_ic_set_match_otp_info(sns_drv_cxt, ARRAY_SIZE(s_gc5035_otp_info_tab), s_gc5035_otp_info_tab);

            return SENSOR_SUCCESS;
        }
    }
    //vendor_2
    otp_module_info_match_num = sizeof(gc5035_otpmodule_ptrarray_2)/sizeof(gc5035_otpmodule_ptrarray_2[0]);
    for(i = 0;i < otp_module_info_match_num;i ++){

        if(gc5035_otpmodule_ptrarray_2[i].module_id== s_gc5035_otp_info.module_id){
            SENSOR_PRINT("gc5035_gcore_match_otp_data Success!   module_id = %d\n", s_gc5035_otp_info.module_id);
            s_gc5035_otp_info_tab[0].otp_info.module_id= s_gc5035_otp_info.module_id;
            s_gc5035_otp_info_tab[0].otp_info.module_vendor_id= 1;
            sensor_ic_set_match_otp_info(sns_drv_cxt, ARRAY_SIZE(s_gc5035_otp_info_tab), s_gc5035_otp_info_tab);
            return SENSOR_SUCCESS;
        }
    }

    SENSOR_PRINT("gc5035_gcore_match_otp_data FAIL!\n");
    return SENSOR_FAIL;
}
#endif

#endif


