#include "test.h"

test_c::test_c()
{
}
test_c::~test_c()
{
}
int test_c::get_number()
{
	int num = rand();
	return num;
}
std::string test_c::get_str()
{
	std::string res = "this is a test str";
	return res;
}
