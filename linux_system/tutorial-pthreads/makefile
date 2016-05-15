all: fork.out pthread_create.out hello.out hello_arg1.out hello_arg2.out
all: hello_arg3.out join.out thread_stack_control.out
all: dot_product_mutex.out dot_product_serial.out condition_variable.out
fork.out: fork.c
	gcc -o fork.out fork.c
pthread_create.out: pthread_create.c
	gcc -o pthread_create.out -lpthread pthread_create.c
hello.out: hello.c
	gcc -o hello.out -lpthread hello.c
hello_arg1.out: hello_arg1.c
	gcc -o hello_arg1.out -lpthread hello_arg1.c
hello_arg2.out: hello_arg2.c
	gcc -o hello_arg2.out -lpthread hello_arg2.c
hello_arg3.out: hello_arg3.c
	gcc -o hello_arg3.out -lpthread hello_arg3.c
join.out: join.c
	gcc -o join.out -lpthread -lm join.c
thread_stack_control.out: thread_stack_control.c
	gcc -Wall -o thread_stack_control.out -lpthread thread_stack_control.c
dot_product_mutex.out: dot_product_mutex.c
	gcc -Wall -o dot_product_mutex.out -lpthread dot_product_mutex.c
dot_product_serial.out: dot_product_serial.c
	gcc -Wall -o dot_product_serial.out -lpthread dot_product_serial.c
condition_variable.out: condition_variable.c
	gcc -Wall -o condition_variable.out -lpthread condition_variable.c
