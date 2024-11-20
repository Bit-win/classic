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

#ifdef __cplusplus
#include "cmr_sensor_info.h"
extern "C"
{
#endif
void *CreateOneVirtualSensor(int SensorId);
void GetVirtualSensorResource(void *SensorTrimInfo, void *p);
void GetVirtualSensorStaticInfo(void *StaticInfo, void *p);
void GetVirtualSensorMipiRawInfo(void *MipiRawInfo, void *p);
void GetVirtualSensorRemosaicInfo(void *RemosaicInfo, void *p);
void GetVirtualSensorModuleCfgInfo(void *ModuleCfgInfo, void *p);
void SetVirtualSensorExternalCxt(void *cxt, void *p);
void PowerOnVirtualSensor(void *p, VirtualSensorMode mode);
void ClearTaskList(void *p);
void PowerOffVirtualSensor(void *p);
void StreamOnOneVirtualSensor(void *p);
void StreamOffOneVirtualSensor(void *p);
void StreamControlVirtualSensor(void *buffer, void *p);
void SetFpsInfoVirtualSensor(int *fps, void *p);
void StreamControlRunOneFrame(void *p);
void DeleteOneVirtualSensor(void *p);
#ifdef __cplusplus
}
#endif