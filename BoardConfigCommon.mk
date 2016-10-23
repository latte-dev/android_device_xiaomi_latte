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

COMMON_PATH := device/intel/cherrytrail-common

# Platform
TARGET_NO_BOOTLOADER := true

TARGET_BOARD_PLATFORM := gmin
TARGET_BOARD_PLATFORM_GPU := intel_gen8
TARGET_BOOTLOADER_BOARD_NAME := cherrytrail

TARGET_ARCH := x86
TARGET_ARCH_VARIANT := silvermont
TARGET_CPU_ABI := x86
TARGET_CPU_ABI_LIST := x86,armeabi-v7a,armeabi
TARGET_CPU_ABI_LIST_32_BIT := x86,armeabi-v7a,armeabi

# Kernel
BOARD_KERNEL_CMDLINE := loglevel=5 androidboot.hardware=cherrytrail firmware_class.path=/system/etc/firmware i915.fastboot=1 vga=current i915.modeset=1 drm.vblankoffdelay=1 console=ttyS0,115200n8 bootboost=1 pm_suspend_debug=1 pstore.backend=ramoops
BOARD_KERNEL_CMDLINE += androidboot.selinux=permissive

# Binder
TARGET_USES_64_BIT_BINDER := true

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(COMMON_PATH)/bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_RTK := true
BLUETOOTH_HCI_USE_RTK_H5 := true

# Charger
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
    device/intel/sepolicy \
    device/intel/cherrytrail-common/sepolicy \
    device/intel/cherrytrail-common/sepolicy/dollar_cove \
    device/intel/cherrytrail-common/sepolicy/intel_prop \
    device/intel/cherrytrail-common/sepolicy/sensorhub \
    device/intel/cherrytrail-common/sepolicy/telephony

BOARD_SEPOLICY_UNION += \
    adbd.te \
    atproxy.te \
    bcu_cpufreqrel.te \
    bluetooth.te \
    cg2k.te \
    coreu.te \
    cws_manu.te \
    device.te \
    drmserver.te \
    dumpstate.te \
    efiprop.te \
    fg_algo_cos.te \
    fg_algo_iface.te \
    file_contexts \
    file.te \
    genfs_contexts \
    gpsd.te \
    hdcpd.te \
    hostapd.te \
    init_shell.te \
    init.te \
    intel_prop.te \
    kernel.te \
    keymaster.te \
    keystore.te \
    mediaserver.te \
    mmgr.te \
    msync.te \
    mts.te \
    netd.te \
    nfc.te \
    nvm_server.te \
    platform_app.te \
    power_hal_helper.te \
    property_contexts \
    property.te \
    pstore-clean.te \
    radio.te \
    recovery.te \
    rild.te \
    sensorhubd.te \
    service_contexts \
    service_manager.te \
    service.te \
    shell.te \
    silentlake.te \
    surfaceflinger.te \
    system_app.te \
    system_server.te \
    te_macros \
    thermal_lite.te \
    thermal.te \
    ueventd.te \
    untrusted_app.te \
    userfastboot.te \
    vdc.te \
    vold.te \
    wlan_prov.te \
    wpa.te

# Wifi
WPA_SUPPLICANT_VERSION := VER_2_1_DEVEL
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WLAN_DEVICE := rtl
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_rtl
