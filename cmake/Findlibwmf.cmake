
#if (NOT LIBWMF_LIBRARIES)

find_path(LIBWMF_INCLUDE_DIR
    NAMES libwmf/api.h api.h
    HINTS ${WMF_PKGCONF_INCLUDE_DIRS} ${WMF_PKGCONF_INCLUDEDIR}
    PATH_SUFFIXES include include/libwmf/
)

#endif ()

if (NOT LIBWMF_LIBRARIES)
    find_library(LIBWMF_LIBRARIES NAMES wmf ${_LIBWMF_PATHS} PATH_SUFFIXES lib)
endif ()

MESSAGE( STATUS "Find Header Directory for libwmf: " ${LIBWMF_INCLUDE_DIR})
MESSAGE( STATUS "Find Dynamic Library for libwmf:  " ${LIBWMF_LIBRARIES})
