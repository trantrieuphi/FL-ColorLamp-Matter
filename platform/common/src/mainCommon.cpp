/*
 *
 *    Copyright (c) 2021-2023 Project CHIP Authors
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

#include <lib/support/CHIPMem.h>
#include <platform/CHIPDeviceLayer.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include "libuser.h"
// #include "UartService.h"
#ifdef CONFIG_USB_DEVICE_STACK
#include <zephyr/usb/usb_device.h>
#endif /* CONFIG_USB_DEVICE_STACK */

#ifdef CONFIG_CHIP_PW_RPC
#include "Rpc.h"
#endif

// #define MSG_SIZE sizeof(frame_protocol_t)

LOG_MODULE_REGISTER(app, CONFIG_CHIP_APP_LOG_LEVEL);

using namespace ::chip;
using namespace ::chip::Inet;
using namespace ::chip::DeviceLayer;


#ifdef CONFIG_CHIP_ENABLE_POWER_ON_FACTORY_RESET
static constexpr uint32_t kFactoryResetOnBootMaxCnt       = 5;
static constexpr const char * kFactoryResetOnBootStoreKey = "TelinkFactoryResetOnBootCnt";
static constexpr uint32_t kFactoryResetUsualBootTimeoutMs = 5000;

static k_timer FactoryResetUsualBootTimer;

static void FactoryResetUsualBoot(struct k_timer * dummy)
{
    (void) dummy;
    (void) chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Delete(kFactoryResetOnBootStoreKey);
    LOG_INF("Schedule factory counter deleted");
}

static void FactoryResetOnBoot(void)
{
    uint32_t FactoryResetOnBootCnt;
    CHIP_ERROR FactoryResetOnBootErr = chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Get(
        kFactoryResetOnBootStoreKey, &FactoryResetOnBootCnt, sizeof(FactoryResetOnBootCnt));

    if (FactoryResetOnBootErr == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND)
    {
        FactoryResetOnBootCnt = 1;
        if (chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Put(kFactoryResetOnBootStoreKey, &FactoryResetOnBootCnt,
                                                                        sizeof(FactoryResetOnBootCnt)) != CHIP_NO_ERROR)
        {
            LOG_ERR("FactoryResetOnBootCnt write fail");
        }
        else
        {
            LOG_INF("Schedule factory counter %u", FactoryResetOnBootCnt);
        }
    }
    else if (FactoryResetOnBootErr == CHIP_NO_ERROR)
    {
        FactoryResetOnBootCnt++;
        if (chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Put(kFactoryResetOnBootStoreKey, &FactoryResetOnBootCnt,
                                                                        sizeof(FactoryResetOnBootCnt)) != CHIP_NO_ERROR)
        {
            LOG_ERR("FactoryResetOnBootCnt write fail");
        }
        else
        {
            LOG_INF("Schedule factory counter %u", FactoryResetOnBootCnt);
            if (FactoryResetOnBootCnt >= kFactoryResetOnBootMaxCnt)
            {
                GetAppTask().PowerOnFactoryReset();
            }
        }
    }
    else
    {
        LOG_ERR("FactoryResetOnBootCnt read fail");
    }
    k_timer_init(&FactoryResetUsualBootTimer, FactoryResetUsualBoot, nullptr);
    k_timer_start(&FactoryResetUsualBootTimer, K_MSEC(kFactoryResetUsualBootTimeoutMs), K_NO_WAIT);
}
#endif /* CONFIG_CHIP_ENABLE_POWER_ON_FACTORY_RESET */

bool IsChangeNumberDevice = false;
bool IsInitApp = false;

ProductInfo_t product_info = {
    .product_id = "unknown",
    .version = 1,
    .num_channels = 1
};

#define USER_SET_FLASH 1


void UserInit(void)
{
    read_product_info((uint8_t *)&product_info, sizeof(ProductInfo_t));
    if(product_info.num_channels == 0xFF) {
        // NUM_LIGHTS = product_info.num_channels;
        IsInitApp = false;
    } else {
        IsInitApp = true;
    }
}

/* queue to store up to 10 data frame protocol */
// K_MSGQ_DEFINE(frame_protocol_msgq, MSG_SIZE, 10, 4);
int main(void)
{
#if defined(CONFIG_USB_DEVICE_STACK) && !defined(CONFIG_CHIP_PW_RPC)
    usb_enable(NULL);
#endif /* CONFIG_USB_DEVICE_STACK */
    CHIP_ERROR err = CHIP_NO_ERROR;
    int err_uart = 0;
#ifdef CONFIG_CHIP_PW_RPC
    rpc::Init();
#endif

#if USER_SET_FLASH
    UserInit();
    
#endif
    err = chip::Platform::MemoryInit();
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("MemoryInit fail");
        goto exit;
    }

    err = PlatformMgr().InitChipStack();
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("InitChipStack fail");
        goto exit;
    }

    err = PlatformMgr().StartEventLoopTask();
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("StartEventLoopTask fail");
        goto exit;
    }
#ifdef CONFIG_CHIP_ENABLE_POWER_ON_FACTORY_RESET
    FactoryResetOnBoot();
#endif /* CONFIG_CHIP_ENABLE_POWER_ON_FACTORY_RESET */
    err = ThreadStackMgr().InitThreadStack();
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("InitThreadStack fail");
        goto exit;
    }

#if defined(CONFIG_CHIP_THREAD_DEVICE_ROLE_ROUTER)
    err = ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_Router);
#elif defined(CONFIG_CHIP_THREAD_DEVICE_ROLE_END_DEVICE)
    err = ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_MinimalEndDevice);
#elif defined(CONFIG_CHIP_THREAD_DEVICE_ROLE_SLEEPY_END_DEVICE)
    err = ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_SleepyEndDevice);
#else
#error THREAD_DEVICE_ROLE not selected
#endif
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("SetThreadDeviceType fail");
        goto exit;
    }
    err_uart = uart_init();
    if (err_uart != 0)
    {
        LOG_ERR("uart_init fail");
        goto exit;
    }
    
    err = GetAppTask().StartApp();

exit:
    LOG_ERR("Exit err %" CHIP_ERROR_FORMAT, err.Format());
    return (err == CHIP_NO_ERROR) ? EXIT_SUCCESS : EXIT_FAILURE;
}
