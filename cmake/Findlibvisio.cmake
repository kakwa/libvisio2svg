# - Try to find LibVisio
# Once done this will define
#
#  LIBVISIO_FOUND       - libvisio is available
#  LIBVISIO_INCLUDE_DIRS - include directory, e.g. /usr/include
#  LIBVISIO_LIBRARIES   - the libraries needed to use LibVisio
#  LIBVISIO_DEFINITIONS - Compiler switches required for using LibVisio
#
# Copyright (C) 2013 Yue Liu <yue.liu@mail.com>
# Redistribution and use is allowed according to the terms of the BSD license.

include(LibFindMacros)

find_path(LIBVISIO_INCLUDE_DIR
    NAMES libvisio/libvisio.h
    HINTS ${LIBVISIO_PKGCONF_INCLUDE_DIRS} ${LIBVISIO_PKGCONF_INCLUDEDIR}
    PATH_SUFFIXES libvisio-0.1
)

find_library(LIBVISIO_LIBRARY
    NAMES visio visio-0.1
    HINTS ${LIBVISIO_PKGCONF_LIBRARY_DIRS} ${LIBVISIO_PKGCONF_LIBDIR}
)

set(LIBVISIO_PROCESS_LIBS LIBVISIO_LIBRARY LIBWPD_LIBRARIES LIBWPG_LIBRARIES)
libfind_process(LIBVISIO)

MESSAGE( STATUS "Find Header Directory for libvisio: " ${LIBVISIO_INCLUDE_DIR})
MESSAGE( STATUS "Find Dynamic Library for libvisio:  " ${LIBVISIO_LIBRARIES})
