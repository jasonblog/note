# 詞法分析器

##什麼是詞法分析器

簡而言之，詞法分析器用於對源碼字符串做預處理，以減少語法分析器的複雜程度。

詞法分析器以源碼字符串為輸入，輸出為標記流（token stream），即一連串的標記，每個標記通常包括： (token, token value) 即標記本身和標記的值。例如，源碼中若包含一個數字 '998' ，詞法分析器將輸出 (Number, 998)，即（數字，998）。再例如：
```c
2 + 3 * (4 - 5)
=>
(Number, 2) Add (Number, 3) Multiply Left-Bracket (Number, 4) Subtract (Number, 5) Right-Bracket

```

通過詞法分析器的預處理，語法分析器的複雜度會大大降低，這點在後面的語法分析器我們就能體會。

## 詞法分析器與編譯器

要是深入詞法分析器，你就會發現，它的本質上也是編譯器。我們的編譯器是以標記流為輸入，輸出彙編代碼，而詞法分析器則是以源碼字符串為輸入，輸出標記流。

```c

                   +-------+                      +--------+
-- source code --> | lexer | --> token stream --> | parser | --> assembly
                   +-------+                      +--------+
```

在這個前提下，我們可以這樣認為：直接從源代碼編譯成彙編代碼是很困難的，因為輸入的字符串比較難處理。所以我們先編寫一個較為簡單的編譯器（詞法分析器）來將字符串轉換成標記流，而標記流對於語法分析器而言就容易處理得多了。


## 詞法分析器的實現

由於詞法分析的工作很常見，但又枯燥且容易出錯，所以人們已經開發出了許多工具來生成詞法分析器，如 lex, flex。這些工具允許我們通過正則表達式來識別標記。

這裡注意的是，我們並不會一次性地將所有源碼全部轉換成標記流，原因有二：

字符串轉換成標記流有時是有狀態的，即與代碼的上下文是有關係的。
保存所有的標記流沒有意義且浪費空間。
所以實際的處理方法是提供一個函數（即前幾篇中提到的 next()），每次調用該函數則返回下一個標記。

## 支持的標記

在全局中添加如下定義：

```c
// tokens and classes (operators last and in precedence order)
enum {
  Num = 128, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};
```

這些就是我們要支持的標記符。例如，我們會將 = 解析為 Assign；將 == 解析為 Eq；將 != 解析為 Ne 等等。

所以這裡我們會有這樣的印象，一個標記（token）可能包含多個字符，且多數情況下如此。而詞法分析器能減小語法分析複雜度的原因，正是因為它相當於通過一定的編碼（更多的標記）來壓縮了源碼字符串。

當然，上面這些標記是有順序的，跟它們在 C 語言中的優先級有關，如 *(Mul) 的優先級就要高於 +(Add)。它們的具體使用在後面的語法分析中會提到。

最後要注意的是還有一些字符，它們自己就構成了標記，如右方括號 ] 或波浪號 ~ 等。我們不另外處理它們的原因是：

- 它們是單字符的，即並不是多個字符共同構成標記（如 == 需要兩個字符）；
- 它們不涉及優先級關係。



## 詞法分析器的框架

即 next() 函數的主體：

```c
void next() {
    char *last_pos;
    int hash;

    while (token = *src) {
        ++src;
        // parse token here
    }
    return;
}
```


這裡的一個問題是，為什麼要用 while 循環呢？這就涉及到編譯器（記得我們說過詞法分析器也是某種意義上的編譯器）的一個問題：如何處理錯誤？

對詞法分析器而言，若碰到了一個我們不認識的字符該怎麼處理？一般處理的方法有兩種：

- 指出錯誤發生的位置，並退出整個程序
- 指出錯誤發生的位置，跳過當前錯誤並繼續編譯

這個 while 循環的作用就是跳過這些我們不識別的字符，我們同時還用它來處理空白字符。我們知道，C 語言中空格是用來作為分隔用的，並不作為語法的一部分。因此在實現中我們將它作為「不識別」的字符，這個 while 循環可以用來跳過它。


## 換行符

換行符和空格類似，但有一點不同，每次遇到換行符，我們需要將當前的行號加一：

```c
// parse token here
...

if (token == '\n') {
    ++line;
}
...
```


## 宏定義

C 語言的宏定義以字符 # 開頭，如 # include <stdio.h>。我們的編譯器並不支持宏定義，所以直接跳過它們。

```c
else if (token == '#') {
    // skip macro, because we will not support it
    while (*src != 0 && *src != '\n') {
        src++;
    }
}

```

## 標識符與符號表

標識符（identifier）可以理解為變量名。對於語法分析而言，我們並不關心一個變量具體叫什麼名字，而只關心這個變量名代表的唯一標識。例如 int a; 定義了變量 a，而之後的語句 a = 10，我們需要知道這兩個 a 指向的是同一個變量。

基於這個理由，詞法分析器會把掃瞄到的標識符全都保存到一張表中，遇到新的標識符就去查這張表，如果標識符已經存在，就返回它的唯一標識。

那麼我們怎麼表示標識符呢？如下：

```c
struct identifier {
    int token;
    int hash;
    char * name;
    int class;
    int type;
    int value;
    int Bclass;
    int Btype;
    int Bvalue;
}
```

這裡解釋一下具體的含義：

- token：該標識符返回的標記，理論上所有的變量返回的標記都應該是 Id，但實際上由於我們還將在符號表中加入關鍵字如 if, while 等，它們都有對應的標記。
- hash：顧名思義，就是這個標識符的哈希值，用於標識符的快速比較。
- name：存放標識符本身的字符串。
- class：該標識符的類別，如數字，全局變量或局部變量等。
- type：標識符的類型，即如果它是個變量，變量是 int 型、char 型還是指針型。
- value：存放這個標識符的值，如標識符是函數，剛存放函數的地址。
- BXXXX：C 語言中標識符可以是全局的也可以是局部的，當局部標識符的名字與全局標識符相同時，用作保存全局標識符的信息。


由上可以看出，我們實現的詞法分析器與傳統意義上的詞法分析器不太相同。傳統意義上的符號表只需要知道標識符的唯一標識即可，而我們還存放了一些只有語法分析器才會得到的信息，如 type 。

由於我們的目標是能自舉，而我們定義的語法不支持 struct，故而使用下列方式。

```c
Symbol table:
----+-----+----+----+----+-----+-----+-----+------+------+----
 .. |token|hash|name|type|class|value|btype|bclass|bvalue| ..
----+-----+----+----+----+-----+-----+-----+------+------+----
    |<---       one single identifier                --->|

```


即用一個整型數組來保存相關的ID信息。每個ID佔用數組中的9個空間，分析標識符的相關代碼如下：

```c
int token_val;                // value of current token (mainly for number)
int *current_id,              // current parsed ID
    *symbols;                 // symbol table

// fields of identifier
enum {Token, Hash, Name, Type, Class, Value, BType, BClass, BValue, IdSize};


void next() {
        ...

        else if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || (token == '_')) {

            // parse identifier
            last_pos = src - 1;
            hash = token;

            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9') || (*src == '_')) {
                hash = hash * 147 + *src;
                src++;
            }

            // look for existing identifier, linear search
            current_id = symbols;
            while (current_id[Token]) {
                if (current_id[Hash] == hash && !memcmp((char *)current_id[Name], last_pos, src - last_pos)) {
                    //found one, return
                    token = current_id[Token];
                    return;
                }
                current_id = current_id + IdSize;
            }


            // store new ID
            current_id[Name] = (int)last_pos;
            current_id[Hash] = hash;
            token = current_id[Token] = Id;
            return;
        }
        ...
}
```
查找已有標識符的方法是線性查找 symbols 表。

## 數字

數字中較為複雜的一點是需要支持十進制、十六進制及八進制。邏輯也較為直接，可能唯一不好理解的是獲取十六進制的值相關的代碼。

```c
token_val = token_val * 16 + (token & 15) + (token >= 'A' ? 9 : 0);
```

這裡要注意的是在ASCII碼中，字符a對應的十六進制值是 61, A是41，故通過 (token & 15) 可以得到個位數的值。其它就不多說了，這裡這樣寫的目的是裝B（其實是抄 c4 的源代碼的）。


```c
void next() {
        ...


        else if (token >= '0' && token <= '9') {
            // parse number, three kinds: dec(123) hex(0x123) oct(017)
            token_val = token - '0';
            if (token_val > 0) {
                // dec, starts with [1-9]
                while (*src >= '0' && *src <= '9') {
                    token_val = token_val*10 + *src++ - '0';
                }
            } else {
                // starts with number 0
                if (*src == 'x' || *src == 'X') {
                    //hex
                    token = *++src;
                    while ((token >= '0' && token <= '9') || (token >= 'a' && token <= 'f') || (token >= 'A' && token <= 'F')) {
                        token_val = token_val * 16 + (token & 15) + (token >= 'A' ? 9 : 0);
                        token = *++src;
                    }
                } else {
                    // oct
                    while (*src >= '0' && *src <= '7') {
                        token_val = token_val*8 + *src++ - '0';
                    }
                }
            }

            token = Num;
            return;
        }

        ...
}
```

##字符串

在分析時，如果分析到字符串，我們需要將它存放到前一篇文章中說的 data 段中。然後返回它在 data 段中的地址。另一個特殊的地方是我們需要支持轉義符。例如用 \n 表示換行符。由於本編譯器的目的是達到自己編譯自己，所以代碼中並沒有支持除 \n 的轉義符，如 \t, \r 等，但仍支持 \a 表示字符 a 的語法，如 \" 表示 "。

在分析時，我們將同時分析單個字符如 'a' 和字符串如 "a string"。若得到的是單個字符，我們以 Num 的形式返回。相關代碼如下：


```c
void next() {
        ...

        else if (token == '"' || token == '\'') {
            // parse string literal, currently, the only supported escape
            // character is '\n', store the string literal into data.
            last_pos = data;
            while (*src != 0 && *src != token) {
                token_val = *src++;
                if (token_val == '\\') {
                    // escape character
                    token_val = *src++;
                    if (token_val == 'n') {
                        token_val = '\n';
                    }
                }

                if (token == '"') {
                    *data++ = token_val;
                }
            }

            src++;
            // if it is a single character, return Num token
            if (token == '"') {
                token_val = (int)last_pos;
            } else {
                token = Num;
            }

            return;
        }
}
```

## 註釋

在我們的 C 語言中，只支持 // 類型的註釋，不支持 /* comments */ 的註釋。
```c
void next() {
        ...

        else if (token == '/') {
            if (*src == '/') {
                // skip comments
                while (*src != 0 && *src != '\n') {
                    ++src;
                }
            } else {
                // divide operator
                token = Div;
                return;
            }
        }

        ...
}
```

這裡我們要額外介紹 lookahead 的概念，即提前看多個字符。上述代碼中我們看到，除了跳過註釋，我們還可能返回除號 /(Div) 標記。

提前看字符的原理是：有一個或多個標記是以同樣的字符開頭的（如本小節中的註釋與除號），因此只憑當前的字符我們並無法確定具體應該解釋成哪一個標記，所以只能再向前查看字符，如本例需向前查看一個字符，若是 / 則說明是註釋，反之則是除號。

我們之前說過，詞法分析器本質上也是編譯器，其實提前看字符的概念也存在於編譯器，只是這時就是提前看k個「標記」而不是「字符」了。平時聽到的 LL(k) 中的 k 就是需要向前看的標記的個數了。

另外，我們用詞法分析器將源碼轉換成標記流，能減小語法分析複雜度，原因之一就是減少了語法分析器需要「向前看」的字符個數。


##其它

其它的標記的解析就相對容易一些了，我們直接貼上代碼：


```c
void next() {
        ...

        else if (token == '=') {
            // parse '==' and '='
            if (*src == '=') {
                src ++;
                token = Eq;
            } else {
                token = Assign;
            }
            return;
        }
        else if (token == '+') {
            // parse '+' and '++'
            if (*src == '+') {
                src ++;
                token = Inc;
            } else {
                token = Add;
            }
            return;
        }
        else if (token == '-') {
            // parse '-' and '--'
            if (*src == '-') {
                src ++;
                token = Dec;
            } else {
                token = Sub;
            }
            return;
        }
        else if (token == '!') {
            // parse '!='
            if (*src == '=') {
                src++;
                token = Ne;
            }
            return;
        }
        else if (token == '<') {
            // parse '<=', '<<' or '<'
            if (*src == '=') {
                src ++;
                token = Le;
            } else if (*src == '<') {
                src ++;
                token = Shl;
            } else {
                token = Lt;
            }
            return;
        }
        else if (token == '>') {
            // parse '>=', '>>' or '>'
            if (*src == '=') {
                src ++;
                token = Ge;
            } else if (*src == '>') {
                src ++;
                token = Shr;
            } else {
                token = Gt;
            }
            return;
        }
        else if (token == '|') {
            // parse '|' or '||'
            if (*src == '|') {
                src ++;
                token = Lor;
            } else {
                token = Or;
            }
            return;
        }
        else if (token == '&') {
            // parse '&' and '&&'
            if (*src == '&') {
                src ++;
                token = Lan;
            } else {
                token = And;
            }
            return;
        }
        else if (token == '^') {
            token = Xor;
            return;
        }
        else if (token == '%') {
            token = Mod;
            return;
        }
        else if (token == '*') {
            token = Mul;
            return;
        }
        else if (token == '[') {
            token = Brak;
            return;
        }
        else if (token == '?') {
            token = Cond;
            return;
        }
        else if (token == '~' || token == ';' || token == '{' || token == '}' || token == '(' || token == ')' || token == ']' || token == ',' || token == ':') {
            // directly return the character as token;
            return;
        }

        ...
}
```

代碼較多，但主要邏輯就是向前看一個字符來確定真正的標記。


## 關鍵字與內置函數

雖然上面寫完了詞法分析器，但還有一個問題需要考慮，那就是「關鍵字」，例如 if, while, return 等。它們不能被作為普通的標識符，因為有特殊的含義。

一般有兩種處理方法：

- 詞法分析器中直接解析這些關鍵字。
- 在語法分析前將關鍵字提前加入符號表。

這裡我們就採用第二種方法，將它們加入符號表，並提前為它們賦予必要的信息（還記得前面說的標識符 Token 字段嗎？）。這樣當源代碼中出現關鍵字時，它們會被解析成標識符，但由於符號表中已經有了相關的信息，我們就能知道它們是特殊的關鍵字。

內置函數的行為也和關鍵字類似，不同的只是賦值的信息，在main函數中進行初始化如下：


```c
// types of variable/function
enum { CHAR, INT, PTR };
int *idmain;                  // the `main` function

void main() {
    ...

    src = "char else enum if int return sizeof while "
          "open read close printf malloc memset memcmp exit void main";

     // add keywords to symbol table
    i = Char;
    while (i <= While) {
        next();
        current_id[Token] = i++;
    }

    // add library to symbol table
    i = OPEN;
    while (i <= EXIT) {
        next();
        current_id[Class] = Sys;
        current_id[Type] = INT;
        current_id[Value] = i++;
    }

    next(); current_id[Token] = Char; // handle void type
    next(); idmain = current_id; // keep track of main

    ...
    program();
}
```

##代碼

本章的代碼可以在 Github 上下載，也可以直接 clone

```c
git clone -b step-2 https://github.com/lotabout/write-a-C-interpreter
```

上面的代碼運行後會出現 『Segmentation Falt'，這是正常的，因為它會嘗試運行我們上一章創建的虛擬機，但其中並沒有任何彙編代碼。

小結

本章我們為我們的編譯器構建了詞法分析器，通過本章的學習，我認為有幾個要點需要強調：

- 詞法分析器的作用是對源碼字符串進行預處理，作用是減小語法分析器的複雜程度。
- 詞法分析器本身可以認為是一個編譯器，輸入是源碼，輸出是標記流。
- lookahead(k) 的概念，即向前看 k 個字符或標記。
- 詞法分析中如何處理標識符與符號表。

下一章中，我們將介紹遞歸下降的語法分析器。我們下一章見。