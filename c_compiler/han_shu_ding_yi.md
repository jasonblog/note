# 函數定義


由於語法分析本身比較複雜，所以我們將它拆分成 3 個部分進行講解，分別是：變量定義、函數定義、表達式。本章講解函數定義相關的內容。


##EBNF 表示

這是上一章的 EBNF 方法中與函數定義相關的內容。


```c
variable_decl ::= type {'*'} id { ',' {'*'} id } ';'

function_decl ::= type {'*'} id '(' parameter_decl ')' '{' body_decl '}'

parameter_decl ::= type {'*'} id {',' type {'*'} id}

body_decl ::= {variable_decl}, {statement}

statement ::= non_empty_statement | empty_statement

non_empty_statement ::= if_statement | while_statement | '{' statement '}'
                     | 'return' expression | expression ';'

if_statement ::= 'if' '(' expression ')' statement ['else' non_empty_statement]

while_statement ::= 'while' '(' expression ')' non_empty_statement
```

解析函數的定義

上一章的代碼中，我們已經知道了什麼時候開始解析函數的定義，相關的代碼如下：
```c
...
if (token == '(') {
    current_id[Class] = Fun;
    current_id[Value] = (int)(text + 1); // the memory address of function
    function_declaration();
} else {
...
```


即在這斷代碼之前，我們已經為當前的標識符（identifier）設置了正確的類型，上面這斷代碼為當前的標識符設置了正確的類別（Fun），以及該函數在代碼段（text segment）中的位置。接下來開始解析函數定義相關的內容：parameter_decl 及 body_decl。

##函數參數與彙編代碼

現在我們要回憶如何將“函數”轉換成對應的彙編代碼，因為這決定了在解析時我們需要哪些相關的信息。考慮下列函數：

```c
int demo(int param_a, int *param_b) {
    int local_1;
    char local_2;

    ...
}
```

那麼它應該被轉換成什麼樣的彙編代碼呢？在思考這個問題之前，我們需要了解當 demo函數被調用時，計算機的棧的狀態，如下（參照第三章講解的虛擬機）：

```c

|    ....       | high address
+---------------+
| arg: param_a  |    new_bp + 3
+---------------+
| arg: param_b  |    new_bp + 2
+---------------+
|return address |    new_bp + 1
+---------------+
| old BP        | <- new BP
+---------------+
| local_1       |    new_bp - 1
+---------------+
| local_2       |    new_bp - 2
+---------------+
|    ....       |  low address
```



這裡最為重要的一點是，無論是函數的參數（如 param_a）還是函數的局部變量（如 local_1）都是存放在計算機的 棧 上的。因此，與存放在 數據段 中的全局變量不同，在函數內訪問它們是通過 new_bp 指針和對應的位移量進行的。因此，在解析的過程中，我們需要知道參數的個數，各個參數的位移量。

##函數定義的解析

這相當於是整個函數定義的語法解析的框架，代碼如下：

```c
void function_declaration() {
    // type func_name (...) {...}
    //               | this part

    match('(');
    function_parameter();
    match(')');
    match('{');
    function_body();
    //match('}');                 //  ①

    // ②
    // unwind local variable declarations for all local variables.
    current_id = symbols;
    while (current_id[Token]) {
        if (current_id[Class] == Loc) {
            current_id[Class] = current_id[BClass];
            current_id[Type]  = current_id[BType];
            current_id[Value] = current_id[BValue];
        }
        current_id = current_id + IdSize;
    }
}
```

其中①中我們沒有消耗最後的}字符。這麼做的原因是：variable_decl 與 function_decl 是放在一起解析的，而 variable_decl 是以字符 ; 結束的。而 function_decl 是以字符 } 結束的，若在此通過 match 消耗了 ‘;’ 字符，那麼外層的 while 循環就沒法準確地知道函數定義已經結束。所以我們將結束符的解析放在了外層的 while 循環中。

而②中的代碼是用於將符號表中的信息恢復成全局的信息。這是因為，局部變量是可以和全局變量同名的，一旦同名，在函數體內局部變量就會覆蓋全局變量，出了函數體，全局變量就恢復了原先的作用。這段代碼線性地遍歷所有標識符，並將保存在 BXXX 中的信息還原。

##解析參數

```c
parameter_decl ::= type {'*'} id {',' type {'*'} id}
```

解析函數的參數就是解析以逗號分隔的一個個標識符，同時記錄它們的位置與類型。


```c
int index_of_bp; // index of bp pointer on stack

void function_parameter() {
    int type;
    int params;
    params = 0;
    while (token != ')') {
        // ①

        // int name, ...
        type = INT;
        if (token == Int) {
            match(Int);
        } else if (token == Char) {
            type = CHAR;
            match(Char);
        }

        // pointer type
        while (token == Mul) {
            match(Mul);
            type = type + PTR;
        }

        // parameter name
        if (token != Id) {
            printf("%d: bad parameter declaration\n", line);
            exit(-1);
        }
        if (current_id[Class] == Loc) {
            printf("%d: duplicate parameter declaration\n", line);
            exit(-1);
        }

        match(Id);

        //②
        // store the local variable
        current_id[BClass] = current_id[Class]; current_id[Class]  = Loc;
        current_id[BType]  = current_id[Type];  current_id[Type]   = type;
        current_id[BValue] = current_id[Value]; current_id[Value]  = params++;   // index of current parameter

        if (token == ',') {
            match(',');
        }
    }

    // ③
    index_of_bp = params+1;
}
```

其中①與全局變量定義的解析十分一樣，用於解析該參數的類型。

而②則與上節中提到的“局部變量覆蓋全局變量”相關，先將全局變量的信息保存（無論是是否真的在全局中用到了這個變量）在 BXXX 中，再賦上局部變量相關的信息，如 Value 中存放的是參數的位置（是第幾個參數）。

③則與彙編代碼的生成有關，index_of_bp 就是前文提到的 new_bp 的位置。

##函數體的解析

我們實現的 C 語言與現代的 C 語言不太一致，我們需要所有的變量定義出現在所有的語句之前。函數體的代碼如下：

```c
void function_body() {
    // type func_name (...) {...}
    //                   -->|   |<--

    // ... {
    // 1. local declarations
    // 2. statements
    // }

    int pos_local; // position of local variables on the stack.
    int type;
    pos_local = index_of_bp;

    // ①
    while (token == Int || token == Char) {
        // local variable declaration, just like global ones.
        basetype = (token == Int) ? INT : CHAR;
        match(token);

        while (token != ';') {
            type = basetype;
            while (token == Mul) {
                match(Mul);
                type = type + PTR;
            }

            if (token != Id) {
                // invalid declaration
                printf("%d: bad local declaration\n", line);
                exit(-1);
            }
            if (current_id[Class] == Loc) {
                // identifier exists
                printf("%d: duplicate local declaration\n", line);
                exit(-1);
            }
            match(Id);

            // store the local variable
            current_id[BClass] = current_id[Class]; current_id[Class]  = Loc;
            current_id[BType]  = current_id[Type];  current_id[Type]   = type;
            current_id[BValue] = current_id[Value]; current_id[Value]  = ++pos_local;   // index of current parameter

            if (token == ',') {
                match(',');
            }
        }
        match(';');
    }

    // ②
    // save the stack size for local variables
    *++text = ENT;
    *++text = pos_local - index_of_bp;

    // statements
    while (token != '}') {
        statement();
    }

    // emit code for leaving the sub function
    *++text = LEV;
}
```

其中①用於解析函數體內的局部變量的定義，代碼與全局的變量定義幾乎一樣。

而②則用於生成彙編代碼，我們在第三章的虛擬機中提到過，我們需要在棧上為局部變量預留空間，這兩行代碼起的就是這個作用。

##代碼

本章的代碼可以在 Github 上下載，也可以直接 clone

```c
git clone -b step-4 https://github.com/lotabout/write-a-C-interpreter
```

本章的代碼依舊無法運行，還有兩個重要函數沒有完成：statement 及 expression，感興趣的話可以嘗試自己實現它們。

## 小結

本章中我們用了不多的代碼完成了函數定義的解析。大部分的代碼依舊是用於解析變量：參數和局部變量，而它們的邏輯和全局變量的解析幾乎一致，最大的區別就是保存的信息不同。

當然，要理解函數定義的解析過程，最重要的是理解我們會為函數生成怎樣的彙編代碼，因為這決定了我們需要從解析中獲取什麼樣的信息（例如參數的位置，個數等），而這些可能需要你重新回顧一下“虛擬機”這一章，或是重新學習學習彙編相關的知識。