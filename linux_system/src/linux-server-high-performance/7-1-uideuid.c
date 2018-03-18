#include <stdio.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
    uid_t  uid = getuid();
    uid_t  euid = geteuid();
    fprintf(stdout, "userid is %d, effective userid is: %d\n", uid, euid);
    return 0;
}
