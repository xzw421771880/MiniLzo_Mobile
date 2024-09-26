LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := jnidemo

#MY_FILES := $(wildcard $*.c)
LOCAL_SRC_FILES := jnitools.c minilzo.c
# 将收集到的.c文件添加到模块源代码列表中
# LOCAL_SRC_FILES := $(MY_FILES)

LOCAL_LDLIBS :=  -L$(SYSROOT)/usr/lib -llog

include $(BUILD_SHARED_LIBRARY)
