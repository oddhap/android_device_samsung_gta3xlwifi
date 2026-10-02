// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <cstdint>
namespace gta3xlwifi {
void initPower();
void interactionBoost(int32_t durationMs);
// Returns true only when this device backend handles the mode successfully.
bool powerMode(int32_t mode, bool enabled);
}
