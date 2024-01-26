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

#include "AppTask.h"
#include <app/server/Server.h>

#include "ColorFormat.h"
#include "DeviceDimmer.h"
#include "PWMDevice.h"
#include "libuser.h"
#include "string.h"
#include <app-common/zap-generated/attributes/Accessors.h>

LOG_MODULE_DECLARE(app, CONFIG_CHIP_APP_LOG_LEVEL);

namespace {
#ifdef CONFIG_WS2812_STRIP
const struct device * const ws2812_dev = DEVICE_DT_GET(DT_ALIAS(led_strip));
#else
// const struct pwm_dt_spec sPwmRgbSpecBlueLed  = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

#if USE_RGB_PWM
// const struct pwm_dt_spec sPwmRgbSpecGreenLed = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led1));
// const struct pwm_dt_spec sPwmRgbSpecRedLed   = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led2));
#endif
#endif // CONFIG_WS2812_STRIP




#if defined(CONFIG_WS2812_STRIP) || USE_RGB_PWM
uint8_t sBrightness;
PWMDevice::Action_t sColorAction = PWMDevice::INVALID_ACTION;
XyColor_t sXY;
HsvColor_t sHSV;
CtColor_t sCT;
#endif // CONFIG_WS2812_STRIP || USE_RGB_PWM
} // namespace

AppTask AppTask::sAppTask;

#ifdef CONFIG_CHIP_ENABLE_POWER_ON_FACTORY_RESET
void AppTask::PowerOnFactoryReset(void)
{
    LOG_INF("Lighting App Power On Factory Reset");
    AppEvent event;
    event.Type    = AppEvent::kEventType_Lighting;
    event.Handler = PowerOnFactoryResetEventHandler;
    GetAppTask().PostEvent(&event);
}
#endif /* CONFIG_CHIP_ENABLE_POWER_ON_FACTORY_RESET */

extern	k_msgq		queue_data_tx;
extern	k_msgq		queue_data_rx;

HsvColor_t sHSV;
HSLColor_t sHSL;

bool CheckACK = false;

extern DOWNLOAD_CMD_S download_cmd[];

void Control_cmd(fl_data_t data);

CHIP_ERROR AppTask::Init(void)
{
    CHIP_ERROR err;

    // Init lighting manager
    uint8_t minLightLevel = kDefaultMinLevel;
    Clusters::LevelControl::Attributes::MinLevel::Get(kExampleEndpointId, &minLightLevel);

    uint8_t maxLightLevel = kDefaultMaxLevel;
    Clusters::LevelControl::Attributes::MaxLevel::Get(kExampleEndpointId, &maxLightLevel);

#ifdef CONFIG_WS2812_STRIP
    err = sAppTask.mWS2812Device.Init(ws2812_dev, STRIP_NUM_PIXELS(led_strip));
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("WS2812 Device Init fail");
        return err;
    }
#else
    // err = sAppTask.mPwmRgbBlueLed.Init(&sPwmRgbSpecBlueLed, minLightLevel, maxLightLevel, maxLightLevel);
    err = sAppTask.mDimmerDevice.Init("Dimmer", "Living Room", 0, minLightLevel, maxLightLevel);
    
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("Dimmer Device Init fail");
        return err;
    }
#if USE_RGB_PWM
    err = sAppTask.mPwmRgbRedLed.Init(&sPwmRgbSpecRedLed, minLightLevel, maxLightLevel, maxLightLevel);
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("Red RGB PWM Device Init fail");
        return err;
    }

    err = sAppTask.mPwmRgbGreenLed.Init(&sPwmRgbSpecGreenLed, minLightLevel, maxLightLevel, maxLightLevel);
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("Green RGB PWM Device Init fail");
        return err;
    }
#endif

    // sAppTask.mDimmerDevice.SetCallbacks(ActionInitiated, ActionCompleted, nullptr);
    // sAppTask.mPwmRgbBlueLed.SetCallbacks(ActionInitiated, ActionCompleted, nullptr);
#endif // CONFIG_WS2812_STRIP

#if APP_USE_EXAMPLE_START_BUTTON
    // SetExampleButtonCallbacks(LightingActionEventHandler);
#endif
    InitCommonParts();

    return CHIP_NO_ERROR;
}

void AppTask::LightingActionEventHandler(AppEvent * aEvent)
{
#ifdef CONFIG_WS2812_STRIP
    if (aEvent->Type == AppEvent::kEventType_Button)
    {
        if (sAppTask.mWS2812Device.IsTurnedOn())
        {
            sAppTask.mWS2812Device.Set(SET_RGB_TURN_OFF);
        }
        else
        {
            sAppTask.mWS2812Device.Set(SET_RGB_TURN_ON);
        }

        sAppTask.UpdateClusterState();
    }
#else
    // PWMDevice::Action_t action = PWMDevice::INVALID_ACTION;
    // int32_t actor              = 0;
    DeviceDimmer::Action_t dimmerAction = DeviceDimmer::INVALID_ACTION;
    int32_t dimmerActor              = 0;

    if (aEvent->Type == AppEvent::kEventType_Lighting)
    {
        // action = static_cast<PWMDevice::Action_t>(aEvent->LightingEvent.Action);
        // actor  = aEvent->LightingEvent.Actor;
        dimmerAction = static_cast<DeviceDimmer::Action_t>(aEvent->LightingEvent.Action);
        dimmerActor  = aEvent->LightingEvent.Actor;
    }
    else if (aEvent->Type == AppEvent::kEventType_Button)
    {
#if USE_RGB_PWM
        if (sAppTask.mPwmRgbRedLed.IsTurnedOn() || sAppTask.mPwmRgbGreenLed.IsTurnedOn() || sAppTask.mPwmRgbBlueLed.IsTurnedOn())
        {
            action = PWMDevice::OFF_ACTION;
        }
        else
        {
            action = PWMDevice::ON_ACTION;
        }
#else
        // action = sAppTask.mPwmRgbBlueLed.IsTurnedOn() ? PWMDevice::OFF_ACTION : PWMDevice::ON_ACTION;
        dimmerAction = sAppTask.mDimmerDevice.IsTurnedOn() ? DeviceDimmer::OFF_ACTION : DeviceDimmer::ON_ACTION;
#endif
        dimmerActor = AppEvent::kEventType_Button;
    }

    if (dimmerAction != DeviceDimmer::INVALID_ACTION && !sAppTask.mDimmerDevice.InitiateAction(dimmerAction, dimmerActor, NULL))
//         (
// #if USE_RGB_PWM
//             !sAppTask.mPwmRgbRedLed.InitiateAction(action, actor, NULL) ||
//             !sAppTask.mPwmRgbGreenLed.InitiateAction(action, actor, NULL) ||
// #endif
//             !sAppTask.mPwmRgbBlueLed.InitiateAction(action, actor, NULL)))
    {
        LOG_INF("Action is in progress or active");
    }
#endif // CONFIG_WS2812_STRIP
}


void AppTask::ActionInitiated(DeviceDimmer::Action_t aAction, int32_t aActor)
{
    if (aAction == DeviceDimmer::ON_ACTION)
    {
        LOG_DBG("ON_ACTION initiated");
    }
    else if (aAction == DeviceDimmer::OFF_ACTION)
    {
        LOG_DBG("OFF_ACTION initiated");
    }
    else if (aAction == DeviceDimmer::LEVEL_ACTION)
    {
        LOG_DBG("LEVEL_ACTION initiated");
    }
}

void AppTask::ActionCompleted(DeviceDimmer::Action_t aAction, int32_t aActor)
{
    if (aAction == DeviceDimmer::ON_ACTION)
    {
        LOG_DBG("ON_ACTION completed");
    }
    else if (aAction == DeviceDimmer::OFF_ACTION)
    {
        LOG_DBG("OFF_ACTION completed");
    }
    else if (aAction == DeviceDimmer::LEVEL_ACTION)
    {
        LOG_DBG("LEVEL_ACTION completed");
    }

    if (aActor == AppEvent::kEventType_Button)
    {
        sAppTask.UpdateClusterState();
    }
}

void AppTask::UpdateClusterState(void)
{
    EmberAfStatus status;
    bool isTurnedOn;
    uint8_t setLevel;

#if defined(CONFIG_WS2812_STRIP) || USE_RGB_PWM
#ifdef CONFIG_WS2812_STRIP
    isTurnedOn = sAppTask.mWS2812Device.IsTurnedOn();
#else
    isTurnedOn =
        sAppTask.mPwmRgbRedLed.IsTurnedOn() || sAppTask.mPwmRgbGreenLed.IsTurnedOn() || sAppTask.mPwmRgbBlueLed.IsTurnedOn();
#endif // CONFIG_WS2812_STRIP

    if (sColorAction == PWMDevice::COLOR_ACTION_XY || sColorAction == PWMDevice::COLOR_ACTION_HSV ||
        sColorAction == PWMDevice::COLOR_ACTION_CT)
    {
        setLevel = sBrightness;
    }
    else
    {
#ifdef CONFIG_WS2812_STRIP
        setLevel = sAppTask.mWS2812Device.GetBlueLevel();
        if (setLevel > kDefaultMaxLevel)
        {
            setLevel = kDefaultMaxLevel;
        }
#else
        setLevel = sAppTask.mPwmRgbBlueLed.GetLevel();
#endif // CONFIG_WS2812_STRIP
    }
#else
    // isTurnedOn = sAppTask.mPwmRgbBlueLed.IsTurnedOn();
    // setLevel   = sAppTask.mPwmRgbBlueLed.GetLevel();
    isTurnedOn = sAppTask.mDimmerDevice.IsTurnedOn();
    setLevel   = sAppTask.mDimmerDevice.GetLevel();
#endif // CONFIG_WS2812_STRIP || USE_RGB_PWM

    // write the new on/off value
    status = Clusters::OnOff::Attributes::OnOff::Set(kExampleEndpointId, isTurnedOn);
    if (status != EMBER_ZCL_STATUS_SUCCESS)
    {
        LOG_ERR("Update OnOff fail: %x", status);
    }

    status = Clusters::LevelControl::Attributes::CurrentLevel::Set(kExampleEndpointId, setLevel);
    if (status != EMBER_ZCL_STATUS_SUCCESS)
    {
        LOG_ERR("Update CurrentLevel fail: %x", status);
    }
}

unsigned char AppTask::HandleOnOffValue(char channel, const unsigned char value[], unsigned short length)
{
    uint8_t ret = 0;
    using namespace Clusters;
    if (*value == 0x01)
    {
        ret = sAppTask.mDimmerDevice.UpdateFromHardware(DeviceDimmer::ON_ACTION, AppEvent::kEventType_Lighting, NULL);
        if(ret == 1)
        {
            Clusters::OnOff::Attributes::OnOff::Set(kExampleEndpointId, true);
        }
    }
    else if (*value == 0x00)
    {
        ret = sAppTask.mDimmerDevice.UpdateFromHardware(DeviceDimmer::OFF_ACTION, AppEvent::kEventType_Lighting, NULL);
        if(ret == 1)
        {
            Clusters::OnOff::Attributes::OnOff::Set(kExampleEndpointId, false);
        }
    }
    else
    {
        ret = 0;
    }

    return ret;
    
}

unsigned char AppTask::HandleBrightValue(char channel, const unsigned char value[], unsigned short length)
{
    uint8_t ret = 0;
    using namespace Clusters;
    uint8_t level;
    if(sAppTask.mDimmerDevice.GetType() == kTypeLight_LevelControl){
        level = (mcu_get_dp_download_value(value,length)*255)/1000;
        level = (level >= 255)? 254: level;
        ret = sAppTask.mDimmerDevice.UpdateFromHardware(DeviceDimmer::LEVEL_ACTION, AppEvent::kEventType_Lighting, &level);

        if(ret == 1)
        {
            Clusters::LevelControl::Attributes::CurrentLevel::Set(kExampleEndpointId, level);
            // if(level == 0 && sAppTask.mDimmerDevice.IsTurnedOn()){
            //     Clusters::OnOff::Attributes::OnOff::Set(kExampleEndpointId, sAppTask.mDimmerDevice.IsTurnedOn());
            // }
            // else if(level != 0 && !sAppTask.mDimmerDevice.IsTurnedOn()){
            //     Clusters::OnOff::Attributes::OnOff::Set(kExampleEndpointId, sAppTask.mDimmerDevice.IsTurnedOn());
            // }
        }
    } else if(sAppTask.mDimmerDevice.GetType() == kTypeLight_ColorControl){
        HSLColor_t sHSLtemp;
        sHSLtemp.level = mcu_get_dp_download_value(value,length);
        if(sHSLtemp.level == sAppTask.mDimmerDevice.GetColor().level){
            return 0;
        }
        sHSL.level = sHSLtemp.level;
        sHSV.v = sHSLtemp.level*255/(sAppTask.mDimmerDevice.MAX_BRIGHTNESS_COLOR);
        sHSV.v = (sHSV.v >= 255)? 254: sHSV.v;
        ret = sAppTask.mDimmerDevice.UpdateFromHardware(DeviceDimmer::LEVEL_ACTION, AppEvent::kEventType_Lighting, (uint8_t *) &sHSL.level);
        if(ret == 1)
        {
            
            if(sHSV.v == 0 && sAppTask.mDimmerDevice.IsTurnedOn()){
                Clusters::OnOff::Attributes::OnOff::Set(kExampleEndpointId, false);
            } 
            Clusters::LevelControl::Attributes::CurrentLevel::Set(kExampleEndpointId, sHSV.v);
            // else if(sHSV.v != 0 && !sAppTask.mDimmerDevice.IsTurnedOn()){
            //     Clusters::OnOff::Attributes::OnOff::Set(kExampleEndpointId, sAppTask.mDimmerDevice.IsTurnedOn());
            // }
        }

    }
    // uart_send((uint8_t *) &level, 1);

    return ret;
    
}

unsigned char AppTask::HandleHSL(char channel, const unsigned char value[], unsigned short length)
{
    uint8_t ret = 0;
    using namespace Clusters;
    if(sAppTask.mDimmerDevice.GetType() == kTypeLight_ColorControl) {
        // sHSL.hue = mcu_get_dp_download_value_2byte(value, length);
        // sHSV.h = sHSL.hue*255/(sAppTask.mDimmerDevice.MAX_BRIGHTNESS_COLOR);
        // sHSL.saturation = mcu_get_dp_download_value_2byte(value + 2, length);
        // sHSV.s = sHSL.saturation*255/(sAppTask.mDimmerDevice.MAX_BRIGHTNESS_COLOR);
        // sHSL.level = mcu_get_dp_download_value_2byte(value + 4, length);
        // sHSV.v = sHSL.level*255/(sAppTask.mDimmerDevice.MAX_BRIGHTNESS_COLOR);
        // sAppTask.mDimmerDevice.UpdateFromHardware(DeviceDimmer::LEVEL_ACTION, AppEvent::kEventType_Lighting, (uint8_t *) &sHSL.level);
        // if(ret == 1)
        // {
            // Clusters::ColorControl::Attributes::CurrentHue::Set(kExampleEndpointId, sHSV.h);
            // Clusters::ColorControl::Attributes::CurrentSaturation::Set(kExampleEndpointId, sHSV.s);
        //     Clusters::LevelControl::Attributes::CurrentLevel::Set(kExampleEndpointId, sHSV.v);
        // }
        HSLColor_t sHSLtemp;
        sHSLtemp.level = mcu_get_dp_download_value_2byte(value + 4, length);
        if(sHSLtemp.level == sAppTask.mDimmerDevice.GetColor().level){
            return 0;
        }
        sHSL.level = sHSLtemp.level;
        sHSV.v = sHSLtemp.level*255/(sAppTask.mDimmerDevice.MAX_BRIGHTNESS_COLOR);
        sHSV.v = (sHSV.v >= 255)? 254: sHSV.v;
        ret = sAppTask.mDimmerDevice.UpdateFromHardware(DeviceDimmer::LEVEL_ACTION, AppEvent::kEventType_Lighting, (uint8_t *) &sHSL.level);
        if(ret == 1)
        {
            if(sHSV.v == 0 && sAppTask.mDimmerDevice.IsTurnedOn()){
                Clusters::OnOff::Attributes::OnOff::Set(kExampleEndpointId, false);
            } 
            Clusters::LevelControl::Attributes::CurrentLevel::Set(kExampleEndpointId, sHSV.v);
            
            // else if(sHSV.v != 0 && !sAppTask.mDimmerDevice.IsTurnedOn()){
            //     Clusters::OnOff::Attributes::OnOff::Set(kExampleEndpointId, sAppTask.mDimmerDevice.IsTurnedOn());
            // }
        }
        
    }


    
}

uint8_t AppTask::UpdateDevice(uint8_t dpid,const uint8_t value[], unsigned short length)
{
//	printf("dp_download_handle\n");
  /* only list of function, mcu need realize these fuction*/
	uint8_t ret = 0;
    uint8_t channel = 0;
    if(dpid == (DPID_SWITCH_LED_1 + channel*16)){
        ret = AppTask::HandleOnOffValue(channel,value,length);
        // print_uart("switch_led_handle\n");
        return ret;
    }
    else if(dpid ==  (DPID_BRIGHT_VALUE_1 + channel*16)){
        ret = AppTask::HandleBrightValue(channel,value,length);
        return ret;
    }
    else if(dpid == (DPID_BRIGHTNESS_MIN_1 + channel*16)){
        // ret = dp_download_brightness_min_handle(channel,value,length);
        return ret;
    }
    else if(dpid == (DPID_LED_TYPE_1 + channel*16)){
        // ret = dp_download_led_type_handle(channel,value,length);
        return ret;
    }
    else if(dpid == (DPID_BRIGHTNESS_MAX_1 + channel*16)){
        // ret = dp_download_brightness_max_handle(channel,value,length);
        return ret;
    }
    else if(dpid == (DPID_TRANSACTION_1 +channel*16)){
        // ret = dp_download_transaction_time_handle(channel,value,length);
        return ret;
    }
    else if(dpid == (DPID_LIGHT_MODE_1 +channel*16)){
        // ret = dp_download_light_mode_handle(channel,value,length);
        return ret;
    }
    else if(dpid == (DPID_CCT_OR_HSL_1 +channel*16)){
        ret = AppTask::HandleHSL(channel,value,length);
        return ret;
    }
    else if(dpid == (DPID_LIGHTNESS_1 +channel*16)){
        // ret = dp_download_lightness_handle(channel,value,length);
        return ret;
    }
    else if(dpid == DPID_TERMP_OR_HUE_1 +channel*16){
        // ret = dp_download_termp_or_hue_handle(channel,value,length);
        return ret;
    }
    else if(dpid == (DPID_SATURATION_1 +channel*16)){
        // ret = dp_download_saturation_handle(channel,value,length);
        return ret;
    }

	if(dpid == (DPID_RESTART_STATUS )){
		// ret = dp_download_restart_status_handle(value,length);
	}

  return ret;
}


void AppTask::ControlCMD(fl_data_t data)
{
    uint8_t dp_id, index;
    uint8_t dp_type;
    uint8_t ret;
    uint8_t cmd_word = data.cmdword;
    unsigned short dp_len;

    dp_id = data.data[0];
    dp_type = data.data[1];
    dp_len = data.data[2] * 0x100;
    dp_len += data.data[3];
    switch(cmd_word)
	{
	case ZIGBEE_FACTORY_NEW_CMD:
        // mcu_reset_rf();
        chip::Server::GetInstance().ScheduleFactoryReset();
		break;
	case PRODUCT_INFO_CMD:
		get_product_info(&data);
		break;
	case ZIGBEE_STATE_CMD:
        mcu_dp_report_network(1, GetNetworkState());
		break;
	case ZIGBEE_CFG_CMD:
		// mcu_reset_rf();
		chip::Server::GetInstance().ScheduleFactoryReset();
		break;
	case ZIGBEE_DATA_REQ_CMD:
		break;
	case DATA_DATA_RES_CMD:
		break;
	case DATA_REPORT_CMD:
		index = get_dowmload_dpid_index(dp_id);

        if(dp_type != download_cmd[index].dp_type){
            return 0;
        }
        else {
            // TODO: check dp_len
            ret = AppTask::UpdateDevice(dp_id, &data.data[4], dp_len);
        }
        return ret;
		break;
	case RF_IBEACON_MESSAGE:
		break;
	default:
		break;
	}


}

void AppTask::HandleUartCallback(void)
{
    //check queue_data_rx, if have data, get and process it
    if(k_msgq_num_used_get(&queue_data_rx) == 0){
        return;
    }
    //get data from queue
    fl_data_t data;
    k_msgq_get(&queue_data_rx, &data, K_NO_WAIT);
    // TODO handle data
    ControlCMD(data);

    // uart_send((uint8_t*)&data, data.datalength[1] + 9);
    return ;
}

void AppTask::UpdateNumberDevice(void)
{
    //query product info
    rf_uart_write_frame(PRODUCT_INFO_CMD, 0);
}

void AppTask::SetNetworkState(uint8_t state)
{
    if(state != networkState){
        networkState = state;
        //report network state
        // mcu_dp_report_network(1, GetNetworkState());
    }
}

void AppTask::SetInitiateAction(DeviceDimmer::Action_t aAction, int32_t aActor, uint8_t * value)
{
#if defined(CONFIG_WS2812_STRIP) || USE_RGB_PWM
    bool setRgbAction = false;
    RgbColor_t rgb;
#endif // CONFIG_WS2812_STRIP || USE_RGB_PWM

    if (aAction == DeviceDimmer::ON_ACTION || aAction == DeviceDimmer::OFF_ACTION)
    {
#ifdef CONFIG_WS2812_STRIP
        if (aAction == PWMDevice::ON_ACTION)
        {
            sAppTask.mWS2812Device.Set(SET_RGB_TURN_ON);
        }
        else if (aAction == PWMDevice::OFF_ACTION)
        {
            sAppTask.mWS2812Device.Set(SET_RGB_TURN_OFF);
        }
#else
        // sAppTask.mPwmRgbBlueLed.InitiateAction(aAction, aActor, value);
        sAppTask.mDimmerDevice.InitiateAction(aAction, aActor, value);
#if USE_RGB_PWM
        sAppTask.mPwmRgbRedLed.InitiateAction(aAction, aActor, value);
        sAppTask.mPwmRgbGreenLed.InitiateAction(aAction, aActor, value);
#endif
#endif // CONFIG_WS2812_STRIP
    }
    else if (aAction == DeviceDimmer::LEVEL_ACTION)
    {
#if defined(CONFIG_WS2812_STRIP) || USE_RGB_PWM
        // Save a new brightness for ColorControl
        sBrightness = *value;

        if (sColorAction == PWMDevice::COLOR_ACTION_XY)
        {
            rgb = XYToRgb(sBrightness, sXY.x, sXY.y);
        }
        else if (sColorAction == PWMDevice::COLOR_ACTION_HSV)
        {
            sHSV.v = sBrightness;
            rgb    = HsvToRgb(sHSV);
        }
        else
        {
            memset(&rgb, sBrightness, sizeof(RgbColor_t));
        }

        ChipLogProgress(Zcl, "New brightness: %u | R: %u, G: %u, B: %u", sBrightness, rgb.r, rgb.g, rgb.b);
        setRgbAction = true;
#else
        // sAppTask.mPwmRgbBlueLed.InitiateAction(aAction, aActor, value);
        sAppTask.mDimmerDevice.InitiateAction(aAction, aActor, value);
#endif // CONFIG_WS2812_STRIP || USE_RGB_PWM
    }
    else if (aAction == DeviceDimmer::COLOR_ACTION_HSV)
    {
        if(sAppTask.mDimmerDevice.GetType() == kTypeLight_ColorControl)
        {
            sAppTask.mDimmerDevice.InitiateAction(aAction, aActor, value);
        }
    }
#if defined(CONFIG_WS2812_STRIP) || USE_RGB_PWM
    else if (aAction == PWMDevice::COLOR_ACTION_XY)
    {
        sXY = *reinterpret_cast<XyColor_t *>(value);
        rgb = XYToRgb(sBrightness, sXY.x, sXY.y);
        ChipLogProgress(Zcl, "XY to RGB: X: %u, Y: %u, Level: %u | R: %u, G: %u, B: %u", sXY.x, sXY.y, sBrightness, rgb.r, rgb.g,
                        rgb.b);
        setRgbAction = true;
        sColorAction = PWMDevice::COLOR_ACTION_XY;
    }
    else if (aAction == PWMDevice::COLOR_ACTION_HSV)
    {
        sHSV   = *reinterpret_cast<HsvColor_t *>(value);
        sHSV.v = sBrightness;
        rgb    = HsvToRgb(sHSV);
        ChipLogProgress(Zcl, "HSV to RGB: H: %u, S: %u, V: %u | R: %u, G: %u, B: %u", sHSV.h, sHSV.s, sHSV.v, rgb.r, rgb.g, rgb.b);
        setRgbAction = true;
        sColorAction = PWMDevice::COLOR_ACTION_HSV;
    }
    else if (aAction == PWMDevice::COLOR_ACTION_CT)
    {
        sCT = *reinterpret_cast<CtColor_t *>(value);
        if (sCT.ctMireds)
        {
            rgb = CTToRgb(sCT);
            ChipLogProgress(Zcl, "ColorTemp to RGB: CT: %u | R: %u, G: %u, B: %u", sCT.ctMireds, rgb.r, rgb.g, rgb.b);
            setRgbAction = true;
            sColorAction = PWMDevice::COLOR_ACTION_CT;
        }
    }

    if (setRgbAction)
    {
#ifdef CONFIG_WS2812_STRIP
        sAppTask.mWS2812Device.SetLevel(&rgb);
#else
        sAppTask.mPwmRgbRedLed.InitiateAction(aAction, aActor, &rgb.r);
        sAppTask.mPwmRgbGreenLed.InitiateAction(aAction, aActor, &rgb.g);
        sAppTask.mPwmRgbBlueLed.InitiateAction(aAction, aActor, &rgb.b);
#endif // CONFIG_WS2812_STRIP
    }
#endif // CONFIG_WS2812_STRIP || USE_RGB_PWM
}

#ifdef CONFIG_CHIP_ENABLE_POWER_ON_FACTORY_RESET
static constexpr uint32_t kPowerOnFactoryResetIndicationMax    = 4;
static constexpr uint32_t kPowerOnFactoryResetIndicationTimeMs = 1000;

unsigned int AppTask::sPowerOnFactoryResetTimerCnt;
k_timer AppTask::sPowerOnFactoryResetTimer;

void AppTask::PowerOnFactoryResetEventHandler(AppEvent * aEvent)
{
    LOG_INF("Lighting App Power On Factory Reset Handler");
    sPowerOnFactoryResetTimerCnt = 1;
#ifdef CONFIG_WS2812_STRIP
    sAppTask.mWS2812Device.Set(sPowerOnFactoryResetTimerCnt % 2);
#else
    sAppTask.mPwmRgbBlueLed.Set(sPowerOnFactoryResetTimerCnt % 2);
#if USE_RGB_PWM
    sAppTask.mPwmRgbRedLed.Set(sPowerOnFactoryResetTimerCnt % 2);
    sAppTask.mPwmRgbGreenLed.Set(sPowerOnFactoryResetTimerCnt % 2);
#endif
#endif // CONFIG_WS2812_STRIP
    k_timer_init(&sPowerOnFactoryResetTimer, PowerOnFactoryResetTimerEvent, nullptr);
    k_timer_start(&sPowerOnFactoryResetTimer, K_MSEC(kPowerOnFactoryResetIndicationTimeMs),
                  K_MSEC(kPowerOnFactoryResetIndicationTimeMs));
}

void AppTask::PowerOnFactoryResetTimerEvent(struct k_timer * timer)
{
    sPowerOnFactoryResetTimerCnt++;
    LOG_INF("Lighting App Power On Factory Reset Handler %u", sPowerOnFactoryResetTimerCnt);
#ifdef CONFIG_WS2812_STRIP
    sAppTask.mWS2812Device.Set(sPowerOnFactoryResetTimerCnt % 2);
#else
    sAppTask.mPwmRgbBlueLed.Set(sPowerOnFactoryResetTimerCnt % 2);
#if USE_RGB_PWM
    sAppTask.mPwmRgbRedLed.Set(sPowerOnFactoryResetTimerCnt % 2);
    sAppTask.mPwmRgbGreenLed.Set(sPowerOnFactoryResetTimerCnt % 2);
#endif
#endif // CONFIG_WS2812_STRIP
    if (sPowerOnFactoryResetTimerCnt > kPowerOnFactoryResetIndicationMax)
    {
        k_timer_stop(timer);
        LOG_INF("schedule factory reset");
        chip::Server::GetInstance().ScheduleFactoryReset();
    }
}
#endif /* CONFIG_CHIP_ENABLE_POWER_ON_FACTORY_RESET */
