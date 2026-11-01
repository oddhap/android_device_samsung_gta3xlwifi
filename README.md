# SM-T510 — LineageOS 24 device bring-up

LineageOS 24.0 / Android 17 development branch for the Samsung Galaxy Tab A
10.1 (2019), **SM-T510 / gta3xlwifi only**, native ARM64 with ARM32 app support.
This is an unfinished source-built device port, not an installation-ready ROM.

The retained baseline is the working LineageOS 21 ARM64 port: Samsung Linux
4.4.302, physical non-A/B partitions, Samsung Android 11 HAL/TEE services and a
minimal A305GT ARM64 graphics donor layer. The selected vendor image includes
the November 1 LineageOS 21 FunctionFS MTP fix (SHA-256
`a847a527931d61e5702b8a025210c84eb88a37ef1e60df93b6393a16dae9183a`); its Android 14 transfer tests do not validate Android 17 USB.
Kernel/DTBO and the reviewed vendor inputs
remain hash-bound; isolated kernel experiments are not promoted into prebuilts.

The 24 branch carries the product partition defaults, forced prebuilt kernel,
optional memory cgroup declaration and the official `aosp_current` lunch release
alias. Its complete platform patches, source locks, setup, audits and current
limitations are in the [LineageOS 24 integration directory](https://github.com/oddhap/android_build_samsung_gta3xlwifi/tree/lineage-24.0-arm64/lineage-24.0).

Strict actual-vendor SELinux compatibility, kernel policycaps, complete legacy
networking, Android 17 cgroup/zygote/runtime behavior, both-ABI HAL/APEX/graphics,
Samsung TEE/FBE, recovery/OTA and hardware testing remain open. Preserve enforcing
policy, neverallows and the Android-14-only installation/recovery guards until
compatibility is proven. Android 14 GApps/recovery test results do not validate
Android 17. Do not flash diagnostic images or replace the working TWRP.

The [existing LineageOS 21 ARM64 release](https://github.com/oddhap/android_build_samsung_gta3xlwifi/releases/tag/lineage-21.0-arm64-20261015)
remains the rollback reference. Verified backups and separate owner approval are
required before a concrete installation experiment.

New configuration is Apache-2.0; upstream files retain their original notices.
