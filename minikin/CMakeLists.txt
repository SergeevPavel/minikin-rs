cmake_minimum_required(VERSION 3.16)
project(minikin)

set(CMAKE_CXX_STANDARD 17)

include_directories(third-party/harfbuzz/src)
include_directories(third-party/icu/icu4c/source/common)
include_directories(src)

FILE(GLOB minikin src/minikin/*.cpp)
FILE(GLOB log src/log/*.cc)
FILE(GLOB utils src/utils/*.cpp)
FILE(GLOB bindings src/bindings/*.cpp)

link_directories("packages/lib")

add_library(minikin STATIC library.cpp library.h ${minikin} ${log} ${utils} ${bindings})
#target_link_libraries(minikin "harfbuzz")
#target_link_libraries(minikin "icuuc")
#target_link_libraries(minikin "icudata")

#add_executable(app main.cpp)
#target_link_libraries(app minikin)
#target_link_libraries(app "harfbuzz")
#target_link_libraries(app "icuuc")
#target_link_libraries(app "icudata")

install (TARGETS minikin DESTINATION .)