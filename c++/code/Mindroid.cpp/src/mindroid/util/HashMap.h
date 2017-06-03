/*
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

#ifndef MINDROID_HASHMAP_H_
#define MINDROID_HASHMAP_H_

#include "mindroid/lang/Object.h"
#include "mindroid/util/Set.h"
#include "mindroid/util/Assert.h"
#include <unordered_map>

namespace mindroid {

/*
 * HashMap template for standard K and V types.
 * Hashing uses std::hash method and comparison uses std::equal_to method.
 */
template<typename K, typename V>
class HashMap :
        public Object {
public:
    HashMap() :
            mMap() {
    }

    HashMap(size_t initialCapacity) :
            mMap(initialCapacity) {
    }

    HashMap(const sp<HashMap<K, V>>& map) :
            mMap() {
        putAll(map);
    }

    virtual ~HashMap() {
        clear();
    }

    void clear() {
        mMap.clear();
    }

    bool containsKey(const K& key) const {
        auto itr = mMap.find(key);
        return itr != mMap.end();
    }

    bool containsValue(const V& value) const {
        for (auto itr = mMap.begin(); itr != mMap.end(); ++itr) {
            if (itr->second == value) {
                return true;
            }
        }
        return false;
    }

    V get(const K& key) {
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            return itr->second;
        } else {
            return mMap[key];
        }
    }

    bool isEmpty() const {
        return mMap.empty();
    }

    sp<Set<K>> keySet() {
        if (isEmpty()) {
            return nullptr;
        }

        sp<Set<K>> keys = new Set<K>();
        auto itr = iterator();
        while (itr.hasNext()) {
            auto pair = itr.next();
            keys->add(pair.getKey());
        }
        return keys;
    }

    V put(const K& key, const V& value) {
        V oldValue = mMap[key];
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            oldValue = itr->second;
        }

        mMap[key] = value;
        return oldValue;
    }

    void putAll(const sp<HashMap>& map) {
        auto itr = map->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            put(entry.getKey(), entry.getValue());
        }
    }

    V remove(const K& key) {
        V oldValue = mMap[key];
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            oldValue = itr->second;
        }

        mMap.erase(key);
        return oldValue;
    }

    size_t size() const {
        return mMap.size();
    }

    class Iterator;

    class Entry {
    public:
        K getKey() const {
            return mKey;
        }

        V getValue() const {
            return mValue;
        }

        V setValue(const V& value) {
            V oldValue = mValue;
            mValue = value;
            return oldValue;
        }

    private:
        Entry(const K& key, V& value) : mKey(key), mValue(value) { }

        const K& mKey;
        V& mValue;

        friend class Iterator;
    };

    class Iterator {
    public:
        Iterator(std::unordered_map<K, V>& map) :
                mForwardIterator(false),
                mMap(&map),
                mIterator(mMap->begin()) {
        }

        ~Iterator() { }

        Iterator& operator=(const Iterator& iterator) {
            mForwardIterator = iterator.mForwardIterator;
            mMap = iterator.mMap;
            mIterator = iterator.mIterator;
            return *this;
        }

        bool hasNext() const {
            if (!mForwardIterator) {
                return mIterator != mMap->end();
            } else {
                typename std::unordered_map<K, V>::iterator itr = mIterator;
                return ++itr != mMap->end();
            }
        }

        Entry next() {
            if (!mForwardIterator) {
                mForwardIterator = true;
            } else {
                ++mIterator;
            }
            Entry entry(mIterator->first, mIterator->second);
            return entry;
        }

        bool remove() {
            if (mForwardIterator) {
                mIterator = mMap->erase(mIterator);
                mForwardIterator = false;
                return true;
            }
            return false;
        }

    private:
        bool mForwardIterator;
        std::unordered_map<K, V>* mMap;
        typename std::unordered_map<K, V>::iterator mIterator;
    };

    inline Iterator iterator() {
        return Iterator(mMap);
    }

private:
    std::unordered_map<K, V> mMap;
};


/*
 * HashMap template specialization if K is a standard type and V inherits from mindroid::Object.
 * Hashing uses std::hash method and comparison uses std::equal_to method.
 */
template<typename K, typename V>
class HashMap<K, sp<V>> :
        public Object {
public:
    HashMap() :
            mMap() {
    }

    HashMap(size_t initialCapacity) :
            mMap(initialCapacity) {
    }

    HashMap(const sp<HashMap<K, sp<V>>>& map) :
            mMap() {
        putAll(map);
    }

    virtual ~HashMap() {
        clear();
    }

    void clear() {
        mMap.clear();
    }

    bool containsKey(const K& key) const {
        auto itr = mMap.find(key);
        return itr != mMap.end();
    }

    bool containsValue(const sp<V>& value) const {
        if (value != nullptr) {
            for (auto itr = mMap.begin(); itr != mMap.end(); ++itr) {
                if (itr->second->equals(value)) {
                    return true;
                }
            }
        }
        return false;
    }

    sp<V> get(const K& key) {
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            return itr->second;
        } else {
            return nullptr;
        }
    }

    bool isEmpty() const {
        return mMap.empty();
    }

    sp<Set<K>> keySet() {
        if (isEmpty()) {
            return nullptr;
        }

        sp<Set<K>> keys = new Set<K>();
        auto itr = iterator();
        while (itr.hasNext()) {
            auto pair = itr.next();
            keys->add(pair.getKey());
        }
        return keys;
    }

    sp<V> put(const K& key, const sp<V>& value) {
        sp<V> oldValue = nullptr;
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            oldValue = itr->second;
        }

        mMap[key] = value;
        return oldValue;
    }

    void putAll(const sp<HashMap>& map) {
        auto itr = map->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            put(entry.getKey(), entry.getValue());
        }
    }

    sp<V> remove(const K& key) {
        sp<V> oldValue = nullptr;
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            oldValue = itr->second;
        }

        mMap.erase(key);
        return oldValue;
    }

    size_t size() const {
        return mMap.size();
    }

    class Iterator;

    class Entry {
    public:
        K getKey() const {
            return mKey;
        }

        sp<V> getValue() const {
            return mValue;
        }

        sp<V> setValue(const sp<V>& value) {
            sp<V> oldValue = mValue;
            mValue = value;
            return oldValue;
        }

    private:
        Entry(const K& key, const sp<V>& value) : mKey(key), mValue(value) { }

        const K& mKey;
        sp<V> mValue;

        friend class Iterator;
    };

    class Iterator {
    public:
        Iterator(std::unordered_map<K, sp<V>>& map) :
                mForwardIterator(false),
                mMap(&map),
                mIterator(mMap->begin()) {
        }

        ~Iterator() { }

        Iterator& operator=(const Iterator& iterator) {
            mForwardIterator = iterator.mForwardIterator;
            mMap = iterator.mMap;
            mIterator = iterator.mIterator;
            return *this;
        }

        bool hasNext() const {
            if (!mForwardIterator) {
                return mIterator != mMap->end();
            } else {
                typename std::unordered_map<K, sp<V>>::iterator itr = mIterator;
                return ++itr != mMap->end();
            }
        }

        Entry next() {
            if (!mForwardIterator) {
                mForwardIterator = true;
            } else {
                ++mIterator;
            }
            Entry entry(mIterator->first, mIterator->second);
            return entry;
        }

        bool remove() {
            if (mForwardIterator) {
                mIterator = mMap->erase(mIterator);
                mForwardIterator = false;
                return true;
            }
            return false;
        }

    private:
        bool mForwardIterator;
        std::unordered_map<K, sp<V>>* mMap;
        typename std::unordered_map<K, sp<V>>::iterator mIterator;
    };

    inline Iterator iterator() {
        return Iterator(mMap);
    }

private:
    std::unordered_map<K, sp<V>> mMap;
};


/*
 * HashMap template specialization if K inherits from mindroid::Object and V is a standard type.
 * Object hashing uses mindroid::Object::hashCode method and object comparison uses mindroid::Object::equals method.
 */
template<typename K, typename V>
class HashMap<sp<K>, V> :
        public Object {
public:
    HashMap() :
            mMap() {
    }

    HashMap(size_t initialCapacity) :
            mMap(initialCapacity) {
    }

    HashMap(const sp<HashMap<sp<K>, V>>& map) :
            mMap() {
        putAll(map);
    }

    virtual ~HashMap() {
        clear();
    }

    void clear() {
        mMap.clear();
    }

    bool containsKey(const sp<K>& key) const {
        if (key != nullptr) {
            auto itr = mMap.find(key);
            return itr != mMap.end();
        }
        return false;
    }

    bool containsValue(const V& value) const {
        for (auto itr = mMap.begin(); itr != mMap.end(); ++itr) {
            if (itr->second == value) {
                return true;
            }
        }
        return false;
    }

    V get(const sp<K>& key) {
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            return itr->second;
        } else {
            return mMap[key];
        }
    }

    bool isEmpty() const {
        return mMap.empty();
    }

    sp<Set<sp<K>>> keySet() {
        if (isEmpty()) {
            return nullptr;
        }

        sp<Set<sp<K>>> keys = new Set<sp<K>>();
        auto itr = iterator();
        while (itr.hasNext()) {
            auto pair = itr.next();
            keys->add(pair.getKey());
        }
        return keys;
    }

    V put(const sp<K>& key, const V& value) {
        V oldValue = mMap[key];
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            oldValue = itr->second;
        }

        mMap[key] = value;
        return oldValue;
    }

    void putAll(const sp<HashMap>& map) {
        auto itr = map->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            put(entry.getKey(), entry.getValue());
        }
    }

    V remove(const sp<K>& key) {
        V oldValue = mMap[key];
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            oldValue = itr->second;
        }

        mMap.erase(key);
        return oldValue;
    }

    size_t size() const {
        return mMap.size();
    }

    class Iterator;

    class Entry {
    public:
        sp<K> getKey() const {
            return mKey;
        }

        V getValue() const {
            return mValue;
        }

        V setValue(const V& value) {
            V oldValue = mValue;
            mValue = value;
            return oldValue;
        }

    private:
        Entry(const sp<K>& key, V& value) : mKey(key), mValue(value) { }

        sp<K> mKey;
        V& mValue;

        friend class Iterator;
    };

    class Iterator {
    public:
        Iterator(std::unordered_map<sp<K>, V, Hasher<sp<K>>, EqualityComparator<sp<K>>>& map) :
                mForwardIterator(false),
                mMap(&map),
                mIterator(mMap->begin()) {
        }

        ~Iterator() { }

        Iterator& operator=(const Iterator& iterator) {
            mForwardIterator = iterator.mForwardIterator;
            mMap = iterator.mMap;
            mIterator = iterator.mIterator;
            return *this;
        }

        bool hasNext() const {
            if (!mForwardIterator) {
                return mIterator != mMap->end();
            } else {
                typename std::unordered_map<sp<K>, V, Hasher<sp<K>>, EqualityComparator<sp<K>>>::iterator itr = mIterator;
                return ++itr != mMap->end();
            }
        }

        Entry next() {
            if (!mForwardIterator) {
                mForwardIterator = true;
            } else {
                ++mIterator;
            }
            Entry entry(mIterator->first, mIterator->second);
            return entry;
        }

        bool remove() {
            if (mForwardIterator) {
                mIterator = mMap->erase(mIterator);
                mForwardIterator = false;
                return true;
            }
            return false;
        }

    private:
        bool mForwardIterator;
        std::unordered_map<sp<K>, V, Hasher<sp<K>>, EqualityComparator<sp<K>>>* mMap;
        typename std::unordered_map<sp<K>, V, Hasher<sp<K>>, EqualityComparator<sp<K>>>::iterator mIterator;
    };

    inline Iterator iterator() {
        return Iterator(mMap);
    }

private:
    std::unordered_map<sp<K>, V, Hasher<sp<K>>, EqualityComparator<sp<K>>> mMap;
};


/*
 * HashMap template specialization if both K and V inherit from mindroid::Object.
 * Object hashing uses mindroid::Object::hashCode method and object comparison uses mindroid::Object::equals method.
 */
template<typename K, typename V>
class HashMap<sp<K>, sp<V>> :
        public Object {
public:
    HashMap() :
            mMap() {
    }

    HashMap(size_t initialCapacity) :
            mMap(initialCapacity) {
    }

    HashMap(const sp<HashMap<sp<K>, sp<V>>>& map) :
            mMap() {
        putAll(map);
    }

    virtual ~HashMap() {
        clear();
    }

    void clear() {
        mMap.clear();
    }

    bool containsKey(const sp<K>& key) const {
        if (key != nullptr) {
            auto itr = mMap.find(key);
            return itr != mMap.end();
        }
        return false;
    }

    bool containsValue(const sp<V>& value) const {
        if (value != nullptr) {
            for (auto itr = mMap.begin(); itr != mMap.end(); ++itr) {
                if (itr->second->equals(value)) {
                    return true;
                }
            }
        }
        return false;
    }

    sp<V> get(const sp<K>& key) {
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            return itr->second;
        } else {
            return nullptr;
        }
    }

    bool isEmpty() const {
        return mMap.empty();
    }

    sp<Set<sp<K>>> keySet() {
        if (isEmpty()) {
            return nullptr;
        }

        sp<Set<sp<K>>> keys = new Set<sp<K>>();
        auto itr = iterator();
        while (itr.hasNext()) {
            auto pair = itr.next();
            keys->add(pair.getKey());
        }
        return keys;
    }

    sp<V> put(const sp<K>& key, const sp<V>& value) {
        sp<V> oldValue = nullptr;
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            oldValue = itr->second;
        }

        mMap[key] = value;
        return oldValue;
    }

    void putAll(const sp<HashMap>& map) {
        auto itr = map->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            put(entry.getKey(), entry.getValue());
        }
    }

    sp<V> remove(const sp<K>& key) {
        sp<V> oldValue = nullptr;
        auto itr = mMap.find(key);
        if (itr != mMap.end()) {
            oldValue = itr->second;
        }

        mMap.erase(key);
        return oldValue;
    }

    size_t size() const {
        return mMap.size();
    }

    class Iterator;

    class Entry {
    public:
        sp<K> getKey() const {
            return mKey;
        }

        sp<V> getValue() const {
            return mValue;
        }

        sp<V> setValue(const sp<V>& value) {
            sp<V> oldValue = mValue;
            mValue = value;
            return oldValue;
        }

    private:
        Entry(const sp<K>& key, const sp<V>& value) : mKey(key), mValue(value) { }

        sp<K> mKey;
        sp<V> mValue;

        friend class Iterator;
    };

    class Iterator {
    public:
        Iterator(std::unordered_map<sp<K>, sp<V>, Hasher<sp<K>>, EqualityComparator<sp<K>>>& map) :
                mForwardIterator(false),
                mMap(&map),
                mIterator(mMap->begin()) {
        }

        ~Iterator() { }

        Iterator& operator=(const Iterator& iterator) {
            mForwardIterator = iterator.mForwardIterator;
            mMap = iterator.mMap;
            mIterator = iterator.mIterator;
            return *this;
        }

        bool hasNext() const {
            if (!mForwardIterator) {
                return mIterator != mMap->end();
            } else {
                typename std::unordered_map<sp<K>, sp<V>, Hasher<sp<K>>, EqualityComparator<sp<K>>>::iterator itr = mIterator;
                return ++itr != mMap->end();
            }
        }

        Entry next() {
            if (!mForwardIterator) {
                mForwardIterator = true;
            } else {
                ++mIterator;
            }
            Entry entry(mIterator->first, mIterator->second);
            return entry;
        }

        bool remove() {
            if (mForwardIterator) {
                mIterator = mMap->erase(mIterator);
                mForwardIterator = false;
                return true;
            }
            return false;
        }

    private:
        bool mForwardIterator;
        std::unordered_map<sp<K>, sp<V>, Hasher<sp<K>>, EqualityComparator<sp<K>>>* mMap;
        typename std::unordered_map<sp<K>, sp<V>, Hasher<sp<K>>, EqualityComparator<sp<K>>>::iterator mIterator;
    };

    inline Iterator iterator() {
        return Iterator(mMap);
    }

private:
    std::unordered_map<sp<K>, sp<V>, Hasher<sp<K>>, EqualityComparator<sp<K>>> mMap;
};

} /* namespace mindroid */

#endif /* MINDROID_HASHMAP_H_ */
