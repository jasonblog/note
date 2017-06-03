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

#ifndef MINDROID_LINKEDLIST_H_
#define MINDROID_LINKEDLIST_H_

#include "mindroid/lang/Object.h"
#include "mindroid/util/Set.h"
#include "mindroid/util/Assert.h"
#include <list>

namespace mindroid {

/*
 * LinkedList template for standard types.
 */
template<typename T>
class LinkedList :
        public Object {
public:
    LinkedList() { }

    LinkedList(const sp<LinkedList<T>>& collection) {
        if (collection != nullptr) {
            auto itr = collection->iterator();
            while (itr.hasNext()) {
                add(itr.next());
            }
        }
    }

    LinkedList(const sp<Set<T>>& collection) {
        if (collection != nullptr) {
            auto itr = collection->iterator();
            while (itr.hasNext()) {
                add(itr.next());
            }
        }
    }

    virtual ~LinkedList() {
        clear();
    }

    bool add(const T& value) {
        mList.push_back(value);
        return true;
    }

    void add(size_t index, const T& value) {
        Assert::assertTrue(index <= size());
        if (index < size()) {
            typename std::list<T>::iterator itr = mList.begin();
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
        typename std::list<T>::const_iterator itr = mList.begin();
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
        typename std::list<T>::iterator itr = mList.begin();
        size_t i = 0;
        while (itr != mList.end()) {
            if (i == index) {
                break;
            }
            ++itr;
            ++i;
        }
        return *itr;
    }

    ssize_t indexOf(const T& value) const {
        typename std::list<T>::const_iterator itr = mList.begin();
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
        typename std::list<T>::iterator itr = mList.begin();
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
        typename std::list<T>::iterator itr = mList.begin();
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
        typename std::list<T>::iterator itr = mList.begin();
        size_t i = 0;
        while (itr != mList.end()) {
            if (i == index) {
                T oldValue = *itr;
                *itr = value;
                return oldValue;
            }
            ++itr;
            ++i;
        }
        return nullptr;
    }

    size_t size() const {
        return mList.size();
    }

    class Iterator {
    public:
        Iterator(std::list<T>& list) :
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
                typename std::list<T>::iterator itr = mIterator;
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
        std::list<T>* mList;
        typename std::list<T>::iterator mIterator;
    };

    inline Iterator iterator() {
        return Iterator(mList);
    }

private:
    std::list<T> mList;
};


/*
 * LinkedList template specialization for mindroid::Object types.
 * Object comparison uses mindroid::Object::equals method.
 */
template<typename T>
class LinkedList<sp<T>> :
        public Object {
public:
    LinkedList() { }

    LinkedList(const sp<LinkedList<sp<T>>>& collection) {
        if (collection != nullptr) {
            auto itr = collection->iterator();
            while (itr.hasNext()) {
                add(itr.next());
            }
        }
    }

    LinkedList(const sp<Set<sp<T>>>& collection) {
        if (collection != nullptr) {
            auto itr = collection->iterator();
            while (itr.hasNext()) {
                add(itr.next());
            }
        }
    }

    virtual ~LinkedList() {
        clear();
    }

    bool add(const sp<T>& value) {
        mList.push_back(value);
        return true;
    }

    void add(size_t index, const sp<T>& value) {
        Assert::assertTrue(index <= size());
        if (index < size()) {
            typename std::list<sp<T>>::iterator itr = mList.begin();
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
            typename std::list<sp<T>>::const_iterator itr = mList.begin();
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
        typename std::list<sp<T>>::iterator itr = mList.begin();
        size_t i = 0;
        while (itr != mList.end()) {
            if (i == index) {
                break;
            }
            ++itr;
            ++i;
        }
        return *itr;
    }

    ssize_t indexOf(const sp<T>& value) const {
        typename std::list<sp<T>>::const_iterator itr = mList.cbegin();
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
        typename std::list<sp<T>>::iterator itr = mList.begin();
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
            typename std::list<sp<T>>::iterator itr = mList.begin();
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
        typename std::list<sp<T>>::iterator itr = mList.begin();
        size_t i = 0;
        while (itr != mList.end()) {
            if (i == index) {
                sp<T> oldValue = *itr;
                *itr = value;
                return oldValue;
            }
            ++itr;
            ++i;
        }
        return nullptr;
    }

    size_t size() const {
        return mList.size();
    }

    class Iterator {
    public:
        Iterator(std::list<sp<T>>& list) :
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
                typename std::list<sp<T>>::iterator itr = mIterator;
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
        std::list<sp<T>>* mList;
        typename std::list<sp<T>>::iterator mIterator;
    };

    inline Iterator iterator() {
        return Iterator(mList);
    }

private:
    std::list<sp<T>> mList;
};

} /* namespace mindroid */

#endif /* MINDROID_LINKEDLIST_H_ */
