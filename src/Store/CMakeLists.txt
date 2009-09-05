set(STOREDIR "../Store")

set(Store_SRCS
	${STOREDIR}/Buffer.cpp
	${STOREDIR}/Buffer.h
	${STOREDIR}/Classes.cpp
	${STOREDIR}/DBDataValue.cpp
	${STOREDIR}/DBDataValue.h
	${STOREDIR}/DBLogicalID.cpp
	${STOREDIR}/DBLogicalID.h
	${STOREDIR}/DBObjectPointer.cpp
	${STOREDIR}/DBObjectPointer.h
	${STOREDIR}/DBPhysicalID.cpp
	${STOREDIR}/DBPhysicalID.h
	${STOREDIR}/DBStoreManager.cpp
	${STOREDIR}/DBStoreManager.h
	${STOREDIR}/File.cpp
	${STOREDIR}/File.h
	${STOREDIR}/Interfaces.cpp
	${STOREDIR}/Map.cpp
	${STOREDIR}/Map.h
	${STOREDIR}/Misc.cpp
	${STOREDIR}/Misc.h
	${STOREDIR}/NamedItems.cpp
	${STOREDIR}/NamedItems.h
	${STOREDIR}/NamedRoots.cpp
	${STOREDIR}/NamedRoots.h
	${STOREDIR}/PageManager.cpp
	${STOREDIR}/PageManager.h
	${STOREDIR}/PagePointer.cpp
	${STOREDIR}/PagePointer.h
	${STOREDIR}/Store.h
	${STOREDIR}/StoreSchemas.cpp
	${STOREDIR}/Struct.h
	${STOREDIR}/SystemViews.cpp
	${STOREDIR}/SystemViews.h
	${STOREDIR}/Views.cpp
	${STOREDIR}/Views.h CACHE INTERNAL "" )

