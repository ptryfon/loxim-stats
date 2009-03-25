
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

add_subdirectory(Data)

set(ProtocolPackages_STAT_SRCS
    ASCByePackage.cpp
    ASCByePackage.h
    ASCErrorPackage.cpp
    ASCErrorPackage.h
    ASCOkPackage.cpp
    ASCOkPackage.h
    ASCPingPackage.cpp
    ASCPingPackage.h
    ASCPongPackage.cpp
    ASCPongPackage.h
    ASCSetoptPackage.cpp
    ASCSetoptPackage.h
    Package.cpp
    Package.h
    QCExecutePackage.cpp
    QCExecutePackage.h
    QCStatementPackage.cpp
    QCStatementPackage.h
    QSExecutingPackage.cpp
    QSExecutingPackage.h
    QSExecutionFinishedPackage.cpp
    QSExecutionFinishedPackage.h
    QSStmtparsedPackage.cpp
    QSStmtparsedPackage.h
    VSCAbortPackage.cpp
    VSCAbortPackage.h
    VSCFinishedPackage.cpp
    VSCFinishedPackage.h
    VSCSendvaluePackage.cpp
    VSCSendvaluePackage.h
    VSCSendvaluesPackage.cpp
    VSCSendvaluesPackage.h
    WCHelloPackage.cpp
    WCHelloPackage.h
    WCLoginPackage.cpp
    WCLoginPackage.h
    WCModePackage.cpp
    WCModePackage.h
    WCPasswordPackage.cpp
    WCPasswordPackage.h
    WSAuthorizedPackage.cpp
    WSAuthorizedPackage.h
    WSHelloPackage.cpp
    WSHelloPackage.h)

add_library(ProtocolPackages STATIC ${ProtocolPackages_STAT_SRCS})


