QueryParser.a: TreeNode.o QueryParser.o lex.yy.o parser.tab.o
	ar -ruc QueryParser.a TreeNode.o QueryParser.o lex.yy.o \
		parser.tab.o
TreeNode.o: TreeNode.cpp TreeNode.h
	g++ -c -Wall TreeNode.cpp
QueryParser.o: QueryParser.cpp TreeNode.h QueryParser.h \
		   QueryParser.h TreeNode.h 
	g++ -c -Wall QueryParser.cpp
lex.yy.o: lex.yy.cc parser.tab.c TreeNode.h 
	g++ -c lex.yy.cc
parser.tab.o: parser.tab.c TreeNode.h
	g++ -c parser.tab.c
parser.tab.c: parser.y
	bison -d -v parser.y
lex.yy.cc: skaner.l
	lex -+ skaner.l
clean:
	-rm -f *.o *.a *~ sample parser.tab.* lex.yy.* parser.output
