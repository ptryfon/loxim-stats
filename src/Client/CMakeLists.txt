include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(Client_STAT_SRCS
    Authenticator.cpp
    Authenticator.h
    ClientConsole.cpp
    ClientConsole.h
    ConsoleAuthenticator.cpp
    ConsoleAuthenticator.h
    Client.cpp
    Client.h
    SimpleClient.cpp
    SimpleClient.h)

add_library(Client STATIC ${Client_STAT_SRCS})
target_link_libraries(Client Errors)
set_target_properties(Client PROPERTIES OUTPUT_NAME client)
set_target_properties(Client PROPERTIES CLEAN_DIRECT_OUTPUT 1)


add_library(Client_shared SHARED ${Client_STAT_SRCS})
target_link_libraries(Client_shared Errors_shared)
set_target_properties(Client_shared PROPERTIES OUTPUT_NAME client)
set_target_properties(Client_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
