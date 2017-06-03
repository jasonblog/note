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

#ifndef MINDROID_INETADDRESS_H_
#define MINDROID_INETADDRESS_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"

#include <netinet/in.h>

namespace mindroid {

class InetAddress :
        public Object {
public:
    InetAddress(sockaddr_in& address) {
       mSize = sizeof(address);
       memcpy(&mAddress, &address, sizeof(address));
    }

    InetAddress(sockaddr_in6& address) {
        mSize = sizeof(address);
        memcpy(&mAddress, &address, sizeof(address));
    }

    virtual ~InetAddress() {
    }

    const sockaddr* getPointer() const { return (const sockaddr*)(&mAddress); }
    size_t getSize() const { return mSize; }
    bool isInet6Address() const { return mSize == sizeof(sockaddr_in6); }

private:
    struct sockaddr_in6 mAddress;
    size_t mSize;
};

} /* namespace mindroid */

#endif /* MINDROID_INETADDRESS_H_ */
