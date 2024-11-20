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
#define LOG_TAG "Virtual_Sensor"

#define SENSOR_STATIC_XML_PATH "/data/vendor/cameraserver/iss/config"
#define SENSOR_STATIC_XML_NAME "SensorInfo.xml"
#define SENSOR_RAW_FILE_PATH "/data/vendor/cameraserver/ICap/Sensor/Dump/RAW"
#define TIME_OUT_COUNT 50

VirtualSensor::VirtualSensor(int id) : mVirtualSensorId(id) {
    mRunningEnd = false;
    mRunningMode = RUNNING_ONE_FRAME;
    mSensorStreamOn = false;
    mFrameId = 0;
    mVirtualSensorBufferList.clear();
    ParseVirtualSensorInfo();
    SENSOR_LOGD("Create One VirtualSensor");
}

VirtualSensor::~VirtualSensor() {
    mVirtualSensorBufferList.clear();
    mRunningEnd = true;
}

VirtualSensor::VirtualSensorThread::VirtualSensorThread(VirtualSensor *Parent) {
    ThreadOwner = Parent;
}

VirtualSensor::VirtualSensorThread::~VirtualSensorThread() {
    ThreadOwner = NULL;
}

bool VirtualSensor::VirtualSensorThread::threadLoop() {
    while(!mMsgQueue.empty()) {
        msg_queue_t RunMsg = *(mMsgQueue.begin());
        {
            Mutex::Autolock l(mMsgQueueLock);
            mMsgQueue.erase(mMsgQueue.begin());
        };
        if(RunMsg.cmd == VIRTUAL_SENSOR_SEND_ONE_FRAME) {
            ThreadOwner->NotifyOneFrame(RunMsg.buffer.buffer, RunMsg.buffer.isCycle);
            return true;
        } else {
            return false;
        }
    };
    waitMsgAvailable();
    return true;
}

void VirtualSensor::VirtualSensorThread::waitMsgAvailable() {
    while(mMsgQueue.empty()) {
        Mutex::Autolock l(mMsgQueueLock);
        mQueueSignal.waitRelative(mMsgQueueLock, VIRUTAL_SENSOR_TIMEOUT * 2);
    }
}

void VirtualSensor::VirtualSensorThread::requestExit() {
    Mutex::Autolock l(mMsgQueueLock);
    msg_queue_t msg = {
        .cmd = VIRTUAL_SENSOR_RELEASE,
    };
    mMsgQueue.push_back(msg);
    mQueueSignal.signal();
    return;
}

void VirtualSensor::PrepareBufferList(List<BufferInfo> *list, int width, int height) {
    // auto mBM = lwp::BufferManager::getInstance();
    // BufferInfo buffer;
    // for(int i = 0; i < VIRTUAL_SENSOR_BUFFER_USAGE; i++) {
    //     mBM->getBuffer(IONBUFFER, width, height, buffer);
    //     list->push_back(buffer);
    // }
    return; 
}

/*===========================================================================
 * FUNCTION    : VirtualSensor start to RUN
 *
 * DESCRIPTION : Handle StreamOn function and Set First Buffer to GRAB
 *
 * PARAMETERS  : TBD, maybe oem cxt
 *
 * RETURN      : None
 *==========================================================================*/
void VirtualSensor::SendOneBuffer2ISP(void *cxt, BufferInfo buffer) {
    VirtualSensorOps *ops = (VirtualSensorOps *)cxt;
    SENSOR_LOGD("Sending One Frame 2ISP wit cxt %p", buffer.descs.addr_vir);
    ops->ops(ops->cxt, buffer.descs.addr_vir);
}

void VirtualSensor::ParseVirtualSensorInfo() {
    char FileName[256];
    sprintf(FileName, "%s/%s", SENSOR_STATIC_XML_PATH, SENSOR_STATIC_XML_NAME);
    ParseSensorStaticInfo(&mParameterInfo, FileName, mVirtualSensorId);
    return;
}

void VirtualSensor::FeedOneBuffer(BufferInfo buffer) {
    SendOneBuffer2ISP(&mExternalCxt, buffer);
    mValidBufferList.push_back(buffer);
}

void VirtualSensor::ReadOneSensorRaw(BufferInfo *validBuffer) {
    auto getFilePath = [&](char *path, int id)->char * {
        sprintf(path, "%s/%d/sensor.raw", SENSOR_RAW_FILE_PATH, id);
        return path;
    };

    auto readRawBuffer = [&](char *path, void *buffer)->void {
        int fileSize;
        FILE *fp = fopen(path, "rb+");
        if(!fp) {
            SENSOR_LOGE("Cannot open file with path %s", path);
            return;
        }
        fseek(fp, 0L, SEEK_END);
        fileSize = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        fread(buffer, fileSize, 1, fp);
        return;
    };

    char path[256];
    readRawBuffer(getFilePath(path, mCurrentFrameId), validBuffer->descs.addr_vir);
    validBuffer->descs.frame_id = mCurrentFrameId;
    mCurrentFrameId++;
    SENSOR_LOGD("mCurrentFrameId is %d", mCurrentFrameId);
}

bool VirtualSensor::WaitBufferPrepared() {
    int timeoutCnt = 0;
    while (mBusyBufferList.empty() && timeoutCnt < TIME_OUT_COUNT) {
        SENSOR_LOGD("No buffer prepared and maybe we could wait a little");
        timeoutCnt++;
        usleep(10 * 1000);
    }
    if(!mBusyBufferList.empty()) {
        return true;
    } else {
        return false;
    }
}

void VirtualSensor::NotifyOneFrame(BufferInfo buffer, bool RecycleMode) {
    FeedOneBuffer(buffer);
    if (mRunningMode == RUNNING_ONE_FRAME) {
        GenerateOneJob(buffer.descs.addr_vir);
    }
    return;
}


void VirtualSensor::ReadControlThread() {
    while(mCurrentFrameId < mMaxFrameNumber && !mRunningEnd) {
        if(!mValidBufferList.empty() && !mStopReading) {
            BufferInfo *ValidBuffer = NULL;
            {
                Mutex::Autolock l(mValidListLock);
                //mValidBufferList.pop_front();
            };
            ReadOneSensorRaw(ValidBuffer);
            {
                Mutex::Autolock l(mBusyListLock);
                mBusyBufferList.push_back(*ValidBuffer);
            };
        }
        continue;
    }
}

void VirtualSensor::GenerateOneJob(Seq FrameId) {
    VirtualSensorBuffer temp;
    auto foundBuffer = find_if(mBusyBufferList.begin(), mBusyBufferList.end(), [&](const BufferInfo p) {
            return p.descs.frame_id == FrameId;
    });
    if(foundBuffer == mBusyBufferList.end()) {
        SENSOR_LOGD("Cannot Find FrameId and ReRead SensorRaw");
        mStopReading = true;
        while(!mBusyBufferList.empty()) {
            {
                Mutex::Autolock l(mValidListLock);
                Mutex::Autolock l1(mBusyListLock);
                mValidBufferList.push_back(*(mBusyBufferList.begin()));
                //mBusyBufferList.pop_front();
            };
        }
        mFrameId = FrameId;
        mCurrentFrameId = FrameId;
        mStopReading = false;
    }

    if(mFrameId == FrameId)
        temp.isCycle = true;
    else
        temp.isCycle = false;

    if(!WaitBufferPrepared()) {
        SENSOR_LOGE("Fail to Prepare Buffer, error, Buffer Not Reading");
        return;
    } else {
        foundBuffer = mBusyBufferList.begin();
    }
    temp.buffer = (*foundBuffer);
    temp.frameNum = FrameId;
    {
        Mutex::Autolock l(mJobListLock);
        pair<map<Seq, VirtualSensorBuffer>::iterator, bool> insertPair =
            mJobList.insert(map<Seq, VirtualSensorBuffer>::value_type(FrameId, temp));
        if(insertPair.second) {
            {
                Mutex::Autolock l(mBusyListLock);
                mBusyBufferList.erase(foundBuffer);
            };
            SENSOR_LOGD("Enqueue Job Sucess and return");
        } else {
            SENSOR_LOGE("Failed to Enqueue Job, error, Same FrameId Exists");
        }
    };
    return;
}

void VirtualSensor::GenerateOneJob(void *srcBuf) {
    VirtualSensorBuffer temp = {
        .frameNum = mFrameId,
        .buffer.descs.addr_vir = srcBuf,
    };
    std::unique_lock<std::mutex> lock(mVirtualListLock, std::defer_lock);
    SENSOR_LOGD("Start push with %p with frame id %ld",
                 temp.buffer.descs.addr_vir, temp.frameNum);
    mVirtualSensorBufferVector.push_back(temp);
    mFrameId++;
    return;
}

void VirtualSensor::NotifyOneParameterDone(Seq FrameId) {
    GenerateOneJob(FrameId);
}

void VirtualSensor::RunOneFrame(Seq FrameId) {
    SENSOR_LOGD("Run with FrameId %ld, BufferList size %d", FrameId, mVirtualSensorBufferList.size());
    if (mVirtualSensorBufferList.empty() || mRunningEnd == true) {
        return;
    }
    auto iter = find_if(mVirtualSensorBufferList.begin(), mVirtualSensorBufferList.end(), [&](const VirtualSensorBuffer p) {
        return p.frameNum == FrameId;
    });
    if(iter == mVirtualSensorBufferList.end()) {
        return;
    }
    SendOneBuffer2ISP(&mExternalCxt, iter->buffer);
    SENSOR_LOGD("Running with mode %d", (int)mRunningMode);
    if (mVirtualSensorBufferList.size() == 1) {
        SENSOR_LOGD("No need to erase, return");
    } else {
        mVirtualSensorBufferList.erase(mVirtualSensorBufferList.begin());
        SENSOR_LOGD("Erase frame");
    }
}

void VirtualSensor::RunOneFrame() {
    if (mVirtualSensorBufferVector.empty() || mRunningEnd == true) {
        SENSOR_LOGD("Not valid BufferList");
        return;
    }
    std::unique_lock<std::mutex> lock(mFlushListLock, std::defer_lock);
    if (!lock.try_lock())
    return;
    else{
        auto iter = mVirtualSensorBufferVector.begin();
        SendOneBuffer2ISP(&mExternalCxt, iter->buffer);
        SENSOR_LOGD("Running with Task size %d", mVirtualSensorBufferVector.size());
        if (mVirtualSensorBufferVector.size() == 1) {
            return;
        } else {
            mVirtualSensorBufferVector.erase(mVirtualSensorBufferVector.begin());
            SENSOR_LOGD("Erase frame");
        }
        lock.unlock();
    }
}

void VirtualSensor::TriggerOneFrame() {
    RunOneFrame();
}

void VirtualSensor::TriggerFrame() {
    SENSOR_LOGD("TriggerFrame Thread Running");
    int frameinterspace = 0;
    if (mFps > SET_FPS_INFO_MAX) {
        mFps = SET_FPS_INFO_MAX;
        frameinterspace = ONE_SEC / mFps;
    } else {
        frameinterspace = ONE_SEC / mFps;
    }
    SENSOR_LOGD("frameinterspace %d ", frameinterspace);
    while(true && mRunningEnd != true) {
        RunOneFrame();
        usleep(frameinterspace);
    };
    return;
}

void VirtualSensor::StreamOn() {
    mCurrentFrameId = 0;
    mRunningEnd = false;
    if (mSensorStreamOn == true) {
        SENSOR_LOGD("Sensor has already stream on");
        return;
    }

    SENSOR_LOGD("Running with mVirtualSensorBufferList size %d", mVirtualSensorBufferVector.size());
    if(mRunningMode == RUNNING_ONE_FRAME) {
        std::thread TriggerThread(&VirtualSensor::TriggerFrame, this);
        TriggerThread.detach();
    } else {
        TriggerOneFrame();
    }
    mSensorStreamOn = true;
}

void VirtualSensor::StreamOff() {
    mSensorStreamOn = false;
    mCurrentFrameId = 0;
    mRunningEnd = true;
    std::unique_lock<std::mutex> lock(mFlushListLock, std::defer_lock);
    while (!lock.try_lock()) {
        usleep(50);
    }
    mVirtualSensorBufferVector.clear();
    return;
}

void VirtualSensor::StreamControl(void *buffer) {
    GenerateOneJob(buffer);
}

void VirtualSensor::PowerOn(VirtualSensorMode mode) {
    mRunningMode = mode;
}

void VirtualSensor::PowerOff() {
    mRunningMode = RUNNING_ONE_FRAME;
}

void VirtualSensor::GetSensorResourceInfo(void *SensorTrimInfo) {
    SENSOR_TRIM_T *Ptr = (SENSOR_TRIM_T *)SensorTrimInfo;
    if (Ptr) {
        memcpy(SensorTrimInfo, &mParameterInfo.TrimTabInfo, sizeof(mParameterInfo.TrimTabInfo));
    } else {
        SENSOR_LOGD("Not valid sensor Trim info");
    }
    return;
}

void VirtualSensor::GetSensorStaticInfo(void *staticInfo) {
    struct sensor_static_info *Ptr = (struct sensor_static_info *)staticInfo;
    if (Ptr) {
        memcpy(staticInfo, &mParameterInfo.StaticInfo, sizeof(mParameterInfo.StaticInfo));
    } else {
        SENSOR_LOGD("Not valid sensor static info");
    }
    return;
}

void VirtualSensor::GetSensorMipiRawInfo(void *MipiRawInfo) {
    SENSOR_INFO_T *Ptr = (SENSOR_INFO_T *)MipiRawInfo;
    if (Ptr) {
        memcpy(MipiRawInfo, &mParameterInfo.MipiRawInfo, sizeof(mParameterInfo.MipiRawInfo));
    } else {
        SENSOR_LOGD("Not valid sensor Mipi Raw info");
    }
    return;
}

void VirtualSensor::GetSensorModuleCfgInfo(void *ModuleCfgInfo) {
    struct module_cfg_info *Ptr = (struct module_cfg_info *)ModuleCfgInfo;
    if (Ptr) {
        memcpy(ModuleCfgInfo, &mParameterInfo.ModuleCfgInfo, sizeof(mParameterInfo.ModuleCfgInfo));
    } else {
        SENSOR_LOGD("Not valid sensor Module Cfg info");
    }
    return;
}

void VirtualSensor::GetSensorRemosaicInfo(void *QuadBayerInfo) {
    if (QuadBayerInfo) {
        memcpy(QuadBayerInfo, &mParameterInfo.RemosaicInfo, sizeof(mParameterInfo.RemosaicInfo));
    } else {
        SENSOR_LOGD("Not valid sensor Module Cfg info");
    }
    return;
}

void VirtualSensor::SetExternalCxt(void *cxt) {
    VirtualSensorOps *VirOps = (VirtualSensorOps *)cxt;
    mExternalCxt.ops = VirOps->ops;
    mExternalCxt.cxt = VirOps->cxt;
    SENSOR_LOGD("CXT is %p", mExternalCxt.cxt);
}

void VirtualSensor::SetFpsInfo(int *fps){
    mFps = *fps;
    return;
}