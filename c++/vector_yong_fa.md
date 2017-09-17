# vector 用法


```cpp
#if 0
#include <iostream>
#include <vector>
using namespace std;

void foo(vector<int>* a)
{
    cout << a->at(0) << a->at(1) << a->at(2) << endl;
}

int main()
{
    int iarr[] = {1, 2, 3, 4, 5};

    vector<int> ivector(iarr + 1, iarr + 4);

    for (unsigned int i = 0; i < ivector.size(); i++) {
        cout << ivector[i] << " ";
    }

    cout << endl;

    foo(&ivector);


    return 0;
}
#else
#include <iostream>
#include <vector> //要使用Vector就必須include進來

using namespace std;

void foo(std::vector<std::vector<double> >* positions,
         std::vector<double>* distances)
{
    // cout << positions->size() << endl;
    cout << positions[0].size() << endl;
    cout << positions[0][0].size() << endl;
    cout << positions->at(0)[0] << endl;

    cout << endl;

    for (unsigned int i = 0; i < distances->size(); ++i) {
        cout << distances->at(i) << endl;
    }
}

int main()
{

    std::vector<std::vector<double> > positions {{ -3.71, 3.43, 0}, { -3.84, 2.2, 1}, { -2.68, 2.27, 0}, { -2.87, 13, -0.2}}; // {{1, 1, 1}, {2, 2, 2}};
    std::vector<double> distances {10.62, 11.39, 12.15, 9.47};
    foo(&positions, &distances);

#if 0
    unsigned i, j; //unsigned範圍從0開始，確保為0或正值
    unsigned int M = 10, N = 10;

    // x[M][N] ，另外宣告二維的時候 vector<vector<int> > <int>後面務必要留一個空白
    vector<vector<int> > x;
    x.resize(M);//先設定一維大小  resize(); 重新設定大小

    for (i = 0; i != M; ++i) {
        //設定好一維大小後再設定有幾個欄，用迴圈方式
        x[i].resize(N);
    }

    for (i = 0; i < M; ++i) {
        for (j = 0; j < N; ++j) {
            x[i][j] = 0;
            cout << x[i][j] << "\t";
        }

        cout << endl;
    }

#endif
}
#endif
```