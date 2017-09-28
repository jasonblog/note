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

int main()
{
    // M∗NM∗N matrix (assuming int type elements) initialized with all values as KK
    const int N = 2;
    const int M = 4;
    const int K = 99;

    std::vector<std::vector<int>>* p = new std::vector<std::vector<int>>(M, std::vector<int>(N, K));


    shared_ptr<std::vector<std::vector<int> > > p1(new std::vector<std::vector<int>>(M, std::vector<int>(N, 0)));

    cout << p[0].size() << endl;
    cout << p[0][0].size() << endl;
    cout << p->at(0)[0] << endl;

    // type_name((*p1)[0][0]);
    cout << p1->size() << endl;
    cout << (*p1)[0].size() << endl;
    cout << p1->at(0)[0] << endl;


    delete p;
    // cout << p->at(0)[0] << endl;

    return 0;
}
```