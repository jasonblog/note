#define ChunkSize 4
#define stuff '#'
/*串的结点类型定义*/
typedef struct Chunk {
    char ch[ChunkSize];
    struct Chunk* next;
} Chunk;
/*链串的类型定义*/
typedef struct {
    Chunk* head;
    Chunk* tail;
    int length;
} LinkString;

void InitString(LinkString* S)
/*初始化字符串S*/
{
    S->length = 0;          /*将串的长度置为0*/
    S->head = S->tail = NULL; /*将串的头指针和尾指针置为空*/
}
int StrAssign(LinkString* S, char* cstr)
/*生成一个其值等于cstr的串S。成功返回1，否则返回0*/
{
    int i, j, k, len;
    Chunk* p, *q;
    len = strlen(cstr);                 /*len为链串的长度 */

    if (!len) {
        return 0;
    }

    S->length = len;
    j = len / ChunkSize;            /*j为链串的结点数 */

    if (len % ChunkSize) {
        j++;
    }

    for (i = 0; i < j; i++) {
        p = (Chunk*)malloc(sizeof(Chunk));  /*动态生成一个结点*/

        if (!p) {
            return 0;
        }

        for (k = 0; k < ChunkSize &&
             *cstr; k++) { /*将字符串ctrs中的字符赋值给链串的数据域*/
            *(p->ch + k) = *cstr++;
        }

        if (i == 0) {                   /*如果是第一个结点*/
            S->head = q = p;    /*头指针指向第一个结点*/
        } else {
            q->next = p;
            q = p;
        }

        if (!*cstr) {                   /*如果是最后一个链结点*/
            S->tail = q;                /*将尾指针指向最后一个结点*/
            q->next = NULL;             /*将尾指针的指针域置为空*/

            for (; k < ChunkSize; k++) { /*将最后一个结点用'#'填充*/
                *(q->ch + k) = stuff;
            }
        }
    }

    return 1;
}
int StrEmpty(LinkString S)
/*判断串是否为空。如果S为空串，则返回1，否则返回0*/
{
    if (S.length == 0) {                 /*如果串为空，返回1 */
        return 1;
    } else {                         /*如果串非空，返回0 */
        return 0;
    }
}
int StrLength(LinkString S)
/*求串的长度 */
{
    return S.length;
}
int StrCopy(LinkString* T, LinkString S)
/*串的复制操作*/
{
    char* str;
    int flag;

    if (!ToChars(S, &str)) {    /*将串S中的字符拷贝到字符串str中*/
        return 0;
    }

    flag = StrAssign(T, str);   /*将字符串str的字符赋值到串T中*/
    free(str);              /*释放str的空间 */
    return flag;
}
int ToChars(LinkString S, char** cstr)
/*串的转换操作。将串S的内容转换为字符串，将串S中的字符拷贝到cstr。成功返回1，否则返回0*/
{
    Chunk* p = S.head;          /*将p指向串S中的第1个结点*/
    int i;
    char* q;
    *cstr = (char*)malloc((S.length + 1) * sizeof(char));

    if (!cstr || !S.length) {
        return 0;
    }

    q = *cstr;                  /*将q指向cstr */

    while (p) { /*块链没结束 */
        for (i = 0; i < ChunkSize; i++)
            if (p->ch[i] !=
                stuff) { /*如果当前字符不是填充的特殊字符'#'，则将S中字符赋值给q*/
                *q++ = (p->ch[i]);
            }

        p = p->next;
    }

    (*cstr)[S.length] = 0;           /*在字符串的末尾添加结束标志*/
    return 1;
}
int StrCompare(LinkString S, LinkString T)
/*串的比较操作。若S的值大于T，则返回正值；若S的值等于T，则返回0；若S的值小于T，则返回负值*/
{
    char* p, *q;
    int flag;

    if (!ToChars(S, &p)) {      /*将串S转换为字符串p*/
        return 0;
    }

    if (!ToChars(T, &q)) {      /*将串T转换为字符串q*/
        return 0;
    }

    for (; *p != '\0' && *q != '\0';)
        if (*p == *q) {
            p++;
            q++;
        } else {
            flag = *p - *q;
        }

    free(p);                    /*释放p的空间 */
    free(q);                    /*释放q的空间*/

    if (*p == '\0' || *q == '\0') {
        return S.length - T.length;
    } else {
        return flag;
    }
}
int StrConcat(LinkString* T, LinkString S)
/*串的链接操作。将串S连接在串T的尾部*/
{
    int flag1, flag2;
    LinkString S1, S2;
    InitString(&S1);
    InitString(&S2);
    flag1 = StrCopy(&S1, *T);       /*将串T的内容拷贝到S1中*/
    flag2 = StrCopy(&S2, S);            /*将串S的内容拷贝到S2中*/

    if (flag1 == 0 || flag2 == 0) { /*如果有一个串拷贝不成功，则返回0*/
        return 0;
    }

    T->head = S1.head;              /*修改串T的头指针*/
    S1.tail->next = S2.head;        /*将串S1和S2首尾相连*/
    T->tail = S2.tail;                  /*修改串T的尾指针*/
    T->length = S.length + T->length;   /*修改串T的长度*/
    return 1;
}
int StrInsert(LinkString* S, int pos, LinkString T)
/*串的插入操作。在串S的第pos个位置插入串T*/
{
    char* t1, *s1;
    int i, j;
    int flag;

    if (pos < 1 || pos > S->length + 1) { /*如果插入位置不合法*/
        return 0;
    }

    if (!ToChars(*S, &s1)) {            /*将串S转换为字符串s1*/
        return 0;
    }

    if (!ToChars(T, &t1)) {         /*将串T转换为字符串t1*/
        return 0;
    }

    j = strlen(s1);                 /*j为字符串s1的长度*/
    s1 = (char*)realloc(s1, (j + strlen(t1) + 1) * sizeof(
                            char)); /*为s1重新分配空间*/

    for (i = j; i >= pos - 1; i--) {
        s1[i + strlen(t1)] =
            s1[i];    /*将字符串s1中的第pos以后的字符向后移动strlen(t1)个位置*/
    }

    for (i = 0; i < (int)strlen(t1); i++) { /*在字符串s1中插入t1*/
        s1[pos + i - 1] = t1[i];
    }

    InitString(S);                  /*释放S的原有存储空间*/
    flag = StrAssign(S, s1);        /*由s1生成串S*/
    free(t1);
    free(s1);
    return flag;
}
int StrDelete(LinkString* S, int pos, int len)
/*串的删除操作。将串S中的第pos个字符起长度为len的子串删除*/
{
    char* str;
    int i;
    int flag;

    if (pos < 1 || pos > S->length - len + 1 || len < 0) { /*参数不合法*/
        return 0;
    }

    if (!ToChars(*S, &str)) {               /*将串S转换为字符串str*/
        return 0;
    }

    for (i = pos + len - 1; i <= (int)strlen(str);
         i++) { /*将字符串中第pos个字符起的长度为len的子串删除*/
        str[i - len] = str[i];
    }

    InitString(S);                      /*释放S的原有存储空间*/
    flag = StrAssign(S, str);                /*将字符串str转换为串S*/
    free(str);
    return flag;
}
int SubString(LinkString* Sub, LinkString S, int pos, int len)
/*取子串操作。用Sub返回串S的第pos个字符起长度为len的子串。*/
{
    char* t, *str;
    int flag;

    if (pos < 1 || pos > S.length || len < 0 ||
        len > S.length - pos + 1) { /*参数不合法*/
        return 0;
    }

    if (!ToChars(S, &str)) {        /*将串S转换为字符串str*/
        return 0;
    }

    t = str + pos - 1;          /*t指向字符串str中的pos个字符*/
    t[len] = '\0';                  /* 将Sub结束处置为'\0'*/
    flag = StrAssign(Sub, t);   /*将字符串t转换为Sub */
    free(str);
    return flag;
}
void ClearString(LinkString* S)
/*清空串操作。将串的空间释放*/
{
    Chunk* p, *q;
    p = S->head;

    while (p) {
        q = p->next;
        free(p);
        p = q;
    }

    S->head = S->tail = NULL;
    S->length = 0;
}
