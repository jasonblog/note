/*
 * Copyright (C) 2011 Daniel Himmelein
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

#include "mindroid/util/concurrent/SerialExecutor.h"
#include "mindroid/os/Handler.h"
#include "mindroid/os/Message.h"

namespace mindroid {

SerialExecutor::SerialExecutor() :
        mHandler(nullptr) {
    mHandlerThread = new HandlerThread("SerialExecutor");
    mHandlerThread->start();
    mHandler = new Handler(mHandlerThread->getLooper());
}

SerialExecutor::~SerialExecutor() {
    mHandlerThread->getLooper()->quit();
    mHandlerThread->join();
}

void SerialExecutor::execute(const sp<Runnable>& runnable) {
    mHandler->obtainMessage(runnable)->sendToTarget();
}

bool SerialExecutor::cancel(const sp<Runnable>& runnable) {
    return mHandler->removeCallbacks(runnable);
}

} /* namespace mindroid */
