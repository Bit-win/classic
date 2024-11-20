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

#ifdef MIT_CAM_MODULE_INFO_TRANSSION

#define gc08a3_OTP_AWB_FLAG_OFFSET 0x8c78
#define gc08a3_OTP_AF_FLAG_OFFSET 0x8c80
#define gc08a3_OTP_AWB_SIZE 14
#define gc08a3_OTP_AF_SIZE 11
#define gc08a3_OTP_AWB_OFFSET 0x8d38
#define gc08a3_OTP_AF_OFFSET 0x8ce0
#define gc08a3_GROUP_SIZE 1826

cmr_u8 AWB_buffer[gc08a3_OTP_AWB_SIZE];
cmr_u8 AF_buffer[gc08a3_OTP_AF_SIZE];
cmr_u8 AF_flag;
cmr_u8 AWB_flag;


static void gc08a3_otp_read_groups(cmr_handle handle, cmr_u16 addr, cmr_u8 *data, cmr_u16 length)
{
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
	cmr_u16 i = 0;

    hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0313, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0a69, (addr >> 8) & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0a6a, addr & 0xff);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0313, 0x20);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0313, 0x12);

	for (i = 0; i < length; i++){
		data[i] = hw_sensor_read_reg(sns_drv_cxt->hw_handle, 0x0a6c);
        SENSOR_LOGD("addr = 0x%x, data[%d] = 0x%x\n", addr + i * 8, i, data[i]);
    }
}


static void gc08a3_otp_init(cmr_handle handle)
{
    
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    
    //otp init
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0315, 0x80);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x031c, 0x60);

	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0324, 0x44);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0316, 0x09);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0a67, 0x80);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0313, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0a53, 0x0e);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0a65, 0x17);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0a68, 0xa1);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0a47, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0a58, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0ace, 0x0c);
}


static void gc08a3_otp_deinit(cmr_handle handle)
{
    
    SENSOR_IC_CHECK_HANDLE_VOID(handle);
    struct sensor_ic_drv_cxt * sns_drv_cxt = (struct sensor_ic_drv_cxt *)handle;
    
    //otp deinit
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0316, 0x00);
	hw_sensor_write_reg(sns_drv_cxt->hw_handle, 0x0a67, 0x00);
}


static uint32_t gc08a3_read_otp_info(cmr_handle handle)
{
    uint32_t rtn = SENSOR_SUCCESS;
    // init
    gc08a3_otp_init(handle);
    usleep(1*1000);
    //read AWB_flag
    gc08a3_otp_read_groups(handle , gc08a3_OTP_AWB_FLAG_OFFSET , &AWB_flag , 1);
    //read AWB_buffer
      if ((AWB_flag & 0x03) == 0x01) {
        OTP_LOGD("group1_awb, size %d, block flag 0x01", gc08a3_OTP_AWB_SIZE);
        gc08a3_otp_read_groups(handle , gc08a3_OTP_AWB_OFFSET , AWB_buffer , gc08a3_OTP_AWB_SIZE);
    } else if ((AWB_flag & 0x0c) == 0x04) {
        OTP_LOGD("group2_awb, size %d, block flag 0x03", gc08a3_OTP_AWB_SIZE);
        gc08a3_otp_read_groups(handle , gc08a3_OTP_AWB_OFFSET + gc08a3_GROUP_SIZE * 8 , AWB_buffer , gc08a3_OTP_AWB_SIZE);
    } else if ((AWB_flag & 0x0f) == 0x00) {
        OTP_LOGD("awb is empty");
    }else {
        rtn = OTP_CAMERA_FAIL;
        OTP_LOGE("invalid block awb flag 0x%x", AWB_flag);
    }
    /*read AF_flag
    gc08a3_otp_read_groups(handle , gc08a3_OTP_AF_FLAG_OFFSET , &AF_flag , 1);
    //read AF_buffer
    if ((AF_flag & 0x03) == 0x01) {
        OTP_LOGD("group1_af, size %d, block flag 0x01", gc08a3_OTP_AF_SIZE);
        gc08a3_otp_read_groups(handle , gc08a3_OTP_AF_OFFSET , AF_buffer , gc08a3_OTP_AF_SIZE);
    } else if ((AF_flag & 0x0c)== 0x04) {
        OTP_LOGD("group2_af, size %d, block flag 0x03", gc08a3_OTP_AF_SIZE);
        gc08a3_otp_read_groups(handle , gc08a3_OTP_AF_OFFSET , AF_buffer + gc08a3_GROUP_SIZE * 8 , gc08a3_OTP_AF_SIZE);
    } else if ((AF_flag & 0x0f)== 0x00) {
        OTP_LOGD("af is empty");
    } else {
        rtn = OTP_CAMERA_FAIL;
        OTP_LOGE("invalid block af flag 0x%x", AF_flag);
    }*/
    //deinit 
    gc08a3_otp_deinit(handle);
    return rtn ;
}


static cmr_int sensor_readotp_awb_af_info(cmr_handle handle, void *param_ptr)
{   
    cmr_int rtn = SENSOR_SUCCESS;
    uint16_t RG_GAIN, BG_GAIN,GG_GAIN, RG_GOLDEN_GAIN, BG_GOLDEN_GAIN,GG_GOLDEN_GAIN, AF_INFI, AF_MAC;
    uint16_t r_gain = 1024, g_gain = 1024, b_gain = 1024;
    uint16_t r_gain_current = 0, g_gain_current = 0, b_gain_current = 0, base_gain = 0;
    uint32_t *otp_ptr = (uint32_t *)param_ptr;

    AF_INFI = 0;
    AF_MAC = 0;
    SENSOR_LOGI("AF_INFI=0x%x AF_MAC=0x%x\n", AF_INFI, AF_MAC);


    RG_GAIN = (((cmr_u16)AWB_buffer[2] << 8) & 0xff00) | AWB_buffer[1];
    GG_GAIN = (((cmr_u16)AWB_buffer[4] << 8) & 0xff00) | AWB_buffer[3];
    BG_GAIN = (((cmr_u16)AWB_buffer[6] << 8) & 0xff00) | AWB_buffer[5];
    SENSOR_LOGI("RG_GAIN=0x%x BG_GAIN=0x%x BG_GAIN=0x%x\n", RG_GAIN, BG_GAIN, GG_GAIN);

    RG_GOLDEN_GAIN = (((cmr_u16)AWB_buffer[8] << 8) & 0xff00) | AWB_buffer[7];
    GG_GOLDEN_GAIN = (((cmr_u16)AWB_buffer[10] << 8) & 0xff00) | AWB_buffer[9];			
    BG_GOLDEN_GAIN = (((cmr_u16)AWB_buffer[12] << 8) & 0xff00) | AWB_buffer[11];			
    SENSOR_LOGI("RG_GOLDEN_GAIN=0x%x BG_GOLDEN_GAIN=0x%x GG_GOLDEN_GAIN=0x%x\n", RG_GAIN, BG_GAIN, GG_GAIN);

    if(RG_GAIN&&BG_GAIN&&RG_GOLDEN_GAIN&&BG_GOLDEN_GAIN){
        r_gain_current = 1024 * RG_GOLDEN_GAIN / RG_GAIN;
        b_gain_current = 1024 * BG_GOLDEN_GAIN / BG_GAIN;
        g_gain_current = 1024;

        base_gain = (r_gain_current < b_gain_current) ? r_gain_current : b_gain_current;
        base_gain = (base_gain < g_gain_current) ? base_gain : g_gain_current;

        r_gain = 0x400 * r_gain_current / base_gain;
        g_gain = 0x400 * g_gain_current / base_gain;
        b_gain = 0x400 * b_gain_current / base_gain;
        SENSOR_LOGI("r_gain = 0x%x, g_gain = 0x%x, b_gain = 0x%x\n", r_gain, g_gain, b_gain);
        *otp_ptr++ = RG_GAIN;
        *otp_ptr++ = BG_GAIN;
        *otp_ptr++ = r_gain;
        *otp_ptr++ = b_gain;
        *otp_ptr++ = g_gain;
        *otp_ptr++ = AF_INFI;
        *otp_ptr++ = AF_MAC;
    }
    return rtn ;
}

#endif