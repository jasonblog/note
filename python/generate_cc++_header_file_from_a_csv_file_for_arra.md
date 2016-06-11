# Generate c/c++ header file from a CSV file (for array declaration)


```py
# -*- coding: utf-8 -*-
import csv

f = open('bb', 'r')
reader = csv.reader(f)
num_rows = sum(1 for each in reader)
f.seek(0)  #reposition
reader = csv.reader(f, delimiter=',', skipinitialspace=True)
num_cols = len(next(reader))
f.seek(0)  #reposition

print num_rows
print num_cols

# prolog
print('float myarray[%d][%d] = {' % (int(num_rows), int(num_cols)))

for row in csv.reader(f):
    for element in row:
        print '%f,' % float(element),
    print ""

# epilog
print('};')

f.close()
```



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