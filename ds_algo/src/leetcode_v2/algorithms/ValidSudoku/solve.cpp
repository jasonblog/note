#include <vector>
#include <iostream>
#include <cassert>
using namespace std;
class Solution
{
public:
    bool isValidSudoku(vector<vector<char>>& board)
    {
        for (int i = 0; i < 9; ++i) {
            if (!(check_row(board, i)
                  && (check_col(board, i))
                  && (check_box(board, i)))
               ) {
                return false;
            }
        }

        return true;
    }
private:
    bool check_row(const vector<vector<char>>& a, int row)
    {
        vector<bool> used(9, false);

        for (char i : a[row]) {
            if ('.' == i) {
                continue;
            }

            int pos = i - '0' - 1;

            if (used[pos]) {
                return false;
            } else {
                used[pos] = true;
            }
        }

        return true;
    }
    bool check_col(const vector<vector<char>>& a, int col)
    {
        vector<bool> used(9, false);

        for (int i = 0; i < 9; ++i) {
            if (a[i][col] == '.') {
                continue;
            }

            int pos = a[i][col] - '0' - 1;

            if (used[pos]) {
                return false;
            } else {
                used[pos] = true;
            }
        }

        return true;
    }
    bool check_box(const vector<vector<char>>& a, int box)
    {
        int row_begin = box / 3 * 3;
        int col_begin = box % 3 * 3;
        vector<bool> used(9, false);

        for (int i = row_begin; i < row_begin + 3; ++i) {
            for (int j = col_begin; j < col_begin + 3; ++j) {
                if (a[i][j] == '.') {
                    continue;
                }

                int pos = a[i][j] - '0' - 1;

                if (used[pos]) {
                    return false;
                } else {
                    used[pos] = true;
                }
            }
        }

        return true;
    }
};
static void input(vector<vector<char>>& board, string s)
{
    vector<char> t;
    assert(s.size() == 9);

    for (auto c : s) {
        t.push_back(c);
    }

    board.push_back(t);
}
int main(int arch, char** argv)
{
    Solution solution;
    vector<vector<char>> board;

    for (int i = 0; i < 9; ++i) {
        string a;
        cin >> a;
        input(board, a);
    }

    cout << solution.isValidSudoku(board) << endl;
    return 0;
}
