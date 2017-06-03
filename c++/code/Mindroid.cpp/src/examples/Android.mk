LOCAL_PATH:= $(call my-dir)

# ---- HelloWorld ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	HelloWorld.cpp

LOCAL_CFLAGS := -DANDROID -Wno-unused-parameter -fexceptions

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils

LOCAL_STATIC_LIBRARIES := \
	libmindroid

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= HelloWorld

include $(BUILD_EXECUTABLE)

# ---- ConcurrencyMess ----

# include $(CLEAR_VARS)

# LOCAL_C_INCLUDES := \
# 	$(LOCAL_PATH)/..

# LOCAL_SRC_FILES:= \
# 	ConcurrencyMess.cpp

# LOCAL_CFLAGS := -DANDROID -Wno-unused-parameter

# LOCAL_SHARED_LIBRARIES := \
# 	libutils \
# 	libcutils

# LOCAL_STATIC_LIBRARIES := \
# 	libmindroid

# LOCAL_MODULE_TAGS := optional
# LOCAL_MODULE:= ConcurrencyMess

# include $(BUILD_EXECUTABLE)

# ---- AsyncTasks ----

# include $(CLEAR_VARS)

# LOCAL_C_INCLUDES := \
# 	$(LOCAL_PATH)/..

# LOCAL_SRC_FILES:= \
# 	AsyncTasks.cpp

# LOCAL_CFLAGS := -DANDROID -Wno-unused-parameter

# LOCAL_SHARED_LIBRARIES := \
# 	libutils \
# 	libcutils

# LOCAL_STATIC_LIBRARIES := \
# 	libmindroid

# LOCAL_MODULE_TAGS := optional
# LOCAL_MODULE:= AsyncTasks

# include $(BUILD_EXECUTABLE)

# ---- Networking ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	Networking.cpp

LOCAL_CFLAGS := -DANDROID -Wno-unused-parameter -fexceptions

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils

LOCAL_STATIC_LIBRARIES := \
	libmindroid

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= Networking

include $(BUILD_EXECUTABLE)

# ---- Services ----

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/..

LOCAL_SRC_FILES:= \
	Services.cpp

LOCAL_CFLAGS := -DANDROID -Wno-unused-parameter -fexceptions

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils

LOCAL_STATIC_LIBRARIES := \
	libmindroid \
	libtinyxml2

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := Services
LOCAL_CFLAGS := -DANDROID -Wno-unused-parameter -fexceptions
LOCAL_STRIP_MODULE := keep_symbols

include $(BUILD_EXECUTABLE)

