noinst_LTLIBRARIES = libQueryParser.la
BUILT_SOURCES = parser.h
AM_YFLAGS = -d -y
AM_LFLAGS = -olex.yy.c
parser.h: parser.y
	$(YACC) parser.y -o parser.c
libQueryParser_la_SOURCES = AuxRmver.cpp AuxRmver.h ClassNames.h DataRead.cpp DataRead.h DeathRmver.cpp DeathRmver.h Deb.cpp Deb.h IndexNode.h JoinRpcer.cpp JoinRpcer.h Optimizer.cpp Optimizer.h Privilige.cpp Privilige.h QueryLexer.h QueryParser.cpp QueryParser.h Stack.cpp Stack.h TreeNode.cpp TreeNode.h scanner.ll ParserWrapper.h ParserWrapper.cpp
INCLUDES=-I$(top_srcdir)/src
