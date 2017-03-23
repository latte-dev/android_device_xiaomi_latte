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

DEVICE_PATH := device/xiaomi/latte

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
BOARD_KERNEL_CMDLINE := loglevel=7 androidboot.hardware=latte firmware_class.path=/system/etc/firmware i915.fastboot=1 vga=current i915.modeset=1 drm.vblankoffdelay=1 bootboost=1 pm_suspend_debug=1 pstore.backend=ramoops thermal.off=1 androidboot.selinux=permissive 

# PowerHAL
INTEL_POWER_HAL_INTERACTIVE_GOV := true

# Binder
TARGET_USES_64_BIT_BINDER := true

# CPUset
ENABLE_CPUSETS := true

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(DEVICE_PATH)/bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true

# Camera
COMMON_GLOBAL_CFLAGS += -DCAMERA_VENDOR_L_COMPAT

# Charger
WITH_CM_CHARGER := true

# Graphics
USE_OPENGL_RENDERER := true
NUM_FRAMEBUFFER_SURFACE_BUFFERS := 3
OVERRIDE_RS_DRIVER := libRSDriver_intel.so
VSYNC_EVENT_PHASE_OFFSET_NS := 7500000
SF_VSYNC_EVENT_PHASE_OFFSET_NS := 5000000

# Houdini
BUILD_ARM_FOR_X86 := true

# Linker
TARGET_NEEDS_PLATFORM_TEXT_RELOCATIONS := true

# Partition sizes
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x01E00000
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x01E00000
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2684354560
BOARD_CACHEIMAGE_PARTITION_SIZE := 268435456
BOARD_FLASH_BLOCK_SIZE := 512

#OTA
BLOCK_BASED_OTA:= false

# Disable journaling to save space
BOARD_SYSTEMIMAGE_JOURNAL_SIZE := 0

# Recovery
TARGET_RECOVERY_FSTAB := $(DEVICE_PATH)/ramdisk/fstab
TARGET_RECOVERY_PIXEL_FORMAT := "BGRA_8888"
TARGET_USERIMAGES_USE_EXT4 := true
COMMON_GLOBAL_CFLAGS += -DNO_SECURE_DISCARD

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

# Assert
TARGET_OTA_ASSERT_DEVICE := latte

# Partition sizes
BOARD_USERDATAIMAGE_PARTITION_SIZE := 27783069696

# Properties
TARGET_SYSTEM_PROP += $(DEVICE_PATH)/system.prop

#Root in lineageOS
WITH_SU := true

# SELinux
BOARD_SEPOLICY_DIRS += \
    device/xiaomi/latte/sepolicy \
    device/xiaomi/latte/sepolicy/dollar_cove \
    device/xiaomi/latte/sepolicy/intel_prop \
    device/xiaomi/latte/sepolicy/sensorhub \
    device/xiaomi/latte/sepolicy/telephony

# Kernel
BOARD_KERNEL_IMAGE_NAME := bzImage
TARGET_KERNEL_ARCH := x86_64
TARGET_KERNEL_SOURCE := kernel/xiaomi/latte
TARGET_KERNEL_CONFIG := lineageos_latte_defconfig
TARGET_KERNEL_CROSS_COMPILE_PREFIX := x86_64-linux-android-

