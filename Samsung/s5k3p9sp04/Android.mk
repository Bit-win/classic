#
# SPDX-FileCopyrightText: 2022 Unisoc (Shanghai) Technologies Co., Ltd
# SPDX-License-Identifier: LicenseRef-Unisoc-General-1.0
#
# Copyright 2022 Unisoc (Shanghai) Technologies Co., Ltd.
# Licensed under the Unisoc General Software License, version 1.0 (the License);
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# https://www.unisoc.com/en_us/license/UNISOC_GENERAL_LICENSE_V1.0-EN_US
# Software distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OF ANY KIND, either express or implied.
# See the Unisoc General Software License, version 1.0 for more details.
#

ifeq ($(strip $(TARGET_BOARD_SENSOR_SS4C)),true)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../../../../../SprdCtrl.mk

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../inc \
                    $(LOCAL_PATH)/../../../../../$(OEM_DIR)/inc \
                    $(LOCAL_PATH)/../../../../../common/inc \
                    $(LOCAL_PATH)/../../../../../$(ISPDRV_DIR)/isp_tune \
                    $(LOCAL_PATH)/../../../../../$(ISPALG_DIR)/common/inc \
                    $(LOCAL_PATH)/../../../../../$(ISPDRV_DIR)/middleware/inc \
                    $(LOCAL_PATH)/../../../../../$(ISPDRV_DIR)/driver/inc \
                    $(LOCAL_PATH)/../../../../../kiface/interface/cam_sys \
                    $(LOCAL_PATH)/../../../../otp_parser \
                    $(LOCAL_PATH)/../4in1/inc


LOCAL_SRC_FILES := sensor_s5k3p9sp04_mipi_raw.c

LOCAL_SHARED_LIBRARIES := libcutils libcamcommon libdl libutils libcamsensor liblog libxml2

ifeq (1, 1) #(strip $(shell expr $(ANDROID_MAJOR_VER) \>= 8)))
LOCAL_ODM_MODULE := true
endif

LOCAL_MODULE := libsensor_s5k3p9sp04
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

#include $(call first-makefiles-under,$(LOCAL_PATH))
endif
