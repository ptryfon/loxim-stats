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
set_target_properties(ProtocolStreams PROPERTIES OUTPUT_NAME protocolstreams)
set_target_properties(ProtocolStreams PROPERTIES CLEAN_DIRECT_OUTPUT 1)

IF (NOT DO_NOT_BUILD_SHARED)
	add_library(ProtocolStreams_shared SHARED ${ProtocolStreams_STAT_SRCS})
	#target_link_libraries(ProtocolStreams_shared Protocol_shared)
	set_target_properties(ProtocolStreams_shared PROPERTIES OUTPUT_NAME protocolstreams)
	set_target_properties(ProtocolStreams_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
ENDIF (NOT DO_NOT_BUILD_SHARED)
