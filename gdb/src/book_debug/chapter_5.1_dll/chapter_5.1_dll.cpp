// chapter_5.1_dll.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "chapter_5.1_dll.h"
#include <stdlib.h>

// 这是导出函数的一个示例。
extern "C" CHAPTER51DLL_API double calc_pi(void)
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
		x = (double)rand() / max_val;
		y =(double) rand() / max_val;
		if ((x * x + y * y) <= 1)
			num++;
	}
	pi = (4.0 * num) / try_times;
	return pi;
}

int Cchapter51dll::add(int i, int j)
{
	return i + j;
}

int Cchapter51dll::sub(int i, int j)
{
	return i - j;
}
// 这是已导出类的构造函数。
Cchapter51dll::Cchapter51dll()
{
}

