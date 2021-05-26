all: target/debug/c-binding

target/debug/c-binding: src/main.rs lib/libhello.a
	cargo build

lib/libhello.a: lib/hello.c
	gcc -c -o lib/hello.o lib/hello.c
	ar rsc lib/libhello.a lib/hello.o
