include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(STATSDISPLAYDIR "../StatsDisplay")
set(STATSDISPLAYGUIDIR ${STATSDISPLAYDIR}/GUI)
set(STATSDISPLAYLOGICDIR ${STATSDISPLAYDIR}/Logic)

if (G_PERF_TOOLS_TESTS_ENABLED)
	set(EXTRALIBS ${TCMALLOC_LIBRARY})
endif (G_PERF_TOOLS_TESTS_ENABLED)

include(${QT_USE_FILE})

SET(StatsDisplay_MOC_CPP
	${STATSDISPLAYGUIDIR}/StatsDisplayWindow.cpp
	${STATSDISPLAYGUIDIR}/FileViewer.cpp
	${STATSDISPLAYGUIDIR}/GraphViewer.cpp
	${STATSDISPLAYGUIDIR}/LogViewer.cpp
	${STATSDISPLAYGUIDIR}/OptionsViewer.cpp
   CACHE INTERNAL "" 
)

SET(StatsDisplay_MOC_HDRS
	${STATSDISPLAYGUIDIR}/StatsDisplayWindow.h
	${STATSDISPLAYGUIDIR}/FileViewer.h
	${STATSDISPLAYGUIDIR}/GraphViewer.h
	${STATSDISPLAYGUIDIR}/LogViewer.h
	${STATSDISPLAYGUIDIR}/OptionsViewer.h
)

QT4_WRAP_CPP(StatsDisplay_MOC_CPP ${StatsDisplay_MOC_HDRS})

SET(StatsDisplayLogic_SRCS
	${STATSDISPLAYLOGICDIR}/LogManager.cpp
	${STATSDISPLAYLOGICDIR}/LogManager.h
	${STATSDISPLAYLOGICDIR}/DirectoryEntry.cpp
	${STATSDISPLAYLOGICDIR}/DirectoryEntry.h
	${STATSDISPLAYLOGICDIR}/FileManager.cpp
	${STATSDISPLAYLOGICDIR}/FileManager.h
	${STATSDISPLAYLOGICDIR}/GraphManager.cpp
	${STATSDISPLAYLOGICDIR}/GraphManager.h
)

SET(StatsDisplay_SRCS
	${StatsDisplayLogic_SRCS}
	StatsDisplay.cpp
	CACHE INTERNAL "" 
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_automoc(${StatsDisplay_SRCS})

add_executable(stats_display ${StatsDisplay_SRCS} ${StatsDisplay_MOC_CPP})
target_link_libraries(stats_display ${QT_LIBRARIES})