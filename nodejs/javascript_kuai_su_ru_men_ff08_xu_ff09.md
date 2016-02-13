# JavaScript 快速入門（序）

http://www.kangting.tw/2013/11/javascript.html#jtable

JavaScript 是進入前端開發最重要的基礎技術，也是前端開發領域的唯一程式語言。

工作室於 2013.09 發表的《HTML5從零開始-前端開發完全入門》這本書當中，收錄了一章JavaScript快速入門，希望用最精簡的內容讓讀者完成入門學習，接下來這一系列逐一釋出相關的內容，希望更多技術人員可以藉此進入前端領域發展。



而在你開始學習 JavaScript 之前，可以先參考【JavaScript 入門要項】這篇文章，其中以一張圖表簡單的闡述JavaScript的內容，對於JavaScript技術輪廓的理解將有一些幫助。

JavaScript 是純文字內容，它可以被嵌入網頁當中，或是以獨立的文字檔存在，例如以下的程式碼片段，在 script 標籤中的內容即為 JavaScript 程式碼：


```js
<!DOCTYPE html>
<html >
<head>
    <title></title>
    <script>
        console.log('Hello JavaScript!');
    </script>
</head>
<body>
     // 網頁內容…
</body>
</html>
```

這段程式碼表示當網頁載入到 script 元素，將於控制台輸出指定的訊息，也就是單引號的內容「Hello JavaScript!」。

JavaScript 可以配置於網頁的其它地方，例如網頁的尾部 body 結束標籤之前，由於瀏覽器會由上往下逐步解譯網頁的內容，若是嵌入的JavaScript需要比較長的執行時間或是耗用大量的運算資源，甚至於載入時發生問題，將導致網頁停頓無法繼續往下執行，直到程式執行完畢。為了避免網頁載入的問題，通常建議將 script 元素配置於body 結束標籤之前，讓網頁標籤完全載入之後，再執行JavaScript程式內容，配置如下：

```js
<!DOCTYPE html>
<html >
<head>
    <title></title>
</head>
<body>
    // 網頁內容…
<script>
        console.log('Hello JavaScript!');
    </script>
</body>
</html>
```

以上兩組程式配置最後都將於控制台輸出指定的訊息，第一組JavaScript於網頁載入前執行，第二組則於網頁載入完成後執行。

JavaScript 可以撰寫在獨立的js 檔案裏面，再從網頁外部引用進來，以上述示範的網頁內容為例，建立一個純文字檔，將其中 script 元素內容的JavaScript 語法，移至文字檔，並且儲存成為副檔名為js的檔案，

接下來透過 script 元素將這個檔案的連結，設定給需要引用此JavaScript 內容的網頁如下：


```js
<!DOCTYPE html>
<html >
<head>
    <title></title>
    <script src="jsdemo.js"></script>
</head>
<body>
     // 網頁內容…
</body>
</html>
```

script 標籤的屬性 src 為所要引用的檔案來源，如此一來jsdemo.js 檔案的內容就會被載入目前的網頁當中了，請參考本章範例資料夾中的 jsdemo.js 與jsdemo.html 兩個檔案。
JavaScript 亦支援註解語法如下：

```js
// 以下的程式碼輸出訊息文字　…
console.log('Hello JavaScript!');
```


當瀏覽器開始執行JavaScript ，以 // 符號標示的內容會被視為註解而直接略過，當程式日益複雜，註解可以協助我們釐清程式的內容，善用註解是一名程式設計師必須具備的良好習慣。

具備最基礎的概念，接下來我們就可以開始逐步學習 JavaScript 。


