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

#ifndef MINDROID_ARRAYLIST_H_
#define MINDROID_ARRAYLIST_H_

#include "mindroid/lang/Object.h"
#include "mindroid/util/Set.h"
#include "mindroid/util/Assert.h"
#include <vector>

namespace mindroid {

/*
 * ArrayList template for standard types.
 */
template<typename T>
class ArrayList :
        public Object {
public:
    ArrayList() :
            mList() {
    }

    ArrayList(size_t initialCapacity) :
            mList() {
        mList.reserve(initialCapacity);
    }

    ArrayList(sp<ArrayList<T>> collection) :
            mList() {
        if (collection != nullptr) {
            auto itr = collection->iterator();
            while (itr.hasNext()) {
                add(itr.next());
            }
        }
    }

    ArrayList(sp<Set<T>> collection) :
            mList() {
        if (collection != nullptr) {
            auto itr = collection->iterator();
            while (itr.hasNext()) {
                add(itr.next());
            }
        }
    }

    ArrayList(const T* collection, size_t size) :
            mList() {
        if (collection != nullptr) {
            mList.reserve(size);
            mList.assign(collection, collection + size);
        }
    }

    virtual ~ArrayList() {
        clear();
    }

    bool add(const T& value) {
        mList.push_back(value);
        return true;
    }

    void add(size_t index, const T& value) {
        Assert::assertTrue(index <= size());
        if (index < size()) {
            typename std::vector<T>::iterator itr = mList.begin();
            size_t i = 0;
            while (itr != mList.end()) {
                if (i == index) {
                    mList.insert(itr, value);
                    break;
                }
                ++itr;
                ++i;
            }
        } else {
            add(value);
        }
    }

    void clear() {
        mList.clear();
    }

    bool contains(const T& value) const {
        typename std::vector<T>::const_iterator itr = mList.begin();
        while (itr != mList.end()) {
            if (*itr == value) {
                return true;
            }
            ++itr;
        }
        return false;
    }

    T get(size_t index) {
        Assert::assertTrue(index < size());
        return mList.at(index);
    }

    ssize_t indexOf(const T& value) const {
        typename std::vector<T>::const_iterator itr = mList.begin();
        ssize_t i = 0;
        while (itr != mList.end()) {
            if (*itr == value) {
                return i;
            }
            ++itr;
            ++i;
        }
        return -1;
    }

    bool isEmpty() const {
        return mList.empty();
    }

    T remove(size_t index) {
        Assert::assertTrue(index < size());
        typename std::vector<T>::iterator itr = mList.begin();
        size_t i = 0;
        while (itr != mList.end()) {
            if (i == index) {
                T value = *itr;
                mList.erase(itr);
                return value;
            }
            ++itr;
            ++i;
        }
        return nullptr;
    }

    bool remove(const T& value) {
        typename std::vector<T>::iterator itr = mList.begin();
        while (itr != mList.end()) {
            if (*itr == value) {
                mList.erase(itr);
                return true;
            }
            ++itr;
        }
        return false;
    }

    T set(size_t index, const T& value) {
        Assert::assertTrue(index < size());
        T& curValue = mList.at(index);
        T oldValue = curValue;
        curValue = value;
        return oldValue;
    }

    size_t size() const {
        return mList.size();
    }

    const T* c_arr() const {
        return &mList[0];
    }

    class Iterator {
    public:
        Iterator(std::vector<T>& list) :
                mForwardIterator(false),
                mList(&list),
                mIterator(mList->begin()) {
        }

        ~Iterator() { }

        Iterator& operator=(const Iterator& iterator) {
            mForwardIterator = iterator.mForwardIterator;
            mList = iterator.mList;
            mIterator = iterator.mIterator;
            return *this;
        }

        bool hasNext() const {
            if (!mForwardIterator) {
                return mIterator != mList->end();
            } else {
                typename std::vector<T>::iterator itr = mIterator;
                return ++itr != mList->end();
            }
        }

        T next() {
            if (!mForwardIterator) {
                mForwardIterator = true;
            } else {
                ++mIterator;
            }
            return *mIterator;
        }

        bool remove() {
            if (mForwardIterator) {
                mIterator = mList->erase(mIterator);
                mForwardIterator = false;
                return true;
            }
            return false;
        }

    private:
        bool mForwardIterator;
        std::vector<T>* mList;
        typename std::vector<T>::iterator mIterator;
    };

    inline Iterator iterator() {
        return Iterator(mList);
    }

private:
    std::vector<T> mList;
};


/*
 * ArrayList template specialization for mindroid::Object types.
 * Object comparison uses mindroid::Object::equals method.
 */
template<typename T>
class ArrayList<sp<T>> :
        public Object {
public:
    ArrayList() :
            mList() {
    }

    ArrayList(size_t initialCapacity) :
            mList() {
        mList.reserve(initialCapacity);
    }

    ArrayList(const sp<ArrayList<sp<T>>>& collection) :
            mList() {
        if (collection != nullptr) {
            auto itr = collection->iterator();
            while (itr.hasNext()) {
                add(itr.next());
            }
        }
    }

    ArrayList(const sp<Set<sp<T>>>& collection) :
            mList() {
        if (collection != nullptr) {
            auto itr = collection->iterator();
            while (itr.hasNext()) {
                add(itr.next());
            }
        }
    }

    virtual ~ArrayList() {
        clear();
    }

    bool add(const sp<T>& value) {
        mList.push_back(value);
        return true;
    }

    void add(size_t index, const sp<T>& value) {
        Assert::assertTrue(index <= size());
        if (index < size()) {
            typename std::vector<sp<T>>::iterator itr = mList.begin();
            size_t i = 0;
            while (itr != mList.end()) {
                if (i == index) {
                    mList.insert(itr, value);
                    break;
                }
                ++itr;
                ++i;
            }
        } else {
            add(value);
        }
    }

    void clear() {
        mList.clear();
    }

    bool contains(const sp<T>& value) const {
        if (value != nullptr) {
            typename std::vector<sp<T>>::const_iterator itr = mList.begin();
            while (itr != mList.end()) {
                const mindroid::sp<mindroid::Object>& o = *itr;
                if (o->equals(value)) {
                    return true;
                }
                ++itr;
            }
        }
        return false;
    }

    sp<T> get(size_t index) {
        Assert::assertTrue(index < size());
        return mList.at(index);
    }

    ssize_t indexOf(const sp<T>& value) const {
        typename std::vector<sp<T>>::const_iterator itr = mList.cbegin();
        ssize_t i = 0;
        while (itr != mList.end()) {
            const mindroid::sp<mindroid::Object>& o = *itr;
            if (o->equals(value)) {
                return i;
            }
            ++itr;
            ++i;
        }
        return -1;
    }

    bool isEmpty() const {
        return mList.empty();
    }

    sp<T> remove(size_t index) {
        Assert::assertTrue(index < size());
        typename std::vector<sp<T>>::iterator itr = mList.begin();
        size_t i = 0;
        while (itr != mList.end()) {
            if (i == index) {
                sp<T> value = *itr;
                mList.erase(itr);
                return value;
            }
            ++itr;
            ++i;
        }
        return nullptr;
    }

    bool remove(const sp<T>& value) {
        if (value != nullptr) {
            typename std::vector<sp<T>>::iterator itr = mList.begin();
            while (itr != mList.end()) {
                const mindroid::sp<mindroid::Object>& o = *itr;
                if (o->equals(value)) {
                    mList.erase(itr);
                    return true;
                }
                ++itr;
            }
        }
        return false;
    }

    sp<T> set(size_t index, const sp<T>& value) {
        Assert::assertTrue(index < size());
        sp<T>& curValue = mList.at(index);
        sp<T> oldValue = curValue;
        curValue = value;
        return oldValue;
    }

    size_t size() const {
        return mList.size();
    }

    class Iterator {
    public:
        Iterator(std::vector<sp<T>>& list) :
                mForwardIterator(false),
                mList(&list),
                mIterator(mList->begin()) {
        }

        ~Iterator() { }

        Iterator& operator=(const Iterator& iterator) {
            mForwardIterator = iterator.mForwardIterator;
            mList = iterator.mList;
            mIterator = iterator.mIterator;
            return *this;
        }

        bool hasNext() const {
            if (!mForwardIterator) {
                return mIterator != mList->end();
            } else {
                typename std::vector<sp<T>>::iterator itr = mIterator;
                return ++itr != mList->end();
            }
        }

        sp<T> next() {
            if (!mForwardIterator) {
                mForwardIterator = true;
            } else {
                ++mIterator;
            }
            return *mIterator;
        }

        bool remove() {
            if (mForwardIterator) {
                mIterator = mList->erase(mIterator);
                mForwardIterator = false;
                return true;
            }
            return false;
        }

    private:
        bool mForwardIterator;
        std::vector<sp<T>>* mList;
        typename std::vector<sp<T>>::iterator mIterator;
    };

    inline Iterator iterator() {
        return Iterator(mList);
    }

private:
    std::vector<sp<T>> mList;
};

} /* namespace mindroid */

#endif /* MINDROID_ARRAYLIST_H_ */
