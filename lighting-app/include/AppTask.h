/*
 *
 *    Copyright (c) 2022-2023 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include "AppTaskCommon.h"
#include "DeviceDimmer.h"
#include "fl.h"

struct CheckACK
{
    uint8_t dp_id;
    bool check;
};

class AppTask : public AppTaskCommon
{
public:
    DeviceDimmer & GetDimmerDevice(void) { return mDimmerDevice; }
    void SetInitiateAction(DeviceDimmer::Action_t aAction, int32_t aActor, uint8_t * value);
    void SetNetworkState(uint8_t state);
    uint8_t GetNetworkState(void) { return networkState; }
    void ControlCMD(fl_data_t data);
    uint8_t UpdateDevice(uint8_t dpid,const uint8_t value[], unsigned short length);
    unsigned char HandleBrightValue(char channel, const unsigned char value[], unsigned short length);
    unsigned char HandleOnOffValue(char channel, const unsigned char value[], unsigned short length);
    unsigned char HandleHSL(char channel, const unsigned char value[], unsigned short length);
    void UpdateClusterState(void);
    void HandleUartCallback(void);
    void UpdateNumberDevice(void);
    void ReportNetworkState(void);
private:
    friend AppTask & GetAppTask(void);
    friend class AppTaskCommon;
    CHIP_ERROR Init(void);
    static void ActionInitiated(DeviceDimmer::Action_t aAction, int32_t aActor);
    static void ActionCompleted(DeviceDimmer::Action_t aAction, int32_t aActor);
    static void LightingActionEventHandler(AppEvent * aEvent);

    DeviceDimmer mDimmerDevice;
    
    uint8_t networkState;

    static AppTask sAppTask;
};

inline AppTask & GetAppTask(void)
{
    return AppTask::sAppTask;
}
