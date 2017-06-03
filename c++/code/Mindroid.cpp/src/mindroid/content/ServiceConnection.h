/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2013 Daniel Himmelein
 * Copyright (C) 2016 E.S.R.Labs
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

#ifndef MINDROID_SERVICECONNECTION_H_
#define MINDROID_SERVICECONNECTION_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

class ComponentName;
class IBinder;

/**
 * Interface for monitoring the state of an application service. See {@link mindroid.app.Service}
 * and {@link Context#bindService Context.bindService()} for more information.
 * <p>
 * Like many callbacks from the system, the methods on this class are called from the main thread of
 * your process.
 */
class ServiceConnection :
        public Object {
public:
    /**
     * Called when a connection to the Service has been established, with the
     * {@link mindroid.os.IBinder} of the communication channel to the Service.
     *
     * @param name The concrete component name of the service that has been connected.
     *
     * @param service The IBinder of the Service's communication channel, which you can now make
     * calls on.
     */
    virtual void onServiceConnected(const sp<ComponentName>& name, const sp<IBinder>& service) = 0;

    /**
     * Called when a connection to the Service has been lost. This typically happens when the
     * process hosting the service has crashed or been killed. This does <em>not</em> remove the
     * ServiceConnection itself -- this binding to the service will remain active, and you will
     * receive a call to {@link #onServiceConnected} when the Service is next running.
     *
     * @param name The concrete component name of the service whose connection has been lost.
     */
    virtual void onServiceDisconnected(const sp<ComponentName>& name) = 0;
};

} /* namespace mindroid */

#endif /* MINDROID_SERVICECONNECTION_H_ */
