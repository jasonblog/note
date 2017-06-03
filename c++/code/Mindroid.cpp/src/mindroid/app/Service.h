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

#ifndef MINDROID_SERVICE_H_
#define MINDROID_SERVICE_H_

#include "mindroid/content/ContextWrapper.h"
#include "mindroid/os/IProcess.h"

namespace mindroid {

class ComponentName;

/**
 * A Service is an application component representing either an application's desire to perform a
 * longer-running operation or to supply functionality for other applications to use. Each service
 * class must have a corresponding service declaration in its package's
 * <code>MindroidManifest.xml</code>. Services can be started with
 * {@link mindroid.content.Context#startService Context.startService()} and
 * {@link mindroid.content.Context#bindService Context.bindService()}.
 *
 * <p>
 * Note that services, like other application objects, run in the main thread of their hosting
 * process. This means that, if your service is going to do any CPU intensive or blocking (such as
 * networking) operations, it should spawn its own thread in which to do that work. More information
 * on this can be found in <a href="{@docRoot}
 * guide/topics/fundamentals/processes-and-threads.html">Processes and Threads</a>.
 * </p>
 *
 * <p>
 * Topics covered here:
 * <ol>
 * <li><a href="#WhatIsAService">What is a Service?</a>
 * <li><a href="#ServiceLifecycle">Service Lifecycle</a>
 * <li><a href="#Permissions">Permissions</a>
 * <li><a href="#ProcessLifecycle">Process Lifecycle</a>
 * <li><a href="#LocalServiceSample">Local Service Sample</a>
 * <li><a href="#RemoteMessengerServiceSample">Remote Messenger Service Sample</a>
 * </ol>
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>
 * For a detailed discussion about how to create services, read the <a href="{@docRoot}
 * guide/topics/fundamentals/services.html">Services</a> developer guide.
 * </p>
 * </div>
 *
 * <a name="WhatIsAService"></a> <h3>What is a Service?</h3>
 *
 * <p>
 * Most confusion about the Service class actually revolves around what it is <em>not</em>:
 * </p>
 *
 * <ul>
 * <li>A Service is <b>not</b> a separate process. The Service object itself does not imply it is
 * running in its own process; unless otherwise specified, it runs in the same process as the
 * application it is part of.
 * <li>A Service is <b>not</b> a thread. It is not a means itself to do work off of the main thread
 * (to avoid Application Not Responding errors).
 * </ul>
 *
 * <p>
 * Thus a Service itself is actually very simple, providing two main features:
 * </p>
 *
 * <ul>
 * <li>A facility for the application to tell the system <em>about</em> something it wants to be
 * doing in the background (even when the user is not directly interacting with the application).
 * This corresponds to calls to {@link mindroid.content.Context#startService Context.startService()}
 * , which ask the system to schedule work for the service, to be run until the service or someone
 * else explicitly stop it.
 * <li>A facility for an application to expose some of its functionality to other applications. This
 * corresponds to calls to {@link mindroid.content.Context#bindService Context.bindService()}, which
 * allows a long-standing connection to be made to the service in order to interact with it.
 * </ul>
 *
 * <p>
 * When a Service component is actually created, for either of these reasons, all that the system
 * actually does is instantiate the component and call its {@link #onCreate} and any other
 * appropriate callbacks on the main thread. It is up to the Service to implement these with the
 * appropriate behavior, such as creating a secondary thread in which it does its work.
 * </p>
 *
 * <a name="ServiceLifecycle"></a> <h3>Service Lifecycle</h3>
 *
 * <p>
 * There are two reasons that a service can be run by the system. If someone calls
 * {@link mindroid.content.Context#startService Context.startService()} then the system will
 * retrieve the service (creating it and calling its {@link #onCreate} method if needed) and then
 * call its {@link #onStartCommand} method with the arguments supplied by the client. The service
 * will at this point continue running until {@link mindroid.content.Context#stopService
 * Context.stopService()} or {@link #stopSelf()} is called. Note that multiple calls to
 * Context.startService() do not nest (though they do result in multiple corresponding calls to
 * onStartCommand()), so no matter how many times it is started a service will be stopped once
 * Context.stopService() or stopSelf() is called; however, services can use their
 * {@link #stopSelf(int)} method to ensure the service is not stopped until started intents have
 * been processed.
 *
 * <p>
 * Clients can also use {@link mindroid.content.Context#bindService Context.bindService()} to obtain
 * a persistent connection to a service. This likewise creates the service if it is not already
 * running (calling {@link #onCreate} while doing so), but does not call onStartCommand(). The
 * client will receive the {@link mindroid.os.IBinder} object that the service returns from its
 * {@link #onBind} method, allowing the client to then make calls back to the service. The service
 * will remain running as long as the connection is established (whether or not the client retains a
 * reference on the service's IBinder).
 *
 * <p>
 * A service can be both started and have connections bound to it. In such a case, the system will
 * keep the service running as long as either it is started <em>or</em> there are one or more
 * connections to it. Once neither of these situations hold, the service's {@link #onDestroy} method
 * is called and the service is effectively terminated. All cleanup (stopping threads, unregistering
 * receivers) should be complete upon returning from onDestroy().
 *
 * <a name="ProcessLifecycle"></a>
 * <h3>Process Lifecycle</h3>
 *
 * <p>
 * The Mindroid system will attempt to keep the process hosting a service around as long as the
 * service has been started or has clients bound to it.
 */
class Service :
        public ContextWrapper {
public:
    Service() : ContextWrapper(nullptr) {
    }

    virtual ~Service() = default;

    /**
     * Called by the system when the service is first created. Do not call this method directly.
     */
    virtual void onCreate() {
    }

    /**
     * Called by the system every time a client explicitly starts the service by calling
     * {@link mindroid.content.Context#startService}, providing the arguments it supplied and a
     * unique integer token representing the start request. Do not call this method directly.
     *
     * @param intent The Intent supplied to {@link mindroid.content.Context#startService}, as given.
     * @param flags Additional data about this start request. Currently this is always 0.
     * @param startId A unique integer representing this specific request to start.
     *
     * @return The return value should always be 0.
     */
    virtual int32_t onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) {
        return 0;
    }

    /**
     * Called by the system to notify a Service that it is no longer used and is being removed. The
     * service should clean up any resources it holds (threads, registered receivers, etc) at this
     * point. Upon return, there will be no more calls in to this Service object and it is
     * effectively dead. Do not call this method directly.
     */
    virtual void onDestroy() {
    }

    /**
     * Return the communication channel to the service. May return null if clients can not bind to
     * the service. The returned {@link mindroid.os.IBinder} is usually for a complex interface.
     *
     * @param intent The Intent that was used to bind to this service, as given to
     * {@link mindroid.content.Context#bindService Context.bindService}. Note that any extras that
     * were included with the Intent at that point will <em>not</em> be seen here.
     *
     * @return Return an IBinder through which clients can call on to the service.
     */
    virtual sp<IBinder> onBind(const sp<Intent>& intent) = 0;

    /**
     * Called when all clients have disconnected from a particular interface published by the
     * service. The default implementation does nothing and returns false.
     *
     * @param intent The Intent that was used to bind to this service, as given to
     * {@link mindroid.content.Context#bindService Context.bindService}. Note that any extras that
     * were included with the Intent at that point will <em>not</em> be seen here.
     *
     * @return Should always return true.
     */
    virtual bool onUnbind(const sp<Intent>& intent) {
        return true;
    }

    /**
     * Stop the service, if it was previously started.  This is the same as
     * calling {@link mindroid.content.Context#stopService} for this particular service.
     */
    void stopSelf() {
        stopSelf(-1);
    }

    /**
     * Stop the service, if it was previously started.
     */
    void stopSelf(int32_t startId);

    /**
     * @hide
     */
    void attach(const sp<Context>& context, const sp<IProcess>& process, const sp<String>& className);

private:
    sp<IProcess> mProcess;
    sp<String> mClassName;
};

} /* namespace mindroid */

#endif /* MINDROID_SERVICE_H_ */
