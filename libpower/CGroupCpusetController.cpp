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

#define LOG_TAG "PowerHAL"
#define LOG_NDEBUG 0

#include "CGroupCpusetController.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>

#include <cutils/log.h>
#include <cutils/properties.h>
#include <errno.h>
#include <string.h>

static const char* CPUSET_ROOT_CPUS = "/dev/cpuset/cpus";
static const char* CPUSET_NON_INTERACTIVE_CPUS = "/dev/cpuset/power_hal/non_interactive/cpus";
static const char* POWER_HAL_CPUSET_PROPERTY = "ro.powerhal.cpuset_config";
static const char* POWER_HAL_CPUSET_PROPERTY_DEBUG = "persist.powerhal.cpuset_config"; /* for userdebug, eng build tuning*/

CGroupCpusetController::CGroupCpusetController()
{
    int fd;
    int ret;
    int len;
    char cpuset_config[PROPERTY_VALUE_MAX];

#ifdef POWERHAL_DEBUG
    len = property_get(POWER_HAL_CPUSET_PROPERTY_DEBUG, cpuset_config, NULL);
    if (len > 0) {
        cpuset_config[len] = '\0';
        char *conf;
        char *next_token;
        conf = strtok_r(cpuset_config, ";", &next_token);
        if (conf)
            snprintf(mCpusetRootCpus, strlen(conf)+1, "%s", conf);
        conf = strtok_r(NULL, ";", &next_token);
        if (conf)
            snprintf(mCpusetNoninterCpus, strlen(conf)+1, "%s", conf);
        return;
    }
#endif
    len = property_get(POWER_HAL_CPUSET_PROPERTY, cpuset_config, NULL);
    if (len > 0) {
        cpuset_config[len] = '\0';
        char *conf;
        char *next_token;
        conf = strtok_r(cpuset_config, ";", &next_token);
        if (conf)
            snprintf(mCpusetRootCpus, strlen(conf)+1, "%s", conf);
        conf = strtok_r(NULL, ";", &next_token);
        if (conf)
            snprintf(mCpusetNoninterCpus, strlen(conf)+1, "%s", conf);
    } else {
        /* Default to set .cpus to 0 */
        snprintf(mCpusetRootCpus, sizeof(mCpusetRootCpus), "0");

        /**
         * Read the default cpuset .cpus number.
         * Will be used when device is interactive.
         */
        fd = open(CPUSET_ROOT_CPUS, O_RDONLY);

        if (fd < 0) {
            /* not a hard error; default is "0" (CPU core #0 only). */
            ALOGV("Could not open the file: %s (%d)", CPUSET_ROOT_CPUS, errno);
            return;
        }

        ret = read(fd, mCpusetRootCpus, sizeof(mCpusetRootCpus));
        if (ret <= 0) {
            /* nothing is being read but not a hard error */
            /* default is "0" (CPU core #0 only).         */
            ALOGV("Error when reading from file (%d)", errno);
            close(fd);
            return;
        }

        mCpusetRootCpus[sizeof(mCpusetRootCpus) - 1] = '\0';
        close(fd);
        mCpusetNoninterCpus[0] = '0';
        mCpusetNoninterCpus[1] = '\0';
    }
    return;
}

void CGroupCpusetController::setState(int state)
{
    int fd;
    int ret = 0;

    /**
     * Enable all cpus if interactive
     * Restrict to certrain CPUs if non-interactive.
     */
    fd = open(CPUSET_NON_INTERACTIVE_CPUS, O_WRONLY);

    if (fd < 0) {
        ALOGE("Could not open the file: %s (%d)", CPUSET_NON_INTERACTIVE_CPUS, errno);
        return;
    }

    if (state) {
        /* Let loose when interactive */
        ret = write(fd, mCpusetRootCpus, sizeof(mCpusetRootCpus));
    }
    else {
        /* Restrict when non-interactive */
        ret = write(fd, mCpusetNoninterCpus, sizeof(mCpusetNoninterCpus));
    }

    if (ret < 0) {
        ALOGE("Error when writing to the file (%d)", errno);
    }

    close(fd);
}
