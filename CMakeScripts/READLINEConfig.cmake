#
# Find the native READLINE includes and library
#

# This module defines
# READLINE_INCLUDE_DIR, where to find art*.h etc
# READLINE_LIBRARY, the libraries to link against to use READLINE.
# READLINE_FOUND, If false, do not try to use READLINE.
# READLINE_LIBS, link information
# READLINE_CFLAGS, cflags for include information


INCLUDE(UsePkgConfig)

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
#PKGCONFIG(readline _readlineIncDir _readlineLinkDir _readlineLinkFlags _readlineCflags)

if (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    include(UsePkgConfig)
    pkgconfig(readline _READLINE_INCLUDEDIR _READLINE_LIBDIR _READLINE_LDFLAGS _READLINE_CFLAGS)
else (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
        pkg_check_modules(_READLINE readline)
    endif (PKG_CONFIG_FOUND)
endif (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)

SET(READLINE_LIBS ${_readlineCflags})

FIND_PATH(READLINE_INCLUDE_DIR readline/readline.h
    ${_readlineIncDir}
    /usr/include
    /usr/local/include/
    /usr/pkg/include/
    PATH_SUFFIXES
    readline
    libreadline)

FIND_LIBRARY(READLINE_LIBRARY NAMES readline libreadline
    PATHS
    ${_readlineLinkDir}
    /usr/lib
    /usr/local/lib
    /usr/pkg/lib)

#SET(READLINE_FIND_QUIETLY 1)

IF (READLINE_LIBRARY)
    IF (READLINE_INCLUDE_DIR)
        SET(READLINE_FOUND "YES" )
        IF(NOT READLINE_FIND_QUIETLY)
            MESSAGE(STATUS "Found readline: ${READLINE_LIBRARY}")
        ENDIF(NOT READLINE_FIND_QUIETLY)
        # SET(READLINE_LIBRARIES ${READLINE_LIBRARY})
    ELSE (READLINE_INCLUDE_DIR)
        MESSAGE(SEND_ERROR "Could NOT find readline")
    ENDIF (READLINE_INCLUDE_DIR)
ELSE (READLINE_LIBRARY)
    MESSAGE(SEND_ERROR "Could NOT find readline")
ENDIF (READLINE_LIBRARY)
