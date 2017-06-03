/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2013 Daniel Himmelein
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mindroid/os/HandlerThread.h"
#include "mindroid/os/Looper.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"

namespace mindroid {

HandlerThread::HandlerThread() :
        Thread(),
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()),
        mLooper(nullptr) {
}

HandlerThread::HandlerThread(const char* name) :
        Thread(name),
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()),
        mLooper(nullptr) {
}

HandlerThread::HandlerThread(const sp<String>& name) :
        Thread(name),
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()),
        mLooper(nullptr) {
}

void HandlerThread::run() {
    Looper::prepare();
    mLock->lock();
    mLooper = Looper::myLooper();
    mCondition->signalAll();
    mLock->unlock();
    onLooperPrepared();
    Looper::loop();
}

sp<Looper> HandlerThread::getLooper() {
    AutoLock autoLock(mLock);
    if (!isAlive()) {
        return nullptr;
    }

    while (isAlive() && mLooper == nullptr) {
        mCondition->await();
    }
    return mLooper;
}

bool HandlerThread::quit() {
    sp<Looper> looper = getLooper();
    if (looper != nullptr) {
        looper->quit();
        return true;
    }
    return false;
}

} /* namespace mindroid */
