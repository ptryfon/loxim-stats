
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(TypeCheck_STAT_SRCS
ClassNames.h
DecisionTable.cpp
DecisionTable.h
DMLControl.cpp
DMLControl.h
RestoreAlgorithm.cpp
RestoreAlgorithm.h
Rule.cpp
Rule.h
TCConstants.h
TypeChecker.cpp
TypeChecker.h
TypeCheckResult.cpp
TypeCheckResult.h)

add_library(TypeCheck STATIC ${TypeCheck_STAT_SRCS})  
