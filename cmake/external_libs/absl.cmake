mindspore_add_pkg(absl
        VER 20200225.2
        LIBS absl_strings absl_throw_delegate absl_raw_logging_internal absl_int128 absl_bad_optional_access
        URL https://github.com/abseil/abseil-cpp/archive/20200225.2.tar.gz
        MD5 73f2b6e72f1599a9139170c29482ddc4
        CMAKE_OPTION -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=TRUE)

include_directories(${absl_INC})

add_library(mindspore_serving::absl_strings ALIAS absl::absl_strings)
add_library(mindspore_serving::absl_throw_delegate ALIAS absl::absl_throw_delegate)
add_library(mindspore_serving::absl_raw_logging_internal ALIAS absl::absl_raw_logging_internal)
add_library(mindspore_serving::absl_int128 ALIAS absl::absl_int128)
add_library(mindspore_serving::absl_bad_optional_access ALIAS absl::absl_bad_optional_access)
