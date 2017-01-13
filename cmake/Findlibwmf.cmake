if (NOT LIBWMF_LIBRARIES)
    find_path(LIBWMF_INCLUDE_DIR wmf.h ${_LIBWMF_PATHS} PATH_SUFFIXES include include/wmf/)
endif ()

if (NOT LIBWMF_LIBRARIES)
    find_library(LIBWMF_LIBRARIES NAMES wmf ${_LIBWMF_PATHS} PATH_SUFFIXES lib)
endif ()
