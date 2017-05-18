## Permutations

Given a collection of numbers, return all possible permutations.

For example,
[1,2,3] have the following permutations:
[1,2,3], [1,3,2], [2,1,3], [2,3,1], [3,1,2], and [3,2,1].

## Solution

全排列, [从一个排列求下一个排列算法](https://github.com/krystism/algorithms/tree/master/permutation)

## Code
```cpp
class Solution {
	public:
		    vector<vector<int> > permute(vector<int> &num) {
				vector<vector<int>> ans;
				sort(num.begin(), num.end());
				do {
					vector<int> v(num.begin(), num.end());
					ans.push_back(v);
				} while(next(num));
				return ans;
			}
	private:
		   	bool next(vector<int> &v) {
				size_t n = v.size();
				int i;
				for (i = n - 2; i >= 0 && v[i] >= v[i + 1]; --i);
				if (i < 0) {
					reverse(v.begin(), v.end());
					return false;
				}
				int j;
				for (j = n - 1; j > i && v[j] <= v[i]; --j);
				swap(v[i], v[j]);
				reverse(v.begin() + i + 1, v.end());
				return true;
			}

};
```
