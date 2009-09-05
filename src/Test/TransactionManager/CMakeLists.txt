
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

add_executable(TransacionManagerTest Main.cpp)
target_link_libraries(TransacionManagerTest serverLib) 

#enable_testing()
add_test(TransacionManagerTest ${CMAKE_CURRENT_BINARY_DIR}/TransacionManagerTest)
