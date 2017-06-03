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

#include "mindroid/net/Socket4Address.h"
#include "mindroid/net/InetAddress.h"
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace mindroid {

Socket4Address::Socket4Address(uint16_t port) :
        SocketAddress(port) {
    sockaddr_in socketAddress;
    ::memset(&socketAddress, 0, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddress.sin_port = htons(port);
    mInetAddress = new InetAddress(socketAddress);
}

Socket4Address::Socket4Address(const sp<String>& host, uint16_t port) {
    struct addrinfo hints, *result;
    int status;

    ::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    status = ::getaddrinfo(host->c_str(), nullptr, &hints, &result);
    mIsUnresolved = (status != 0);

    if (!mIsUnresolved) {
        sockaddr_in socketAddress;
        ::memset(&socketAddress, 0, sizeof(socketAddress));
        socketAddress.sin_family = AF_INET;
        socketAddress.sin_addr.s_addr = ((struct sockaddr_in *) (result->ai_addr))->sin_addr.s_addr;
        socketAddress.sin_port = htons(port);
        mInetAddress = new InetAddress(socketAddress);
        ::freeaddrinfo(result);
    }
}

uint16_t Socket4Address::getPort() const {
    if (mInetAddress == nullptr) {
        return 0;
    }
    return ntohs(((sockaddr_in*)mInetAddress->getPointer())->sin_port);
}

} /* namespace mindroid */
