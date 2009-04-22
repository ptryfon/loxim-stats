
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(CBindings_SRCS
access.c
convert.cpp
exports.cpp
loxim_client.h)

set(CBindings_DEPS
Config
Errors
Client
Util
Protocol
ProtocolStreams
ProtocolPackages
ProtocolPackagesData
SystemStats
)

#TODO: use CBindings_DEPS to generate this list instead of hardcoding it
set(CBindings_ext_SRCS
../Config/SBQLConfig.cpp
../Errors/Exceptions.cpp
../Errors/ErrorConsole.cpp
../Errors/ErrorMsg.cpp
../Client/Authenticator.cpp
../Client/Client.cpp
../Client/ClientConsole.cpp
../Client/ConsoleAuthenticator.cpp
../Client/SimpleClient.cpp
../Util/Backtrace.cpp
../Util/Concurrency.cpp
../Util/InfLoopThread.cpp
../Util/SignalReceiver.cpp
../Util/SignalRouter.cpp
../Protocol/ByteBuffer.cpp
../Protocol/DataPackageFactory.cpp
../Protocol/Exceptions.cpp
../Protocol/PackageFactory.cpp
../Protocol/TCPServer.cpp
../Protocol/Streams/DataStream.cpp
../Protocol/Streams/FileDataStream.cpp
../Protocol/Streams/PackageCodec.cpp
../Protocol/Streams/PackageStream.cpp
../Protocol/Streams/TCPClientStream.cpp
../Protocol/Packages/ASCByePackage.cpp
../Protocol/Packages/ASCErrorPackage.cpp
../Protocol/Packages/ASCOkPackage.cpp
../Protocol/Packages/ASCPingPackage.cpp
../Protocol/Packages/ASCPongPackage.cpp
../Protocol/Packages/ASCSetoptPackage.cpp
../Protocol/Packages/Package.cpp
../Protocol/Packages/QCExecutePackage.cpp
../Protocol/Packages/QCStatementPackage.cpp
../Protocol/Packages/QSExecutingPackage.cpp
../Protocol/Packages/QSExecutionFinishedPackage.cpp
../Protocol/Packages/QSStmtparsedPackage.cpp
../Protocol/Packages/VSCAbortPackage.cpp
../Protocol/Packages/VSCFinishedPackage.cpp
../Protocol/Packages/VSCSendvaluePackage.cpp
../Protocol/Packages/VSCSendvaluesPackage.cpp
../Protocol/Packages/WCHelloPackage.cpp
../Protocol/Packages/WCLoginPackage.cpp
../Protocol/Packages/WCModePackage.cpp
../Protocol/Packages/WCPasswordPackage.cpp
../Protocol/Packages/WSAuthorizedPackage.cpp
../Protocol/Packages/WSHelloPackage.cpp
../Protocol/Packages/Data/BagPackage.cpp
../Protocol/Packages/Data/BindingPackage.cpp
../Protocol/Packages/Data/BobPackage.cpp
../Protocol/Packages/Data/BoolPackage.cpp
../Protocol/Packages/Data/CollectionPackage.cpp
../Protocol/Packages/Data/DatePackage.cpp
../Protocol/Packages/Data/DatetimePackage.cpp
../Protocol/Packages/Data/DatetimetzPackage.cpp
../Protocol/Packages/Data/DoublePackage.cpp
../Protocol/Packages/Data/ExternalRefPackage.cpp
../Protocol/Packages/Data/LinkPackage.cpp
../Protocol/Packages/Data/RefPackage.cpp
../Protocol/Packages/Data/SequencePackage.cpp
../Protocol/Packages/Data/Sint16Package.cpp
../Protocol/Packages/Data/Sint32Package.cpp
../Protocol/Packages/Data/Sint64Package.cpp
../Protocol/Packages/Data/Sint8Package.cpp
../Protocol/Packages/Data/StructPackage.cpp
../Protocol/Packages/Data/TimePackage.cpp
../Protocol/Packages/Data/TimetzPackage.cpp
../Protocol/Packages/Data/Uint16Package.cpp
../Protocol/Packages/Data/Uint32Package.cpp
../Protocol/Packages/Data/Uint64Package.cpp
../Protocol/Packages/Data/Uint8Package.cpp
../Protocol/Packages/Data/VarcharPackage.cpp
../Protocol/Packages/Data/VoidPackage.cpp
../SystemStats/AllStats.cpp
../SystemStats/ConfigStats.cpp
../SystemStats/QueriesStats.cpp
../SystemStats/SessionStats.cpp
../SystemStats/StoreStats.cpp
../SystemStats/SystemStats.cpp
../SystemStats/TransactionsStats.cpp
)
add_library(loxim_client_static STATIC ${CBindings_SRCS} ${CBindings_ext_SRCS})
add_library(loxim_client_shared SHARED ${CBindings_SRCS} ${CBindings_ext_SRCS})

set_target_properties(loxim_client_static PROPERTIES OUTPUT_NAME loxim_client)
set_target_properties(loxim_client_shared PROPERTIES OUTPUT_NAME loxim_client)
set_target_properties(loxim_client_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(loxim_client_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)

install(TARGETS loxim_client_static loxim_client_shared DESTINATION lib)
install(FILES loxim_client.h DESTINATION include)

