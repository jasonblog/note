/*
 * Copyright 2008 Google Inc.  All rights reserved.
 * http://code.google.com/p/protobuf/
 * Copyright (C) 2013 Daniel Himmelein
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of Google Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * sp<Runnable> runnable = obtainClosure(instance, &Test::test, 17)
 *
 * runnable->run();
 */

#ifndef MINDROID_CLOSURE_H_
#define MINDROID_CLOSURE_H_

#include "mindroid/lang/Runnable.h"
#include <functional>

namespace mindroid {

class Handler;

class Closure :
        public Runnable {
public:
    Closure(const sp<Handler>& handler, const std::function<void (void)>& func) : mHandler(handler), mFunc(func) { }
    Closure(const sp<Handler>& handler, std::function<void (void)>&& func) : mHandler(handler), mFunc(std::move(func)) { }
    Closure(const Closure&) = delete;
    Closure& operator=(const Closure&) = delete;
    Closure(Closure&&) = delete;
    Closure& operator=(Closure&&) = delete;

    virtual void run() {
        mFunc();
    }

    void cancel();

private:
    sp<Handler> mHandler;
    std::function<void (void)> mFunc;
};

class FuncClosure0 :
        public Runnable {
public:
    typedef void (*FuncType)();

    FuncClosure0(FuncType func) :
            mFunc(func) {
    }

    virtual ~FuncClosure0() = default;

    FuncClosure0(const FuncClosure0&) = delete;
    FuncClosure0& operator=(const FuncClosure0&) = delete;

    virtual void run() {
        mFunc();
    }

private:
    FuncType mFunc;
};

template<typename Class>
class Closure0 :
        public Runnable {
public:
    typedef void (Class::*MethodType)();

    Closure0(Class& object, MethodType method) :
            mObject(&object),
            mMethod(method) {
    }

    virtual ~Closure0() = default;

    Closure0(const Closure0&) = delete;
    Closure0& operator=(const Closure0&) = delete;

    virtual void run() {
        (mObject->*mMethod)();
    }

private:
    Class* mObject;
    MethodType mMethod;
};

template<typename Arg1>
class FuncClosure1 :
        public Runnable {
public:
    typedef void (*FuncType)(Arg1 arg1);

    FuncClosure1(FuncType func, Arg1 arg1) :
            mFunc(func),
            mArg1(arg1) {
    }

    virtual ~FuncClosure1() = default;

    FuncClosure1(const FuncClosure1&) = delete;
    FuncClosure1& operator=(const FuncClosure1&) = delete;

    virtual void run() {
        mFunc(mArg1);
    }

private:
    FuncType mFunc;
    Arg1 mArg1;
};

template<typename Class, typename Arg1>
class Closure1 :
        public Runnable {
public:
    typedef void (Class::*MethodType)(Arg1 arg1);

    Closure1(Class& object, MethodType method, Arg1 arg1) :
            mObject(&object),
            mMethod(method),
            mArg1(arg1) {
    }

    virtual ~Closure1() = default;

    Closure1(const Closure1&) = delete;
    Closure1& operator=(const Closure1&) = delete;

    virtual void run() {
        (mObject->*mMethod)(mArg1);
    }

private:
    Class* mObject;
    MethodType mMethod;
    Arg1 mArg1;
};

template<typename Arg1, typename Arg2>
class FuncClosure2 :
        public Runnable {
public:
    typedef void (*FuncType)(Arg1 arg1, Arg2 arg2);

    FuncClosure2(FuncType func, Arg1 arg1, Arg2 arg2) :
            mFunc(func),
            mArg1(arg1),
            mArg2(arg2) {
    }

    virtual ~FuncClosure2() = default;

    FuncClosure2(const FuncClosure2&) = delete;
    FuncClosure2& operator=(const FuncClosure2&) = delete;

    virtual void run() {
        mFunc(mArg1, mArg2);
    }

private:
    FuncType mFunc;
    Arg1 mArg1;
    Arg2 mArg2;
};

template<typename Class, typename Arg1, typename Arg2>
class Closure2 :
        public Runnable {
public:
    typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2);

    Closure2(Class& object, MethodType method, Arg1 arg1, Arg2 arg2) :
            mObject(&object),
            mMethod(method),
            mArg1(arg1),
            mArg2(arg2) {
    }

    virtual ~Closure2() = default;

    Closure2(const Closure2&) = delete;
    Closure2& operator=(const Closure2&) = delete;

    virtual void run() {
        (mObject->*mMethod)(mArg1, mArg2);
    }

private:
    Class* mObject;
    MethodType mMethod;
    Arg1 mArg1;
    Arg2 mArg2;
};

template<typename Arg1, typename Arg2, typename Arg3>
class FuncClosure3 :
        public Runnable {
public:
    typedef void (*FuncType)(Arg1 arg1, Arg2 arg2, Arg3 arg3);

    FuncClosure3(FuncType func, Arg1 arg1, Arg2 arg2, Arg3 arg3) :
            mFunc(func),
            mArg1(arg1),
            mArg2(arg2),
            mArg3(arg3) {
    }

    virtual ~FuncClosure3() = default;

    FuncClosure3(const FuncClosure3&) = delete;
    FuncClosure3& operator=(const FuncClosure3&) = delete;

    virtual void run() {
        mFunc(mArg1, mArg2, mArg3);
    }

private:
    FuncType mFunc;
    Arg1 mArg1;
    Arg2 mArg2;
    Arg3 mArg3;
};

template<typename Class, typename Arg1, typename Arg2, typename Arg3>
class Closure3 :
        public Runnable {
public:
    typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3);

    Closure3(Class& object, MethodType method, Arg1 arg1, Arg2 arg2, Arg3 arg3) :
            mObject(&object),
            mMethod(method),
            mArg1(arg1),
            mArg2(arg2),
            mArg3(arg3) {
    }

    virtual ~Closure3() = default;

    Closure3(const Closure3&) = delete;
    Closure3& operator=(const Closure3&) = delete;

    virtual void run() {
        (mObject->*mMethod)(mArg1, mArg2, mArg3);
    }

private:
    Class* mObject;
    MethodType mMethod;
    Arg1 mArg1;
    Arg2 mArg2;
    Arg3 mArg3;
};

template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class FuncClosure4 :
        public Runnable {
public:
    typedef void (*FuncType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4);

    FuncClosure4(FuncType func, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) :
            mFunc(func),
            mArg1(arg1),
            mArg2(arg2),
            mArg3(arg3),
            mArg4(arg4) {
    }

    virtual ~FuncClosure4() = default;

    FuncClosure4(const FuncClosure4&) = delete;
    FuncClosure4& operator=(const FuncClosure4&) = delete;

    virtual void run() {
        mFunc(mArg1, mArg2, mArg3, mArg4);
    }

private:
    FuncType mFunc;
    Arg1 mArg1;
    Arg2 mArg2;
    Arg3 mArg3;
    Arg4 mArg4;
};

template<typename Class, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class Closure4 :
        public Runnable {
public:
    typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4);

    Closure4(Class& object, MethodType method, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) :
            mObject(&object),
            mMethod(method),
            mArg1(arg1),
            mArg2(arg2),
            mArg3( arg3),
            mArg4(arg4) {
    }

    virtual ~Closure4() = default;

    Closure4(const Closure4&) = delete;
    Closure4& operator=(const Closure4&) = delete;

    virtual void run() {
        (mObject->*mMethod)(mArg1, mArg2, mArg3, mArg4);
    }

private:
    Class* mObject;
    MethodType mMethod;
    Arg1 mArg1;
    Arg2 mArg2;
    Arg3 mArg3;
    Arg4 mArg4;
};

inline sp<Runnable> obtainClosure(void(*func)()) {
    return new FuncClosure0(func);
}

template<typename Class>
inline sp<Runnable> obtainClosure(Class& object, void(Class::*method)()) {
    return new Closure0<Class>(object, method);
}

template<typename Arg1>
inline sp<Runnable> obtainClosure(void(*func)(Arg1), Arg1 arg1) {
    return new FuncClosure1<Arg1>(func, arg1);
}

template<typename Class, typename Arg1>
inline sp<Runnable> obtainClosure(Class& object, void(Class::*method)(Arg1), Arg1 arg1) {
    return new Closure1<Class, Arg1>(object, method, arg1);
}

template<typename Arg1, typename Arg2>
inline sp<Runnable> obtainClosure(void(*func)(Arg1, Arg2), Arg1 arg1, Arg2 arg2) {
    return new FuncClosure2<Arg1, Arg2>(func, arg1, arg2);
}

template<typename Class, typename Arg1, typename Arg2>
inline sp<Runnable> obtainClosure(Class& object, void(Class::*method)(Arg1, Arg2), Arg1 arg1, Arg2 arg2) {
    return new Closure2<Class, Arg1, Arg2>(object, method, arg1, arg2);
}

template<typename Arg1, typename Arg2, typename Arg3>
inline sp<Runnable> obtainClosure(void(*func)(Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    return new FuncClosure3<Arg1, Arg2, Arg3>(func, arg1, arg2, arg3);
}

template<typename Class, typename Arg1, typename Arg2, typename Arg3>
inline sp<Runnable> obtainClosure(Class& object, void(Class::*method)(Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    return new Closure3<Class, Arg1, Arg2, Arg3>(object, method, arg1, arg2, arg3);
}

template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline sp<Runnable> obtainClosure(void(*func)(Arg1, Arg2, Arg3, Arg4), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
    return new FuncClosure4<Arg1, Arg2, Arg3, Arg4>(func, arg1, arg2, arg3, arg4);
}

template<typename Class, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline sp<Runnable> obtainClosure(Class& object, void(Class::*method)(Arg1, Arg2, Arg3, Arg4), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
    return new Closure4<Class, Arg1, Arg2, Arg3, Arg4>(object, method, arg1, arg2, arg3, arg4);
}

} /* namespace mindroid */

#endif /* MINDROID_CLOSURE_H_ */
