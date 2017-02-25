/*
 * Copyright (C) 2014 The Android Open Source Project
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

#include "DevicePowerMonitor.h"

#include <cutils/log.h>
#include <errno.h>

static const char* HAL_DIR = "/sys/power/power_HAL_suspend";
static const char* DEVICE_CONTROL_FILE = "power_HAL_suspend";

void DevicePowerMonitor::scanPaths()
{
    char deviceNamePath[PATH_MAX];
    DIR *dir;
    struct dirent *de;
    int cnt = 0;

    if(!mScanNeeded)
        return;

    mDevicePaths.erase(mDevicePaths.begin(), mDevicePaths.end());
    dir = opendir(HAL_DIR);
    if(dir == NULL){
        ALOGE("Could not open directory '%s': %s", HAL_DIR, strerror(errno));
        return;
    }
    while((de = readdir(dir))) {
        if(de->d_name[0] == '.')
            continue;

        snprintf(deviceNamePath, sizeof(deviceNamePath), "%s/%s/%s", HAL_DIR, de->d_name, DEVICE_CONTROL_FILE);
        int fd = ::open(deviceNamePath, O_WRONLY);
        if(fd < 0){
            ALOGE("Could not open file '%s': %s", deviceNamePath, strerror(errno));
            continue;
        }
        close(fd);

        bool blacklist = false;
        unsigned int i;
        for(i = 0;  i < DevicePowerMonitorInfo::numDev; i++){
            if(!strncmp(DevicePowerMonitorInfo::deviceBlackList[i], de->d_name,
	       strlen(DevicePowerMonitorInfo::deviceBlackList[i]))){
                ALOGD("Found device: %s in blacklist", de->d_name);
                blacklist = true;
                break;
            }
        }

        if(!blacklist){
            snprintf(deviceNamePath, sizeof(deviceNamePath), "%s/%s/%s", HAL_DIR, de->d_name, DEVICE_CONTROL_FILE);
            mDevicePaths.push_back(deviceNamePath);
        }
    }
    if(mDevicePaths.size() > 0){
        mScanNeeded = false;
    }

    closedir(dir);
}

void DevicePowerMonitor::setState(int state)
{
    unsigned int quitLoop = 0;
    ssize_t ret = 0;
    scanPaths();
    std::vector<std::string>::iterator it=mDevicePaths.begin();
    while(it != mDevicePaths.end())
    {
        int fd = ::open((char*) it->c_str(), O_WRONLY);
        if(fd < 0){
            ALOGE("Could not open the file:%s", (char*) it->c_str());
            /*
                We might have issue that the kernel removed the node so we need to re-scan.
                However if we have permission problem we do not want to be stuck forever in this loop
            */
            mScanNeeded = true;
            scanPaths();
            it = mDevicePaths.begin();
            if(quitLoop++ > 0)
                break;
            continue;
        }

        if(state){
            ret = write(fd, "0", 1);
        }
        else{
            ret = write(fd, "1", 1);
        }
        if (ret < 0)
            ALOGE("Error when trying to write to the file errno:%d", errno);
        close(fd);
        it++;

    }
}
