## DO NOT MODIFY, this file is auto generated using lw_protogen

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(ProtocolStreams_STAT_SRCS
    DataStream.cpp
    DataStream.h
    FileDataStream.cpp
    FileDataStream.h
    PackageCodec.cpp
    PackageCodec.h
    PackageStream.cpp
    PackageStream.h
    TCPClientStream.cpp
    TCPClientStream.h)

add_library(ProtocolStreams STATIC ${ProtocolStreams_STAT_SRCS})
target_link_libraries(ProtocolStreams Protocol)

