# Samsung Galaxy Tab A 10.1 (2019), SM-T510 / gta3xlwifi

Native LineageOS 19.1 (Android 12L) device tree, without GApps. ARM32 userspace,
ARM64 kernel/Binder, stock T510XXU5CWA1 vendor. This is a development port.
Android boots; Wi-Fi, browser video/audio, both cameras and rotation have been
reported working on one tablet. Long-term stability, Bluetooth, microSD and
credential-protected encryption/decryption need further testing.

Contains the tested SystemUI/device integration, legacy qtaguid configuration,
bounded CPU/GPU interaction and launch boosts, enforcing system SELinux rules,
power-control permissions, configfs recovery USB and Samsung image packaging.
Do not repartition or relock the bootloader with these custom images installed.

Clone this repository into `device/samsung/gta3xlwifi` in the pinned Lineage tree.
Generated `prebuilt/` and stock compatibility-policy files are staged by the
integration tools, rather than checked into this device repository.

Build instructions, pinned manifests, platform patches and validation tools:
https://github.com/oddhap/android_build_samsung_gta3xlwifi
Kernel: https://github.com/oddhap/android_kernel_samsung_gta3xlwifi
Vendor inputs: https://github.com/oddhap/android_vendor_samsung_gta3xlwifi
Separate TWRP device: https://github.com/oddhap/android_device_samsung_gta3xlwifi_twrp

New device code is Apache-2.0 (see per-file notices). Stock-derived metadata and
upstream-derived code retain their original copyright and licensing terms.
