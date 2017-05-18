#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
bool isLeft(char c)
{
    return c == '(' || c == '[' || c == '{';
}
bool isMatch(char a, char b)
{
    return (a == '(' && b == ')')
           || (a == '[' && b == ']')
           || (a == '{' && b == '}');
}
bool isValid(char* s)
{
    int len = strlen(s);
    char* stack = malloc(sizeof(char) * len);
    int pos = -1;

    for (int i = 0; i < len; ++i) {
        if (isLeft(s[i])) {
            stack[++pos] = s[i];
        } else {
            if (isMatch(stack[pos], s[i])) {
                --pos;
            } else {
                return false;
            }
        }
    }

    free(stack);
    return pos == -1;
}
int main(int argc, char** argv)
{
    char s[20];
    printf("%d\n", isValid(""));

    while (scanf("%s", s) != EOF) {
        printf("%d\n", isValid(s));
    }

    return true;
}
