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

#include "DeviceDimmer.h"

#include "UartService.h"
#include "libuser.h"
#include <app-common/zap-generated/attributes/Accessors.h>

#include <cstdio>
// #include <lib/support/CHIPMemString.h>
// #include <platform/CHIPDeviceLayer.h>
// #include <app/server/Server.h>
#include "AppTask.h"


using namespace ::chip::Platform;
using namespace ::chip::DeviceLayer;
using namespace ::chip::app::Clusters;

extern ProductInfo_t product_info;
// uint8_t kExampleEndpointId = 1;

LOG_MODULE_DECLARE(app, CONFIG_CHIP_APP_LOG_LEVEL);

CHIP_ERROR DeviceDimmer::Init(const char * szDeviceName, const char * szLocation, uint8_t level, uint8_t min, uint8_t max)
{
    //check product id
    if(product_info.product_id[3] == 's')
    {
        mState = kState_Off;
        mType = kTypeLight_OnOff;

    }
    else if(product_info.product_id[3] == 'd')
    {
        mType = kTypeLight_LevelControl;
        mState = kState_Off;
        mLevel = level;
        mMinLevel = min;
        mMaxLevel = max;
        // print_uart("Init dimmer");
    }
    else if(product_info.product_id[3] == 'r')
    {
        mType = kTypeLight_ColorControl;
        mState = kState_Off;
        mLevel = level;
        mMinLevel = min;
        mMaxLevel = max;
        mHsvColor.h = 0;
        mHsvColor.s = 0;
        mHsvColor.v = 0;
        // print_uart("Init RGB");
    }
    else
    {
        mType = kTypeLight_ColorControl;
        mState = kState_Off;
        mLevel = level;
        mMinLevel = min;
        mMaxLevel = max;
        mHsvColor.h = 0;
        mHsvColor.s = 0;
        mHsvColor.v = 0;
        // print_uart("default init");
    }


    CopyString(mName, sizeof(mName), szDeviceName);
    CopyString(mLocation, sizeof(mLocation), szLocation);

    ClearAction();
    Set(false);

    return CHIP_NO_ERROR;

}

void DeviceDimmer::Set(bool aOn)
{
    if (aOn)
    {
        mState = kState_On;
        ChipLogProgress(Zcl, "Turn on LED");
    }
    else
    {
        mState = kState_Off;
        ChipLogProgress(Zcl, "Turn off LED");
    }
}

void DeviceDimmer::SetLevel(uint8_t aLevel)
{
    mLevel = aLevel;
    if(mType == kTypeLight_ColorControl)
    {
        mHsvColor.v = aLevel;
    }

}

void DeviceDimmer::SetHSVColor(uint8_t hue, uint8_t saturation)
{
    mHsvColor.h = hue;
    mHsvColor.s = saturation;
    ChipLogProgress(Zcl, "Set LED color to %u, %u", hue, saturation);
}

void DeviceDimmer::SetCCTColor(uint16_t colorTemperature)
{
    mCCTColor.colorTemperature = colorTemperature;
    ChipLogProgress(Zcl, "Set LED CCT color to %u", colorTemperature);
}

void DeviceDimmer::ClearAction(void)
{
    mLevel = 0;
}
bool DeviceDimmer::InitiateAction(Action_t aAction, int32_t aActor, uint8_t * value)
{
    bool action_initiated = false;
    State_t new_state;

    // Initiate On/Off Action only when the previous one is complete.
    if (mState == kState_Off && aAction == ON_ACTION)
    {
        action_initiated = true;
        new_state        = kState_On;
    }
    else if (mState == kState_On && aAction == OFF_ACTION)
    {
        action_initiated = true;
        new_state        = kState_Off;
    }
    else if ((aAction == LEVEL_ACTION || aAction == COLOR_ACTION_HSV)&& value[0] != mLevel)
    {
        action_initiated = true;
        if (value[0] == 0)
        {
            new_state = kState_Off;
        }
        else
        {
            new_state = kState_On;
        }
    }

    if (action_initiated)
    {
        if (aAction == ON_ACTION || aAction == OFF_ACTION)
        {
            Set(new_state == kState_On);
            int DPID_SWITCH_1 = DPID_SWITCH_LED_1;
            mcu_dp_bool_update(DPID_SWITCH_1, IsTurnedOn());
        }
        else if (aAction == LEVEL_ACTION )
        {
            if(mType == kTypeLight_LevelControl)
            {
                SetLevel(value[0]);
                int DPID_BRIGHT_VALUE = DPID_BRIGHT_VALUE_1;
                unsigned long level = this->GetLevel()*1000/254;
                mcu_dp_value_update(DPID_BRIGHT_VALUE, level);
            }
            else if(mType == kTypeLight_ColorControl)
            {
                SetLevel(value[0]);
                int DPID_COLOR_HSV_1 = DPID_BRIGHT_VALUE_1;
                unsigned long level = (this->GetHSVColor().v*MAX_BRIGHTNESS_COLOR)/254;
                mcu_dp_value_update(DPID_COLOR_HSV_1, level);
            }
            
        }
        else if (aAction == COLOR_ACTION_HSV && mType == kTypeLight_ColorControl)
        {
            SetHSVColor(value[0], value[1]);
            uint16_t hue = (this->GetHSVColor().h*MAX_BRIGHTNESS_COLOR)/254;
            uint16_t saturation = (this->GetHSVColor().s*MAX_BRIGHTNESS_COLOR)/254;
            uint16_t lightness = (this->GetHSVColor().v*MAX_BRIGHTNESS_COLOR)/254;
            int DPI_COLOR_HSV_1 = DPID_CCT_OR_HSL_1;
            mcu_dp_hsl_value_update(DPI_COLOR_HSV_1, hue, saturation, lightness);
        }

    }
    return action_initiated;

}
bool DeviceDimmer::UpdateFromHardware(Action_t aAction, int32_t aActor, uint8_t * value)
{
    bool action_initiated = false;
    switch (aAction)
    {
    case ON_ACTION:
        if(mState == kState_On)
        {
            return false;
        }
        Set(true);
        action_initiated = true;
        break;
    case OFF_ACTION:
        if(mState == kState_Off)
        {
            return false;
        }
        Set(false);
        action_initiated = true;
        break;
    case LEVEL_ACTION:
        if(mType == kTypeLight_LevelControl)
        {
            if(value[0] == this->GetLevel())
            {
                return false;
            }
            this->SetLevel(value[0]);
            if(value[0] == 0 && mState == kState_On)
            {
                this->Set(false);
            }
            else if(value[0] != 0 && mState == kState_Off)
            {
                this->Set(true);
            }
            action_initiated = true;
        }
        else if(mType == kTypeLight_ColorControl)
        {
            uint8_t level = value[0];
            if(level == this->GetHSVColor().v)
            {
                return false;
            }
            this->SetLevel(level);
            if(level == 0 && mState == kState_On)
            {
                this->Set(false);
            }
            else if(level != 0 && mState == kState_Off)
            {
                this->Set(true);
            }
            action_initiated = true;
        }
        break;

    case COLOR_ACTION_HSV:
        if(mType == kTypeLight_ColorControl)
        {
            uint8_t hue = value[0];
            uint8_t saturation = value[1];
            uint8_t level = value[2];
            // if(hue == this->GetHSVColor().h && saturation == this->GetHSVColor().s && level == this->GetHSVColor().v)
            // {
            //     return false;
            // }
            if(level == this->GetHSVColor().v)
            {
                return false;
            }
            this->SetLevel(level);
            if(level == 0 && mState == kState_On)
            {
                this->Set(false);
            }
            else if(level != 0 && mState == kState_Off)
            {
                this->Set(true);
            }
            action_initiated = true;
            // this->SetHSVColor(hue, saturation);
        }
        break;
    default:
        break;
    }

    return action_initiated;
}
void DeviceDimmer::StopAction(void)
{
    ClearAction();
    Set(false);
}

void DeviceDimmer::UpdateAction(void)
{
    // switch (mAction)
    // {
    // case ON_ACTION:
    //     Set(true);
    //     break;

    // case OFF_ACTION:
    //     Set(false);
    //     break;

    // case LEVEL_ACTION:
    //     SetLevel(*mValue);
    //     break;

    // default:
    //     break;
    // }

    // ClearAction();
}

// void DeviceDimmer::SetCallbacks(DimmerCallback_fn aActionInitiated_CB, DimmerCallback_fn aActionCompleted_CB,
//                                 DimmerTimerCallback_fn aActionBlinkStateUpdate_CB)
// {
//     mActionInitiatedClb = aActionInitiated_CB;
//     mActionCompletedClb = aActionCompleted_CB;
//     mActionBlinkStateUpdateClb = aActionBlinkStateUpdate_CB;
// }

// void DeviceDimmer::ActionInitiated(Action_t aAction, int32_t aActor)
// {
//     if (mActionInitiatedClb != nullptr)
//     {
//         mActionInitiatedClb(aAction, aActor);
//     }
// }

// void DeviceDimmer::ActionCompleted(Action_t aAction, int32_t aActor)
// {
//     if (mActionCompletedClb != nullptr)
//     {
//         mActionCompletedClb(aAction, aActor);
//     }
// }

// void DeviceDimmer::ActionBlinkStateUpdate(k_timer * dummy)
// {
//     if (mActionBlinkStateUpdateClb != nullptr)
//     {
//         mActionBlinkStateUpdateClb(dummy);
//     }
// }

// void DeviceDimmer::InitiateBlinkAction(uint32_t onTimeMS, uint32_t offTimeMS)
// {
//     mBlinkOnTimeMS = onTimeMS;
//     mBlinkOffTimeMS = offTimeMS;
//     mBlinkState = true;
//     mBlinkTimer = k_timer_create(&mBlinkTimer, (k_timer_expiry_t) ActionBlinkStateUpdate, nullptr);
//     k_timer_start(&mBlinkTimer, K_MSEC(mBlinkOnTimeMS), K_MSEC(mBlinkOnTimeMS));
// }



