/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2013 Daniel Himmelein
 * Copyright (C) 2016 E.S.R.Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mindroid/content/Context.h"
#include "mindroid/lang/String.h"

namespace mindroid {

const sp<String> Context::SERVICE_MANAGER = String::valueOf("serviceManager");
const sp<String> Context::PACKAGE_MANAGER = String::valueOf("packageManager");
const sp<String> Context::ALARM_MANAGER = String::valueOf("alarmManager");
const sp<String> Context::POWER_MANAGER = String::valueOf("powerManager");
const sp<String> Context::LOCATION_SERVICE = String::valueOf("locationService");
const sp<String> Context::AUDIO_SERVICE = String::valueOf("audioService");
const sp<String> Context::TELEPHONY_SERVICE = String::valueOf("telephonyService");
const sp<String> Context::MEDIA_PLAYER_SERVICE = String::valueOf("mediaPlayerService");
const sp<String> Context::MESSAGE_BROKER = String::valueOf("messageBroker");
const sp<String> Context::VOICE_SERVICE = String::valueOf("voiceService");

} /* namespace mindroid */
