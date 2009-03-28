#
# Find the native TCMALLOC library
#

# This module defines
# TCMALLOC_LIBRARY, the libraries to link against to use TCMALLOC.
# TCMALLOC_FOUND, If false, do not try to use TCMALLOC.
# TCMALLOC_LIBS, link information
# TCMALLOC_CFLAGS, cflags for include information


INCLUDE(UsePkgConfig)

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
#PKGCONFIG(tcmalloc _tcmallocIncDir _tcmallocLinkDir _tcmallocLinkFlags _tcmallocCflags)

if (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    include(UsePkgConfig)
    pkgconfig(tcmalloc _TCMALLOC_INCLUDEDIR _TCMALLOC_LIBDIR _TCMALLOC_LDFLAGS _TCMALLOC_CFLAGS)
else (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
        pkg_check_modules(_TCMALLOC tcmalloc)
    endif (PKG_CONFIG_FOUND)
endif (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)

SET(TCMALLOC_LIBS ${_tcmallocCflags})


FIND_LIBRARY(TCMALLOC_LIBRARY NAMES tcmalloc libtcmalloc
    PATHS
    ${_tcmallocLinkDir}
    /usr/lib
    /usr/local/lib
    /usr/pkg/lib)

#SET(TCMALLOC_FIND_QUIETLY 1)

IF (TCMALLOC_LIBRARY)
    SET(TCMALLOC_FOUND "YES" )
    IF(NOT TCMALLOC_FIND_QUIETLY)
        MESSAGE(STATUS "Found tcmalloc: ${TCMALLOC_LIBRARY}")
    ENDIF(NOT TCMALLOC_FIND_QUIETLY)
    # SET(TCMALLOC_LIBRARIES ${TCMALLOC_LIBRARY})
ELSE (TCMALLOC_LIBRARY)
    MESSAGE(STATUS "Could NOT find tcmalloc")
ENDIF (TCMALLOC_LIBRARY)
