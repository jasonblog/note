/*
 * Copyright (C) 2017 E.S.R. Labs
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

#include "mindroid/net/Socket6Address.h"
#include "mindroid/net/InetAddress.h"
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace mindroid {

Socket6Address::Socket6Address(uint16_t port) :
        SocketAddress(port) {
    sockaddr_in6 socketAddress;
    ::memset(&socketAddress, 0, sizeof(socketAddress));
    socketAddress.sin6_family = AF_INET6;
    socketAddress.sin6_addr = in6addr_any;
    socketAddress.sin6_port = htons(port);
    mInetAddress = new InetAddress(socketAddress);
}

Socket6Address::Socket6Address(const sp<String>& host, uint16_t port) {
    struct addrinfo hints, *result;
    int status;

    ::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    status = ::getaddrinfo(host->c_str(), nullptr, &hints, &result);
    mIsUnresolved = (status != 0);

    if (!mIsUnresolved) {
        sockaddr_in6 socketAddress;
        ::memset(&socketAddress, 0, sizeof(socketAddress));
        socketAddress.sin6_family = AF_INET6;
        ::memcpy(&socketAddress.sin6_addr, &((struct sockaddr_in6 *) (result->ai_addr))->sin6_addr, sizeof(((struct sockaddr_in6 *) (result->ai_addr))->sin6_addr));
        socketAddress.sin6_port = htons(port);
        mInetAddress = new InetAddress(socketAddress);
        ::freeaddrinfo(result);
    }
}

uint16_t Socket6Address::getPort() const {
    if (mInetAddress == nullptr) {
        return 0;
    }
    return ntohs(((sockaddr_in6*)mInetAddress->getPointer())->sin6_port);
}

} /* namespace mindroid */

