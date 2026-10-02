// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <algorithm>
#include <chrono>
#include <optional>

namespace gta3xlwifi {
using Clock = std::chrono::steady_clock;
using Millis = std::chrono::milliseconds;

struct Floors {
    int little = 0;
    int big = 0;
    int gpu = 0;
    bool operator==(const Floors& other) const {
        return little == other.little && big == other.big && gpu == other.gpu;
    }
    bool operator!=(const Floors& other) const { return !(*this == other); }
};

// The thermal/governor maximum remains in control; these are SKU-valid floors.
class BoostWindow {
  public:
    void interaction(int durationMs, Clock::time_point now) {
        if (durationMs < 0) {
            mInteraction.reset();
        } else if (mInteractive && !mLowPower) {
            const int duration = durationMs == 0 ? 400 : std::clamp(durationMs, 1, 2000);
            mInteraction = now + Millis(duration);
        }
    }
    void launch(bool enabled, Clock::time_point now) {
        if (!enabled) mLaunch.reset();
        else if (mInteractive && !mLowPower) mLaunch = now + Millis(2000);
    }
    void interactive(bool enabled) {
        mInteractive = enabled;
        if (!enabled) clear();
    }
    void lowPower(bool enabled) {
        mLowPower = enabled;
        if (enabled) clear();
    }
    Floors floors(Clock::time_point now) const {
        if (!mInteractive || mLowPower) return {};
        if (mLaunch && *mLaunch > now) return {1248000, 1560000, 676000};
        if (mInteraction && *mInteraction > now) return {1248000, 1352000, 545000};
        return {};
    }
    std::optional<Clock::time_point> nextDeadline(Clock::time_point now) const {
        if (!mInteractive || mLowPower) return {};
        std::optional<Clock::time_point> result;
        for (const auto& deadline : {mInteraction, mLaunch}) {
            if (deadline && *deadline > now && (!result || *deadline < *result)) result = deadline;
        }
        return result;
    }
  private:
    void clear() { mInteraction.reset(); mLaunch.reset(); }
    bool mInteractive = true;
    bool mLowPower = false;
    std::optional<Clock::time_point> mInteraction;
    std::optional<Clock::time_point> mLaunch;
};
}  // namespace gta3xlwifi
