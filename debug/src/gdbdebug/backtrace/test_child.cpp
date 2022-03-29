#include "test.h"

int test_child::test_member(int x,int y)
{	
	int z = test_c::test_member(x + x,y + y);
	int res = z + x*y;
	return res;
}
