cmake_minimum_required (VERSION 2.6)
project (LoXiM)
set (LoXiM_VERSION_MAJOR 0)
set (LoXiM_VERSION_MINOR 01)
set (CMAKE_VERBOSE_MAKEFILE 1)
set (GLOBAL_DEPENDS_DEBUG_MODE 1)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Obcject oriented semistructual DBMS")
SET(CPACK_PACKAGE_VENDOR "Grzegorz Timoszuk")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "01")
#SET(CPACK_PACKAGE_VERSION_PATCH "2")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeScripts")

set(READLINE_DIR "${CMAKE_SOURCE_DIR}/CMakeScripts/") 
find_package(READLINE REQUIRED)

include (CPack)
include (CTest)

enable_testing()

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


if( ${FLEX_EXECUTABLE} )
    MESSAGE (STATUS "flex found -- ${FLEX_EXECUTABLE}")
endif(${FLEX_EXECUTABLE}) 

if( ${BISON_EXECUTABLE} )
    MESSAGE (STATUS "bison found -- ${BISON_EXECUTABLE}")
endif ( ${BISON_EXECUTABLE})

add_subdirectory (src)
add_subdirectory (Documentation)


