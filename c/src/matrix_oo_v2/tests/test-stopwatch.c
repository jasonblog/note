#include "stopwatch.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
    watch_p ctx = Stopwatch.create();
    if (!ctx) return -1;

    Stopwatch.start(ctx);
    for (int i = 0; i < 5; i++) {
        printf("\b  %d\r", i);;
	fflush(stdout);
        sleep(1);
    }
    double now = Stopwatch.read(ctx);

    Stopwatch.destroy(ctx);
    return !now;
}
