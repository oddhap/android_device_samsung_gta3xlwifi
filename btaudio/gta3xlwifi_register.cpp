// SPDX-License-Identifier: Apache-2.0
// Registers the AOSP HIDL 2.0 Bluetooth audio provider factory in the audio HAL
// process that loads this module, so provider and stream share one session.
#define LOG_TAG "gta3xlwifi_btaudio"
#include <log/log.h>
#include <utils/Log.h>
#include <mutex>
#include "BluetoothAudioProvidersFactory.h"

using ::android::sp;
using ::android::hardware::bluetooth::audio::V2_0::IBluetoothAudioProvidersFactory;
using ::android::hardware::bluetooth::audio::V2_0::implementation::BluetoothAudioProvidersFactory;

// VNDK v30 libfmq exports only check(bool); current FMQ headers also call this
// overload. Identical to system/libfmq/FmqInternal.cpp.
#include <android-base/logging.h>
namespace android::hardware::details {
void check(bool exp, const char* message) {
  CHECK(exp) << message;
}
// Android 14 FMQ headers also call this helper, absent from VNDK v30.
// Keep the exact security-event logging from system/libfmq/FmqInternal.cpp.
void errorWriteLog(int tag, const char* info) {
  android_errorWriteLog(tag, info);
}
}  // namespace android::hardware::details

extern "C" void gta3xlwifi_register_bt_audio_provider() {
  static std::once_flag once;
  static sp<IBluetoothAudioProvidersFactory> factory;
  std::call_once(once, [] {
    factory = new BluetoothAudioProvidersFactory();
    const ::android::status_t status = factory->registerAsService("default");
    ALOGI("IBluetoothAudioProvidersFactory@2.0/default registration: %d", status);
  });
}
