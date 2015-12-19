#ifndef MY_DEBUG_LENKY_H
#define MY_DEBUG_LENKY_H
#include <stdio.h>

void enable_my_debug( void ) __attribute__((no_instrument_function));
void disable_my_debug( void ) __attribute__((no_instrument_function));
int get_my_debug_flag( void ) __attribute__((no_instrument_function));
void set_my_debug_flag( int ) __attribute__((no_instrument_function));
void main_constructor( void ) __attribute__((no_instrument_function, constructor));
void main_destructor( void ) __attribute__((no_instrument_function, destructor));
void __cyg_profile_func_enter( void *,void *) __attribute__((no_instrument_function));
void __cyg_profile_func_exit( void *, void *) __attribute__((no_instrument_function));

#ifndef MY_DEBUG_MAIN
extern FILE *my_debug_fd;
#else
FILE *my_debug_fd;
#endif
#endif
