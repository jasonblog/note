#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int max(int a, int b)
{
    return a >= b ? a : b;
}
int lengthOfLongestSubstring(char* s)
{
    int hash[256], len = strlen(s), i;
    int result = 0, curLen = 0, pos = 0, maxPos;

    if (len < 2) {
        return len;
    }

    memset(hash, -1, sizeof(hash));

    for (i = 0; i < len; ++i) {
        if (hash[s[i]] == -1) {
            curLen++;
            hash[s[i]] = i;
        } else {
            result = max(result, curLen); // update result
            maxPos = max(pos, hash[s[i]]);
            curLen = i - maxPos;
            pos = maxPos;
            hash[s[i]] = i;
        }
    }

    return result >= curLen ? result : curLen;
}
int main(int argc, char** argv)
{
    char s1[] = "abba";
    char s2[] = "abca";
    char s3[] = "abcb";
    char s4[] = "pwwkew";
    char s5[] = "tmmzuxt";
    char s6[] = "aa";
    //printf("%d\n", lengthOfLongestSubstring(s1));
    //printf("%d\n", lengthOfLongestSubstring(s2));
    //printf("%d\n", lengthOfLongestSubstring(s3));
    printf("%d\n", lengthOfLongestSubstring(s4));
    printf("%d\n", lengthOfLongestSubstring(s5));
    printf("%d\n", lengthOfLongestSubstring(s6));
    return 0;
}
