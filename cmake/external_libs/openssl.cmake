if(ENABLE_GITEE)
    set(REQ_URL "https://gitee.com/mirrors/openssl/repository/archive/OpenSSL_1_1_1k.tar.gz")
    set(MD5 "bdd51a68ad74618dd2519da8e0bcc759")
else()
    set(REQ_URL "https://github.com/openssl/openssl/archive/refs/tags/OpenSSL_1_1_1k.tar.gz")
    set(MD5 "bdd51a68ad74618dd2519da8e0bcc759")
endif()

set(OPENSSL_PATCH_ROOT ${CMAKE_SOURCE_DIR}/third_party/patch/openssl)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR APPLE)
    set(openssl_CFLAGS -fvisibility=hidden)
    mindspore_add_pkg(openssl
            VER 1.1.1k
            LIBS ssl crypto
            URL ${REQ_URL}
            MD5 ${MD5}
            CONFIGURE_COMMAND ./config no-zlib no-shared
            PATCHES ${OPENSSL_PATCH_ROOT}/CVE-2021-3711.patch
            PATCHES ${OPENSSL_PATCH_ROOT}/CVE-2021-3712.patch
            PATCHES ${OPENSSL_PATCH_ROOT}/CVE-2022-0778.patch
            )
    include_directories(${openssl_INC})
    add_library(mindspore_serving::ssl ALIAS openssl::ssl)
    add_library(mindspore_serving::crypto ALIAS openssl::crypto)
endif()
