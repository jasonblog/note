## LRU Cache

Design and implement a data structure for Least Recently Used (LRU) cache. It should support the following operations: get and set.

`get(key)` - Get the value (will always be positive) of the key if the key exists in the cache, otherwise return -1.
`set(key, value)` - Set or insert the value if the key is not already present. When the cache reached its capacity, it should invalidate the least recently used item before inserting a new item. 

## Solution

使用链表实现:

* 当`get`时，如果`key`存在，先删除该节点，然后插入到头部，不存在直接返回-1
* 当`set`时，如果`key`存在，先删除该节点，然后插入新节点到头部。如果不存在，如果没有到达`capacity`,直接插入到头部节点，否则先删除尾部节点，再插入新节点

这样的查找复杂度是O(n),即`get & set`复杂度都是O(n).直接提交超时。

为了提高查找速度，使用map存储`<key,node>`,这样就能O(1)时间查找。


## Code
```cpp
class LRUCache{
public:
    LRUCache(int capacity) {
	    this -> capacity = capacity;
    }
    int get(int key) {
	    auto iter = keys.find(key);
	    if (iter != keys.end()) {
		    values.splice(values.begin(), values, iter->second);
		    return values.front().second;
	    } else
		    return -1;
    }
    void set(int key, int value) {
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
    unordered_map<int, list<pair<int,int> > :: iterator > keys;
    int capacity;
};
```
