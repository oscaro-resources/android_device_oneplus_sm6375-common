/*
 * Copyright (C) 2026 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "CameraProviderExtension.h"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/parseint.h>
#include <android-base/strings.h>

#include <string>

using ::android::base::ParseInt;
using ::android::base::ReadFileToString;
using ::android::base::Trim;
using ::android::base::WriteStringToFile;

namespace {

constexpr char kTorchBrightnessNode[] =
        "/sys/devices/platform/soc/5c1c000.qcom,cci1/"
        "5c1c000.qcom,cci1:qcom,camera-flash@0/torch_brightness";
constexpr int32_t kTorchDefaultStrength = 60;
constexpr int32_t kTorchMaxStrength = 127;

bool setTorchBrightness(int32_t strength) {
    const std::string value = std::to_string(strength);
    if (!WriteStringToFile(value, kTorchBrightnessNode, true)) {
        PLOG(ERROR) << "Unable to write torch brightness " << strength << " to "
                    << kTorchBrightnessNode;
        return false;
    }

    LOG(INFO) << "Torch brightness set to " << strength;
    return true;
}

int32_t getTorchBrightness() {
    std::string value;
    int32_t strength = 0;
    if (ReadFileToString(kTorchBrightnessNode, &value)) {
        if (ParseInt(Trim(value), &strength) && strength > 0) {
            return strength;
        }
    } else {
        PLOG(ERROR) << "Unable to read torch brightness node " << kTorchBrightnessNode;
    }

    return kTorchDefaultStrength;
}

}  // namespace

bool supportsTorchStrengthControlExt() {
    return true;
}

bool supportsSetTorchModeExt() {
    return false;
}

int32_t getTorchDefaultStrengthLevelExt() {
    return kTorchDefaultStrength;
}

int32_t getTorchMaxStrengthLevelExt() {
    return kTorchMaxStrength;
}

int32_t getTorchStrengthLevelExt() {
    return getTorchBrightness();
}

void setTorchStrengthLevelExt(int32_t torchStrength, bool enabled) {
    if (!enabled) {
        setTorchBrightness(-1);
        return;
    }

    if (torchStrength > 0 && torchStrength <= kTorchMaxStrength) {
        setTorchBrightness(torchStrength);
    }
}

void setTorchModeExt(bool enabled) {
    int32_t strength = getTorchDefaultStrengthLevelExt();
    setTorchStrengthLevelExt(enabled ? strength : 0, enabled);
}
