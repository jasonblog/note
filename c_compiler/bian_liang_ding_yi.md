# 變量定義


本章中我們用 EBNF 來大致描述我們實現的 C 語言的文法，並實現其中解析變量定義部分。

由於語法分析本身比較複雜，所以我們將它拆分成 3 個部分進行講解，分別是：變量定義、函數定義、表達式。

##EBNF 表示

EBNF 是對前一章提到的 BNF 的擴展，它的語法更容易理解，實現起來也更直觀。但真正看起來還是很煩，如果不想看可以跳過。
```c
program ::= {global_declaration}+

global_declaration ::= enum_decl | variable_decl | function_decl

enum_decl ::= 'enum' [id] '{' id ['=' 'num'] {',' id ['=' 'num'} '}'

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


其中 expression 相關的內容我們放到後面解釋，主要原因是我們的語言不支持跨函數遞歸，而為了實現自舉，實際上我們也不能使用遞歸（虧我們說了一章的遞歸下降）。

P.S. 我是先寫程序再總結上面的文法，所以實際上它們間的對應關係並不是特別明顯。

##解析變量的定義

本章要講解的就是上節文法中的 enum_decl 和 variable_decl 部分。

program()

首先是之前定義過的 program 函數，將它改成：

```c
void program() {
    // get next token
    next();
    while (token > 0) {
        global_declaration();
    }
}
```

我知道 global_declaration 函數還沒有出現過，但沒有關係，採用自頂向下的編寫方法就是要不斷地實現我們需要的內容。下面是 global_declaration 函數的內容：

### global_declaration()

即全局的定義語句，包括變量定義，類型定義（只支持枚舉）及函數定義。代碼如下：

```c
int basetype;    // the type of a declaration, make it global for convenience
int expr_type;   // the type of an expression

void global_declaration() {
    // global_declaration ::= enum_decl | variable_decl | function_decl
    //
    // enum_decl ::= 'enum' [id] '{' id ['=' 'num'] {',' id ['=' 'num'} '}'
    //
    // variable_decl ::= type {'*'} id { ',' {'*'} id } ';'
    //
    // function_decl ::= type {'*'} id '(' parameter_decl ')' '{' body_decl '}'


    int type; // tmp, actual type for variable
    int i; // tmp

    basetype = INT;

    // parse enum, this should be treated alone.
    if (token == Enum) {
        // enum [id] { a = 10, b = 20, ... }
        match(Enum);
        if (token != '{') {
            match(Id); // skip the [id] part
        }
        if (token == '{') {
            // parse the assign part
            match('{');
            enum_declaration();
            match('}');
        }

        match(';');
        return;
    }

    // parse type information
    if (token == Int) {
        match(Int);
    }
    else if (token == Char) {
        match(Char);
        basetype = CHAR;
    }

    // parse the comma seperated variable declaration.
    while (token != ';' && token != '}') {
        type = basetype;
        // parse pointer type, note that there may exist `int ****x;`
        while (token == Mul) {
            match(Mul);
            type = type + PTR;
        }

        if (token != Id) {
            // invalid declaration
            printf("%d: bad global declaration\n", line);
            exit(-1);
        }
        if (current_id[Class]) {
            // identifier exists
            printf("%d: duplicate global declaration\n", line);
            exit(-1);
        }
        match(Id);
        current_id[Type] = type;

        if (token == '(') {
            current_id[Class] = Fun;
            current_id[Value] = (int)(text + 1); // the memory address of function
            function_declaration();
        } else {
            // variable declaration
            current_id[Class] = Glo; // global variable
            current_id[Value] = (int)data; // assign memory address
            data = data + sizeof(int);
        }

        if (token == ',') {
            match(',');
        }
    }
    next();
}
```

看了上面的代碼，能大概理解嗎？這裡我們講解其中的一些細節。

`向前看標記` ：其中的 if (token == xxx) 語句就是用來向前查看標記以確定使用哪一個產生式，例如只要遇到 enum 我們就知道是需要解析枚舉類型。而如果只解析到類型，如 int identifier 時我們並不能確定 identifier 是一個普通的變量還是一個函數，所以還需要繼續查看後續的標記，如果遇到 ( 則可以斷定是函數了，反之則是變量。

`變量類型的表示` ：我們的編譯器支持指針類型，那意味著也支持指針的指針，如 int **data;。那麼我們如何表示指針類型呢？前文中我們定義了支持的類型：

```c
// types of variable/function
enum { CHAR, INT, PTR };
```

所以一個類型首先有基本類型，如 CHAR 或 INT，當它是一個指向基本類型的指針時，如 int *data，我們就將它的類型加上 PTR 即代碼中的：type = type + PTR;。同理，如果是指針的指針，則再加上 PTR。

enum_declaration()

用於解析枚舉類型的定義。主要的邏輯用於解析用逗號（,）分隔的變量，值得注意的是在編譯器中如何保存枚舉變量的信息。

即我們將該變量的類別設置成了 Num，這樣它就成了全局的常量了，而注意到上節中，正常的全局變量的類別則是 Glo，類別信息在後面章節中解析 expression 會使用到。

```c
void enum_declaration() {
    // parse enum [id] { a = 1, b = 3, ...}
    int i;
    i = 0;
    while (token != '}') {
        if (token != Id) {
            printf("%d: bad enum identifier %d\n", line, token);
            exit(-1);
        }
        next();
        if (token == Assign) {
            // like {a=10}
            next();
            if (token != Num) {
                printf("%d: bad enum initializer\n", line);
                exit(-1);
            }
            i = token_val;
            next();
        }

        current_id[Class] = Num;
        current_id[Type] = INT;
        current_id[Value] = i++;

        if (token == ',') {
            next();
        }
    }
}
```

##其它

其中的 function_declaration 函數我們將放到下一章中講解。match 函數是一個輔助函數：

```c
void match(int tk) {
    if (token == tk) {
        next();
    } else {
        printf("%d: expected token: %d\n", line, tk);
        exit(-1);
    }
}
```


它將 next 函數包裝起來，如果不是預期的標記則報錯並退出。

## 代碼

本章的代碼可以在 Github 上下載，也可以直接 clone

```c
git clone -b step-3 https://github.com/lotabout/write-a-C-interpreter
```

本章的代碼還無法正常運行，因為還有許多功能沒有實現，但如果有興趣的話，可以自己先試著去實現它。

##小結

本章的內容應該不難，除了開頭的 EBNF 表達式可能相對不好理解一些，但如果你查看了 EBNF 的具體表示方法後就不難理解了。

剩下的內容就是按部就班地將 EBNF 的產生式轉換成函數的過程，如果你理解了上一章中的內容，相信這部分也不難理解。

下一章中我們將介紹如何解析函數的定義，敬請期待。