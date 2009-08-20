
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

add_executable(UtilThreadTest thread_test.cpp)
add_executable(spin_lock_test spin_lock_test.cpp)

add_test(UtilThreadTest ${CMAKE_CURRNENT_BINARY_DIR}/UtilThreadTest)  
target_link_libraries(UtilThreadTest Util pthread Errors)

add_test(spin_lock_test ${CMAKE_CURRNENT_BINARY_DIR}/spin_lock_test)  
target_link_libraries(spin_lock_test Util pthread Errors)
