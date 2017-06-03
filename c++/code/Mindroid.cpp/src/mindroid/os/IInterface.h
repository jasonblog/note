/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2013 Daniel Himmelein
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

#ifndef MINDROID_IINTERFACE_H_
#define MINDROID_IINTERFACE_H_

#include "mindroid/lang/Object.h"
#include "mindroid/os/IBinder.h"

namespace mindroid {

/**
 * Base class for Binder interfaces. When defining a new interface, you must derive it from
 * IInterface.
 */
class IInterface :
        public virtual Object {
public:
    /**
     * Retrieve the Binder object associated with this interface. You must use this instead of a
     * plain cast, so that proxy objects can return the correct result.
     */
    virtual sp<IBinder> asBinder() = 0;

    /**
     * Release the unmanaged Binder resources associated with this interface.
     */
    void dispose() {
        asBinder()->dispose();
    }
};

template<typename INTERFACE>
inline sp<INTERFACE> interface_cast(const sp<IInterface>& interface) {
    return static_cast<INTERFACE*>(interface.getPointer());
}

} /* namespace mindroid */

#endif /* MINDROID_IINTERFACE_H_ */
