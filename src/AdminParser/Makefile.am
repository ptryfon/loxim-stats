noinst_LTLIBRARIES = libAdminParser.la
BUILT_SOURCES = parser.h
AM_YFLAGS = -d 
AM_LFLAGS = -olex.yy.c
parser.h: parser.y 
	$(YACC) parser.y -o parser.c
parser.c: parser.y 
	$(YACC) parser.y -o parser.c
libAdminParser_la_SOURCES = AdminExecutableTreeNode.cpp AdminExecutableTreeNode.h AdminExecutor.cpp AdminExecutor.h AdminLexer.h AdminTreeNode.cpp AdminTreeNode.h ShutdownTreeNode.cpp ShutdownTreeNode.h scanner.ll AdminParser.cpp AdminParser.h
EXTRA_DIST=parser.y
INCLUDES=-I$(top_srcdir)/src
