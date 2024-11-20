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
#include <cutils/properties.h>

#define I2C_SLAVE_ADDR 0x20
#define VENDORID_JK 0x08

#define GROUP1ADDR 0x827A
#define STARTADDR1 0x8200
#define ENDADDR1 0x83FF

#define GROUP2ADDR 0x8C7A
#define STARTADDR2 0x8C00
#define ENDADDR2 0x8DFF

static uint16_t Sensor_readreg8bits(cmr_handle handle, uint16_t addr) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    uint8_t cmd_val[5] = {0x00};
    uint16_t slave_addr = 0;
    uint16_t cmd_len = 0;
    uint16_t ret_value = SENSOR_SUCCESS;

    slave_addr = I2C_SLAVE_ADDR >> 1;

    uint16_t reg_value = 0;
    cmd_val[0] = addr >> 8;
    cmd_val[1] = addr & 0xff;
    cmd_len = 2;
    ret_value = hw_sensor_read_i2c(sns_drv_cxt->hw_handle, slave_addr,
                                   (uint8_t *)&cmd_val[0], cmd_len);
    if (SENSOR_SUCCESS == ret_value) {
        reg_value = cmd_val[0];
    }
    return reg_value;
}
static uint16_t Sensor_writereg8bits(cmr_handle handle, uint16_t addr,
                                     uint8_t val) {
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    uint8_t cmd_val[5] = {0x00};
    uint16_t slave_addr = 0;
    uint16_t cmd_len = 0;
    uint16_t ret_value = SENSOR_SUCCESS;

    slave_addr = I2C_SLAVE_ADDR >> 1;

    cmd_val[0] = addr >> 8;
    cmd_val[1] = addr & 0xff;
    cmd_val[2] = val;
    cmd_len = 3;
    ret_value = hw_sensor_write_i2c(sns_drv_cxt->hw_handle, slave_addr,
                                    (uint8_t *)&cmd_val[0], cmd_len);

    return ret_value;
}

cmr_u8 sc820cs_Sensor_OTP_read(cmr_handle handle, uint16_t otp_addr) {
    SENSOR_IC_CHECK_HANDLE(handle);
    return Sensor_readreg8bits(handle, otp_addr); // OTP data read
}

static uint16_t sc820cs_read_otp_info(cmr_handle handle) {

    uint16_t vendorid,flag= 0;
    SENSOR_IC_CHECK_HANDLE(handle);
    struct sensor_ic_drv_cxt *sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    cmr_u8 otp_buffer[10];
    cmr_u8 temp[2];
    cmr_bzero(otp_buffer, sizeof(otp_buffer));

    uint16_t reg_offset;
    uint16_t i;

    /*OTP Initial Setting,read buffer*/
    SENSOR_PRINT("init group2");
    Sensor_writereg8bits(handle, 0x36b0, 0x48);
    Sensor_writereg8bits(handle, 0x36b1, 0x38);
    Sensor_writereg8bits(handle, 0x36b2, 0x41);
    Sensor_writereg8bits(handle, 0x4408, (STARTADDR2 >> 8)&0xff);
    Sensor_writereg8bits(handle, 0x4409, (STARTADDR2)&0xff);
    Sensor_writereg8bits(handle, 0x440a, (ENDADDR2 >> 8)&0xff);
    Sensor_writereg8bits(handle, 0x440b, (ENDADDR2)&0xff);
    Sensor_writereg8bits(handle, 0x4401, 0x13);
    Sensor_writereg8bits(handle, 0x4412, 0x0D);
    Sensor_writereg8bits(handle, 0x4407, 0x00);
    Sensor_writereg8bits(handle, 0x4400, 0x11);

    usleep(10 * 1000);
    // read otp
    reg_offset = GROUP2ADDR;
    for (i = 0; i < 6; i++) {
        otp_buffer[i] = sc820cs_Sensor_OTP_read(handle, reg_offset + i);
        SENSOR_PRINT("first six reg value %d:0x%x\n", i,otp_buffer[i]);
    }
    flag = otp_buffer[0];
    SENSOR_PRINT("groupflag:0x%x",flag);
    if(1 == flag){
        SENSOR_PRINT("group2 valid");
        vendorid = otp_buffer[1];
        return vendorid;
    }else{
        SENSOR_PRINT("group invalid,need read group1");
        Sensor_writereg8bits(handle, 0x36b0, 0x48);
        Sensor_writereg8bits(handle, 0x36b1, 0x38);
        Sensor_writereg8bits(handle, 0x36b2, 0x41);
        Sensor_writereg8bits(handle, 0x4408, (STARTADDR1 >> 8)&0xff);
        Sensor_writereg8bits(handle, 0x4409, (STARTADDR1)&0xff);
        Sensor_writereg8bits(handle, 0x440a, (ENDADDR1 >> 8)&0xff);
        Sensor_writereg8bits(handle, 0x440b, (ENDADDR1)&0xff);
        Sensor_writereg8bits(handle, 0x4401, 0x13);
        Sensor_writereg8bits(handle, 0x4412, 0x03);
        Sensor_writereg8bits(handle, 0x4407, 0x00);
        Sensor_writereg8bits(handle, 0x4400, 0x11);
        usleep(10 * 1000);

        reg_offset = GROUP1ADDR;
        for (i = 0; i < 6; i++) {
            otp_buffer[i] = sc820cs_Sensor_OTP_read(handle, reg_offset + i);
            SENSOR_PRINT("first six reg value %d:0x%x\n", i,otp_buffer[i]);
        }
        flag = otp_buffer[0];
        SENSOR_PRINT("groupflag:0x%x",flag);
        if(1 == flag){
            SENSOR_PRINT("group1 valid");
            vendorid = otp_buffer[1];
            return vendorid;
        }else{
            SENSOR_PRINT("all group flag invalid need call vendor_kingcome");
            return 0;
        }
    return 0;
    }
}
