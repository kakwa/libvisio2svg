cmake_minimum_required (VERSION 2.6)

project (vss2svg)

# Project version (sematic versionning)
set(vss2svg_VERSION_MAJOR 0)
set(vss2svg_VERSION_MINOR 4)
set(vss2svg_VERSION_PATCH 1)

set(vss2svg_VERSION ${vss2svg_VERSION_MAJOR}.${vss2svg_VERSION_MINOR}.${vss2svg_VERSION_PATCH})
# set version as a definition
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -DV2S_VERSION='\"${vss2svg_VERSION}\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DV2S_VERSION='\"${vss2svg_VERSION}\"'")

add_custom_target(tag
	COMMAND git tag -a ${vss2svg_VERSION} -m "tagging version ${vss2svg_VERSION}"
	COMMAND git push origin ${vss2svg_VERSION}
)

# Options
option(DEBUG     "compile with debug symbol"    OFF)
option(STATIC    "compile statically"           OFF)
option(USE_CLANG "build application with clang" OFF)
option(USE_GCC   "build application with gcc"   OFF)
option(DEPS_PATH   "enable linking in ./tests/usr/local"   OFF)
option(FORCELE   "force little endian architecture"   OFF)
option(UNSAFE_FILENAME     "disable safe file naming (if your stdlib doesn't support regex)"    OFF)

if(UNSAFE_FILENAME)
else(UNSAFE_FILENAME)
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -DSAFE_FILENAME")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSAFE_FILENAME")
endif(UNSAFE_FILENAME)

if(USE_CLANG)
    set(CMAKE_CXX_COMPILER "clang++")
    set(CMAKE_CC_COMPILER  "clang"  )
endif(USE_CLANG)

if(DEPS_PATH)
    set(DEPS_INC_RVNG "${CMAKE_SOURCE_DIR}/tests/usr/local/include/librevenge-0.0/")
    set(DEPS_INC "${CMAKE_SOURCE_DIR}/tests/usr/local/include/")
    set(DEPS_INC_VISIO "${CMAKE_SOURCE_DIR}/tests/usr/local/include/libvisio-0.1/")
    set(DEPS_LINK  "${CMAKE_SOURCE_DIR}/tests/usr/local/lib"  )
endif(DEPS_PATH)


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
    ${DEPS_LINK}
)

include_directories(
    inc/
    /usr/local/include/
    /usr/include/
    /usr/local/include/freetype2
    /usr/include/freetype2
    /usr/include/librevenge-0.0/
    /usr/include/libxml2/
    /usr/local/include/librevenge-0.0/
    /usr/include/libvisio-0.1/
    /usr/local/include/libvisio-0.1/
    ${DEPS_INC_RVNG}
    ${DEPS_INC_VISIO}
    ${DEPS_INC}
)

add_executable(vss2svg-conv src/conv/vss2svg.cpp)
add_executable(vsd2svg-conv src/conv/vsd2svg.cpp)

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

target_link_libraries(TitleGenerator
    revenge-0.0
    visio-0.1
    revenge-stream-0.0
)

target_link_libraries(Visio2Svg
    revenge-0.0
    visio-0.1
    revenge-stream-0.0
    emf2svg
    wmf
    TitleGenerator
)

target_link_libraries(vss2svg-conv
    Visio2Svg
)

target_link_libraries(vsd2svg-conv
    Visio2Svg
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
INSTALL(TARGETS vss2svg-conv vsd2svg-conv TitleGenerator Visio2Svg
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

INSTALL(DIRECTORY inc/visio2svg DESTINATION "include")
