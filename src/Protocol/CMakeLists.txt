# This file is generated by lw_protogen. DO NOT EDIT.

add_subdirectory(Enums)
add_subdirectory(Packages)
add_subdirectory(Streams)

set(PROTOCOLDIR "../Protocol")

set(Protocol_SRCS
	${PROTOCOLDIR}/Exceptions.cpp 
	${PROTOCOLDIR}/Exceptions.h 
	${PROTOCOLDIR}/TCPServer.h 
	${PROTOCOLDIR}/TCPServer.cpp 
	${PROTOCOLDIR}/ByteBuffer.h 
	${PROTOCOLDIR}/ByteBuffer.cpp 
	${PROTOCOLDIR}/DataPackageFactory.h 
	${PROTOCOLDIR}/DataPackageFactory.cpp  
	${PROTOCOLDIR}/PackageFactory.h  
	${PROTOCOLDIR}/PackageFactory.cpp  
	${PROTOCOLDIR}/VarUint.h CACHE INTERNAL "" )


set(ProtocolAllInOne_SRCS 
	${Protocol_SRCS} ${ProtocolEnums_SRCS} ${ProtocolStreams_SRCS} ${ProtocolPackages_SRCS} ${ProtocolPackagesData_SRCS} CACHE INTERNAL "" )


