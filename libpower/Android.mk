# Copyright (C) 2014 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#include external/stlport/libstlport.mk
LOCAL_C_INCLUDES += $(LOCAL_PATH) \
                    external/thermal_daemon/src \
                    external/libxml2/include \
                    external/icu/icu4c/source/common \
                    system/core/include/ \
                    hardware/include \
                    system/native/include

LOCAL_MODULE := power.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_RELATIVE_PATH := hw

# main libpower source
LOCAL_SRC_FILES := power.cpp

# for all devices under /sys/power/power_HAL_suspend
LOCAL_SRC_FILES += DevicePowerMonitor.cpp \
                   DevicePowerMonitorInfo.cpp \
                   CGroupCpusetController.cpp

LOCAL_SHARED_LIBRARIES := liblog libcutils libutils libdl libicuuc libicui18n libbinder

LOCAL_MODULE_TAGS := optional


ifeq ($(APP_LAUNCH_BOOST), true)
   LOCAL_CFLAGS += -DAPP_LAUNCH_BOOST
endif
ifneq ($(TARGET_BUILD_VARIANT),user)
    LOCAL_CFLAGS += -DPOWERHAL_DEBUG
endif

ifeq ($(THERMAL_DAEMON_SUPPORT), true)
    LOCAL_SRC_FILES += thd_binder_client.cpp
    LOCAL_CFLAGS += -DTHERMAL_DAEMON_SUPPORT
endif

LOCAL_MODULE_OWNER := intel

include $(BUILD_SHARED_LIBRARY)

include $(call first-makefiles-under,$(LOCAL_PATH))
