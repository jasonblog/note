---
title: MySQL 語法解析
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,lex,bison,yacc,flex,詞法,語法
description: 當服務器接收到一條 SQL 語句時，其處理過程為：詞法分析，語法分析，語義分析，構造執行樹，生成執行計劃，計劃的執行。其中，詞法語法解析的處理過程根編譯原理上的東西基本類似；MySQL 並沒有使用 lex 來實現詞法分析，但是語法分析卻用了 yacc。與之對比的 SQLite 數據庫，其詞法分析器是手工寫的，語法分析器由 Lemon 生成。在此介紹其在 MySQL 中的使用。
---

當服務器接收到一條 SQL 語句時，其處理過程為：詞法分析，語法分析，語義分析，構造執行樹，生成執行計劃，計劃的執行。

其中，詞法語法解析的處理過程根編譯原理上的東西基本類似；MySQL 並沒有使用 lex 來實現詞法分析，但是語法分析卻用了 yacc。

與之對比的 SQLite 數據庫，其詞法分析器是手工寫的，語法分析器由 Lemon 生成。

在此介紹其在 MySQL 中的使用。

<!-- more -->

## 簡介

Lex-Yacc (詞法掃描器-語法分析器) 在 Linux 上就是 flex-bison；使用 bison 時，採用的語法必須是上下文無關文法 (context-free grammar)；可以通過 ```yum install flex flex-devel bison``` 進行安裝。

首先，介紹下基本的概念。

### BNF

也就是 Backus-Naur Form 巴科斯範式，由 John Backus 和 Peter Naur 首先引入的，用來描述計算機語言語法的符號集，現在，幾乎每一位新編程語言書籍的作者都使用巴科斯範式來定義編程語言的語法規則。

其推到規則通過 ```::=``` 定義，左側為非終結符，右側為一個表達式；表達式由一個符號序列，或用 ```'|'``` 分隔的多個符號序列構成，從未在左端出現的符號叫做終結符。

{% highlight text %}
"..."  : 術語符號，表示字符本身，用double_quote用來代表雙引號
< >    : 必選項
[ ]    : 可選項，最多出現一次
{ }    : 可重複0至無數次的項
|      : 左右兩邊任選一項，相當於OR
::=    : 被定義為
{% endhighlight %}

如下是 Java 中的 For 語句實例：

{% highlight text %}
FOR_STATEMENT ::=
"for" "(" ( variable_declaration |
( expression ";" ) | ";" )
[ expression ] ";"
[ expression ] ";"
")" statement
{% endhighlight %}

其中 RFC2234 定義了擴展的巴科斯範式 (ABNF)。

### 上下文無關文法

簡單來說就是每個產生式的左邊只有一個非終結符；首先，試著用漢語來稍微解釋一下。

{% highlight text %}
本來這個進球就是違例的，但你不肯承認也沒辦法
我有一本來自美國的花花公子雜誌
拿我的筆記本來
{% endhighlight %}

如果漢語是上下文無關文法，那麼我們任何時候看見 ```"本來"``` 兩個字，都可以把它規約為一個詞；可惜漢語不是上下文無關文法，所以能否歸約為一個詞，要看它的上下文是什麼。如上的示例中，只有第一句可以規約為一個詞。

<!--
上下文無關文法就是說這個文法中所有的產生式左邊只有一個非終結符，比如：
S -> aSb
S -> ab
這個文法有兩個產生式，每個產生式左邊只有一個非終結符S，這就是上下文無關文法，因為你只要找到符合產生式右邊的串，就可以把它歸約為對應的非終結符。

比如：
aSb -> aaSbb
S -> ab
這就是上下文相關文法，因為它的第一個產生式左邊有不止一個符號，所以你在匹配這個產生式中的S的時候必需確保這個S有正確的“上下文”，也就是左邊的a和右邊的b，所以叫上下文相關文法。




其中，生成式由終結符和非終結符組成，分別用大寫、小寫表示；如果終結字符是一個單字符，那麼可以直接使用該字符，如;、)、[。對於語義來說，如整型 INTEGER 的值可以是 1、34、5555。當匹配一個語義時可以直接執行一個動作。

解析器相對於狀態機多了一個棧，從而可以處理移進和規約。簡單的說，LR(1) 就是指，只需要預讀一個 token 那麼就可以知道如何解析字符串中的任意一部分。雖然 bison 可以適用於幾乎所有的上下文無關語法，不過其針對 LR(1) 做了專門的優化。

在一些確定性的 LR(1) 語法中，仍然會存在這歧義，可能不知道對那個語法規則執行規約，或者不知道是執行規約還是移進，分別被稱為 規約/規約衝突 或者 移進/規約衝突。

關於上下文相關、無關語法可以參考 WiKi <a href="https://en.wikipedia.org/wiki/Context-free_grammar">Context-free grammar</a>、<a href="https://en.wikipedia.org/wiki/Context-sensitive_grammar">Context-sensitive grammar</a>，以及 <a href="http://cs.union.edu/~striegnk/courses/nlp-with-prolog/html/node37.html">Context Free Grammars</a>，也可以參考本地文檔。
-->

## Lex 詞法分析

Flex 採用的是狀態機，通過分析輸入流 (字符流)，只要發現一段字符能夠匹配一個關鍵字 (正則表達式)，就會採取對應的動作。

Flex 文件被 ```%%``` 分成了上中下三個部分：

1. 第一部分中要寫入 C/C++ 代碼必須用 ```%{``` 和 ```%}``` 括起來，將原封不動放到生成源碼中。

2. 第二部分是規則段，包括了模式 (正則表達式) 和動作，由空白分開，當匹配到模式時，就會執行後面的動作。

3. 第三部分可以直接寫入 C/C++ 代碼。

yylex() 是掃描程序的入口，調用該函數啟動或重新開始，該函數會初始化一些全局變量，然後開始掃描。如果定義的 flex 動作是將數值傳遞給調用程序 (return)，那麼對 yylex() 的下次調用就從它停止的地方繼續掃描。

{% highlight text %}
%{
#include <stdio.h>
%}
%%
is |
are                     { printf("%s: VERB ", yytext); }
island                  printf("LAND ");
[0-9]+                  printf("NUMBER ");
[ \t]+                  /* ignore whitespace */;
[a-zA-Z][a-zA-Z0-9]*    printf("COMMON WORD ");
.|\n                    { ECHO; }
%%
int main(void) {
    yylex();
}
{% endhighlight %}

特殊字符 ```'.'``` 表示匹配換行符以外的任意單個字符，```'\n'``` 匹配換行符；```ECHO``` 表示輸出匹配的模式，默認行為為，也就是將輸入原樣輸出：

{% highlight c %}
#define ECHO fwrite(yytext, yyleng, 1, yyout)
{% endhighlight %}

默認將 stdin 作為輸入，可以通過如下命令測試。

{% highlight text %}
----- 首先按照規則生成C源碼lex.yy.c
$ flex example.l

----- 然後進行編譯
$ cc lex.yy.c -o example -lfl

----- 直接執行測試，Ctrl-D退出
$ ./example
is
is: VERB
are
are: VERB
island
LAND
89
NUMBER
foobar
COMMON WORD
^*&
^*&
{% endhighlight %}

在處理時，flex 採用兩個原則：A) 只匹配一次；B) 執行當前輸入的最長可能匹配值。也就是對與 island 不會匹配 is 和 land 。當然，我們可以使用一個文件作為關鍵字列表，而非每次都需要編譯。

解析時會通過 yyin 讀取，如果需要在 yacc 或者其它文件中設置，那麼可以通過如下方式修改。

{% highlight c %}
extern FILE *yyin;
yyin = fopen("filename","r");
{% endhighlight %}

通過 flex 處理文件後，會將匹配轉化為指定的符號，然後供 yacc 處理。

### 常用功能

簡單列舉常用函數。


#### 正則表達式

flex 常用正則表達式：

1. 格式與 grep 相似；
2. ```<<EOF>>``` 標示文件結束；
3. 常用字符集，如 ```[:alpha:]```, ```[:digit:]```, ```[:alnum:]```, ```[:space:]``` 等；
4. ```{name}``` 使用預定義的 name 。

簡單示例，計算平均值。

{% highlight text %}
%{
#include <stdio.h>
#include <stdlib.h>
%}
dgt    [0-9] // 通過name方式定義
%%
{dgt}+   return atoi(yytext);
%%
void main()
{
   int val, total = 0, n = 0;
   while ( (val = yylex()) > 0 ) { // 到文件結束時返回0
      total += val;
      n++;
   }
   if (n > 0) printf(“ave = %d\n”, total/n);
}
{% endhighlight %}

如上如果在編譯時使用 ```-Wall``` 參數，會報 ```warning: `yyunput’ defined but not used``` 之類的異常，如下介紹可以通過如下選項關閉。

{% highlight text %}
%option nounput
%option noinput
{% endhighlight %}

#### 多規則匹配

如果有多個值匹配，那麼 flex 會按照如下的規則選取。

1. 貪婪匹配，選擇最大的匹配值；
2. 多個規則匹配，選擇第一個；
3. 沒有規則匹配則會選擇默認規則。

例如，通過 ```"/*"(.|\n)*"*/"``` 規則匹配 C 語言中的註釋，那麼如下場景可能出錯。

{% highlight c %}
#include <stdio.h>  /* definitions */
int main(int argc, char * argv[ ]) {
   if (argc <= 1) {
       printf("Error!\n");  /* no arguments */
   }
   printf("%d args given\n", argc);
   return 0;
}
{% endhighlight %}

貪婪匹配，會從 ```/* def``` 到 ```nts */``` 之間的內容都作為註釋，此時就需要使用條件 (Condition) 規則。例如，以 ```<S>``` 開始的規則，只有在條件 S 時才會進行匹配，可以在 definition section 段通過如下方式定義條件。

{% highlight text %}
%x S exclusive start conditions
%s S inclusive start conditions
{% endhighlight %}

然後，通過 ```BEGIN(S)``` 進入條件，另外，flex 有個初始條件，可以通過 ```BEGIN(INITIAL)``` 返回；如果使用多個狀態，那麼實際上可以實現一個狀態機，詳見 [lex tutorial.ppt](https://www2.cs.arizona.edu/~debray/Teaching/CSc453/DOCS/lex tutorial.ppt) 或者 [本地文檔](/reference/databases/mysql/lex_tutorial.ppt) 。

關於上述內容，也可以參考 [Start conditions](http://dinosaur.compilertools.net/flex/flex_11.html) 中的介紹。


#### yyterminate()

可在一個動作中代替 return 使用，用於結束掃描並向掃描器的調用者返回 0；可以通過如下方式自定義。

{% highlight c %}
#ifdef yyterminate
# undef yyterminate
#endif
#define yyterminate() \
    do { free (foobar); foobar = NULL; pos = 0; len = 0; \
        return YY_NULL; } while (0)
{% endhighlight %}

#### 配置選項

{% highlight text %}
%option yylineno    提供當前的行信息，通常用於後續打印錯誤行信息
%option noyywrap    不生成yywrap()聲明
%option noinput     會生成#define YY_NO_INPUT 1定義
%option nounput     會生成#define YY_NO_UNPUT 1定義
{% endhighlight %}

flex 會聲明一個 ```int yywarp(void);``` 函數，但是不會自動定義，所以通常會在最後的 section 實現該函數。該函數的作用是將多個輸入文件打包成一個輸入，也就是當 ```yylex()``` 讀取到一個文件結束 (EOF) 時，會調用 ```yywrap()``` ，如果返回 1 則表示後面沒有其它輸入文件了，此時 ```yylex()``` 函數結束；當然，```yywrap()``` 也可以打開下一個輸入文件，再向 ```yylex()``` 函數返回 0 ，告訴它後面還有別的輸入文件。

如果只有一個文件，那麼可以通過 ```%option noyywrap``` 不聲明該函數，也就不需要再實現。


### 其它


#### 值傳遞

在通過 flex 進行掃描時，會將值保存在 yylval 變量中，而 bison 則讀取 yylval 中的值，該變量默認是 int 類型，如果要使用字符串類型，那麼可以在 .l+.y 的頭部第一句加入 ```#define YYSTYPE char*``` 即可。

{% highlight text %}
// 在.l賦值的時候，要特別注意，需要拷貝字符串
yylval = strdup(yytext);  return WORD;
// 在.y取用的時候，直接強轉就可以了
(char*)$1
{% endhighlight %}

關於更優雅的實現方式，當然是用 union 啦，仿照上面，很容易寫出來的。


#### 標準格式

{% highlight text %}
%{
/* C語言定義，包括了頭文件、宏定義、全局變量定義、函數聲明等 */
}%
%option noinput       /* 常見的配置選項 */
WHITE_SPACE [\ \t\b]  /* 正則表達式的定義，如下section時可以直接使用這裡定義的宏 */
COMMENT #.*
%%
{WHITE_SPACE}           |
{COMMENT}               {/* ignore */}  /* 規則定義處理 */
%%
/* C語言，函數實現等 */
{% endhighlight %}



## YACC 語法分析

bison 讀入一個 CFG 文法的文件，在程序內經過計算，輸出一個 parser generator 的 c 文件；也就是說 Bison 適合上下文無關文法，採用 LALR Parser (LALR語法分析器)。

在實現時，bison 會創建一組狀態，每個狀態用來表示規則中的一個可能位置，同時還會維護一個堆棧，這個堆棧叫做分析器堆棧 (parser stack)。每次讀入一個終結符 (token)，它會將該終結符及其語意值一起壓入堆棧，把一個 token 壓入堆棧通常叫做移進 (shifting)。

當已經移進的後 n 個終結符可以與一個左側的文法規則相匹配時，這個 n 各終結符會被根據那個規則結合起來，同時將這 n 個終結符出棧，左側的符號如棧，這叫做歸約 (reduction)。

如果可以將 bison+flex 混合使用，當語法分析需要輸入標記 (token) 時，就會調用 yylex() ，然後匹配規則，如果找到則返回。




<!--


<br><br><h2>預讀</h2><p>
Bison 分析器並不總是當 N 個終結符與組匹配某一規則時立即進行歸約，這種策略對於大部分語言來說並不合適。相反，當可以進行歸約時，分析器有時會“預讀” (looks ahead) 下一個終結符來決定做什麼。<br><br>

當一個終結符被讀進來後，並不會立即移進堆棧，而是首先作為一個預讀終結符 (look-ahead token)。此後，分析器開始對棧上的終結符和組執行一個或多個歸約，而預讀終結符仍然放在一邊。當沒有歸約可做時，這個預讀終結符才會被移進堆棧。這並不表示所有可能的歸約都已經做了，這要取決於預讀終結符的類型，一些規則可能選擇推遲它們的使用。<br><br>



 主程序分為如下幾個步驟進行:
    1. 讀取/解析命令行選項 Main.getargs(), 打開文件 Files.openfiles()
       這些是程序基本的準備工作, 一般不屬於算法範疇, 我們略去不述.
    2. 讀入文法文件 Reader.reader(), 文法文件一般以 .y 為後綴.
       讀取時建立內存的一些中間數據結構(主要是單鏈結構), 後面詳述.
    3. 將第2步讀入的數據進行檢查, 消除無用的,錯誤的產生式, 建立/轉換為適合
       計算 LR 的數據結構(主要是單鏈=>數組結構). 預先計算一些輔助數據.
    4. 計算 LR0 狀態集, 結果可能是一個非確定的(有衝突的)有限狀態機.
    5. 轉變第 4 步的狀態機為確定的 LALR 狀態機.
    6. 如果第 5 步中有 s/r, r/r 衝突, 則解決衝突.
    7. 輸出及別的收尾工作. 一般略去不細述了.
-->

### 語法定義

同 flex 相似，仍然通過 ```%%``` 將文件分為三部分：

1. 第一部分將原封不動放到生成源碼中，如果要寫入 C/C++ 代碼，則必須用 ```%{``` 和 ```%}``` 括起來。

2. 第二部分是規則段，包括了模式 (正則表達式) 和動作，由空白分開，當匹配到模式時，就會執行後面的動作。每條規則都是由 ```':'``` 操作符左側的一個名字、右側的符號列表、動作代碼、規則結束符 ```(;)``` 組成。

3. 第三部分可以直接寫入 C/C++ 代碼。

如下，是一個簡單示例，分別是 frame.l 和 frame.y 文件。

{% highlight text %}
%{
int yywrap(void);
%}
%%
%%
int yywrap(void)
{
    return 1;
}
{% endhighlight %}

{% highlight text %}
%{
void yyerror(const char *s);
%}
%%
program:
   ;
%%
void yyerror(const char *s)
{
}
int main()
{
    yyparse();
    return 0;
}
{% endhighlight %}

然後，通過如下命令進行測試。

{% highlight text %}
----- 編譯生成lex.yy.c文件
$ flex frame.l

----- 產生frame.tab.c和frame.tab.h文件
$ bison -d frame.y

----- 編譯生成二進制文件
$ gcc frame.tab.c lex.yy.c
{% endhighlight %}

### 常用功能

yacc 中定義了很多的符號，詳細的可以查看 [Bison Symbols](http://dinosaur.compilertools.net/bison/bison_13.html) 中的介紹，如下簡單介紹常見的符號定義：

{% highlight text %}
%start foobar
  修改默認的開始規則，例如從foobar規則開始解析，默認從第一條規則開始
{% endhighlight %}


#### 高級yylval

YACC 的 yylval 類型取決於 YYSTYPE 定義 (一般通過 typedef 定義)，可以通過定義 YYSTYPE 為聯合體，在 YACC 中，也可以使用 ```%union``` 語句，此時會自動定義該類型的變量。

{% highlight text %}
%token TOKHEATER TOKHEAT TOKTARGET TOKTEMPERATURE
%union {
    int number;
    char *string;
}
%token <number> STATE
%token <number> NUMBER
%token <string> WORD
{% endhighlight %}

定義了我們的聯合體，它僅包含數字和字體串，然後使用一個擴展的%token語法，告訴YACC應該取聯合體的哪一個部分。


<!--
這個例子中，我們定義STATE 為一個整數，這點跟前面一樣，NUMBER符號用於讀取溫度值。

不過新的WORD被定義為一個字符串。

分詞器文件也有很多改變：

%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
%}
%%
[0−9]+             yylval.number=atoi(yytext); return NUMBER;
heater             return TOKHEATER;
heat               return TOKHEATER;
on|off             yylval.number=!strcmp(yytext,"on"); return STATE;
target             return TOKTARGET;
temperature        return TOKTEMPERATURE;
[a−z0−9]+          yylval.string=strdup(yytext);return WORD;
\n                 /* ignore end of line */;
[ \t]+             /* ignore whitespace */;
%%

如你所見，我們不再直接獲取yylval的值，而是添加一個後綴指示想取得哪個部分的值。不過在YACC語法中，我們無須這樣做，因為YACC為我們做了神奇的這些：

heater_select:
        TOKHEATER WORD
        {
            printf("\tSelected heater '%s'\n",$2);
            heater=$2;
        }
        ;

由於上面的%token定義，YACC自動從聯合體中挑選string成員。同時也請注意，我們保存了一份$2的副本，它在後面被用於告訴用戶是哪一個加熱器發出的命令：

target_set:
        TOKTARGET TOKTEMPERATURE NUMBER
        {
            printf("\tHeater '%s' temperature set to %d\n",heater,$3);
        }
        ;
-->

#### 雜項

簡單介紹其它功能。

##### 變量

```$$ $1 $2 ...``` 定義了默認的參數，示例如下：

{% highlight text %}
exp:
| exp '+' exp     { $$ = $1 + $3; }

exp[result]:
| exp[left] '+' exp[right]  { $result = $left + $right; }
{% endhighlight %}



<!--
上述函數的作用為<ul><li>
    yywrap<br>
    可以在lex或者yacc文件中定義，該函數是必須的，給了這個函數實現之後不再需要依賴flex庫，在次只是簡單返回1，表示輸入已經結束。函數yywrap能夠用於是否繼續讀取其它的文件，當遇到EOF時，你可以打開其它文件並返回0。或者，返回1，意味著真正的結束。</li><br><li>

    program<br>
    這是語法規則裡面的第一個非終結符，注意上面的格式哦：“program”後 面緊跟著一個冒號“:”，然後換行之後有一個分號“;”，這表明這個 program是由空串組成的。至於什麼是非終結符以及什麼是終結符，還有什 麼是語法規則都會在後面的章節中進行詳細介 紹。</li><br><li>

    yyerror<br>
    錯誤處理函數，為了保證代碼儘可能的簡潔，在此什麼都不做。</li><br><li>

    yyparse<br>
    這個函數是yacc生成的，在代碼裡可以直接使用。lex生成的函數為yylex，實際上yyparse還間接調用了yylex函數，可以查看生成的C源文件。
</li></ul>
</p>


* lex/yacc程序組成結構、文件格式。
* 如何在lex/yacc中使用C++和STL庫，用extern "C"聲明那些lex/yacc生成的、要鏈接的C函數，如yylex(), yywrap(), yyerror()。
* 重定義YYSTYPE/yylval為複雜類型。
* lex裡多狀態的定義和使用，用BEGIN宏在初始態和其它狀態間切換。
* lex里正則表達式的定義、識別方式。
* lex裡用yylval向yacc返回數據。
* yacc裡用%token<>方式聲明yacc記號。
* yacc裡用%type<>方式聲明非終結符的類型。
* 在yacc嵌入的C代碼動作裡，對記號屬性($1, $2等)、和非終結符屬性($$)的正確引用方法。
* 對yyin/yyout重賦值，以改變yacc默認的輸入/輸出目標。





<br><br><h2>調試</h2><p>
通常來說 bison 生成 *.tab.{c,h} 兩個文件，如果通過 --report=state 或者 --verbose 生成 *.output 輸出。
-->




<!--
<ul><li>
token 標記<br>
在yacc文件中進行定義，通過bison生成頭文件，頭文件中會定義為宏或者enmu(yacc生成的.c文件直接使用數字，在yytoknum[]中)，通常從258開始，通常1～265表示字符，還有一些內部的定義。如frame.y，則生成frame.tab.h，在frame.l中通常需要包含該頭文件。</li><br><li>

    int yyparse(void)/int yylex(void)<br>
    yyparse()為bison的執行入口，程序可以直接通過該函數執行；yyparse()會通過yylex()獲得token。
    </li></ul>
    flex frame.l<br>
    bison -d frame.y<br>
    cc frame.tab.c lex.yy.c -ll -o example<br><br>

    如果需要進行調試需要通過如下方式進行編譯<br>
    flex frame.l<br>
    bison -y -d -t frame.l<br>
    cc -g frame.tab.c lex.yy.c -ll -o example<br>
    gdb example<br>
    (gdb) set yydebug=1<br>
    (gdb) continue
    </p>
-->





## 源碼解析

Linux 一般來說，詞法和語法解析都是通過 Flex 與 Bison 完成的；而在 MySQL 中，詞法分析使用自己的程序，而語法分析使用的是 Bison；Bison 會根據 MySQL 定義的語法規則，進行語法解析。

完成語法解析後，會將解析結果生成的數據結構保存在 struct LEX 中，該結構體在 sql/sql_lex.h 文件中定義。

{% highlight cpp %}
struct LEX: public Query_tables_list
{
  friend bool lex_start(THD *thd);

  SELECT_LEX_UNIT *unit;                 ///< Outer-most query expression
  /// @todo: select_lex can be replaced with unit->first-select()
  SELECT_LEX *select_lex;                ///< First query block
  SELECT_LEX *all_selects_list;          ///< List of all query blocks
private:
  /* current SELECT_LEX in parsing */
  SELECT_LEX *m_current_select;
  ... ...
}
{% endhighlight %}

優化器會根據這裡的數據，生成相應的執行計劃，最後調用存儲引擎執行。

### 執行過程

以下是語法解析模塊掉用過程。

{% highlight text %}
mysql_parse()
 |-mysql_reset_thd_for_next_command()
 |-lex_start()
 |-query_cache_send_result_to_client()              #  首先查看cache
 |-parse_sql()                                      #  MYSQLparse的外包函數
   |-MYSQLparse()                                   #  實際的解析函數入口
{% endhighlight %}

如上，SQL 解析入口會調用 MYSQLparse ，而在 sql/sql_yacc.cc 中有如下的宏定義，也就說，在預編譯階段，會將 yyparse 替換為 MYSQLparse ，所以 **實際調用的仍是 yyparse 函數**。

{% highlight c %}
#define yyparse         MYSQLparse
{% endhighlight %}

記下來詳細介紹其實現細節。

### 詞法解析

MYSQL 的詞法分析並沒有使用 LEX，而是有自己的一套詞法分析，代碼詳見 sql/sql_lex.cc 中的實現，其入口函數是 MYSQLlex() 。

{% highlight cpp %}
int MYSQLlex(YYSTYPE *yylval, YYLTYPE *yylloc, THD *thd)
{
  ... ...
  token= lex_one_token(yylval, thd);
  yylloc->cpp.start= lip->get_cpp_tok_start();
  yylloc->raw.start= lip->get_tok_start();

  switch(token) {
  case WITH:
    /*
      Parsing 'WITH' 'ROLLUP' or 'WITH' 'CUBE' requires 2 look ups,
      which makes the grammar LALR(2).
      Replace by a single 'WITH_ROLLUP' or 'WITH_CUBE' token,
      to transform the grammar into a LALR(1) grammar,
      which sql_yacc.yy can process.
    */
    token= lex_one_token(yylval, thd);
    switch(token) {
    case CUBE_SYM:
      yylloc->cpp.end= lip->get_cpp_ptr();
      yylloc->raw.end= lip->get_ptr();
      lip->add_digest_token(WITH_CUBE_SYM, yylval);
      return WITH_CUBE_SYM;
    case ROLLUP_SYM:
      yylloc->cpp.end= lip->get_cpp_ptr();
      yylloc->raw.end= lip->get_ptr();
      lip->add_digest_token(WITH_ROLLUP_SYM, yylval);
      return WITH_ROLLUP_SYM;
    default:
      /*
        Save the token following 'WITH'
      */
      lip->lookahead_yylval= lip->yylval;
      lip->yylval= NULL;
      lip->lookahead_token= token;
      yylloc->cpp.end= lip->get_cpp_ptr();
      yylloc->raw.end= lip->get_ptr();
      lip->add_digest_token(WITH, yylval);
      return WITH;
    }
    break;
  }
  ... ...
}
{% endhighlight %}



### 語法分析

Bison 和詞法分析的函數接口是 yylex()，在需要的時候掉用 yylex() 獲取詞法解析的數據，並完成自己的語法解析。

正常來說，Bison 的實際入口函數應該是 yyparse() ，而在 MySQL 中通過宏定義，將 yyparse() 替換為 MYSQLParse()；如上所述，實際調用的仍然是 yyparse() 。

另外，我們可以根據 Bison 中的 Action 操作來查看 MySQL 解析結果的存儲結構。



## 調試

在這裡通過考察存儲的 WHERE 數據結構來查看語法解析的結果。

{% highlight text %}
(gdb) attach PID
(gdb) set print pretty on                                  # 設置顯示樣式
(gdb) b mysql_execute_command                              # 可以用來查看所有的SQL
(gdb) p thd->lex->select_lex
(gdb) p ((Item_cond*)thd->lex->select_lex->where)->list    # 查看WHERE中的list
(gdb) detach
{% endhighlight %}



## 參考

### Flex/Bison

關於最原始的論文，可以參考 [Lex - A Lexical Analyzer Generator](http://www.cs.utexas.edu/users/novak/lexpaper.htm) ([本地](/reference/databases/mysql/LEX_A_Lexical_Analyzer_Generator.html))，以及 [Yacc: Yet Another Compiler-Compiler](http://www.cs.utexas.edu/users/novak/yaccpaper.htm) ([本地](/reference/databases/mysql/YACC_Yet_Another_Compiler_Compiler.html))。

對於 Lex 和 Yacc 來說，比較經典的入門可以參考 [Lex & Yacc Tutorial](http://epaperpress.com/lexandyacc/index.html)，其中包括瞭如何編寫一個計算器，以及相關的調試等信息；也可以參考 [本地文檔](/reference/databases/mysql/LexAndYaccTutorial.pdf)，以及相關的 [源碼](/reference/databases/mysql/LexAndYaccCode.zip) 。

關於總體介紹可以參考 [Lex and YACC primer](http://www.tldp.org/HOWTO/Lex-YACC-HOWTO.html)，或者 [本地文檔](/reference/databases/mysql/LEX_YACC_Primer_HOWTO.html)，也可以查看中文翻譯 [如何使用 Lex/YACC](http://segmentfault.com/blog/icattlecoder/1190000000396608)，以及 [本地](/reference/databases/mysql/LEX_YACC_Primer_HOWTO_cn.mht)，以及 [示例源碼](/reference/databases/mysql/lex-yacc-examples.tar.gz) ；以及 [Bison-Flex 筆記](/reference/databases/mysql/Bison_Flex_Notes.mht)、[Flex/Bison Tutorial](/reference/databases/mysql/Tutorial-Flex_Bison.pdf) 。

關於調試方法可以參考 [Understanding Your Parser](http://www.gnu.org/software/bison/manual/html_node/Understanding.html)，這個是 [Bison Offical Documents](http://www.gnu.org/software/bison/manual/html_node/index.html) 文檔的一部分；更多可以參考 [dinosaur.compilertools.net](http://dinosaur.compilertools.net/) 查看相關的資料。


<!--
自己動手寫編譯器
http://pandolia.net/tinyc/
https://bellard.org/tcc/
計算器
http://good-ed.blogspot.tw/2010/04/lexyacc.html
編寫自己的編譯器
http://coolshell.cn/articles/1547.html

http://blog.csdn.net/huyansoft/article/details/8860224
http://blog.csdn.net/lidan3959/article/details/8237914
http://www.tuicool.com/articles/3aMVzi
http://blog.csdn.net/sfifei/article/details/9449629
-->




{% highlight text %}
{% endhighlight %}
