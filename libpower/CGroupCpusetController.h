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

#ifndef ANDROID_CGROUP_CPUSET_CONTROLLER_H
#define ANDROID_CGROUP_CPUSET_CONTROLLER_H

#include <string>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <vector>

class CGroupCpusetController {

  public:
      CGroupCpusetController();
      virtual ~CGroupCpusetController() {};
      void setState(int state);

  private:
      /* "all" cpus string in root cpuset */
      char mCpusetRootCpus[10];
      char mCpusetNoninterCpus[10];
};
#endif  // ANDROID_CGROUP_CPUSET_CONTROLLER_H
