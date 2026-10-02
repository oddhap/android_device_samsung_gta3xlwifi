# SPDX-License-Identifier: Apache-2.0
# The verified stock boot and recovery images carry this Samsung trailer.
# This marker is not a Samsung signature and does not enable locked boot.
ifeq ($(BOARD_AVB_ENABLE),true)
$(error Review Samsung image packaging before enabling AVB)
endif

# Samsung mounts the physical system partition as root and adds skip_initramfs.
# Preserve the tested kernel-only boot image; recovery keeps its own ramdisk.
GTA3XLWIFI_BOOTIMAGE_ARGS = $(filter-out --ramdisk $(INSTALLED_RAMDISK_TARGET),$(INTERNAL_BOOTIMAGE_ARGS))

# Include the source-built kernel explicitly in the custom packaging rule.
$(INSTALLED_BOOTIMAGE_TARGET): $(INSTALLED_KERNEL_TARGET) $(TARGET_PREBUILT_KERNEL) $(MKBOOTIMG) $(INTERNAL_BOOTIMAGE_FILES) $(BOOTIMAGE_EXTRA_DEPS) $(BOARD_CUSTOM_BOOTIMG_MK)
	$(call pretty,"Target Samsung boot image: $@")
	$(MKBOOTIMG) --kernel $(call bootimage-to-kernel,$@) $(GTA3XLWIFI_BOOTIMAGE_ARGS) $(INTERNAL_MKBOOTIMG_VERSION_ARGS) $(BOARD_MKBOOTIMG_ARGS) --output $@
	$(hide) python3 $(DEVICE_PATH)/check-ramdisk-size.py $@
	$(hide) printf 'SEANDROIDENFORCE' >> $@
	$(call assert-max-image-size,$@,$(BOARD_BOOTIMAGE_PARTITION_SIZE))

$(INSTALLED_RECOVERYIMAGE_TARGET): $(INSTALLED_KERNEL_TARGET) $(TARGET_PREBUILT_KERNEL) $(recoveryimage-deps) $(RECOVERYIMAGE_EXTRA_DEPS) $(BOARD_CUSTOM_BOOTIMG_MK)
	$(call build-recoveryimage-target,$@)
	$(hide) python3 $(DEVICE_PATH)/check-ramdisk-size.py $@
	$(hide) printf 'SEANDROIDENFORCE' >> $@
	$(call assert-max-image-size,$@,$(BOARD_RECOVERYIMAGE_PARTITION_SIZE))
