/*
 * Copyright (C) 2014 Intel Corporation
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Implementation to add battery ocv property and disable periodic polling
 */

#define LOG_TAG "healthd-intel"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utils/String8.h>
#include <cutils/klog.h>

#include <healthd.h>


#define POWER_SUPPLY_SUBSYSTEM "power_supply"
#define POWER_SUPPLY_SYSFS_PATH "/sys/class/" POWER_SUPPLY_SUBSYSTEM
#define PERIODIC_CHORES_INTERVAL_FAST (60 * 1)
#define PERIODIC_CHORES_INTERVAL_FAST_INITIAL 1

static struct healthd_config *ghc;

using namespace android;

static int readFromFile(const String8& path, char* buf, size_t size) {
    char *cp = NULL;

    if (path.isEmpty())
        return -1;
    int fd = open(path.string(), O_RDONLY, 0);
    if (fd == -1) {
        KLOG_ERROR(LOG_TAG, "Could not open '%s'\n", path.string());
        return -1;
    }

    ssize_t count = TEMP_FAILURE_RETRY(read(fd, buf, size));
    if (count > 0)
            cp = (char *)memrchr(buf, '\n', count);

    if (cp)
        *cp = '\0';
    else
        buf[0] = '\0';

    close(fd);
    return count;
}

static bool isBattery(const String8 &path)
{
    const int SIZE = 128;
    char buf[SIZE] = {0};

    int length = readFromFile(path, buf, SIZE);

    if (length <= 0)
        return false;

    if (strcmp(buf, "Battery") == 0)
        return true;

    return false;
}

static void init_battery_path(struct healthd_config *hc) {
    String8 path;

    DIR* dir = opendir(POWER_SUPPLY_SYSFS_PATH);
    if (dir == NULL) {
        KLOG_ERROR(LOG_TAG, "Could not open %s\n", POWER_SUPPLY_SYSFS_PATH);
    } else {
        struct dirent* entry;

        while ((entry = readdir(dir))) {
            const char* name = entry->d_name;

            if (!strcmp(name, ".") || !strcmp(name, ".."))
                continue;

            char buf[20];
            // Look for "type" file in each subdirectory
            path.clear();
            path.appendFormat("%s/%s/type", POWER_SUPPLY_SYSFS_PATH, name);
            if (isBattery(path)) {
                if (hc->batteryVoltagePath.isEmpty()) {
                    path.clear();
                    path.appendFormat("%s/%s/voltage_ocv",
                                      POWER_SUPPLY_SYSFS_PATH, name);
                    if (access(path, R_OK) == 0) {
                        hc->batteryVoltagePath = path;
                    } else {
                        path.clear();
                        path.appendFormat("%s/%s/voltage_now",
                                          POWER_SUPPLY_SYSFS_PATH, name);
                        if (access(path, R_OK) == 0) {
                            hc->batteryVoltagePath = path;
                        } else {
                                path.clear();
                                path.appendFormat("%s/%s/batt_vol",
                                          POWER_SUPPLY_SYSFS_PATH, name);
                                if (access(path, R_OK) == 0)
                                    hc->batteryVoltagePath = path;
                        }
                    }
                    break;
                }

            }
        }
        closedir(dir);
    }
}

void healthd_board_init(struct healthd_config *config)
{
     init_battery_path(config);
     config->periodic_chores_interval_fast =
                      PERIODIC_CHORES_INTERVAL_FAST_INITIAL;
     config->periodic_chores_interval_slow = -1;
     ghc = config;
}


int healthd_board_battery_update(struct android::BatteryProperties *props)
{
    if (ghc->periodic_chores_interval_fast ==
               PERIODIC_CHORES_INTERVAL_FAST_INITIAL)
       ghc->periodic_chores_interval_fast = -1;

    if ((props->chargerAcOnline | props->chargerUsbOnline |
           props->chargerWirelessOnline) && (props->batteryLevel == 0))
        ghc->periodic_chores_interval_fast =
                                     PERIODIC_CHORES_INTERVAL_FAST;
    return 0;
}
