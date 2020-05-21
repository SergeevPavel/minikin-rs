cmake_minimum_required(VERSION 3.16)
project(minikin)

set(CMAKE_CXX_STANDARD 17)

option(MSVC_DYNAMIC_RT OFF)

include_directories(third-party/harfbuzz/src)
include_directories(third-party/icu/icu4c/source/common)
include_directories(src)

FILE(GLOB minikin src/minikin/*.cpp)
FILE(GLOB log src/log/*.cc)
FILE(GLOB utils src/utils/*.cpp)

add_library(minikin STATIC library.cpp library.h ${minikin} ${log} ${utils})
set_property(TARGET minikin PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:${MSVC_DYNAMIC_RT}>:DLL>")

install (TARGETS minikin DESTINATION .)