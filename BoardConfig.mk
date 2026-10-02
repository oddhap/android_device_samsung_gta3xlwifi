# SPDX-License-Identifier: Apache-2.0
DEVICE_PATH := device/samsung/gta3xlwifi
SYSTEM_EXT_PRIVATE_SEPOLICY_DIRS += $(DEVICE_PATH)/sepolicy/private

# The stock vendor ABI is 32-bit; the separately compiled kernel is ARM64.
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_VARIANT := generic
TARGET_CPU_VARIANT_RUNTIME := cortex-a53
TARGET_USES_64_BIT_BINDER := true

TARGET_BOARD_PLATFORM := universal7904
TARGET_SOC := exynos7904
TARGET_BOOTLOADER_BOARD_NAME := universal7904
BOARD_VENDOR := samsung
TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

# Initial Android integration uses our source-built kernel artifact. Its source,
# config, changes, and SHA-256 are recorded by tools/build-kernel-smoke.sh.
TARGET_KERNEL_ARCH := arm64
TARGET_KERNEL_CONFIG := exynos7885-gta3xlwifi_defconfig
BOARD_KERNEL_IMAGE_NAME := Image
TARGET_PREBUILT_KERNEL := $(DEVICE_PATH)/prebuilt/Image
BOARD_KERNEL_BASE := 0x10000000
BOARD_KERNEL_PAGESIZE := 2048
BOARD_BOOT_HEADER_VERSION := 1
BOARD_MKBOOTIMG_ARGS := --header_version 1 --kernel_offset 0x00008000 --ramdisk_offset 0x01000000 --tags_offset 0x00000100 --board SRPSA25A005KU
BOARD_KERNEL_CMDLINE := androidboot.hardware=exynos7904 androidboot.selinux=enforcing
BOARD_RECOVERY_MKBOOTIMG_ARGS := --header_version 1 --kernel_offset 0x00008000 --ramdisk_offset 0x01000000 --tags_offset 0x00000100 --board SRPSA25A005RU
BOARD_PREBUILT_DTBOIMAGE := $(DEVICE_PATH)/prebuilt/dtbo.img
BOARD_INCLUDE_RECOVERY_DTBO := true
BOARD_CUSTOM_BOOTIMG_MK := $(DEVICE_PATH)/mkbootimg.mk
# Preserve the custom images in target-files and recovery-from-boot generation.
# Otherwise releasetools recreates recovery.img and drops the Samsung trailer.
BOARD_CUSTOM_BOOTIMG := true

# Physical sizes read from this tablet; no dynamic partitions or A/B slots.
BOARD_BUILD_SYSTEM_ROOT_IMAGE := true
TARGET_USERIMAGES_USE_EXT4 := true
TARGET_USERIMAGES_USE_F2FS := true
BOARD_BOOTIMAGE_PARTITION_SIZE := 33554432
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 47185920
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 3196059648
BOARD_VENDORIMAGE_PARTITION_SIZE := 343932928
BOARD_PRODUCTIMAGE_PARTITION_SIZE := 327155712
BOARD_CACHEIMAGE_PARTITION_SIZE := 159383552
BOARD_SYSTEMIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_PRODUCTIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_FLASH_BLOCK_SIZE := 131072
TARGET_COPY_OUT_VENDOR := vendor
TARGET_COPY_OUT_PRODUCT := product
BOARD_PREBUILT_VENDORIMAGE := $(DEVICE_PATH)/prebuilt/vendor.img

# Preserve stock vendor policy. Do not allow permissive domains or disable
# neverallow checks to make the build pass.
# Generated staging policy uses the framework version. The actual stock image
# carries version 30.0 itself; init selects its compatibility mapping at boot.
BOARD_VNDK_VERSION := current
DEVICE_MANIFEST_FILE := $(DEVICE_PATH)/configs/manifest.xml
DEVICE_MANIFEST_FILE += $(wildcard $(DEVICE_PATH)/configs/stock/etc/vintf/manifest/*.xml)
DEVICE_MATRIX_FILE := $(DEVICE_PATH)/configs/device_matrix.xml
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE := $(DEVICE_PATH)/configs/framework_matrix.xml

BOARD_HAS_DOWNLOAD_MODE := true
TARGET_RECOVERY_FSTAB := $(DEVICE_PATH)/rootdir/etc/recovery.fstab
TARGET_OTA_ASSERT_DEVICE := gta3xlwifi
BOARD_AVB_ENABLE := false
TARGET_SCREEN_DENSITY := 240

# Select the kernel requirements for the framework's restored qtaguid path.
SOONG_CONFIG_NAMESPACES += gta3xlwifi
SOONG_CONFIG_gta3xlwifi += legacy_networking
SOONG_CONFIG_gta3xlwifi_legacy_networking := true

# Native PowerManager backend complements the unmodified stock Power HAL.
SOONG_CONFIG_gta3xlwifi += native_power
SOONG_CONFIG_gta3xlwifi_native_power := true
