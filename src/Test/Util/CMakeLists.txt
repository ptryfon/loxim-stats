
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

add_executable(UtilThreadTest thread_test.cpp)
add_executable(spin_lock_test spin_lock_test.cpp)
add_executable(thread_test2 thread_test2.cpp)
add_executable(semaphore_test semaphore_test.cpp)

add_test(UtilThreadTest ${CMAKE_CURRNENT_BINARY_DIR}/UtilThreadTest)  
target_link_libraries(UtilThreadTest Util pthread Errors)

add_test(spin_lock_test ${CMAKE_CURRNENT_BINARY_DIR}/spin_lock_test)  
target_link_libraries(spin_lock_test Util pthread Errors)

add_test(thread_test2 ${CMAKE_CURRNENT_BINARY_DIR}/thread_test2)  
target_link_libraries(thread_test2 Util pthread Errors)

add_test(semaphore_test ${CMAKE_CURRNENT_BINARY_DIR}/semaphore_test)
target_link_libraries(semaphore_test Util pthread Errors)
