/*
 * Copyright (C) 2005 The Android Open Source Project
 * Copyright (C) 2011 Daniel Himmelein
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

#include "mindroid/lang/Object.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <typeinfo>
#include <sys/types.h>
#include <pthread.h>

// Log all reference counting operations
#define PRINT_REFERENCES 0
// Debugging operations
#define DEBUG_REFERENCES 0
#define DEBUG_REFERENCES_TRACK_REFERENCES 0
#define DEBUG_REFERENCES_MEMORIZE_REF_OPERATIONS_DURING_REF_TRACKING 0

namespace mindroid {

#define INITIAL_STRONG_REFERENCE_VALUE (1<<28)

class Object::WeakReferenceImpl : public Object::WeakReference {
public:
    volatile int32_t mStrongReferenceCounter;
    volatile int32_t mWeakReferenceCounter;
    Object* const mObject;
    volatile int32_t mFlags;
    Destroyer* mDestroyer;

#if !DEBUG_REFERENCES
    WeakReferenceImpl(Object* object) :
            mStrongReferenceCounter(INITIAL_STRONG_REFERENCE_VALUE),
            mWeakReferenceCounter(0),
            mObject(object),
            mFlags(0),
            mDestroyer(nullptr) {
    }

    void addStrongReference(const void* id) { }
    void removeStrongReference(const void* id) { }
    void addWeakReference(const void* id) { }
    void removeWeakReference(const void* id) { }
    void printReferences() const { }
    void trackReference(bool, bool) { }

#else
    WeakReferenceImpl(Object* object) :
            mStrongReferenceCounter(INITIAL_STRONG_REFERENCE_VALUE),
            mWeakReferenceCounter(0),
            mObject(object),
            mFlags(0),
            mDestroyer(nullptr),
            mLock(PTHREAD_MUTEX_INITIALIZER),
            mStrongReferences(nullptr),
            mWeakReferences(nullptr),
            mTrackReferences(!!DEBUG_REFERENCES_TRACK_REFERENCES),
            mMemorizeRefOperationsDuringRefTracking(!!DEBUG_REFERENCES_MEMORIZE_REF_OPERATIONS_DURING_REF_TRACKING) {
    }

    ~WeakReferenceImpl() {
        if (!mMemorizeRefOperationsDuringRefTracking && mStrongReferences != nullptr) {
            DEBUG_INFO("Remaining strong references: ");
            Item* item = mStrongReferences;
            while (item) {
                char c = item->referenceCounter >= 0 ? '+' : '-';
                DEBUG_INFO("\t%c ID %p (reference couter is %d):", c, item->id, item->referenceCounter);
                item = item->nextItem;
            }
        }

        if (!mMemorizeRefOperationsDuringRefTracking && mWeakReferences != nullptr) {
            DEBUG_INFO("Remaining weak references: ");
            Item* item = mWeakReferences;
            while (item) {
                char c = item->referenceCounter >= 0 ? '+' : '-';
                DEBUG_INFO("\t%c ID %p (reference counter is %d):", c, item->id, item->referenceCounter);
                item = item->nextItem;
            }
        }
    }

    void addStrongReference(const void* id) {
        addItem(&mStrongReferences, id, mStrongReferenceCounter);
    }

    void removeStrongReference(const void* id) {
        if (!mMemorizeRefOperationsDuringRefTracking) {
            removeItem(&mStrongReferences, id);
        } else {
            addItem(&mStrongReferences, id, -mStrongReferenceCounter);
        }
    }

    void addWeakReference(const void* id) {
        addItem(&mWeakReferences, id, mWeakReferenceCounter);
    }

    void removeWeakReference(const void* id) {
        if (!mMemorizeRefOperationsDuringRefTracking) {
            removeItem(&mWeakReferences, id);
        } else {
            addItem(&mWeakReferences, id, -mWeakReferenceCounter);
        }
    }

    void trackReference(bool trackReferences, bool memorizeRefOperationsDuringRefTracking) {
        mTrackReferences = trackReferences;
        mMemorizeRefOperationsDuringRefTracking = memorizeRefOperationsDuringRefTracking;
    }

    void printReferences() const {
        char* buffer = (char*) malloc(1);
        buffer[0] = '\0';

        {
            pthread_mutex_lock(&mLock);
            char tmpBuffer[128];
            sprintf(tmpBuffer, "Strong references on Object %p (WeakReference %p):\n", mObject, this);
            buffer = (char*) realloc(buffer, strlen(buffer) + strlen(tmpBuffer) + 1);
            strcat(buffer, tmpBuffer);
            buffer = printItems(buffer, mStrongReferences);
            sprintf(tmpBuffer, "Weak references on Object %p (WeakReference %p):\n", mObject, this);
            buffer = (char*) realloc(buffer, strlen(buffer) + strlen(tmpBuffer) + 1);
            strcat(buffer, tmpBuffer);
            buffer = printItems(buffer, mWeakReferences);
            pthread_mutex_unlock(&mLock);
        }

        DEBUG_INFO("%s", buffer);

        free(buffer);
    }

private:
    struct Item {
        const void* id;
        int32_t referenceCounter;
        Item* nextItem;
    };

    void addItem(Item** items, const void* id, int32_t refCounter) {
        if (mTrackReferences) {
            pthread_mutex_lock(&mLock);
            Item* item = new Item;
            item->id = id;
            item->referenceCounter = refCounter;
            item->nextItem = *items;
            *items = item;
            pthread_mutex_unlock(&mLock);
        }
    }

    void removeItem(Item** items, const void* id) {
        if (mTrackReferences) {
            pthread_mutex_lock(&mLock);
            Item* item = *items;
            while (item != nullptr) {
                if (item->id == id) {
                    *items = item->nextItem;
                    delete item;
                    return;
                }

                items = &item->nextItem;
                item = *items;
            }
            pthread_mutex_unlock(&mLock);

            ERROR_INFO("Removing id %p on Object %p (WeakReference %p) that doesn't exist!\n", id, mObject, this);
        }
    }

    char* printItems(char* buffer, const Item* items) const {
        char tmpBuffer[128];
        while (items) {
            char c = items->referenceCounter >= 0 ? '+' : '-';
            sprintf(tmpBuffer, "\t%c ID %p (reference count: %d):\n", c, items->id, items->referenceCounter);
            buffer = (char*) realloc(buffer, strlen(buffer) + strlen(tmpBuffer) + 1);
            strcat(buffer, tmpBuffer);
            items = items->nextItem;
        }
        return buffer;
    }

    mutable pthread_mutex_t mLock;
    Item* mStrongReferences;
    Item* mWeakReferences;
    bool mTrackReferences;
    bool mMemorizeRefOperationsDuringRefTracking;
#endif
};

Object::Object() : mReference(new WeakReferenceImpl(this)) { }

Object::~Object() {
    if ((mReference->mFlags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_WEAK_REFERENCE) {
        if (mReference->mWeakReferenceCounter == 0) {
            delete mReference;
        }
    }
}

bool Object::equals(const sp<Object>& other) const {
    if (other == nullptr) {
        return false;
    }
    return this == other.getPointer();
}

size_t Object::hashCode() const {
    return (size_t) mReference->mObject;
}

void Object::incStrongReference(const void* id) const {
    WeakReferenceImpl* const reference = mReference;
    reference->incWeakReference(id);
    reference->addStrongReference(id);
    const int32_t oldStrongReferenceCount = AtomicInteger::getAndIncrement(&reference->mStrongReferenceCounter);
    ASSERT(oldStrongReferenceCount > 0, "Object::incStrongReference() called on %p after underflow", reference);
#if PRINT_REFERENCES
    if (oldStrongReferenceCount == INITIAL_STRONG_REFERENCE_VALUE) {
        DEBUG_INFO("Object::incStrongReference() of %p from %p: reference count is %d\n", this, id, 1);
    } else {
        DEBUG_INFO("Object::incStrongReference() of %p from %p: reference count is %d\n", this, id, oldStrongReferenceCount + 1);
    }
#endif
    if (oldStrongReferenceCount != INITIAL_STRONG_REFERENCE_VALUE) {
        return;
    }
    AtomicInteger::getAndAdd(-INITIAL_STRONG_REFERENCE_VALUE, &reference->mStrongReferenceCounter);
    const_cast<Object*>(this)->onFirstReference();
}

void Object::decStrongReference(const void* id) const {
    WeakReferenceImpl* const reference = mReference;
    reference->removeStrongReference(id);
    const int32_t oldStrongReferenceCount = AtomicInteger::getAndDecrement(&reference->mStrongReferenceCounter);
#if PRINT_REFERENCES
    DEBUG_INFO("Object::decStrongReference() of %p from %p: reference count is %d\n", this, id, oldStrongReferenceCount - 1);
#endif
    ASSERT(oldStrongReferenceCount >= 1, "Object::decStrongReference() called on %p too many times", reference);
    if (oldStrongReferenceCount == 1) {
        const_cast<Object*>(this)->onLastReference(id);
        if ((reference->mFlags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG_REFERENCE) {
            if (reference->mDestroyer == nullptr) {
                delete this;
            } else {
                reference->mDestroyer->destroy(const_cast<Object*>(this));
            }
        }
    }
    reference->decWeakReference(id);
}

int32_t Object::getStrongReferenceCount() const {
    return mReference->mStrongReferenceCounter;
}

void Object::setObjectLifetime(int32_t mode) const {
    switch (mode) {
    case OBJECT_LIFETIME_STRONG_REFERENCE:
        AtomicInteger::getAndAnd(mode, &mReference->mFlags);
        break;
    case OBJECT_LIFETIME_WEAK_REFERENCE:
        AtomicInteger::getAndOr(mode, &mReference->mFlags);
        break;
    }
}

void Object::onFirstReference() {
}

void Object::onLastReference(const void* id) {
}

void Object::setDestroyer(Object::Destroyer* destroyer) {
    mReference->mDestroyer = destroyer;
}

Object::Destroyer::~Destroyer() {
}

Object* Object::WeakReference::toObject() const {
    return static_cast<const WeakReferenceImpl*>(this)->mObject;
}

void Object::WeakReference::incWeakReference(const void* id) {
    WeakReferenceImpl* const reference = static_cast<WeakReferenceImpl*>(this);
    reference->addWeakReference(id);
    const int32_t oldWeakReferenceCount = AtomicInteger::getAndIncrement(&reference->mWeakReferenceCounter);
    ASSERT(oldWeakReferenceCount >= 0, "Object::WeakReference::incWeakReference() called on %p after underflow", this);
}

void Object::WeakReference::decWeakReference(const void* id) {
    WeakReferenceImpl* const reference = static_cast<WeakReferenceImpl*>(this);
    reference->removeWeakReference(id);
    const int32_t oldWeakReferenceCount = AtomicInteger::getAndDecrement(&reference->mWeakReferenceCounter);
    ASSERT(oldWeakReferenceCount >= 1, "Object::WeakReference::decWeakReference() called on %p too many times", this);
    if (oldWeakReferenceCount != 1) {
        return;
    }

    if ((reference->mFlags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG_REFERENCE) {
        if (reference->mStrongReferenceCounter == INITIAL_STRONG_REFERENCE_VALUE) {
            if (reference->mDestroyer == nullptr) {
                delete reference->mObject;
            } else {
                if (reference->mObject != nullptr) {
                    reference->mDestroyer->destroy(reference->mObject);
                }
            }
        }
        delete reference;
    } else {
        if (reference->mDestroyer == nullptr) {
            delete reference->mObject;
        } else {
            if (reference->mObject != nullptr) {
                reference->mDestroyer->destroy(reference->mObject);
            }
        }
    }
}

bool Object::WeakReference::tryIncStrongReference(const void* id) {
    incWeakReference(id);

    WeakReferenceImpl* const reference = static_cast<WeakReferenceImpl*>(this);
    int32_t strongReferenceCount = reference->mStrongReferenceCounter;
    ASSERT(strongReferenceCount >= 0, "Object::WeakReference::tryIncStrongReference() called on %p after underflow", this);

    while (strongReferenceCount > 0 && strongReferenceCount != INITIAL_STRONG_REFERENCE_VALUE) {
        if (AtomicInteger::compareAndSwap(strongReferenceCount, strongReferenceCount + 1, &reference->mStrongReferenceCounter) == 0) {
            break;
        }
        strongReferenceCount = reference->mStrongReferenceCounter;
    }

    if (strongReferenceCount <= 0 || strongReferenceCount == INITIAL_STRONG_REFERENCE_VALUE) {
        if ((reference->mFlags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG_REFERENCE) {
            if (strongReferenceCount <= 0) {
                decWeakReference(id);
                return false;
            }

            while (strongReferenceCount > 0) {
                if (AtomicInteger::compareAndSwap(strongReferenceCount, strongReferenceCount + 1, &reference->mStrongReferenceCounter) == 0) {
                    break;
                }
                strongReferenceCount = reference->mStrongReferenceCounter;
            }

            if (strongReferenceCount <= 0) {
                decWeakReference(id);
                return false;
            }
        } else {
            strongReferenceCount = AtomicInteger::getAndIncrement(&reference->mStrongReferenceCounter);
        }
    }

    reference->addStrongReference(id);

#if PRINT_REFERENCES
    DEBUG_INFO("Object::WeakReference::tryIncStrongReference() of %p from %p: reference count is %d\n", this, id, strongReferenceCount + 1);
#endif

    strongReferenceCount = reference->mStrongReferenceCounter;
    while (strongReferenceCount >= INITIAL_STRONG_REFERENCE_VALUE) {
        ASSERT(strongReferenceCount > INITIAL_STRONG_REFERENCE_VALUE, "Object::WeakReference::tryIncStrongReference() of %p underflowed INITIAL_STRONG_REFERENCE_VALUE", this);
        if (AtomicInteger::compareAndSwap(strongReferenceCount, strongReferenceCount - INITIAL_STRONG_REFERENCE_VALUE, &reference->mStrongReferenceCounter) == 0) {
            break;
        }
        strongReferenceCount = reference->mStrongReferenceCounter;
    }

    return true;
}

int32_t Object::WeakReference::getWeakReferenceCount() const {
    return static_cast<const WeakReferenceImpl*>(this)->mWeakReferenceCounter;
}

void Object::WeakReference::printReferences() const {
    static_cast<const WeakReferenceImpl*>(this)->printReferences();
}

void Object::WeakReference::trackReference(bool trackReference, bool memorizeRefOperationsDuringRefTracking) {
    static_cast<WeakReferenceImpl*>(this)->trackReference(trackReference, memorizeRefOperationsDuringRefTracking);
}

Object::WeakReference* Object::createWeakReference(const void* id) const {
    mReference->incWeakReference(id);
    return mReference;
}

Object::WeakReference* Object::getWeakReference() const {
    return mReference;
}

} /* namespace mindroid */
