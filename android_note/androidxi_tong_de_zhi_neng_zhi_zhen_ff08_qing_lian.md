# Android系統的智能指針（輕量級指針、強指針和弱指針）的實現原理分析



Android系統的運行時庫層代碼是用C++來編寫的，用C++來寫代碼最容易出錯的地方就是指針了，一旦使用不當，輕則造成內存洩漏，重則造成系統崩潰。不過系統為我們提供了智能指針，避免出現上述問題，本文將系統地分析Android系統智能指針（輕量級指針、強指針和弱指針）的實現原理。


在使用C++來編寫代碼的過程中，指針使用不當造成內存洩漏一般就是因為new了一個對象並且使用完之後，忘記了delete這個對象，而造成系統崩潰一般就是因為一個地方delete了這個對象之後，其它地方還在繼續使原來指向這個對象的指針。為了避免出現上述問題，一般的做法就是使用引用計數的方法，每當有一個指針指向了一個new出來的對象時，就對這個對象的引用計數增加1，每當有一個指針不再使用這個對象時，就對這個對象的引用計數減少1，每次減1之後，如果發現引用計數值為0時，那麼，就要delete這個對象了，這樣就避免了忘記delete對象或者這個對象被delete之後其它地方還在使用的問題了。但是，如何實現這個對象的引用計數呢？肯定不是由開發人員來手動地維護了，要開發人員時刻記住什麼時候該對這個對象的引用計數加1，什麼時候該對這個對象的引用計數減1，一來是不方便開發，二來是不可靠，一不小心哪裡多加了一個1或者多減了一個1，就會造成災難性的後果。這時候，智能指針就粉墨登場了。首先，智能指針是一個對象，不過這個對象代表的是另外一個真實使用的對象，當智能指針指向實際對象的時候，就是智能指針對象創建的時候，當智能指針不再指向實際對象的時候，就是智能指針對象銷毀的時候，我們知道，在C++中，對象的創建和銷毀時會分別自動地調用對象的構造函數和析構函數，這樣，負責對真實對象的引用計數加1和減1的工作就落實到智能指針對象的構造函數和析構函數的身上了，這也是為什麼稱這個指針對象為智能指針的原因。


在計算機科學領域中，提供垃圾收集（Garbage Collection）功能的系統框架，即提供對象託管功能的系統框架，例如Java應用程序框架，也是採用上述的引用計數技術方案來實現的，然而，簡單的引用計數技術不能處理系統中對象間循環引用的情況。考慮這樣的一個場景，系統中有兩個對象A和B，在對象A的內部引用了對象B，而在對象B的內部也引用了對象A。當兩個對象A和B都不再使用時，垃圾收集系統會發現無法回收這兩個對象的所佔據的內存的，因為系統一次只能收集一個對象，而無論系統決定要收回對象A還是要收回對象B時，都會發現這個對象被其它的對象所引用，因而就都回收不了，這樣就造成了內存洩漏。這樣，就要採取另外的一種引用計數技術了，即對象的引用計數同時存在強引用和弱引用兩種計數，例如，Apple公司提出的Cocoa框架，當父對象要引用子對象時，就對子對象使用強引用計數技術，而當子對象要引用父對象時，就對父對象使用弱引用計數技術，而當垃圾收集系統執行對象回收工作時，只要發現對象的強引用計數為0，而不管它的弱引用計數是否為0，都可以回收這個對象，但是，如果我們只對一個對象持有弱引用計數，當我們要使用這個對象時，就不直接使用了，必須要把這個弱引用升級成為強引用時，才能使用這個對象，在轉換的過程中，如果對象已經不存在，那麼轉換就失敗了，這時候就說明這個對象已經被銷毀了，不能再使用了。


瞭解了這些背景知識後，我們就可以進一步學習Android系統的智能指針的實現原理了。Android系統提供了強大的智能指針技術供我們使用，這些智能指針實現方案既包括簡單的引用計數技術，也包括了複雜的引用計數技術，即對象既有強引用計數，也有弱引用計數，對應地，這三種智能指針分別就稱為輕量級指針（Light Pointer）、強指針（Strong Pointer）和弱指針（Weak Pointer）。

無論是輕量級指針，還是強指針和弱指針，它們的實現框架都是一致的，即由對象本身來提供引用計數器，但是它不會去維護這個引用計數器的值，而是由智能指針來維護，就好比是對象提供素材，但是具體怎麼去使用這些素材，就交給智能指針來處理了。由於不管是什麼類型的對象，它都需要提供引用計數器這個素材，在C++中，我們就可以把這個引用計數器素材定義為一個公共類，這個類只有一個成員變量，那就是引用計數成員變量，其它提供智能指針引用的對象，都必須從這個公共類繼承下來，這樣，這些不同的對象就天然地提供了引用計數器給智能指針使用了。總的來說就是我們在實現智能指會的過程中，第一是要定義一個負責提供引用計數器的公共類，第二是我們要實現相應的智能指針對象類，後面我們會看到這種方案是怎麼樣實現的。
接下來，我們就先介紹輕量級指針的實現原理，然後再接著介紹強指針和弱指針的實現原理。

###1. 輕量級指針
先來看一下實現引用計數的類LightRefBase，它定義在frameworks/base/include/utils/RefBase.h文件中：

```cpp
template <class T>
class LightRefBase
{
public:
    inline LightRefBase() : mCount(0) { }
    inline void incStrong(const void* id) const
    {
        android_atomic_inc(&mCount);
    }
    inline void decStrong(const void* id) const
    {
        if (android_atomic_dec(&mCount) == 1) {
            delete static_cast<const T*>(this);
        }
    }
    //! DEBUGGING ONLY: Get current strong ref count.
    inline int32_t getStrongCount() const
    {
        return mCount;
    }

protected:
    inline ~LightRefBase() { }

private:
    mutable volatile int32_t mCount;
};
```

這個類很簡單，它只一個成員變量mCount，這就是引用計數器了，它的初始化值為0，另外，這個類還提供兩個成員函數incStrong和decStrong來維護引用計數器的值，這兩個函數就是提供給智能指針來調用的了，這裡要注意的是，在decStrong函數中，如果當前引用計數值為

1，那麼當減1後就會變成0，於是就會delete這個對象。

前面說過，要實現自動引用計數，除了要有提供引用計數器的基類外，還需要有智能指針類。在Android系統中，配合LightRefBase引用計數使用的智能指針類便是sp了，它也是定義在frameworks/base/include/utils/RefBase.h文件中：


```cpp
template <typename T>
class sp
{
public:
    typedef typename RefBase::weakref_type weakref_type;

    inline sp() : m_ptr(0) { }

    sp(T* other);
    sp(const sp<T>& other);
    template<typename U> sp(U* other);
    template<typename U> sp(const sp<U>& other);

    ~sp();

    // Assignment

    sp& operator = (T* other);
    sp& operator = (const sp<T>& other);

    template<typename U> sp& operator = (const sp<U>& other);
    template<typename U> sp& operator = (U* other);

    //! Special optimization for use by ProcessState (and nobody else).
    void force_set(T* other);

    // Reset

    void clear();

    // Accessors

    inline  T&      operator* () const
    {
        return *m_ptr;
    }
    inline  T*      operator-> () const
    {
        return m_ptr;
    }
    inline  T*      get() const
    {
        return m_ptr;
    }

    // Operators

    COMPARE( ==)
    COMPARE( !=)
    COMPARE( >)
    COMPARE( <)
    COMPARE( <=)
    COMPARE( >=)

private:
    template<typename Y> friend class sp;
    template<typename Y> friend class wp;

    // Optimization for wp::promote().
    sp(T* p, weakref_type* refs);

    T*              m_ptr;
};
```

這個類的內容比較多，但是這裡我們只關注它的成員變量m_ptr、構造函數和析構函數。不難看出，成員變量m_ptr就是指向真正的對象了，它是在構造函數裡面初始化的。接下來我們就再看一下它的兩個構造函數，一個是普通構造函數，一個拷貝構造函數：

```cpp
template<typename T>
sp<T>::sp(T* other)
    : m_ptr(other)
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
```

析構函數也很簡單，只是調用m_ptr的成員函數decStrong來減少對象的引用計數值，這裡就是調用LightRefBase類的decStrong函數了，前面我們看到，當這個引用計數減1後變成0時，就會自動delete這個對象了。
輕量級智能指針的實現原理大概就是這樣了，比較簡單，下面我們再用一個例子來說明它的用法。

###2. 輕量級指針的用法
參考在Ubuntu上為Android系統內置C可執行程序測試Linux內核驅動程序一文，我們在external目錄下建立一個C++工程目錄lightpointer，它裡面有兩個文件，一個lightpointer.cpp文件，另外一個是Android.mk文件。
源文件lightpointer.cpp的內容如下：


```cpp
#include <stdio.h>
#include <utils/RefBase.h>

using namespace android;

class LightClass : public LightRefBase<LightClass>
{
public:
    LightClass()
    {
        printf("Construct LightClass Object.");

    }

    virtual ~LightClass()
    {
        printf("Destory LightClass Object.");

    }

};

int main(int argc, char** argv)
{
    LightClass* pLightClass = new LightClass();
    sp<LightClass> lpOut = pLightClass;

    printf("Light Ref Count: %d.\n", pLightClass->getStrongCount());

    {
        sp<LightClass> lpInner = lpOut;

        printf("Light Ref Count: %d.\n", pLightClass->getStrongCount());

    }

    printf("Light Ref Count: %d.\n", pLightClass->getStrongCount());

    return 0;
}
```

我們創建一個自己的類LightClass，繼承了LightRefBase模板類，這樣類LightClass就具有引用計數的功能了。在main函數裡面，我們首先new一個LightClass對象，然後把這個對象賦值給智能指針lpOut，這時候通過一個printf語句來將當前對象的引用計數值打印出來，從前面的分析可以看出，如果一切正常的話，這裡打印出來的引用計數值為1。接著，我們又在兩個大括號裡面定義了另外一個智能指針lpInner，它通過lpOut間接地指向了前面我們所創建的對象，這時候再次將當前對象的引用計數值打印出來，從前面 的分析也可以看出，如果一切正常的話，這裡打印出來的引用計數值應該為2。程序繼承往下執行，當出了大括號的範圍的時候，智能指針對象lpInner就被析構了，從前面的分析可以知道，智能指針在析構的時候，會減少當前對象的引用計數值，因此，最後一個printf語句打印出來的引用計數器值應該為1。當main函數執行完畢後，智能指針lpOut也會被析構，被析構時，它會再次減少當前對象的引用計數，這時候，對象的引用計數值就為0了，於是，它就會被delete了。
編譯腳本文件Android.mk的內容如下：

```sh
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := lightpointer
LOCAL_SRC_FILES := lightpointer.cpp
LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libutils
include $(BUILD_EXECUTABLE)
```

最後，我們參照如何單獨編譯Android源代碼中的模塊一文，使用mmm命令對工程進行編譯：
 
```sh
USER-NAME@MACHINE-NAME:~/Android$ mmm ./external/lightpointer  
```
編譯之後，就可以打包了：

```sh
USER-NAME@MACHINE-NAME:~/Android$ make snod  
```

最後得到可執行程序lightpointer就位於設備上的/system/bin/目錄下。啟動模擬器，通過adb shell命令進入到模擬器終端，進入到/system/bin/目錄，執行lightpointer可執行程序，驗證程序是否按照我們設計的邏輯運行：

```sh
USER-NAME@MACHINE-NAME:~/Android$ adb shell  
root@android:/ # cd system/bin/          
root@android:/system/bin # ./lightpointer                                        
Construct LightClass Object.  
Light Ref Count: 1.  
Light Ref Count: 2.  
Light Ref Count: 1.  
Destory LightClass Object.  
```

這裡可以看出，程序一切都是按照我們的設計來運行，這也驗證了我們上面分析的輕量級智能指針的實現原理。

###  3. 強指針

強指針所使用的引用計數類為RefBase，它LightRefBase類要複雜多了，所以才稱後者為輕量級的引用計數基類吧。我們先來看看RefBase類的實現，它定義在frameworks/base/include/utils/RefBase.h文件中：


```cpp
class RefBase
{
public:
    void            incStrong(const void* id) const;
    void            decStrong(const void* id) const;

    void            forceIncStrong(const void* id) const;

    //! DEBUGGING ONLY: Get current strong ref count.
    int32_t         getStrongCount() const;

    class weakref_type
    {
    public:
        RefBase*            refBase() const;

        void                incWeak(const void* id);
        void                decWeak(const void* id);

        bool                attemptIncStrong(const void* id);

        //! This is only safe if you have set OBJECT_LIFETIME_FOREVER.
        bool                attemptIncWeak(const void* id);

        //! DEBUGGING ONLY: Get current weak ref count.
        int32_t             getWeakCount() const;

        //! DEBUGGING ONLY: Print references held on object.
        void                printRefs() const;

        //! DEBUGGING ONLY: Enable tracking for this object.
        // enable -- enable/disable tracking
        // retain -- when tracking is enable, if true, then we save a stack trace
        //           for each reference and dereference; when retain == false, we
        //           match up references and dereferences and keep only the
        //           outstanding ones.

        void                trackMe(bool enable, bool retain);
    };

    weakref_type*   createWeak(const void* id) const;

    weakref_type*   getWeakRefs() const;

    //! DEBUGGING ONLY: Print references held on object.
    inline  void            printRefs() const
    {
        getWeakRefs()->printRefs();
    }

    //! DEBUGGING ONLY: Enable tracking of object.
    inline  void            trackMe(bool enable, bool retain)
    {
        getWeakRefs()->trackMe(enable, retain);
    }

protected:
    RefBase();
    virtual                 ~RefBase();

    //! Flags for extendObjectLifetime()
    enum {
        OBJECT_LIFETIME_WEAK    = 0x0001,
        OBJECT_LIFETIME_FOREVER = 0x0003
    };

    void            extendObjectLifetime(int32_t mode);

    //! Flags for onIncStrongAttempted()
    enum {
        FIRST_INC_STRONG = 0x0001
    };

    virtual void            onFirstRef();
    virtual void            onLastStrongRef(const void* id);
    virtual bool            onIncStrongAttempted(uint32_t flags, const void* id);
    virtual void            onLastWeakRef(const void* id);

private:
    friend class weakref_type;
    class weakref_impl;

    RefBase(const RefBase& o);
    RefBase&        operator=(const RefBase& o);

    weakref_impl* const mRefs;
};
```

RefBase類和LightRefBase類一樣，提供了incStrong和decStrong成員函數來操作它的引用計數器；而RefBase類與LightRefBase類最大的區別是，它不像LightRefBase類一樣直接提供一個整型值（mutable volatile int32_t mCount）來維護對象的引用計數，前面我們說過，複雜的引用計數技術同時支持強引用計數和弱引用計數，在RefBase類中，這兩種計數功能是通過其成員變量mRefs來提供的。

RefBase類的成員變量mRefs的類型為weakref_impl指針，它實現在frameworks/base/libs/utils/RefBase.cpp文件中：

```cpp
class RefBase::weakref_impl : public RefBase::weakref_type
{
public:
    volatile int32_t    mStrong;
    volatile int32_t    mWeak;
    RefBase* const      mBase;
    volatile int32_t    mFlags;


#if !DEBUG_REFS

    weakref_impl(RefBase* base)
        : mStrong(INITIAL_STRONG_VALUE)
        , mWeak(0)
        , mBase(base)
        , mFlags(0)
    {
    }

    void addStrongRef(const void* /*id*/) { }
    void removeStrongRef(const void* /*id*/) { }
    void addWeakRef(const void* /*id*/) { }
    void removeWeakRef(const void* /*id*/) { }
    void printRefs() const { }
    void trackMe(bool, bool) { }

#else
    weakref_impl(RefBase* base)
        : mStrong(INITIAL_STRONG_VALUE)
        , mWeak(0)
        , mBase(base)
        , mFlags(0)
        , mStrongRefs(NULL)
        , mWeakRefs(NULL)
        , mTrackEnabled(!!DEBUG_REFS_ENABLED_BY_DEFAULT)
        , mRetain(false)
    {
        //LOGI("NEW weakref_impl %p for RefBase %p", this, base);
    }

    ~weakref_impl()
    {
        LOG_ALWAYS_FATAL_IF(!mRetain &&
                            mStrongRefs != NULL, "Strong references remain!");
        LOG_ALWAYS_FATAL_IF(!mRetain && mWeakRefs != NULL, "Weak references remain!");
    }

    void addStrongRef(const void* id)
    {
        addRef(&mStrongRefs, id, mStrong);
    }

    void removeStrongRef(const void* id)
    {
        if (!mRetain) {
            removeRef(&mStrongRefs, id);
        } else {
            addRef(&mStrongRefs, id, -mStrong);
        }
    }

    void addWeakRef(const void* id)
    {
        addRef(&mWeakRefs, id, mWeak);
    }
    void removeWeakRef(const void* id)
    {
        if (!mRetain) {
            removeRef(&mWeakRefs, id);
        } else {
            addRef(&mWeakRefs, id, -mWeak);
        }
    }

    void trackMe(bool track, bool retain)
    {
        mTrackEnabled = track;
        mRetain = retain;
    }

    ......

private:
    struct ref_entry {
        ref_entry* next;
        const void* id;
#if DEBUG_REFS_CALLSTACK_ENABLED
        CallStack stack;
#endif
        int32_t ref;
    };

    void addRef(ref_entry** refs, const void* id, int32_t mRef)
    {
        if (mTrackEnabled) {
            AutoMutex _l(mMutex);
            ref_entry* ref = new ref_entry;
            // Reference count at the time of the snapshot, but before the
            // update.  Positive value means we increment, negative--we
            // decrement the reference count.
            ref->ref = mRef;
            ref->id = id;
#if DEBUG_REFS_CALLSTACK_ENABLED
            ref->stack.update(2);
#endif

            ref->next = *refs;
            *refs = ref;
        }
    }

    void removeRef(ref_entry** refs, const void* id)
    {
        if (mTrackEnabled) {
            AutoMutex _l(mMutex);

            ref_entry* ref = *refs;

            while (ref != NULL) {
                if (ref->id == id) {
                    *refs = ref->next;
                    delete ref;
                    return;
                }

                refs = &ref->next;
                ref = *refs;
            }

            LOG_ALWAYS_FATAL("RefBase: removing id %p on RefBase %p (weakref_type %p) that doesn't exist!",
                             id, mBase, this);
        }
    }

    ......

    Mutex mMutex;
    ref_entry* mStrongRefs;
    ref_entry* mWeakRefs;

    bool mTrackEnabled;
    // Collect stack traces on addref and removeref, instead of deleting the stack references
    // on removeref that match the address ones.
    bool mRetain;

    ......
#endif
};
```
這個類看起來實現得很複雜，其實不然，這個類的實現可以分成兩部分：

```cpp
#if !DEBUG_REFS  
  
......  
  
#else 
```
編譯指令之間的這部分源代碼是Release版本的源代碼，它的成員函數都是空實現；

```sh
#else   
  
......  
  
#endif  
```
編譯指令之間的部分源代碼是Debug版本的源代碼，它的成員函數都是有實現的，實現這些函數的目的都是為了方便開發人員調試引用計數用的，除此之外，還在內部實現了一個結構體ref_entry：

```cpp
struct ref_entry {
    ref_entry* next;
    const void* id;
#if DEBUG_REFS_CALLSTACK_ENABLED
    CallStack stack;
#endif
    int32_t ref;
};
```

這個結構體也是為了方便調試而使用的，我們可以不關注這部分用於調試的代碼。

總的來說，weakref_impl類只要提供了以下四個成員變量來維護對象的引用計數：

```cpp
volatile int32_t    mStrong;  
volatile int32_t    mWeak;  
RefBase* const      mBase;  
volatile int32_t    mFlags;  
```

其中mStrong和mWeak分別表示對象的強引用計數和弱引用計數；RefBase類包含了一個weakref_impl類指針mRefs，而這裡的weakref_impl類也有一個成員變量mBase來指向它的宿主類RefBase；mFlags是一個標誌位，它指示了維護對象引用計數所使用的策略，後面我們將會分析到，它的取值為0，或者以下的枚舉值：

```cpp
//! Flags for extendObjectLifetime()
enum {
    OBJECT_LIFETIME_WEAK    = 0x0001,
    OBJECT_LIFETIME_FOREVER = 0x0003
};
```

這裡我們還需要注意的一點的是，從weakref_impl的類名來看，它應該是一個實現類，那麼，就必然有一個對應的接口類，這個對應的接口類的就是RefBase類內部定義的weakref_type類了，這是一種把類的實現與接口定義分離的設計方法。學習過設計模式的讀者應該知道，在設計模式裡面，非常強調類的接口定義和類的實現分離，以便利於後續擴展和維護，這裡就是用到了這種設計思想。
        
說了這多，RefBase類給人的感覺還是挺複雜的，不要緊，我們一步步來，先通過下面這個圖來梳理一下這些類之間的關係：


從這個類圖可以看出，每一個RefBase對象包含了一個weakref_impl對象，而weakref_impl對象實現了weakref_type接口，同時它可以包含多個ref_entry對象，前面說過，ref_entry是調試用的一個結構體，實際使用中可以不關注。

提供引用計數器的類RefBase我們就暫時介紹到這裡，後面我們再結合智能指針類一起分析，現在先來看看強指針類和弱指針類的定義。強指針類的定義我們在前面介紹輕量級指針的時候已經見到了，就是sp類了，這裡就不再把它的代碼列出來了。我們來看看它的構造函數的實現：


```cpp
template<typename T>
sp<T>::sp(T* other)
    : m_ptr(other)
{
    if (other) {
        other->incStrong(this);
    }
}
```

這裡傳進來的參數other一定是繼承於RefBase類的，因此，在函數的內部，它調用的是RefBase類的incStrong函數，它定義在frameworks/base/libs/utils/RefBase.cpp文件中：

```cpp
void RefBase::incStrong(const void* id) const
{
    weakref_impl* const refs = mRefs;
    refs->addWeakRef(id);
    refs->incWeak(id);
    refs->addStrongRef(id);

    const int32_t c = android_atomic_inc(&refs->mStrong);
    LOG_ASSERT(c > 0, "incStrong() called on %p after last strong ref", refs);

#if PRINT_REFS
    LOGD("incStrong of %p from %p: cnt=%d\n", this, id, c);
#endif

    if (c != INITIAL_STRONG_VALUE) {
        return;
    }

    android_atomic_add(-INITIAL_STRONG_VALUE, &refs->mStrong);
    const_cast<RefBase*>(this)->onFirstRef();
}
```

成員變量mRefs是在RefBase類的構造函數中創建的：

```cpp
RefBase::RefBase()
    : mRefs(new weakref_impl(this))
{
    //    LOGV("Creating refs %p with RefBase %p\n", mRefs, this);
}
```

在這個incStrong函數中，主要做了三件事情：
###一是增加弱引用計數：

```cpp
refs->addWeakRef(id);  
refs->incWeak(id);
```

###二是增加強引用計數：

```cpp
refs->addStrongRef(id);  
const int32_t c = android_atomic_inc(&refs->mStrong);  
```

###三是如果發現是首次調用這個對象的incStrong函數，就會調用一個這個對象的onFirstRef函數，讓對象有機會在對象被首次引用時做一些處理邏輯：

```cpp
if (c != INITIAL_STRONG_VALUE)
{
    return;
}
android_atomic_add(-INITIAL_STRONG_VALUE, &refs->mStrong);  
const_cast<RefBase*>(this)->onFirstRef();  
```

這裡的c返回的是refs->mStrong加1前的值，如果發現等於INITIAL_STRONG_VALUE，就說明這個對象的強引用計數是第一次被增加，因此，refs->mStrong就是初始化為INITIAL_STRONG_VALUE的，它的值為：

```cpp
#define INITIAL_STRONG_VALUE (1<<28)  
```

這個值加1後等於1<<28 + 1，不等於1，因此，後面要再減去-INITIAL_STRONG_VALUE，於是，refs->mStrong就等於1了，就表示當前對象的強引用計數值為1了，這與這個對象是第一次被增加強引用計數值的邏輯是一致的。

回過頭來看弱引用計數是如何增加的，首先是調用weakref_impl類的addWeakRef函數，我們知道，在Release版本中，這個函數也不做，而在Debug版本中，這個函數增加了一個ref_entry對象到了weakref_impl對象的mWeakRefs列表中，表示此weakref_impl對象的弱引用計數被增加了一次。接著又調用了weakref_impl類的incWeak函數，真正增加弱引用計數值就是在這個函數實現的了，weakref_impl類的incWeak函數繼承於其父類weakref_type的incWeak函數：

```cpp
void RefBase::weakref_type::incWeak(const void* id)
{
    weakref_impl* const impl = static_cast<weakref_impl*>(this);
    impl->addWeakRef(id);
    const int32_t c = android_atomic_inc(&impl->mWeak);
    LOG_ASSERT(c >= 0, "incWeak called on %p after last weak ref", this);
}
```

增加弱引用計數是下面語句執行的：

```cpp
const int32_t c = android_atomic_inc(&impl->mWeak);  
```

但是前面為什麼又調用了一次addWeakRef函數呢？前面不是已經調用過了嗎？在Release版本中，因為weakref_impl類的addWeakRef函數是空實現，這裡再調用一次沒有什麼害處，但是如果在Debug版本，豈不是冗餘了嗎？搞不清，有人問過負責開發Android系統Binder通信機制模塊的作者Dianne Hackborn這個問題，他是這樣回答的：
http://groups.google.com/group/android-platform/browse_thread/thread/cc641db8487dd83

Ah I see.  Well the debug code may be broken, though I wouldn't leap to that 
conclusion without actually testing it; I know it has been used in the 
past.  Anyway, these things get compiled out in non-debug builds, so there 
is no reason to change them unless you are actually trying to use this debug 
code and it isn't working and need to do this to fix it. 

既然他也不知道怎麼回事，我們也不必深究了，知道有這麼回事就行。

這裡總結一下強指針類sp在其構造函數裡面所做的事情就是分別為目標對象的強引用計數和弱引和計數增加了1。

再來看看強指針類的析構函數的實現：

```cpp
template<typename T>
sp<T>::~sp()
{
    if (m_ptr) {
        m_ptr->decStrong(this);
    }
}
```

同樣，這裡的m_ptr指向的目標對象一定是繼承了RefBase類的，因此，這裡調用的是RefBase類的decStrong函數，這也是定義在frameworks/base/libs/utils/RefBase.cpp文件中：

```cpp
void RefBase::decStrong(const void* id) const
{
    weakref_impl* const refs = mRefs;
    refs->removeStrongRef(id);
    const int32_t c = android_atomic_dec(&refs->mStrong);
#if PRINT_REFS
    LOGD("decStrong of %p from %p: cnt=%d\n", this, id, c);
#endif
    LOG_ASSERT(c >= 1, "decStrong() called on %p too many times", refs);

    if (c == 1) {
        const_cast<RefBase*>(this)->onLastStrongRef(id);

        if ((refs->mFlags & OBJECT_LIFETIME_WEAK) != OBJECT_LIFETIME_WEAK) {
            delete this;
        }
    }

    refs->removeWeakRef(id);
    refs->decWeak(id);
}
```

這裡的refs->removeStrongRef函數調用語句是對應前面在RefBase::incStrong函數裡的refs->addStrongRef函數調用語句的，在Release版本中，這也是一個空實現函數，真正實現強引用計數減1的操作是下面語句：

```cpp
const int32_t c = android_atomic_dec(&refs->mStrong);  
```

如果發現減1前，此對象的強引用計數為1，就說明從此以後，就再沒有地方引用這個目標對象了，這時候，就要看看是否要delete這個目標對象了：

```cpp
if (c == 1)
{
    const_cast<RefBase*>(this)->onLastStrongRef(id);

    if ((refs->mFlags & OBJECT_LIFETIME_WEAK) != OBJECT_LIFETIME_WEAK) {
        delete this;
    }
}
```
在強引用計數為0的情況下，如果對象的標誌位OBJECT_LIFETIME_WEAK被設置了，就說明這個對象的生命週期是受弱引用計數所控制的，因此，這時候就不能delete對象，要等到弱引用計數也為0的情況下，才能delete這個對象。

接下來的ref->removeWeakRef函數調用語句是對應前面在RefBase::incStrong函數裡的refs->addWeakRef函數調用語句的，在Release版本中，這也是一個空實現函數，真正實現強引用計數減1的操作下面的refs->decWeak函數，weakref_impl類沒有實現自己的decWeak函數，它繼承了weakref_type類的decWeak函數：


```cpp
void RefBase::weakref_type::decWeak(const void* id)
{
    weakref_impl* const impl = static_cast<weakref_impl*>(this);
    impl->removeWeakRef(id);
    const int32_t c = android_atomic_dec(&impl->mWeak);
    LOG_ASSERT(c >= 1, "decWeak called on %p too many times", this);

    if (c != 1) {
        return;
    }

    if ((impl->mFlags & OBJECT_LIFETIME_WEAK) != OBJECT_LIFETIME_WEAK) {
        if (impl->mStrong == INITIAL_STRONG_VALUE) {
            delete impl->mBase;
        } else {
            //            LOGV("Freeing refs %p of old RefBase %p\n", this, impl->mBase);
            delete impl;
        }
    } else {
        impl->mBase->onLastWeakRef(id);

        if ((impl->mFlags & OBJECT_LIFETIME_FOREVER) != OBJECT_LIFETIME_FOREVER) {
            delete impl->mBase;
        }
    }
}
```

 
這裡又一次調用了weakref_impl對象的removeWeakRef函數，這也是和RefBase::weakref_type::incWeak函數裡面的impl->addWeakRef語句所對應的，實現弱引用計數減1的操作是下面語句：

```cpp
const int32_t c = android_atomic_dec(&impl->mWeak);  
```

減1前如果發現不等於1，那麼就什麼也不用做就返回了，如果發現等於1，就說明當前對象的弱引用計數值為0了，這時候，就要看看是否要delete這個對象了：

```cpp

if ((impl->mFlags& OBJECT_LIFETIME_WEAK) != OBJECT_LIFETIME_WEAK)
{
    if (impl->mStrong == INITIAL_STRONG_VALUE) {
        delete impl->mBase;
    } else {
        //      LOGV("Freeing refs %p of old RefBase %p\n", this, impl->mBase);
        delete impl;
    }
} else
{
    impl->mBase->onLastWeakRef(id);

    if ((impl->mFlags & OBJECT_LIFETIME_FOREVER) != OBJECT_LIFETIME_FOREVER) {
        delete impl->mBase;
    }
}
```
 
如果目標對象的生命週期是不受弱引用計數控制的，就執行下面語句：

```cpp

if (impl->mStrong == INITIAL_STRONG_VALUE)
{
    delete impl->mBase;
} else
{
    //  LOGV("Freeing refs %p of old RefBase %p\n", this, impl->mBase);
    delete impl;
}
```
 
這個代碼段是什麼意思呢？這裡是減少對象的弱引用計數的地方，如果調用到這裡，那麼就說明前面一定有增加過此對象的弱引用計數，而增加對象的弱引用計數有兩種場景的，一種場景是增加對象的強引用計數的時候，會同時增加對象的弱引用計數，另一種場景是當我們使用一個弱指針來指向對象時，在弱指針對象的構造函數裡面，也會增加對象的弱引用計數，不過這時候，就只是增加對象的弱引用計數了，並沒有同時增加對象的強引用計數。因此，這裡在減少對象的弱引用計數時，就要分兩種情況來考慮。

如果是前一種場景，這裡的impl->mStrong就必然等於0，而不會等於INITIAL_STRONG_VALUE值，因此，這裡就不需要delete目標對象了（impl->mBase），因為前面的RefBase::decStrong函數會負責delete這個對象。這裡唯一需要做的就是把weakref_impl對象delete掉，但是，為什麼要在這裡delete這個weakref_impl對象呢？這裡的weakref_impl對象是在RefBase的構造函數裡面new出來的，理論上說應該在在RefBase的析構函數裡delete掉這個weakref_impl對象的。在RefBase的析構函數裡面，的確是會做這件事情：

```cpp
RefBase::~RefBase()
{
    //    LOGV("Destroying RefBase %p (refs %p)\n", this, mRefs);
    if (mRefs->mWeak == 0) {
        //        LOGV("Freeing refs %p of old RefBase %p\n", mRefs, this);
        delete mRefs;
    }
}
```

但是不要忘記，在這個場景下，目標對象是前面的RefBase::decStrong函數delete掉的，這時候目標對象就會被析構，但是它的弱引用計數值尚未執行減1操作，因此，這裡的mRefs->mWeak == 0條件就不成立，於是就不會delete這個weakref_impl對象，因此，就延遲到執行這裡decWeak函數時再執行。

如果是後一種情景，這裡的impl->mStrong值就等於INITIAL_STRONG_VALUE了，這時候由於沒有地方會負責delete目標對象，因此，就需要把目標對象（imp->mBase）delete掉了，否則就會造成內存洩漏。在delete這個目標對象的時候，就會執行RefBase類的析構函數，這時候目標對象的弱引用計數等於0，於是，就會把weakref_impl對象也一起delete掉了。

回到外層的if語句中，如果目標對象的生命週期是受弱引用計數控制的，就執行下面語句：

```cpp
impl->mBase->onLastWeakRef(id);

if ((impl->mFlags& OBJECT_LIFETIME_FOREVER) != OBJECT_LIFETIME_FOREVER)
{
    delete impl->mBase;
}
```

理論上說，如果目標對象的生命週期是受弱引用計數控制的，那麼當強引用計數和弱引用計數都為0的時候，這時候就應該delete目標對象了，但是這裡還有另外一層控制，我們可以設置目標對象的標誌值為OBJECT_LIFETIME_FOREVER，即目標對象的生命週期完全不受強引用計數和弱引用計數控制，在這種情況下，即使目標對象的強引用計數和弱引用計數都同時為0，這裡也不能delete這個目標對象，那麼，由誰來delete掉呢？當然是誰new出來的，就誰來delete掉了，這時候智能指針就完全退化為普通指針了，這裡的智能指針設計的非常強大。
分析到這裡，有必要小結一下：

A. 如果對象的標誌位被設置為0，那麼只要發現對象的強引用計數值為0，那就會自動delete掉這個對象；

B. 如果對象的標誌位被設置為OBJECT_LIFETIME_WEAK，那麼只有當對象的強引用計數和弱引用計數都為0的時候，才會自動delete掉這個對象；

C. 如果對象的標誌位被設置為OBJECT_LIFETIME_FOREVER，那麼對象就永遠不會自動被delete掉，誰new出來的對象誰來delete掉。

到了這裡，強指針就分析完成了，最後來分析弱指針。

###4. 弱指針

弱指針所使用的引用計數類與強指針一樣，都是RefBase類，因此，這裡就不再重複介紹了，我們直接來弱指針的實現，它定義在frameworks/base/include/utils/RefBase.h文件中：

```cpp
template <typename T>
class wp
{
public:
    typedef typename RefBase::weakref_type weakref_type;

    inline wp() : m_ptr(0) { }

    wp(T* other);
    wp(const wp<T>& other);
    wp(const sp<T>& other);
    template<typename U> wp(U* other);
    template<typename U> wp(const sp<U>& other);
    template<typename U> wp(const wp<U>& other);

    ~wp();

    // Assignment

    wp& operator = (T* other);
    wp& operator = (const wp<T>& other);
    wp& operator = (const sp<T>& other);

    template<typename U> wp& operator = (U* other);
    template<typename U> wp& operator = (const wp<U>& other);
    template<typename U> wp& operator = (const sp<U>& other);

    void set_object_and_refs(T* other, weakref_type* refs);

    // promotion to sp

    sp<T> promote() const;

    // Reset

    void clear();

    // Accessors

    inline  weakref_type* get_refs() const
    {
        return m_refs;
    }

    inline  T* unsafe_get() const
    {
        return m_ptr;
    }

    // Operators

    COMPARE_WEAK( ==)
    COMPARE_WEAK( !=)
    COMPARE_WEAK( >)
    COMPARE_WEAK( <)
    COMPARE_WEAK( <=)
    COMPARE_WEAK( >=)

    inline bool operator == (const wp<T>& o) const
    {
        return (m_ptr == o.m_ptr) && (m_refs == o.m_refs);
    }
    template<typename U>
    inline bool operator == (const wp<U>& o) const
    {
        return m_ptr == o.m_ptr;
    }

    inline bool operator > (const wp<T>& o) const
    {
        return (m_ptr == o.m_ptr) ? (m_refs > o.m_refs) : (m_ptr > o.m_ptr);
    }
    template<typename U>
    inline bool operator > (const wp<U>& o) const
    {
        return (m_ptr == o.m_ptr) ? (m_refs > o.m_refs) : (m_ptr > o.m_ptr);
    }

    inline bool operator < (const wp<T>& o) const
    {
        return (m_ptr == o.m_ptr) ? (m_refs < o.m_refs) : (m_ptr < o.m_ptr);
    }
    template<typename U>
    inline bool operator < (const wp<U>& o) const
    {
        return (m_ptr == o.m_ptr) ? (m_refs < o.m_refs) : (m_ptr < o.m_ptr);
    }
    inline bool operator != (const wp<T>& o) const
    {
        return m_refs != o.m_refs;
    }
    template<typename U> inline bool operator != (const wp<U>& o) const
    {
        return !operator == (o);
    }
    inline bool operator <= (const wp<T>& o) const
    {
        return !operator > (o);
    }
    template<typename U> inline bool operator <= (const wp<U>& o) const
    {
        return !operator > (o);
    }
    inline bool operator >= (const wp<T>& o) const
    {
        return !operator < (o);
    }
    template<typename U> inline bool operator >= (const wp<U>& o) const
    {
        return !operator < (o);
    }

private:
    template<typename Y> friend class sp;
    template<typename Y> friend class wp;

    T*              m_ptr;
    weakref_type*   m_refs;
};
```
  
與強指針類相比，它們都有一個成員變量m_ptr指向目標對象，但是弱指針還有一個額外的成員變量m_refs，它的類型是weakref_type指針，下面我們分析弱指針的構造函數時再看看它是如果初始化的。這裡我們需要關注的仍然是弱指針的構造函數和析構函數。
先來看構造函數：

```cpp
template<typename T>
wp<T>::wp(T* other)
    : m_ptr(other)
{
    if (other) {
        m_refs = other->createWeak(this);
    }
}
```

這裡的參數other一定是繼承了RefBase類，因此，這裡調用了RefBase類的createWeak函數，它定義在frameworks/base/libs/utils/RefBase.cpp文件中：

```cpp
RefBase::weakref_type* RefBase::createWeak(const void* id) const
{
    mRefs->incWeak(id);
    return mRefs;
}
```

這裡的成員變量mRefs的類型為weakref_impl指針，weakref_impl類的incWeak函數我們在前面已經看過了，它的作用就是增加對象的弱引用計數。函數最後返回mRefs，於是，弱指針對象的成員變量m_refs就指向目標對象的weakref_impl對象了。
再來看析構函數：

```cpp
template<typename T>
wp<T>::~wp()
{
    if (m_ptr) {
        m_refs->decWeak(this);
    }
}
```

這裡，弱指針在析構的時候，與強指針析構不一樣，它直接就調用目標對象的weakref_impl對象的decWeak函數來減少弱引用計數了，當弱引用計數為0的時候，就會根據在目標對象的標誌位（0、OBJECT_LIFETIME_WEAK或者OBJECT_LIFETIME_FOREVER）來決定是否要delete目標對象，前面我們已經介紹過了，這裡就不再介紹了。

分析到這裡，弱指針還沒介紹完，它最重要的特性我們還沒有分析到。前面我們說過，弱指針的最大特點是它不能直接操作目標對象，這是怎麼樣做到的呢？祕密就在於弱指針類沒有重載*和->操作符號，而強指針重載了這兩個操作符號。但是，如果我們要操作目標對象，應該怎麼辦呢，這就要把弱指針升級為強指針了：


```cpp
template<typename T>
sp<T> wp<T>::promote() const
{
    return sp<T>(m_ptr, m_refs);
}
```

升級的方式就使用成員變量m_ptr和m_refs來構造一個強指針sp，這裡的m_ptr為指目標對象的一個指針，而m_refs則是指向目標對象裡面的weakref_impl對象。
我們再來看看這個強指針的構造過程：

```cpp
template<typename T>
sp<T>::sp(T* p, weakref_type* refs)
    : m_ptr((p && refs->attemptIncStrong(this)) ? p : 0)
{
}
```

主要就是初始化指向目標對象的成員變量m_ptr了，如果目標對象還存在，這個m_ptr就指向目標對象，如果目標對象已經不存在，m_ptr就為NULL，升級成功與否就要看refs->attemptIncStrong函數的返回結果了：


```cpp
bool RefBase::weakref_type::attemptIncStrong(const void* id)
{
    incWeak(id);

    weakref_impl* const impl = static_cast<weakref_impl*>(this);

    int32_t curCount = impl->mStrong;
    LOG_ASSERT(curCount >= 0, "attemptIncStrong called on %p after underflow",
               this);

    while (curCount > 0 && curCount != INITIAL_STRONG_VALUE) {
        if (android_atomic_cmpxchg(curCount, curCount + 1, &impl->mStrong) == 0) {
            break;
        }

        curCount = impl->mStrong;
    }

    if (curCount <= 0 || curCount == INITIAL_STRONG_VALUE) {
        bool allow;

        if (curCount == INITIAL_STRONG_VALUE) {
            // Attempting to acquire first strong reference...  this is allowed
            // if the object does NOT have a longer lifetime (meaning the
            // implementation doesn't need to see this), or if the implementation
            // allows it to happen.
            allow = (impl->mFlags & OBJECT_LIFETIME_WEAK) != OBJECT_LIFETIME_WEAK
                    || impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);
        } else {
            // Attempting to revive the object...  this is allowed
            // if the object DOES have a longer lifetime (so we can safely
            // call the object with only a weak ref) and the implementation
            // allows it to happen.
            allow = (impl->mFlags & OBJECT_LIFETIME_WEAK) == OBJECT_LIFETIME_WEAK
                    && impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);
        }

        if (!allow) {
            decWeak(id);
            return false;
        }

        curCount = android_atomic_inc(&impl->mStrong);

        // If the strong reference count has already been incremented by
        // someone else, the implementor of onIncStrongAttempted() is holding
        // an unneeded reference.  So call onLastStrongRef() here to remove it.
        // (No, this is not pretty.)  Note that we MUST NOT do this if we
        // are in fact acquiring the first reference.
        if (curCount > 0 && curCount < INITIAL_STRONG_VALUE) {
            impl->mBase->onLastStrongRef(id);
        }
    }

    impl->addWeakRef(id);
    impl->addStrongRef(id);

#if PRINT_REFS
    LOGD("attemptIncStrong of %p from %p: cnt=%d\n", this, id, curCount);
#endif

    if (curCount == INITIAL_STRONG_VALUE) {
        android_atomic_add(-INITIAL_STRONG_VALUE, &impl->mStrong);
        impl->mBase->onFirstRef();
    }

    return true;
}

```

這個函數的作用是試圖增加目標對象的強引用計數，但是有可能會失敗，失敗的原因可能是因為目標對象已經被delete掉了，或者是其它的原因，下面會分析到。前面我們在討論強指針的時候說到，增加目標對象的強引用計數的同時，也會增加目標對象的弱引用計數，因此，函數在開始的地方首先就是調用incWeak函數來先增加目標對象的引用計數，如果後面試圖增加目標對象的強引用計數失敗時，會調用decWeak函數來回滾前面的incWeak操作。

這裡試圖增加目標對象的強引用計數時，分兩種情況討論，一種情況是此時目標對象正在被其它強指針引用，即它的強引用計數大於0，並且不等於INITIAL_STRONG_VALUE，另一種情況是此時目標對象沒有被任何強指針引用，即它的強引用計數小於等於0，或者等於INITIAL_STRONG_VALUE。

第一種情況比較簡單，因為這時候說明目標對象一定存在，因此，是可以將這個弱指針提升為強指針的，在這種情況下，只要簡單地增加目標對象的強引用計數值就行了：

```cpp
while (curCount > 0 && curCount != INITIAL_STRONG_VALUE)
{
    if (android_atomic_cmpxchg(curCount, curCount + 1, &impl->mStrong) == 0) {
        break;
    }

    curCount = impl->mStrong;
}
```

當我們在這裡對目標對象的強引用計數執行加1操作時，要保證原子性，因為其它地方也有可能正在對這個目標對象的強引用計數執行加1的操作，前面我們一般是調用android_atomic_inc函數來完成，但是這裡是通過調用android_atomic_cmpxchg函數來完成，android_atomic_cmpxchg函數是體系結構相關的函數，在提供了一些特殊的指令的體系結構上，調用android_atomic_cmpxchg函數來執行加1操作的效率會比調用android_atomic_inc函數更高一些。函數android_atomic_cmpxchg是在system/core/include/cutils/atomic.h文件中定義的一個宏：

```cpp
int android_atomic_release_cas(int32_t oldvalue, int32_t newvalue,  
volatile int32_t* addr);  

#define android_atomic_cmpxchg android_atomic_release_cas  
```

它實際執行的函數是android_atomic_release_cas，這個函數的工作原理大概是這樣的：如果它發現*addr == oldvalue，就會執行*addr = newvalue的操作，然後返回0，否則什麼也不做，返回1。在我們討論的這個場景中，oldvalue等於curCount，而newvalue等於curCount + 1，於是，在*addr == oldvalue的條件下，就相當於是對目標對象的強引用計數值增加了1。什麼情況下*addr != oldvalue呢？在調用android_atomic_release_cas函數之前，oldvalue和值就是從地址addr讀出來的，如果在執行android_atomic_release_cas函數的時候，有其它地方也對地址addr進行操作，那麼就會有可能出現*addr != oldvalue的情況，這時候就說明其它地方也在操作目標對象的強引用計數了，因此，這裡就不能執行增加目標對象的強引用計數的操作了，它必須要等到其它地方操作完目標對象的強引用計數之後再重新執行，這就是為什麼要通過一個while循環來執行了。

第二種情況比較複雜一點，因為這時候目標對象可能還存在，也可能不存了，這要根據實際情況來判斷。如果此時目標對象的強引用計數值等於INITIAL_STRONG_VALUE，說明此目標對象還從未被強指針引用過，這時候弱指針能夠被提升為強指針的條件就為：

```cpp
allow = (impl->mFlags&OBJECT_LIFETIME_WEAK) != OBJECT_LIFETIME_WEAK  
|| impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);  
```

即如果目標對象的生命週期只受到強引用計數控制或者在目標對象的具體實現中總是允許這種情況發生。怎麼理解呢？如果目標對象的生命週期只受強引用計數控制（它的標誌位mFlags為0），而這時目標對象又還未被強指針引用過，它自然就不會被delete掉，因此，這時候可以判斷出目標對象是存在的；如果目標對象的生命週期受弱引用計數控制（OBJECT_LIFETIME_WEAK），這時候由於目標對象正在被弱指針引用，因此，弱引用計數一定不為0，目標對象一定存在；如果目標對象的生命週期不受引用計數控制（BJECT_LIFETIME_FOREVER），這時候目標對象也是下在被弱指針引用，因此，目標對象的所有者必須保證這個目標對象還沒有被delete掉，否則就會出問題了。在後面兩種場景下，因為目標對象的生命週期都是不受強引用計數控制的，而現在又要把弱指針提升為強指針，就需要進一步調用目標對象的onIncStrongAttempted來看看是否允許這種情況發生，這又該怎麼理解呢？可以這樣理解，目標對象的設計者可能本身就不希望這個對象被強指針引用，只能通過弱指針來引用它，因此，這裡它就可以重載其父類的onIncStrongAttempted函數，然後返回false，這樣就可以阻止弱指針都被提升為強指針。在RefBase類中，其成員函數onIncStrongAttempted默認是返回true的：

```cpp
bool RefBase::onIncStrongAttempted(uint32_t flags, const void* id)
{
    return (flags & FIRST_INC_STRONG) ? true : false;
}
```

如果此時目標對象的強引用計數值小於等於0，那就說明該對象之前一定被強指針引用過，這時候就必須保證目標對象是被弱引用計數控制的（BJECT_LIFETIME_WEAK），否則的話，目標對象就已經被delete了。同樣，這裡也要調用一下目標對象的onIncStrongAttempted成員函數，來詢問一下目標對象在強引用計數值小於等於0的時候，是否允計將弱指針提升為強指針。下面這個代碼段就是執行上面所說的邏輯：

```cpp
allow = (impl->mFlags&OBJECT_LIFETIME_WEAK) == OBJECT_LIFETIME_WEAK  
&& impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id);  
```

繼續往下看：

```cpp
if (!allow)
{
    decWeak(id);
    return false;
}

curCount = android_atomic_inc(&impl->mStrong);  
```

如果allow值為false，那麼就說明不允計把這個弱指針提升為強指針，因此就返回false了，在返回之前，要先調用decWeak函數來減少目標對象的弱引用計數，因為函數的開頭不管三七二十一，首先就調用了incWeak來增加目標對象的弱引用計數值。
函數attemptIncStrong的主體邏輯大概就是這樣了，比較複雜，讀者要細細體會一下。函數的最後，如果此弱指針是允計提升為強指針的，並且此目標對象是第一次被強指針引用，還需要調整一下目標對象的強引用計數值：

```cpp
if (curCount == INITIAL_STRONG_VALUE)
{
    android_atomic_add(-INITIAL_STRONG_VALUE, &impl->mStrong);
    impl->mBase->onFirstRef();
}
```
 
這個邏輯我們在前面分析強指針時已經分析過了，這裡不再詳述。
分析到這裡，弱指針就介紹完了。強指針和弱指針的關係比較密切，同時它們也比較複雜，下面我們再舉一個例子來說明強指針和弱指針的用法，同時也驗證一下它們的實現原理。

###5. 強指針和弱指針的用法

參考在Ubuntu上為Android系統內置C可執行程序測試Linux內核驅動程序一文，我們在external目錄下建立一個C++工程目錄weightpointer，它裡面有兩個文件，一個weightpointer.cpp文件，另外一個是Android.mk文件。

源文件weightpointer.cpp的內容如下：

```cpp
#include <stdio.h>
#include <utils/RefBase.h>

#define INITIAL_STRONG_VALUE (1<<28)

using namespace android;

class WeightClass : public RefBase
{
public:
    void printRefCount()
    {
        int32_t strong = getStrongCount();
        weakref_type* ref = getWeakRefs();

        printf("-----------------------\n");
        printf("Strong Ref Count: %d.\n",
               (strong  == INITIAL_STRONG_VALUE ? 0 : strong));
        printf("Weak Ref Count: %d.\n", ref->getWeakCount());
        printf("-----------------------\n");
    }
};

class StrongClass : public WeightClass
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


class WeakClass : public WeightClass
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

class ForeverClass : public WeightClass
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

    sp<StrongClass> spOut = wpOut.promote();
    printf("spOut: %p.\n", spOut.get());
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
}


void TestForeverClass(ForeverClass* pForeverClass)
{
    wp<ForeverClass> wpOut = pForeverClass;
    pForeverClass->printRefCount();

    {
        sp<ForeverClass> spInner = pForeverClass;
        pForeverClass->printRefCount();
    }
}

int main(int argc, char** argv)
{
    printf("Test Strong Class: \n");
    StrongClass* pStrongClass = new StrongClass();
    TestStrongClass(pStrongClass);

    printf("\nTest Weak Class: \n");
    WeakClass* pWeakClass = new WeakClass();
    TestWeakClass(pWeakClass);

    printf("\nTest Froever Class: \n");
    ForeverClass* pForeverClass = new ForeverClass();
    TestForeverClass(pForeverClass);
    pForeverClass->printRefCount();
    delete pForeverClass;

    return 0;
}

```

首先定義了一個基類WeightClass，繼承於RefBase類，它只有一個成員函數printRefCount，作用是用來輸出引用計數。接著分別定義了三個類StrongClass、WeakClass和ForeverClass，其中實例化StrongClass類的得到的對象的標誌位為默認值0，實例化WeakClass類的得到的對象的標誌位為OBJECT_LIFETIME_WEAK，實例化ForeverClass類的得到的對象的標誌位為OBJECT_LIFETIME_FOREVER，後兩者都是通過調用RefBase類的extendObjectLifetime成員函數來設置的。

在main函數裡面，分別實例化了這三個類的對象出來，然後分別傳給TestStrongClass函數、TestWeakClass函數和TestForeverClass函數來說明智能指針的用法，我們主要是通過考察它們的強引用計數和弱引用計數來驗證智能指針的實現原理。

編譯腳本文件Android.mk的內容如下：

```sh
LOCAL_PATH := $(call my-dir)  
include $(CLEAR_VARS)  
LOCAL_MODULE_TAGS := optional  
LOCAL_MODULE := weightpointer  
LOCAL_SRC_FILES := weightpointer.cpp  
LOCAL_SHARED_LIBRARIES := \  
libcutils \  
libutils  
include $(BUILD_EXECUTABLE)  
```

最後，我們參照如何單獨編譯Android源代碼中的模塊一文，使用mmm命令對工程進行編譯：
```sh
USER-NAME@MACHINE-NAME:~/Android$ mmm ./external/weightpointer  
```

編譯之後，就可以打包了：

```sh
USER-NAME@MACHINE-NAME:~/Android$ make snod  
```
最後得到可執行程序weightpointer就位於設備上的/system/bin/目錄下。啟動模擬器，通過adb shell命令進入到模擬器終端，進入到/system/bin/目錄，執行weightpointer可執行程序，驗證程序是否按照我們設計的邏輯運行：

```sh
USER-NAME@MACHINE-NAME:~/Android$ adb shell  
root@android:/ # cd system/bin/          
root@android:/system/bin # ./weightpointer    
```

執行TestStrongClass函數的輸出為：

```sh
Test Strong Class:   
Construct StrongClass Object.  
-----------------------  
Strong Ref Count: 0.  
Weak Ref Count: 1.  
-----------------------  
-----------------------  
Strong Ref Count: 1.  
Weak Ref Count: 2.  
-----------------------  
Destory StrongClass Object.  
spOut: 0x0.  
```

在TestStrongClass函數裡面，首先定義一個弱批針wpOut指向從main函數傳進來的StrongClass對象，這時候我們可以看到StrongClass對象的強引用計數和弱引用計數值分別為0和1；接著在一個大括號裡面定義一個強指針spInner指向這個StrongClass對象，這時候我們可以看到StrongClass對象的強引用計數和弱引用計數值分別為1和2；當程序跳出了大括號之後，強指針spInner就被析構了，從上面的分析我們知道，強指針spInner析構時，會減少目標對象的強引用計數值，因為前面得到的強引用計數值為1，這裡減1後，就變為0了，又由於這個StrongClass對象的生命週期只受強引用計數控制，因此，這個StrongClass對象就被delete了，這一點可以從後面的輸出（「Destory StrongClass Object.」）以及試圖把弱指針wpOut提升為強指針時得到的對象指針為0x0得到驗證。

執行TestWeakClass函數的輸出為：

```sh
Test Weak Class:   
Construct WeakClass Object.  
-----------------------  
Strong Ref Count: 0.  
Weak Ref Count: 1.  
-----------------------  
-----------------------  
Strong Ref Count: 1.  
Weak Ref Count: 2.  
-----------------------  
-----------------------  
Strong Ref Count: 0.  
Weak Ref Count: 1.  
-----------------------  
spOut: 0xa528.  
Destory WeakClass Object.  
```

TestWeakClass函數和TestStrongClass函數的執行過程基本一樣，所不同的是當程序跳出大括號之後，雖然這個WeakClass對象的強引用計數值已經為0，但是由於它的生命週期同時受強引用計數和弱引用計數控制，而這時它的弱引用計數值大於0，因此，這個WeakClass對象不會被delete掉，這一點可以從後面試圖把弱批針wpOut提升為強指針時得到的對象指針不為0得到驗證。

執行TestForeverClass函數的輸出來：

```sh
Test Froever Class:   
Construct ForeverClass Object.  
-----------------------  
Strong Ref Count: 0.  
Weak Ref Count: 1.  
-----------------------  
-----------------------  
Strong Ref Count: 1.  
Weak Ref Count: 2.  
-----------------------  
````

當執行完TestForeverClass函數返回到main函數的輸出來：
```sh
-----------------------  
Strong Ref Count: 0.  
Weak Ref Count: 0.  
-----------------------  
Destory ForeverClass Object.  
```

這裡我們可以看出，雖然這個ForeverClass對象的強引用計數和弱引用計數值均為0了，但是它不自動被delete掉，雖然由我們手動地delete這個對象，它才會被析構，這是因為這個ForeverClass對象的生命週期是既不受強引用計數值控制，也不會弱引用計數值控制。
這樣，從TestStrongClass、TestWeakClass和TestForeverClass這三個函數的輸出就可以驗證了我們上面對Android系統的強指針和弱指針的實現原理的分析。

至此，Android系統的智能指針（輕量級指針、強指針和弱指針）的實現原理就分析完成了，它實現得很小巧但是很精緻，希望讀者可以通過實際操作細細體會一下。

