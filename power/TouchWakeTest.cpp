// SPDX-License-Identifier: Apache-2.0
#include "TouchWake.h"
#include <cassert>
#include <utility>
#include <vector>

using gta3xlwifi::TouchControl;
using gta3xlwifi::TouchWake;
using Operation = std::pair<TouchControl, bool>;

int main() {
    std::vector<Operation> ops;
    int failAt = -1;
    TouchWake touch([&](TouchControl control, bool enabled) {
        ops.emplace_back(control, enabled);
        return static_cast<int>(ops.size()) != failAt;
    });
    assert(touch.doubleTap(true));
    assert((ops == std::vector<Operation>{{TouchControl::DoubleTap, true}}));
    ops.clear();
    assert(touch.interactive(false));
    assert((ops == std::vector<Operation>{{TouchControl::InputEnabled, false}}));
    ops.clear();
    assert(touch.doubleTap(false));
    assert((ops == std::vector<Operation>{{TouchControl::DoubleTap, false},
                                        {TouchControl::InputEnabled, true},
                                        {TouchControl::InputEnabled, false}}));
    ops.clear();
    assert(touch.doubleTap(true));
    assert((ops == std::vector<Operation>{{TouchControl::DoubleTap, true},
                                        {TouchControl::InputEnabled, true},
                                        {TouchControl::InputEnabled, false}}));
    ops.clear();
    failAt = 1;
    assert(!touch.doubleTap(false));
    assert(ops.size() == 1);  // Flag failure must not change power mode.
    ops.clear();
    failAt = 2;
    assert(!touch.doubleTap(false));
    assert(ops.size() == 3 && ops.back() == Operation(TouchControl::InputEnabled, false));
    ops.clear();
    failAt = 3;
    assert(!touch.doubleTap(true));
    ops.clear();
    failAt = -1;
    assert(touch.interactive(true));
    assert(touch.doubleTap(false));
    assert((ops == std::vector<Operation>{{TouchControl::InputEnabled, true},
                                        {TouchControl::DoubleTap, false}}));
    ops.clear();
    failAt = 1;
    assert(!touch.interactive(false));
    ops.clear();
    failAt = -1;
    assert(touch.interactive(true));  // Failed control remains retryable.
    return 0;
}
