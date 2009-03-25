
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})


set(Store_STAT_SRCS
Buffer.cpp
Buffer.h
Classes.cpp
DBDataValue.cpp
DBDataValue.h
DBLogicalID.cpp
DBLogicalID.h
DBObjectPointer.cpp
DBObjectPointer.h
DBPhysicalID.cpp
DBPhysicalID.h
DBStoreManager.cpp
DBStoreManager.h
File.cpp
File.h
Interfaces.cpp
Map.cpp
Map.h
Misc.cpp
Misc.h
NamedItems.cpp
NamedItems.h
NamedRoots.cpp
NamedRoots.h
PageManager.cpp
PageManager.h
PagePointer.cpp
PagePointer.h
Store.h
StoreSchemas.cpp
Struct.h
SystemViews.cpp
SystemViews.h
Views.cpp
Views.h)

add_library(Store STATIC ${Store_STAT_SRCS})
