/*
 * Copyright (C) 2016 Daniel Himmelein
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

#include "mindroid/os/IServiceManager.h"
#include "mindroid/os/IRemoteCallback.h"

namespace mindroid {
namespace binder {

const char* const ServiceManager::Stub::DESCRIPTOR = "mindroid.os.IServiceManager";

void ServiceManager::Stub::onTransact(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj, const sp<Bundle>& data, const sp<Object>& result) {
    switch (what) {
    case MSG_START_SERVICE: {
        auto promise = object_cast<Promise<sp<ComponentName>>>(result);
        sp<Intent> service = object_cast<Intent>(obj);
        sp<ComponentName> component = startService(service);
        promise->set(component);
        break;
    }
    case MSG_STOP_SERVICE: {
        auto promise = object_cast<Promise<bool>>(result);
        sp<Intent> service = object_cast<Intent>(obj);
        promise->set(stopService(service));
        break;
    }
    case MSG_BIND_SERVICE: {
        auto promise = object_cast<Promise<bool>>(result);
        sp<Intent> service = object_cast<Intent>(data->getObject("intent"));
        sp<ServiceConnection> conn = object_cast<ServiceConnection>(data->getObject("conn"));
        int32_t flags = data->getInt("flags");
        sp<IBinder> binder = data->getBinder("binder");
        promise->set(bindService(service, conn, flags, binder::RemoteCallback::Stub::asInterface(binder)));
        break;
    }
    case MSG_UNBIND_SERVICE: {
        sp<Intent> intent = object_cast<Intent>(data->getObject("intent"));
        sp<ServiceConnection> conn = object_cast<ServiceConnection>(data->getObject("conn"));
        sp<IBinder> binder = nullptr;
        if (binder == nullptr) {
            unbindService(intent, conn);
        } else {
            unbindService(intent, conn, binder::RemoteCallback::Stub::asInterface(binder));
        }
        break;
    }
    case MSG_START_SYSTEM_SERVICE: {
        auto promise = object_cast<Promise<sp<ComponentName>>>(result);
        sp<Intent> service = object_cast<Intent>(obj);
        sp<ComponentName> component = startSystemService(service);
        promise->set(component);
        break;
    }
    case MSG_STOP_SYSTEM_SERVICE: {
        auto promise = object_cast<Promise<bool>>(result);
        sp<Intent> service = object_cast<Intent>(obj);
        promise->set(stopSystemService(service));
        break;
    }
    default:
        Binder::onTransact(what, arg1, arg2, obj, data, result);
    }
}

sp<ComponentName> ServiceManager::Stub::Proxy::startService(const sp<Intent>& service) {
    sp<Promise<sp<ComponentName>>> promise = new Promise<sp<ComponentName>>();
    mRemote->transact(MSG_START_SERVICE, object_cast<Object>(service), promise, 0);
    return promise->get();
}

bool ServiceManager::Stub::Proxy::stopService(const sp<Intent>& service) {
    sp<Promise<bool>> promise = new Promise<bool>();
    mRemote->transact(MSG_STOP_SERVICE, object_cast<Object>(service), promise, 0);
    return promise->get();
}

bool ServiceManager::Stub::Proxy::bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) {
    sp<Promise<bool>> promise = new Promise<bool>();
    sp<Bundle> data = new Bundle();
    data->putObject("intent", service);
    data->putObject("conn", conn);
    data->putInt("flags", flags);
    data->putBinder("binder", callback->asBinder());
    mRemote->transact(MSG_BIND_SERVICE, data, promise, 0);
    return promise->get();
}

void ServiceManager::Stub::Proxy::unbindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn) {
    sp<Bundle> data = new Bundle();
    data->putObject("intent", intent);
    data->putObject("conn", conn);
    mRemote->transact(MSG_UNBIND_SERVICE, data, nullptr, FLAG_ONEWAY);
}

void ServiceManager::Stub::Proxy::unbindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn, const sp<IRemoteCallback>& callback) {
    sp<Bundle> data = new Bundle();
    data->putObject("intent", intent);
    data->putObject("conn", conn);
    data->putBinder("binder", callback->asBinder());
    mRemote->transact(MSG_UNBIND_SERVICE, data, nullptr, FLAG_ONEWAY);
}

sp<ComponentName> ServiceManager::Stub::Proxy::startSystemService(const sp<Intent>& service) {
    sp<Promise<sp<ComponentName>>> promise = new Promise<sp<ComponentName>>();
    mRemote->transact(MSG_START_SYSTEM_SERVICE, object_cast<Object>(service), promise, 0);
    return promise->get();
}

bool ServiceManager::Stub::Proxy::stopSystemService(const sp<Intent>& service) {
    sp<Promise<bool>> promise = new Promise<bool>();
    mRemote->transact(MSG_STOP_SYSTEM_SERVICE, object_cast<Object>(service), promise, 0);
    return promise->get();
}

ServiceManager::Stub::SmartProxy::SmartProxy(const sp<IBinder>& remote) {
    mRemote = remote;
    mStub = interface_cast<IServiceManager>(remote->queryLocalInterface(DESCRIPTOR));
    mProxy = new ServiceManager::Stub::Proxy(remote);
}

sp<ComponentName> ServiceManager::Stub::SmartProxy::startService(const sp<Intent>& service) {
    if (mRemote->runsOnSameThread()) {
        return mStub->startService(service);
    } else {
        return mProxy->startService(service);
    }
}

bool ServiceManager::Stub::SmartProxy::stopService(const sp<Intent>& service) {
    if (mRemote->runsOnSameThread()) {
        return mStub->stopService(service);
    } else {
        return mProxy->stopService(service);
    }
}

bool ServiceManager::Stub::SmartProxy::bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) {
    if (mRemote->runsOnSameThread()) {
        return mStub->bindService(service, conn, flags, callback);
    } else {
        return mProxy->bindService(service, conn, flags, callback);
    }
}

void ServiceManager::Stub::SmartProxy::unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn) {
    if (mRemote->runsOnSameThread()) {
        mStub->unbindService(service, conn);
    } else {
        mProxy->unbindService(service, conn);
    }
}

void ServiceManager::Stub::SmartProxy::unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, const sp<IRemoteCallback>& callback) {
    if (mRemote->runsOnSameThread()) {
        mStub->unbindService(service, conn, callback);
    } else {
        mProxy->unbindService(service, conn, callback);
    }
}

sp<ComponentName> ServiceManager::Stub::SmartProxy::startSystemService(const sp<Intent>& service) {
    if (mRemote->runsOnSameThread()) {
        return mStub->startSystemService(service);
    } else {
        return mProxy->startSystemService(service);
    }
}

bool ServiceManager::Stub::SmartProxy::stopSystemService(const sp<Intent>& service) {
    if (mRemote->runsOnSameThread()) {
        return mStub->stopSystemService(service);
    } else {
        return mProxy->stopSystemService(service);
    }
}

} /* namespace binder */
} /* namespace mindroid */
