# 2D vector array using std::vector


M∗NM∗N matrix (assuming int type elements) initialized with all values as KK

```cpp
std::vector<std::vector<int>> matrix(M, std::vector<int>(N, K));
```

Otherwise,
```cpp
std::vector<int> va(5, 1);    // vector a {1, 1, 1, 1, 1}
std::vector<int> vb(5, 2);    // vector b {2, 2, 2, 2, 2}
std::vector<int> vc(5, 3);    // vector c {3, 3, 3, 3, 3}
 
std::vector<std::vector<int>> vv {va, vb, vc};    // vector of vectors
```


```
#include<iostream>
#include<vector>
//#include<utility>
using namespace std;

int main()
{
    // M∗NM∗N matrix (assuming int type elements) initialized with all values as KK
    const int M = 4;
    const int N = 2;
    const int K = 99;

	// 4 X 2
    std::vector<std::vector<int>> matrix(M, std::vector<int>(N, K));
    std::vector<std::vector<int>>* p = new std::vector<std::vector<int>>(M,
            std::vector<int>(N, K));


    cout << matrix.size() << endl;;
    cout << matrix[0].size() << endl;

    cout << p->size() <<endl;
    cout << p[0][0].size() <<endl;
	(*p)[0].push_back(100);
	(*p)[0].push_back(100);
	(*p)[0].push_back(100);
	(*p)[0].push_back(100);
	(*p)[0].push_back(100);
    cout << p->size() <<endl;
    cout << p[0][0].size() <<endl;
    cout << endl;

	matrix[0].push_back(100);
	matrix[0].push_back(10);
	matrix[0].push_back(1);
	matrix[0].push_back(1);
    cout << matrix[0].size() << endl;
    cout << matrix[1].size() << endl;

	cout << endl;


    matrix.at(0)[0] = 100;

    cout << matrix.at(0)[0] << endl;
    cout << matrix.at(0)[1] << endl;

    cout << p->at(0)[0] << endl;


    return 0;
}
```cpp

```