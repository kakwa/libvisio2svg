cmake_minimum_required (VERSION 2.6)

project (vss2svg)

# Project version (sematic versionning)
set(vss2svg_VERSION_MAJOR 0)
set(vss2svg_VERSION_MINOR 1)
set(vss2svg_VERSION_PATCH 0)

set(vss2svg_VERSION ${vss2svg_VERSION_MAJOR}.${vss2svg_VERSION_MINOR}.${vss2svg_VERSION_PATCH})
# set version as a definition
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -DV2S_VERSION='\"${vss2svg_VERSION}\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DV2S_VERSION='\"${vss2svg_VERSION}\"'")

# Options
option(DEBUG     "compile with debug symbol"    OFF)
option(STATIC    "compile statically"           OFF)
option(USE_CLANG "build application with clang" OFF)
option(USE_GCC   "build application with gcc"   OFF)
option(FORCELE   "force little endian architecture"   OFF)

if(USE_CLANG)
    set(CMAKE_CXX_COMPILER "clang++")
    set(CMAKE_CC_COMPILER  "clang"  )
endif(USE_CLANG)

if(USE_GCC)
    set(CMAKE_CXX_COMPILER "g++")
    set(CMAKE_CC_COMPILER  "gcc")
endif(USE_GCC)

if(DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")
    set(CMAKE_BUILD_TYPE Debug)
endif(DEBUG)

if(STATIC)
    set(SHARED "")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBRARIES OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
else(STATIC)
    set(SHARED "SHARED")
    set(BUILD_SHARED_LIBRARIES ON)
endif(STATIC)

LINK_DIRECTORIES(
    /usr/local/lib
    /usr/lib/
    ${CMAKE_BINARY_DIR}/
)

include_directories(
    ./inc/
    /usr/local/include/
    /usr/include/
    /usr/include/librevenge-0.0/
    /usr/local/include/librevenge-0.0/
    /usr/include/libvisio-0.1/
    /usr/local/include/libvisio-0.1/
)


add_library(TitleGenerator
    ${SHARED}
    src/lib/visio2svg/TitleGenerator.cpp
)

add_library(Visio2Svg
    ${SHARED}
    src/lib/visio2svg/Visio2Svg.cpp
)

set_target_properties(Visio2Svg
    PROPERTIES
    VERSION ${vss2svg_VERSION}
    SOVERSION ${vss2svg_VERSION_MAJOR}
)

set_target_properties(TitleGenerator
    PROPERTIES
    VERSION ${vss2svg_VERSION}
    SOVERSION ${vss2svg_VERSION_MAJOR}
)

add_executable(vss2svg-conv src/conv/vss2svg.cpp)

target_link_libraries(vss2svg-conv
    revenge-0.0
    visio-0.1
    revenge-stream-0.0
    emf2svg
    TitleGenerator
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
INSTALL(TARGETS vss2svg-conv TitleGenerator
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

INSTALL(DIRECTORY inc/visio2svg DESTINATION "include")
