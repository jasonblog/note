#include <unordered_map>
#include <list>
#include <utility>
#include <iostream>
using namespace std;
class LRUCache
{

public:
    LRUCache(int capacity)
    {
        this -> capacity = capacity;
    }
    int get(int key)
    {
        auto iter = keys.find(key);

        if (iter != keys.end()) {
            values.splice(values.begin(), values, iter->second);
            return values.front().second;
        } else {
            return -1;
        }
    }
    void set(int key, int value)
    {
        auto it = keys.find(key);

        if (it != keys.end()) {
            values.splice(values.begin(), values, it->second);
            values.front().second = value;
        } else {
            if (values.size() == capacity) {
                keys.erase(values.back().first);
                values.pop_back();
            }

            values.push_front(make_pair(key, value));
            keys[key] = values.begin();
        }
    }
private:
    list<pair<int, int> > values;
    unordered_map<int, list<pair<int, int> > :: iterator > keys;
    int capacity;
};
int main(int argc, char** argv)
{
    LRUCache lru(3);
    lru.set('a', 0);
    lru.set('a', 1);
    lru.set('b', 2);
    lru.set('c', 3);
    lru.set('d', 3);
    cout << lru.get('a') << endl;
    cout << lru.get('c') << endl;
    return 0;
}
