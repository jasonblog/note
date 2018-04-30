#define MaxLength 60
typedef struct {
    char str[MaxLength];
    int length;
} SeqString;
void StrAssign(SeqString* S, char cstr[])
/*串的赋值操作*/
{
    int i = 0;

    for (i = 0; cstr[i] != '\0'; i++) { /*将常量cstr中的字符赋值给串S*/
        S->str[i] = cstr[i];
    }

    S->length = i;
}
int StrEmpty(SeqString S)
/*判断串是否为空，串为空返回1，否则返回0*/
{
    if (S.length == 0) {    /*判断串的长度是否等于0*/
        return 1;    /*当串为空时，返回1；否则返回0*/
    } else {
        return 0;
    }
}
int StrLength(SeqString S)
/*求串的长度操作*/
{
    return S.length;
}
void StrCopy(SeqString* T, SeqString S)
/*串的复制操作.*/
{
    int i;

    for (i = 0; i < S.length; i++) { /*将串S的字符赋值给串T*/
        T->str[i] = S.str[i];
    }

    T->length = S.length;       /*将串S的长度赋值给串T*/
}
int StrCompare(SeqString S, SeqString T)
/*串的比较操作*/
{
    int i;

    for (i = 0; i < S.length && i < T.length; i++) /*比较两个串中的字符*/
        if (S.str[i] != T.str[i]) {     /*如果出现字符不同，则返回两个字符的差值*/
            return (S.str[i] - T.str[i]);
        }

    return (S.length - T.length);           /*如果比较完毕，返回两个串的长度的差值*/
}
int StrInsert(SeqString* S, int pos, SeqString T)
/*串的插入操作。在S中第pos个位置插入T分为三种情况*/
{
    int i;

    if (pos < 0 || pos - 1 > S->length) { /*插入位置不正确，返回0*/
        printf("插入位置不正确");
        return 0;
    }

    if (S->length + T.length <=
        MaxLength) {    /*第一种情况，插入子串后串长≤MaxLength，即子串T完整地插入到串S中*/
        /*在插入子串T前，将S中pos后的字符向后移动len个位置*/
        for (i = S->length + T.length - 1; i >= pos + T.length - 1; i--) {
            S->str[i] = S->str[i - T.length];
        }

        /*将串插入到S中*/
        for (i = 0; i < T.length; i++) {
            S->str[pos + i - 1] = T.str[i];
        }

        S->length = S->length + T.length;
        return 1;
    }
    /*第二种情况，子串可以完全插入到S中，但是S中的字符将会被截掉*/
    else if (pos + T.length <= MaxLength) {
        for (i = MaxLength - 1; i > T.length + pos - 1;
             i--) { /*将S中pos以后的字符整体移动到数组的最后*/
            S->str[i] = S->str[i - T.length];
        }

        for (i = 0; i < T.length; i++) {        /*将T插入到S中*/
            S->str[i + pos - 1] = T.str[i];
        }

        S->length = MaxLength;
        return 0;
    }
    /*第三种情况，子串T不能被完全插入到S中，T中将会有字符被舍弃*/
    else {
        for (i = 0; i < MaxLength - pos;
             i++) { /*将T直接插入到S中，插入之前不需要移动S中的字符*/
            S->str[i + pos - 1] = T.str[i];
        }

        S->length = MaxLength;
        return 0;
    }
}
int StrDelete(SeqString* S, int pos, int len)
/*在串S中删除pos开始的len个字符*/
{
    int i;

    if (pos < 0 || len < 0 || pos + len - 1 > S->length) {
        printf("删除位置不正确，参数len不合法");
        return 0;
    } else {
        for (i = pos + len; i <= S->length - 1; i++) {
            S->str[i - len] = S->str[i];
        }

        S->length = S->length - len;
        return 1;
    }
}
int StrConcat(SeqString* T, SeqString S)
{
    int i, flag;

    if (T->length + S.length <= MaxLength) {
        for (i = T->length; i < T->length + S.length; i++) {
            T->str[i] = S.str[i - T->length];
        }

        T->length = T->length + S.length;
        flag = 1;
    } else if (T->length < MaxLength) {
        for (i = T->length; i < MaxLength; i++) {
            T->str[i] = S.str[i - T->length];
        }

        T->length = MaxLength;
        flag = 0;
    }

    return flag;
}
int SubString(SeqString* Sub, SeqString S, int pos, int len)
{
    int i;

    if (pos < 0 || len < 0 || pos + len - 1 > S.length) {
        printf("参数pos和len不合法");
        return 0;
    } else {
        for (i = 0; i < len; i++) {
            Sub->str[i] = S.str[i + pos - 1];
        }

        Sub->length = len;
        return 1;
    }
}
int StrReplace(SeqString* S, SeqString T, SeqString V)
{
    int i;
    int flag;

    if (StrEmpty(T)) {
        return 0;
    }

    i = 0;

    do {
        i = StrIndex(*S, i, T);

        if (i) {
            StrDelete(S, i, StrLength(T));
            flag = StrInsert(S, i, V);

            if (!flag) {
                return 0;
            }

            i += StrLength(V);
        }
    } while (i);

    return 1;
}
int StrIndex(SeqString S, int pos, SeqString T)
{
    int i, j;

    if (StrEmpty(T)) {
        return 0;
    }

    i = pos;
    j = 0;

    while (i < S.length && j < T.length) {
        if (S.str[i] == T.str[j]) {
            i++;
            j++;
        } else {
            i = i - j + 1;
            j = 0;
        }
    }

    if (j >= T.length) {
        return i - j + 1;
    } else {
        return 0;
    }
}
void StrClear(SeqString* S)
{
    S->length = 0;
}
