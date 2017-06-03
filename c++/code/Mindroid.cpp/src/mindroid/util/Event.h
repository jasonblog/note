/*
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

#ifndef MINDROID_EVENT_H_
#define MINDROID_EVENT_H_

#include "mindroid/util/LinkedList.h"
#include "mindroid/util/Delegate.h"

namespace mindroid {

template<typename R = void, typename A1 = void, typename A2 = void, typename A3 = void, typename A4 = void>
class Event;

// 4 arguments
template<typename R, typename A1, typename A2, typename A3, typename A4>
class Event {
public:
    typedef Delegate<R, A1, A2, A3, A4> DelegateType;

    Event() { mEventHandlers = new LinkedList<DelegateType>(); }
    ~Event() { }
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event& operator+=(const DelegateType& delegate) {
        mEventHandlers->add(delegate);
        return *this;
    }

    Event& operator-=(const DelegateType& delegate) {
        mEventHandlers->remove(delegate);
        return *this;
    }

    void operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4) const {
        auto itr = mEventHandlers->iterator();
        while (itr.hasNext()) {
            DelegateType delegate = itr.next();
            delegate(arg1, arg2, arg3, arg4);
        }
    }

private:
    sp<LinkedList<DelegateType>> mEventHandlers;
};

template<typename R, typename A1, typename A2, typename A3, typename A4>
class Event<R(A1, A2, A3, A4)> {
public:
    typedef Delegate<R(A1, A2, A3, A4)> DelegateType;

    Event() { mEventHandlers = new LinkedList<DelegateType>(); }
    ~Event() { }
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event& operator+=(const DelegateType& delegate) {
        mEventHandlers->add(delegate);
        return *this;
    }

    Event& operator-=(const DelegateType& delegate) {
        mEventHandlers->remove(delegate);
        return *this;
    }

    void operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4) const {
        auto itr = mEventHandlers->iterator();
        while (itr.hasNext()) {
            DelegateType delegate = itr.next();
            delegate(arg1, arg2, arg3, arg4);
        }
    }

private:
    sp<LinkedList<DelegateType>> mEventHandlers;
};

// 3 arguments
template<typename R, typename A1, typename A2, typename A3>
class Event<R, A1, A2, A3, void> {
public:
    typedef Delegate<R, A1, A2, A3> DelegateType;

    Event() { mEventHandlers = new LinkedList<DelegateType>(); }
    ~Event() { }
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event& operator+=(const DelegateType& delegate) {
        mEventHandlers->add(delegate);
        return *this;
    }

    Event& operator-=(const DelegateType& delegate) {
        mEventHandlers->remove(delegate);
        return *this;
    }

    void operator()(A1 arg1, A2 arg2, A3 arg3) const {
        auto itr = mEventHandlers->iterator();
        while (itr.hasNext()) {
            DelegateType delegate = itr.next();
            delegate(arg1, arg2, arg3);
        }
    }

private:
    sp<LinkedList<DelegateType>> mEventHandlers;
};

template<typename R, typename A1, typename A2, typename A3>
class Event<R(A1, A2, A3), void> {
public:
    typedef Delegate<R(A1, A2, A3)> DelegateType;

    Event() { mEventHandlers = new LinkedList<DelegateType>(); }
    ~Event() { }
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event& operator+=(const DelegateType& delegate) {
        mEventHandlers->add(delegate);
        return *this;
    }

    Event& operator-=(const DelegateType& delegate) {
        mEventHandlers->remove(delegate);
        return *this;
    }

    void operator()(A1 arg1, A2 arg2, A3 arg3) const {
        auto itr = mEventHandlers->iterator();
        while (itr.hasNext()) {
            DelegateType delegate = itr.next();
            delegate(arg1, arg2, arg3);
        }
    }

private:
    sp<LinkedList<DelegateType>> mEventHandlers;
};

// 2 arguments
template<typename R, typename A1, typename A2>
class Event<R, A1, A2, void, void> {
public:
    typedef Delegate<R, A1, A2> DelegateType;

    Event() { mEventHandlers = new LinkedList<DelegateType>(); }
    ~Event() { }
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event& operator+=(const DelegateType& delegate) {
        mEventHandlers->add(delegate);
        return *this;
    }

    Event& operator-=(const DelegateType& delegate) {
        mEventHandlers->remove(delegate);
        return *this;
    }

    void operator()(A1 arg1, A2 arg2) const {
        auto itr = mEventHandlers->iterator();
        while (itr.hasNext()) {
            DelegateType delegate = itr.next();
            delegate(arg1, arg2);
        }
    }

private:
    sp<LinkedList<DelegateType>> mEventHandlers;
};

template<typename R, typename A1, typename A2>
class Event<R(A1, A2), void, void> {
public:
    typedef Delegate<R(A1, A2)> DelegateType;

    Event() { mEventHandlers = new LinkedList<DelegateType>(); }
    ~Event() { }
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event& operator+=(const DelegateType& delegate) {
        mEventHandlers->add(delegate);
        return *this;
    }

    Event& operator-=(const DelegateType& delegate) {
        mEventHandlers->remove(delegate);
        return *this;
    }

    void operator()(A1 arg1, A2 arg2) const {
        auto itr = mEventHandlers->iterator();
        while (itr.hasNext()) {
            DelegateType delegate = itr.next();
            delegate(arg1, arg2);
        }
    }

private:
    sp<LinkedList<DelegateType>> mEventHandlers;
};

// 1 argument
template<typename R, typename A1>
class Event<R, A1, void, void, void> {
public:
    typedef Delegate<R, A1> DelegateType;

    Event() { mEventHandlers = new LinkedList<DelegateType>(); }
    ~Event() { }
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event& operator+=(const DelegateType& delegate) {
        mEventHandlers->add(delegate);
        return *this;
    }

    Event& operator-=(const DelegateType& delegate) {
        mEventHandlers->remove(delegate);
        return *this;
    }

    void operator()(A1 arg1) const {
        auto itr = mEventHandlers->iterator();
        while (itr.hasNext()) {
            DelegateType delegate = itr.next();
            delegate(arg1);
        }
    }

private:
    sp<LinkedList<DelegateType>> mEventHandlers;
};

template<typename R, typename A1>
class Event<R(A1), void, void, void> {
public:
    typedef Delegate<R(A1)> DelegateType;

    Event() { mEventHandlers = new LinkedList<DelegateType>(); }
    ~Event() { }
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event& operator+=(const DelegateType& delegate) {
        mEventHandlers->add(delegate);
        return *this;
    }

    Event& operator-=(const DelegateType& delegate) {
        mEventHandlers->remove(delegate);
        return *this;
    }

    void operator()(A1 arg1) const {
        auto itr = mEventHandlers->iterator();
        while (itr.hasNext()) {
            DelegateType delegate = itr.next();
            delegate(arg1);
        }
    }

private:
    sp<LinkedList<DelegateType>> mEventHandlers;
};

// no arguments
template<typename R>
class Event<R, void, void, void, void> {
public:
    typedef Delegate<R> DelegateType;

    Event() { mEventHandlers = new LinkedList<DelegateType>(); }
    ~Event() { }
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event& operator+=(const DelegateType& delegate) {
        mEventHandlers->add(delegate);
        return *this;
    }

    Event& operator-=(const DelegateType& delegate) {
        mEventHandlers->remove(delegate);
        return *this;
    }

    void operator()() const {
        auto itr = mEventHandlers->iterator();
        while (itr.hasNext()) {
            DelegateType delegate = itr.next();
            delegate();
        }
    }

private:
    sp<LinkedList<DelegateType>> mEventHandlers;
};

template<typename R>
class Event<R(void), void, void, void> {
public:
    typedef Delegate<R> DelegateType;

    Event() { mEventHandlers = new LinkedList<DelegateType>(); }
    ~Event() { }
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event& operator+=(const DelegateType& delegate) {
        mEventHandlers->add(delegate);
        return *this;
    }

    Event& operator-=(const DelegateType& delegate) {
        mEventHandlers->remove(delegate);
        return *this;
    }

    void operator()() const {
        auto itr = mEventHandlers->iterator();
        while (itr.hasNext()) {
            DelegateType delegate = itr.next();
            delegate();
        }
    }

private:
    sp<LinkedList<DelegateType>> mEventHandlers;
};

} /* namespace mindroid */

#endif /* MINDROID_EVENT_H_ */
