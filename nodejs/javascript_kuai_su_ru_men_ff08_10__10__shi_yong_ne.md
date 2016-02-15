# JavaScript 快速入門（10/10）－ 使用內建物件


JavaScript內建大量的程式物件，對於這一類的物件，我們可以透過 new 關鍵字取得，然後進行相關方法與屬性的引用以建立所需的應用程式功能。

##》Date－日期時間資訊

Date 物件包裝日期相關資料，支援日期資訊的取得，考慮以下的程式碼，建立一個 Data 物件 ：

```js
var d = new Date();
```

接下來透過調用特定的方法，即可取得各種不同型式的日期資訊。

```js
d.toLocaleString();         // 2013年2月4日 下午8:27:47
d.toLocaleDateString();    // 2013年2月4日
d.toLocaleTimeString();    // 下午8:27:47
```
以上列舉的三種方法可以分別用來取出完整日期時間資訊，或是僅取得日期、時間等特定資訊。你也可以指定一個特定的日期資訊以初始化 Date 物件，語法格式如下：

```js
new Date(ms) // 1970/01/01 開始至指定時間的總毫秒數（milliseconds）
new Date(dateString)
new Date(year, month, day, hours, minutes, seconds, ms)
```

第一種格式接受整數參數 ms ，表示從指定時間到 1970/01/01 之間所經過的時間長度，以毫秒為單位的整數。接下來第二種格式則以表示某一天的日期字串為參數，第三種格式則分別填入代表各日期時間單位的數字。

```js
var d1 = new Date("October 16, 1995 10:21:00")
var d2 = new Date(95,6,24)
var d3 = new Date(89,11,22,10,33,0)
```

以上三組Date物件宣告均合法。除此之外，Date物件還提供其它一系列的方法成員，支援日期時間的操作，這些方法可以概略方為getxxx 與 setxxx ，前者用以取得日期時間的各單位值，包含年、月、日、星期與時、分、秒，甚至以毫秒錶示的特定日期時間等等，後者則用以設定某個日期時間。

```js
getxxx
```
取得日期時間中的某個特定單位，包含年、月、日、時、分、秒等等，例如 getFullYear()取得表示年的數字，getTime() 表示取得至 1970.1.1 以來的時間毫秒數。

```js
setxxx
```
設定一個特定的日期時間，包含年、月、日、時、分、秒等等，例如 setFullYear() 設定其中的年，而 setTime() 以毫秒數表示設定時間。

##》Math－數學運算

當你要在程式中執行特定的數學運算，例如三角函數、對數或是平方、甚至取得亂數等等，都可以透過 Math 成員的引用執行來取得，包含幾個特定常數與靜態方法函式。Math提供了數種常數屬性，直接引用可以取得特定的數學值，例如 Math.PI回傳表示圓周率的常數值。

除此之外Math亦提供與數學有關的方法成員，支援包含三角函式、絕對值、近似值的運算等等。

##》String－字串與數字處理

String物件支援字串的處理，提供如合併、切割與大小寫轉換的字串處理功能，數字的處理比較單純，你可以透過 Number 物件的屬性以及方法，來取得某些特定的數字，例如JavaScript 的最大可能值，或是將數字轉換成以科學表示法表示的字串。

##》全域物件與函式

JavaScript有一個預先定義的全域物件，提供各種屬性與函式成員，支援各種基本的程式功能，由於它位於整個 JavaScript程式碼的最頂層，因此不需要可以直接引用物件的屬性或方法函式，例如以下的程式片段：

```js
isNaN('ABC')   // 回傳true
isNaN(true)    // 回傳false
isNaN(123)     // 回傳false
```

其中 isNaN測試傳入的參數是否為一個非數字，回傳true表示其為非數字，false表示數字，由於這是一個全域物件的內建函式，因此JavaScript可以直接調用，例如parseInt()支援字串轉換為數字的功能，而isNaN()　則提供測試某個字值是否為非數字的判斷。下表列舉可用的全域函式。


<table border="1" cellpadding="0" cellspacing="0" class="MsoNormalTable" style="border-collapse: collapse; border: none; mso-border-alt: solid windowtext .5pt; mso-border-insideh: .5pt solid windowtext; mso-border-insidev: .5pt solid windowtext; mso-padding-alt: 0cm 5.4pt 0cm 5.4pt; mso-yfti-tbllook: 1184;"><tbody>
<tr>   <td style="border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 62.1pt;" valign="top" width="83"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">類型</span></div></td>   <td style="border-left: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 92.15pt;" valign="top" width="123"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">函式</span></div></td>   <td style="border-left: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 281.8pt;" valign="top" width="376"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">說明</span></div></td>  </tr>
<tr>   <td rowspan="2" style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 62.1pt;" valign="top" width="83"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">數值特性</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 92.15pt;" valign="top" width="123"><div class="a"><span lang="EN-US">isFinite()</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 281.8pt;" valign="top" width="376"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">檢視一個數值是否為有限數值。</span></div></td>  </tr>
<tr>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 92.15pt;" valign="top" width="123"><div class="a"><span lang="EN-US">isNaN()</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 281.8pt;" valign="top" width="376"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">檢視一個數值是否為為非數字。</span></div></td>  </tr>
<tr>   <td rowspan="4" style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 62.1pt;" valign="top" width="83"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">編碼</span><span lang="EN-US">/</span><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">解碼</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 92.15pt;" valign="top" width="123"><div class="a"><span lang="EN-US">decodeURI()</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 281.8pt;" valign="top" width="376"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">反轉</span><span lang="EN-US"> escape() </span><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">轉換後的</span></div></td>  </tr>
<tr>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 92.15pt;" valign="top" width="123"><div class="a"><span lang="EN-US">encodeURI()</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 281.8pt;" valign="top" width="376"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">將一段</span><span lang="EN-US">URI</span><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">中某些特定字元轉換為跳脫字元。</span></div></td>  </tr>
<tr>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 92.15pt;" valign="top" width="123"><div class="a"><span lang="EN-US">escape()</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 281.8pt;" valign="top" width="376"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">將一段字串中某些特定字元轉換為跳脫字元。</span></div></td>  </tr>
<tr>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 92.15pt;" valign="top" width="123"><div class="a"><span lang="EN-US">unescape()</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 281.8pt;" valign="top" width="376"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">反轉</span><span lang="EN-US"> escape() </span><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">轉換後的字串。</span></div></td>  </tr>
<tr>   <td rowspan="2" style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 62.1pt;" valign="top" width="83"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">數值轉換</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 92.15pt;" valign="top" width="123"><div class="a"><span lang="EN-US">parseFloat()</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 281.8pt;" valign="top" width="376"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">將字串格式的數值轉換為浮點數。</span></div></td>  </tr>
<tr>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 92.15pt;" valign="top" width="123"><div class="a"><span lang="EN-US">parseInt()</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 281.8pt;" valign="top" width="376"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">將字串格式的數值轉換為整數。</span></div></td>  </tr>
<tr>   <td style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 62.1pt;" valign="top" width="83"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">其它</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 92.15pt;" valign="top" width="123"><div class="a"><span lang="EN-US">eval()</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 281.8pt;" valign="top" width="376"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">將一段字串轉換為</span><span lang="EN-US">JavaScript</span><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">進行運算。</span></div></td>  </tr>
</tbody></table>

除了回傳特定JavaScript的全域屬性，另外還有數個代表特定值的全域屬性，如下表：

<table border="1" cellpadding="0" cellspacing="0" class="MsoTableGrid" style="border-collapse: collapse; border: none; mso-border-alt: solid windowtext .5pt; mso-padding-alt: 0cm 5.4pt 0cm 5.4pt; mso-yfti-tbllook: 1184;"><tbody>
<tr>   <td style="border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 119.55pt;" valign="top" width="159"><div class="a" style="margin-bottom: 6.0pt; margin-left: 0cm; margin-right: 0cm; margin-top: 6.0pt;"><b><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">全域屬性</span><span lang="EN-US"><o:p></o:p></span></b></div></td>   <td style="border-left: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 316.5pt;" valign="top" width="422"><div class="a"><b><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">說明</span><span lang="EN-US"><o:p></o:p></span></b></div></td>  </tr>
<tr>   <td style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 119.55pt;" valign="top" width="159"><div class="a"><span lang="EN-US">Infinity</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 316.5pt;" valign="top" width="422"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">表示一個正無限大的數值。</span></div></td>  </tr>
<tr>   <td style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 119.55pt;" valign="top" width="159"><div class="a"><span lang="EN-US">NaN</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 316.5pt;" valign="top" width="422"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">表示一個非數字的值。</span></div></td>  </tr>
<tr>   <td style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 119.55pt;" valign="top" width="159"><div class="a"><span lang="EN-US">undefined</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 316.5pt;" valign="top" width="422"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">表示一個</span><span lang="EN-US"> undefined </span><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">的值。</span></div></td>  </tr>
</tbody></table>

同樣的，表列的屬性只需直接引用即可取得其表示的值，稍早討論型別已針對此三種屬性進行了相關的說明。你也可以透過關鍵字this取得全域物件，例如以下這一行程式碼：

```js
var g = this  ; 
```
以下的程式碼透過不同的方式調用全域物件的方法，進行字串與數字的轉換。

```js
    <script>
        var a = '100';
        var b = '200.02';
        var sum1 = a + b;
        console.log(sum1);
        var sum2 = parseInt(a) + parseFloat(b);
        console.log(sum2);
        var sum3 = this.parseInt(a) + this.parseFloat(b);
        console.log(sum3);
        var g = this;
        var sum4 = g.parseInt(a) + g.parseFloat(b);
        console.log(sum4);
    </script>
```

變數 a 與 b  分別指定字串內容，並且利用+運算子將其合併輸出，由於是字串，因此輸出結果是兩個數字的合併而非加總。接下來調用全域函式parseInt() 與 parseFloat() ，將兩個變數轉換為對應的數值然後重新以+運算子進行加總輸出。其中的程式碼分別以不同的方式調用全域函式，首先是直接調用，接下來是以 this 關鍵字調用，接下來則是透過 this 關鍵用取得全域物件的參照變數 g ，然後透過 g 進行調用。

```js
100200.02

300.02
300.02
300.02
```

讀者可以從輸出結果發現，不同的調用方式效果相同。而對於瀏覽而言，關鍵字this事實上即為Window 物件，直接透過 window 引用意思相同，考慮以下的程式碼：
```js
var sum5 = window.parseInt(a) + window.parseFloat(b);
```

其中以 window取代this 並得到相同的結果。

##》Window物件

Window是JavaScript最重要的全域物件，支援瀏覽器視窗的操作功能，在 JavaScript 中，Window 物件可以透過 window 屬性取得，因此直接引用即可，由於它是最頂層的物件，除了內建的功能成員－例如前述的parseInt()可以直接引用之外，網頁中動態建立的成員，包含全域變數與各種函式，同樣可以透過此物件進行存取。


```js
<script>
        var g = 'ABCDE';
        function showMsg() {
            alert('showMsg輸出:'+g);
        }
        console.log(g);
        console.log(this.g);
        console.log(window.g);
        window.showMsg();
</script>
```
一開始宣告一個變數 g ，同時定義一個函式 showMsg() ，兩者均成為全域物件的成員，因此接下來無論直接引用、透過 this 或是 window 均能存取這些自訂的成員。全域物件可以透過this關鍵字或是window屬性進行存取，window是全域物件 Window的一個屬性，除此之外，還有其它屬性支援更多的功能。

## 》螢幕大小－Screen 物件

window.screen 回傳一個 Screen 物件，透過此物件調用相關的屬性，我們可以取得螢幕大小與像素位元等相關資訊，下表列舉相關的資訊：


<table border="1" cellpadding="0" cellspacing="0" class="MsoTableGrid" style="border-collapse: collapse; border: none; mso-border-alt: solid windowtext .5pt; mso-padding-alt: 0cm 5.4pt 0cm 5.4pt; mso-yfti-tbllook: 1184;"><tbody>
<tr>   <td style="border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 62.1pt;" valign="top" width="83"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">屬性</span></div></td>   <td style="border-left: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 4.0cm;" valign="top" width="151"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">說明</span></div></td>   <td style="border-left: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 70.9pt;" valign="top" width="95"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">屬性</span></div></td>   <td style="border-left: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 181.65pt;" valign="top" width="242"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">說明</span></div></td>  </tr>
<tr>   <td style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 62.1pt;" valign="top" width="83"><div class="a"><span lang="EN-US">width</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 4.0cm;" valign="top" width="151"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">螢幕寬度</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 70.9pt;" valign="top" width="95"><div class="a"><span lang="EN-US">availWidth</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 181.65pt;" valign="top" width="242"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">扣除工具列等實際可用螢幕寬度。</span></div></td>  </tr>
<tr>   <td style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 62.1pt;" valign="top" width="83"><div class="a"><span lang="EN-US">height</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 4.0cm;" valign="top" width="151"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">螢幕高度</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 70.9pt;" valign="top" width="95"><div class="a"><span lang="EN-US">availHeight</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 181.65pt;" valign="top" width="242"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">扣除工具列等實際可用螢幕高度。</span></div></td>  </tr>
<tr>   <td style="border-top: none; border: solid windowtext 1.0pt; mso-border-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 62.1pt;" valign="top" width="83"><div class="a"><span lang="EN-US">colorDepth</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 4.0cm;" valign="top" width="151"><div class="a"><span style="font-family: &quot;新細明體&quot;,&quot;serif&quot;; mso-ascii-font-family: &quot;Times New Roman&quot;; mso-hansi-font-family: &quot;Times New Roman&quot;;">螢幕每個像素的位元</span></div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 70.9pt;" valign="top" width="95"><div class="a"><br>
</div></td>   <td style="border-bottom: solid windowtext 1.0pt; border-left: none; border-right: solid windowtext 1.0pt; border-top: none; mso-border-alt: solid windowtext .5pt; mso-border-left-alt: solid windowtext .5pt; mso-border-top-alt: solid windowtext .5pt; padding: 0cm 5.4pt 0cm 5.4pt; width: 181.65pt;" valign="top" width="242"><div class="a"><br>
</div></td>  </tr>
</tbody></table>

考慮以下的程式片段：

```js
<script>
        document.writeln(
            '寬度：' + screen.width + '/' +
            '高度：' + screen.height + '<br/>');
        document.writeln(
        '可用寬度：' + screen.availWidth + '/' +
        '可用高度：' + screen.availHeight + '<br/>');
        document.writeln('位元像素：' + screen.colorDepth);
</script>
```

其中逐項引用上述的表列屬性，最後輸出結果如下：

```js
寬度：1680/高度：1050
可用寬度：1680/可用高度：1010
位元像素：32
```



