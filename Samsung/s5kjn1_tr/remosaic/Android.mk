#
# SPDX-FileCopyrightText: 2009-2015 Sprdtrum Communications (Shanghai) Co., Ltd
# SPDX-FileCopyrightText: 2016-2023 Unisoc (Shanghai) Technologies Co., Ltd
# SPDX-License-Identifier: LicenseRef-Unisoc-General-1.0
#
# Copyright 2016-2023 Unisoc (Shanghai) Technologies Co., Ltd.
# Licensed under the Unisoc General Software License, version 1.0 (the License);
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# https://www.unisoc.com/en_us/license/UNISOC_GENERAL_LICENSE_V1.0-EN_US
# Software distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OF ANY KIND, either express or implied.
# See the Unisoc General Software License, version 1.0 for more details.
#
ifeq ($(strip $(TARGET_BOARD_SENSOR_SS4C_S5KJN1_TRANSSION)),true)
LOCAL_PATH := $(call my-dir)
ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), arm arm64))
LIB_PATH :=
else ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), x86 x86_64))
LIB_PATH := x86_lib
endif

include $(CLEAR_VARS)
$(warning $(LIB_PATH))
LOCAL_MODULE := libdlrmsc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_STEM_64 := $(LOCAL_MODULE).so
LOCAL_SRC_FILES_64 := $(LIB_PATH)/lib64/libdlrmsc.so
LOCAL_MODULE_TAGS := optional
LOCAL_ODM_MODULE := true
LOCAL_SHARED_LIBRARIES += libc libdl libm liblog libEGL libGLESv2
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
$(warning $(LIB_PATH))
LOCAL_MODULE := libremosaiclib
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CFLAGS += -m64
LOCAL_MODULE_STEM_64 := $(LOCAL_MODULE).so
LOCAL_SRC_FILES_64 := $(LIB_PATH)/lib64/libremosaiclib.so
LOCAL_MODULE_TAGS := optional
LOCAL_ODM_MODULE := true
LOCAL_SHARED_LIBRARIES += liblog libdlrmsc
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libremosaic_wrapper
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MULTILIB := both
LOCAL_MODULE_STEM_32 := $(LOCAL_MODULE).so
LOCAL_MODULE_STEM_64 := $(LOCAL_MODULE).so
LOCAL_SRC_FILES_32  := $(LIB_PATH)/lib/libremosaic_wrapper.so
LOCAL_SRC_FILES_64 := $(LIB_PATH)/lib64/libremosaic_wrapper.so
LOCAL_MODULE_TAGS := optional
LOCAL_ODM_MODULE := true
LOCAL_SHARED_LIBRARIES += liblog libcutils libutils libbinder libion
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= libsprd_fcell_ss
LOCAL_CFLAGS := -O3
LOCAL_SRC_FILES := sprd_ssfcell_process.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_EXPORT_C_INCLUDE := $(LOCAL_PATH)/inc/sprd_fcell_ss.h
LOCAL_MODULE_TAGS:= optional
LOCAL_ODM_MODULE := true
LOCAL_SHARED_LIBRARIES += libremosaic_wrapper
include $(BUILD_SHARED_LIBRARY)
endif