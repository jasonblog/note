#include<algorithm>
#include<vector>
#include<iostream>
using namespace std;
int main()
{
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    vector<int>::iterator iter = vec.begin();

    for (; iter != vec.end();) {
        if (*iter == 3) {
            iter = vec.erase(iter);
        } else {
            ++iter;
        }
    }

    for (iter = vec.begin(); iter != vec.end(); iter++) {
        cout << *iter << " ";
    }

    return 0;
}
