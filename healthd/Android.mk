LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := healthd_board_intel.cpp
LOCAL_MODULE := libhealthd.intel
LOCAL_C_INCLUDES := \
    system/core/healthd \
    bootable/recovery
#LOCAL_MODULE_TAGS := optional
include $(BUILD_STATIC_LIBRARY)

