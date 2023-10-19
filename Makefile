C_FLAGS=-g3

quash: parser.tab.o parser_helper.o lex.yy.o runline.o
	gcc parser.tab.o parser_helper.o lex.yy.o runline.o -o quash

lex.yy.c : lex.l
	flex lex.l

parser.tab.c : parser.y
	bison -d parser.y

clean:
	rm *.o quash parser.tab.c parser.tab.h lex.yy.c

.PHONY : clean
