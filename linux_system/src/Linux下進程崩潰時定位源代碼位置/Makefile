all : a.out
	
a.out:segment_violation.c libmy
	gcc -g  -rdynamic segment_violation.c -o a.out -L. -lmyhello

libmy:
	gcc -fPIC -shared -o libmyhello.so hello.c

clean:
	@echo "remove the a.out"
	rm -rf  a.out *.so
