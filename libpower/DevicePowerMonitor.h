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

#ifndef ANDROID_POWER_MONITOR_H
#define ANDROID_POWER_MONITOR_H

#include <string>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <vector>

#include "DevicePowerMonitorInfo.h"

#define DEVICE_NAME_MAX 256

struct sensors_event_t;

/**
 * The class is used to remove/add input i2c devices through input sysfs system
 * so the devices would power down correctly
 */
class DevicePowerMonitor {

  private:
      std::vector<std::string> mDevicePaths;
      bool mScanNeeded;
      void cleanPaths();
      void scanPaths();

  public:
      DevicePowerMonitor():
          mScanNeeded(true){};
      virtual ~DevicePowerMonitor(){};
      void setState(int state);

};
#endif  // ANDROID_I2C_POWER_MONITOR_H
