#include "my_debug.h"
#define MY_DEBUG_FILE_PATH "/usr/local/nginx/sbin/mydebug.log"
int _flag = 0;

#define open_my_debug_file()  \
     (my_debug_fd = fopen(MY_DEBUG_FILE_PATH, "a"))

#define close_my_debug_file()  \
     do {  \
          if (NULL != my_debug_fd) {  \
                fclose(my_debug_fd);  \
          }  \
     }while(0)

#define my_debug_print(args, fmt...) \
      do{  \
          if (0 == _flag) {  \
                 break;  \
          }  \
          if (NULL == my_debug_fd && NULL == open_my_debug_file()) {  \
                printf("Err: Can not open output file.\n");  \
                break;  \
          }  \
          fprintf(my_debug_fd, args, ##fmt);  \
          fflush(my_debug_fd);  \
     }while(0)

void enable_my_debug( void )
{
     _flag = 1;
}
void disable_my_debug( void )
{
     _flag = 0;
}
int get_my_debug_flag( void )
{
     return _flag;
}
void set_my_debug_flag( int flag )
{
     _flag = flag;
}
void main_constructor( void )
{
     //Do Nothing
}
void main_destructor( void )
{
     close_my_debug_file();
}
void __cyg_profile_func_enter( void *this, void *call )
{
     my_debug_print("Enter\n%p\n%p\n", call, this);
}
void __cyg_profile_func_exit( void *this, void *call )
{
     my_debug_print("Exit\n%p\n%p\n", call, this);
}