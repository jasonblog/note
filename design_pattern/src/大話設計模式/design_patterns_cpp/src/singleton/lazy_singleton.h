#ifndef design_patterns_lazy_singleton_h
#define design_patterns_lazy_singleton_h

class LazySingleton
{
public:
    static LazySingleton* instance()
    {
        if (instance_ == nullptr) {
            instance_ = new LazySingleton;
        }

        return instance_;
    }
protected:
    LazySingleton() {}
    ~LazySingleton() {}
    LazySingleton(const LazySingleton&) = delete;
    LazySingleton& operator=(const LazySingleton&) = delete;
private:
    static LazySingleton* instance_;
};

#endif
