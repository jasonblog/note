# arraycopy using std::vector


```cpp
#include <iostream>
#include <vector>
using namespace std;

void arraycopy(std::vector<double>* src,
               int srcPos,
               std::vector<double>* dest,
               int destPos,
               int length)
{
    int i, j, k = 0;

    for (i = srcPos, j = destPos; k < length ; ++i, ++j, ++k) {
        dest->at(j) = src->at(i);
    }
}

int main(int argc, char* argv[])
{
    unsigned int i;
    std::vector<std::vector<double> > arr1 {{ -3.71, 3.43, 0}, { -3.84, 2.2, 1}, { -2.68, 2.27, 0}, { -2.87, 13, -0.2}};
    std::vector<std::vector<double> > arr2 {{ -3.71, 3.43, 0}, { -3.84, 2.2, 1}, { -2.68, 2.27, 0}, { -2.87, 13, -0.2}};

    std::vector<double>* pp;

    std::vector<std::vector<double> >* p = &arr1;
    std::vector<std::vector<double> >* p1 = &arr1;

    cout << arr1.size() << endl;
    cout << arr1[0].size() << endl;

    cout << p->size() << endl;
    cout << p[0][1].size() << endl;

    pp = &p[0][0];
    cout << pp->size() << endl;

    cout << pp->at(0) << endl;
    cout << pp->at(1) << endl;
    cout << pp->at(2) << endl;



#if 1
    arraycopy(&p[0][1], 0, &p1[0][0], 0, 2);

	cout << endl;
    for (i = 0; i < p1[0][0].size(); ++i) {
        cout << p1->at(0)[i] << endl;
    }

#endif

    return 0;
}
```