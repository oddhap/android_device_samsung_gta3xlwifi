# SM-T510 LineageOS 21 — ARM64 beta device tree

Native ARM64 userspace with ARM32 app support; Galaxy Tab A 10.1 (2019), Wi-Fi
**SM-T510 / gta3xlwifi only**. Android 14, no GApps, FBE and SELinux Enforcing.

This branch uses the existing ARM64 Linux 4.4.302 kernel and a minimal hybrid
vendor: CWA1 Samsung HAL/TEE services plus selected A305GT ARM64 graphics clients.
The beta ZIP installs vendor as well as boot/system/product.

- [Download and installation/rollback guide](https://github.com/oddhap/android_build_samsung_gta3xlwifi/releases/tag/lineage-21.0-arm64-beta-20261014)
- [Pinned platform patches, donor inputs and build tools](https://github.com/oddhap/android_build_samsung_gta3xlwifi/tree/lineage-21.0-arm64)
- [Kernel](https://github.com/oddhap/android_kernel_samsung_gta3xlwifi/tree/lineage-21.0)

`abi-graphics` and `hardware-probe` are separate test modules; they are not
installed in the released ROM. The October 15 beta uses the LineageOS 21.0 snapshot fetched again October 15
(no revision changes since October 14), with framework security patch 2026-10-01; the
Samsung vendor/kernel remain older. The previous ARM32 branch is retained.

The beta adds the standard Display setting for double tap to wake. It
requires the matching FTS driver update: `KEY_WAKEUP`, a boolean
`double_tap_to_wake` sysfs control, and touch suspend/resume via the native power
backend. The setting, native backend, sysfs ownership and narrow SELinux labels
must be shipped together with that kernel. Host tests and physical double-tap
wake validation pass on the signed October 14 ROM. ARM32/ARM64 Mali graphics and basic hardware regression checks pass.
See the integration repository for the precise scope and remaining beta limits.

The October 15 follow-up explicitly packages the native ARM64 charger and
starts it through `init.gta3xlwifi.charger.rc`, in the charger class only.
Powered-off animation/percentage, normal automatic blanking and Power button
behavior have been physically verified. The Samsung health HAL, charging
driver, DT2W kernel and native wake code are retained; no SELinux bypass is added.

New configuration is Apache-2.0; upstream files retain their original notices.
