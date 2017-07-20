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

DEVICE_PACKAGE_OVERLAYS += device/xiaomi/latte/overlay

$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)
$(call inherit-product, vendor/xiaomi/latte/latte-vendor.mk)

PRODUCT_AAPT_CONFIG := normal

# Audio
PRODUCT_PACKAGES += \
    audio.a2dp.default \
    audio.r_submix.default \
    audio.usb.default

# Charger
PRODUCT_PACKAGES += \
    charger_res_images 

# Dalvik
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapstartsize=16m \
    dalvik.vm.heapgrowthlimit=200m \
    dalvik.vm.heapsize=512m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=512k \
    dalvik.vm.heapmaxfree=16m

# Display
ADDITIONAL_DEFAULT_PROPERTIES += \
    persist.intel.ogl.username=Developer \
    persist.intel.ogl.debug=/data/ufo.prop \
    persist.intel.ogl.dumpdebugvars=1 \
    ro.ufo.use_msync=1 \
    ro.ufo.use_coreu=1

# FRP
ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.frp.pst=/dev/block/by-name/android_persistent

# Houdini
ADDITIONAL_BUILD_PROPERTIES += \
    ro.dalvik.vm.isa.arm=x86 \
    ro.enable.native.bridge.exec=1

PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    ro.dalvik.vm.native.bridge=libhoudini.so

# Lights
#PRODUCT_PACKAGES += \
    lights.gmin

# Media utils
PRODUCT_PACKAGES += \
    libI420colorconvert

# Pstore
PRODUCT_PACKAGES += \
    pstore-clean

# Thermal daemon
PRODUCT_PACKAGES += \
    thermal-daemon

PRODUCT_COPY_FILES += \
    device/xiaomi/latte/thermal_daemon/data/thermal-conf.xml:system/etc/thermal-daemon/thermal-conf.xml \
    device/xiaomi/latte/thermal_daemon/data/thermal-cpu-cdev-order.xml:system/etc/thermal-daemon/thermal-cpu-cdev-order.xml

# Wifi
PRODUCT_PACKAGES += \
    hostapd \
    wpa_supplicant

ADDITIONAL_DEFAULT_PROPERTIES += \
    wifi.interface=wlan0

# Ramdisk
PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,device/xiaomi/latte/ramdisk,root)

# Media codecs
PRODUCT_COPY_FILES += \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:system/etc/media_codecs_google_video.xml

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.manual_postprocessing.xml:system/etc/permissions/android.hardware.camera.manual_postprocessing.xml \
    frameworks/native/data/etc/android.hardware.camera.manual_sensor.xml:system/etc/permissions/android.hardware.camera.manual_sensor.xml \
    frameworks/native/data/etc/android.hardware.ethernet.xml:system/etc/permissions/android.hardware.ethernet.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:system/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:system/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/native/data/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml


# Video Acceleration API for Video Encoding and Decoding
#PRODUCT_PACKAGES += \
	libva \
	libva-android \
	libva-tpi \
	libva_videoencoder \
	libva_videodecoder


# OpenMAX Video Encoders/Decoders
#PRODUCT_PACKAGES += \
	libOMXVideoDecoderAVC \
	libOMXVideoDecoderAVCSecure \
	libOMXVideoDecoderH263 \
	libOMXVideoDecoderMPEG4 \
	libOMXVideoDecoderWMV \
	libOMXVideoEncoderAVC \
	libOMXVideoEncoderH263 \
	libOMXVideoEncoderMPEG4

# OpenMAX Interaction Layer Implementation for Intel VA API
#PRODUCT_PACKAGES += \
	wrs_omxil_core \
	libwrs_omxil_core \
	libwrs_omxil_core_pvwrapped

# StageFright Hardware Decoding
#PRODUCT_PACKAGES += \
	libstagefrighthw

# Modules location
ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.modules.location=/system/lib/modules


#Dirac
ADDITIONAL_DEFAULT_PROPERTIES += \
    persist.audio.dirac.speaker=true

#Intel prop
ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.sf.lcd_density=320 \
    persist.intel.ogl.username=Developer \
    persist.intel.ogl.debug=/data/ufo.prop \
    persist.intel.ogl.dumpdebugvars=1 \
    persist.ims_support=0 \
    config.disable_cellcoex=true \
    persist.coreu.dpst.disabled=1 

#Other stuff
ADDITIONAL_DEFAULT_PROPERTIES += \
   persist.sys.telephony.off=1 \
   persist.sys.strictmode.disable=true \
   telephony.tcs.sw_folder=generic_nomodem \
   ro.swconf.info=generic_nomodem \
   
PRODUCT_PROPERTY_OVERRIDES += \
   ro.dalvik.vm.isa.arm64=x86_64 \
   ro.enable.native.bridge.exec64=1
   
