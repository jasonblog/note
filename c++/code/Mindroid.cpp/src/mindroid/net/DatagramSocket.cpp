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

#include "mindroid/net/DatagramSocket.h"
#include "mindroid/net/SocketAddress.h"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

namespace mindroid {

DatagramSocket::DatagramSocket() :
        mSocketId(-1),
        mIsBound(false),
        mIsClosed(false) {
}

DatagramSocket::DatagramSocket(uint16_t port) :
        mSocketId(-1),
        mIsBound(false),
        mIsClosed(false) {
    bind(port);
}

DatagramSocket::DatagramSocket(const sp<String>& host, uint16_t port) :
        mSocketId(-1),
        mIsBound(false),
        mIsClosed(false) {
    bind(host, port);
}

DatagramSocket::~DatagramSocket() {
    close();
}

bool DatagramSocket::bind(uint16_t port) {
    if (mIsBound || mSocketId != -1 ) {
        return false;
    }
    mSocketId = ::socket(AF_INET6, SOCK_DGRAM, 0);
    int optval = 0;
    ::setsockopt(mSocketId, SOL_SOCKET, IPV6_V6ONLY, &optval, sizeof(optval));
    sp<SocketAddress> socketAddress = SocketAddress::getSocketAddress(port);
    if (socketAddress->getInetAddress() != nullptr && mSocketId != -1) {
        mIsBound = ::bind(mSocketId, socketAddress->getInetAddress()->getPointer(), socketAddress->getInetAddress()->getSize()) == 0;
    }
    return mIsBound;
}

bool DatagramSocket::bind(const sp<String>& host, uint16_t port) {
    if (mIsBound) {
        return false;
    }
    sp<SocketAddress> socketAddress = SocketAddress::getSocketAddress(host, port);
    if (socketAddress->getInetAddress() != nullptr) {
        mSocketId = ::socket(socketAddress->getInetAddress()->isInet6Address() ? AF_INET6 : AF_INET, SOCK_DGRAM, 0);
        if (mSocketId != -1) {
            mIsBound = ::bind(mSocketId, socketAddress->getInetAddress()->getPointer(), socketAddress->getInetAddress()->getSize()) == 0;
        }
    }
    return mIsBound;
}

ssize_t DatagramSocket::recv(uint8_t* data, size_t size) {
    return ::recvfrom(mSocketId, reinterpret_cast<char*>(data), size, 0, nullptr, 0);
}

ssize_t DatagramSocket::recv(uint8_t* data, size_t size, sp<SocketAddress>& sender) {
    ssize_t result = 0;
    socklen_t socketSize = sender->getInetAddress()->getSize();
    if (sender->getInetAddress() != nullptr) {
        result = ::recvfrom(mSocketId, reinterpret_cast<char*>(data), size, 0,
                (sockaddr*)sender->getInetAddress()->getPointer(), &socketSize);
        sender->mIsUnresolved = false;
    }
    return result;
}

bool DatagramSocket::send(const void* data, size_t size, const sp<SocketAddress>& receiver) {
    if (receiver->getInetAddress() != nullptr) {
        if (mSocketId == -1) {
            mSocketId = ::socket(receiver->getInetAddress()->isInet6Address() ? AF_INET6 : AF_INET, SOCK_DGRAM, 0);
        }
        socklen_t socketSize = receiver->getInetAddress()->getSize();
        return (size_t) ::sendto(mSocketId, reinterpret_cast<const char*>(data), size, 0,
                receiver->getInetAddress()->getPointer(), socketSize) == size;
    }
    return false;
}

void DatagramSocket::close() {
    mIsClosed = true;
    mIsBound = false;
    if (mSocketId != -1) {
        ::shutdown(mSocketId, SHUT_RDWR);
        ::close(mSocketId);
        mSocketId = -1;
    }
}

} /* namespace mindroid */
