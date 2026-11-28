# SPDX-License-Identifier: Apache-2.0
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)
$(call inherit-product, vendor/lineage/config/common_full_tablet_wifionly.mk)
$(call inherit-product, device/samsung/gta3xlwifi/device.mk)

PRODUCT_NAME := lineage_gta3xlwifi
PRODUCT_DEVICE := gta3xlwifi
PRODUCT_BRAND := samsung
PRODUCT_MANUFACTURER := samsung
PRODUCT_MODEL := SM-T510
PRODUCT_CHARACTERISTICS := tablet
PRODUCT_SHIPPING_API_LEVEL := 28
# The 4.4 kernel has no userfaultfd, so ART runs the concurrent-copying GC and
# cannot execute code dexpreopted for CMC. The build cannot read the kernel
# version, and "default" then assumes CMC, so state it explicitly.
PRODUCT_ENABLE_UFFD_GC := false
PRODUCT_EXTRA_VNDK_VERSIONS := 30

# SD-card storage (issue 4). Adoptable volumes are F2FS, and F2FS on this 4.4
# kernel uses the generic fscrypt code, which accepts only AES-256-XTS with
# AES-256-CTS filenames; vold's pre-Q default (AES-256-HEH filenames) is
# rejected with EINVAL. The SD slot line is in the pinned Samsung vendor fstab
# without encryptable=userdata, so offer adoptable storage through the
# framework override (also applies to USB disks).
PRODUCT_SYSTEM_PROPERTIES += \
    ro.crypto.volume.options=aes-256-xts:aes-256-cts:v1 \
    persist.sys.adoptable=force_on

LINEAGE_BUILDTYPE := UNOFFICIAL
