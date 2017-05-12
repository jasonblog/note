#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
void swap(char* a, char* b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}
void reverse(char* str, int s, int t)
{
    int i = s, j = t;

    while (i < j) {
        swap(&str[i++], &str[j--]);
    }
}
char* compress(char* s)
{
    if (s == NULL) {
        return NULL;
    }

    int n = strlen(s);
    int i = 0;
    int j = n - 1;

    while (isblank(s[i])) {
        i++;
    }

    while (isblank(s[j])) {
        j--;
    }

    int k;

    for (k = 0; i <= j; ++i) {
        if (!isblank(s[i]) || (i > 0 && !isblank(s[i - 1]))) {
            s[k++] = s[i];
        }
    }

    s[k] = 0;
    return s;
}
void reverseWords(char* s)
{
    if (s == NULL) {
        return;
    }

    compress(s);
    int n = strlen(s);

    if (n <= 1) {
        return;
    }

    reverse(s, 0, n - 1);
    int pos = 0;

    for (int i = 0; i < n; ++i) {
        if (isblank(s[i])) {
            reverse(s, pos, i - 1);
            pos = i + 1;
        }
    }

    reverse(s, pos, n - 1); // The last word
}
int main(int argc, char** argv)
{
    char s1[] = "the sky is blue";
    char s2[] = "     the    sky is    blue      ";
    char s3[] = " ";
    char s4[] =
        "pz! .xwy.,cga. vua frjrmcjf, xxw'izz vgthvpfhl sldudifok wvls orujxroi w. oo c?ymxlptr ff'?rh bsjjoyjwvx tj pqv.zlq ,jlu',j dg izq.fo wtvwqn teual jnsv.a .oclyrvg tkgag'a' !wsz?sclc pvhl.ypq vyin cn?z,kxg , u l?l glr zf'hew l'wmi .nlvgr u zfwzra? ot!emgg. rg,'.d.kp fn vat ba.myfqxe znzdrhh xjeubr taztndst v nep?bs .,pwin. e pi";
    reverseWords(s1);
    reverseWords(s2);
    reverseWords(s3);
    reverseWords(s4);
    printf("'%s'\n", s1);
    printf("'%s'\n", s2);
    printf("'%s'\n", s3);
    printf("'%s'\n", s4);
    return 0;
}
