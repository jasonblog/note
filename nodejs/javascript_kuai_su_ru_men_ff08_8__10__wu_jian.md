# JavaScript 快速入門（8/10）－ 物件


在 JavaScript 裏面，一個基礎型別是一個數、布林值（true/false）或字串，而物件可能是一個空值或是一群屬性（property）組合而成的複雜型態資料。最簡單的物件可以是一個空值，你可以建立一個物件如下：

```js
var o = {} ; 
```

大括弧代表一個物件，由於完全沒有內容，因此是一個空的物件，以下則是一個典型的物件：

```js
var o = {x:10,y:20} ;
```

物件內容由兩組 key:value 的資料所組成，這兩組資料以逗點分隔，而你可以經由以下的語法，藉由 o 引用 key 取得其對應的 value 。

```js
o.x    // 回傳 10
o.y    // 回傳 20
```

如你所見，o是一個物件，不同於基本型別資料，它是由數量不等的資料組成的複合值，由於一組資料中，每一個key均為物件的一個特定屬性，value則為其值。假設要建立一個表示特定書籍資料的物件，可以表示如下：

```js
var book = {
    "title": "HTML5從零開始",
    "author": "呂高旭",
    "price": 650
}
```

變數 book 是一個物件，內容描述一本書籍的資料，其中包含三組以逗點分隔的資料，每一組資料代表一個屬性，內容由分號（:）隔開，左邊是屬性名稱，右邊是對應的值，當你要取得其中任何一項資料，例如書名，只要透過以下的語法即可：

```js
book.title 
```

這一行將回傳字串「HTML5從零開始」，其它的資料類推。除此之外，你也可以針對其中特定的屬性項目重設其屬性值，例如：

```js
book.title= "HTML5完美風暴" ;
```


這一行程式碼將一段新的字串設定給 title 屬性。瞭解最簡單的物件之後，現在我們進一步來看它與基本型別的差異，考慮以下兩個變數資料：

```js
var a = 100 ;             // 基本型別
var b = {x:100,y:200} ;   // 物件型別
```
第一行的 a 是數字，這是基本型別，它的值是儲存至變數的固定值，不會因為其它外部因素而改變，第二行的b是物件，它的值來自參考，當其它物件參考同一個值，並且改變了這個值，則原來的物件值也會跟著被改變。考慮以下的程式片段：

```js
<script>
    var x = 100;
    var o = { title: 'HTML5' };
    var y = x;
    var a = o;

    a.title = 'JavaScript';
    y = 200;

    document.write('o.title：' + o.title);
    document.write('<br/>');
    document.write('x：' + x);
</script>
```

變數 x 的值被指定給 y ，而變數 o 的值則指定給變數 a ，接下來調整 a 與 y 的值，最後輸出 o 與 x 的值，結果如下：

```js
o.title：JavaScript
x：100
```

由於 x 的數值是基本型別，因此它無法被改變，所以輸出結果依然是原來的 100 ，而 o 本身為一參考物件，它的 title 屬性值透過 a 修改了，因此輸出的結果是新值「JavaScript」。

##》函式物件

JavaScript函式本身可以被當作物件值進行處理，將其指定給某個特定的物件屬性，當作其屬性值，例如以下的語法：

```js
var obj = {x:f};
```

其中 f 是預先定義的函式，接下來便能如下調用此函式：

```js
obj.x() ; 
```
更進一步的，我們可以將函式主體直接指定給屬性，如下式：

```js
var obj = {x:function(){ // 函式內容}};
```

同樣的，透過 obj.x() 即可引用此函式，考慮以下的程式片段：

```js
function w(message) { console.log(message) };
var obj = {
    msg: function () { console.log('Hello JavaScript !') },
    hello: function (message) { console.log(message) },
    welcome: w,
};
obj.msg();
obj.hello('HELLO');
obj.welcome('WELCOME');
```


其中以三種方式分別將各種不同的函式指定給物件屬性，最後經用屬性調用相關的函式將結果訊息輸出，結果如下：

```js
Hello JavaScript !
HELLO
WELCOME
```

我們可以將函式當作值儲存至物件屬性的原因，在於函式本身亦是一個物件，你甚至可以將函式設定給一個普通的變數，例如以下的程式片段：

```js
var x=f ; 
```
其中 x 是一個變數，而 f 則是預先定義的函式，接下來你可以透過變數 x 調用此函式例如 x() ，來看另外一段程式碼：


```js
function add(x, y) {
    return x + y;
}
var a = add;
a(100,200);
```


最後調用 a 輸出結果為 300 。如你所見，函式本身可以當作一個值來處理，而它自己也能建立自己專屬的屬性，這對於某些專屬的區域變數設計特別的有用，先來看相關作法：

```js
f.message = pvalue ;
function f() { };
```

其中的 pvalue 是一個特定的值，例如字串、數字，甚至其它的物件等等，一旦你這麼作，這個屬性就只會屬於此函數 f 。接下來我們便能透過 f 存取 message 這個屬性值，來看一段範例程式碼如下：

```js
f.message = 'Hello';
function f() { };
var a = f;
a.message += ' JavaScript !';
console.log(a.message);
```


第1行將字串 Hello 設定給函式 f 的 message 屬性，接下來定義 f 函式主體，將 f 設定給變數 a ，如此一來， a 參照至 f 函式， a.message 則會取得 Hello 這個字串，因此將其與另外一個 JavaScrtip 字串合併，以下為輸出結果：

```js
Hello JavaScript !
```