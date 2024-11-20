/*
 * SPDX-FileCopyrightText: 2022-2023 Unisoc (Shanghai) Technologies Co., Ltd
 * SPDX-License-Identifier: LicenseRef-Unisoc-General-1.0
 *
 * Copyright 2022-2023 Unisoc (Shanghai) Technologies Co., Ltd.
 * Licensed under the Unisoc General Software License, version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * https://www.unisoc.com/en_us/license/UNISOC_GENERAL_LICENSE_V1.0-EN_US
 * Software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 * See the Unisoc General Software License, version 1.0 for more details.
 */

#include "virtual_sensor_wrapper.h"
#include "virtual_sensor.h"
void *CreateOneVirtualSensor(int SensorId) {
    return new VirtualSensor(SensorId);
}

void GetVirtualSensorResource(void *SensorTrimInfo, void *p) {
    return static_cast<VirtualSensor *>(p)->GetSensorResourceInfo(SensorTrimInfo);
}

void GetVirtualSensorStaticInfo(void *StaticInfo, void *p) {
    return static_cast<VirtualSensor *>(p)->GetSensorStaticInfo(StaticInfo);
}

void GetVirtualSensorMipiRawInfo(void *MipiRawInfo, void *p) {
    return static_cast<VirtualSensor *>(p)->GetSensorMipiRawInfo(MipiRawInfo);
}

void GetVirtualSensorModuleCfgInfo(void *ModuleCfgInfo, void *p) {
    return static_cast<VirtualSensor *>(p)->GetSensorModuleCfgInfo(ModuleCfgInfo);
}

void GetVirtualSensorRemosaicInfo(void *RemosaicInfo, void *p) {
    return static_cast<VirtualSensor *>(p)->GetSensorRemosaicInfo(RemosaicInfo);
}

void SetVirtualSensorExternalCxt(void *cxt, void *p) {
    return static_cast<VirtualSensor *>(p)->SetExternalCxt(cxt);
}

void PowerOnVirtualSensor(void *p, VirtualSensorMode mode) {
    return static_cast<VirtualSensor *>(p)->PowerOn(mode);
}

void PowerOffVirtualSensor(void *p) {
    return static_cast<VirtualSensor *>(p)->PowerOff();
}

void StreamOnOneVirtualSensor(void *p) {
    return static_cast<VirtualSensor *>(p)->StreamOn();
}

void StreamOffOneVirtualSensor(void *p) {
    return static_cast<VirtualSensor *>(p)->StreamOff();
}

void StreamControlVirtualSensor(void *buffer, void *p) {
    return static_cast<VirtualSensor *>(p)->GenerateOneJob(buffer);
}

void StreamControlRunOneFrame(void *p) {
    return static_cast<VirtualSensor *>(p)->TriggerOneFrame();
}

void DeleteOneVirtualSensor(void *p) {
    delete static_cast<VirtualSensor *>(p);
}

void SetFpsInfoVirtualSensor(int *fps, void *p) {
    return static_cast<VirtualSensor *>(p)->SetFpsInfo(fps);
}