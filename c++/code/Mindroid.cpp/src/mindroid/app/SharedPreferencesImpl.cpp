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

#include "mindroid/app/SharedPreferencesImpl.h"
#include "mindroid/io/File.h"
#include "mindroid/util/Log.h"
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;

namespace mindroid {

const char* const SharedPreferencesImpl::TAG = "SharedPreferences";
const char* const SharedPreferencesImpl::UTF_8 = "UTF-8";
const char* const SharedPreferencesImpl::MAP_TAG = "map";
const char* const SharedPreferencesImpl::BOOLEAN_TAG = "boolean";
const char* const SharedPreferencesImpl::INT_TAG = "int";
const char* const SharedPreferencesImpl::LONG_TAG = "long";
const char* const SharedPreferencesImpl::FLOAT_TAG = "float";
const char* const SharedPreferencesImpl::STRING_TAG = "string";
const char* const SharedPreferencesImpl::STRING_SET_TAG = "set";
const char* const SharedPreferencesImpl::NAME_ATTR = "name";
const char* const SharedPreferencesImpl::VALUE_ATTR = "value";

SharedPreferencesImpl::SharedPreferencesImpl(const sp<File>& file, int32_t mode) :
        mFile(file),
        mLock(new ReentrantLock()),
        mListeners(new HashMap<sp<SharedPreferences::OnSharedPreferenceChangeListener>, sp<IOnSharedPreferenceChangeListener>>()) {
    mBackupFile = makeBackupFile(file);
    AutoLock autoLock(mLock);
    loadSharedPrefs();
}

sp<HashMap<sp<String>, sp<Variant>>> SharedPreferencesImpl::getAll() {
    AutoLock autoLock(mLock);
    return new HashMap<sp<String>, sp<Variant>>(mMap);
}

sp<String> SharedPreferencesImpl::getString(const sp<String>& key, const sp<String>& defValue) {
    AutoLock autoLock(mLock);
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isString()) {
            return value->getString();
        } else {
            return defValue;
        }
    }
    return defValue;
}

sp<Set<sp<String>>> SharedPreferencesImpl::getStringSet(const sp<String>& key, const sp<Set<sp<String>>>& defValues) {
    AutoLock autoLock(mLock);
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isStringSet()) {
            return new Set<sp<String>>(value->getStringSet());
        } else {
            return defValues;
        }
    }
    return defValues;
}

int32_t SharedPreferencesImpl::getInt(const sp<String>& key, int32_t defValue) {
    AutoLock autoLock(mLock);
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isInt()) {
            return value->getInt();
        } else {
            return defValue;
        }
    }
    return defValue;
}

int64_t SharedPreferencesImpl::getLong(const sp<String>& key, int64_t defValue) {
    AutoLock autoLock(mLock);
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isLong()) {
            return value->getLong();
        } else {
            return defValue;
        }
    }
    return defValue;
}

float SharedPreferencesImpl::getFloat(const sp<String>& key, float defValue) {
    AutoLock autoLock(mLock);
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isFloat()) {
            return value->getFloat();
        } else {
            return defValue;
        }
    }
    return defValue;
}

bool SharedPreferencesImpl::getBoolean(const sp<String>& key, bool defValue) {
    AutoLock autoLock(mLock);
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isBool()) {
            return value->getBool();
        } else {
            return defValue;
        }
    }
    return defValue;
}

bool SharedPreferencesImpl::EditorImpl::commit() {
    AutoLock autoLock(mSharedPreferences->mLock);
    bool modifications = false;

    bool hasListeners = mSharedPreferences->mListeners->size() > 0;
    sp<ArrayList<sp<String>>> modifiedKeys = nullptr;
    if (hasListeners) {
        modifiedKeys = new ArrayList<sp<String>>();
    }

    if (mClearMap) {
        if (!mSharedPreferences->mMap->isEmpty()) {
            modifications = true;
            mSharedPreferences->mMap->clear();
        }
        mClearMap = false;
    }

    auto itr = mModifications->iterator();
    while (itr.hasNext()) {
        auto pair = itr.next();
        sp<String> k = pair.getKey();
        sp<Variant> v = pair.getValue();
        // "null" is the magic value for entry removal.
        if (v == nullptr || v->isNull()) {
            if (!mSharedPreferences->mMap->containsKey(k)) {
                continue;
            }
            mSharedPreferences->mMap->remove(k);
        } else {
            if (mSharedPreferences->mMap->containsKey(k)) {
                sp<Variant> existingValue = mSharedPreferences->mMap->get(k);
                if (existingValue != nullptr && existingValue->equals(v)) {
                    continue;
                }
            }
            mSharedPreferences->mMap->put(k, v);
        }

        modifications = true;
        if (hasListeners) {
            modifiedKeys->add(k);
        }
    }

    mModifications->clear();

    bool result = false;
    if (modifications) {
        result = mSharedPreferences->storeSharedPrefs();
        if (result && hasListeners) {
            mSharedPreferences->notifySharedPreferenceChangeListeners(modifiedKeys);
        }
    }

    return result;
}

void SharedPreferencesImpl::registerOnSharedPreferenceChangeListener(const sp<SharedPreferences::OnSharedPreferenceChangeListener>& listener) {
    if (listener != nullptr) {
        AutoLock autoLock(mLock);
        if (!mListeners->containsKey(listener)) {
            sp<OnSharedPreferenceChangeListenerWrapper> wrapper = new OnSharedPreferenceChangeListenerWrapper(listener);
            mListeners->put(listener, binder::OnSharedPreferenceChangeListener::Stub::asInterface(wrapper->asBinder()));
        }
    }
}

void SharedPreferencesImpl::unregisterOnSharedPreferenceChangeListener(const sp<SharedPreferences::OnSharedPreferenceChangeListener>& listener) {
    if (listener != nullptr) {
        AutoLock autoLock(mLock);
        auto wrapper = mListeners->remove(listener);
        if (wrapper != nullptr) {
            wrapper->dispose();
        }
    }
}

void SharedPreferencesImpl::loadSharedPrefs() {
    if (mMap != nullptr) {
        return;
    }

    if (mBackupFile->exists()) {
        mFile->remove();
        mBackupFile->renameTo(mFile);
    }

    sp<HashMap<sp<String>, sp<Variant>>> map = nullptr;
    if (mFile->canRead()) {
        map = readMap(mFile);
    }
    if (map != nullptr) {
        mMap = map;
    } else {
        mMap = new HashMap<sp<String>, sp<Variant>>();
        Log::e(TAG, "Cannot read shared preferences: %s", mFile->getPath()->c_str());
    }
}

bool SharedPreferencesImpl::storeSharedPrefs() {
    if (mFile->exists()) {
        if (!mBackupFile->exists()) {
            if (!mFile->renameTo(mBackupFile)) {
                Log::e(TAG, "Cannot rename file %s to backup file %s",  mFile->getPath()->c_str(), mBackupFile->getPath()->c_str());
                return false;
            }
        } else {
            mFile->remove();
        }
    }

    if (writeMap(mFile, mMap)) {
        mBackupFile->remove();
        return true;
    } else {
        Log::e(TAG, "Cannot write shared preferences: %s", mFile->getPath()->c_str());
        if (mFile->exists()) {
            if (!mFile->remove()) {
                Log::e(TAG, "Cannot clean up partially-written file %s", mFile->getPath()->c_str());
            }
        }
        return false;
    }
}

void SharedPreferencesImpl::notifySharedPreferenceChangeListeners(const sp<ArrayList<sp<String>>>& keys) {
    auto itr = keys->iterator();
    while (itr.hasNext()) {
        sp<String> key = itr.next();
        auto listenerItr = mListeners->iterator();
        while (listenerItr.hasNext()) {
            auto entry = listenerItr.next();
            sp<IOnSharedPreferenceChangeListener> listener = entry.getValue();
            try {
                listener->onSharedPreferenceChanged(this, key);
            } catch (const RemoteException& e) {
                listenerItr.remove();
            }
        }
    }
}

sp<File> SharedPreferencesImpl::makeBackupFile(const sp<File>& prefsFile) {
    return new File(prefsFile->getPath()->append(".bak"));
}

sp<HashMap<sp<String>, sp<Variant>>> SharedPreferencesImpl::readMap(const sp<File>& file) {
    XMLDocument doc;
    if (doc.LoadFile(file->getPath()->c_str()) != XML_SUCCESS) {
        return nullptr;
    }

    XMLElement* rootNode = doc.FirstChildElement(MAP_TAG);
    if (rootNode != nullptr) {
        sp<HashMap<sp<String>, sp<Variant>>> map = new HashMap<sp<String>, sp<Variant>>();
        XMLElement* element;
        for(element = rootNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
            if (XMLUtil::StringEqual(BOOLEAN_TAG, element->Name())) {
                parseBoolean(map, element);
            } else if (XMLUtil::StringEqual(INT_TAG, element->Name())) {
                parseInt(map, element);
            } else if (XMLUtil::StringEqual(LONG_TAG, element->Name())) {
                parseLong(map, element);
            } else if (XMLUtil::StringEqual(FLOAT_TAG, element->Name())) {
                parseFloat(map, element);
            } else if (XMLUtil::StringEqual(STRING_TAG, element->Name())) {
                parseString(map, element);
            } else if (XMLUtil::StringEqual(STRING_SET_TAG, element->Name())) {
                parseStringSet(map, element);
            }
        }
        return map;
    }
    return nullptr;
}

void SharedPreferencesImpl::parseBoolean(sp<HashMap<sp<String>, sp<Variant>>>& map, const XMLElement* element) {
    const XMLAttribute* name = element->FindAttribute(NAME_ATTR);
    const XMLAttribute* value = element->FindAttribute(VALUE_ATTR);
    if (name != nullptr && value != nullptr) {
        map->put(String::valueOf(name->Value()), new Variant(value->BoolValue()));
    }
}

void SharedPreferencesImpl::parseInt(sp<HashMap<sp<String>, sp<Variant>>>& map, const XMLElement* element) {
    const XMLAttribute* name = element->FindAttribute(NAME_ATTR);
    const XMLAttribute* value = element->FindAttribute(VALUE_ATTR);
    if (name != nullptr && value != nullptr) {
        map->put(String::valueOf(name->Value()), new Variant((int32_t) value->IntValue()));
    }
}

void SharedPreferencesImpl::parseLong(sp<HashMap<sp<String>, sp<Variant>>>& map, const XMLElement* element) {
    const XMLAttribute* name = element->FindAttribute(NAME_ATTR);
    const XMLAttribute* value = element->FindAttribute(VALUE_ATTR);
    if (name != nullptr && value != nullptr) {
        map->put(String::valueOf(name->Value()), new Variant((int64_t) value->IntValue()));
    }
}

void SharedPreferencesImpl::parseFloat(sp<HashMap<sp<String>, sp<Variant>>>& map, const XMLElement* element) {
    const XMLAttribute* name = element->FindAttribute(NAME_ATTR);
    const XMLAttribute* value = element->FindAttribute(VALUE_ATTR);
    if (name != nullptr && value != nullptr) {
        map->put(String::valueOf(name->Value()), new Variant((float) value->FloatValue()));
    }
}

void SharedPreferencesImpl::parseString(sp<HashMap<sp<String>, sp<Variant>>>& map, const XMLElement* element) {
    const XMLAttribute* name = element->FindAttribute(NAME_ATTR);
    const XMLNode* value = element->FirstChild();
    if (name != nullptr) {
        sp<String> str = value == nullptr ? String::valueOf("") : String::valueOf(value->Value());
        map->put(String::valueOf(name->Value()), new Variant(str));
    }
}

void SharedPreferencesImpl::parseStringSet(sp<HashMap<sp<String>, sp<Variant>>>& map, const XMLElement* element) {
    const XMLAttribute* name = element->FindAttribute(NAME_ATTR);
    if (name != nullptr) {
        sp<Set<sp<String>>> values = new Set<sp<String>>();
        const XMLElement* childElement;
        for(childElement = element->FirstChildElement(); childElement != nullptr; childElement = childElement->NextSiblingElement()) {
            if (XMLUtil::StringEqual(STRING_TAG, childElement->Name())) {
                const XMLNode* value = childElement->FirstChild();
                if (value != nullptr) {
                    values->add(String::valueOf(value->Value()));
                }
            }
        }
        map->put(String::valueOf(name->Value()), new Variant(values));
    }
}

bool SharedPreferencesImpl::writeMap(const sp<File>& file, const sp<HashMap<sp<String>, sp<Variant>>>& map) {
    XMLDocument doc;
    doc.InsertFirstChild(doc.NewDeclaration());
    XMLElement* rootNode = doc.NewElement(MAP_TAG);
    doc.InsertEndChild(rootNode);

    auto itr = mMap->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        XMLElement* e = writeValue(doc, entry.getKey(), entry.getValue());
        if (e != nullptr) {
            rootNode->InsertEndChild(e);
        }
    }

    return (doc.SaveFile(file->getPath()->c_str()) == XML_SUCCESS);
}

XMLElement* SharedPreferencesImpl::writeValue(XMLDocument& doc, const sp<String>& name, const sp<Variant>& value) {
    if (value != nullptr) {
        XMLElement* element = nullptr;
        switch (value->getType()) {
        case Variant::Bool: {
            element = doc.NewElement(BOOLEAN_TAG);
            element->SetAttribute(NAME_ATTR, name->c_str());
            element->SetAttribute(VALUE_ATTR, value->getBool() ? "true" : "false");
            break;
        }
        case Variant::Int: {
            element = doc.NewElement(INT_TAG);
            element->SetAttribute(NAME_ATTR, name->c_str());
            element->SetAttribute(VALUE_ATTR, (int) value->getInt());
            break;
        }
        case Variant::Long: {
            element = doc.NewElement(LONG_TAG);
            element->SetAttribute(NAME_ATTR, name->c_str());
            element->SetAttribute(VALUE_ATTR, (int) value->getLong());
            break;
        }
        case Variant::Float: {
            element = doc.NewElement(FLOAT_TAG);
            element->SetAttribute(NAME_ATTR, name->c_str());
            element->SetAttribute(VALUE_ATTR, (float) value->getFloat());
            break;
        }
        case Variant::String: {
            element = doc.NewElement(STRING_TAG);
            element->SetAttribute(NAME_ATTR, name->c_str());
            XMLText* text = doc.NewText(value->getString()->c_str());
            element->InsertFirstChild(text);
            break;
        }
        case Variant::StringSet: {
            element = doc.NewElement(STRING_SET_TAG);
            element->SetAttribute(NAME_ATTR, name->c_str());

            sp<Set<sp<String>>> values = value->getStringSet();
            auto itr = values->iterator();
            while (itr.hasNext()) {
                sp<String> s = itr.next();
                XMLElement* childElement = doc.NewElement(STRING_TAG);
                XMLText* text = doc.NewText(s->c_str());
                childElement->InsertFirstChild(text);
                element->InsertEndChild(childElement);
            }
            break;
        }
        default:
            element = nullptr;
        }

        return element;
    }
    return nullptr;
}

} /* namespace mindroid */
