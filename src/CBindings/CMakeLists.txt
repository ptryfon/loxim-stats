
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(CBindings_SRCS
access.c
convert.cpp
exports.cpp
loxim_client.h)

set(CBindings_DEPS
Errors
Client
Util
ProtocolEnums
Protocol
ProtocolStreams
ProtocolPackages
ProtocolPackagesData
Config
)

#target_link_libraries should not be used
#all the source files shold be listed here
set(CBindings_ext_SRCS
)
add_library(loxim_client_static STATIC ${CBindings_SRCS} ${CBindings_ext_SRCS})
add_library(loxim_client_shared SHARED ${CBindings_SRCS} ${CBindings_ext_SRCS})

#this works:
target_link_libraries(loxim_client_shared ${CBindings_DEPS})
#but this does not:
#target_link_libraries(loxim_client_static ${CBindings_DEPS})
set_target_properties(loxim_client_static PROPERTIES OUTPUT_NAME loxim_client)
set_target_properties(loxim_client_shared PROPERTIES OUTPUT_NAME loxim_client)
set_target_properties(loxim_client_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(loxim_client_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)

install(TARGETS loxim_client_static loxim_client_shared DESTINATION lib)
install(FILES loxim_client.h DESTINATION include)

