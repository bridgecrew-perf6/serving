set(openssl_USE_STATIC_LIBS ON)
set(libevent_CFLAGS "-fPIC -fvisibility=hidden -fstack-protector-all -D_FORTIFY_SOURCE=2 -O2")
if(NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(libevent_LDFLAGS "-Wl,-z,now")
endif()

if(NOT MINDSPORE_PROJECT_DIR)
    set(MINDSPORE_PROJECT_DIR ${CMAKE_SOURCE_DIR})
endif()

if(ENABLE_GITEE)
    set(REQ_URL "https://gitee.com/mirrors/libevent/repository/archive/release-2.1.12-stable.tar.gz")
    set(MD5 "0d5a27436bf7ff8253420c8cf09f47ca")
else()
    set(REQ_URL
        "https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz")
    set(MD5 "b5333f021f880fe76490d8a799cd79f4")
endif()

message("libevent using openssl stub dir: " ${openssl_ROOT})

mindspore_add_pkg(libevent
        VER 2.1.12
        LIBS event event_pthreads event_core event_openssl
        URL ${REQ_URL}
        MD5 ${MD5}
        PATCHES ${MINDSPORE_PROJECT_DIR}/third_party/patch/libevent/libevent.patch001
        CMAKE_OPTION -DCMAKE_BUILD_TYPE:STRING=Release -DBUILD_TESTING=OFF -DOPENSSL_ROOT_DIR:PATH=${openssl_ROOT}
        -DEVENT__LIBRARY_TYPE:STRING=STATIC)

include_directories(${libevent_INC})

add_library(mindspore_serving::event ALIAS libevent::event)
add_library(mindspore_serving::event_pthreads ALIAS libevent::event_pthreads)
add_library(mindspore_serving::event_core ALIAS libevent::event_core)
add_library(mindspore_serving::event_openssl ALIAS libevent::event_openssl)
