
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

add_executable(UtilThreadTest thread_test.cpp)

add_test(UtilThreadTest ${CMAKE_CURRNENT_BINARY_DIR}/UtilThreadTest)  
target_link_libraries(UtilThreadTest Util pthread Errors)
