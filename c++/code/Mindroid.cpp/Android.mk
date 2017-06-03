#  Copyright (c) 2016 E.S.R. Labs. All rights reserved.
#
#  NOTICE:  All information contained herein is, and remains
#  the property of E.S.R.Labs and its suppliers, if any.
#  The intellectual and technical concepts contained herein are
#  proprietary to E.S.R.Labs and its suppliers and may be covered
#  by German and Foreign Patents, patents in process, and are protected
#  by trade secret or copyright law.
#  Dissemination of this information or reproduction of this material
#  is strictly forbidden unless prior written permission is obtained
#  from E.S.R.Labs.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/mindroid/app/SharedPreferencesImpl.cpp \
	src/mindroid/app/IOnSharedPreferenceChangeListener.cpp \
	src/mindroid/app/Service.cpp \
	src/mindroid/app/ContextImpl.cpp \
	src/mindroid/content/Context.cpp \
	src/mindroid/content/ComponentName.cpp \
	src/mindroid/content/ContextWrapper.cpp \
	src/mindroid/content/pm/IPackageManager.cpp \
	src/mindroid/content/pm/PackageManagerService.cpp \
	src/mindroid/content/Intent.cpp \
	src/mindroid/io/File.cpp \
	src/mindroid/lang/Boolean.cpp \
	src/mindroid/lang/Byte.cpp \
	src/mindroid/lang/Character.cpp \
	src/mindroid/lang/Class.cpp \
	src/mindroid/lang/Closure.cpp \
	src/mindroid/lang/Double.cpp \
	src/mindroid/lang/Float.cpp \
	src/mindroid/lang/Integer.cpp \
	src/mindroid/lang/Long.cpp \
	src/mindroid/lang/Object.cpp \
	src/mindroid/lang/Short.cpp \
	src/mindroid/lang/String.cpp \
	src/mindroid/lang/System.cpp \
	src/mindroid/lang/Thread.cpp \
	src/mindroid/net/DatagramSocket.cpp \
	src/mindroid/net/SocketAddress.cpp \
	src/mindroid/net/Socket4Address.cpp \
	src/mindroid/net/Socket6Address.cpp \
	src/mindroid/net/Socket.cpp \
	src/mindroid/net/ServerSocket.cpp \
	src/mindroid/os/Environment.cpp \
	src/mindroid/os/Handler.cpp \
	src/mindroid/os/Message.cpp \
	src/mindroid/os/Binder.cpp \
	src/mindroid/os/IRemoteCallback.cpp \
	src/mindroid/os/Process.cpp \
	src/mindroid/os/Bundle.cpp \
	src/mindroid/os/HandlerThread.cpp \
	src/mindroid/os/IProcess.cpp \
	src/mindroid/os/IServiceManager.cpp \
	src/mindroid/os/ServiceManager.cpp \
	src/mindroid/os/Looper.cpp \
	src/mindroid/os/AsyncTask.cpp \
	src/mindroid/os/SystemClock.cpp \
	src/mindroid/os/MessageQueue.cpp \
	src/mindroid/util/Assert.cpp \
	src/mindroid/util/Variant.cpp \
	src/mindroid/util/concurrent/SerialExecutor.cpp \
	src/mindroid/util/concurrent/atomic/AtomicInteger.cpp \
	src/mindroid/util/concurrent/locks/ReentrantLock.cpp \
	src/mindroid/util/concurrent/locks/ConditionImpl.cpp \
	src/mindroid/util/concurrent/ThreadPoolExecutor.cpp \
	src/mindroid/util/concurrent/Semaphore.cpp \
	src/mindroid/util/Log.cpp \
	src/mindroid/util/logging/ConsoleHandler.cpp \
	src/mindroid/util/logging/FileHandler.cpp \
	src/mindroid/util/logging/LogBuffer.cpp \
	src/mindroid/util/logging/Logger.cpp

LOCAL_CFLAGS := -DANDROID -Wno-unused-parameter -fexceptions
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= libmindroid
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src
LOCAL_RTTI_FLAG := -frtti
include $(BUILD_STATIC_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
