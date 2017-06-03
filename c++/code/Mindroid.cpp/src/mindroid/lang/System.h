/*
 * Copyright (C) 2016 E.S.R.Labs
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

#ifndef MINDROID_SYSTEM_H_
#define MINDROID_SYSTEM_H_

#include "mindroid/lang/String.h"
#include "mindroid/util/HashMap.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"
#include <cstdint>
#include <ctime>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME  0
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

namespace mindroid {

class System final {
public:
    ~System() noexcept = delete;
    System(const System&) = delete;
    System& operator=(const System&) = delete;

    /**
     * Returns the current time in milliseconds since January 1, 1970 00:00:00.0 UTC.
     *
     * <p>This method always returns UTC times, regardless of the system's time zone.
     * This is often called "Unix time" or "epoch time".
     *
     * <p>This method shouldn't be used for measuring timeouts or
     * other elapsed time measurements, as changing the system time can affect
     * the results.
     */
    static uint64_t currentTimeMillis() {
        timespec now;
        clock_gettime(CLOCK_REALTIME, &now);
        return (((uint64_t) now.tv_sec * 1000LL) + (now.tv_nsec / 1000000LL));
    }

    /**
     * Copies {@code length} elements from the array {@code src},
     * starting at offset {@code srcPos}, into the array {@code dst},
     * starting at offset {@code dstPos}.
     *
     * <p>The source and destination arrays can be the same array,
     * in which case copying is performed as if the source elements
     * are first copied into a temporary array and then into the
     * destination array.
     *
     * @param src
     *            the source array to copy the content.
     * @param srcPos
     *            the starting index of the content in {@code src}.
     * @param srcLength
     *            the length of the source array.
     * @param dst
     *            the destination array to copy the data into.
     * @param dstPos
     *            the starting index for the copied content in {@code dst}.
     * @param destLength
     *            the length of the destination array.
     * @param length
     *            the number of elements to be copied.
     */
    static bool arraycopy(const void* src, const size_t srcPos, const size_t srcLength,
            const void* dest, const size_t destPos, const size_t destLength, const size_t length) {
        if (srcPos + length > srcLength) {
            return false;
        }
        if (destPos + length > destLength) {
            return false;
        }
        if (src == nullptr || dest == nullptr) {
            return false;
        }
        memcpy(((uint8_t*) dest) + destPos, ((uint8_t*) src) + srcPos, length);
        return true;
    }

    /**
     * Returns the value of a particular system property or {@code null} if no
     * such property exists.
     *
     * <p>The following properties are always provided by Mindroid:</p>
     * <tr><td>os.arch</td>            <td>OS architecture</td>                   <td>{@code armv7l}</td></tr>
     * <tr><td>os.name</td>            <td>OS (kernel) name</td>                  <td>{@code Linux}</td></tr>
     * <tr><td>os.version</td>         <td>OS (kernel) version</td>               <td>{@code 2.6.32.9-g103d848}</td></tr>
     * <tr><td>user.dir</td>           <td>Base of non-absolute paths</td>        <td>{@code /}</td></tr>
     * </table>
     *
     * <p> All of the above properties except for <b>cannot be modified</b>.
     * Any attempt to change them will be a no-op.
     *
     * @param propertyName
     *            the name of the system property to look up.
     * @return the value of the specified system property or {@code null} if the
     *         property doesn't exist.
     */
    static sp<String> getProperty(const char* propertyName) {
        return getProperty(String::valueOf(propertyName), nullptr);
    }
    static sp<String> getProperty(const sp<String>& propertyName) {
        return getProperty(propertyName, nullptr);
    }

    /**
     * Returns the value of a particular system property. The {@code
     * defaultValue} will be returned if no such property has been found.
     */
    static sp<String> getProperty(const char* name, const char* defaultValue) {
        return getProperty(String::valueOf(name), String::valueOf(defaultValue));
    }
    static sp<String> getProperty(const sp<String>& name, const sp<String>& defaultValue);

    /**
     * Sets the value of a particular system property. Most system properties
     * are read only and cannot be cleared or modified. See {@link #getProperty} for a
     * list of such properties.
     *
     * @return the old value of the property or {@code null} if the property
     *         didn't exist.
     */
    static sp<String> setProperty(const char* name, const char* value) {
        return setProperty(String::valueOf(name), String::valueOf(value));
    }
    static sp<String> setProperty(const sp<String>& name, const sp<String>& value);

    /**
     * Causes the process to stop running and the program to exit with the given exit status.
     */
    static void exit(int32_t status) {
        ::exit(status);
    }

private:
    System();

    static System* getInstance();

    sp<ReentrantLock> mLock = new ReentrantLock();
    sp<HashMap<sp<String>, sp<String>>> mSystemProperties = new HashMap<sp<String>, sp<String>>();

    static pthread_mutex_t sMutex;
    static System* sInstance;
};

} /* namespace mindroid */

#endif /* MINDROID_SYSTEM_H_ */
