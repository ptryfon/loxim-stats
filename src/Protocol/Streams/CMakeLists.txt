set(STREAMSDIR "../Protocol/Streams")

set(ProtocolStreams_SRCS
	${STREAMSDIR}/DataStream.cpp
	${STREAMSDIR}/DataStream.h
	${STREAMSDIR}/FileDataStream.cpp
	${STREAMSDIR}/FileDataStream.h
	${STREAMSDIR}/PackageCodec.cpp
	${STREAMSDIR}/PackageCodec.h
	${STREAMSDIR}/PackageStream.cpp
	${STREAMSDIR}/PackageStream.h
	${STREAMSDIR}/TCPClientStream.cpp
	${STREAMSDIR}/TCPClientStream.h CACHE INTERNAL "" )

