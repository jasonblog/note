class test_c
{
public:
	test_c();
	virtual ~test_c();
	virtual int test_member(int x,int y);
};
class test_child:public test_c
{
public:
	test_child(){}
	virtual ~test_child(){}
	int test_member(int x,int y);
};
