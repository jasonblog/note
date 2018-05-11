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
    virtual ~RefBase();//delete this时候会调用子类

    enum {
        OBJECT_LIFETIME_WEAK    = 0x0001,
        OBJECT_LIFETIME_FOREVER = 0x0003
    };

    void extendObjectLifetime(int  mode);

    enum {
        FIRST_INC_STRONG = 0x0001
    };

    virtual bool            onIncStrongAttempted(int flags,
            const void* id);//子类可以覆盖

private:
    //friend class weakref_type;
    class weakref_impl; //不能include，只能前向声明
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
    //相当于typedef  RefBase::weakref_type weakref_typ
    inline sp() : m_ptr(0) {  }
    sp(T* other);
    sp(const sp<T>& other);
    ~sp();

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

private:
    template<typename Y> friend class
        wp;//wp可以操作sp的私有变量，如构造函数
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
