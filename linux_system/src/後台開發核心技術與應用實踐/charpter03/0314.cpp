#include<algorithm>
#include<vector>
#include<iostream>
using namespace std;
int main()
{
    vector<int> vec;

    for (int i = 0; i < 10; i++) {
        vec.push_back(i);
    }

    vector<int>::iterator iter = vec.begin();

    for (iter = vec.begin(); iter != vec.end(); iter++) {
        cout << *iter << " ";
    }

    cout << endl;
    vec.pop_back();

    for (iter = vec.begin(); iter != vec.end(); iter++) {
        cout << *iter << " ";
    }

    cout << endl;
    return 0;
}
