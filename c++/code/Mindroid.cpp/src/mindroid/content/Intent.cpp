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

#include "mindroid/content/Intent.h"

namespace mindroid {

Intent::Intent(const sp<Intent>& o) {
    mComponent = o->mComponent;
    if (o->mExtras != nullptr) {
        mExtras = new Bundle(o->mExtras);
    }
}

sp<Intent> Intent::putExtra(const sp<String>& name, bool value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putBoolean(name, value);
    return this;
}

sp<Intent> Intent::putExtra(const sp<String>& name, uint8_t value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putByte(name, value);
    return this;
}

sp<Intent> Intent::putExtra(const sp<String>& name, char value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putChar(name, value);
    return this;
}

sp<Intent> Intent::putExtra(const sp<String>& name, int16_t value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putShort(name, value);
    return this;
}

sp<Intent> Intent::putExtra(const sp<String>& name, int32_t value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putInt(name, value);
    return this;
}

sp<Intent> Intent::putExtra(const sp<String>& name, int64_t value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putLong(name, value);
    return this;
}

sp<Intent> Intent::putExtra(const sp<String>& name, float value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putFloat(name, value);
    return this;
}

sp<Intent> Intent::putExtra(const sp<String>& name, double value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putDouble(name, value);
    return this;
}

sp<Intent> Intent::putExtra(const sp<String>& name, const sp<String>& value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putString(name, value);
    return this;
}

sp<Intent> Intent::putIntegerArrayListExtra(const sp<String>& name, sp<ArrayList<int32_t>> value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putIntegerArrayList(name, value);
    return this;
}

sp<Intent> Intent::putStringArrayListExtra(const sp<String>& name, const sp<ArrayList<sp<String>>>& value) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putStringArrayList(name, value);
    return this;
}

sp<Intent> Intent::putExtras(const sp<Bundle>& extras) {
    if (mExtras == nullptr) {
        mExtras = new Bundle();
    }
    mExtras->putAll(extras);
    return this;
}

void Intent::removeExtra(const sp<String>& name) {
    if (mExtras != nullptr) {
        mExtras->remove(name);
        if (mExtras->size() == 0) {
            mExtras = nullptr;
        }
    }
}

} /* namespace mindroid */
