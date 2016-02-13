# JavaScript 快速入門（5/10）－ 敘述句 if / switch


運算式本身定義了程式的邏輯運算原理，而運算式與各種型式的符號、關鍵字組成敘述句，並且以分號（;）標示為一段敘述句的結束，表示JavaScript程式的一段可執行單位，以底下所討論的運算式為例：

```js
var x,Y,Z ;
x=100 ;
y=200 ;
z=x+y;
```

如你所見，只要加上一個分號即可形成一段合法的敘述句，這裏總共有四段運算式，關鍵字var所建立的變數宣告則最簡單的敘述句。在預設的情形下，程式會從第一行往下逐行執行，直到最後一行執行完畢。 敘述句可能非常複雜，一個以上的敘述句甚至可以結合成一段複合敘述句，而某些敘述句則有特定功能，而通常我們會需要改變逐行執行的程式流程，流程控制敘述句支援相關的運算，這一類的敘述句有三種：條件控制敘述句、迴圈敘述句與跳躍敘述句等三種。

##》條件控制敘述句 - if

if判斷式的語法形成一個獨立的程式區塊，以大括弧規範區塊範圍，如果小括弧中的判斷式結果為true ，則執行其中區塊的內容，否則直接跳過。

```js
if(expression){ 
// expression 為 true 執行這裏的內容　… 
}
```


語法中包含了幾個重要的部份，列舉說明如下：
- if關鍵字構成了判斷式的程式區塊。
- 關鍵字後方小括弧的expression是運算式，它會回傳boolean型態的true/false結果。
- 區塊中的程式碼，根據expression的執行結果來決定是否執行，若是expression的回傳值為true，則進入區塊中執行其中的程式碼，否則省略大括弧的內容程式碼。
考慮以下的語法：


```js
<script>
    var a = 100;
    var b = 200;
    if (a < b) {
        console.log('a<b');
    }
    if (a > b) {
        console.log('a>b');
    }
    console.log('end');
</script>
```


其中宣告了兩個測試變數，並且分別指定了變數值，第一個 if 判斷式檢視 a<b 比較運算。由於 a 的值小於 b ，因此第一個 if 判斷式判斷式a<b 結果為 true ，執行其中的輸出程式碼，第二個 if 判斷式則為 false ，直接跳過大括弧的內容，並執行end訊息輸出。

大括弧中的程式碼如果只有一行，可以省略大括弧符號，例如上述的程式碼可以改寫如下：

```js
if (a < b)
   console.log('a<b');    
```

if 條件式可以另外增加 else 判斷式，這在 if 判斷式結果為false時執行，語法如下：

```js
if (test) {
exp_true ;
} else {
exp_false ;
}    
```



在這個語法中有兩段程式敘述，分別是配置於if大括弧內部的 exp_true 與配置於else大括弧內部的exp_false 。if判斷式test若結果是true，則執行緊接著大括弧內部的程式敘述，否則的話，執行else大括弧，考慮以下的程式片段：

```js
<script>
    var a = 100;
    var b = 200;
    console.log('a:' + a);
    console.log('b:' + b);
    if (a < b) {
        console.log('a<b');
    } else {
        console.log('a>b');
    }
</script>
```


其中一開始宣告兩個變數，並且指定了變數值，接下來的if判斷式，根據 a 與 b的比較結果，輸出相關的訊息。由於 a 的值小於 b ，因此執行if 大括弧中的程式碼。

```js
a:100
b:200
a<b 
```

讀者可以嘗試修改a與b兩個值，當 a 大於或是等於 b 時，即會執行 else 大括弧區段中的程式碼。

##》條件控制敘述句 - switch

當判斷式超過一個以上並且有一定的規則時，可以使用switch語法，以下語法：

```js
switch(n)
{
    case 1:
      // n等於1 執行這個區塊 …
      break;
    case 2:
      // n等於2 執行這個區塊 …
      break;
    default:
      // n不等於上述的值則預設執行這個區塊 …
}
```


switch 後方的小括弧中為比對值，接下來大括弧中的 case 則逐一列舉條件式，若 n 的值與 case 中的比對值相符，則執行其中的程式碼，否則的話持續往下比對，直到所有的 case 比對完畢。最後若是完全沒有符合的結果，則執行 default 中的程式碼。每一個 case 中的 break 敘述是必要的，若是沒有加上 break ，既使某個 case 比對符合，依然會繼續接下來的比對運算。考慮以下的範例程式碼：

```js
<script>
    var month = new Date().getMonth();
    var msg = '';
    switch (month) {
        case 0:
            msg = '一月（JANUARY）'
            break;
        case 1:
            msg = '二月（FEBRUARY)'
            break;
        case 2:
            msg = '三月（MARCH)'
            break;           
       // 4 ~ 10
        case 11:
            msg = '十二月（DECEMBER)'
            break;
    }
    console.log('現在月份：' + msg);
</script>
```


首先取得代表目前月份的數字並儲存於變數 month，接下來的switch 則根據month的變數值，決定所要輸出的訊息。讀者可以自嘗試這個範例，檢視輸出的月份說明。
