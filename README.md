# SM-T510 native LineageOS 21 development port

This device product builds Android 14 for `gta3xlwifi` from pinned LineageOS 21
sources. Candidate 12 boots and survives an ordinary reboot with encrypted
file data and SELinux enforcing.
Offline images/package checks and actual-library GPU-memory/ordinary-RAM tests
pass. The prior system-server abort in AppProfiler GPU memory reporting is
corrected by the integration libmeminfo patch. This is a development port;
complete hardware and stability testing remains pending.

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

Candidate 7 also accepts the framework INVALID_UID sentinel in bulk firewall
updates without discarding real app rules. Restricted mode remains enforced.
A migrated browser block-all policy was repaired through the policy service;
other app policies remain intact. Candidate 7 device-input code revision is
38fe8f491e6ba959824cd6c11ef5965f1163b17f; this update changes documentation only.

## Wi-Fi-only and usage cleanup (2026-10-04)

The tested device build-input revision is
`2cd63e9d051ca181621fc20ddff6a4820f3b0713`; this follow-up changes documentation.
Framework capabilities describe zero SIM/modem hardware. A static TeleService
capability overlay disables both application enablement and persistence; the
matching integration platform patches are required. Init stops cpboot-daemon
before class main and stops MobiCore before shutdown unmount. Exact three type
files and OTG online are labeled for the existing health permission. I/O
profiles retain default scheduling where the kernel has no blkio controller.

Candidate 12 pairs this tree with distinct release identity in the integration
build wrapper. Android replaces stale package parse caches automatically. All
210 installed package/UID records survive update and normal reboot; encryption,
key availability, SELinux, signing and exact label checks pass. The final reboot
has no current-boot crash/ANR/native tombstone, and EFS unmount succeeds after
MobiCore stops. Previous user-authorized data formatting removed old data.
See the integration's `lineage-21/notes/usage-compat.md` and scoped reports.
Full hardware and long-term stability testing remain incomplete.
