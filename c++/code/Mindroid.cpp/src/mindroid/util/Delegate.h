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

/**
 * Delegate<void, uint32_t> delegate = Delegate<void, uint32_t>::create<Test, &Test::test>(instance)
 * or
 * Delegate<void (uint32_t)> delegate = Delegate<void (uint32_t)>::create<Test, &Test::test>(instance)
 *
 * delegate(123);
 */

#ifndef MINDROID_DELEGATE_H_
#define MINDROID_DELEGATE_H_

#include <cstdlib>

namespace mindroid {

template<typename R = void, typename A1 = void, typename A2 = void, typename A3 = void, typename A4 = void>
class Delegate;

// 4 arguments
template<typename R, typename A1, typename A2, typename A3, typename A4>
class Delegate {
public:
    Delegate() :
            mObject(nullptr),
            mMethod(nullptr) {
    }

    template<class T, R(T::*TMethod)(A1, A2, A3, A4)>
    static Delegate create(T& object) {
        Delegate delegate;
        delegate.mObject = &object;
        delegate.mMethod = &exec<T, TMethod>;
        return delegate;
    }

    template<R(*TMethod)(A1, A2, A3, A4)>
    static Delegate create() {
        Delegate delegate;
        delegate.mObject = nullptr;
        delegate.mMethod = &exec<TMethod>;
        return delegate;
    }

    void destroy() {
        mObject = nullptr;
        mMethod = nullptr;
    }

    bool callable() {
        return (mMethod != nullptr);
    }

    R operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4) const {
        return (*mMethod)(mObject, arg1, arg2, arg3, arg4);
    }

    bool operator==(Delegate const& rhs) {
        return (mObject == rhs.mObject && mMethod == rhs.mMethod);
    }

private:
    typedef R (*MethodType)(void* object, A1 arg1, A2 arg2, A3 arg3, A4 arg4);

    template<class T, R(T::*TMethod)(A1, A2, A3, A4)>
    static R exec(void* object, A1 arg1, A2 arg2, A3 arg3, A4 arg4) {
        T* self = static_cast<T*>(object);
        return (self->*TMethod)(arg1, arg2, arg3, arg4);
    }

    template<R(*TMethod)(A1, A2, A3, A4)>
    static R exec(void* object, A1 arg1, A2 arg2, A3 arg3, A4 arg4) {
        return (*TMethod)(arg1, arg2, arg3, arg4);
    }

    void* mObject;
    MethodType mMethod;
};

template<typename R, typename A1, typename A2, typename A3, typename A4>
class Delegate<R(A1, A2, A3, A4)> {
public:
    Delegate() :
            mObject(nullptr),
            mMethod(nullptr) {
    }

    template<class T, R(T::*TMethod)(A1, A2, A3, A4)>
    static Delegate create(T& object) {
        Delegate delegate;
        delegate.mObject = &object;
        delegate.mMethod = &exec<T, TMethod>;
        return delegate;
    }

    template<R(*TMethod)(A1, A2, A3, A4)>
    static Delegate create() {
        Delegate delegate;
        delegate.mObject = nullptr;
        delegate.mMethod = &exec<TMethod>;
        return delegate;
    }

    void destroy() {
        mObject = nullptr;
        mMethod = nullptr;
    }

    bool callable() {
        return (mMethod != nullptr);
    }

    R operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4) const {
        return (*mMethod)(mObject, arg1, arg2, arg3, arg4);
    }

    bool operator==(Delegate const& rhs) {
        return (mObject == rhs.mObject && mMethod == rhs.mMethod);
    }

private:
    typedef R (*MethodType)(void* object, A1 arg1, A2 arg2, A3 arg3, A4 arg4);

    template<class T, R(T::*TMethod)(A1, A2, A3, A4)>
    static R exec(void* object, A1 arg1, A2 arg2, A3 arg3, A4 arg4) {
        T* self = static_cast<T*>(object);
        return (self->*TMethod)(arg1, arg2, arg3, arg4);
    }

    template<R(*TMethod)(A1, A2, A3, A4)>
    static R exec(void* object, A1 arg1, A2 arg2, A3 arg3, A4 arg4) {
        return (*TMethod)(arg1, arg2, arg3, arg4);
    }

    void* mObject;
    MethodType mMethod;
};

// 3 arguments
template<typename R, typename A1, typename A2, typename A3>
class Delegate<R, A1, A2, A3, void> {
public:
    Delegate() :
            mObject(nullptr),
            mMethod(nullptr) {
    }

    template<class T, R(T::*TMethod)(A1, A2, A3)>
    static Delegate create(T& object) {
        Delegate delegate;
        delegate.mObject = &object;
        delegate.mMethod = &exec<T, TMethod>;
        return delegate;
    }

    template<R(*TMethod)(A1, A2, A3)>
    static Delegate create() {
        Delegate delegate;
        delegate.mObject = nullptr;
        delegate.mMethod = &exec<TMethod>;
        return delegate;
    }

    void destroy() {
        mObject = nullptr;
        mMethod = nullptr;
    }

    bool callable() {
        return (mMethod != nullptr);
    }

    R operator()(A1 arg1, A2 arg2, A3 arg3) const {
        return (*mMethod)(mObject, arg1, arg2, arg3);
    }

    bool operator==(Delegate const& rhs) {
        return (mObject == rhs.mObject && mMethod == rhs.mMethod);
    }

private:
    typedef R (*MethodType)(void* object, A1 arg1, A2 arg2, A3 arg3);

    template<class T, R(T::*TMethod)(A1, A2, A3)>
    static R exec(void* object, A1 arg1, A2 arg2, A3 arg3) {
        T* self = static_cast<T*>(object);
        return (self->*TMethod)(arg1, arg2, arg3);
    }

    template<R(*TMethod)(A1, A2, A3)>
    static R exec(void* object, A1 arg1, A2 arg2, A3 arg3) {
        return (*TMethod)(arg1, arg2, arg3);
    }

    void* mObject;
    MethodType mMethod;
};

template<typename R, typename A1, typename A2, typename A3>
class Delegate<R(A1, A2, A3), void> {
public:
    Delegate() :
            mObject(nullptr),
            mMethod(nullptr) {
    }

    template<class T, R(T::*TMethod)(A1, A2, A3)>
    static Delegate create(T& object) {
        Delegate delegate;
        delegate.mObject = &object;
        delegate.mMethod = &exec<T, TMethod>;
        return delegate;
    }

    template<R(*TMethod)(A1, A2, A3)>
    static Delegate create() {
        Delegate delegate;
        delegate.mObject = nullptr;
        delegate.mMethod = &exec<TMethod>;
        return delegate;
    }

    void destroy() {
        mObject = nullptr;
        mMethod = nullptr;
    }

    bool callable() {
        return (mMethod != nullptr);
    }

    R operator()(A1 arg1, A2 arg2, A3 arg3) const {
        return (*mMethod)(mObject, arg1, arg2, arg3);
    }

    bool operator==(Delegate const& rhs) {
        return (mObject == rhs.mObject && mMethod == rhs.mMethod);
    }

private:
    typedef R (*MethodType)(void* object, A1 arg1, A2 arg2, A3 arg3);

    template<class T, R(T::*TMethod)(A1, A2, A3)>
    static R exec(void* object, A1 arg1, A2 arg2, A3 arg3) {
        T* self = static_cast<T*>(object);
        return (self->*TMethod)(arg1, arg2, arg3);
    }

    template<R(*TMethod)(A1, A2, A3)>
    static R exec(void* object, A1 arg1, A2 arg2, A3 arg3) {
        return (*TMethod)(arg1, arg2, arg3);
    }

    void* mObject;
    MethodType mMethod;
};

// 2 arguments
template<typename R, typename A1, typename A2>
class Delegate<R, A1, A2, void, void> {
public:
    Delegate() :
            mObject(nullptr),
            mMethod(nullptr) {
    }

    template<class T, R(T::*TMethod)(A1, A2)>
    static Delegate create(T& object) {
        Delegate delegate;
        delegate.mObject = &object;
        delegate.mMethod = &exec<T, TMethod>;
        return delegate;
    }

    template<R(*TMethod)(A1, A2)>
    static Delegate create() {
        Delegate delegate;
        delegate.mObject = nullptr;
        delegate.mMethod = &exec<TMethod>;
        return delegate;
    }

    void destroy() {
        mObject = nullptr;
        mMethod = nullptr;
    }

    bool callable() {
        return (mMethod != nullptr);
    }

    R operator()(A1 arg1, A2 arg2) const {
        return (*mMethod)(mObject, arg1, arg2);
    }

    bool operator==(Delegate const& rhs) {
        return (mObject == rhs.mObject && mMethod == rhs.mMethod);
    }

private:
    typedef R (*MethodType)(void* object, A1 arg1, A2 arg2);

    template<class T, R(T::*TMethod)(A1, A2)>
    static R exec(void* object, A1 arg1, A2 arg2) {
        T* self = static_cast<T*>(object);
        return (self->*TMethod)(arg1, arg2);
    }

    template<R(*TMethod)(A1, A2)>
    static R exec(void* object, A1 arg1, A2 arg2) {
        return (*TMethod)(arg1, arg2);
    }

    void* mObject;
    MethodType mMethod;
};

template<typename R, typename A1, typename A2>
class Delegate<R(A1, A2), void, void> {
public:
    Delegate() :
            mObject(nullptr),
            mMethod(nullptr) {
    }

    template<class T, R(T::*TMethod)(A1, A2)>
    static Delegate create(T& object) {
        Delegate delegate;
        delegate.mObject = &object;
        delegate.mMethod = &exec<T, TMethod>;
        return delegate;
    }

    template<R(*TMethod)(A1, A2)>
    static Delegate create() {
        Delegate delegate;
        delegate.mObject = nullptr;
        delegate.mMethod = &exec<TMethod>;
        return delegate;
    }

    void destroy() {
        mObject = nullptr;
        mMethod = nullptr;
    }

    bool callable() {
        return (mMethod != nullptr);
    }

    R operator()(A1 arg1, A2 arg2) const {
        return (*mMethod)(mObject, arg1, arg2);
    }

    bool operator==(Delegate const& rhs) {
        return (mObject == rhs.mObject && mMethod == rhs.mMethod);
    }

private:
    typedef R (*MethodType)(void* object, A1 arg1, A2 arg2);

    template<class T, R(T::*TMethod)(A1, A2)>
    static R exec(void* object, A1 arg1, A2 arg2) {
        T* self = static_cast<T*>(object);
        return (self->*TMethod)(arg1, arg2);
    }

    template<R(*TMethod)(A1, A2)>
    static R exec(void* object, A1 arg1, A2 arg2) {
        return (*TMethod)(arg1, arg2);
    }

    void* mObject;
    MethodType mMethod;
};

// 1 argument
template<typename R, typename A1>
class Delegate<R, A1, void, void, void> {
public:
    Delegate() :
            mObject(nullptr),
            mMethod(nullptr) {
    }

    template<class T, R(T::*TMethod)(A1)>
    static Delegate create(T& object) {
        Delegate delegate;
        delegate.mObject = &object;
        delegate.mMethod = &exec<T, TMethod>;
        return delegate;
    }

    template<R(*TMethod)(A1)>
    static Delegate create() {
        Delegate delegate;
        delegate.mObject = nullptr;
        delegate.mMethod = &exec<TMethod>;
        return delegate;
    }

    void destroy() {
        mObject = nullptr;
        mMethod = nullptr;
    }

    bool callable() {
        return (mMethod != nullptr);
    }

    R operator()(A1 arg1) const {
        return (*mMethod)(mObject, arg1);
    }

    bool operator==(Delegate const& rhs) {
        return (mObject == rhs.mObject && mMethod == rhs.mMethod);
    }

private:
    typedef R (*MethodType)(void* object, A1 arg1);

    template<class T, R(T::*TMethod)(A1)>
    static R exec(void* object, A1 arg1) {
        T* self = static_cast<T*>(object);
        return (self->*TMethod)(arg1);
    }

    template<R(*TMethod)(A1)>
    static R exec(void* object, A1 arg1) {
        return (*TMethod)(arg1);
    }

    void* mObject;
    MethodType mMethod;
};

template<typename R, typename A1>
class Delegate<R(A1), void, void, void> {
public:
    Delegate() :
            mObject(nullptr),
            mMethod(nullptr) {
    }

    template<class T, R(T::*TMethod)(A1)>
    static Delegate create(T& object) {
        Delegate delegate;
        delegate.mObject = &object;
        delegate.mMethod = &exec<T, TMethod>;
        return delegate;
    }

    template<R(*TMethod)(A1)>
    static Delegate create() {
        Delegate delegate;
        delegate.mObject = nullptr;
        delegate.mMethod = &exec<TMethod>;
        return delegate;
    }

    void destroy() {
        mObject = nullptr;
        mMethod = nullptr;
    }

    bool callable() {
        return (mMethod != nullptr);
    }

    R operator()(A1 arg1) const {
        return (*mMethod)(mObject, arg1);
    }

    bool operator==(Delegate const& rhs) {
        return (mObject == rhs.mObject && mMethod == rhs.mMethod);
    }

private:
    typedef R (*MethodType)(void* object, A1 arg1);

    template<class T, R(T::*TMethod)(A1)>
    static R exec(void* object, A1 arg1) {
        T* self = static_cast<T*>(object);
        return (self->*TMethod)(arg1);
    }

    template<R(*TMethod)(A1)>
    static R exec(void* object, A1 arg1) {
        return (*TMethod)(arg1);
    }

    void* mObject;
    MethodType mMethod;
};

// no arguments
template<typename R>
class Delegate<R, void, void, void, void> {
public:
    Delegate() :
            mObject(nullptr),
            mMethod(nullptr) {
    }

    template<class T, R(T::*TMethod)()>
    static Delegate create(T& object) {
        Delegate delegate;
        delegate.mObject = &object;
        delegate.mMethod = &exec<T, TMethod>;
        return delegate;
    }

    template<R(*TMethod)()>
    static Delegate create() {
        Delegate delegate;
        delegate.mObject = nullptr;
        delegate.mMethod = &exec<TMethod>;
        return delegate;
    }

    void destroy() {
        mObject = nullptr;
        mMethod = nullptr;
    }

    bool callable() {
        return (mMethod != nullptr);
    }

    R operator()() const {
        return (*mMethod)(mObject);
    }

    bool operator==(Delegate const& rhs) {
        return (mObject == rhs.mObject && mMethod == rhs.mMethod);
    }

private:
    typedef R (*MethodType)(void* object);

    template<class T, R(T::*TMethod)()>
    static R exec(void* object) {
        T* self = static_cast<T*>(object);
        return (self->*TMethod)();
    }

    template<R(*TMethod)()>
    static R exec(void* object) {
        return (*TMethod)();
    }

    void* mObject;
    MethodType mMethod;
};

template<typename R>
class Delegate<R(void), void, void, void> {
public:
    Delegate() :
            mObject(nullptr),
            mMethod(nullptr) {
    }

    template<class T, R(T::*TMethod)()>
    static Delegate create(T& object) {
        Delegate delegate;
        delegate.mObject = &object;
        delegate.mMethod = &exec<T, TMethod>;
        return delegate;
    }

    template<R(*TMethod)()>
    static Delegate create() {
        Delegate delegate;
        delegate.mObject = nullptr;
        delegate.mMethod = &exec<TMethod>;
        return delegate;
    }

    void destroy() {
        mObject = nullptr;
        mMethod = nullptr;
    }

    bool callable() {
        return (mMethod != nullptr);
    }

    R operator()() const {
        return (*mMethod)(mObject);
    }

    bool operator==(Delegate const& rhs) {
        return (mObject == rhs.mObject && mMethod == rhs.mMethod);
    }

private:
    typedef R (*MethodType)(void* object);

    template<class T, R(T::*TMethod)()>
    static R exec(void* object) {
        T* self = static_cast<T*>(object);
        return (self->*TMethod)();
    }

    template<R(*TMethod)()>
    static R exec(void* object) {
        return (*TMethod)();
    }

    void* mObject;
    MethodType mMethod;
};

} /* namespace mindroid */

#endif /* MINDROID_DELEGATE_H_ */
