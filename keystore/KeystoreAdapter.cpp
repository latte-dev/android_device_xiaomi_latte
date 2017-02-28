/*
 * Copyright (C) 2015, The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// #define LOG_NDEBUG 0

#define LOG_TAG "KeystoreAdapter"
#include <cutils/log.h>

#include <utils/threads.h>
#include <utils/String8.h>
#include <hardware/keymaster0.h>

static struct keystore_module *gVendorModule;

static int keymaster_device_open(const hw_module_t* module, const char* name,
                hw_device_t** device)
{
    int rv;

    rv = hw_get_module_by_class("keystore", "vendor", (const hw_module_t **)&gVendorModule);
    if (rv) {
        ALOGE("failed to open vendor camera module");
        return rv;
    } else {
        return gVendorModule->common.methods->open(
                    (const hw_module_t*)gVendorModule, name, device);
    }
}

static struct hw_module_methods_t keymaster_module_methods = {
    .open = keymaster_device_open
};

struct keystore_module HAL_MODULE_INFO_SYM = {
    .common = {
         tag: HARDWARE_MODULE_TAG,
         .module_api_version = KEYMASTER_DEVICE_API_VERSION_0_3,
         .hal_api_version = HARDWARE_HAL_API_VERSION,
         .id = KEYSTORE_HARDWARE_MODULE_ID,
         .name = "Cherry-Trail Keystore Adapter",
         .author = "The LIneageOS Project",
         .methods = &keymaster_module_methods,
         .dso = NULL, /* remove compilation warnings */
         .reserved = {0}, /* remove compilation warnings */
    }
};
