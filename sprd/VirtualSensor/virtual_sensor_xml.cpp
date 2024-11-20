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

#include "virtual_sensor.h"

#define LOG_TAG "VirtualSensorParser"

using namespace std;
using namespace pugi;
char SensorNamer[16][128]= {0};
string CameraModuleConfig = "CameraModuleConfig";
string SlotId = "SlotId";

/*sensor sensor_static_info*/
string StaticInfo = "StaticInfo",
       Aperture = "Aperture",
       FocalLength = "FocalLength",
       MiniFocalLength = "MiniFocalDistance",
       MaxFps = "MaxFps",
       MaxGain = "MaxGain",
       OIS = "OisSupported",
       PDAFtype = "PdafType",
       LongExp = "LongExpSupported",
       ZslLongExp = "ZslLongExpSupported",
       EmbeddedLine = "EmbeddedLineEnable",
       ExpValidFrm = "ExpValidFrame",
       ClampLevel = "ClampLevel",
       GainValidFrm = "GainValidFrame",
       FovAngle = "FovAngle",
       FovWidth = "FovWidth",
       FovHeight = "FovHeight",
       FocalLengths = "FocalLengths",
       MonoSensor = "MonoSensor",
       MinFocusDistance = "MinFocusDistance",
       StartOffsetTime = "StartOffsetTime";

/*sensor sensor_trim_tag*/
string TrimTable = "TrimTable",
       TrimX = "TrimX",
       TrimY = "TrimY",
       TrimWidth = "TrimWidth",
       TrimHeight = "TrimHeight",
       LineTime = "LineTime",
       BpsPerLane = "BpsPerLane",
       FrameLength = "FrameLength";

/*sensor module_cfg_info*/
string ModuleInfo = "ModuleInfo",
       MajorI2C = "MajorI2C",
       MinorI2C = "MinorI2C",
       I2cBurst = "I2cBurstMode" ,
       RegisterBits = "RegisterBits",
       AnalogVoltage = "AnalogVoltage",
       IOVoltage = "IOVoltage",
       DigitalVoltage = "DigitalVoltage",
       RawPattern = "RawPattern",
       PreviewSkipNum = "PreviewSkilNum",
       CaptureSkipNum = "CaptureSkipNum",
       PreviewDeciNum = "PreviewDeciNum",
       FlashSkipNum = "FlashSkipNum",
       MipiCapSkipNum = "MipiCapSkipNum",
       VideoDeciNum = "VideoDeciNum",
       ThresholdEb = "ThresholdEb",
       ThresholdMode = "ThresholdMode",
       ThresholdStart = "ThresholdStart",
       ThresholdEnd = "ThresholdEnd",
       I2cDevHandler = "I2cDevHandler",
       SensorInterfaceType = "InterfaceType",
       BusWidth = "BusWidth",
       PixelWidth = "PixelWidth",
       IsLoose = "IsLoose",
       LaneSwitchEb = "LaneSwitchEb",
       LaneSeq = "LaneSeq",
       IsCPHY = "IsCPHY",
       SettingSkipNum = "SettingSkipNum",
       HorizontalViewAngle = "HorizontalViewAngle",
       VerticalViewAngle = "VerticalViewAngle";

/*sensor mipi_raw_info*/
string MipiRawInfo = "MipiRawInfo",
       HWSignal = "HWSignal",
       EnvMode = "EnvMode",
       IMGEffect = "IMGEffect",
       WBMode = "WBMode",
       StepCount = "StepCount",
       ResetPulseLevel = "ResetPulseLevel",
       ResetPulseWidth ="ResetPulseWidth",
       PowerDownLevel = "PowerDownLevel",
       IMAGEFormat = "IMAGEFormat",
       MaxWith = "MaxWidth",
       Name = "SensorName",
       MaxHeight = "MaxHeight";

string QuadBayerSensor = "QuadBayerSensor",
       RemosaicType = "RemosaicType",
       LimitedWidth = "LimitedWidth",
       LimitedHeight = "LimitedHeight";

vector<string> SplitString(string input) {
    vector<string> dstVector;
    istringstream iss(input);
    string token;
    while (std::getline(iss, token, ' '))
        dstVector.push_back(token);
    return dstVector;
}

unsigned int *Trans2IntArray (xml_node Node, string str, int array) {
    xml_object_range<xml_node_iterator> child = Node.children();
    unsigned int evDst[array];
    for (xml_node_iterator iterChild = child.begin(); iterChild != child.end(); iterChild++) {
        if(!strcmp(str.c_str(), iterChild->name()) && iterChild->text().as_string()) {
            vector<string> EvArrary = SplitString(iterChild->text().as_string());
            for (int i = 0; i < EvArrary.size(); i++)
                evDst[i] = atof(EvArrary[i].c_str());
                return &evDst[0];
        }
    }
    return nullptr;
}

unsigned int Trans2Int(xml_node Node, string str) {
    xml_object_range<xml_node_iterator> child = Node.children();
    for (xml_node_iterator iterChild = child.begin(); iterChild != child.end(); iterChild++) {
        if(!strcmp(str.c_str(), iterChild->name()) && iterChild->text().as_string()) {
            SENSOR_LOGD("Now with %s, value %s", iterChild->name(), iterChild->text().as_string());
            return atoi(iterChild->text().as_string());
        } else {
            continue;
        }
    }
    return 0;
}

float Trans2Float (xml_node Node, string str) {
    xml_object_range<xml_node_iterator> child = Node.children();
    for (xml_node_iterator iterChild = child.begin(); iterChild != child.end(); iterChild++) {
        if(!strcmp(str.c_str(), iterChild->name()) && iterChild->text().as_string()) {
            SENSOR_LOGD("Now with %s, value %s", iterChild->name(), iterChild->text().as_string());
            return atof(iterChild->text().as_string());
        } else {
            continue;
        }
    }
    return 0.0;
}

const char* Trans2Char (xml_node Node, string str, int array) {
    xml_object_range<xml_node_iterator> child = Node.children();
    for (xml_node_iterator iterChild = child.begin(); iterChild != child.end(); iterChild++) {
        if(!strcmp(str.c_str(), iterChild->name()) && iterChild->text().as_string()) {
            SENSOR_LOGD("Now with %s, value %s", iterChild->name(), iterChild->text().as_string());
            return iterChild->text().as_string();
        }
    }
    return nullptr;
}

void FullFillModuleCfgInfo(SensorParameterInfo *ParamPtr, xml_node path) {
    if(!ParamPtr) {
        SENSOR_LOGD("Not Valid PTR, return");
        return;
    }
    ParamPtr->ModuleCfgInfo.major_i2c_addr = Trans2Int(path, MajorI2C);
    ParamPtr->ModuleCfgInfo.minor_i2c_addr = Trans2Int(path, MinorI2C);
    ParamPtr->ModuleCfgInfo.i2c_burst_mode = Trans2Int(path, I2cBurst);
    ParamPtr->ModuleCfgInfo.reg_addr_value_bits = Trans2Int(path, RegisterBits);
    ParamPtr->ModuleCfgInfo.avdd_val = SENSOR_AVDD_VAL_E(Trans2Int(path, AnalogVoltage));
    ParamPtr->ModuleCfgInfo.dvdd_val = SENSOR_AVDD_VAL_E(Trans2Int(path, DigitalVoltage));
    ParamPtr->ModuleCfgInfo.iovdd_val = SENSOR_AVDD_VAL_E(Trans2Int(path, IOVoltage));
    ParamPtr->ModuleCfgInfo.image_pattern = Trans2Int(path, RawPattern);
    ParamPtr->ModuleCfgInfo.preview_skip_num = Trans2Int(path, PreviewSkipNum);
    ParamPtr->ModuleCfgInfo.capture_skip_num = Trans2Int(path, CaptureSkipNum);
    ParamPtr->ModuleCfgInfo.flash_capture_skip_num = Trans2Int(path, FlashSkipNum);
    ParamPtr->ModuleCfgInfo.mipi_cap_skip_num = Trans2Int(path, MipiCapSkipNum);
    ParamPtr->ModuleCfgInfo.preview_deci_num = Trans2Int(path, PreviewDeciNum);
    ParamPtr->ModuleCfgInfo.video_preview_deci_num = Trans2Int(path, VideoDeciNum);
    ParamPtr->ModuleCfgInfo.threshold_eb = Trans2Int(path, ThresholdEb);
    ParamPtr->ModuleCfgInfo.threshold_mode = Trans2Int(path, ThresholdMode);
    ParamPtr->ModuleCfgInfo.threshold_start = Trans2Int(path, ThresholdStart);
    ParamPtr->ModuleCfgInfo.threshold_end = Trans2Int(path, ThresholdEnd);
    ParamPtr->ModuleCfgInfo.i2c_dev_handler = Trans2Int(path, I2cDevHandler);
    ParamPtr->ModuleCfgInfo.sensor_interface.type = SENSOR_INF_TYPE_E(Trans2Int(path, SensorInterfaceType));
    ParamPtr->ModuleCfgInfo.sensor_interface.bus_width = SENSOR_INF_TYPE_E(Trans2Int(path, BusWidth));
    ParamPtr->ModuleCfgInfo.sensor_interface.pixel_width = SENSOR_INF_TYPE_E(Trans2Int(path, PixelWidth));
    ParamPtr->ModuleCfgInfo.sensor_interface.is_cphy = SENSOR_INF_TYPE_E(Trans2Int(path, IsCPHY));
    ParamPtr->ModuleCfgInfo.sensor_interface.lane_switch_eb = SENSOR_INF_TYPE_E(Trans2Int(path, LaneSwitchEb));
    ParamPtr->ModuleCfgInfo.sensor_interface.lane_seq = SENSOR_INF_TYPE_E(Trans2Int(path, LaneSeq));
    ParamPtr->ModuleCfgInfo.sensor_interface.is_loose = SENSOR_INF_TYPE_E(Trans2Int(path, IsLoose));
    ParamPtr->ModuleCfgInfo.change_setting_skip_num = Trans2Int(path, SettingSkipNum);
    ParamPtr->ModuleCfgInfo.horizontal_view_angle = Trans2Int(path, HorizontalViewAngle);
    ParamPtr->ModuleCfgInfo.vertical_view_angle = Trans2Int(path, VerticalViewAngle);
}

void FullFillStaticInfo(SensorParameterInfo *ParamPtr, xml_node OneModule, int Id) {
    if(!ParamPtr) {
        SENSOR_LOGD("Not Valid PTR, return");
        return;
    }
    ParamPtr->SensorId = Id;
    ParamPtr->StaticInfo.f_num = Trans2Float(OneModule, Aperture);
    ParamPtr->StaticInfo.focal_length = Trans2Int(OneModule, FocalLength);
    ParamPtr->StaticInfo.min_focal_distance = Trans2Int(OneModule, MiniFocalLength);
    ParamPtr->StaticInfo.max_fps = Trans2Int(OneModule, MaxFps);
    ParamPtr->StaticInfo.max_adgain = Trans2Int(OneModule, MaxGain);
    ParamPtr->StaticInfo.ois_supported = Trans2Int(OneModule, OIS);
    ParamPtr->StaticInfo.pdaf_supported = Trans2Int(OneModule, PDAFtype);
    ParamPtr->StaticInfo.zsl_long_expose_supported = Trans2Int(OneModule, LongExp);
    ParamPtr->StaticInfo.exp_valid_frame_num = Trans2Int(OneModule, ExpValidFrm);
    ParamPtr->StaticInfo.adgain_valid_frame_num = Trans2Int(OneModule, GainValidFrm);
    ParamPtr->StaticInfo.clamp_level = Trans2Int(OneModule, ClampLevel);
    ParamPtr->StaticInfo.fov_angle = Trans2Float(OneModule, FovAngle);
    ParamPtr->StaticInfo.fov_info.focal_lengths = ParamPtr->StaticInfo.focal_length / 100;
    ParamPtr->StaticInfo.fov_info.physical_size[0] = Trans2Float(OneModule, FovWidth);
    ParamPtr->StaticInfo.fov_info.physical_size[1] = Trans2Float(OneModule, FovHeight);
    ParamPtr->StaticInfo.mono_sensor = Trans2Float(OneModule, MonoSensor);
    ParamPtr->StaticInfo.mim_focus_distance = Trans2Float(OneModule, MinFocusDistance);
    ParamPtr->StaticInfo.start_offset_time = Trans2Float(OneModule, StartOffsetTime);
    SENSOR_LOGD("Static Info with [%f, %u, %u, %u]",
                ParamPtr->StaticInfo.f_num, ParamPtr->StaticInfo.focal_length,
                ParamPtr->StaticInfo.max_fps, ParamPtr->StaticInfo.max_adgain);
}


void FullFillTrimInfo(SensorParameterInfo *ParamPtr, xml_node path, int Seq) {
    if(!ParamPtr || Seq > SENSOR_MODE_MAX) {
        SENSOR_LOGD("Not Valid PTR, return");
        return;
    }
    struct sensor_trim_tag *TrimTabInfo = &ParamPtr->TrimTabInfo.trim_info[Seq];
    TrimTabInfo->trim_start_x = Trans2Int(path, TrimX);
    TrimTabInfo->trim_start_y = Trans2Int(path, TrimY);
    TrimTabInfo->trim_width = Trans2Int(path, TrimWidth);
    TrimTabInfo->trim_height = Trans2Int(path, TrimHeight);
    TrimTabInfo->line_time = Trans2Int(path, LineTime);
    TrimTabInfo->bps_per_lane = Trans2Int(path, BpsPerLane);
    TrimTabInfo->frame_line = Trans2Int(path, FrameLength);
    TrimTabInfo->scaler_trim = {
        .x = TrimTabInfo->trim_start_x,
        .y = TrimTabInfo->trim_start_y,
        .w = TrimTabInfo->trim_width,
        .h = TrimTabInfo->trim_height
    };
    SENSOR_LOGD("Passer with Trim [%d, %d, %d, %d]",
                TrimTabInfo->trim_start_x, TrimTabInfo->trim_start_y,
                TrimTabInfo->trim_width, TrimTabInfo->trim_height);
}

void FullFillMipiRawInfo(SensorParameterInfo *ParamPtr, xml_node path, int Id) {
    if(!ParamPtr) {
        SENSOR_LOGD("Not Valid PTR, return");
        return;
    }
    ParamPtr->MipiRawInfo.hw_signal_polarity = Trans2Int(path, HWSignal);
    ParamPtr->MipiRawInfo.environment_mode = Trans2Int(path, EnvMode);
    ParamPtr->MipiRawInfo.image_effect = Trans2Int(path, IMGEffect);
    ParamPtr->MipiRawInfo.wb_mode = Trans2Int(path, WBMode);
    ParamPtr->MipiRawInfo.step_count = Trans2Int(path, StepCount);
    ParamPtr->MipiRawInfo.reset_pulse_level = Trans2Int(path, ResetPulseLevel);
    ParamPtr->MipiRawInfo.reset_pulse_width = Trans2Int(path, ResetPulseWidth);
    ParamPtr->MipiRawInfo.power_down_level = Trans2Int(path, PowerDownLevel);
    ParamPtr->MipiRawInfo.image_format = SENSOR_IMAGE_FORMAT(Trans2Int(path, IMAGEFormat));
    ParamPtr->MipiRawInfo.source_width_max = Trans2Int(path, MaxWith);
    ParamPtr->MipiRawInfo.source_height_max = Trans2Int(path, MaxHeight);
    memset(SensorNamer[Id], 0, sizeof(SensorNamer[Id]));
    strcpy(SensorNamer[Id], Trans2Char(path, Name, 1));
    ParamPtr->MipiRawInfo.name = (const signed char *)&SensorNamer[Id];
    SENSOR_LOGD("%s", ParamPtr->MipiRawInfo.name);
}

void FullFillRemosaicInfo(SensorParameterInfo *ParamPtr, xml_node path) {
    if(!ParamPtr) {
        SENSOR_LOGD("Not Valid PTR, return");
        return;
    }
    memcpy((void *)&ParamPtr->RemosaicInfo.remosaic_mode_info[0],
           Trans2IntArray(path, RemosaicType, sizeof(ParamPtr->RemosaicInfo.remosaic_mode_info)),
           sizeof(ParamPtr->RemosaicInfo.remosaic_mode_info));
    ParamPtr->RemosaicInfo.limited_width = Trans2Int(path, LimitedWidth);
    ParamPtr->RemosaicInfo.limited_height = Trans2Int(path, LimitedHeight);
    return;
}

void ParseSensorStaticInfo(SensorParameterInfo *ParamPtr, const char *filePath, int Id) {
    xml_document doc;
    xml_parse_result result = doc.load_file(filePath);
    xml_node root_node = doc.child("cfg");
    if (result.status != status_ok || NULL == root_node) {
        if (result.status != status_ok)
            SENSOR_LOGD("Document not parsed successfully! with file path %s", filePath);
            else
            SENSOR_LOGD("Could not find rootnode");
        return;
      }

    xpath_node_set ModuleCfg = root_node.select_nodes(CameraModuleConfig.c_str());
    SENSOR_LOGD("Size of ModuleCfg is %ld", ModuleCfg.size());
    for (xpath_node_set::const_iterator iter = ModuleCfg.begin();
         iter != ModuleCfg.end(); ++iter) {
        int Sequence = 0;
        xpath_node OneModule = *iter;
        xml_node temp = iter->node();

        std::string SensorId = OneModule.node().child_value("SlotId");

        if (atoi(SensorId.c_str()) != Id) {
            SENSOR_LOGD("Now with slot id %s", SensorId.c_str());
            continue;
        }

        xpath_node_set StaticParam = OneModule.node().select_nodes(StaticInfo.c_str());
        if (StaticParam.size() != 1) {
            SENSOR_LOGD("Multi Static Parameter with %d, should return", StaticParam.size());
            return;
        }
        for (xpath_node_set::const_iterator it = StaticParam.begin();
             it != StaticParam.end(); ++it) {
                xml_node tempIter = it->node();
                FullFillStaticInfo(ParamPtr, tempIter, Id);
        }

        xpath_node_set MipiRawParam = OneModule.node().select_nodes(MipiRawInfo.c_str());
        if (MipiRawParam.size() != 1) {
            SENSOR_LOGD("Multi MipiRawParam Parameter with %d, should return", MipiRawParam.size());
            return;
        }
        for (xpath_node_set::const_iterator it = MipiRawParam.begin();
             it != MipiRawParam.end(); ++it) {
                xml_node tempIter = it->node();
                FullFillMipiRawInfo(ParamPtr, tempIter, Id);
        }

        xpath_node_set ModuleInfoParam = OneModule.node().select_nodes(ModuleInfo.c_str());
        if (ModuleInfoParam.size() != 1) {
            SENSOR_LOGD("Multi ModuleInfoParam Parameter with %d, should return", ModuleInfoParam.size());
            return;
        }
        for (xpath_node_set::const_iterator it = ModuleInfoParam.begin();
             it != ModuleInfoParam.end(); ++it) {
                xml_node tempIter = it->node();
                FullFillModuleCfgInfo(ParamPtr, tempIter);
        }

        xpath_node_set TrimInfo = OneModule.node().select_nodes(TrimTable.c_str());
        for (xpath_node_set::const_iterator iterTrim = TrimInfo.begin();
             iterTrim != TrimInfo.end(); iterTrim++) {
                 xml_node tempIter = iterTrim->node();
                 FullFillTrimInfo(ParamPtr, tempIter, Sequence);
            Sequence++;
        }

        xpath_node_set QuadBayerModule = OneModule.node().select_nodes(QuadBayerSensor.c_str());
        if (QuadBayerModule.size() != 1) {
            SENSOR_LOGD("Multi ModuleInfoParam Parameter with %d, should return", QuadBayerModule.size());
            return;
        }
        for (xpath_node_set::const_iterator it = QuadBayerModule.begin();
             it != QuadBayerModule.end(); ++it) {
                xml_node tempIter = it->node();
                FullFillRemosaicInfo(ParamPtr, tempIter);
        }
    }
}

