#GLOBAL CMAKE SETTINGS 

set (CMAKE_VERBOSE_MAKEFILE 0)

################################################################################


#DEBUG INFO
#MESSAGE(STATUS "DEB CXX ${CMAKE_CXX_FLAGS_DEBUG}")
#MESSAGE(STATUS "REL CXX ${CMAKE_CXX_FLAGS_RELEASE}")
#MESSAGE(STATUS "DEB C ${CMAKE_C_FLAGS_DEBUG}")
#MESSAGE(STATUS "REL C ${CMAKE_C_FLAGS_RELEASE}")
#MESSAGE(STATUS "NONE LINK ${CMAKE_EXE_LINKER_FLAGS}")
#MESSAGE(STATUS "REL LINK ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
#MESSAGE(STATUS "DEB LINK  ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")

################################################################################

#BUILD FLAGS DEFINITIONS 

SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "-fprofile-arcs -ftest-coverage")

SET(CMAKE_CXX_FLAGS_RELEASE "-03 -DNDEBUG -fomit-frame-pointer")
SET(CMAKE_C_FLAGS_RELEASE "-03 -DNDEBUG -fomit-frame-pointer")

#BUILD TYPE DEFINITION

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF(NOT CMAKE_BUILD_TYPE)
MESSAGE(STATUS "CURRNET BUILD TYPE -- ${CMAKE_BUILD_TYPE}")

################################################################################

cmake_minimum_required (VERSION 2.6)

#Definitions for packaging
project (LoXiM)
set (LoXiM_VERSION_MAJOR 0)
set (LoXiM_VERSION_MINOR 01)
set (GLOBAL_DEPENDS_DEBUG_MODE 1)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Obcject oriented semistructual DBMS")
SET(CPACK_PACKAGE_VENDOR "Grzegorz Timoszuk")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "01")
#SET(CPACK_PACKAGE_VERSION_PATCH "2")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "LoXiM-${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
MESSAGE (STATUS "CPACK_SOURCE_INGNORE_FILES ${CPACK_SOURCE_INGNORE_FILES}")
SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_SOURCE_IGNORE_FILES} CMakeFiles/ CMakeCache.txt _CPack_Packages .svn/ CPackConfig.cmake DartConfiguration.tcl CPackSourceConfig.cmake LoXiM-0.01.1* Protogen/ lw_protogen/ cmake_install.cmake CTestTestfile.cmake install_manifest.txt "\\\\.swp$" "~$" "\\\\.a$" "\\\\.tmp$" loxim_server lsbql ) 
MESSAGE (STATUS "CPACK_SOURCE_INGNORE_FILES ${CPACK_SOURCE_INGNORE_FILES}")

#path to addtional modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeScripts")

#set(INCDIR
#    ${CMAKE_SOURCE_DIR}/src/
#    ${CMAKE_SOURCE_DIR}/src/AdminParser/
#    ${CMAKE_SOURCE_DIR}/src/ARIESLog/
#    ${CMAKE_SOURCE_DIR}/src/CBindings/
#    ${CMAKE_SOURCE_DIR}/src/Client/
#    ${CMAKE_SOURCE_DIR}/src/config/
#    ${CMAKE_SOURCE_DIR}/src/Errors/
#    ${CMAKE_SOURCE_DIR}/src/Indexes/
#    ${CMAKE_SOURCE_DIR}/Lock/
#    ${CMAKE_SOURCE_DIR}/Log/
#    ${CMAKE_SOURCE_DIR}/Protocol/
#    ${CMAKE_SOURCE_DIR}/Protocol/Enums/
#    ${CMAKE_SOURCE_DIR}/Protocol/Packages/
#    ${CMAKE_SOURCE_DIR}/Protocol/Packages/Data/
#    ${CMAKE_SOURCE_DIR}/Protocol/Streams/
#    ${CMAKE_SOURCE_DIR}/QueryExecutor/
#    ${CMAKE_SOURCE_DIR}/QueryParser/
#    ${CMAKE_SOURCE_DIR}/Server/
#    ${CMAKE_SOURCE_DIR}/Store/
#    ${CMAKE_SOURCE_DIR}/SystemStats/
#    ${CMAKE_SOURCE_DIR}/TransactionManager/
#    ${CMAKE_SOURCE_DIR}/Util/
#    ${CMAKE_SOURCE_DIR}/TypeCheck/)

#programs and libs required by LoXiM to build

set(READLINE_DIR "${CMAKE_SOURCE_DIR}/CMakeScripts/") 
find_package(READLINE REQUIRED)

find_program(BISON_EXECUTABLE bison
    PATHS 
    /usr/bin
    /bin
    /usr/pkg/bin
    /usr/pkg/usr/bin)

find_program(FLEX_EXECUTABLE flex
    PATHS 
    /usr/bin
    /bin
    /usr/pkg/bin
    /usr/pkg/usr/bin)

if(BISON_EXECUTABLE)
    MESSAGE (STATUS "bison found -- ${BISON_EXECUTABLE}")
else (BISON_EXECUTABLE)
    MESSAGE (FATAL_ERROR "bison NOT found")
endif (BISON_EXECUTABLE)

if(FLEX_EXECUTABLE)
    MESSAGE (STATUS "flex found -- ${FLEX_EXECUTABLE}")
else (FLEX_EXECUTABLE)
    MESSAGE (FATAL_ERROR "flex NOT found")
endif(FLEX_EXECUTABLE) 

###############################################################################



#Libraries needed by googletest - unit testing framework

set(GOOGLETEST_DIR "${CMAKE_SOURCE_DIR}/CMakeScripts/") 
find_package(GOOGLETEST)

include(FindPythonInterp)
include(FindPythonLibs)

find_package (PythonInterp)
find_package (PythonLibs)
if (PYTHONINTERP_FOUND) 
    MESSAGE(STATUS "python found -- ${PYTHON_EXECUTABLE}")
endif (PYTHONINTERP_FOUND) 

if (PYTHONLIBS_FOUND)
    MESSAGE(STATUS "python libs --  ${PYTHON_LIBRARIES}, includes ${PYTHON_INCLUDE_PATH}")
endif ( PYTHONLIBS_FOUND )

if ( GOOGLETEST_FOUND )
    if ( PYTHONINTERP_FOUND ) 
        if ( PYTHONLIBS_FOUND )
            if ( ${CMAKE_BUILD_TYPE} MATCHES  "Debug" )
                SET ( G_UNIT_TESTS_ENABLED 1 )
                #                MESSAGE(STATUS "HHHHHHHUUUUUUUUURRRRA")
            endif ( ${CMAKE_BUILD_TYPE} MATCHES  "Debug" )
        endif ( PYTHONLIBS_FOUND )
    endif ( PYTHONINTERP_FOUND ) 
endif ( GOOGLETEST_FOUND )

################################################################################


#Libraries and programs used during optmimalization tests

set(TCMALLOC_DIR "${CMAKE_SOURCE_DIR}/CMakeScripts/") 
find_package(TCMALLOC)

if ( USE_TCMALLOC ) 
    if ( TCMALLOC_FOUND ) 
        SET ( G_PERF_TOOLS_TESTS_ENABLED 1 )
        #        MESSAGE ( STATUS "ALA MA KOTA")
    endif ( TCMALLOC_FOUND )
endif ( USE_TCMALLOC ) 

find_program(VALGRIND_EXECUTABLE valgrind valgrind.bin 
    PATHS 
    /usr/bin
    /bin
    /usr/pkg/bin
    /usr/pkg/usr/bin)

if(VALGRIND_EXECUTABLE) 
    MESSAGE(STATUS "Found valgrind  -- ${VALGRIND_EXECUTABLE}")
    SET ( VALGRING_TESTS_ENABLED 1 )
else (VALGRIND_EXECUTABLE) 
    MESSAGE(STATUS "Valgrind not found")
endif(VALGRIND_EXECUTABLE) 

if ( G_PERF_TOOLS_TESTS_ENABLED ) 
    if ( VALGRING_TESTS_ENABLED )
        SET ( VALGRING_TESTS_ENABLED 0 )
    endif ( VALGRING_TESTS_ENABLED )
endif ( G_PERF_TOOLS_TESTS_ENABLED )

###############################################################################


include (CPack)
include (CTest)

enable_testing()


#recursive build for subdirectiories
if (FLEX_EXECUTABLE)
    if(BISON_EXECUTABLE) 
        add_subdirectory (src)
        add_subdirectory (Documentation)
    endif (BISON_EXECUTABLE) 
endif (FLEX_EXECUTABLE) 


#LOGGING LEVEL DEFINITON


################################################################################


#CMAKE DEBUG INFO 
MESSAGE(STATUS "DEB CXX ${CMAKE_CXX_FLAGS_DEBUG}")
MESSAGE(STATUS "REL CXX ${CMAKE_CXX_FLAGS_RELEASE}")
MESSAGE(STATUS "DEB C ${CMAKE_C_FLAGS_DEBUG}")
MESSAGE(STATUS "REL C ${CMAKE_C_FLAGS_RELEASE}")
MESSAGE(STATUS "NONE LINK ${CMAKE_EXE_LINKER_FLAGS}")
MESSAGE(STATUS "REL LINK ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
MESSAGE(STATUS "DEB LINK  ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
################################################################################
