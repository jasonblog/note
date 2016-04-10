# C++ 智能指針


一、RefBase.h

```cpp
class RefBase
{
public:
    void incStrong(const void* id) const;
    void decStrong(const void* id) const;
    int  getStrongCount() const;

    class weakref_type
    {
    public:
        RefBase* refBase() const;
        void incWeak(const void* id);
        void decWeak(const void* id);
        bool attemptIncStrong(const void* id);
        int getWeakCount() const;
    };

    weakref_type* createWeak(const void* id) const;
    weakref_type* getWeakRefs() const;

protected:
    RefBase();
    virtual ~RefBase();//delete this時候會調用子類

    enum {
        OBJECT_LIFETIME_WEAK    = 0x0001,
        OBJECT_LIFETIME_FOREVER = 0x0003
    };

    void extendObjectLifetime(int  mode);

    enum {
        FIRST_INC_STRONG = 0x0001
    };

    virtual bool            onIncStrongAttempted(int flags,
            const void* id);//子類可以覆蓋

private:
    //friend class weakref_type;
    class weakref_impl; //不能include，只能前向聲明
    RefBase(const RefBase& o);
    RefBase&    operator=(const RefBase& o);
    weakref_impl* const mRefs;
};

// ---------------------------------------------------------------------------

template <typename T>
class sp
{
public:
    typedef typename RefBase::weakref_type weakref_type;
    //相當於typedef  RefBase::weakref_type weakref_typ
    inline sp() : m_ptr(0) {  }
    sp(T* other);
    sp(const sp<T>& other);
    ~sp();

    inline  T&      operator* () const  { return *m_ptr; }
    inline  T*      operator-> () const { return m_ptr;  }
    inline  T*      get() const         { return m_ptr; }

private:
    template<typename Y> friend class
    wp;//wp可以操作sp的私有變量，如構造函數
    sp(T* p, weakref_type* refs);
    T* m_ptr;
};

// ---------------------------------------------------------------------------

template <typename T>
class wp
{
public:
    typedef  RefBase::weakref_type weakref_type;
    wp(T* other);
    wp(const wp<T>& other);
    ~wp();
    sp<T> promote() const;
    inline  weakref_type* get_refs() const
    {
        return m_refs;
    }
private:
    T*              m_ptr;//strongPointer
    weakref_type*   m_refs;//weakref_impl

};

// ---------------------------------------------------------------------------

template<typename T>
sp<T>::sp(T* other)
    : m_ptr(other)//strongPointer
{
    if (other) {
        other->incStrong(this);
    }
}

template<typename T>
sp<T>::sp(const sp<T>& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr) {
        m_ptr->incStrong(this);
    }
}

template<typename T>
sp<T>::~sp()
{
    if (m_ptr) {
        m_ptr->decStrong(this);
    }
}

template<typename T>
sp<T>::sp(T* p, weakref_type* refs)
    : m_ptr((p && refs->attemptIncStrong(this)) ? p : 0)
{
}

// ---------------------------------------------------------------------------

template<typename T>
wp<T>::wp(T* other)
    : m_ptr(other)//strongPointer
{
    if (other) {
        m_refs = other->createWeak(this);    //weakref_impl
    }
}

template<typename T>
wp<T>::wp(const wp<T>& other)
    : m_ptr(other.m_ptr), m_refs(other.m_refs)
{
    if (m_ptr) {
        m_refs->incWeak(this);
    }
}


template<typename T>
wp<T>::~wp()
{
    if (m_ptr) {
        m_refs->decWeak(this);
    }
}

template<typename T>
sp<T> wp<T>::promote() const
{
    return sp<T>(m_ptr, m_refs);
}
```

二、RefBase.cpp

```cpp
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
            OBJECT_LIFETIME_WEAK) { //受到強指針控制
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
        OBJECT_LIFETIME_WEAK) { //受強指針控制
        if (impl->mStrong == INITIAL_STRONG_VALUE) {
            delete impl->mBase;
        } else {
            delete impl;
        }
    } else {
        if ((impl->mFlags & OBJECT_LIFETIME_FOREVER) !=
            OBJECT_LIFETIME_FOREVER) { //受弱指針控制
            delete impl->mBase;
        }
    }
}

bool RefBase::weakref_type::attemptIncStrong(const void* id)
{
    incWeak(id);//先增加，後面有減少，整體不變

    weakref_impl* const impl = static_cast<weakref_impl*>(this);

    int curCount = impl->mStrong;

    if (curCount > 0 &&
        curCount != INITIAL_STRONG_VALUE) {//mStrong變化過，且大於0
        impl->mStrong = curCount + 1;
        curCount = impl->mStrong;
    }

    if (curCount <= 0 ||
        curCount ==
        INITIAL_STRONG_VALUE) {//mStrong沒有變化過；或者變化後，為0
        bool allow;

        if (curCount == INITIAL_STRONG_VALUE) {
            allow = (impl->mFlags & OBJECT_LIFETIME_WEAK) != OBJECT_LIFETIME_WEAK
                    || impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);
        } else {
            allow = (impl->mFlags & OBJECT_LIFETIME_WEAK) == OBJECT_LIFETIME_WEAK
                    && impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);
        }

        if (!allow) {
            decWeak(id);//再減少
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
```

```cpp
void RefBase::decStrong(const void* id) const
{
    weakref_impl* const refs = mRefs;
    const int c = --refs->mStrong;

    if (c == 0) {
        if ((refs->mFlags & OBJECT_LIFETIME_WEAK) !=
            OBJECT_LIFETIME_WEAK) { //受到強指針控制
            delete this;
        }

    }

    refs->decWeak(id);
}
```

減少強引用計數時候，同時減少弱引用計數，如果強引用計數為0，並且受強指針控制，那麼調用子類和RefBase的析構函數

```cpp
void RefBase::weakref_type::decWeak(const void* id)
{
    weakref_impl* const impl = static_cast<weakref_impl*>(this);
    const int c = --impl->mWeak;

    if (c != 0) {
        return;
    }

    if ((impl->mFlags & OBJECT_LIFETIME_WEAK) !=
        OBJECT_LIFETIME_WEAK) { //受強指針控制
        if (impl->mStrong == INITIAL_STRONG_VALUE) {
            delete impl->mBase;
        } else {
            delete impl;
        }
    } else {
        if ((impl->mFlags & OBJECT_LIFETIME_FOREVER) !=
            OBJECT_LIFETIME_FOREVER) { //受弱指針控制
            delete impl->mBase;
        }
    }
}
```


```cpp
RefBase::~RefBase()  
{  
    if (mRefs->mWeak == 0) {  
        delete mRefs;  
    }  
}  
```

- 減少弱引用計數，如果弱引用計數為0，並且受弱指針控制，那麼調用delete impl->mBase，調用上面的析構函數，因為此時mWeek==0，所以執行delete mRefs
- 如果弱引用計數為0，並且不受強指針，也不受弱指針控制，那麼由自己調用delete來刪除
- 如果弱引用計數為0，並且受強指針控制，如果強引用計數為0，那麼只delete impl，因為RefBase及其子類已經被delelte掉了
- 如果弱引用計數為0，並且受強指針控制，如果強引用計數為INITIAL_STRONG_VALUE，那麼說明RefBase及其子類還沒有刪除，那麼要delelte impl->mBase，調用上面的析構函數，因為此時mWeek==0，所以執行delete mRefs

```cpp
bool RefBase::weakref_type::attemptIncStrong(const void* id)
{
    incWeak(id);
    weakref_impl* const impl = static_cast<weakref_impl*>(this);
    int curCount = impl->mStrong;

    if (curCount > 0 && curCount != INITIAL_STRONG_VALUE) {
        impl->mStrong = curCount + 1;
        curCount = impl->mStrong;
    }

    if (curCount <= 0 || curCount == INITIAL_STRONG_VALUE) {
        bool allow;

        if (curCount == INITIAL_STRONG_VALUE) {
            allow = (impl->mFlags & OBJECT_LIFETIME_WEAK) != OBJECT_LIFETIME_WEAK
                    || impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);
        } else {
            allow = (impl->mFlags & OBJECT_LIFETIME_WEAK) == OBJECT_LIFETIME_WEAK
                    && impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);
        }

        if (!allow) {
            decWeak(id);
            return false;
        }

        curCount = impl->mStrong++;
    }

    if (curCount == INITIAL_STRONG_VALUE + 1) {
        impl->mStrong = impl->mStrong - INITIAL_STRONG_VALUE;
    }

    return true;
}
```


```cpp
if (curCount > 0 && curCount != INITIAL_STRONG_VALUE)
{
    impl->mStrong = curCount + 1;
    curCount = impl->mStrong;
}
```

如果此時強引用計數不為0，或者初始值，那麼說明RefBase及其子類還沒有刪除，所以可以提升為強引用

```cpp
allow = (impl->mFlags& OBJECT_LIFETIME_WEAK) == OBJECT_LIFETIME_WEAK
        && impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);

```

- 如果此時強引用計數為0，並且還受強指針控制，那麼此時，RefBase及其子類已經被刪除了，所以不能提升為強引用
- 如果此時強引用計數為0，並且還受弱指針控制，那麼此時，RefBase及其子類還沒有刪除，能不能提升就看impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id)的了

```cpp
allow = (impl->mFlags&OBJECT_LIFETIME_WEAK) != OBJECT_LIFETIME_WEAK  
      || impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);  
```


- 如果此時強引用計數為INITIAL_STRONG_VALUE，並且還受強指針控制，此時RefBase及其子類還沒有刪除，所以能夠提升。

- 如果此時強引用計數為INITIAL_STRONG_VALUE，並且還受弱指針控制，此時RefBase及其子類還沒有刪除，能不能提升就看impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id)的了


- main.cpp

```cpp
#include <stdio.h>
#include <iostream>
#include "RefBase.h"
using namespace std;

#define INITIAL_STRONG_VALUE (1<<28)

class WeightClass: public RefBase
{
public:
    void printRefCount()
    {
        int strong = getStrongCount();
        weakref_type* ref = getWeakRefs();

        printf("-----------------------\n");
        printf("Strong Ref Count: %d.\n",
               (strong == INITIAL_STRONG_VALUE ? 0 : strong));
        printf("Weak Ref Count: %d.\n", ref->getWeakCount());
        printf("-----------------------\n");
    }
};

class StrongClass: public WeightClass
{
public:
    StrongClass()
    {
        printf("Construct StrongClass Object.\n");
    }

    virtual ~StrongClass()
    {
        printf("Destory StrongClass Object.\n");
    }
};

class WeakClass: public WeightClass
{
public:
    WeakClass()
    {
        extendObjectLifetime(OBJECT_LIFETIME_WEAK);

        printf("Construct WeakClass Object.\n");
    }

    virtual ~WeakClass()
    {
        printf("Destory WeakClass Object.\n");
    }
};

class ForeverClass: public WeightClass
{
public:
    ForeverClass()
    {
        extendObjectLifetime(OBJECT_LIFETIME_FOREVER);

        printf("Construct ForeverClass Object.\n");
    }

    virtual ~ForeverClass()
    {
        printf("Destory ForeverClass Object.\n");
    }
};

void TestStrongClass(StrongClass* pStrongClass)
{
    wp<StrongClass> wpOut = pStrongClass;
    pStrongClass->printRefCount();
    {
        sp<StrongClass> spInner = pStrongClass;
        pStrongClass->printRefCount();
    }
    pStrongClass->printRefCount();
    sp<StrongClass> spOut = wpOut.promote();
    printf("spOut: %p.\n", spOut.get());
    pStrongClass->printRefCount();
}

void TestStrongClass2(StrongClass* pStrongClass)
{
    wp<StrongClass> wpOut = pStrongClass;
    pStrongClass->printRefCount();
}

void TestStrongClass4(StrongClass* pStrongClass)
{
    wp<StrongClass> wpOut = pStrongClass;
    pStrongClass->printRefCount();
    sp<StrongClass> spInner = pStrongClass;
    pStrongClass->printRefCount();
    sp<StrongClass> spOut = wpOut.promote();
    printf("spOut: %p.\n", spOut.get());
    pStrongClass->printRefCount();
}

void TestStrongClass3(StrongClass* pStrongClass)
{
    wp<StrongClass> wpOut = pStrongClass;
    pStrongClass->printRefCount();
    sp<StrongClass> spOut = wpOut.promote();
    printf("spOut: %p.\n", spOut.get());
    pStrongClass->printRefCount();
}

void TestWeakClass(WeakClass* pWeakClass)
{
    wp<WeakClass> wpOut = pWeakClass;
    pWeakClass->printRefCount();
    {
        sp<WeakClass> spInner = pWeakClass;
        pWeakClass->printRefCount();
    }

    pWeakClass->printRefCount();
    sp<WeakClass> spOut = wpOut.promote();
    printf("spOut: %p.\n", spOut.get());
    pWeakClass->printRefCount();
}

void TestForeverClass(ForeverClass* pForeverClass)
{
    wp<ForeverClass> wpOut = pForeverClass;
    pForeverClass->printRefCount();
    {
        sp<ForeverClass> spInner = pForeverClass;
        pForeverClass->printRefCount();
    }
    pForeverClass->printRefCount();
}

int main(int argc, char** argv)
{
    /*      printf("Test Strong Class: \n");
     StrongClass* pStrongClass = new StrongClass();
     TestStrongClass(pStrongClass);*/

    /*        printf("Test Strong Class2: \n");
     StrongClass* pStrongClass = new StrongClass();
     TestStrongClass2(pStrongClass);*/

    /*        printf("\nTest Weak Class: \n");
     WeakClass* pWeakClass = new WeakClass();
     TestWeakClass(pWeakClass);*/

    /*        printf("\nTest Froever Class: \n");
     ForeverClass* pForeverClass = new ForeverClass();
     TestForeverClass(pForeverClass);
     pForeverClass->printRefCount();
     delete pForeverClass;*/

    /*   printf("Test Strong Class3: \n");
     StrongClass* pStrongClass = new StrongClass();
     TestStrongClass3(pStrongClass);*/

    printf("Test Strong Class4: \n");
    StrongClass* pStrongClass = new StrongClass();
    TestStrongClass4(pStrongClass);

    return 0;
}

```

```sh
g++ main.cpp RefBase.cpp -o main
```

```sh
Test Strong Class4: 
Construct StrongClass Object.
-----------------------
Strong Ref Count: 0.
Weak Ref Count: 1.
-----------------------
-----------------------
Strong Ref Count: 1.
Weak Ref Count: 2.
-----------------------
spOut: 0x2308c20.
-----------------------
Strong Ref Count: 2.
Weak Ref Count: 3.
-----------------------
Destory StrongClass Object.
~weakref_impl
```
## 本文參考

http://blog.csdn.net/luoshengyang/article/details/6786239