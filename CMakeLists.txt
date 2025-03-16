cmake_minimum_required (VERSION 2.6)

project (vss2svg)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

find_package(libemf2svg REQUIRED)
find_package(libwmf REQUIRED)
find_package(librevenge REQUIRED)
find_package(libvisio REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(Freetype REQUIRED)

# Project version (sematic versionning)
set(vss2svg_VERSION_MAJOR 0)
set(vss2svg_VERSION_MINOR 5)
set(vss2svg_VERSION_PATCH 6)

set(vss2svg_VERSION ${vss2svg_VERSION_MAJOR}.${vss2svg_VERSION_MINOR}.${vss2svg_VERSION_PATCH})
# set version as a definition
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -DV2S_VERSION='\"${vss2svg_VERSION}\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DV2S_VERSION='\"${vss2svg_VERSION}\"'")

add_custom_target(tag
	COMMAND git tag -a ${vss2svg_VERSION} -m "tagging version ${vss2svg_VERSION}"
	COMMAND git push origin ${vss2svg_VERSION}
)

# Options
option(DEBUG               "compile with debug symbol"                                          OFF)
option(STATIC              "compile statically"                                                 OFF)
option(USE_CLANG           "build application with clang"                                       OFF)
option(USE_GCC             "build application with gcc"                                         OFF)
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

if(USE_GCC)
    set(CMAKE_CXX_COMPILER "g++")
    set(CMAKE_CC_COMPILER  "gcc")
endif(USE_GCC)

# Build external dependancies if we are on OSX
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   # Mac OS X specific code
   set(EXTERNAL_MEMSTREAM "memstream")
   set(EXTERNAL_ARGP "argp")
   add_definitions(-DDARWIN)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if(DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -DDEBUG")
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
)

include_directories(
    inc/
    ${LIBEMF2SVG_INCLUDE_DIR}
    ${LIBWMF_INCLUDE_DIR}
    ${LIBREVENGE_INCLUDE_DIR}
    ${LIBVISIO_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIR_ft2build}
    ${FREETYPE_INCLUDE_DIR_freetype2}
    ${LIBXML2_INCLUDE_DIR}
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
    ${LIBREVENGE_LIBRARIES}
    ${LIBVISIO_LIBRARIES}
)

target_link_libraries(Visio2Svg
    ${LIBREVENGE_LIBRARIES}
    ${LIBVISIO_LIBRARIES}
    ${LIBEMF2SVG_LIBRARIES}
    ${LIBWMF_LIBRARIES}
    ${LIBWMFLITE_LIBRARIES}
    ${LIBREVENGE_STREAM_LIBRARY}
    ${LIBXML2_LIBRARIES}
    TitleGenerator
    ${EXTERNAL_ARGP}
)

target_link_libraries(vss2svg-conv
    Visio2Svg
)

target_link_libraries(vsd2svg-conv
    Visio2Svg
)

if (NOT LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR lib)
endif ()

if (NOT BIN_INSTALL_DIR)
    set(BIN_INSTALL_DIR bin)
endif ()

if (NOT INCLUDE_INSTALL_DIR)
    set(INCLUDE_INSTALL_DIR include)
endif ()


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
INSTALL(TARGETS vss2svg-conv vsd2svg-conv TitleGenerator Visio2Svg
  RUNTIME DESTINATION ${BIN_INSTALL_DIR}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)

INSTALL(DIRECTORY inc/visio2svg DESTINATION ${INCLUDE_INSTALL_DIR})
