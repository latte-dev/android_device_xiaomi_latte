LOCAL_PATH := $(call my-dir)

# Audio
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
     shim_icu53.c

LOCAL_SHARED_LIBRARIES := libicuuc libicui18n
LOCAL_MODULE := libshim_audio
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

# Camera
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    camera/CameraMetadata.cpp \
    gui/SensorManager.cpp \
    ui/GraphicBufferAllocator.cpp \
    ui/GraphicBuffer.cpp \
    ui/GraphicBufferMapper.cpp \
    shim_fence.cpp

LOCAL_C_INCLUDES += \
    system/media/camera/include

LOCAL_SHARED_LIBRARIES := liblog libcutils libhardware libui libgui libbinder libutils libsync libcamera_metadata
LOCAL_MODULE := libshim_camera
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_SHARED_LIBRARY)

# Sensors
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
     shim_icu53.c

LOCAL_SHARED_LIBRARIES := libicuuc libicui18n
LOCAL_MODULE := libshim_sensors
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
