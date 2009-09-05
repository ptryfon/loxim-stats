set(PACKAGESDIR "../Protocol/Packages")

add_subdirectory(Data)

set(ProtocolPackages_SRCS 
	${PACKAGESDIR}/ASCByePackage.h 
	${PACKAGESDIR}/ASCByePackage.cpp 
	${PACKAGESDIR}/ASCErrorPackage.h 
	${PACKAGESDIR}/ASCErrorPackage.cpp 
	${PACKAGESDIR}/ASCOkPackage.h 
	${PACKAGESDIR}/ASCOkPackage.cpp 
	${PACKAGESDIR}/ASCPingPackage.h 
	${PACKAGESDIR}/ASCPingPackage.cpp 
	${PACKAGESDIR}/ASCPongPackage.h 
	${PACKAGESDIR}/ASCPongPackage.cpp 
	${PACKAGESDIR}/ASCSetoptPackage.h 
	${PACKAGESDIR}/ASCSetoptPackage.cpp 
	${PACKAGESDIR}/QCExecutePackage.h 
	${PACKAGESDIR}/QCExecutePackage.cpp 
	${PACKAGESDIR}/QCStatementPackage.h 
	${PACKAGESDIR}/QCStatementPackage.cpp 
	${PACKAGESDIR}/QSExecutingPackage.h 
	${PACKAGESDIR}/QSExecutingPackage.cpp 
	${PACKAGESDIR}/QSExecutionFinishedPackage.h 
	${PACKAGESDIR}/QSExecutionFinishedPackage.cpp 
	${PACKAGESDIR}/QSStmtparsedPackage.h 
	${PACKAGESDIR}/QSStmtparsedPackage.cpp 
	${PACKAGESDIR}/VSCAbortPackage.h 
	${PACKAGESDIR}/VSCAbortPackage.cpp 
	${PACKAGESDIR}/VSCFinishedPackage.h 
	${PACKAGESDIR}/VSCFinishedPackage.cpp 
	${PACKAGESDIR}/VSCSendvaluePackage.h 
	${PACKAGESDIR}/VSCSendvaluePackage.cpp 
	${PACKAGESDIR}/VSCSendvaluesPackage.h 
	${PACKAGESDIR}/VSCSendvaluesPackage.cpp 
	${PACKAGESDIR}/WCHelloPackage.h 
	${PACKAGESDIR}/WCHelloPackage.cpp 
	${PACKAGESDIR}/WCLoginPackage.h 
	${PACKAGESDIR}/WCLoginPackage.cpp 
	${PACKAGESDIR}/WCModePackage.h 
	${PACKAGESDIR}/WCModePackage.cpp 
	${PACKAGESDIR}/WCPasswordPackage.h 
	${PACKAGESDIR}/WCPasswordPackage.cpp 
	${PACKAGESDIR}/WSAuthorizedPackage.h 
	${PACKAGESDIR}/WSAuthorizedPackage.cpp 
	${PACKAGESDIR}/WSHelloPackage.h 
	${PACKAGESDIR}/WSHelloPackage.cpp 
	${PACKAGESDIR}/Package.h
	${PACKAGESDIR}/Package.cpp CACHE INTERNAL "" )

