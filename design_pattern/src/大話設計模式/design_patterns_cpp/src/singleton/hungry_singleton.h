#ifndef design_patterns_hungry_singleton_h
#define design_patterns_hungry_singleton_h

class HungrySingleton
{
public:
    static HungrySingleton* instance()
    {
        return instance_;
    }
protected:
    HungrySingleton() {}
    ~HungrySingleton() {}
    HungrySingleton(const HungrySingleton&) = delete;
    HungrySingleton& operator=(const HungrySingleton&) = delete;
private:
    static HungrySingleton* instance_;
};

#endif
