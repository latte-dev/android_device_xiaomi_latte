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
PRODUCT_CHARACTERISTICS := tablet
PRODUCT_AAPT_PREF_CONFIG := hdpi

#Prebuilt kernel
TARGET_PREBUILT_KERNEL:= device/xiaomi/latte/zImage

# Audio
PRODUCT_PACKAGES += \
    audio.a2dp.default \
    audio.r_submix.default \
    audio.usb.default

# Charger
PRODUCT_PACKAGES += \
    charger_res_images

# PowerHAL
PRODUCT_PACKAGES += \
    power.gmin \
    power_hal_helper

# Pstore
PRODUCT_PACKAGES += \
    pstore-clean

# Compatibility
PRODUCT_PACKAGES += \
    libshim_audio \
    libshim_camera \
    libshim_sensors \
    libstlport

# Lights
PRODUCT_PACKAGES += \
    lights.gmin

# Dalvik
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapstartsize=8m \
    dalvik.vm.heapgrowthlimit=144m \
    dalvik.vm.heapsize=384m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=512k \
    dalvik.vm.heapmaxfree=8m \
    dalvik.vm.dex2oat-filter=speed

# Display
ADDITIONAL_DEFAULT_PROPERTIES += \
    intel.hwc.autovideoddr=1 \
    ro.ufo.use_msync=1 \
    ro.ufo.use_coreu=1 \
    persist.gen_gfxd.enable=0

# FRP
ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.frp.pst=/dev/block/by-name/android_persistent

# Houdini
ADDITIONAL_BUILD_PROPERTIES += \
    ro.dalvik.vm.isa.arm=x86 \
    ro.enable.native.bridge.exec=1

ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.dalvik.vm.native.bridge=libhoudini.so


# Wifi
PRODUCT_PACKAGES += \
    hostapd \
    wpa_supplicant

ADDITIONAL_DEFAULT_PROPERTIES += \
    wifi.interface=wlan0

# Modules location
ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.modules.location=/system/lib/modules

#Dirac
ADDITIONAL_DEFAULT_PROPERTIES += \
    persist.audio.dirac.speaker=true

#Else
ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.sf.lcd_density=320 \
    persist.intel.ogl.username=Developer \
    persist.intel.ogl.debug=/data/ufo.prop \
    persist.intel.ogl.dumpdebugvars=1 \
    persist.ims_support=0 \
    config.disable_cellcoex=true

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
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.ethernet.xml:system/etc/permissions/android.hardware.ethernet.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.software.midi.xml:system/etc/permissions/android.software.midi.xml \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/native/data/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml
