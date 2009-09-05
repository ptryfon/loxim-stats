
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
	CACHE INTERNAL "" 
)

QT4_WRAP_CPP (LoximGui_MOC_CPP ${LoximGui_MOC_HDRS})

set (LoximGui_SRCS
	Application.cpp
	Application.h
	${LoximGui_UIS_H}
	CACHE INTERNAL "" 
	)

MESSAGE(STATUS "LoximGui_UIS: ${LoximGui_UIS}")
MESSAGE(STATUS "LoximGui_UIS_H: ${LoximGui_UIS_H}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_automoc(${LoximGui_SRCS})

add_executable(loxim_gui ${LoximGui_SRCS} ${LoximGui_MOC_CPP})
target_link_libraries(loxim_gui ${QT_LIBRARIES} lsbqlLib)
