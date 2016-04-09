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

