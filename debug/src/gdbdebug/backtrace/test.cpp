#include "test.h"

test_c::test_c()
{
}
test_c::~test_c()
{
}

int test_c::test_member(int x,int y)
{
	int z = x * x + y * y;
	return z;
}

