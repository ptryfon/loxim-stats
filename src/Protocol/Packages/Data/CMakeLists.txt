
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(ProtocolPackagesData_STAT_SRCS
    BagPackage.cpp
    BagPackage.h
    BindingPackage.cpp
    BindingPackage.h
    BobPackage.cpp
    BobPackage.h
    BoolPackage.cpp
    BoolPackage.h
    CollectionPackage.cpp
    CollectionPackage.h
    DatePackage.cpp
    DatePackage.h
    DatetimePackage.cpp
    DatetimePackage.h
    DatetimetzPackage.cpp
    DatetimetzPackage.h
    DoublePackage.cpp
    DoublePackage.h
    ExternalRefPackage.cpp
    ExternalRefPackage.h
    LinkPackage.cpp
    LinkPackage.h
    RefPackage.cpp
    RefPackage.h
    SequencePackage.cpp
    SequencePackage.h
    Sint16Package.cpp
    Sint16Package.h
    Sint32Package.cpp
    Sint32Package.h
    Sint64Package.cpp
    Sint64Package.h
    Sint8Package.cpp
    Sint8Package.h
    StructPackage.cpp
    StructPackage.h
    TimePackage.cpp
    TimePackage.h
    TimetzPackage.cpp
    TimetzPackage.h
    Uint16Package.cpp
    Uint16Package.h
    Uint32Package.cpp
    Uint32Package.h
    Uint64Package.cpp
    Uint64Package.h
    Uint8Package.cpp
    Uint8Package.h
    VarcharPackage.cpp
    VarcharPackage.h
    VoidPackage.cpp
    VoidPackage.h)

add_library(ProtocolPackagesData STATIC ${ProtocolPackagesData_STAT_SRCS})
target_link_libraries(ProtocolPackagesData Protocol ProtocolStreams)

