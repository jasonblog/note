# 語句


整個編譯器還剩下最後兩個部分：語句和表達式的解析。它們的內容比較多，主要涉及如何將語句和表達式編譯成彙編代碼。這章講解語句的解析，相對於表達式來說它還是較為容易的。

## 語句

C 語言區分“語句”（statement）和“表達式”（expression）兩個概念。簡單地說，可以認為語句就是表達式加上末尾的分號。

在我們的編譯器中共識別 6 種語句：

- if (...) <statement> [else <statement>]
- while (...) <statement>
- { <statement> }
- return xxx;
- <empty statement>;
- expression; (expression end with semicolon)

它們的語法分析都相對容易，重要的是去理解如何將這些語句編譯成彙編代碼，下面我們逐一解釋。

##IF 語句

IF 語句的作用是跳轉，跟據條件表達式決定跳轉的位置。我們看看下面的偽代碼：

```c
if (...) <statement> [else <statement>]

  if (<cond>)                   <cond>
                                JZ a
    <true_statement>   ===>     <true_statement>
  else:                         JMP b
a:                           a:
    <false_statement>           <false_statement>
b:                           b:
```


對應的彙編代碼流程為：

- 執行條件表達式 <cond>。
- 如果條件失敗，則跳轉到 a 的位置，執行 else 語句。這裡 else 語句是可以省略的，此時 a 和 b 都指向 IF 語句後方的代碼。
- 因為彙編代碼是順序排列的，所以如果執行了 true_statement，為了防止因為順序排列而執行了 false_statement，所以需要無條件跳轉 JMP b。


對應的 C 代碼如下：

```c
if (token == If) {
    match(If);
    match('(');
    expression(Assign);  // parse condition
    match(')');

    *++text = JZ;
    b = ++text;

    statement();         // parse statement
    if (token == Else) { // parse else
        match(Else);

        // emit code for JMP B
        *b = (int)(text + 3);
        *++text = JMP;
        b = ++text;

        statement();
    }

    *b = (int)(text + 1);
}
```

## While 語句

While 語句比 If 語句簡單，它對應的彙編代碼如下：

```c
a:                     a:
   while (<cond>)        <cond>
                         JZ b
    <statement>          <statement>
                         JMP a
b:                     b:
```

沒有什麼值得說明的內容，它的 C 代碼如下：

```c
else if (token == While) {
    match(While);

    a = text + 1;

    match('(');
    expression(Assign);
    match(')');

    *++text = JZ;
    b = ++text;

    statement();

    *++text = JMP;
    *++text = (int)a;
    *b = (int)(text + 1);
}
```

## Return 語句

Return 唯一特殊的地方是：一旦遇到了 Return 語句，則意味著函數要退出了，所以需要生成彙編代碼 LEV 來表示退出。

```c
else if (token == Return) {
    // return [expression];
    match(Return);

    if (token != ';') {
        expression(Assign);
    }

    match(';');

    // emit code for return
    *++text = LEV;
}
```

##其它語句

其它語句並不直接生成彙編代碼，所以不多做說明，代碼如下：

```c
else if (token == '{') {
    // { <statement> ... }
    match('{');

    while (token != '}') {
        statement();
    }

    match('}');
}
else if (token == ';') {
    // empty statement
    match(';');
}
else {
    // a = b; or function_call();
    expression(Assign);
    match(';');
}
```

##代碼

本章的代碼可以在 Github 上下載，也可以直接 clone
```c
git clone -b step-5 https://github.com/lotabout/write-a-C-interpreter
```

本章的代碼依舊無法運行，還剩最後一部分沒有完成：expression。

##小結

本章講解了如何將語句編譯成彙編代碼，內容相對容易一些，關鍵就是去理解彙編代碼的執行原理。

同時值得一提的是，編譯器的語法分析部分其實是很簡單的，而真正的難點是如何在語法分析時收集足夠多的信息，最終把源代碼轉換成目標代碼（彙編）。我認為這也是初學者實現編譯器的一大難點，往往比詞法分析/語法分析更困難。

所以建議如果沒有學過彙編，可以學習學習，它本身不難，但對理解計算機的原理有很大幫助。