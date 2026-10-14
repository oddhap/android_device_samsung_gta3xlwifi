// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <functional>
#include <utility>

namespace gta3xlwifi {
enum class TouchControl { DoubleTap, InputEnabled };

// Calls are serialized by the power backend's existing mutex. The FTS close
// callback selects powerdown or gesture mode from the double-tap flag.
class TouchWake {
  public:
    using Writer = std::function<bool(TouchControl, bool)>;
    explicit TouchWake(Writer writer) : mWrite(std::move(writer)) {}
    bool interactive(bool enabled) {
        mInteractive = enabled;
        return mWrite(TouchControl::InputEnabled, enabled);
    }
    bool doubleTap(bool enabled) {
        if (!mWrite(TouchControl::DoubleTap, enabled)) return false;
        if (mInteractive) return true;
        // A preference may change while asleep. Reopen/close to apply the new
        // flag even when the controller was already powered down.
        const bool opened = mWrite(TouchControl::InputEnabled, true);
        const bool closed = mWrite(TouchControl::InputEnabled, false);
        return opened && closed;
    }
  private:
    Writer mWrite;
    bool mInteractive = true;
};
}  // namespace gta3xlwifi
