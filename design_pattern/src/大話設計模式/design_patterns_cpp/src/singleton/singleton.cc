#include "hungry_singleton.h"
#include "lazy_singleton.h"
#include "lock_singleton.h"

HungrySingleton* HungrySingleton::instance_ = new HungrySingleton;

LazySingleton* LazySingleton::instance_ = nullptr;

std::mutex LockSingleton::lock_for_new_;
LockSingleton* LockSingleton::instance_ = nullptr;
