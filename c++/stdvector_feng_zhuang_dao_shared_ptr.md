# std::vector 封裝到 shared_ptr

```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <typeinfo>
#include <cxxabi.h>
using namespace std;

class type_name
{
public:
    template <class T>
    type_name(T const t): realname(
            abi::__cxa_demangle(typeid(T).name(), 0, 0, &status)
        )
    {
        cout << realname << endl;

    }
    ~type_name()
    {
        free(realname);

    }
    int status;
    char* realname;
#define type_name(i) type_name(std::move(i));

};
//================================================================


void test(shared_ptr<std::vector<std::vector<int> > > p)
{
    p->at(0)[0] = 8888;
    cout << "p address " << p.get() << endl;
}

int main()
{
#if 1
    const int N = 2;
    const int M = 4;
    const int K = 99;

    shared_ptr<std::vector<double> > p1 = NULL;
    shared_ptr<std::vector<std::vector<double> > > p2 = NULL;

    p1 = shared_ptr<std::vector<double> >(new std::vector<double>(M));

    p2 = shared_ptr<std::vector<std::vector<double> > >(new
            std::vector<std::vector<double> >(M, std::vector<double>(N, K)));

    cout << "p1 length : " << p1->size() << endl;
    cout << "p2 Row length : " << p2->size() << endl;
    cout << "p2 Col length : " << (*p2)[0].size() << endl;
    
#else
    // M∗NM∗N matrix (assuming int type elements) initialized with all values as KK
    const int N = 2;
    const int M = 4;
    const int K = 99;

    std::vector<std::vector<int>>* p = new std::vector<std::vector<int>>(M,
            std::vector<int>(N, K));

    shared_ptr<std::vector<std::vector<int> > > p1(new
            std::vector<std::vector<int>>(M, std::vector<int>(N, 0)));

    std::shared_ptr<int> p3 = std::shared_ptr<int>(new int);

    shared_ptr<std::vector<std::vector<int> > > p2 = NULL;
    p2 = shared_ptr<std::vector<std::vector<int> > >(new
            std::vector<std::vector<int>>(M, std::vector<int>(N, 0)));

    cout << p[0].size() << endl;
    cout << p[0][0].size() << endl;
    cout << p->at(0)[0] << endl;

    // type_name((*p1)[0][0]);
    cout << p1->size() << endl;
    cout << (*p1)[0].size() << endl;
    (*p)[0].push_back(100);

    cout << "p1 address " << p1.get() << endl;
    cout << "test[+++] " << p1->at(0)[0] << endl;
    test(p1);
    cout << "test[---] " << p1->at(0)[0] << endl;

    delete p;
    // cout << p->at(0)[0] << endl;
#endif

    return 0;
}
```