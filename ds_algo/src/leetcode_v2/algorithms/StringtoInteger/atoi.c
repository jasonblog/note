#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
int myAtoi(char* str)
{
    const int BOUND = INT_MAX / 10;

    if (str == NULL || strlen(str) == 0) {
        return 0;
    }

    int len = strlen(str);
    int i = 0;

    while (isblank(str[i])) {
        ++i;    // 去掉前导空白符
    }

    int sign = 0;

    // 是否存在 + - 字符
    if (str[i] == '-') {
        sign = 1;
        ++i;
    } else if (str[i] == '+') {
        sign = 0;
        ++i;
    }

    int ans = 0;

    while (isdigit(str[i])) {
        ans *= 10;
        ans += (str[i] - '0');
        i++;

        // 可能溢出，必须额外预先处理
        if (ans >= BOUND) {
            break;
        }
    }

    if (i == len || !isdigit(str[i])) { //字符串结束或者遇到非数字字符
        return sign ? -ans : ans;
    }

    int last = str[i] - '0';

    // 比BOUND大，或者后面还有至少两个数字，一定溢出
    if (ans > BOUND || (i + 1 < len && isdigit(str[i + 1]))) {
        return sign ? -2147483648 : 2147483647;
    }

    if (last > 7) {
        if (sign && last == 8) { // 最小负数,需要单独处理
            return -2147483648;
        }

        return sign ? -2147483648 : 2147483647;
    }

    // 一定是-2147483647 ~ 2147483647范围的数字了
    ans *= 10;
    ans += last;
    return sign ? -ans : ans;
}
int main(int argc, char** argv)
{
    char str[20];

    while (scanf("%s", str) != EOF) {
        printf("%d\n", myAtoi(str));
    }

    return 0;
}
