# struct pointer to array


```c
#include <stdio.h>
#include <string.h>

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

int main(int argc, char* argv[])
{
    char fifo[28];
    char fifo_tmp[28];
    imu_data_t* imu_p = (imu_data_t*)fifo;

    printf("%ld\n", sizeof(imu_data_t));

    imu_p->q_x = 0.780517;
    imu_p->q_y = 0.031600;
    imu_p->q_z = -0.023300;
    imu_p->q_w = -0.623900;
    imu_p->g_x = -0.80000;
    imu_p->g_y = -0.24000;
    imu_p->g_z = -1.47000;

    memcpy(fifo_tmp, fifo, sizeof(imu_data_t));

    imu_p = (imu_data_t*)fifo_tmp;
    printf("q_x=%f\n", imu_p->q_x);
    printf("q_y=%f\n", imu_p->q_y);
    printf("q_z=%f\n", imu_p->q_z);
    printf("q_w=%f\n", imu_p->q_w);
    printf("g_x=%f\n", imu_p->g_x);
    printf("g_y=%f\n", imu_p->g_y);
    printf("g_z=%f\n", imu_p->g_z);

    return 0;
}
```