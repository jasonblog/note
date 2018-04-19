#指定编译的文件夹  指定当前文件目录
LOCAL_PATH := $(call my-dir)
#编译器会定义很多的临时变量，中间变量，最好清空所有的中间变量。
include $(CLEAR_VARS)
#编译出来模块的名称
LOCAL_MODULE    := hello-jni
# 编译的源代码的名称
LOCAL_SRC_FILES := hello-jni.c
#编译一个动态库，静态库
#静态库 文件名.a   包含所有的函数并且函数运行的依赖，体积大，包含所有的API
#动态库 文件名.so  包含函数，不包含函数运行的依赖，体积小，运行的时候，去操作系统寻找需要的API
include $(BUILD_SHARED_LIBRARY)