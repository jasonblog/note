#ifndef design_patterns_lock_singleton_h
#define design_patterns_lock_singleton_h

#include <thread>
#include <mutex>

class LockSingleton
{
public:
    static LockSingleton* instance()
    {
        if (instance_ == nullptr) {
            lock_for_new_.lock();

            if (instance_ == nullptr) {
                instance_ = new LockSingleton;
            }

            lock_for_new_.unlock();
        }

        return instance_;
    }
protected:
    LockSingleton() {}
    ~LockSingleton() {}
    LockSingleton(const LockSingleton&) = delete;
    LockSingleton& operator=(const LockSingleton&) = delete;
private:
    static LockSingleton* instance_;
    static std::mutex lock_for_new_;
};

#endif
