LOCAL_PATH := $(call my-dir)

# Audio
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
     shim_icu53.c

LOCAL_SHARED_LIBRARIES := libicuuc libicui18n
LOCAL_MODULE := libshim_audio
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

# Sensors
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
     shim_icu53.c

LOCAL_SHARED_LIBRARIES := libicuuc libicui18n
LOCAL_MODULE := libshim_sensors
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
