#
# Copyright (C) 2016 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

include $(call all-makefiles-under,$(LOCAL_PATH))

# Hack for prebuilt kernel
$(shell mkdir -p $(OUT)/obj/KERNEL_OBJ/usr/include)

# Hack for prebuilt bluetooth
$(shell mkdir -p $(TARGET_OUT)/app/Bluetooth/lib/x86; \
    ln -sf /system/lib/libbluetooth_jni.so \
    $(TARGET_OUT)/app/Bluetooth/lib/x86/libbluetooth_jni.so)

# Houdini
$(shell mkdir -p $(TARGET_OUT)/bin; \
    mkdir -p $(TARGET_OUT)/lib; \
    ln -sf /vendor/bin/houdini \
    $(TARGET_OUT)/bin/houdini; \
    ln -sf /vendor/lib/arm \
    $(TARGET_OUT)/lib/arm; \
    ln -sf /vendor/lib/libhoudini.so \
    $(TARGET_OUT)/lib/libhoudini.so)
