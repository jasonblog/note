#include <stdio.h>
#include <stdlib.h>
extern "C"
double calc_pi()
{
	double x = 0;
	double y = 0;
	double pi = 0;
	int num = 0;
	int iter = 0;
	const int try_times = 10000000;
	const double max_val = 32767.0;
	while (iter++ <= try_times)
	{
		x = (double)(rand() % 32768) / max_val;
		y = (double)(rand() % 32768) / max_val;
		if ((x * x + y * y) <= 1)
			num++;
	}
	pi = (4.0 * num) / try_times;
	return pi;
}
