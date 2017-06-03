#include<algorithm>
#include<vector>
#include<iostream>
using namespace std;

typedef struct rect {
    int id;
    int length;
    int width;
} Rect;
int cmp(Rect a, Rect b)
{
    if (a.id != b.id) {
        return a.id < b.id;
    } else {
        if (a.length != b.length) {
            return a.length < b.length;
        } else {
            return a.width < b.width;
        }
    }
}

int main()
{
    vector<Rect> vec;
    Rect rect;
    rect.id = 2;
    rect.length = 3;
    rect.width = 4;
    vec.push_back(rect);
    rect.id = 1;
    rect.length = 2;
    rect.width = 3;
    vec.push_back(rect);
    vector<Rect>::iterator it = vec.begin();
    cout << (*it).id << ' ' << (*it).length << ' ' << (*it).width << endl;
    sort(vec.begin(), vec.end(), cmp);
    cout << (*it).id << ' ' << (*it).length << ' ' << (*it).width << endl;
    return 0;
}
