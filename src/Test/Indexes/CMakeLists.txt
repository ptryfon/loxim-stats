#
#include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})
#
#set(Indexes_Test_STAT_SRCS
#BTreeTest.cpp
#Bugs.cpp
#CleanTest.cpp
#ComparatorTest.cpp
#ConnectionThread.cpp
#ConnectionThread.h
#DropNodes.cpp
#ImplicitCalling.cpp
#IntegrationTest.cpp
#NodeBuffer.cpp
#ParentName.cpp
#ScopeSelection.cpp
#StateSaving.cpp
#Suits.h
#Syntax.cpp
#Tester.cpp
#Tester.h)
#
#add_library(indexesTestLib STATIC ${Indexes_Test_STAT_SRCS})
#
#add_executable(testRun testRun.cpp)
#target_link_libraries(testRun indexesTestLib)
#
#add_test(IndexesTests ${CMAKE_CURRNENT_BINARY_DIR}/testRun)
