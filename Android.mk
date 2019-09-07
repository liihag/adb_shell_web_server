LOCAL_PATH := $(call my-dir)
INCLUDES_PATH:= $(LOCAL_PATH)
INCLUDES_ROOT:= .

include $(CLEAR_VARS)

APP_STL := stlport_static
LOCAL_MODULE:= wisp_server
LOCAL_SRC_FILES:=  \
									tinyxml/tinyxmlparser.cpp \
									tinyxml/tinyxmlerror.cpp \
									tinyxml/tinyxml.cpp \
									tinyxml/tinystr.cpp \
									util/debug.cpp \
									util/util.cpp  \
									libmicrohttpd/src/microhttpd/base64.c \
									libmicrohttpd/src/microhttpd/basicauth.c \
									libmicrohttpd/src/microhttpd/connection.c \
									libmicrohttpd/src/microhttpd/daemon.c \
									libmicrohttpd/src/microhttpd/digestauth.c \
									libmicrohttpd/src/microhttpd/internal.c \
									libmicrohttpd/src/microhttpd/md5.c \
									libmicrohttpd/src/microhttpd/memorypool.c \
									libmicrohttpd/src/microhttpd/postprocessor.c \
									libmicrohttpd/src/microhttpd/reason_phrase.c \
									libmicrohttpd/src/microhttpd/response.c \
									libmicrohttpd/src/microhttpd/tsearch.c \
									wisp_server.cpp
									

LOCAL_C_INCLUDES += \
        $(INCLUDES_PATH)/  \
        $(INCLUDES_PATH)/tinyxml \
        $(INCLUDES_PATH)/util/ \
        $(INCLUDES_PATH)/libmicrohttpd \
        $(INCLUDES_PATH)/libmicrohttpd/src/include \
        $(INCLUDES_PATH)/libmicrohttpd/src/include/plibc \
        $(INCLUDES_PATH)/libmicrohttpd/src/microhttpd \
     	$(INCLUDES_ROOT)/prebuilts/ndk/current/sources/cxx-stl/stlport/stlport/

LOCAL_SHARED_LIBRARIES += libandroid liblog libcutils libdl libbinder
LOCAL_LDLIBS += -landroid -lcutils  -ldl
LOCAL_LDFLAGS :=$(INCLUDES_ROOT)/prebuilts/ndk/current/sources/cxx-stl/stlport/libs/armeabi/*.a




include $(BUILD_EXECUTABLE)


