/*
 * Copyright (C) 2010 The Android Open Source Project
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

#ifndef MINDROID_SHAREDPREFERENCESIMPL_H_
#define MINDROID_SHAREDPREFERENCESIMPL_H_

#include "mindroid/app/IOnSharedPreferenceChangeListener.h"
#include "mindroid/content/SharedPreferences.h"
#include "mindroid/util/ArrayList.h"
#include "mindroid/util/HashMap.h"
#include "mindroid/util/Variant.h"

namespace tinyxml2 {
class XMLDocument;
class XMLElement;
}

namespace mindroid {

class File;

class SharedPreferencesImpl :
        public SharedPreferences {
public:
    SharedPreferencesImpl(const sp<File>& file, int32_t mode);

    virtual sp<HashMap<sp<String>, sp<Variant>>> getAll();
    virtual sp<String> getString(const sp<String>& key, const sp<String>& defValue);
    virtual sp<Set<sp<String>>> getStringSet(const sp<String>& key, const sp<Set<sp<String>>>& defValues);
    virtual int32_t getInt(const sp<String>& key, int32_t defValue);
    virtual int64_t getLong(const sp<String>& key, int64_t defValue);
    virtual float getFloat(const sp<String>& key, float defValue);
    virtual bool getBoolean(const sp<String>& key, bool defValue);

    virtual bool contains(const sp<String>& key) {
        AutoLock autoLock(mLock);
        return mMap->containsKey(key);
    }

    virtual sp<Editor> edit() {
        return new EditorImpl(this);
    }

    class EditorImpl :
            public Editor {
    public:
        EditorImpl(sp<SharedPreferencesImpl> sharedPreferences) :
                mSharedPreferences(sharedPreferences),
                mClearMap(false) {
            mModifications = new HashMap<sp<String>, sp<Variant>>();
        }

        virtual sp<Editor> putString(const sp<String>& key, const sp<String>& value) {
            AutoLock autoLock(mSharedPreferences->mLock);
            mModifications->put(key, (value == nullptr) ? nullptr : new Variant(value));
            return this;
        }

        virtual sp<Editor> putStringSet(const sp<String>& key, const sp<Set<sp<String>>>& values) {
            AutoLock autoLock(mSharedPreferences->mLock);
            mModifications->put(key, (values == nullptr) ? nullptr : new Variant(object_cast<Set<sp<String>>>(new Set<sp<String>>(values))));
            return this;
        }

        virtual sp<Editor> putInt(const sp<String>& key, int32_t value) {
            AutoLock autoLock(mSharedPreferences->mLock);
            mModifications->put(key, new Variant(value));
            return this;
        }

        virtual sp<Editor> putLong(const sp<String>& key, int64_t value) {
            AutoLock autoLock(mSharedPreferences->mLock);
            mModifications->put(key, new Variant(value));
            return this;
        }

        virtual sp<Editor> putFloat(const sp<String>& key, float value) {
            AutoLock autoLock(mSharedPreferences->mLock);
            mModifications->put(key, new Variant(value));
            return this;
        }

        virtual sp<Editor> putBoolean(const sp<String>& key, bool value) {
            AutoLock autoLock(mSharedPreferences->mLock);
            mModifications->put(key, new Variant(value));
            return this;
        }

        virtual sp<Editor> remove(const sp<String>& key) {
            AutoLock autoLock(mSharedPreferences->mLock);
            mModifications->put(key, nullptr);
            return this;
        }

        virtual sp<Editor> clear() {
            AutoLock autoLock(mSharedPreferences->mLock);
            mClearMap = true;
            return this;
        }

        virtual void apply() {
            commit();
        }

        virtual bool commit();

    private:
        sp<SharedPreferencesImpl> mSharedPreferences;
        sp<HashMap<sp<String>, sp<Variant>>> mModifications;
        bool mClearMap;
    };


    void registerOnSharedPreferenceChangeListener(const sp<SharedPreferences::OnSharedPreferenceChangeListener>& listener);
    void unregisterOnSharedPreferenceChangeListener(const sp<SharedPreferences::OnSharedPreferenceChangeListener>& listener);

private:
    static const char* const TAG;
    static const char* const UTF_8;
    static const char* const MAP_TAG;
    static const char* const BOOLEAN_TAG;
    static const char* const INT_TAG;
    static const char* const LONG_TAG;
    static const char* const FLOAT_TAG;
    static const char* const STRING_TAG;
    static const char* const STRING_SET_TAG;
    static const char* const NAME_ATTR;
    static const char* const VALUE_ATTR;

    class OnSharedPreferenceChangeListenerWrapper :
            public binder::OnSharedPreferenceChangeListener::Stub {
    public:
        OnSharedPreferenceChangeListenerWrapper(const sp<OnSharedPreferenceChangeListener>& listener) {
            mListener = listener;
        }

        void onSharedPreferenceChanged(const sp<SharedPreferences>& sharedPreferences, const sp<String>& key) {
            mListener->onSharedPreferenceChanged(sharedPreferences, key);
        }

        void dispose() {
            Binder::dispose();
        }

    private:
        sp<OnSharedPreferenceChangeListener> mListener;
    };

    void loadSharedPrefs();
    bool storeSharedPrefs();

    void notifySharedPreferenceChangeListeners(const sp<ArrayList<sp<String>>>& keys);

    sp<HashMap<sp<String>, sp<Variant>>> readMap(const sp<File>& file);
    void parseBoolean(sp<HashMap<sp<String>, sp<Variant>>>& map, const tinyxml2::XMLElement* element);
    void parseInt(sp<HashMap<sp<String>, sp<Variant>>>& map, const tinyxml2::XMLElement* element);
    void parseLong(sp<HashMap<sp<String>, sp<Variant>>>& map, const tinyxml2::XMLElement* element);
    void parseFloat(sp<HashMap<sp<String>, sp<Variant>>>& map, const tinyxml2::XMLElement* element);
    void parseString(sp<HashMap<sp<String>, sp<Variant>>>& map, const tinyxml2::XMLElement* element);
    void parseStringSet(sp<HashMap<sp<String>, sp<Variant>>>& map, const tinyxml2::XMLElement* element);

    bool writeMap(const sp<File>& file, const sp<HashMap<sp<String>, sp<Variant>>>& map);
    tinyxml2::XMLElement* writeValue(tinyxml2::XMLDocument& doc, const sp<String>& name, const sp<Variant>& value);

    sp<File> makeBackupFile(const sp<File>& prefsFile);

    sp<File> mFile;
    sp<File> mBackupFile;
    sp<ReentrantLock> mLock;
    sp<HashMap<sp<String>, sp<Variant>>> mMap;
    sp<HashMap<sp<SharedPreferences::OnSharedPreferenceChangeListener>, sp<IOnSharedPreferenceChangeListener>>> mListeners;
};

} /* namespace mindroid */

#endif /* MINDROID_SHAREDPREFERENCESIMPL_H_ */
