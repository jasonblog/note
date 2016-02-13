# JavaScript 快速入門（3/10）－ 數字/ 字串/ 布林值


這一篇很快的說明 JavaScript 三種基礎的基別。

##》數字

JavaScript以具小數點的數字表示所有的數值資料，以下均是合法的數值寫法。

```js
100
0.25
.123
1.23E6
```


以指數符號（e或是E）來表示一個數值亦合法。JavaScript中可表示的數值範圍最大值為 1.7976931348623157e+308，最小值則是5e-324，你可以透過Number物件的屬性來取得這兩個值，列舉如下表：

<table border="1" cellpadding="0" cellspacing="0" class="MsoTableGrid" style="border-collapse: collapse; border: none; mso-border-alt: solid windowtext .5pt; mso-padding-alt: 0cm 5.4pt 0cm 5.4pt; mso-yfti-tbllook: 1184;"><tbody>
<tr>   <td style="border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 97.55pt;" valign="top" width="130"><div class="a"><b><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">最大與最小值</span><span lang="EN-US"><o:p></o:p></span></b></div></td>   <td style="border-left: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 127.6pt;" valign="top" width="170"><div class="a"><b><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">語法</span><span lang="EN-US"><o:p></o:p></span></b></div></td>   <td style="border-left: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 202.9pt;" valign="top" width="271"><div class="a"><b><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">數字</span><span lang="EN-US"><o:p></o:p></span></b></div></td>  </tr>
<tr>   <td style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 97.55pt;" valign="top" width="130"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">最大值</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 127.6pt;" valign="top" width="170"><div class="a"><span lang="EN-US">Number.</span><span lang="EN-US" style="background-color: white; background-position: initial initial; background-repeat: initial initial; font-family: 'Courier New'; font-size: 10pt;">MAX_VALUE</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 202.9pt;" valign="top" width="271"><div class="a"><span lang="EN-US">1.7976931348623157e+308</span></div></td>  </tr>
<tr>   <td style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 97.55pt;" valign="top" width="130"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">最小值</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 127.6pt;" valign="top" width="170"><div class="a"><span lang="EN-US">Number</span><span lang="EN-US">.</span><span lang="EN-US" style="background-color: white; font-family: 'Courier New'; font-size: 10pt;">MIN_VALUE</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 202.9pt;" valign="top" width="271"><div class="a"><span lang="EN-US">5e-324</span></div></td>  </tr>
</tbody></table>

來看以下的程式碼：
```js
<script>
    var x = 100;
    var y = 1.02;
    var z = 0.12;
    var a = .135246;
    var b = 2.123e6;
    var c = 2.123E6;
    var d = 4.1e-6;
    //
    console.log(x);
    console.log(y);
    console.log(z);
    console.log(a);
    console.log(b);
    console.log(c);
//
console.log('最大可能值：' + Number.MAX_VALUE);
console.log('最小可能值：' + Number.MIN_VALUE);
</script>
```


其中的程式碼宣告了數個變數，並且設定了數種不同格式的數值，最後輸出指定的值，讀者請自比對以下的結果：

```js
100
1.02
0.12
0.135246
2123000
0.0000041
最大可能值：1.7976931348623157e+308
最小可能值：5e-324  
```


最大與最小值透過 Number 物件引用 MAX_VALUE 與 MIN_VALUE 取得。

關於數值資料，另外有兩個相關的屬性必須特別注意，NaN 表示「不是一個數字」的意思，可以用函式 isNaN() 判斷一個值是否為數字，考慮以下的程式碼：

```js
<script>
    var s = 'HTML5';
    var x = 100;
    console.log(isNaN(s));
    console.log(isNaN(x));
</script>
```


第一行與第二行定義字串與數值資料變數，緊接著調用isNaN() 判斷是否這些變數「不是一個數字」，針對 s 變數，由於它是一個字串，所以回傳 true ，而 x 是數值，因此回傳 false 。另外一個變數是 Infinity，這個變數表示一個正無限值的結果，如果是負無限值，則以負號表示為 –Infinity ，以上述的 x 為例，它是一個 100 的值，如果將其除以 0 ，將回傳 Infinity 。

##》字串

字串型別用以表示文字資料，以 UTF-16 編碼Uniode 字元表示，在JavaScript中表示字串必須以單引號或是雙引號包含所要表示的字元，例如以下均表示字串：

```js
var s0 = "HTML5";
var s1 = 'JavaScript';
var s2 = '';
```

無論單引號或是雙引號都必須成對，若其中沒有任何字元則為空字串。但是你可以將成對的單引號包在成對的雙引號裏面，形成巢狀式的結構，反之亦同，例如以下的字串：

```js
var a ="HTML5='JavaScript'+'HTML'+'CSS'" ;
var b ='HTML5="JavaScript"+"HTML"+"CSS"' ;
```

這兩行均是合法的字串，依序將a與b輸出會得到以下的結果：

```js
HTML5='JavaScript'+'HTML'+'CSS'
HTML5="JavaScript"+"HTML"+"CSS"
```

使用字串資料必須特別注意特定的字元，例如斷行，或是必須將引號視為單純的字元時，可以使用跳脫字元作表示，這是一個倒斜線（\），例如以下這一行：

```js
var s ='HTML5=\nJavaScript+HTML+CSS'
```

其中的 s 變數儲存等號後方的字串，此字串內包夾的 \n 表示一個斷行符號，如果於控制台輸出s會得到以下的結果：

```js
HTML5=
JavaScript+HTML+CSS
```


考慮以下另外一個字串宣告：

```js
var a ='HTML5='JavaScript'+'HTML'+'CSS''
```

其中的子字串，包含 'JavaScript' 、'CSS ' 與 'JavaScript' ，與外部字串同樣使用單引號，如此一來將導致衝突，若不以雙引號區隔標示，可以使用跳脫字元標示如下：

```js
var a ='HTML5=\'JavaScript\'+\'HTML\'+\'CSS\''
```

將其中的 a 輸出，會得到以下的結果：

```js
HTML5='JavaScript'+'HTML'+'CSS'
```


其它有數個特定的字元可以透過跳脫序列作表示。除了跳脫序列，一旦以單引號或是雙引號包覆便會被視為字串，既使其中包含了數字亦同，例如以下這一行程式碼：

```js
var a = '100';
```

其中 a 所儲存的 100 是字串，會以字串格式進行程式運算。

##》布林值

布林型態只有兩個可能的值，由關鍵字 true 與 false 表示，通常運用於比較運算，例如以下這一行程式碼：

```js
x==12
```

其中的 x 變數如果是 12 ，則這一行程式碼將回傳 true ，否則為 false 。你可以將任何值轉換成布林值，例如 0 或 null 轉換為布林值為 false ，而其它的數值轉換為布林值則為 true 。最常使用布林值的場合是各種敘述控制句，例如 if/else 。

瞭解資料型態之後，接下來還有 typeof() 與另外兩種特定的資料，以下進行很速的討論。

##》typeof()

你可以透過 typeof 檢視資料型別，例如 typeof(100) 將回傳 number ，以下的範例分別測試數種不同型別的資料。

```js
<script>
    var n0 = typeof (101);
    var n1 = typeof (1.01);
    var n2 = typeof ('101');
    var n3 = typeof (true);
    console.log(n0);
    console.log(n1);
    console.log(n2);
    console.log(n3);
</script>
```

其中透過調用 typeof() 檢視數種資料型別，並回傳結果。

```js
number
number
string
boolean 
```

##》特殊值－null 與 undefined

null 與 undefined都表示值不存在，考慮以下的程式碼：

```js
console.log(typeof(null));
console.log(typeof(undefined));
```

如果調用 typeof() 直接輸出兩者結果如下：

```js
object
undefined
```

null 是一個物件（object），而 undefined 則是一個值。但這兩者在意義上並沒有太大的差別，通常在程式當中如果需要指定一個不存在的空值，使用 null 即可，例如：

```js
var a = null ;
```

至於 undefined ，通常會在程式過程中發生非預期或空值錯誤時出現。
