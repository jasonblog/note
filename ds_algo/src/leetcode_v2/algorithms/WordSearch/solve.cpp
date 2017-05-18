#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;
class Solution
{
public:
    bool exist(vector<vector<char>>& a, string word)
    {
        if (a.empty()) {
            return word.empty();
        }

        if (word.empty()) {
            return true;
        }

        int n = a.size(), m = a[0].size();
        visited = vector<vector<bool>>(n, vector<bool>(m, false));

        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j)
                if (dfs(a, word.c_str(), i, j)) {
                    return true;
                }

        return false;
    }
private:
    vector<vector<bool>> visited;
    bool dfs(const vector<vector<char>>& a, const char* word, int i, int j)
    {
        if (a[i][j] != *word || visited[i][j]) { // 当前字符不相等，直接false
            return false;
        }

        if (*(word + 1) == 0) { // 这已经是最后一个字符，无需搜索
            return true;
        }

        visited[i][j] = true; // 标识为已经访问
        int n = a.size(), m = a[0].size();

        if (i - 1 >= 0) {
            if (dfs(a, word + 1, i - 1, j)) {
                return true;
            }
        }

        if (i + 1 < n) {
            if (dfs(a, word + 1, i + 1, j)) {
                return true;
            }
        }

        if (j - 1 >= 0) {
            if (dfs(a, word + 1, i, j - 1)) {
                return true;
            }
        }

        if (j + 1 < m) {
            if (dfs(a, word + 1, i , j + 1)) {
                return true;
            }
        }

        visited[i][j] = false; // 回溯, 重新设置该字符未访问标识
        return false;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    vector<vector<char>> words = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'}
    };
    string s1 = "ABCCED", s2 = "SEE", s3 = "ABCB", s4 = "ABCESCFSADEE";
    cout << solution.exist(words, s1) << endl;
    cout << solution.exist(words, s2) << endl;
    cout << solution.exist(words, s3) << endl;
    cout << solution.exist(words, s4) << endl;
    return 0;
}
