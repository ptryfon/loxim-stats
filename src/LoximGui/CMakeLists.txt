
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

if (G_PERF_TOOLS_TESTS_ENABLED)
	set(EXTRALIBS ${TCMALLOC_LIBRARY})
endif (G_PERF_TOOLS_TESTS_ENABLED)

include(${QT_USE_FILE})

SET(LoximGui_UIS
    LoximGui.ui
	Authenticator.ui
	ConnectDialog.ui
)

QT4_WRAP_UI(LoximGui_UIS_H ${LoximGui_UIS})

SET (LoximGui_MOC_HDRS
	LoximGui.h
	Authenticator.h
	ConnectDialog.h
)

SET (LoximGui_MOC_CPP
	LoximGui.cpp
	Authenticator.cpp
	ConnectDialog.cpp
)

QT4_WRAP_CPP (LoximGui_MOC_CPP ${LoximGui_MOC_HDRS})

set (LoximGui_SRCS
	Main.cpp
	${LoximGui_UIS_H})

MESSAGE(STATUS "LoximGui_UIS: ${LoximGui_UIS}")
MESSAGE(STATUS "LoximGui_UIS_H: ${LoximGui_UIS_H}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_automoc(${LoximGui_SRCS})

#add_library(LoximGui_lib STATIC ${LoximGui_SRCS} ${ProtocolEnums_STAT_SRCS} ${ProtocolPackagesDataSTAT_SRCS} ${ProtocolPackagesSTAT_SRCS} ${Protocol_STAT_SRCS} ${ProtocolStreams_STAT_SRCS})

add_executable(loxim_gui ${LoximGui_SRCS} ${LoximGui_MOC_CPP})
add_dependencies(loxim_gui LoximGui_lib)
target_link_libraries(loxim_gui LoximGui_lib ${QT_LIBRARIES})

add_library(LoximGui_lib STATIC ${LoximGui_SRCS}) 
target_link_libraries(LoximGui_lib Client Errors Util Config ProtocolAllInOne ${READLINE_LIBRARY} pthread)
