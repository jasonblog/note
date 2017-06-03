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

#ifndef MINDROID_SERVERSOCKET_H_
#define MINDROID_SERVERSOCKET_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"

namespace mindroid {

class Socket;

class ServerSocket :
        public Object {
public:
    static const int DEFAULT_BACKLOG = 10;

    ServerSocket();
    ServerSocket(uint16_t port);
    ServerSocket(uint16_t port, int backlog);
    ServerSocket(const char* host, uint16_t port, int backlog) :
            ServerSocket(String::valueOf(host), port, backlog) {
    }
    ServerSocket(const sp<String>& host, uint16_t port, int backlog);
    virtual ~ServerSocket();
    ServerSocket(const ServerSocket&) = delete;
    ServerSocket& operator=(const ServerSocket&) = delete;

    bool bind(uint16_t port, int backlog = DEFAULT_BACKLOG);
    bool bind(const char* host, uint16_t port, int backlog = DEFAULT_BACKLOG) {
        return bind(String::valueOf(host), port, backlog);
    }
    bool bind(const sp<String>& host, uint16_t port, int backlog = DEFAULT_BACKLOG);

    sp<Socket> accept();
    void close();
    bool isBound() const { return mIsBound; }
    bool isClosed() const { return mIsClosed; }
    void setReuseAddress(bool reuse);
    int getId() const { return mSocketId; }

private:
    int mSocketId = -1;
    bool mIsBound = false;
    bool mIsClosed = false;
    bool mReuseAddress = false;
};

} /* namespace mindroid */

#endif /* MINDROID_SERVERSOCKET_H_ */
