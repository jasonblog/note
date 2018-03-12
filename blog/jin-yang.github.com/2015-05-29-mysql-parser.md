---
title: MySQL 语法解析
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,lex,bison,yacc,flex,词法,语法
description: 当服务器接收到一条 SQL 语句时，其处理过程为：词法分析，语法分析，语义分析，构造执行树，生成执行计划，计划的执行。其中，词法语法解析的处理过程根编译原理上的东西基本类似；MySQL 并没有使用 lex 来实现词法分析，但是语法分析却用了 yacc。与之对比的 SQLite 数据库，其词法分析器是手工写的，语法分析器由 Lemon 生成。在此介绍其在 MySQL 中的使用。
---

当服务器接收到一条 SQL 语句时，其处理过程为：词法分析，语法分析，语义分析，构造执行树，生成执行计划，计划的执行。

其中，词法语法解析的处理过程根编译原理上的东西基本类似；MySQL 并没有使用 lex 来实现词法分析，但是语法分析却用了 yacc。

与之对比的 SQLite 数据库，其词法分析器是手工写的，语法分析器由 Lemon 生成。

在此介绍其在 MySQL 中的使用。

<!-- more -->

## 简介

Lex-Yacc (词法扫描器-语法分析器) 在 Linux 上就是 flex-bison；使用 bison 时，采用的语法必须是上下文无关文法 (context-free grammar)；可以通过 ```yum install flex flex-devel bison``` 进行安装。

首先，介绍下基本的概念。

### BNF

也就是 Backus-Naur Form 巴科斯范式，由 John Backus 和 Peter Naur 首先引入的，用来描述计算机语言语法的符号集，现在，几乎每一位新编程语言书籍的作者都使用巴科斯范式来定义编程语言的语法规则。

其推到规则通过 ```::=``` 定义，左侧为非终结符，右侧为一个表达式；表达式由一个符号序列，或用 ```'|'``` 分隔的多个符号序列构成，从未在左端出现的符号叫做终结符。

{% highlight text %}
"..."  : 术语符号，表示字符本身，用double_quote用来代表双引号
< >    : 必选项
[ ]    : 可选项，最多出现一次
{ }    : 可重复0至无数次的项
|      : 左右两边任选一项，相当于OR
::=    : 被定义为
{% endhighlight %}

如下是 Java 中的 For 语句实例：

{% highlight text %}
FOR_STATEMENT ::=
"for" "(" ( variable_declaration |
( expression ";" ) | ";" )
[ expression ] ";"
[ expression ] ";"
")" statement
{% endhighlight %}

其中 RFC2234 定义了扩展的巴科斯范式 (ABNF)。

### 上下文无关文法

简单来说就是每个产生式的左边只有一个非终结符；首先，试着用汉语来稍微解释一下。

{% highlight text %}
本来这个进球就是违例的，但你不肯承认也没办法
我有一本来自美国的花花公子杂志
拿我的笔记本来
{% endhighlight %}

如果汉语是上下文无关文法，那么我们任何时候看见 ```"本来"``` 两个字，都可以把它规约为一个词；可惜汉语不是上下文无关文法，所以能否归约为一个词，要看它的上下文是什么。如上的示例中，只有第一句可以规约为一个词。

<!--
上下文无关文法就是说这个文法中所有的产生式左边只有一个非终结符，比如：
S -> aSb
S -> ab
这个文法有两个产生式，每个产生式左边只有一个非终结符S，这就是上下文无关文法，因为你只要找到符合产生式右边的串，就可以把它归约为对应的非终结符。

比如：
aSb -> aaSbb
S -> ab
这就是上下文相关文法，因为它的第一个产生式左边有不止一个符号，所以你在匹配这个产生式中的S的时候必需确保这个S有正确的“上下文”，也就是左边的a和右边的b，所以叫上下文相关文法。




其中，生成式由终结符和非终结符组成，分别用大写、小写表示；如果终结字符是一个单字符，那么可以直接使用该字符，如;、)、[。对于语义来说，如整型 INTEGER 的值可以是 1、34、5555。当匹配一个语义时可以直接执行一个动作。

解析器相对于状态机多了一个栈，从而可以处理移进和规约。简单的说，LR(1) 就是指，只需要预读一个 token 那么就可以知道如何解析字符串中的任意一部分。虽然 bison 可以适用于几乎所有的上下文无关语法，不过其针对 LR(1) 做了专门的优化。

在一些确定性的 LR(1) 语法中，仍然会存在这歧义，可能不知道对那个语法规则执行规约，或者不知道是执行规约还是移进，分别被称为 规约/规约冲突 或者 移进/规约冲突。

关于上下文相关、无关语法可以参考 WiKi <a href="https://en.wikipedia.org/wiki/Context-free_grammar">Context-free grammar</a>、<a href="https://en.wikipedia.org/wiki/Context-sensitive_grammar">Context-sensitive grammar</a>，以及 <a href="http://cs.union.edu/~striegnk/courses/nlp-with-prolog/html/node37.html">Context Free Grammars</a>，也可以参考本地文档。
-->

## Lex 词法分析

Flex 采用的是状态机，通过分析输入流 (字符流)，只要发现一段字符能够匹配一个关键字 (正则表达式)，就会采取对应的动作。

Flex 文件被 ```%%``` 分成了上中下三个部分：

1. 第一部分中要写入 C/C++ 代码必须用 ```%{``` 和 ```%}``` 括起来，将原封不动放到生成源码中。

2. 第二部分是规则段，包括了模式 (正则表达式) 和动作，由空白分开，当匹配到模式时，就会执行后面的动作。

3. 第三部分可以直接写入 C/C++ 代码。

yylex() 是扫描程序的入口，调用该函数启动或重新开始，该函数会初始化一些全局变量，然后开始扫描。如果定义的 flex 动作是将数值传递给调用程序 (return)，那么对 yylex() 的下次调用就从它停止的地方继续扫描。

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

特殊字符 ```'.'``` 表示匹配换行符以外的任意单个字符，```'\n'``` 匹配换行符；```ECHO``` 表示输出匹配的模式，默认行为为，也就是将输入原样输出：

{% highlight c %}
#define ECHO fwrite(yytext, yyleng, 1, yyout)
{% endhighlight %}

默认将 stdin 作为输入，可以通过如下命令测试。

{% highlight text %}
----- 首先按照规则生成C源码lex.yy.c
$ flex example.l

----- 然后进行编译
$ cc lex.yy.c -o example -lfl

----- 直接执行测试，Ctrl-D退出
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

在处理时，flex 采用两个原则：A) 只匹配一次；B) 执行当前输入的最长可能匹配值。也就是对与 island 不会匹配 is 和 land 。当然，我们可以使用一个文件作为关键字列表，而非每次都需要编译。

解析时会通过 yyin 读取，如果需要在 yacc 或者其它文件中设置，那么可以通过如下方式修改。

{% highlight c %}
extern FILE *yyin;
yyin = fopen("filename","r");
{% endhighlight %}

通过 flex 处理文件后，会将匹配转化为指定的符号，然后供 yacc 处理。

### 常用功能

简单列举常用函数。


#### 正则表达式

flex 常用正则表达式：

1. 格式与 grep 相似；
2. ```<<EOF>>``` 标示文件结束；
3. 常用字符集，如 ```[:alpha:]```, ```[:digit:]```, ```[:alnum:]```, ```[:space:]``` 等；
4. ```{name}``` 使用预定义的 name 。

简单示例，计算平均值。

{% highlight text %}
%{
#include <stdio.h>
#include <stdlib.h>
%}
dgt    [0-9] // 通过name方式定义
%%
{dgt}+   return atoi(yytext);
%%
void main()
{
   int val, total = 0, n = 0;
   while ( (val = yylex()) > 0 ) { // 到文件结束时返回0
      total += val;
      n++;
   }
   if (n > 0) printf(“ave = %d\n”, total/n);
}
{% endhighlight %}

如上如果在编译时使用 ```-Wall``` 参数，会报 ```warning: `yyunput’ defined but not used``` 之类的异常，如下介绍可以通过如下选项关闭。

{% highlight text %}
%option nounput
%option noinput
{% endhighlight %}

#### 多规则匹配

如果有多个值匹配，那么 flex 会按照如下的规则选取。

1. 贪婪匹配，选择最大的匹配值；
2. 多个规则匹配，选择第一个；
3. 没有规则匹配则会选择默认规则。

例如，通过 ```"/*"(.|\n)*"*/"``` 规则匹配 C 语言中的注释，那么如下场景可能出错。

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

贪婪匹配，会从 ```/* def``` 到 ```nts */``` 之间的内容都作为注释，此时就需要使用条件 (Condition) 规则。例如，以 ```<S>``` 开始的规则，只有在条件 S 时才会进行匹配，可以在 definition section 段通过如下方式定义条件。

{% highlight text %}
%x S exclusive start conditions
%s S inclusive start conditions
{% endhighlight %}

然后，通过 ```BEGIN(S)``` 进入条件，另外，flex 有个初始条件，可以通过 ```BEGIN(INITIAL)``` 返回；如果使用多个状态，那么实际上可以实现一个状态机，详见 [lex tutorial.ppt](https://www2.cs.arizona.edu/~debray/Teaching/CSc453/DOCS/lex tutorial.ppt) 或者 [本地文档](/reference/databases/mysql/lex_tutorial.ppt) 。

关于上述内容，也可以参考 [Start conditions](http://dinosaur.compilertools.net/flex/flex_11.html) 中的介绍。


#### yyterminate()

可在一个动作中代替 return 使用，用于结束扫描并向扫描器的调用者返回 0；可以通过如下方式自定义。

{% highlight c %}
#ifdef yyterminate
# undef yyterminate
#endif
#define yyterminate() \
    do { free (foobar); foobar = NULL; pos = 0; len = 0; \
        return YY_NULL; } while (0)
{% endhighlight %}

#### 配置选项

{% highlight text %}
%option yylineno    提供当前的行信息，通常用于后续打印错误行信息
%option noyywrap    不生成yywrap()声明
%option noinput     会生成#define YY_NO_INPUT 1定义
%option nounput     会生成#define YY_NO_UNPUT 1定义
{% endhighlight %}

flex 会声明一个 ```int yywarp(void);``` 函数，但是不会自动定义，所以通常会在最后的 section 实现该函数。该函数的作用是将多个输入文件打包成一个输入，也就是当 ```yylex()``` 读取到一个文件结束 (EOF) 时，会调用 ```yywrap()``` ，如果返回 1 则表示后面没有其它输入文件了，此时 ```yylex()``` 函数结束；当然，```yywrap()``` 也可以打开下一个输入文件，再向 ```yylex()``` 函数返回 0 ，告诉它后面还有别的输入文件。

如果只有一个文件，那么可以通过 ```%option noyywrap``` 不声明该函数，也就不需要再实现。


### 其它


#### 值传递

在通过 flex 进行扫描时，会将值保存在 yylval 变量中，而 bison 则读取 yylval 中的值，该变量默认是 int 类型，如果要使用字符串类型，那么可以在 .l+.y 的头部第一句加入 ```#define YYSTYPE char*``` 即可。

{% highlight text %}
// 在.l赋值的时候，要特别注意，需要拷贝字符串
yylval = strdup(yytext);  return WORD;
// 在.y取用的时候，直接强转就可以了
(char*)$1
{% endhighlight %}

关于更优雅的实现方式，当然是用 union 啦，仿照上面，很容易写出来的。


#### 标准格式

{% highlight text %}
%{
/* C语言定义，包括了头文件、宏定义、全局变量定义、函数声明等 */
}%
%option noinput       /* 常见的配置选项 */
WHITE_SPACE [\ \t\b]  /* 正则表达式的定义，如下section时可以直接使用这里定义的宏 */
COMMENT #.*
%%
{WHITE_SPACE}           |
{COMMENT}               {/* ignore */}  /* 规则定义处理 */
%%
/* C语言，函数实现等 */
{% endhighlight %}



## YACC 语法分析

bison 读入一个 CFG 文法的文件，在程序内经过计算，输出一个 parser generator 的 c 文件；也就是说 Bison 适合上下文无关文法，采用 LALR Parser (LALR语法分析器)。

在实现时，bison 会创建一组状态，每个状态用来表示规则中的一个可能位置，同时还会维护一个堆栈，这个堆栈叫做分析器堆栈 (parser stack)。每次读入一个终结符 (token)，它会将该终结符及其语意值一起压入堆栈，把一个 token 压入堆栈通常叫做移进 (shifting)。

当已经移进的后 n 个终结符可以与一个左侧的文法规则相匹配时，这个 n 各终结符会被根据那个规则结合起来，同时将这 n 个终结符出栈，左侧的符号如栈，这叫做归约 (reduction)。

如果可以将 bison+flex 混合使用，当语法分析需要输入标记 (token) 时，就会调用 yylex() ，然后匹配规则，如果找到则返回。




<!--


<br><br><h2>预读</h2><p>
Bison 分析器并不总是当 N 个终结符与组匹配某一规则时立即进行归约，这种策略对于大部分语言来说并不合适。相反，当可以进行归约时，分析器有时会“预读” (looks ahead) 下一个终结符来决定做什么。<br><br>

当一个终结符被读进来后，并不会立即移进堆栈，而是首先作为一个预读终结符 (look-ahead token)。此后，分析器开始对栈上的终结符和组执行一个或多个归约，而预读终结符仍然放在一边。当没有归约可做时，这个预读终结符才会被移进堆栈。这并不表示所有可能的归约都已经做了，这要取决于预读终结符的类型，一些规则可能选择推迟它们的使用。<br><br>



 主程序分为如下几个步骤进行:
    1. 读取/解析命令行选项 Main.getargs(), 打开文件 Files.openfiles()
       这些是程序基本的准备工作, 一般不属于算法范畴, 我们略去不述.
    2. 读入文法文件 Reader.reader(), 文法文件一般以 .y 为后缀.
       读取时建立内存的一些中间数据结构(主要是单链结构), 后面详述.
    3. 将第2步读入的数据进行检查, 消除无用的,错误的产生式, 建立/转换为适合
       计算 LR 的数据结构(主要是单链=>数组结构). 预先计算一些辅助数据.
    4. 计算 LR0 状态集, 结果可能是一个非确定的(有冲突的)有限状态机.
    5. 转变第 4 步的状态机为确定的 LALR 状态机.
    6. 如果第 5 步中有 s/r, r/r 冲突, 则解决冲突.
    7. 输出及别的收尾工作. 一般略去不细述了.
-->

### 语法定义

同 flex 相似，仍然通过 ```%%``` 将文件分为三部分：

1. 第一部分将原封不动放到生成源码中，如果要写入 C/C++ 代码，则必须用 ```%{``` 和 ```%}``` 括起来。

2. 第二部分是规则段，包括了模式 (正则表达式) 和动作，由空白分开，当匹配到模式时，就会执行后面的动作。每条规则都是由 ```':'``` 操作符左侧的一个名字、右侧的符号列表、动作代码、规则结束符 ```(;)``` 组成。

3. 第三部分可以直接写入 C/C++ 代码。

如下，是一个简单示例，分别是 frame.l 和 frame.y 文件。

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

然后，通过如下命令进行测试。

{% highlight text %}
----- 编译生成lex.yy.c文件
$ flex frame.l

----- 产生frame.tab.c和frame.tab.h文件
$ bison -d frame.y

----- 编译生成二进制文件
$ gcc frame.tab.c lex.yy.c
{% endhighlight %}

### 常用功能

yacc 中定义了很多的符号，详细的可以查看 [Bison Symbols](http://dinosaur.compilertools.net/bison/bison_13.html) 中的介绍，如下简单介绍常见的符号定义：

{% highlight text %}
%start foobar
  修改默认的开始规则，例如从foobar规则开始解析，默认从第一条规则开始
{% endhighlight %}


#### 高级yylval

YACC 的 yylval 类型取决于 YYSTYPE 定义 (一般通过 typedef 定义)，可以通过定义 YYSTYPE 为联合体，在 YACC 中，也可以使用 ```%union``` 语句，此时会自动定义该类型的变量。

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

定义了我们的联合体，它仅包含数字和字体串，然后使用一个扩展的%token语法，告诉YACC应该取联合体的哪一个部分。


<!--
这个例子中，我们定义STATE 为一个整数，这点跟前面一样，NUMBER符号用于读取温度值。

不过新的WORD被定义为一个字符串。

分词器文件也有很多改变：

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

如你所见，我们不再直接获取yylval的值，而是添加一个后缀指示想取得哪个部分的值。不过在YACC语法中，我们无须这样做，因为YACC为我们做了神奇的这些：

heater_select:
        TOKHEATER WORD
        {
            printf("\tSelected heater '%s'\n",$2);
            heater=$2;
        }
        ;

由于上面的%token定义，YACC自动从联合体中挑选string成员。同时也请注意，我们保存了一份$2的副本，它在后面被用于告诉用户是哪一个加热器发出的命令：

target_set:
        TOKTARGET TOKTEMPERATURE NUMBER
        {
            printf("\tHeater '%s' temperature set to %d\n",heater,$3);
        }
        ;
-->

#### 杂项

简单介绍其它功能。

##### 变量

```$$ $1 $2 ...``` 定义了默认的参数，示例如下：

{% highlight text %}
exp:
| exp '+' exp     { $$ = $1 + $3; }

exp[result]:
| exp[left] '+' exp[right]  { $result = $left + $right; }
{% endhighlight %}



<!--
上述函数的作用为<ul><li>
    yywrap<br>
    可以在lex或者yacc文件中定义，该函数是必须的，给了这个函数实现之后不再需要依赖flex库，在次只是简单返回1，表示输入已经结束。函数yywrap能够用于是否继续读取其它的文件，当遇到EOF时，你可以打开其它文件并返回0。或者，返回1，意味着真正的结束。</li><br><li>

    program<br>
    这是语法规则里面的第一个非终结符，注意上面的格式哦：“program”后 面紧跟着一个冒号“:”，然后换行之后有一个分号“;”，这表明这个 program是由空串组成的。至于什么是非终结符以及什么是终结符，还有什 么是语法规则都会在后面的章节中进行详细介 绍。</li><br><li>

    yyerror<br>
    错误处理函数，为了保证代码尽可能的简洁，在此什么都不做。</li><br><li>

    yyparse<br>
    这个函数是yacc生成的，在代码里可以直接使用。lex生成的函数为yylex，实际上yyparse还间接调用了yylex函数，可以查看生成的C源文件。
</li></ul>
</p>


* lex/yacc程序组成结构、文件格式。
* 如何在lex/yacc中使用C++和STL库，用extern "C"声明那些lex/yacc生成的、要链接的C函数，如yylex(), yywrap(), yyerror()。
* 重定义YYSTYPE/yylval为复杂类型。
* lex里多状态的定义和使用，用BEGIN宏在初始态和其它状态间切换。
* lex里正则表达式的定义、识别方式。
* lex里用yylval向yacc返回数据。
* yacc里用%token<>方式声明yacc记号。
* yacc里用%type<>方式声明非终结符的类型。
* 在yacc嵌入的C代码动作里，对记号属性($1, $2等)、和非终结符属性($$)的正确引用方法。
* 对yyin/yyout重赋值，以改变yacc默认的输入/输出目标。





<br><br><h2>调试</h2><p>
通常来说 bison 生成 *.tab.{c,h} 两个文件，如果通过 --report=state 或者 --verbose 生成 *.output 输出。
-->




<!--
<ul><li>
token 标记<br>
在yacc文件中进行定义，通过bison生成头文件，头文件中会定义为宏或者enmu(yacc生成的.c文件直接使用数字，在yytoknum[]中)，通常从258开始，通常1～265表示字符，还有一些内部的定义。如frame.y，则生成frame.tab.h，在frame.l中通常需要包含该头文件。</li><br><li>

    int yyparse(void)/int yylex(void)<br>
    yyparse()为bison的执行入口，程序可以直接通过该函数执行；yyparse()会通过yylex()获得token。
    </li></ul>
    flex frame.l<br>
    bison -d frame.y<br>
    cc frame.tab.c lex.yy.c -ll -o example<br><br>

    如果需要进行调试需要通过如下方式进行编译<br>
    flex frame.l<br>
    bison -y -d -t frame.l<br>
    cc -g frame.tab.c lex.yy.c -ll -o example<br>
    gdb example<br>
    (gdb) set yydebug=1<br>
    (gdb) continue
    </p>
-->





## 源码解析

Linux 一般来说，词法和语法解析都是通过 Flex 与 Bison 完成的；而在 MySQL 中，词法分析使用自己的程序，而语法分析使用的是 Bison；Bison 会根据 MySQL 定义的语法规则，进行语法解析。

完成语法解析后，会将解析结果生成的数据结构保存在 struct LEX 中，该结构体在 sql/sql_lex.h 文件中定义。

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

优化器会根据这里的数据，生成相应的执行计划，最后调用存储引擎执行。

### 执行过程

以下是语法解析模块掉用过程。

{% highlight text %}
mysql_parse()
 |-mysql_reset_thd_for_next_command()
 |-lex_start()
 |-query_cache_send_result_to_client()              #  首先查看cache
 |-parse_sql()                                      #  MYSQLparse的外包函数
   |-MYSQLparse()                                   #  实际的解析函数入口
{% endhighlight %}

如上，SQL 解析入口会调用 MYSQLparse ，而在 sql/sql_yacc.cc 中有如下的宏定义，也就说，在预编译阶段，会将 yyparse 替换为 MYSQLparse ，所以 **实际调用的仍是 yyparse 函数**。

{% highlight c %}
#define yyparse         MYSQLparse
{% endhighlight %}

记下来详细介绍其实现细节。

### 词法解析

MYSQL 的词法分析并没有使用 LEX，而是有自己的一套词法分析，代码详见 sql/sql_lex.cc 中的实现，其入口函数是 MYSQLlex() 。

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



### 语法分析

Bison 和词法分析的函数接口是 yylex()，在需要的时候掉用 yylex() 获取词法解析的数据，并完成自己的语法解析。

正常来说，Bison 的实际入口函数应该是 yyparse() ，而在 MySQL 中通过宏定义，将 yyparse() 替换为 MYSQLParse()；如上所述，实际调用的仍然是 yyparse() 。

另外，我们可以根据 Bison 中的 Action 操作来查看 MySQL 解析结果的存储结构。



## 调试

在这里通过考察存储的 WHERE 数据结构来查看语法解析的结果。

{% highlight text %}
(gdb) attach PID
(gdb) set print pretty on                                  # 设置显示样式
(gdb) b mysql_execute_command                              # 可以用来查看所有的SQL
(gdb) p thd->lex->select_lex
(gdb) p ((Item_cond*)thd->lex->select_lex->where)->list    # 查看WHERE中的list
(gdb) detach
{% endhighlight %}



## 参考

### Flex/Bison

关于最原始的论文，可以参考 [Lex - A Lexical Analyzer Generator](http://www.cs.utexas.edu/users/novak/lexpaper.htm) ([本地](/reference/databases/mysql/LEX_A_Lexical_Analyzer_Generator.html))，以及 [Yacc: Yet Another Compiler-Compiler](http://www.cs.utexas.edu/users/novak/yaccpaper.htm) ([本地](/reference/databases/mysql/YACC_Yet_Another_Compiler_Compiler.html))。

对于 Lex 和 Yacc 来说，比较经典的入门可以参考 [Lex & Yacc Tutorial](http://epaperpress.com/lexandyacc/index.html)，其中包括了如何编写一个计算器，以及相关的调试等信息；也可以参考 [本地文档](/reference/databases/mysql/LexAndYaccTutorial.pdf)，以及相关的 [源码](/reference/databases/mysql/LexAndYaccCode.zip) 。

关于总体介绍可以参考 [Lex and YACC primer](http://www.tldp.org/HOWTO/Lex-YACC-HOWTO.html)，或者 [本地文档](/reference/databases/mysql/LEX_YACC_Primer_HOWTO.html)，也可以查看中文翻译 [如何使用 Lex/YACC](http://segmentfault.com/blog/icattlecoder/1190000000396608)，以及 [本地](/reference/databases/mysql/LEX_YACC_Primer_HOWTO_cn.mht)，以及 [示例源码](/reference/databases/mysql/lex-yacc-examples.tar.gz) ；以及 [Bison-Flex 笔记](/reference/databases/mysql/Bison_Flex_Notes.mht)、[Flex/Bison Tutorial](/reference/databases/mysql/Tutorial-Flex_Bison.pdf) 。

关于调试方法可以参考 [Understanding Your Parser](http://www.gnu.org/software/bison/manual/html_node/Understanding.html)，这个是 [Bison Offical Documents](http://www.gnu.org/software/bison/manual/html_node/index.html) 文档的一部分；更多可以参考 [dinosaur.compilertools.net](http://dinosaur.compilertools.net/) 查看相关的资料。


<!--
自己动手写编译器
http://pandolia.net/tinyc/
https://bellard.org/tcc/
计算器
http://good-ed.blogspot.tw/2010/04/lexyacc.html
编写自己的编译器
http://coolshell.cn/articles/1547.html

http://blog.csdn.net/huyansoft/article/details/8860224
http://blog.csdn.net/lidan3959/article/details/8237914
http://www.tuicool.com/articles/3aMVzi
http://blog.csdn.net/sfifei/article/details/9449629
-->




{% highlight text %}
{% endhighlight %}
