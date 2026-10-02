// SPDX-License-Identifier: Apache-2.0
#include "BoostWindow.h"
#include <gtest/gtest.h>
using namespace gta3xlwifi;

TEST(BoostWindow, DefaultTouchExpiresWithoutAnotherHint) {
    BoostWindow window;
    const Clock::time_point now{};
    window.interaction(0, now);
    EXPECT_EQ(545000, window.floors(now + Millis(399)).gpu);
    EXPECT_EQ(0, window.floors(now + Millis(400)).gpu);
    EXPECT_FALSE(window.nextDeadline(now + Millis(400)));
}
TEST(BoostWindow, ExcessiveDurationIsBoundedAndNegativeCancels) {
    BoostWindow window;
    const Clock::time_point now{};
    window.interaction(2147483647, now);
    EXPECT_NE(0, window.floors(now + Millis(1999)).big);
    EXPECT_EQ(0, window.floors(now + Millis(2000)).big);
    window.interaction(400, now);
    window.interaction(-1, now);
    EXPECT_EQ(0, window.floors(now).gpu);
}
TEST(BoostWindow, LaunchFallbackReturnsToOutstandingTouch) {
    BoostWindow window;
    const Clock::time_point now{};
    window.interaction(1000, now);
    window.launch(true, now);
    EXPECT_EQ(676000, window.floors(now).gpu);
    window.launch(false, now + Millis(100));
    EXPECT_EQ(545000, window.floors(now + Millis(100)).gpu);
    EXPECT_EQ(0, window.floors(now + Millis(1000)).gpu);
}
TEST(BoostWindow, ForgottenLaunchEndExpiresAndFreshTouchCanFollow) {
    BoostWindow window;
    const Clock::time_point now{};
    window.launch(true, now);
    EXPECT_EQ(0, window.floors(now + Millis(2000)).big);
    window.interaction(0, now + Millis(2001));
    EXPECT_EQ(1352000, window.floors(now + Millis(2001)).big);
}
TEST(BoostWindow, ScreenOffCancelsAndDoesNotResurrectOnWake) {
    BoostWindow window;
    const Clock::time_point now{};
    window.launch(true, now);
    window.interactive(false);
    window.interaction(1000, now);
    EXPECT_EQ(0, window.floors(now).gpu);
    window.interactive(true);
    EXPECT_EQ(0, window.floors(now).gpu);
}
TEST(BoostWindow, BatterySaverCancelsAndIgnoresNewRequests) {
    BoostWindow window;
    const Clock::time_point now{};
    window.interaction(1000, now);
    window.lowPower(true);
    window.launch(true, now);
    EXPECT_EQ(0, window.floors(now).little);
    window.lowPower(false);
    EXPECT_EQ(0, window.floors(now).gpu);
}
