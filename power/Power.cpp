// SPDX-License-Identifier: Apache-2.0
#define LOG_TAG "Gta3xlwifiPower"
#include "BoostWindow.h"
#include "Gta3xlwifiPower.h"
#include "TouchWake.h"
#include <aidl/android/hardware/power/Mode.h>
#include <android-base/file.h>
#include <android-base/properties.h>
#include <android-base/unique_fd.h>
#include <android-base/strings.h>
#include <log/log.h>
#include <fcntl.h>
#include <unistd.h>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

using android::base::unique_fd;
using aidl::android::hardware::power::Mode;
namespace gta3xlwifi {
class Power {
  public:
    Power()
        : mLittle(open("/dev/gta3xlwifi_cluster0_min", O_RDWR | O_CLOEXEC)),
          mBig(open("/dev/gta3xlwifi_cluster1_min", O_RDWR | O_CLOEXEC)),
          mGpu(open("/sys/devices/platform/11500000.mali/gta3xlwifi_min_lock", O_WRONLY | O_CLOEXEC)) {
        mReady = mLittle.ok() && mBig.ok() && mGpu.ok();
        if (!mReady) {
            ALOGE("Power boost disabled: cannot open controls (%d/%d/%d), errno=%d",
                  mLittle.ok(), mBig.ok(), mGpu.ok(), errno);
            return;
        }
        // Constructed by nativeInit inside system_server, never in the zygote.
        apply({});
        if (!mReady) return;
        mWorker = std::thread([this] { run(); });
        ALOGI("Bounded CPU/GPU boost ready: touch=400ms launch<=2000ms");
    }
    void interaction(int32_t durationMs) {
        std::lock_guard<std::mutex> guard(mMutex);
        if (!mReady) return;
        ++mInteractions;
        mWindow.interaction(durationMs, Clock::now());
        update("interaction");
    }
    bool mode(int32_t value, bool enabled) {
        std::lock_guard<std::mutex> guard(mMutex);
        // Gesture/touch control is independent of CPU/GPU boost readiness.
        if (static_cast<Mode>(value) == Mode::DOUBLE_TAP_TO_WAKE)
            return mTouch.doubleTap(enabled);
        const bool touchHandled = static_cast<Mode>(value) == Mode::INTERACTIVE &&
                                  mTouch.interactive(enabled);
        if (!mReady) return touchHandled;
        switch (static_cast<Mode>(value)) {
            case Mode::INTERACTIVE: mWindow.interactive(enabled); break;
            case Mode::LOW_POWER: mWindow.lowPower(enabled); break;
            case Mode::LAUNCH:
                if (enabled) ++mLaunches;
                mWindow.launch(enabled, Clock::now());
                break;
            default: return false;
        }
        // Clear before the caller can suspend, not after a worker wakeup.
        update("mode");
        return mReady || touchHandled;
    }
  private:
    static bool writeTouchControl(TouchControl control, bool enabled) {
        const char* path = control == TouchControl::DoubleTap
                ? "/sys/class/sec/tsp/double_tap_to_wake"
                : "/sys/class/sec/tsp/input_enabled";
        const char value = enabled ? '1' : '0';
        unique_fd fd(open(path, O_WRONLY | O_CLOEXEC));
        if (!fd.ok() || TEMP_FAILURE_RETRY(write(fd.get(), &value, 1)) != 1) {
            ALOGE("Touch power control failed: %s=%c errno=%d", path, value, errno);
            return false;
        }
        std::string readback;
        if (!android::base::ReadFileToString(path, &readback) ||
                android::base::Trim(readback) != std::string(1, value)) {
            ALOGE("Touch power readback failed: %s=%c", path, value);
            return false;
        }
        return true;
    }
    static bool writeCpu(int fd, int frequency) {
        const int32_t value = frequency;
        return TEMP_FAILURE_RETRY(write(fd, &value, sizeof(value))) == static_cast<ssize_t>(sizeof(value));
    }
    bool writeGpu(int frequency) {
        const std::string value = std::to_string(frequency);
        return lseek(mGpu.get(), 0, SEEK_SET) == 0 &&
               TEMP_FAILURE_RETRY(write(mGpu.get(), value.data(), value.size())) == static_cast<ssize_t>(value.size());
    }
    void apply(const Floors& floors) {
        const bool little = writeCpu(mLittle.get(), floors.little);
        const bool big = writeCpu(mBig.get(), floors.big);
        const bool gpu = writeGpu(floors.gpu);
        if (!little || !big || !gpu) {
            ALOGE("Power control write failed (%d/%d/%d), disabling boost", little, big, gpu);
            // Closing a QoS fd removes only our request, including on process death.
            mLittle.reset();
            mBig.reset();
            writeGpu(0);
            mReady = false;
            mApplied = {};
        } else {
            mApplied = floors;
        }
    }
    void update(const char* reason) {
        const auto now = Clock::now();
        const Floors floors = mWindow.floors(now);
        if (floors != mApplied) apply(floors);
        if (android::base::GetBoolProperty("debug.gta3xlwifi.power_log", false)) {
            ALOGI("%s: requests=%llu/%llu floor_khz=%d/%d/%d ready=%d",
                  reason, static_cast<unsigned long long>(mInteractions),
                  static_cast<unsigned long long>(mLaunches), mApplied.little,
                  mApplied.big, mApplied.gpu, mReady);
        }
        mChanged.notify_all();
    }
    void run() {
        std::unique_lock<std::mutex> guard(mMutex);
        while (mReady) {
            const auto now = Clock::now();
            const Floors floors = mWindow.floors(now);
            if (floors != mApplied) {
                apply(floors);
                if (android::base::GetBoolProperty("debug.gta3xlwifi.power_log", false))
                    ALOGI("expiry: floor_khz=%d/%d/%d ready=%d", mApplied.little,
                          mApplied.big, mApplied.gpu, mReady);
            }
            if (!mReady) break;
            const auto deadline = mWindow.nextDeadline(now);
            if (deadline) mChanged.wait_until(guard, *deadline);
            else mChanged.wait(guard);
        }
    }
    unique_fd mLittle, mBig, mGpu;
    TouchWake mTouch{writeTouchControl};
    bool mReady = false;
    BoostWindow mWindow;
    Floors mApplied;
    std::mutex mMutex;
    std::condition_variable mChanged;
    std::thread mWorker;
    uint64_t mInteractions = 0, mLaunches = 0;
};
// Lifetime is that of system_server. Never destroy a joinable worker at exit;
// kernel fd cleanup and init's zygote restart trigger release the constraints.
static Power& power() { static Power* instance = new Power(); return *instance; }
void initPower() { (void)power(); }
void interactionBoost(int32_t durationMs) { power().interaction(durationMs); }
bool powerMode(int32_t mode, bool enabled) { return power().mode(mode, enabled); }
}  // namespace gta3xlwifi
