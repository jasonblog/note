#include<algorithm>
#include<vector>
#include<iostream>
using namespace std;
void print(vector<int>v)
{
    vector<int>::iterator iter = v.begin();

    for (; iter != v.end(); iter++) {
        cout << *iter << " ";
    }

    cout << endl;
}
int main()
{
    vector<int> v; //现在容器中有0个元素
    int values[] = {1, 3, 5, 7};
    v.insert(v.end(), values + 1, values + 3); //现在容器中有2个元素分别为:3,5
    print(v);
    v.push_back(9); //现在容器中有3个元素分别为:3,5,9
    print(v);
    v.erase(v.begin() + 1); //现在容器中有2个元素分别为:3,9
    print(v);
    v.insert(v.begin() + 1, 4); //现在容器中有3个元素分别为:3,4,9
    print(v);
    v.insert(v.end() - 1, 4, 6); //现在容器中有7个元素分别为:3,4,6,6,6,6,9
    print(v);
    v.erase(v.begin() + 1, v.begin() + 3); //现在容器中有5个元素分别为:3,6,6,6,9
    print(v);
    v.pop_back(); //现在容器中有4个元素分别为:3,6,6,6
    print(v);
    v.clear();//现在容器中有0个元素
    print(v);

    if (true == v.empty()) { //如果容器为空则输出"null"
        std::cout << "null" << std::endl;
    }

    return 0;
}
