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
    SimpleClient.h
    Lsbql.cpp)

add_library(Client STATIC ${Client_STAT_SRCS})
target_link_libraries(Client Errors)
