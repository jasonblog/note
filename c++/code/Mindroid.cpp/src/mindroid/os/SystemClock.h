/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2011 Daniel Himmelein
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

#ifndef MINDROID_SYSTEMCLOCK_H_
#define MINDROID_SYSTEMCLOCK_H_

#include <cstdint>
#include <ctime>

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME  0
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

namespace mindroid {

/**
 * Core timekeeping facilities.
 *
 * <p>
 * Three different clocks are available, and they should not be confused:
 *
 * <ul>
 * <li>
 * <p>
 * {@link System#currentTimeMillis System.currentTimeMillis()} is the standard "wall" clock (time
 * and date) expressing milliseconds since the epoch. The wall clock can be set by the phone
 * network, so the time may jump backwards or forwards unpredictably. This clock should only be used
 * when correspondence with real-world dates and times is important, such as in a calendar or alarm
 * clock application. Interval or elapsed time measurements should use a different clock.
 *
 * <li>
 * <p>
 * {@link #uptimeMillis} is counted in milliseconds since the system was booted. This clock stops
 * when the system enters deep sleep (CPU off, display dark, device waiting for external input), but
 * is not affected by clock scaling, idle, or other power saving mechanisms. This is the basis for
 * most interval timing such as {@link Thread#sleep(long) Thread.sleep(millls)},
 * {@link Object#wait(long) Object.wait(millis)}, and {@link System#nanoTime System.nanoTime()}.
 * This clock is guaranteed to be monotonic, and is the recommended basis for the general purpose
 * interval timing of user interface events, performance measurements, and anything else that does
 * not need to measure elapsed time during device sleep. Most methods that accept a timestamp value
 * expect the {@link #uptimeMillis} clock.
 *
 * <li>
 * <p>
 * {@link #elapsedRealtime} is counted in milliseconds since the system was booted, including deep
 * sleep. This clock should be used when measuring time intervals that may span periods of system
 * sleep.
 * </ul>
 *
 * There are several mechanisms for controlling the timing of events:
 *
 * <ul>
 * <li>
 * <p>
 * Standard functions like {@link Thread#sleep(long) Thread.sleep(millis)} and
 * {@link Object#wait(long) Object.wait(millis)} are always available. These functions use the
 * {@link #uptimeMillis} clock; if the device enters sleep, the remainder of the time will be
 * postponed until the device wakes up. These synchronous functions may be interrupted with
 * {@link Thread#interrupt Thread.interrupt()}, and you must handle {@link InterruptedException}.
 *
 * <li>
 * <p>
 * {@link #sleep SystemClock.sleep(millis)} is a utility function very similar to
 * {@link Thread#sleep(long) Thread.sleep(millis)}, but it ignores {@link InterruptedException}. Use
 * this function for delays if you do not use {@link Thread#interrupt Thread.interrupt()}, as it
 * will preserve the interrupted state of the thread.
 *
 * <li>
 * <p>
 * The {@link mindroid.os.Handler} class can schedule asynchronous callbacks at an absolute or
 * relative time. Handler objects also use the {@link #uptimeMillis} clock, and require an
 * {@link mindroid.os.Looper event loop} (normally present in any GUI application).
 *
 * <li>
 * <p>
 * The {@link mindroid.app.AlarmManager} can trigger one-time or recurring events which occur even
 * when the device is in deep sleep or your application is not running. Events may be scheduled with
 * your choice of {@link java.lang.System#currentTimeMillis} (RTC) or {@link #elapsedRealtime}
 * (ELAPSED_REALTIME), and cause an {@link mindroid.content.Intent} broadcast when they occur.
 * </ul>
 */
class SystemClock {
public:
    /**
     * This class is uninstantiable.
     */
    SystemClock() noexcept = delete;
    ~SystemClock() noexcept = delete;
    SystemClock(const SystemClock&) = delete;
    SystemClock& operator=(const SystemClock&) = delete;

    /**
     * Waits a given number of milliseconds (of uptimeMillis) before returning. Similar to
     * {@link java.lang.Thread#sleep(long)}, but does not throw {@link InterruptedException};
     * {@link Thread#interrupt()} events are deferred until the next interruptible operation. Does
     * not return until at least the specified number of milliseconds has elapsed.
     *
     * @param ms to sleep before returning, in milliseconds of uptime.
     */
    static void sleep(uint64_t ms);

    /**
     * Returns milliseconds since boot, not counting time spent in deep sleep. <b>Note:</b> This
     * value may get reset occasionally (before it would otherwise wrap around).
     *
     * @return milliseconds of non-sleep uptime since boot.
     */
    static uint64_t uptimeMillis();

    /**
     * Returns milliseconds since boot, including time spent in sleep.
     *
     * @return elapsed milliseconds since boot.
     */
    static uint64_t elapsedRealtime();

    /**
     * Returns nanoseconds since boot, including time spent in sleep.
     *
     * @return elapsed nanoseconds since boot.
     */
    static uint64_t elapsedRealtimeNanos() {
        return elapsedRealtime() * 1000000;
    }
};

} /* namespace mindroid */

#endif /* MINDROID_SYSTEMCLOCK_H_ */
