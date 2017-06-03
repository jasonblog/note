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

#ifndef MINDROID_CONTEXT_H_
#define MINDROID_CONTEXT_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

class String;
class IBinder;
class ComponentName;
class SharedPreferences;
class Intent;
class ServiceConnection;
class Looper;
class PackageManager;
class File;

/**
 * Interface to global information about an application environment. This is an class whose
 * implementation is provided by the Mindroid system. It allows access to application-specific
 * resources and classes, as well as up-calls for application-level operations such as launching
 * services, etc.
 */
class Context :
        public Object {
public:
    static const sp<String> SERVICE_MANAGER;
    static const sp<String> PACKAGE_MANAGER;
    static const sp<String> ALARM_MANAGER;
    static const sp<String> POWER_MANAGER;
    static const sp<String> LOCATION_SERVICE;
    static const sp<String> AUDIO_SERVICE;
    static const sp<String> TELEPHONY_SERVICE;
    static const sp<String> MEDIA_PLAYER_SERVICE;
    static const sp<String> MESSAGE_BROKER;
    static const sp<String> VOICE_SERVICE;

    /**
     * File creation mode: the default mode.
     */
    static const int32_t MODE_PRIVATE = 0x0000;

    /**
     * File creation mode: for use with {@link #openFileOutput}, if the file already exists then
     * write data to the end of the existing file instead of erasing it.
     *
     * @see #openFileOutput
     */
    static const int32_t MODE_APPEND = 0x8000;

    /** Return PackageManager instance to find global package information. */
    virtual sp<PackageManager> getPackageManager() = 0;

    /**
     * Return the Looper for the main thread of the current process. This is the thread used to
     * dispatch calls to application components (services, etc).
     */
    virtual sp<Looper> getMainLooper() = 0;

    /** Return the name of this application's package. */
    virtual sp<String> getPackageName() = 0;

    /**
     * {@hide} Return the full path to the shared prefs file for the given prefs group name.
     *
     * <p>
     * Note: this is not generally useful for applications, since they should not be directly
     * accessing the file system.
     */
    virtual sp<File> getSharedPrefsFile(const sp<String>& name) = 0;

    /**
     * Retrieve and hold the contents of the preferences file 'name', returning a SharedPreferences
     * through which you can retrieve and modify its values. Only one instance of the
     * SharedPreferences object is returned to any callers for the same name, meaning they will see
     * each other's edits as soon as they are made.
     *
     * @param name Desired preferences file. If a preferences file by this name does not exist, it
     * will be created when you retrieve an editor (SharedPreferences.edit()) and then commit
     * changes (Editor.commit()).
     * @param mode Operating mode. Use 0 or {@link #MODE_PRIVATE} for the default operation.
     *
     * @return Returns the single SharedPreferences instance that can be used to retrieve and modify
     * the preference values.
     */
    virtual sp<SharedPreferences> getSharedPreferences(const char* name, int32_t mode) = 0;
    virtual sp<SharedPreferences> getSharedPreferences(const sp<String>& name, int32_t mode) = 0;

    /**
     * Return the handle to a system-level service by name. The class of the returned object varies
     * by the requested name.
     *
     * @param name Name of the system service.
     */
    virtual sp<IBinder> getSystemService(const char* name) = 0;
    virtual sp<IBinder> getSystemService(const sp<String>& name) = 0;

    /**
     * Request that a given application service be started. The Intent must contain the complete
     * class name of a specific service implementation to start. If this service is not already
     * running, it will be instantiated and started (creating a process for it if needed); if it is
     * running then it remains running.
     *
     * <p>
     * Every call to this method will result in a corresponding call to the target service's
     * {@link mindroid.app.Service#onStartCommand} method, with the <var>intent</var> given here.
     * This provides a convenient way to submit jobs to a service without having to bind and call on
     * to its interface.
     *
     * <p>
     * Using startService() overrides the default service lifetime that is managed by
     * {@link #bindService}: it requires the service to remain running until {@link #stopService} is
     * called, regardless of whether any clients are connected to it. Note that calls to
     * startService() are not nesting: no matter how many times you call startService(), a single
     * call to {@link #stopService} will stop it.
     *
     * <p>
     * The system attempts to keep running services around as much as possible.
     *
     * @param service Identifies the service to be started. The Intent must specify an explicit
     * component name to start. Additional values may be included in the Intent extras to supply
     * arguments along with this specific start call.
     *
     * @return If the service is being started or is already running, the {@link ComponentName} of
     * the actual service that was started is returned; else if the service does not exist null is
     * returned.
     *
     * @see #stopService
     * @see #bindService
     */
    virtual sp<ComponentName> startService(const sp<Intent>& service) = 0;

    /**
     * Request that a given application service be stopped. If the service is not running, nothing
     * happens. Otherwise it is stopped. Note that calls to startService() are not counted -- this
     * stops the service no matter how many times it was started.
     *
     * <p>
     * Note that if a stopped service still has {@link ServiceConnection} objects bound to it, it
     * will not be destroyed until all of these bindings are removed. See the
     * {@link mindroid.app.Service} documentation for more details on a service's lifecycle.
     *
     * @param service Description of the service to be stopped. The Intent must specify either an
     * explicit component name to start.
     *
     * @return If there is a service matching the given Intent that is already running, then it is
     * stopped and true is returned; else false is returned.
     *
     * @see #startService
     */
    virtual bool stopService(const sp<Intent>& service) = 0;

    /**
     * Connect to an application service, creating it if needed. This defines a dependency between
     * your service and the requested service. The given <var>conn</var> will receive the service
     * object when it is created and be told if it dies and restarts. The service will be considered
     * required by the system only for as long as the calling context exists. For example, if this
     * Context is an Service that is stopped, the service will not be required to continue running
     * until the Service is recreated.
     *
     * @param service Identifies the service to connect to. The Intent must specify an explicit
     * component name.
     * @param conn Receives information as the service is started and stopped. This must be a valid
     * ServiceConnection object; it must not be null.
     * @param flags Currently not used. Should be 0.
     * @return If you have successfully bound to the service, true is returned; false is returned if
     * the connection is not made so you will not receive the service object.
     *
     * @see #unbindService
     * @see #startService
     */
    virtual bool bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags) = 0;

    /**
     * Disconnect from an application service. You will no longer receive calls as the service is
     * restarted, and the service is now allowed to stop at any time.
     *
     * @param conn The connection interface previously supplied to bindService(). This parameter
     * must not be null.
     *
     * @see #bindService
     */
    virtual void unbindService(const sp<ServiceConnection>& conn) = 0;

    virtual void cleanup() {
    }
};

} /* namespace mindroid */

#endif /* MINDROID_CONTEXT_H_ */
