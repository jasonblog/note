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

#include "mindroid/net/SocketAddress.h"
#include "mindroid/net/Socket4Address.h"
#include "mindroid/net/Socket6Address.h"

#include <netdb.h>
#include <arpa/inet.h>

namespace mindroid {

sp<String> SocketAddress::getHostName() const {
    char host[NI_MAXHOST] = { 0 };
    if (mInetAddress != nullptr) {
        getnameinfo(mInetAddress->getPointer(), mInetAddress->getSize(), host, sizeof(host), nullptr, 0, 0);
    }
    return String::valueOf(host);
}

sp<SocketAddress> SocketAddress::getSocketAddress(uint16_t port) {
    return new Socket6Address(port);
}

sp<SocketAddress> SocketAddress::getSocketAddress(const sp<String>& host, uint16_t port) {
    if (host == nullptr) {
        return getSocketAddress(port);
    }
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, host->c_str(), &sa.sin_addr) != 0) {
        return new Socket4Address(host, port);
    }
    return new Socket6Address(host, port);
}

} /* namespace mindroid */
