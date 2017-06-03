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

#ifndef MINDROID_INTENT_H_
#define MINDROID_INTENT_H_

#include "mindroid/content/ComponentName.h"
#include "mindroid/lang/Object.h"
#include "mindroid/os/Bundle.h"

namespace mindroid {

/**
 * An intent is an abstract description of an operation to be performed. It can be used with
 * {@link mindroid.content.Context#startService} or {@link mindroid.content.Context#bindService} to
 * communicate with a {@link mindroid.app.Service}.
 *
 * <p>
 * An Intent provides a facility for performing late runtime binding between the code in different
 * applications. Its most significant use is in the launching of services, where it can be thought
 * of as the glue between services. It is basically a passive data structure holding an abstract
 * description of an action to be performed.
 * </p>
 *
 */
class Intent :
        public Object {
public:
    /**
     * Create an empty intent.
     */
    Intent() = default;

    /**
     * Copy constructor.
     */
    Intent(const sp<Intent>& o);

    /**
     * Returns true if an extra value is associated with the given name.
     *
     * @param name the extra's name
     * @return true if the given extra is present.
     */
    bool hasExtra(const char* name) {
        return hasExtra(String::valueOf(name));
    }
    bool hasExtra(const sp<String>& name) {
        return mExtras != nullptr && mExtras->containsKey(name);
    }

    /**
     * Retrieve extended data from the intent.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, boolean)
     */
    bool getBooleanExtra(const char* name, bool defaultValue) {
        return getBooleanExtra(String::valueOf(name), defaultValue);
    }
    bool getBooleanExtra(const sp<String>& name, bool defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getBoolean(name, defaultValue);
    }

    /**
     * Retrieve extended data from the intent.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, byte)
     */
    uint8_t getByteExtra(const char* name, uint8_t defaultValue) {
        return getByteExtra(String::valueOf(name), defaultValue);
    }
    uint8_t getByteExtra(const sp<String>& name, uint8_t defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getByte(name, defaultValue);
    }

    /**
     * Retrieve extended data from the intent.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, short)
     */
    uint16_t getShortExtra(const char* name, uint16_t defaultValue) {
        return getShortExtra(String::valueOf(name), defaultValue);
    }
    uint16_t getShortExtra(const sp<String>& name, uint16_t defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getShort(name, defaultValue);
    }

    /**
     * Retrieve extended data from the intent.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, char)
     */
    char getCharExtra(const char* name, char defaultValue) {
        return getCharExtra(String::valueOf(name), defaultValue);
    }
    char getCharExtra(const sp<String>& name, char defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getChar(name, defaultValue);
    }

    /**
     * Retrieve extended data from the intent.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, int)
     */
    int32_t getIntExtra(const char* name, int32_t defaultValue) {
        return getIntExtra(String::valueOf(name), defaultValue);
    }
    int32_t getIntExtra(const sp<String>& name, int32_t defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getInt(name, defaultValue);
    }

    /**
     * Retrieve extended data from the intent.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, long)
     */
    int64_t getLongExtra(const char* name, int64_t defaultValue) {
        return getLongExtra(String::valueOf(name), defaultValue);
    }
    int64_t getLongExtra(const sp<String>& name, int64_t defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getLong(name, defaultValue);
    }

    /**
     * Retrieve extended data from the intent.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra(), or the default value if
     * no such item is present
     *
     * @see #putExtra(String, float)
     */
    float getFloatExtra(const char* name, float defaultValue) {
        return getFloatExtra(String::valueOf(name), defaultValue);
    }
    float getFloatExtra(const sp<String>& name, float defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getFloat(name, defaultValue);
    }

    /**
     * Retrieve extended data from the intent.
     *
     * @param name The name of the desired item.
     * @param defaultValue the value to be returned if no value of the desired type is stored with
     * the given name.
     *
     * @return the value of an item that previously added with putExtra() or the default value if
     * none was found.
     *
     * @see #putExtra(String, double)
     */
    double getDoubleExtra(const char* name, double defaultValue) {
        return getDoubleExtra(String::valueOf(name), defaultValue);
    }
    double getDoubleExtra(const sp<String>& name, double defaultValue) {
        return mExtras == nullptr ? defaultValue :
            mExtras->getDouble(name, defaultValue);
    }

    /**
     * Retrieve extended data from the intent.
     *
     * @param name The name of the desired item.
     *
     * @return the value of an item that previously added with putExtra() or null if no String value
     * was found.
     *
     * @see #putExtra(String, String)
     */
    sp<String> getStringExtra(const char* name) {
        return getStringExtra(String::valueOf(name));
    }
    sp<String> getStringExtra(const sp<String>& name) {
        return mExtras == nullptr ? nullptr : mExtras->getString(name);
    }

    /**
     * Retrieve extended data from the intent.
     *
     * @param name The name of the desired item.
     *
     * @return the value of an item that previously added with putExtra() or null if no
     * ArrayList<Integer> value was found.
     *
     * @see #putIntegerArrayListExtra(String, ArrayList)
     */
    sp<ArrayList<int32_t>> getIntegerArrayListExtra(const char* name) {
        return getIntegerArrayListExtra(String::valueOf(name));
    }
    sp<ArrayList<int32_t>> getIntegerArrayListExtra(const sp<String>& name) {
        return mExtras == nullptr ? nullptr : mExtras->getIntegerArrayList(name);
    }

    /**
     * Retrieve extended data from the intent.
     *
     * @param name The name of the desired item.
     *
     * @return the value of an item that previously added with putExtra() or null if no
     * ArrayList<String> value was found.
     *
     * @see #putStringArrayListExtra(String, ArrayList)
     */
    sp<ArrayList<sp<String>>> getStringArrayListExtra(const char* name) {
        return getStringArrayListExtra(String::valueOf(name));
    }
    sp<ArrayList<sp<String>>> getStringArrayListExtra(const sp<String>& name) {
        return mExtras == nullptr ? nullptr : mExtras->getStringArrayList(name);
    }

    /**
     * Retrieves a map of extended data from the intent.
     *
     * @return the map of all extras previously added with putExtra(), or null if none have been
     * added.
     */
    sp<Bundle> getExtras() {
        return (mExtras != nullptr)
                ? new Bundle(mExtras)
                : nullptr;
    }

    /**
     * Retrieve the concrete component associated with the intent. When receiving an intent, this is
     * the component that was found to best handle it (that is, yourself) and will always be
     * non-null; in all other cases it will be null unless explicitly set.
     *
     * @return The name of the application component to handle the intent.
     *
     * @see #setComponent
     */
    sp<ComponentName> getComponent() {
        return mComponent;
    }

    /**
     * Add extended data to the intent.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The boolean data value.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getBooleanExtra(String, boolean)
     */
    sp<Intent> putExtra(const char* name, bool value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Intent> putExtra(const sp<String>& name, bool value);

    /**
     * Add extended data to the intent.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The byte data value.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getByteExtra(String, byte)
     */
    sp<Intent> putExtra(const char* name, uint8_t value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Intent> putExtra(const sp<String>& name, uint8_t value);

    /**
     * Add extended data to the intent.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The char data value.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getCharExtra(String, char)
     */
    sp<Intent> putExtra(const char* name, char value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Intent> putExtra(const sp<String>& name, char value);

    /**
     * Add extended data to the intent.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The short data value.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getShortExtra(String, short)
     */
    sp<Intent> putExtra(const char* name, int16_t value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Intent> putExtra(const sp<String>& name, int16_t value);

    /**
     * Add extended data to the intent.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The integer data value.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getIntExtra(String, int)
     */
    sp<Intent> putExtra(const char* name, int32_t value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Intent> putExtra(const sp<String>& name, int32_t value);

    /**
     * Add extended data to the intent.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The long data value.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getLongExtra(String, long)
     */
    sp<Intent> putExtra(const char* name, int64_t value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Intent> putExtra(const sp<String>& name, int64_t value);

    /**
     * Add extended data to the intent.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The float data value.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getFloatExtra(String, float)
     */
    sp<Intent> putExtra(const char* name, float value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Intent> putExtra(const sp<String>& name, float value);

    /**
     * Add extended data to the intent.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The double data value.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getDoubleExtra(String, double)
     */
    sp<Intent> putExtra(const char* name, double value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Intent> putExtra(const sp<String>& name, double value);

    /**
     * Add extended data to the intent.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The String data value.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getStringExtra(String)
     */
    sp<Intent> putExtra(const char* name, const char* value) {
        return putExtra(String::valueOf(name), String::valueOf(value));
    }
    sp<Intent> putExtra(const char* name, const sp<String>& value) {
        return putExtra(String::valueOf(name), value);
    }
    sp<Intent> putExtra(const sp<String>& name, const sp<String>& value);

    /**
     * Add extended data to the intent.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The ArrayList<Integer> data value.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getIntegerArrayListExtra(String)
     */
    sp<Intent> putIntegerArrayListExtra(const char* name, sp<ArrayList<int32_t>> value) {
        return putIntegerArrayListExtra(String::valueOf(name), value);
    }
    sp<Intent> putIntegerArrayListExtra(const sp<String>& name, sp<ArrayList<int32_t>> value);

    /**
     * Add extended data to the intent.
     *
     * @param name The name of the extra data, with package prefix.
     * @param value The ArrayList<String> data value.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #putExtras
     * @see #removeExtra
     * @see #getStringArrayListExtra(String)
     */
    sp<Intent> putStringArrayListExtra(const char* name, const sp<ArrayList<sp<String>>>& value) {
        return putStringArrayListExtra(String::valueOf(name), value);
    }
    sp<Intent> putStringArrayListExtra(const sp<String>& name, const sp<ArrayList<sp<String>>>& value);

    /**
     * Add a set of extended data to the intent.
     *
     * @param extras The Bundle of extras to add to this intent.
     *
     * @see #putExtra
     * @see #removeExtra
     */
    sp<Intent> putExtras(const sp<Bundle>& extras);

    /**
     * Completely replace the extras in the Intent with the given Bundle of extras.
     *
     * @param extras The new set of extras in the Intent, or null to erase all extras.
     */
    sp<Intent> replaceExtras(const sp<Bundle>& extras) {
        mExtras = extras != nullptr ? new Bundle(extras) : nullptr;
        return this;
    }

    /**
     * Remove extended data from the intent.
     *
     * @see #putExtra
     */
    void removeExtra(const char* name) {
        removeExtra(String::valueOf(name));
    }
    void removeExtra(const sp<String>& name);

    /**
     * Explicitly set the component to handle the intent. If left with the default value of null,
     * the system will determine the appropriate class to use based on the other fields (action,
     * data, type, categories) in the Intent. If this class is defined, the specified class will
     * always be used regardless of the other fields. You should only set this value when you know
     * you absolutely want a specific class to be used; otherwise it is better to let the system
     * find the appropriate class so that you will respect the installed applications and user
     * preferences.
     *
     * @param component The name of the application component to handle the intent, or null to let
     * the system find one for you.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #setClassName(String, String)
     * @see #getComponent
     */
    sp<Intent> setComponent(const sp<ComponentName>& component) {
        mComponent = component;
        return this;
    }

    /**
     * Convenience for calling {@link #setComponent} with an explicit class name.
     *
     * @param packageContext A Context of the application package implementing this class.
     * @param className The name of a class inside of the application package that will be used as
     * the component for this Intent.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #setComponent
     * @see #setClass
     */
    sp<Intent> setClassName(const sp<Context>& packageContext, const sp<String>& className) {
        mComponent = new ComponentName(packageContext, className);
        return this;
    }

    /**
     * Convenience for calling {@link #setComponent} with an explicit application package name and
     * class name.
     *
     * @param packageName The name of the package implementing the desired component.
     * @param className The name of a class inside of the application package that will be used as
     * the component for this Intent.
     *
     * @return Returns the same Intent object, for chaining multiple calls into a single statement.
     *
     * @see #setComponent
     */
    sp<Intent> setClassName(const char* packageName, const char* className) {
        return setClassName(String::valueOf(packageName), String::valueOf(className));
    }
    sp<Intent> setClassName(const sp<String>& packageName, const sp<String>& className) {
        mComponent = new ComponentName(packageName, className);
        return this;
    }

private:
    sp<ComponentName> mComponent;
    sp<Bundle> mExtras;
};

} /* namespace mindroid */

#endif /* MINDROID_INTENT_H_ */
