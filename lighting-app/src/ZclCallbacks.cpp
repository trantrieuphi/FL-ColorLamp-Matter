/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
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

#include "AppTask.h"
#include "ColorFormat.h"
#include "DeviceDimmer.h"

#include "UartService.h"

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/ConcreteAttributePath.h>
#include <lib/support/logging/CHIPLogging.h>

LOG_MODULE_DECLARE(app, CONFIG_CHIP_APP_LOG_LEVEL);

using namespace chip;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OnOff;

void MatterPostAttributeChangeCallback(const chip::app::ConcreteAttributePath & attributePath, uint8_t type, uint16_t size,
                                       uint8_t * value)
{
    static HsvColor_t hsv;
    static XyColor_t xy;
    ClusterId clusterId     = attributePath.mClusterId;
    AttributeId attributeId = attributePath.mAttributeId;

    if (clusterId == OnOff::Id && attributeId == OnOff::Attributes::OnOff::Id)
    {
        ChipLogDetail(Zcl, "Cluster OnOff: attribute OnOff set to %u", *value);
        GetAppTask().SetInitiateAction(*value ? DeviceDimmer::ON_ACTION : DeviceDimmer::OFF_ACTION,
                                       static_cast<int32_t>(AppEvent::kEventType_Lighting), value);
    }
    else if (clusterId == LevelControl::Id && attributeId == LevelControl::Attributes::CurrentLevel::Id)
    {
        if (GetAppTask().GetDimmerDevice().IsTurnedOn())
        {
            ChipLogDetail(Zcl, "Cluster LevelControl: attribute CurrentLevel set to %u", *value);
            GetAppTask().SetInitiateAction(DeviceDimmer::LEVEL_ACTION, static_cast<int32_t>(AppEvent::kEventType_Lighting), value);
        }
        else
        {
            ChipLogDetail(Zcl, "LED is off. Try to use move-to-level-with-on-off instead of move-to-level");
        }
    }
    else if (clusterId == ColorControl::Id)
    {
        /* Ignore several attributes that are currently not processed */
        if ((attributeId == ColorControl::Attributes::RemainingTime::Id) ||
            (attributeId == ColorControl::Attributes::EnhancedColorMode::Id) ||
            (attributeId == ColorControl::Attributes::ColorMode::Id))
        {
            return;
        }

        /* XY color space */
        if (attributeId == ColorControl::Attributes::CurrentX::Id || attributeId == ColorControl::Attributes::CurrentY::Id)
        {
            // print_uart("XY color space\r\n");
            if (attributeId == ColorControl::Attributes::CurrentX::Id)
            {
                xy.x = *reinterpret_cast<uint16_t *>(value);
                // print_uart("update x\r\n");
            }
            else if (attributeId == ColorControl::Attributes::CurrentY::Id)
            {
                xy.y = *reinterpret_cast<uint16_t *>(value);
                // uart_send((uint8_t *) &xy.y, 2);
            }

            ChipLogDetail(Zcl, "New XY color: %u|%u", xy.x, xy.y);
            GetAppTask().SetInitiateAction(DeviceDimmer::COLOR_ACTION_XY, static_cast<int32_t>(AppEvent::kEventType_Lighting),
                                           (uint8_t *) &xy);
        }
        /* HSV color space */
        else if (attributeId == ColorControl::Attributes::CurrentHue::Id ||
                 attributeId == ColorControl::Attributes::CurrentSaturation::Id ||
                 attributeId == ColorControl::Attributes::EnhancedCurrentHue::Id)
        {
            // print_uart("HSV color space\r\n");
            if (attributeId == ColorControl::Attributes::EnhancedCurrentHue::Id)
            {
                hsv.h = (uint8_t) (((*reinterpret_cast<uint16_t *>(value)) & 0xFF00) >> 8);
                hsv.s = (uint8_t) ((*reinterpret_cast<uint16_t *>(value)) & 0xFF);
             
            }
            else if (attributeId == ColorControl::Attributes::CurrentHue::Id)
            {
                hsv.h = value[0];
                // print_uart("update hue\r\n");
            }
            else if (attributeId == ColorControl::Attributes::CurrentSaturation::Id)
            {
                hsv.s = value[0];
                // print_uart("update saturation\r\n");
            }
            ChipLogDetail(Zcl, "New HSV color: hue = %u| saturation = %u", hsv.h, hsv.s);
            GetAppTask().SetInitiateAction(DeviceDimmer::COLOR_ACTION_HSV, static_cast<int32_t>(AppEvent::kEventType_Lighting),
                                           (uint8_t *) &hsv);

        }
        /* Temperature Mireds color space */
        else if (attributeId == ColorControl::Attributes::ColorTemperatureMireds::Id)
        {
            // print_uart("Temperature Mireds color space\r\n");
            ChipLogDetail(Zcl, "New Temperature Mireds color = %u", *(uint16_t *) value);
            GetAppTask().SetInitiateAction(DeviceDimmer::COLOR_ACTION_CT, static_cast<int32_t>(AppEvent::kEventType_Lighting), value);
        }
        else
        {
            ChipLogDetail(Zcl, "Ignore ColorControl attribute (%u) that is not currently processed!", attributeId);
        }
    }
}

/** @brief OnOff Cluster Init
 *
 * This function is called when a specific cluster is initialized. It gives the
 * application an opportunity to take care of cluster initialization procedures.
 * It is called exactly once for each endpoint where cluster is present.
 *
 * @param endpoint   Ver.: always
 *
 */
void emberAfOnOffClusterInitCallback(EndpointId endpoint)
{
    // EmberAfStatus status;
    // bool storedValue;

    // // Read storedValue on/off value
    // status = Attributes::OnOff::Get(1, &storedValue);
    // if (status == EMBER_ZCL_STATUS_SUCCESS)
    // {
    //     // Set actual state to stored before reboot
    //     GetAppTask().GetDimmerDevice().Set(storedValue);
    // }
    // GetAppTask().SetInitiateAction(DeviceDimmer::ON_ACTION, static_cast<int32_t>(AppEvent::kEventType_Lighting), (uint8_t *) &storedValue);
}
