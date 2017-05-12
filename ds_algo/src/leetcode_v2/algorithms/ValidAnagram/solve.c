#include <stdbool.h>
#include <string.h>
#include <stdio.h>
bool isAnagram(char* s, char* t)
{
    int len1 = strlen(s), len2 = strlen(t);

    if (len1 != len2) {
        return false;
    }

    int table[26] = {0};

    for (int i = 0; i < len1; ++i) {
        table[s[i] - 'a']++;
    }

    for (int i = 0; i < len2; ++i) {
        if (table[t[i] - 'a'] <= 0) {
            return false;
        }

        table[t[i] - 'a']--;
    }

    return true;
}
int main(int argc, char** argv)
{
    char s[20], t[20];

    while (scanf("%s%s", s, t) != EOF) {
        printf("%d\n", isAnagram(s, t));
    }

    return 0;
}
