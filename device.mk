# SPDX-License-Identifier: Apache-2.0
LOCAL_PATH := device/samsung/gta3xlwifi
DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay
# Manifest enablement must be compiled into TeleService, not deferred to RRO.
PRODUCT_ENFORCE_RRO_EXCLUDED_OVERLAYS += $(LOCAL_PATH)/overlay/packages/services/Telephony
$(call inherit-product, frameworks/native/build/phone-xhdpi-2048-dalvik-heap.mk)

PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := hdpi
TARGET_SCREEN_WIDTH := 1200
TARGET_SCREEN_HEIGHT := 1920
TARGET_EXCLUDES_AUDIOFX := true
PRODUCT_ENFORCE_VINTF_MANIFEST := true
PRODUCT_USE_DYNAMIC_PARTITIONS := false
PRODUCT_BUILD_SUPER_PARTITION := false
PRODUCT_BUILD_SYSTEM_IMAGE := true
PRODUCT_BUILD_PRODUCT_IMAGE := true
PRODUCT_BUILD_VENDOR_IMAGE := false
PRODUCT_BUILD_RECOVERY_IMAGE := true

PRODUCT_PACKAGES += \
    android.hardware.health-service.example_recovery

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/cgroups.gta3xlwifi.json:$(TARGET_COPY_OUT_SYSTEM)/etc/cgroups.gta3xlwifi.json \
    $(LOCAL_PATH)/configs/task_profiles.gta3xlwifi.json:$(TARGET_COPY_OUT_SYSTEM)/etc/task_profiles.gta3xlwifi.json \
    $(LOCAL_PATH)/rootdir/init.gta3xlwifi.crypto.rc:$(TARGET_COPY_OUT_SYSTEM)/etc/init/init.gta3xlwifi.crypto.rc \
    $(LOCAL_PATH)/rootdir/init.gta3xlwifi.power.rc:$(TARGET_COPY_OUT_SYSTEM_EXT)/etc/init/init.gta3xlwifi.power.rc \
    $(LOCAL_PATH)/rootdir/etc/fstab.exynos7904:$(TARGET_COPY_OUT_ROOT)/fstab.exynos7904 \
    $(LOCAL_PATH)/rootdir/init.exynos7904.rc:$(TARGET_COPY_OUT_ROOT)/init.exynos7904.rc \
    $(LOCAL_PATH)/rootdir/ueventd.exynos7904.rc:$(TARGET_COPY_OUT_ROOT)/ueventd.exynos7904.rc

PRODUCT_SYSTEM_PROPERTIES += \
    ro.sf.lcd_density=240 \
    ro.opengles.version=196610 \
    ro.hardware.egl=mali \
    ro.hardware.vulkan=universal7904 \
    ro.hardware.gralloc=exynos7904

PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.hardware=exynos7904 \
    ro.kernel.ebpf.supported=false

# VINTF metadata is registered in BoardConfig; stock vendor policy remains in
# the prebuilt vendor image and is checked separately against the new system.

PRODUCT_PACKAGES += framework_compatibility_matrix.gta3xlwifi.3.xml libaudiohal@4.0
PRODUCT_PACKAGES += gta3xlwifi_framework_legacy_sdk_manifest
PRODUCT_PACKAGES += gta3xlwifi_recovery_header_sync

# Select the device-specific iptables/qtaguid implementation.
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += ro.gta3xlwifi.legacy_networking=true

# Samsung's kernel supports usr/grp quota; project quota prevents /data mounting.
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += ro.gta3xlwifi.ext4_project_quota=false

# Parse service overrides after vendor init, and remove the inapplicable IMS flag.
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/init.gta3xlwifi.compat.rc:$(TARGET_COPY_OUT_PRODUCT)/etc/init/init.gta3xlwifi.compat.rc \
    $(LOCAL_PATH)/configs/permissions.gta3xlwifi.xml:$(TARGET_COPY_OUT_PRODUCT)/etc/permissions/permissions.gta3xlwifi.xml

# Legacy kernel has no blkio controller. The device I/O profiles retain the
# kernel's default I/O scheduling instead of attempting nonexistent groups.
