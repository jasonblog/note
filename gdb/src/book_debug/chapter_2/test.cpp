typedef struct _TEST_
{

	int value;

}TEST,*PTEST;
class test_c
{
public:
	test_c()
	{
	}
	test_c(int i) { _i = i; }
private:
	int _i;
};	


void test_fun()
{
	test_c t;
	test_c t2(10);
}