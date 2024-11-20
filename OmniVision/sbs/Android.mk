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

ifeq ($(strip $(TARGET_BOARD_SBS_MODE_SENSOR)),true)
LOCAL_PATH := $(call my-dir)
ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), arm arm64))
LIB_PATH := lib/lib
else ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), x86 x86_64))
LIB_PATH := lib/x86_lib
endif
include $(CLEAR_VARS)
LOCAL_MODULE := libsensor_sbs
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MULTILIB := both
LOCAL_MODULE_STEM_32 := libsensor_sbs.a
LOCAL_MODULE_STEM_64 := libsensor_sbs.a
LOCAL_SRC_FILES_32 := $(LIB_PATH)/libsensor_sbs.a
LOCAL_SRC_FILES_64 := $(LIB_PATH)64/libsensor_sbs.a
LOCAL_MODULE_TAGS := optional
include $(BUILD_PREBUILT)
endif
