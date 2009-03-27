#
# Find the native GOOGLETEST includes and library
#

# This module defines
# GOOGLETEST_INCLUDE_DIR, where to find art*.h etc
# GOOGLETEST_LIBRARY, the libraries to link against to use GOOGLETEST.
# GOOGLETEST_FOUND, If false, do not try to use GOOGLETEST.
# GOOGLETEST_LIBS, link information
# GOOGLETEST_CFLAGS, cflags for include information


INCLUDE(UsePkgConfig)

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
#PKGCONFIG(gtest _gtestIncDir _gtestLinkDir _gtestLinkFlags _gtestCflags)

if (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    include(UsePkgConfig)
    pkgconfig(gtest _GOOGLETEST_INCLUDEDIR _GOOGLETEST_LIBDIR _GOOGLETEST_LDFLAGS _GOOGLETEST_CFLAGS)
else (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
        pkg_check_modules(_GOOGLETEST gtest)
    endif (PKG_CONFIG_FOUND)
endif (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)

SET(GOOGLETEST_LIBS ${_gtestCflags})

FIND_PATH(GOOGLETEST_INCLUDE_DIR gtest/gtest.h
    ${_gtestIncDir}
    /usr/include
    /usr/local/include/
    /usr/pkg/include/
    PATH_SUFFIXES
    gtest
    libgtest)

FIND_LIBRARY(GOOGLETEST_LIBRARY NAMES gtest libgtest
    PATHS
    ${_gtestLinkDir}
    /usr/lib
    /usr/local/lib
    /usr/pkg/lib)

#SET(GOOGLETEST_FIND_QUIETLY 1)

IF (GOOGLETEST_LIBRARY)
    IF (GOOGLETEST_INCLUDE_DIR)
        SET(GOOGLETEST_FOUND "YES" )
        IF(NOT GOOGLETEST_FIND_QUIETLY)
            MESSAGE(STATUS "Found gtest: ${GOOGLETEST_LIBRARY}")
        ENDIF(NOT GOOGLETEST_FIND_QUIETLY)
        # SET(GOOGLETEST_LIBRARIES ${GOOGLETEST_LIBRARY})
    ELSE (GOOGLETEST_INCLUDE_DIR)
        MESSAGE(SEND_ERROR "Could NOT find gtest")
    ENDIF (GOOGLETEST_INCLUDE_DIR)
ELSE (GOOGLETEST_LIBRARY)
    MESSAGE(SEND_ERROR "Could NOT find gtest")
ENDIF (GOOGLETEST_LIBRARY)
