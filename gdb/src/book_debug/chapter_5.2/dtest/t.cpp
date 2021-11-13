#include <dlfcn.h>
#include <iostream>
using namespace std;
typedef double (*CALC_PI)();
int main()
{

	void* handle = dlopen("../testso/libtest.so",RTLD_NOW);
	if(!handle)
	{
		cout << "can't open the dll libtest.so" <<endl;
		return 0;
	}
	CALC_PI calc_pi =(CALC_PI) dlsym(handle,"calc_pi");
	if(!calc_pi)
	{
		cout << "can't get the calc_pi" <<endl;
		return 0;
	}
	double pi = calc_pi();
	cout << "pi is " << pi << endl;
	dlclose(handle);
	return 0;
}
