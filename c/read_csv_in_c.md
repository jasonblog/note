# read csv in c


```c
#include <stdio.h>
#include <stdlib.h>

void Load_CSV(void);

int main(int argc, char* argv[])
{
    Load_CSV();
    return 0;
}

void Load_CSV(void)
{
    FILE* fp;
    float accx, accy, accz, gyrox, gyroy, gyroz, magx, magy, magz;
    long long acc_timestamp, gyro_timestamp, mag_timestamp;

    if ((fp = fopen("5.csv", "r")) == NULL) {
        puts("Cannot oepn the file");
    }

    fscanf(fp, "%f,%f,%f,%lld,%f,%f,%f,%lld,%f,%f,%f,%lld",
           &accx, &accy, &accz, &acc_timestamp,
           &gyrox, &gyroy, &gyroz, &gyro_timestamp,
           &magx, &magy, &magz, &mag_timestamp);

    while (!feof(fp)) {
        printf("%f,%f,%f,%lld,%f,%f,%f,%lld,%f,%f,%f,%lld\n",
               accx, accy, accz, acc_timestamp,
               gyrox, gyroy, gyroz, gyro_timestamp,
               magx, magy, magz, mag_timestamp);

        fscanf(fp, "%f,%f,%f,%lld,%f,%f,%f,%lld,%f,%f,%f,%lld",
               &accx, &accy, &accz, &acc_timestamp,
               &gyrox, &gyroy, &gyroz, &gyro_timestamp,
               &magx, &magy, &magz, &mag_timestamp);
    }

    fclose(fp);
}
```

- 5.csv

```sh
0.90000,-0.04000,9.59000,887669967101548,-0.01000,0.00000,0.00000,887669967101548,26.59375,-12.19531,-112.56250,887669967101548
0.89000,-0.20000,9.69000,887669977147193,0.00000,0.00000,0.00000,887669977147193,26.59375,-12.19531,-112.56250,887669977147193
0.92000,-0.16000,9.71000,887669987192838,0.00000,0.00000,0.00000,887669987192838,26.59375,-12.19531,-112.56250,887669987192838
0.87000,-0.09000,9.62000,887669997238483,-0.01000,0.00000,0.00000,887669997238483,26.68750,-12.19531,-112.56250,887669997238483
0.84000,-0.08000,9.56000,887670007284128,0.00000,0.00000,0.00000,887670007284128,26.68750,-12.19531,-112.68750,887670007284128

```