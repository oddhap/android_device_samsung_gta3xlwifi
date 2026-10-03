// SPDX-License-Identifier: Apache-2.0
#include <android-base/properties.h>
#include <meminfo/sysmeminfo.h>
#include <cstdio>
#include <unordered_map>
#include <unistd.h>

int main() {
    if (android::base::GetBoolProperty("ro.kernel.ebpf.supported", true)) {
        std::fputs("Expected this device's unsupported BPF backend\n", stderr);
        return 1;
    }
    uint64_t value = 123456;
    std::unordered_map<uint32_t, uint64_t> processes;
    if (android::meminfo::ReadGpuTotalUsageKb(&value) ||
        android::meminfo::ReadProcessGpuUsageKb(getpid(), 0, &value) ||
        android::meminfo::ReadPerProcessGpuMem(&processes)) {
        std::fputs("Unavailable GPU memory was reported as a measurement\n", stderr);
        return 1;
    }
    android::meminfo::SysMemInfo memory;
    if (!memory.ReadMemInfo() || memory.mem_total_kb() == 0) {
        std::fputs("Ordinary RAM accounting failed\n", stderr);
        return 1;
    }
    std::puts("PASS: all three GPU memory queries unavailable without abort; ordinary RAM accounting works");
    return 0;
}
