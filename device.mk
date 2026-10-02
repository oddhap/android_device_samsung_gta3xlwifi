# SPDX-License-Identifier: Apache-2.0
LOCAL_PATH := device/samsung/gta3xlwifi
DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay
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
    android.hardware.health@2.1-impl.recovery \
    android.hardware.health@2.1-service.recovery

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/init.gta3xlwifi.power.rc:$(TARGET_COPY_OUT_SYSTEM_EXT)/etc/init/init.gta3xlwifi.power.rc \
    $(LOCAL_PATH)/rootdir/etc/fstab.exynos7904:$(TARGET_COPY_OUT_RAMDISK)/fstab.exynos7904 \
    $(LOCAL_PATH)/rootdir/init.exynos7904.rc:$(TARGET_COPY_OUT_RAMDISK)/init.exynos7904.rc \
    $(LOCAL_PATH)/rootdir/ueventd.exynos7904.rc:$(TARGET_COPY_OUT_RAMDISK)/ueventd.exynos7904.rc

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
