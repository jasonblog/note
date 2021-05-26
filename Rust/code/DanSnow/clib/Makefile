all: hello

hello: main.c target/debug/libclib.a
	gcc -o hello main.c -L target/debug -l clib -l pthread -l dl

target/debug/libclib.a: src/lib.rs
	cargo build
