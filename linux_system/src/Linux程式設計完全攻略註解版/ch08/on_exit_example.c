#include<stdlib.h>
void test_exit(int status, void* arg)
{
    printf("before exit()!\n");
    printf("exit %d\n", status);
    printf("arg=%s\n", (char*)arg);

}
int main()
{
    char* str = "test";
    on_exit(test_exit, (void*)str);
    exit(4321);
}


