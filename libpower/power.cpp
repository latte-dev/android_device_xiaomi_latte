/*
 * Copyright (C) 2014 Intel Corporation
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
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <pthread.h>

#include <hardware/power.h>
#include <fcntl.h>

#define LOG_TAG "PowerHAL"
#include <utils/Log.h>

#include <cutils/log.h>
#include <cutils/properties.h>
#include <hardware/hardware.h>
#include "CGroupCpusetController.h"
#include "DevicePowerMonitor.h"

#ifdef THERMAL_DAEMON_SUPPORT
#include <thd_binder_client.h>
#endif

#define ENABLE 1
#define TOUCHBOOST_PULSE_SYSFS "/sys/devices/system/cpu/cpufreq/interactive/touchboostpulse"
static const char cpufreq_boost_interactive[] = "/sys/devices/system/cpu/cpufreq/interactive/boost";
static const char cpufreq_boost_intel_pstate[] = "/sys/devices/system/cpu/intel_pstate/min_perf_pct";

/*
 * This parameter is to identify continuous touch/scroll events.
 * Any two touch hints received between a 20 interval ms is
 * considered as a scroll event.
 */
#define SHORT_TOUCH_TIME 20

/*
 * This parameter is to identify first touch events.
 * Any two touch hints received after 100 ms is considered as
 * a first touch event.
 */
#define LONG_TOUCH_TIME 100

/*
 * This parameter defines the number of vsync boost to be
 * done after the finger release event.
 */
#define VSYNC_BOOST_COUNT 4

/*
 * This parameter defines the time between a touch and a vsync
 * hint. the time if is > 30 ms, we do a vsync boost.
 */
#define VSYNC_TOUCH_TIME 30

#ifdef THERMAL_DAEMON_SUPPORT
using namespace powerhal_api;
static android::sp<IThermalAPI> shw;
#endif

static CGroupCpusetController cgroupCpusetController;
static DevicePowerMonitor powerMonitor;
static bool serviceRegistered = false;
static bool interactiveActive = false;
static bool intelPStateActive = false;

struct intel_power_module{
    struct power_module container;
    int touchboost_disable;
    int timer_set;
    int vsync_boost;
};

static int sysfs_write(const char *path, char *s)
{
    char buf[80];
    int len;
    int fd = open(path, O_WRONLY);

    if (fd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error opening %s: %s\n", path, buf);
        return -1;
    }

    len = write(fd, s, strlen(s));
    if (len < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error writing to %s: %s\n", path, buf);
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static int sysfs_read(const char *path, char *s, int length)
{
    char buf[80];
    int len;
    int fd = open(path, O_RDONLY);

    if (fd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error opening %s: %s\n", path, buf);
        return -1;
    }

    len = read(fd, s, length);
    if (len < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error reading from %s: %s\n", path, buf);
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

#ifdef APP_LAUNCH_BOOST
static void app_launch_boost_interactive(void *hint_data)
{
    if (hint_data != NULL) {
        ALOGI("PowerHAL HAL:App Boost ON");
        sysfs_write(cpufreq_boost_interactive,(char *)"1");
    } else {
        ALOGI("PowerHAL HAL:App Boost OFF");
        sysfs_write(cpufreq_boost_interactive,(char *)"0");
    }
}

static void app_launch_boost_intel_pstate(void *hint_data)
{
    static char old_min_perf_pct[4];
    static bool boosted = false;

    if (hint_data != NULL) {
        ALOGI("PowerHAL HAL:App Boost ON");
        if (boosted == false) {
            if (!sysfs_read(cpufreq_boost_intel_pstate, old_min_perf_pct, sizeof(old_min_perf_pct))) {
                sysfs_write(cpufreq_boost_intel_pstate,(char *)"100");
                boosted = true;
            }
        }
    } else {
        ALOGI("PowerHAL HAL:App Boost OFF");
        if (boosted == true) {
            sysfs_write(cpufreq_boost_intel_pstate, old_min_perf_pct);
            boosted = false;
        }
    }
}

#endif

static bool itux_or_dptf_enabled() {
    char value[PROPERTY_VALUE_MAX];
    int length = property_get("persist.thermal.mode", value, "thermald");
    std::string mode(value);

    if (mode == "itux" || mode == "ituxd" || mode == "dptf")
        return true;

    return false;
}

static void power_init(__attribute__((unused))struct power_module *module)
{
#ifdef THERMAL_DAEMON_SUPPORT
    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> binder;
#endif
    int cnt = 0;
    char buf[1];

    /* Enable all devices by default */
    powerMonitor.setState(ENABLE);
    cgroupCpusetController.setState(ENABLE);

    if (!sysfs_read(TOUCHBOOST_PULSE_SYSFS, buf, 1))
        interactiveActive = true;
    if (!sysfs_read(cpufreq_boost_intel_pstate, buf, 1))
	intelPStateActive = true;

    if (itux_or_dptf_enabled()) //we do not need the connection
        return;

#ifdef THERMAL_DAEMON_SUPPORT
    do {
        binder = sm->getService(android::String16(SERVICE_NAME));
        if (binder != 0)
            break;
        ALOGE("thd_binder_service not published, waiting...");
        usleep(500000); // 0.5 s
        if (cnt++ > 10) //do not wait forever
            return;
    } while (true);

    serviceRegistered = true;
    shw = android::interface_cast<IThermalAPI>(binder);
#endif
}

static void power_set_interactive(__attribute__((unused))struct power_module *module, int on)
{
    powerMonitor.setState(on);
    cgroupCpusetController.setState(on);
}

static void power_hint_worker(void *hint_data)
{
#ifdef THERMAL_DAEMON_SUPPORT
    struct PowerSaveMessage data = { 1 , 50 };
    status_t status;

    if (!serviceRegistered)
        return;

    if (NULL == hint_data)
        data.on = 0;

    shw->sendPowerSaveMsg(&data);
#endif
}

static void power_hint(struct power_module *module, power_hint_t hint,
                       void *data)
{
    struct intel_power_module *intel = (struct intel_power_module *) module;
    static struct timespec curr_time, prev_time = {0,0}, vsync_time;
    double diff;
    static int vsync_count;
    static int consecutive_touch_int;

    switch(hint) {
    case POWER_HINT_INTERACTION:
        if (!interactiveActive)
            return;
        clock_gettime(CLOCK_MONOTONIC, &curr_time);
        diff = (curr_time.tv_sec - prev_time.tv_sec) * 1000 +
               (double)(curr_time.tv_nsec - prev_time.tv_nsec) / 1e6;
        prev_time = curr_time;
        if (diff < SHORT_TOUCH_TIME) {
            consecutive_touch_int++;
        }
        else if (diff > LONG_TOUCH_TIME) {
            intel->vsync_boost = 0;
            intel->timer_set = 0;
            intel->touchboost_disable = 0;
            vsync_count = 0;
            consecutive_touch_int = 0;
        }
        /* Simple touch: timer rate need not be changed here */
        if ((diff < SHORT_TOUCH_TIME) && (intel->touchboost_disable == 0)
                        && (consecutive_touch_int > 4))
            intel->touchboost_disable = 1;
        /*
         * Scrolling: timer rate reduced to increase sensitivity. No more touch
         * boost after this
         */
        if ((intel->touchboost_disable == 1) && (consecutive_touch_int > 15)
                        && (intel->timer_set == 0)) {
           intel->timer_set = 1;
        }
        if (!intel->touchboost_disable) {
            sysfs_write(TOUCHBOOST_PULSE_SYSFS, (char *)"1");
        }
        break;
    case POWER_HINT_VSYNC:
        if (!interactiveActive)
            return;
        if (intel->touchboost_disable == 1) {
            clock_gettime(CLOCK_MONOTONIC, &vsync_time);
            diff = (vsync_time.tv_sec - curr_time.tv_sec) * 1000 +
            (double)(vsync_time.tv_nsec - curr_time.tv_nsec) / 1e6;
            if (diff > VSYNC_TOUCH_TIME) {
                intel->timer_set = 0;
                intel->vsync_boost = 1;
                intel->touchboost_disable = 0;
                vsync_count = VSYNC_BOOST_COUNT;
            }
        }
        if (intel->vsync_boost) {
            if (((unsigned long)data != 0) && (vsync_count > 0)) {
                sysfs_write(TOUCHBOOST_PULSE_SYSFS, (char *)"1");
                vsync_count-- ;
            if (vsync_count == 0)
               intel->vsync_boost = 0;
            }
        }
        break;
    case POWER_HINT_LOW_POWER:
        power_hint_worker(data);
        break;

#if defined(APP_LAUNCH_BOOST) && defined(APP_LAUNCH_BOOST_SUPPORTED)
    case POWER_HINT_APP_LAUNCH:
        if (interactiveActive)
            app_launch_boost_interactive(data);
	else if (intelPStateActive)
            app_launch_boost_intel_pstate(data);
#endif
    default:
        break;
    }
}

static struct hw_module_methods_t power_module_methods = {
    .open = NULL,
};

struct intel_power_module HAL_MODULE_INFO_SYM = {
    container:{
        .common = {
            .tag = HARDWARE_MODULE_TAG,
            .module_api_version = POWER_MODULE_API_VERSION_0_2,
            .hal_api_version = HARDWARE_HAL_API_VERSION,
            .id = POWER_HARDWARE_MODULE_ID,
            .name = "Intel PC Compatible Power HAL",
            .author = "Intel Open Source Technology Center",
            .methods = &power_module_methods,
            .dso = NULL,
            .reserved = {},
        },
    .init = power_init,
    .setInteractive = power_set_interactive,
    .powerHint = power_hint,
    },
    touchboost_disable: 0,
    timer_set: 0,
    vsync_boost: 0,
};
