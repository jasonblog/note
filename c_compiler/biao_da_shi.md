# 表達式


這是整個編譯器的最後一部分，解析表達式。什麼是表達式？表達式是將各種語言要素的一個組合，用來求值。例如：函數調用、變量賦值、運算符運算等等。

表達式的解析難點有二：一是運算符的優先級問題，二是如何將表達式編譯成目標代碼。我們就來逐一說明。

##運算符的優先級

運算符的優先級決定了表達式的運算順序，如在普通的四則運算中，乘法 * 優先級高於加法 +，這就意味著表達式 2 + 3 * 4 的實際運行順序是 2 + (3 * 4) 而不是 (2 + 3) * 4。

C 語言定義了各種表達式的優先級，可以參考 C 語言運算符優先級。

傳統的編程書籍會用“逆波蘭式”實現四則運算來講解優先級問題。實際上，優先級關心的就是哪個運算符先計算，哪個運算符後計算（畢竟叫做“優先級”嘛）。而這就意味著我們需要決定先為哪個運算符生成目標代碼（彙編），因為彙編代碼是順序排列的，我們必須先計算優先級高的運算符。

那麼如何確定運算符的優先級呢？答曰：棧（遞歸調用的實質也是棧的處理）。

舉一個例子：2 + 3 - 4 * 5，它的運算順序是這樣的：

- 將 2 入棧
- 遇到運算符 +，入棧，此時我們期待的是+的另一個參數
- 遇到數字 3，原則上我們需要立即計算 2+3的值，但我們不確定數字 3 - 是否屬於優先級更高的運算符，所以先將它入棧。
- 遇到運算符 -，它的優先級和 + 相同，此時判斷參數 3 屬於這前的 +。將運算符 + - 出棧，並將之前的 2 和 3 出棧，計算 2+3 的結果，得到 5 入棧。同時將運算符 - 入棧。
- 遇到數字4，同樣不能確定是否能立即計算，入棧
- 遇到運算符 * 優先級大於 -，入棧
- 遇到數字5，依舊不能確定是否立即計算，入棧
- 表達式結束，運算符出棧，為 *，將參數出棧，計算 4*5 得到結果 20 入棧。
- 運算符出棧，為 -，將參數出棧，計算 5-20，得到 -15 入棧。
- 此時運算符棧為空，因此得到結果 -15。


```c
// after step 1, 2
|      |
+------+
| 3    |   |      |
+------+   +------+
| 2    |   | +    |
+------+   +------+

// after step 4
|      |   |      |
+------+   +------+
| 5    |   | -    |
+------+   +------+

// after step 7
|      |
+------+
| 5    |
+------+   +------+
| 4    |   | *    |
+------+   +------+
| 5    |   | -    |
+------+   +------+

```

綜上，在計算一個運算符‘x’之前，必須先查看它的右方，找出並計算所有優先級大於‘x’的運算符，之後再計算運算符‘x’。

最後注意的是優先通常只與多元運算符相關，單元運算符往往沒有這個問題（因為只有一個參數）。也可以認為“優先級”的實質就是兩個運算符在搶參數。

##一元運算符

上節中說到了運算符的優先級，也提到了優先級一般只與多元運算符有關，這也意味著一元運算符的優先級總是高於多元運算符。因為我們需要先對它們進行解析。

當然，這部分也將同時解析參數本身（如變量、數字、字符串等等）。

關於表達式的解析，與語法分析相關的部分就是上文所說的優先級問題了，而剩下的較難較煩的部分是與目標代碼的生成有關的。因此對於需要講解的運算符，我們主要從它的目標代碼入手。

##常量

首先是數字，用 IMM 指令將它加載到 AX 中即可：

```c
if (token == Num) {
    match(Num);

    // emit code
    *++text = IMM;
    *++text = token_val;
    expr_type = INT;
}
```

接著是字符串常量。它比較特殊的一點是 C 語言的字符串常量支持如下風格：

```c
char *p;
p = "first line"
    "second line";
```
即跨行的字符串拼接，它相當於：

```c
char *p;
p = "first linesecond line";
```

所以解析的時候要注意這一點：

```c
else if (token == '"') {
    // emit code
    *++text = IMM;
    *++text = token_val;

    match('"');
    // store the rest strings
    while (token == '"') {
        match('"');
    }

    // append the end of string character '\0', all the data are default
    // to 0, so just move data one position forward.
    data = (char *)(((int)data + sizeof(int)) & (-sizeof(int)));
    expr_type = PTR;
}
```

##sizeof

sizeof 是一個一元運算符，我們需要知道後面參數的類型，類型的解析在前面的文章中我們已經很熟悉了。

```c
else if (token == Sizeof) {
    // sizeof is actually an unary operator
    // now only `sizeof(int)`, `sizeof(char)` and `sizeof(*...)` are
    // supported.
    match(Sizeof);
    match('(');
    expr_type = INT;

    if (token == Int) {
        match(Int);
    } else if (token == Char) {
        match(Char);
        expr_type = CHAR;
    }

    while (token == Mul) {
        match(Mul);
        expr_type = expr_type + PTR;
    }

    match(')');

    // emit code
    *++text = IMM;
    *++text = (expr_type == CHAR) ? sizeof(char) : sizeof(int);

    expr_type = INT;
}
```

注意的是隻支持 sizeof(int)，sizeof(char) 及 sizeof(pointer type...)。並且它的結果是 int 型。

##變量與函數調用

由於取變量的值與函數的調用都是以 Id 標記開頭的，因此將它們放在一起處理。

```c
else if (token == Id) {
    // there are several type when occurs to Id
    // but this is unit, so it can only be
    // 1. function call
    // 2. Enum variable
    // 3. global/local variable
    match(Id);

    id = current_id;

    if (token == '(') {
        // function call
        match('(');

        // ①
        // pass in arguments
        tmp = 0; // number of arguments
        while (token != ')') {
            expression(Assign);
            *++text = PUSH;
            tmp ++;

            if (token == ',') {
                match(',');
            }
        }
        match(')');

        // ②
        // emit code
        if (id[Class] == Sys) {
            // system functions
            *++text = id[Value];
        }
        else if (id[Class] == Fun) {
            // function call
            *++text = CALL;
            *++text = id[Value];
        }
        else {
            printf("%d: bad function call\n", line);
            exit(-1);
        }

        // ③
        // clean the stack for arguments
        if (tmp > 0) {
            *++text = ADJ;
            *++text = tmp;
        }
        expr_type = id[Type];
    }
    else if (id[Class] == Num) {
        // ④
        // enum variable
        *++text = IMM;
        *++text = id[Value];
        expr_type = INT;
    }
    else {
        // ⑤
        // variable
        if (id[Class] == Loc) {
            *++text = LEA;
            *++text = index_of_bp - id[Value];
        }
        else if (id[Class] == Glo) {
            *++text = IMM;
            *++text = id[Value];
        }
        else {
            printf("%d: undefined variable\n", line);
            exit(-1);
        }

        //⑥
        // emit code, default behaviour is to load the value of the
        // address which is stored in `ax`
        expr_type = id[Type];
        *++text = (expr_type == Char) ? LC : LI;
    }
}
```

①中注意我們是順序將參數入棧，這和第三章：虛擬機中講解的指令是對應的。與之不同，標準 C 是逆序將參數入棧的。

②中判斷函數的類型，同樣在第三章：“虛擬機”中我們介紹過內置函數的支持，如 printf, read, malloc 等等。內置函數有對應的彙編指令，而普通的函數則編譯成 CALL <addr> 的形式。

③用於清除入棧的參數。因為我們不在乎出棧的值，所以直接修改棧指針的大小即可。

④：當該標識符是全局定義的枚舉類型時，直接將對應的值用 IMM 指令存入 AX 即可。

⑤則是用於加載變量的值，如果是局部變量則採用與 bp 指針相對位置的形式（參見第 7章函數定義）。而如果是全局變量則用 IMM 加載變量的地址。

⑥：無論是全局還是局部變量，最終都根據它們的類型用 LC 或 LI 指令加載對應的值。

關於變量，你可能有疑問，如果遇到標識符就用 LC/LI 載入相應的值，那諸如 a[10] 之類的表達式要如何實現呢？後面我們會看到，根據標識符後的運算符，我們可能會修改或刪除現有的 LC/LI 指令。

##強制轉換

雖然我們前面沒有提到，但我們一直用 expr_type 來保存一個表達式的類型，強制轉換的作用是獲取轉換的類型，並直接修改 expr_type 的值。

```c
else if (token == '(') {
    // cast or parenthesis
    match('(');
    if (token == Int || token == Char) {
        tmp = (token == Char) ? CHAR : INT; // cast type
        match(token);
        while (token == Mul) {
            match(Mul);
            tmp = tmp + PTR;
        }

        match(')');

        expression(Inc); // cast has precedence as Inc(++)

        expr_type  = tmp;
    } else {
        // normal parenthesis
        expression(Assign);
        match(')');
    }
}
```

##指針取值

諸如 *a 的指針取值，關鍵是判斷 a 的類型，而就像上節中提到的，當一個表達式解析結束時，它的類型保存在變量 expr_type 中。

```c
else if (token == Mul) {
    // dereference *<addr>
    match(Mul);
    expression(Inc); // dereference has the same precedence as Inc(++)

    if (expr_type >= PTR) {
        expr_type = expr_type - PTR;
    } else {
        printf("%d: bad dereference\n", line);
        exit(-1);
    }

    *++text = (expr_type == CHAR) ? LC : LI;
}
```

##取址操作

這裡我們就能看到“變量與函數調用”一節中所說的修改或刪除 LC/LI 指令了。前文中我們說到，對於變量，我們會先加載它的地址，並根據它們類型使用 LC/LI 指令加載實際內容，例如對變量 a：

```c
IMM <addr>
LI
```

那麼對變量 a 取址，其實只要不執行 LC/LI 即可。因此我們刪除相應的指令。

```c
else if (token == And) {
    // get the address of
    match(And);
    expression(Inc); // get the address of
    if (*text == LC || *text == LI) {
        text --;
    } else {
        printf("%d: bad address of\n", line);
        exit(-1);
    }

    expr_type = expr_type + PTR;
}
```

##邏輯取反

我們沒有直接的邏輯取反指令，因此我們判斷它是否與數字 0 相等。而數字 0 代表了邏輯 “False”。

```c
else if (token == '!') {
    // not
    match('!');
    expression(Inc);

    // emit code, use <expr> == 0
    *++text = PUSH;
    *++text = IMM;
    *++text = 0;
    *++text = EQ;

    expr_type = INT;
}
```

##按位取反

同樣我們沒有相應的指令，所以我們用異或來實現，即 ~a = a ^ 0xFFFF。

```c
else if (token == '~') {
    // bitwise not
    match('~');
    expression(Inc);

    // emit code, use <expr> XOR -1
    *++text = PUSH;
    *++text = IMM;
    *++text = -1;
    *++text = XOR;

    expr_type = INT;
}
```

##正負號

注意這裡並不是四則運算中的加減法，而是單個數字的取正取負操作。同樣，我們沒有取負的操作，用 0 - x 來實現 -x。

```c
else if (token == Add) {
    // +var, do nothing
    match(Add);
    expression(Inc);

    expr_type = INT;
}
else if (token == Sub) {
    // -var
    match(Sub);

    if (token == Num) {
        *++text = IMM;
        *++text = -token_val;
        match(Num);
    } else {

        *++text = IMM;
        *++text = -1;
        *++text = PUSH;
        expression(Inc);
        *++text = MUL;
    }

    expr_type = INT;
}
```

##自增自減

注意的是自增自減操作的優先級是和它的位置有關的。如 ++p 的優先級高於 p++，這裡我們解析的就是類似 ++p 的操作。

```
else if (token == Inc || token == Dec) {
    tmp = token;
    match(token);
    expression(Inc);
    // ①
    if (*text == LC) {
        *text = PUSH;  // to duplicate the address
        *++text = LC;
    } else if (*text == LI) {
        *text = PUSH;
        *++text = LI;
    } else {
        printf("%d: bad lvalue of pre-increment\n", line);
        exit(-1);
    }
    *++text = PUSH;
    *++text = IMM;
    // ②
    *++text = (expr_type > PTR) ? sizeof(int) : sizeof(char);
    *++text = (tmp == Inc) ? ADD : SUB;
    *++text = (expr_type == CHAR) ? SC : SI;
}
```

對應的彙編代碼也比較直觀，只是在實現 ++p時，我們要使用變量 p 的地址兩次，所以我們需要先 PUSH （①）。

②則是因為自增自減操作還需要處理是指針的情形。

##二元運算符

這裡，我們需要處理多運算符的優先級問題，就如前文的“優先級”一節提到的，我們需要不斷地向右掃描，直到遇到優先級 小於 當前優先級的運算符。

回想起我們之前定義過的各個標記，它們是以優先級從低到高排列的，即 Assign 的優先級最低，而 Brak（[） 的優先級最高。

```c
enum {
  Num = 128, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};
```

所以，當我們調用 expression(level) 進行解析的時候，我們其實通過了參數 level 指定了當前的優先級。在前文的一元運算符處理中也用到了這一點。

所以，此時的二元運算符的解析的框架為：

```c
while (token >= level) {
    // parse token for binary operator and postfix operator
}
```

解決了優先級的問題，讓我們繼續講解如何把運算符編譯成彙編代碼吧。

##賦值操作

賦值操作是優先級最低的運算符。考慮諸如 a = (expession) 的表達式，在解析 = 之前，我們已經為變量 a 生成了如下的彙編代碼：
```c
IMM <addr>
LC/LI
```

當解析完=右邊的表達式後，相應的值會存放在 ax 中，此時，為了實際將這個值保存起來，我們需要類似下面的彙編代碼：

```c
IMM <addr>
PUSH
SC/SI
```

明白了這點，也就能理解下面的源代碼了：

```c
tmp = expr_type;
if (token == Assign) {
    // var = expr;
    match(Assign);
    if (*text == LC || *text == LI) {
        *text = PUSH; // save the lvalue's pointer
    } else {
        printf("%d: bad lvalue in assignment\n", line);
        exit(-1);
    }
    expression(Assign);

    expr_type = tmp;
    *++text = (expr_type == CHAR) ? SC : SI;
}
```

## 三目運算符

這是 C 語言中唯一的一個三元運算符： ? :，它相當於一個小型的 If 語句，所以生成的代碼也類似於 If 語句，這裡就不多作解釋。

```c
else if (token == Cond) {
    // expr ? a : b;
    match(Cond);
    *++text = JZ;
    addr = ++text;
    expression(Assign);
    if (token == ':') {
        match(':');
    } else {
        printf("%d: missing colon in conditional\n", line);
        exit(-1);
    }
    *addr = (int)(text + 3);
    *++text = JMP;
    addr = ++text;
    expression(Cond);
    *addr = (int)(text + 1);
}
```

##邏輯運算符

這包括 || 和 &&。它們對應的彙編代碼如下：

```c
<expr1> || <expr2>     <expr1> && <expr2>

  ...<expr1>...          ...<expr1>...
  JNZ b                  JZ b
  ...<expr2>...          ...<expr2>...
b:                     b:
```

所以源碼如下：

```c
else if (token == Lor) {
    // logic or
    match(Lor);
    *++text = JNZ;
    addr = ++text;
    expression(Lan);
    *addr = (int)(text + 1);
    expr_type = INT;
}
else if (token == Lan) {
    // logic and
    match(Lan);
    *++text = JZ;
    addr = ++text;
    expression(Or);
    *addr = (int)(text + 1);
    expr_type = INT;
}
```

##數學運算符

它們包括 |, ^, &, ==, != <=, >=, <, >, <<, >>, +, -, *, /, %。它們的實現都很類似，我們以異或 ^ 為例：

```c
<expr1> ^ <expr2>

...<expr1>...          <- now the result is on ax
PUSH
...<expr2>...          <- now the value of <expr2> is on ax
XOR
```

所以它對應的代碼為：

```c
else if (token == Xor) {
    // bitwise xor
    match(Xor);
    *++text = PUSH;
    expression(And);
    *++text = XOR;
    expr_type = INT;
}
```

其它的我們便不再詳述。但這當中還有一個問題，就是指針的加減。在 C 語言中，指針加上數值等於將指針移位，且根據不同的類型移動的位移不同。如 a + 1，如果 a 是 char * 型，則移動一字節，而如果 a 是 int * 型，則移動 4 個字節（32位系統）。

另外，在作指針減法時，如果是兩個指針相減（相同類型），則結果是兩個指針間隔的元素個數。因此要有特殊的處理。

下面以加法為例，對應的彙編代碼為：

```c
<expr1> + <expr2>

normal         pointer

<expr1>        <expr1>
PUSH           PUSH
<expr2>        <expr2>     |
ADD            PUSH        | <expr2> * <unit>
               IMM <unit>  |
               MUL         |
               ADD
```


即當 <expr1> 是指針時，要根據它的類型放大 <expr2> 的值，因此對應的源碼如下：

```c
else if (token == Add) {
    // add
    match(Add);
    *++text = PUSH;
    expression(Mul);

    expr_type = tmp;
    if (expr_type > PTR) {
        // pointer type, and not `char *`
        *++text = PUSH;
        *++text = IMM;
        *++text = sizeof(int);
        *++text = MUL;
    }
    *++text = ADD;
}
```

相應的減法的代碼就不貼了，可以自己實現看看，也可以看文末給出的鏈接。

## 自增自減

這次是後綴形式的，即 p++ 或 p--。與前綴形式不同的是，在執行自增自減後， ax上需要保留原來的值。所以我們首先執行類似前綴自增自減的操作，再將 ax 中的值執行減/增的操作。

```c
// 前綴形式 生成彙編代碼
*++text = PUSH;
*++text = IMM;
*++text = (expr_type > PTR) ? sizeof(int) : sizeof(char);
*++text = (tmp == Inc) ? ADD : SUB;
*++text = (expr_type == CHAR) ? SC : SI;

// 後綴形式 生成彙編代碼
*++text = PUSH;
*++text = IMM;
*++text = (expr_type > PTR) ? sizeof(int) : sizeof(char);
*++text = (token == Inc) ? ADD : SUB;
*++text = (expr_type == CHAR) ? SC : SI;
*++text = PUSH;                                             //
*++text = IMM;                                              // 執行相反的增/減操作
*++text = (expr_type > PTR) ? sizeof(int) : sizeof(char);   //
*++text = (token == Inc) ? SUB : ADD;                       //
```

## 數組取值操作

在學習 C 語言的時候你可能已經知道了，諸如 a[10] 的操作等價於 *(a + 10)。因此我們要做的就是生成類似的彙編代碼：

```c
else if (token == Brak) {
    // array access var[xx]
    match(Brak);
    *++text = PUSH;
    expression(Assign);
    match(']');

    if (tmp > PTR) {
        // pointer, `not char *`
        *++text = PUSH;
        *++text = IMM;
        *++text = sizeof(int);
        *++text = MUL;
    }
    else if (tmp < PTR) {
        printf("%d: pointer type expected\n", line);
        exit(-1);
    }
    expr_type = tmp - PTR;
    *++text = ADD;
    *++text = (expr_type == CHAR) ? LC : LI;
}
```

##代碼

除了上述對錶達式的解析外，我們還需要初始化虛擬機的棧，我們可以正確調用 main 函數，且當 main 函數結束時退出進程。

```c
int *tmp;
// setup stack
sp = (int *)((int)stack + poolsize);
*--sp = EXIT; // call exit if main returns
*--sp = PUSH; tmp = sp;
*--sp = argc;
*--sp = (int)argv;
*--sp = (int)tmp;
```


當然，最後要注意的一點是：所有的變量定義必須放在語句之前。

本章的代碼可以在 Github 上下載，也可以直接 clone
```c
git clone -b step-6 https://github.com/lotabout/write-a-C-interpreter
```

通過 gcc -o xc-tutor xc-tutor.c 進行編譯。並執行 ./xc-tutor hello.c 查看結果。

正如我們保證的那樣，我們的代碼是自舉的，能自己編譯自己，所以你可以執行 ./xc-tutor xc-tutor.c hello.c。可以看到和之前有同樣的輸出。

##小結

本章我們進行了最後的解析，解析表達式。本章有兩個難點：

- 如何通過遞歸調用 expression 來實現運算符的優先級。
- 如何為每個運算符生成對應的彙編代碼。

儘管代碼看起來比較簡單（雖然多），但其中用到的原理還是需要仔細推敲的。

最後，恭喜你！通過一步步的學習，自己實現了一個C語言的編譯器（好吧，是解釋器）。