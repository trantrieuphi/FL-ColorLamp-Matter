/*
 *
 *    Copyright (c) 2023 Project CHIP Authors
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

#include <app/util/attribute-storage.h>

#include <functional>
#include <stdbool.h>
#include <system/SystemError.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

#include <cstdint>

#include <stdint.h>

enum TypeLight_t : uint8_t
{
    kTypeLight_OnOff = 0,
    kTypeLight_LevelControl,
    kTypeLight_ColorControl,
    kTypeLight_ColorTemperature,
    kTypeLight_Invalid
};

struct HSLColor_t
{
    uint16_t hue;
    uint16_t saturation;
    uint16_t level;
};
struct CCTColor_t
{
    uint16_t colorTemperature;
    uint16_t level;
};

class DeviceDimmer
{
public:
    static const int kDeviceNameSize     = 32;
    static const int kDeviceLocationSize = 32;

    enum Action_t : uint8_t
    {
        ON_ACTION = 0,
        OFF_ACTION,
        LEVEL_ACTION,
        COLOR_ACTION_XY,
        COLOR_ACTION_HSV,
        COLOR_ACTION_CT,

        INVALID_ACTION
    };

    enum State_t : uint8_t
    {
        kState_On = 0,
        kState_Off,
        kUnknownState
    };

    enum BreatheType_t : uint8_t
    {
        kBreatheType_Invalid = 0,
        kBreatheType_Rising,
        kBreatheType_Falling,
        kBreatheType_Both,
    };
    const unsigned long MAX_BRIGHTNESS_COLOR = 65535;
    using DimmerCallback_fn      = void (*)(Action_t, int32_t);
    using DimmerTimerCallback_fn = void (*)(k_timer *);

    CHIP_ERROR Init(const char * szDeviceName, const char * szLocation, uint8_t level, uint8_t min, uint8_t max);
    void Set(bool aOn);
    bool IsTurnedOn(void) const { return mState == kState_On; }
    uint8_t GetLevel(void) const { return mLevel; }
    uint8_t GetMinLevel(void) const { return mMinLevel; }
    uint8_t GetMaxLevel(void) const { return mMaxLevel; }
    HSLColor_t GetColor(void) const { return mColor; }
    CCTColor_t GetCCTColor(void) const { return CCTColor_t(); }
    TypeLight_t GetType(void) const { return mType; }
    // void SetCallbacks(DimmerCallback_fn aActionInitiated_CB, DimmerCallback_fn aActionCompleted_CB,
    //                   DimmerTimerCallback_fn aActionBlinkStateUpdate_CB);
    bool InitiateAction(Action_t aAction, int32_t aActor, uint8_t * value);
    bool UpdateFromHardware(Action_t aAction, int32_t aActor, uint8_t * value);
    void StopAction(void);
    void UpdateAction(void);

private:
    TypeLight_t mType;
    State_t mState;
    uint8_t mMinLevel;
    uint8_t mMaxLevel;
    uint8_t mLevel;
    HSLColor_t mColor;
    CCTColor_t mCCTColor;
    char mName[kDeviceNameSize];
    char mLocation[kDeviceLocationSize];

    DimmerCallback_fn mActionInitiatedClb;
    DimmerCallback_fn mActionCompletedClb;
    DimmerTimerCallback_fn mActionBlinkStateUpdateClb;

    void SetLevel(uint8_t level);
    void SetColor(uint16_t hue, uint16_t saturation);
    void SetCCTColor(uint16_t colorTemperature);
    void UpdateLight(void);
    void ClearAction(void);

};