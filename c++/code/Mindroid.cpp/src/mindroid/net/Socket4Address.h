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

#ifndef MINDROID_SOCKET4ADDRESS_H_
#define MINDROID_SOCKET4ADDRESS_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"
#include "mindroid/net/SocketAddress.h"

namespace mindroid {

class Socket4Address :
        public SocketAddress {
public:
    virtual ~Socket4Address() = default;

    uint16_t getPort() const override;

private:
    Socket4Address() = default;
    Socket4Address(uint16_t port);
    Socket4Address(const char* host, uint16_t port) :
            Socket4Address(String::valueOf(host), port) {
    }
    Socket4Address(const sp<String>& host, uint16_t port);

    Socket4Address(const Socket4Address&) = delete;
    Socket4Address& operator=(const Socket4Address&) = delete;

    friend class SocketAddress;
};

} /* namespace mindroid */

#endif /* MINDROID_SOCKET4ADDRESS_H_ */
