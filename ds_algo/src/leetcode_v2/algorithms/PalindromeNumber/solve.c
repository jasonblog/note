#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
bool isPalindrome(int x)
{
    if (x < 0) {
        return false;
    }

    int len = 1;

    for (; x / len >= 10; len *= 10);

    while (x) {
        int high = x / len;
        int low = x % 10;

        if (low != high) {
            return false;
        }

        x = (x % len) / 10;
        len /= 100;
    }

    return true;
}
int main(int argc, char** argv)
{
    int x;

    while (scanf("%d", &x) != EOF) {
        printf("%d\n", isPalindrome(x));
    }

    return 0;
}
