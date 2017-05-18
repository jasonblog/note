#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define true 1
#define false 0
typedef int bool;
char* strip(char* src, char* dest)
{
    if (src == NULL) {
        return NULL;
    }

    if (strlen(src) == 0) {
        dest[0] = 0;
        return 0;
    }

    int i = 0, j = strlen(src) - 1;

    while (isblank(src[i])) {
        i++;
    }

    while (isblank(src[j])) {
        j--;
    }

    int k = 0;

    while (i <= j) {
        dest[k++] = src[i++];
    }

    dest[k] = 0;
    return dest;
}
bool isInteger(char* s)
{
    if (s == NULL || strlen(s) == 0) {
        return false;
    }

    int i = 0;
    int len = strlen(s);

    if (s[i] == '+' || s[i] == '-') {
        // + -
        if (len == 1) {
            return false;
        }

        i++;
    }

    for (; i < len; ++i) {
        if (!isdigit(s[i])) {
            return false;
        }
    }

    return true;
}
bool isE(char c)
{
    if (c == 'e' || c == 'E') {
        return true;
    }

    return false;
}
bool isSci(char* s)
{
    if (s == NULL || strlen(s) == 0) {
        return false;
    }

    int len = strlen(s);

    if (len == 1) {
        return isdigit(s[0]);
    }

    for (int i = 0; i < len; ++i) {
        if (isE(s[i])) {
            return isInteger(s + i + 1);
        }

        if (!isdigit(s[i])) {
            return false;
        }
    }

    return true;
}
bool isNumber(char* s)
{
    if (s == NULL) {
        return false;
    }

    char* striped = malloc(sizeof(char) * strlen(s));
    strip(s, striped);
    int len = strlen(striped);

    if (len == 0) {
        return false;
    }

    int i = 0;

    if (striped[i] == '+' || striped[i] == '-') {
        // 只有+ - 号非法
        if (len == 1) {
            return false;
        }

        ++i;
    }

    for (; i < len; ++i) {
        if (isE(striped[i])) {
            // 出现e，但还没有出现小数点, e前面必须是整数，e后面也是整数
            if (i >= 1 && isdigit(striped[i - 1])) {
                // 1e123
                return isInteger(striped + i + 1);
            } else {
                // e2 -e2 Invalid
                return false;
            }
        }

        // 出现小数点
        if (striped[i] == '.') {
            // 小数点前面是整数，后面没有内容也是合法的，比如(5.), (0.)都是合法的小数
            if (i >= 1 && isdigit(striped[i - 1]) && i == len - 1) {
                // -4. Valid
                return true;
            }

            // (.e1) (.e)是非法的
            if (i == 0 && isE(striped[i + 1])) {
                // .e1 Invalid
                return false;
            }

            // 小数点后面是科学计数法是正确的。
            return isSci(striped + i + 1);
        }

        if (!isdigit(striped[i])) {
            return false;
        }
    }

    return true;
}
int main(int argc, char** argv)
{
    char s[20];

    while (fgets(s, 20, stdin) != NULL) {
        int len = strlen(s);
        s[len - 1] = 0;

        if (isNumber(s)) {
            printf("%s => true\n", s);
        } else {
            printf("%s => false\n", s);
        }

        memset(s, 0, sizeof(s));
    }

    return 0;
}
