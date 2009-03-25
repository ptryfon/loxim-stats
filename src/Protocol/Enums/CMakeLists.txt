
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(ProtocolEnums_STAT_SRCS
    AbortReasonCodes.h
    AuthMethods.h
    ByeReasons.h
    Collations.h
    DataPackages.h
    dummy.cpp
    Enums.h
    Errors.h
    Features.h
    Modes.h
    Packages.h
    SendValueFlags.h
    StatementFlags.h)
    
add_library(ProtocolEnums STATIC ${ProtocolEnums_STAT_SRCS})
