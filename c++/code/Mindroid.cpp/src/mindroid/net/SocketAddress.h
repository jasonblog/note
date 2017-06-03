/*
 * Copyright (C) 2012 Daniel Himmelein
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

#ifndef MINDROID_SOCKETADDRESS_H_
#define MINDROID_SOCKETADDRESS_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"
#include "mindroid/net/InetAddress.h"

#include <netinet/in.h>

namespace mindroid {

class SocketAddress :
        public Object {
public:
    virtual ~SocketAddress() = default;
    static sp<SocketAddress> getSocketAddress(uint16_t port);

    static sp<SocketAddress> getSocketAddress(const char* host, uint16_t port) {
        if (host == nullptr) {
            return getSocketAddress(port);
        }
        return getSocketAddress(String::valueOf(host), port);
    }

    static sp<SocketAddress> getSocketAddress(const sp<String>& host, uint16_t port);

    sp<String> getHostName() const;
    virtual uint16_t getPort() const = 0;
    bool isUnresolved() const { return mIsUnresolved; }
    virtual sp<InetAddress> getInetAddress() const { return mInetAddress; }

protected:
    SocketAddress() = default;
    SocketAddress(uint16_t port) :
        mIsUnresolved(false) {
    }

    SocketAddress(const char* host, uint16_t port) {
    }

    SocketAddress(const sp<String>& host, uint16_t port) {
    }

    SocketAddress(const SocketAddress&) = delete;
    SocketAddress& operator=(const SocketAddress&) = delete;

    sp<InetAddress> mInetAddress = nullptr;
    bool mIsUnresolved = true;

    friend class ServerSocket;
    friend class Socket;
    friend class DatagramSocket;
};

} /* namespace mindroid */

#endif /* MINDROID_SOCKETADDRESS_H_ */
