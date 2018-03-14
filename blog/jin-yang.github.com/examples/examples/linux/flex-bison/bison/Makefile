all:
	flex frame.l
	bison -d frame.y
	gcc frame.tab.c lex.yy.c -o example

clean:
	rm -f example frame.tab* lex.yy.c
