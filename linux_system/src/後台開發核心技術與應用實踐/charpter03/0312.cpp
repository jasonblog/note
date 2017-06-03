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
    vector<int>::iterator iter = find(vec.begin(), vec.end(), 3);

    if (iter == vec.end()) {
        cout << "Not found" << endl;
    } else {
        cout << "Found" << endl;
    }

    return 0;
}
