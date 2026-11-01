# SM-T510 LineageOS 21 — ARM64 device tree

Native ARM64 Android14 with ARM32 app support, for Galaxy Tab A10.1 (2019),
Wi-Fi **SM-T510 / gta3xlwifi only**. Uses file-based encryption, SELinux Enforcing,
the existing ARM64 Linux4.4.302 DT2W kernel and a minimal CWA1/A305GT hybrid vendor.
The ROM installs boot, system, product and vendor; Google apps are optional.

The November1 update pins the reviewed FunctionFS USB/MTP vendor image. Its
configuration source and image-audit tools are in the integration repository.
The native power backend, double tap to wake, charger UI, both app ABIs and
Android14-only recovery compatibility guards are retained.

- [Download and installation](https://github.com/oddhap/android_build_samsung_gta3xlwifi/releases/tag/lineage-21.0-arm64-20261101)
- [Pinned sources and build tools](https://github.com/oddhap/android_build_samsung_gta3xlwifi/tree/lineage-21.0-arm64)
- [Kernel source](https://github.com/oddhap/android_kernel_samsung_gta3xlwifi/tree/lineage-21.0)

The exact MTP rc was physically tested on the October15 ROM. The complete new
ROM has not been test-installed; see the integration release validation report.
`abi-graphics` and `hardware-probe` are separate test modules and are not shipped
as ROM apps. Only the ARM64 ROM branch is maintained. New configuration is
Apache-2.0; upstream files retain their original notices.
