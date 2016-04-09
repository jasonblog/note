#include "RefBase.h"
#include <iostream>
using namespace std;
#define INITIAL_STRONG_VALUE (1<<28)

class RefBase::weakref_impl : public RefBase::weakref_type
{
public:
    volatile int    mStrong;
    volatile int    mWeak;
    RefBase* const      mBase;
    volatile int    mFlags;

    weakref_impl(RefBase* base)
        : mStrong(INITIAL_STRONG_VALUE)
        , mWeak(0)
        , mBase(base)
        , mFlags(0)
    {
    }

    ~weakref_impl()
    {
        // cout << "~weakref_impl" << endl;
    }
};

// ---------------------------------------------------------------------------

void RefBase::incStrong(const void* id) const
{
    weakref_impl* const refs = mRefs;
    refs->incWeak(id);

    const int c = refs->mStrong++;

    if (c != INITIAL_STRONG_VALUE)  {
        return;
    }

    refs->mStrong = refs->mStrong - INITIAL_STRONG_VALUE;
}

void RefBase::decStrong(const void* id) const
{
    weakref_impl* const refs = mRefs;
    const int c = --refs->mStrong;

    if (c == 0) {
        if ((refs->mFlags & OBJECT_LIFETIME_WEAK) !=
            OBJECT_LIFETIME_WEAK) { //受到强指针控制
            delete this;
        }
    }

    refs->decWeak(id);
}

int RefBase::getStrongCount() const
{
    return mRefs->mStrong;
}

RefBase* RefBase::weakref_type::refBase() const
{
    return static_cast<const weakref_impl*>(this)->mBase;
}

void RefBase::weakref_type::incWeak(const void* id)
{
    weakref_impl* const impl = static_cast<weakref_impl*>(this);
    impl->mWeak++;
}

void RefBase::weakref_type::decWeak(const void* id)
{
    weakref_impl* const impl = static_cast<weakref_impl*>(this);
    const int c = --impl->mWeak;

    if (c != 0) {
        return;
    }

    if ((impl->mFlags & OBJECT_LIFETIME_WEAK) !=
        OBJECT_LIFETIME_WEAK) { //受强指针控制
        if (impl->mStrong == INITIAL_STRONG_VALUE) {
            delete impl->mBase;
        } else {
            delete impl;
        }
    } else {
        if ((impl->mFlags & OBJECT_LIFETIME_FOREVER) !=
            OBJECT_LIFETIME_FOREVER) { //受弱指针控制
            delete impl->mBase;
        }
    }
}

bool RefBase::weakref_type::attemptIncStrong(const void* id)
{
    incWeak(id);//先增加，后面有减少，整体不变

    weakref_impl* const impl = static_cast<weakref_impl*>(this);

    int curCount = impl->mStrong;

    if (curCount > 0 &&
        curCount != INITIAL_STRONG_VALUE) {//mStrong变化过，且大于0
        impl->mStrong = curCount + 1;
        curCount = impl->mStrong;
    }

    if (curCount <= 0 ||
        curCount ==
        INITIAL_STRONG_VALUE) {//mStrong没有变化过；或者变化后，为0
        bool allow;

        if (curCount == INITIAL_STRONG_VALUE) {
            allow = (impl->mFlags & OBJECT_LIFETIME_WEAK) != OBJECT_LIFETIME_WEAK
                    || impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);
        } else {
            allow = (impl->mFlags & OBJECT_LIFETIME_WEAK) == OBJECT_LIFETIME_WEAK
                    && impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);
        }

        if (!allow) {
            decWeak(id);//再减少
            return false;
        }

        curCount = impl->mStrong++;
    }



    if (curCount == INITIAL_STRONG_VALUE) {
        impl->mStrong = impl->mStrong - INITIAL_STRONG_VALUE;
    }

    return true;
}

int RefBase::weakref_type::getWeakCount() const
{
    return static_cast<const weakref_impl*>(this)->mWeak;
}

RefBase::weakref_type* RefBase::createWeak(const void* id) const
{
    mRefs->incWeak(id);
    return mRefs;
}

RefBase::weakref_type* RefBase::getWeakRefs() const
{
    return mRefs;
}

RefBase::RefBase()
    : mRefs(new weakref_impl(this))
{
}

RefBase::~RefBase()
{
    if (mRefs->mWeak == 0) {
        delete mRefs;
    }
}

void RefBase::extendObjectLifetime(int mode)
{
    mRefs->mFlags = mode;
}

bool RefBase::onIncStrongAttempted(int flags, const void* id)
{
    return (flags & FIRST_INC_STRONG) ? true : false;
}
