all:
	flex example.l
	cc lex.yy.c -o example -lfl
	./example

clean:
	rm example lex.yy.c -rf
