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

#include "sensor_drv_u.h"
#include "pugixml.hpp"
#include <vector>
#include <memory>
#include <list>
#include <map>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <utils/Log.h>
#include <utils/Errors.h>
#include <utils/List.h>
#include <utils/Mutex.h>
#include <utils/Thread.h>
#include <cutils/properties.h>
#include <future>
#include <mutex>
#include "SimulationWrapper.h"
#include <iostream>
#include <sstream>

#define VIRTUAL_SENSOR_BUFFER_USAGE 5
#define VIRUTAL_SENSOR_TIMEOUT 50e6
#define ONE_SEC 10e5
#define SET_FPS_INFO_MAX 20

typedef unsigned long long Seq;

typedef struct {
    void *addr_vir;
    Seq frame_id;
} BufferAddr;

typedef struct {
    BufferAddr descs;
} BufferInfo;

typedef enum {
    VIRTUAL_SENSOR_SEND_ONE_FRAME = 0,
    VIRTUAL_SENSOR_RELEASE,
} VirtualSensorCmd;

typedef struct {
    struct sensor_static_info StaticInfo;
    struct module_cfg_info ModuleCfgInfo;
    SENSOR_INFO_T MipiRawInfo;
    SENSOR_TRIM_T TrimTabInfo;
    int SensorId;
    struct sensor_remosaic_info RemosaicInfo;
} SensorParameterInfo;

typedef struct {
    Seq frameNum;
    bool isCycle;
    BufferInfo buffer;
} VirtualSensorBuffer;

typedef struct {
    VirtualSensorBuffer buffer;
    VirtualSensorCmd cmd;
} msg_queue_t;

using namespace std;
using namespace android;
using namespace pugi;

class VirtualSensor {
    private:
        List<BufferInfo> mBusyBufferList;
        List<BufferInfo> mValidBufferList;
        List<VirtualSensorBuffer> mVirtualSensorBufferList;
        vector<VirtualSensorBuffer> mVirtualSensorBufferVector;
        map<Seq, VirtualSensorBuffer> mJobList;

        std::mutex mVirtualListLock;
        std::mutex mFlushListLock;
        Mutex mJobListLock;
        Mutex mBusyListLock;
        Mutex mValidListLock;

        SensorParameterInfo mParameterInfo;
        VirtualSensorMode mRunningMode;
        Seq mMaxFrameNumber;
        Seq mCurrentFrameId;
        Seq mTriggerFrameId;
        Seq mFrameId;

        int mVirtualSensorId;
        int mFps;
        bool mRunningEnd;
        bool mStopReading;
        bool mSensorStreamOn;
        VirtualSensorOps mExternalCxt;
    private:
        void PrepareBufferList(List<BufferInfo> *list, int width, int height);
        void ReadControlThread();
        void ReadOneSensorRaw(BufferInfo *validBuffer);
        void FeedOneBuffer(BufferInfo buffer);
        void RunOneFrame(Seq FrameId);
        void RunOneFrame();
        void ParseVirtualSensorInfo();
        void SendOneBuffer2ISP(void *cxt, BufferInfo buffer);
        bool WaitBufferPrepared();
        void NotifyOneFrame(BufferInfo buffer, bool RecycleMode);
        void NotifyOneParameterDone(Seq FrameId);
        void GenerateOneJob(Seq FrameId);
        void TriggerFrame();
    public:
        VirtualSensor(int id);
        ~VirtualSensor();
        void GetSensorResourceInfo(void *SensorTrimInfo);
        void GetSensorModuleCfgInfo(void *ModuleCfgInfo);
        void GetSensorMipiRawInfo(void *MipiRawInfo);
        void GetSensorStaticInfo(void *StaticInfo);
        void GetSensorRemosaicInfo(void *QuadBayerInfo);
        void StreamOn();
        void StreamOff();
        void StreamControl(void *buffer);
        void PowerOn(VirtualSensorMode mode);
        void PowerOff();
        void SetExternalCxt(void *cxt);
        void TriggerOneFrame();
        void GenerateOneJob(void *srcBuf);
        void SetFpsInfo(int *fps);

    class VirtualSensorThread : public android::Thread {
        public:
            VirtualSensorThread(VirtualSensor *Parent);
            ~VirtualSensorThread();
            virtual bool threadLoop();
            virtual void requestExit();
        private:
            VirtualSensor *ThreadOwner;
            void waitMsgAvailable();
        public:
            List<msg_queue_t> mMsgQueue;
            Mutex mMsgQueueLock;
            Condition mQueueSignal;
    };

    VirtualSensorThread *mChildThread;
};

void ParseSensorStaticInfo(SensorParameterInfo *ParamPtr, const char *filePath, int Id);