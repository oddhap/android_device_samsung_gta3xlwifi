# SM-T510 LineageOS 21 — ARM64 beta device tree

Native ARM64 userspace with ARM32 app support; Galaxy Tab A 10.1 (2019), Wi-Fi
**SM-T510 / gta3xlwifi only**. Android 14, no GApps, FBE and SELinux Enforcing.

This branch uses the existing ARM64 Linux 4.4.302 kernel and a minimal hybrid
vendor: CWA1 Samsung HAL/TEE services plus selected A305GT ARM64 graphics clients.
The beta ZIP installs vendor as well as boot/system/product.

- [Download and installation/rollback guide](https://github.com/oddhap/android_build_samsung_gta3xlwifi/releases/tag/lineage-21.0-arm64-beta-20261007)
- [Pinned platform patches, donor inputs and build tools](https://github.com/oddhap/android_build_samsung_gta3xlwifi/tree/lineage-21.0-arm64)
- [Kernel](https://github.com/oddhap/android_kernel_samsung_gta3xlwifi/tree/lineage-21.0)

`abi-graphics` and `hardware-probe` are separate test modules; they are not
installed in the released ROM. The October 14 candidate uses the latest fetched
LineageOS 21.0 source snapshot, with framework security patch 2026-10-01; the
Samsung vendor/kernel remain older. The previous ARM32 branch is retained.

The candidate adds the standard Display setting for double tap to wake. It
requires the matching FTS driver update: `KEY_WAKEUP`, a boolean
`double_tap_to_wake` sysfs control, and touch suspend/resume via the native power
backend. The setting, native backend, sysfs ownership and narrow SELinux labels
must be shipped together with that kernel. Host tests pass; physical validation
of the new ROM is still pending. The linked October 7 release is unchanged.

New configuration is Apache-2.0; upstream files retain their original notices.
