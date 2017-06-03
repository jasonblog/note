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

#ifndef MINDROID_SET_H_
#define MINDROID_SET_H_

#include "mindroid/lang/Object.h"
#include "mindroid/util/Assert.h"
#include <set>

namespace mindroid {

/*
 * Set template for standard types.
 */
template<typename T>
class Set :
        public Object {
public:
    Set() { }

    Set(const sp<Set<T>>& collection) {
        if (collection != nullptr) {
            auto itr = collection->iterator();
            while (itr.hasNext()) {
                add(itr.next());
            }
        }
    }

    virtual ~Set() {
        clear();
    }

    bool add(const T& value) {
        auto pair = mSet.insert(value);
        return pair.second;
    }

    void clear() {
        mSet.clear();
    }

    bool contains(const T& value) const {
        typename std::set<T>::const_iterator itr = mSet.find(value);
        return itr != mSet.end();
    }

    bool isEmpty() const {
        return mSet.empty();
    }

    bool remove(const T& value) {
        return mSet.erase(value) == 1;
    }

    size_t size() const {
        return mSet.size();
    }

    class Iterator {
    public:
        Iterator(std::set<T>& set) :
                mForwardIterator(false),
                mSet(&set),
                mIterator(mSet->begin()) {
        }

        ~Iterator() { }

        Iterator& operator=(const Iterator& iterator) {
            mForwardIterator = iterator.mForwardIterator;
            mSet = iterator.mSet;
            mIterator = iterator.mIterator;
            return *this;
        }

        bool hasNext() const {
            if (!mForwardIterator) {
                return mIterator != mSet->end();
            } else {
                typename std::set<T>::iterator itr = mIterator;
                return ++itr != mSet->end();
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
                mIterator = mSet->erase(mIterator);
                mForwardIterator = false;
                return true;
            }
            return false;
        }

    private:
        bool mForwardIterator;
        std::set<T>* mSet;
        typename std::set<T>::iterator mIterator;
    };

    inline Iterator iterator() {
        return Iterator(mSet);
    }

private:
    std::set<T> mSet;
};


/*
 * Set template specialization for mindroid::Object types.
 * Object comparison uses mindroid::Object::equals method.
 */
template<typename T>
class Set<sp<T>> :
        public Object {
public:
    Set() { }

    Set(const sp<Set<sp<T>>>& collection) {
        if (collection != nullptr) {
            auto itr = collection->iterator();
            while (itr.hasNext()) {
                add(itr.next());
            }
        }
    }

    virtual ~Set() {
        clear();
    }

    bool add(const sp<T>& value) {
        auto pair = mSet.insert(value);
        return pair.second;
    }

    void clear() {
        mSet.clear();
    }

    bool contains(const sp<T>& value) const {
        typename std::set<sp<T>, LessThanComparator<sp<T>>>::const_iterator itr = mSet.find(value);
        return itr != mSet.end();
    }

    bool isEmpty() const {
        return mSet.empty();
    }

    bool remove(const sp<T>& value) {
        return mSet.erase(value) == 1;
    }

    size_t size() const {
        return mSet.size();
    }

    class Iterator {
    public:
        Iterator(std::set<sp<T>, LessThanComparator<sp<T>>>& set) :
                mForwardIterator(false),
                mSet(&set),
                mIterator(mSet->begin()) {
        }

        ~Iterator() { }

        Iterator& operator=(const Iterator& iterator) {
            mForwardIterator = iterator.mForwardIterator;
            mSet = iterator.mSet;
            mIterator = iterator.mIterator;
            return *this;
        }

        bool hasNext() const {
            if (!mForwardIterator) {
                return mIterator != mSet->end();
            } else {
                typename std::set<sp<T>, LessThanComparator<sp<T>>>::iterator itr = mIterator;
                return ++itr != mSet->end();
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
                mIterator = mSet->erase(mIterator);
                mForwardIterator = false;
                return true;
            }
            return false;
        }

    private:
        bool mForwardIterator;
        std::set<sp<T>, LessThanComparator<sp<T>>>* mSet;
        typename std::set<sp<T>, LessThanComparator<sp<T>>>::iterator mIterator;
    };

    inline Iterator iterator() {
        return Iterator(mSet);
    }

private:
    std::set<sp<T>, LessThanComparator<sp<T>>> mSet;
};

} /* namespace mindroid */

#endif /* MINDROID_SET_H_ */
