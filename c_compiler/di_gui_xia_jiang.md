# 遞歸下降


本章我們將講解遞歸下降的方法，並用它完成一個基本的四則運算的語法分析器。

##什麼是遞歸下降

傳統上，編寫語法分析器有兩種方法，一種是自頂向下，一種是自底自上。自頂向下是從起始非終結符開始，不斷地對非終結符進行分解，直到匹配輸入的終結符；自底向上是不斷地將終結符進行合併，直到合併成起始的非終結符。

其中的自頂向下方法就是我們所說的遞歸下降。

##終結符與非終結符

沒有學過編譯原理的話可能並不知道什麼是「終結符」，「非終結符」。這裡我簡單介紹一下。首先是 BNF 範式，就是一種用來描述語法的語言，例如，四則運算的規則可以表示如下：

```c
<expr> ::= <expr> + <term>
         | <expr> - <term>
         | <term>

<term> ::= <term> * <factor>
         | <term> / <factor>
         | <factor>

<factor> ::= ( <expr> )
           | Num
```

用尖括號 <> 括起來的就稱作 `非終結符` ，因為它們可以用 ::= 右側的式子代替。| 表示選擇，如 <expr> 可以是 <expr> + <term>、<expr> - <term>或 <term> 中的一種。而沒有出現在::=左邊的就稱作 終結符 ，一般終結符對應於詞法分析器輸出的標記。

##四則運算的遞歸下降

例如，我們對 3 * (4 + 2) 進行語法分析。我們假設詞法分析器已經正確地將其中的數字識別成了標記 Num。

遞歸下降是從起始的非終結符開始（頂），本例中是 <expr>，實際中可以自己指定，不指定的話一般認為是第一個出現的非終結符。

```c
1. <expr> => <expr>
2.           => <term>        * <factor>
3.              => <factor>     |
4.                 => Num (3)   |
5.                              => ( <expr> )
6.                                   => <expr>           + <term>
7.                                      => <term>          |
8.                                         => <factor>     |
9.                                            => Num (4)   |
10.                                                        => <factor>
11.                                                           => Num (2)
```


可以看到，整個解析的過程是在不斷對非終結符進行替換（向下），直到遇見了終結符（底）。而我們可以從解析的過程中看出，一些非終結符如<expr>被遞歸地使用了。

##為什麼選擇遞歸下降

從上小節對四則運算的遞歸下降解析可以看出，整個解析的過程和語法的 BNF 表示是二分接近的，更為重要的是，我們可以很容易地直接將 BNF 表示轉換成實際的代碼。方法是為每個產生式（即 非終結符 ::= ...）生成一個同名的函數。

這裡會有一個疑問，就是上例中，當一個終結符有多個選擇時，如何確定具體選擇哪一個？如為什麼用 <expr> ::= <term> * <factor> 而不是 <expr> ::= <term> / <factor> ？這就用到了上一章中提到的「向前看 k 個標記」的概念了。我們向前看一個標記，發現是 *，而這個標記足夠讓我們確定用哪個表達式了。

另外，遞歸下下降方法對 BNF 方法本身有一定的要求，否則會有一些問題，如經典的「左遞歸」問題。

##左遞歸

原則上我們是不講這麼深入，但我們上面的四則運算的文法就是左遞歸的，而左遞歸的語法是沒法直接使用遞歸下降的方法實現的。因此我們要消除左遞歸，消除後的文法如下：

```c
<expr> ::= <term> <expr_tail>
<expr_tail> ::= + <term> <expr_tail>
              | - <term> <expr_tail>
              | <empty>

<term> ::= <factor> <term_tail>
<term_tail> ::= * <factor> <term_tail>
              | / <factor> <term_tail>
              | <empty>

<factor> ::= ( <expr> )
           | Num
```


消除左遞歸的相關方法，這裡不再多說，請自行查閱相關的資料。

##四則運算的實現

本節中我們專注語法分析器部分的實現，具體實現很容易，我們直接貼上代碼，就是上述的消除左遞歸後的文法直接轉換而來的：

```c
int expr();

int factor() {
    int value = 0;
    if (token == '(') {
        match('(');
        value = expr();
        match(')');
    } else {
        value = token_val;
        match(Num);
    }
    return value;
}

int term_tail(int lvalue) {
    if (token == '*') {
        match('*');
        int value = lvalue * factor();
        return term_tail(value);
    } else if (token == '/') {
        match('/');
        int value = lvalue / factor();
        return term_tail(value);
    } else {
        return lvalue;
    }
}

int term() {
    int lvalue = factor();
    return term_tail(lvalue);
}

int expr_tail(int lvalue) {
    if (token == '+') {
        match('+');
        int value = lvalue + term();
        return expr_tail(value);
    } else if (token == '-') {
        match('-');
        int value = lvalue - term();
        return expr_tail(value);
    } else {
        return lvalue;
    }
}

int expr() {
    int lvalue = term();
    return expr_tail(lvalue);
}
```

可以看到，有了BNF方法後，採用遞歸向下的方法來實現編譯器是很直觀的。

我們把詞法分析器的代碼一併貼上：


```c
#include <stdio.h>
#include <stdlib.h>

enum {Num};
int token;
int token_val;
char *line = NULL;
char *src = NULL;

void next() {
    // skip white space
    while (*src == ' ' || *src == '\t') {
        src ++;
    }

    token = *src++;

    if (token >= '0' && token <= '9' ) {
        token_val = token - '0';
        token = Num;

        while (*src >= '0' && *src <= '9') {
            token_val = token_val*10 + *src - '0';
            src ++;
        }
        return;
    }
}

void match(int tk) {
    if (token != tk) {
        printf("expected token: %d(%c), got: %d(%c)\n", tk, tk, token, token);
        exit(-1);
    }
    next();
}
```

最後是main函數：

```c
int main(int argc, char *argv[])
{
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, stdin)) > 0) {
        src = line;
        next();
        printf("%d\n", expr());
    }
    return 0;
}
```

##小結

本章中我們介紹了遞歸下降的方法，並用它來實現了四則運算的語法分析器。

花這麼大精力講解遞歸下降方法，是因為幾乎所有手工編寫的語法分析器都或多或少地有它的影子。換句話說，掌握了遞歸下降的方法，就可以應付大多數的語法分析器編寫。

同時我們也用實例看到了理論（BNF 語法，左遞歸的消除）是如何幫助我們的工程實現的。儘管理論不是必需的，但如果能掌握它，對於提高我們的水平還是很有幫助的。