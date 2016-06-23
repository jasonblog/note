#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

typedef struct {
    // Quaternion x y z w
    float   q_x;
    float   q_y;
    float   q_z;
    float   q_w;

    // Gyro x,y,z
    float g_x;
    float g_y;
    float g_z;
} imu_data_t;

#define PAGE_SIZE 4096

int main(int argc, char** argv)
{
    int configfd;
    configfd = open("/sys/kernel/debug/mmap_example", O_RDWR);

    if (configfd < 0) {
        perror("open");
        return -1;
    }

    char* address = NULL;
    address = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, configfd,
                   0);

    if (address == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

#if 1
    imu_data_t *imu_p = (imu_data_t*)address;
    printf("q_x=%f\n", imu_p->q_x);
    printf("q_y=%f\n", imu_p->q_y);
    printf("q_z=%f\n", imu_p->q_z);
    printf("q_w=%f\n", imu_p->q_w);
    printf("g_x=%f\n", imu_p->g_x);
    printf("g_y=%f\n", imu_p->g_y);
    printf("g_z=%f\n", imu_p->g_z);
#else
    printf("initial message: %s\n", address);
    memcpy(address + 11 , "*user*", 6);
    printf("changed message: %s\n", address);
#endif
    close(configfd);
    return 0;
}
