if (NOT LIBEMF2SVG_LIBRARIES)
    find_path(LIBEMF2SVG_INCLUDE_DIR emf2svg.h ${_LIBEMF2SVG_PATHS} PATH_SUFFIXES include include/emf2svg/)
endif ()

if (NOT LIBEMF2SVG_LIBRARIES)
    find_library(LIBEMF2SVG_LIBRARIES NAMES emf2svg ${_LIBEMF2SVG_PATHS} PATH_SUFFIXES lib)
endif ()

MESSAGE(STATUS "Find Header Directory for libemf2svg: " ${LIBEMF2SVG_INCLUDE_DIR})
MESSAGE(STATUS "Find Dynamic Library for libemf2svg:  " ${LIBEMF2SVG_LIBRARIES})
