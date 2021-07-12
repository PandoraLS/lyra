// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lyra_config.h"

#include <climits>

#include "absl/strings/str_cat.h"

namespace chromemedia {
namespace codec {

// The Lyra version is |kVersionMajor|.|kVersionMinor|.|kVersionMicro|
// The version is not used internally, but clients may use it to configure
// behavior, such as checking for version bumps that break the bitstream.
// The major version should be bumped whenever the bitstream breaks.
const int kVersionMajor = 0;
// |kVersionMinor| needs to be increased every time a new version requires a
// simultaneous change in code and weights or if the bit stream is modified. The
// |identifier| field needs to be set in lyra_config.textproto to match this.
const int kVersionMinor = 0;
// The micro version is for other things like a release of bugfixes.
const int kVersionMicro = 1;

const int kNumFeatures = 160;
const int kNumExpectedOutputFeatures = 160;
const int kNumChannels = 1;
const int kFrameRate = 25 ; // origin is  25
const int kFrameOverlapFactor = 2;
const int kNumFramesPerPacket = 1;

// TODO(b/133794927): Calculation of kPacketSize will be determined by future
// considerations.
// LINT.IfChange
const int kPacketSize = 15;
// LINT.ThenChange(
// lyra_components.cc,
// )

const int kBitrate = kPacketSize * CHAR_BIT * kFrameRate * kNumChannels;

const std::string& GetVersionString() {
  static const std::string kVersionString = [] {
    return absl::StrCat(kVersionMajor, ".", kVersionMinor, ".", kVersionMicro);
  }();
  return kVersionString;
}

}  // namespace codec
}  // namespace chromemedia
