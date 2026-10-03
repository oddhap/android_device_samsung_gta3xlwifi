# SM-T510 native LineageOS 21 development port

This device product builds Android 14 for `gta3xlwifi` from pinned LineageOS 21
sources. It remains a development port: a full ROM and offline image, layout,
ZIP, VINTF and runtime-policy checks pass. Candidate 6 boots Android 14 with
encrypted file data and SELinux enforcing. Bluetooth, IPv4/DNS, process-group
lifecycle and bounded touch/app-launch power tests pass. Complete physical
function and stability tests remain pending. The working Android 12L build provides
hardware/configuration provenance, not Android 14 validation.

Android userspace remains ARM32 to match Samsung's graphics and HAL libraries.
The ARM64 Samsung kernel and CWA1 vendor image are retained. No GApps, GSI,
A/B layout or dynamic partition conversion is included.

The integration tools under `lineage-21/tools` stage verified kernel/vendor
artifacts, apply a bounded native CPU/GPU boost, and implement device-only
iptables/qtaguid networking for the old kernel. The platform source remains
current; compatibility adaptations do not replace projects with old forks.

Boot retains Samsung's physical system-as-root layout and kernel-only boot
image, using the existing unlocked bootloader. Recovery has a separate ramdisk.
SELinux remains enforcing and generated platform policy tests pass. The actual
stock vendor CIL links using Android init's runtime flags. A separate strict
cross-version neverallow check fails on old Samsung policy, as it does on the
Android 12 baseline. This is an explicit legacy vendor limitation, not a full
security-equivalence claim. No permissive domain or build-wide neverallow bypass
has been added. Credential/recovery and complete hardware tests remain pending.

Build on the Ubuntu VM with `lunch lineage_gta3xlwifi-ap2a-userdebug`.
Use the integration manifest, patches and verification tools for reproducibility.
The optional packet probe is a test executable and must not ship in the ROM.

The first hardware boot reached SELinux setup, then failed because Android 14
requires cgroup v2 process tracking. A second candidate mounts real cpuacct v1
groups and uses bounded signaling/cleanup in the integration's system/core
patch. Device task profiles use supported CPU controller directories. Cached
app v2 freezing is unsupported. ARM32 BoringSSL self-test triggers are kept in
a regular init file, preserving upstream services and failure handling.
The second boot exposed an ownership change on the read-only /acct mountpoint
before the v1 mount. A third integration candidate prepares the mountpoint
without ownership changes and assigns ownership only after mounting. Hardware
boot and runtime verification remain pending.

Third boot passes process-group/APEX/BoringSSL startup, then returns to recovery
because duplicate mounts suppress FBE keyring creation. The fourth candidate
keeps root fstab for first stage and delegates cache/EFS/userdata mounts to the
stock vendor init script once. Encryption and runtime checks remain pending.

The fifth candidate completes Android 14 boot with encrypted data, enforcing
SELinux and running audio/GPU services. Bluetooth exposes zero RT budget in
new CPU scheduling subgroups. The sixth candidate places scheduling in CPU
root (original finite RT budget) and uses real cpuset performance affinity.
It also labels the exact legacy Exynos vsync property and grants composer
read access. Physical performance, camera/audio and Bluetooth pairing tests
are still required before a stability claim.

Candidate 6 physical boot confirms automatic Bluetooth ON with zero crashes,
CPU root placement, root RT budget 950000/1000000 us and foreground RT budget 0.
The repeated vsync property read denials are absent. Real touch and launch
boost peaks, expiry, screen-off/wake and battery-saver cancellation pass; test
battery overrides and diagnostic logging are restored. Both Samsung miscpower
control descriptors open successfully. The tested build device-input revision
is e3a925e24228d903683ccccb45661b82f4ef7fdd; this update changes documentation only.
