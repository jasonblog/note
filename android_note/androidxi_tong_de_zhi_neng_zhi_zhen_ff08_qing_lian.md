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



