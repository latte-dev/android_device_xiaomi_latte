
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

COMMON_PATH := device/xiaomi/latte

# Platform
TARGET_NO_BOOTLOADER := true

TARGET_BOARD_PLATFORM := gmin
TARGET_BOARD_PLATFORM_GPU := intel_gen8
TARGET_BOOTLOADER_BOARD_NAME := latte

TARGET_ARCH := x86
TARGET_ARCH_VARIANT := silvermont
TARGET_CPU_ABI := x86
TARGET_CPU_ABI_LIST := x86,armeabi-v7a,armeabi
TARGET_CPU_ABI_LIST_32_BIT := x86,armeabi-v7a,armeabi

# Kernel
BOARD_KERNEL_CMDLINE := loglevel=7 androidboot.hardware=latte firmware_class.path=/system/etc/firmware i915.fastboot=1 vga=current i915.modeset=1 drm.vblankoffdelay=1 bootboost=1 pm_suspend_debug=1 pstore.backend=ramoops androidboot.selinux=permissive

# Binder
TARGET_USES_64_BIT_BINDER := true

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(COMMON_PATH)/bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true

# Charger
BOARD_CHARGER_SHOW_PERCENTAGE := true
BOARD_HAL_STATIC_LIBRARIES := libhealthd.intel

# Graphics
USE_OPENGL_RENDERER := true
NUM_FRAMEBUFFER_SURFACE_BUFFERS := 3
OVERRIDE_RS_DRIVER := libRSDriver_intel.so
VSYNC_EVENT_PHASE_OFFSET_NS := 7500000
SF_VSYNC_EVENT_PHASE_OFFSET_NS := 5000000

# Houdini
BUILD_ARM_FOR_X86 := true

# Partition sizes
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x01E00000
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x01E00000
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2684354560
BOARD_CACHEIMAGE_PARTITION_SIZE := 268435456
BOARD_FLASH_BLOCK_SIZE := 512

# Properties
TARGET_SYSTEM_PROP += $(COMMON_PATH)/system.prop

# Recovery
TARGET_RECOVERY_FSTAB := $(COMMON_PATH)/ramdisk/fstab
TARGET_RECOVERY_PIXEL_FORMAT := "BGRA_8888"
TARGET_USERIMAGES_USE_EXT4 := true
COMMON_GLOBAL_CFLAGS += -DNO_SECURE_DISCARD

# SELinux
BOARD_SEPOLICY_DIRS += \
    $(COMMON_PATH)/sepolicy \
    $(COMMON_PATH)/sepolicy/atomisp \
    $(COMMON_PATH)/sepolicy/bluetooth \
    $(COMMON_PATH)/sepolicy/color_config \
    $(COMMON_PATH)/sepolicy/coredump \
    $(COMMON_PATH)/sepolicy/crashlogd \
    $(COMMON_PATH)/sepolicy/debug-logs \
    $(COMMON_PATH)/sepolicy/device \
    $(COMMON_PATH)/sepolicy/dollar_cove \
    $(COMMON_PATH)/sepolicy/gps \
    $(COMMON_PATH)/sepolicy/graphics \
    $(COMMON_PATH)/sepolicy/intel_prop \
    $(COMMON_PATH)/sepolicy/media \
    $(COMMON_PATH)/sepolicy/sensorhub \
    $(COMMON_PATH)/sepolicy/set_storage \
    $(COMMON_PATH)/sepolicy/telephony 


# OpenMAX Interaction Layer Implementation for Intel VA API
#BOARD_USES_MRST_OMX := true
#BOARD_USES_WRS_OMXIL_CORE := true
#TARGET_HAS_ISV := false

# Video Acceleration API for Video Encoding and Decoding
#INTEL_VA := true
#BOARD_USE_LIBVA := true
#BOARD_USE_LIBVA_INTEL_DRIVER := true
#USE_INTEL_SECURE_AVC := true
#BOARD_WIDEVINE_OEMCRYPTO_LEVEL := 1

# Intel Moorestown Mix Library
#BOARD_USE_LIBMIX := true
#ENABLE_IMG_GRAPHICS = true


# Wifi
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_HOSTAPD_PRIVATE_LIB      := lib_driver_cmd_bcmdhd
BOARD_HOSTAPD_DRIVER           := NL80211
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WLAN_DEVICE := bcmdhd
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_bcmdhd
WIFI_DRIVER_FW_PATH_PARAM := "/sys/module/bcmdhd_pcie/parameters/firmware_path"
WIFI_DRIVER_FW_PATH_STA := "/vendor/firmware/brcm/fw_bcmdhd_4356a2_pcie.bin"
WIFI_DRIVER_FW_PATH_AP := "/vendor/firmware/brcm/fw_bcmdhd_4356a2_pcie_apsta.bin"
WIFI_DRIVER_FW_PATH_P2P := "/vendor/firmware/brcm/fw_bcmdhd_4356a2_pcie.bin"

DEVICE_PATH := device/xiaomi/latte

# Assert
TARGET_OTA_ASSERT_DEVICE := latte

# Partition sizes
BOARD_USERDATAIMAGE_PARTITION_SIZE := 27783069696

#Kernel Build from source
TARGET_KERNEL_CROSS_COMPILE_PREFIX := x86_64-linux-android-
TARGET_KERNEL_CONFIG := lineageos_latte_defconfig
TARGET_KERNEL_SOURCE := kernel/xiaomi/latte
TARGET_KERNEL_ARCH := x86_64
BOARD_KERNEL_IMAGE_NAME := bzImage

