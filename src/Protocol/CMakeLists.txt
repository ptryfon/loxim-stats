include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

add_subdirectory(Streams)
add_subdirectory(Packages)
add_subdirectory(Enums)



set(Protocol_STAT_SRCS
    ByteBuffer.cpp
    ByteBuffer.h
    DataPackageFactory.cpp
    DataPackageFactory.h
    Exceptions.cpp
    Exceptions.h
    PackageFactory.cpp
    PackageFactory.h
    TCPServer.cpp
    TCPServer.h
    VarUint.h)

add_library(Protocol STATIC ${Protocol_STAT_SRCS})
target_link_libraries(Protocol ProtocolPackages ProtocolPackagesData)


