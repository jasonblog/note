# gyro raw data filter


```c
#include <stdio.h>
#include <string.h>

struct rawdata_buf {
    double x[5], y[5], z[5];
    double sum[3];
    size_t index;
};
void rawdata_buf_append(struct rawdata_buf* raw, double x, double y, double z)
{
    raw->sum[0] += -raw->x[raw->index] + x;
    raw->sum[1] += -raw->y[raw->index] + y;
    raw->sum[2] += -raw->z[raw->index] + z;
    raw->x[raw->index] = x;
    raw->y[raw->index] = y;
    raw->z[raw->index] = z;
    raw->index = (raw->index + 1) % 5;
}
void rawdata_buf_average(struct rawdata_buf* raw, double* x, double* y,
                         double* z)
{
#ifdef _DEBUG

    for (size_t i = raw->index; i != raw->index + 5; ++i) {
        printf("(%g,%g,%g)", raw->x[i % 5], raw->y[i % 5], raw->z[i % 5]);
    }

    putchar('\n');
#endif

    *x = raw->sum[0] / 5;
    *y = raw->sum[1] / 5;
    *z = raw->sum[2] / 5;
}

int main(void)
{
    double avgx, avgy, avgz;

    struct rawdata_buf buf = { 0 };
    rawdata_buf_append(&buf, 0, 0, 0);
    rawdata_buf_append(&buf, 1, 1, 1);
    rawdata_buf_append(&buf, 2, 2, 2);
    rawdata_buf_append(&buf, 3, 3, 3);
    rawdata_buf_append(&buf, 4, 4, 4);
    rawdata_buf_average(&buf, &avgx, &avgy, &avgz);
    printf("avg = (%g,%g,%g)\n", avgx, avgy, avgz);

    rawdata_buf_append(&buf, 5, 5, 5);
    rawdata_buf_average(&buf, &avgx, &avgy, &avgz);
    printf("avg = (%g,%g,%g)\n", avgx, avgy, avgz);

    rawdata_buf_append(&buf, 6, 6, 6);
    rawdata_buf_average(&buf, &avgx, &avgy, &avgz);
    printf("avg = (%g,%g,%g)\n", avgx, avgy, avgz);

    rawdata_buf_append(&buf, 7, 7, 7);
    rawdata_buf_average(&buf, &avgx, &avgy, &avgz);
    printf("avg = (%g,%g,%g)\n", avgx, avgy, avgz);

    rawdata_buf_append(&buf, 8, 8, 8);
    rawdata_buf_average(&buf, &avgx, &avgy, &avgz);
    printf("avg = (%g,%g,%g)\n", avgx, avgy, avgz);

    rawdata_buf_append(&buf, 9, 9, 9);
    rawdata_buf_average(&buf, &avgx, &avgy, &avgz);
    printf("avg = (%g,%g,%g)\n", avgx, avgy, avgz);

    return 0;
}
```

---

```c
float calc(float a[], int size, int newNum)
{
    static bool inited = false;
    static float sum = 0;
    static int idx = 0;

    if (!inited) {
        for (int i = 0; i < size; i++) {
            sum += a[i];
        }

        inited = true;
    } else {
        sum -= (a[idx] - newNum);
        a[idx++] = newNum;
        idx %= size;
    }

    return sum / size;
}
```

