include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

add_executable(CBindingsTest test.c)
target_link_libraries(CBindingsTest loxim_client_shared)

add_test(CBindingsTest ${CMAKE_CURRNENT_BINARY_DIR}/CBindingsTest)
