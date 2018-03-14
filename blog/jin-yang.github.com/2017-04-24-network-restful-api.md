---
title: RESTful
layout: post
comments: true
language: chinese
category: [network,linux]
keywords: hello world,示例,sample,markdown
description: 簡單記錄一下一些與 Markdown 相關的內容，包括了一些使用模版。
---

RESTful 架構是一種互聯網軟件架構，它結構清晰、符合標準、易於理解、擴展方便，所以正得到越來越多網站的採用。

<!-- more -->

<!--
出來好多文章都是說怎麼在RESTful Uri裡使用動詞等等，這些人除了一部分是把文章拿來抄一抄的，其他的其實搞混了REST和RPC的概念了，REST強調資源，RPC強調動作，所以REST的Uri組成為名詞，RPC多為動詞短語。然而它們也並不是完全不相關的兩個東西，本文中的實現就會參考一部分JSON-RPC的設計思想。
-->

![restful api logo]({{ site.url }}/images/network/restful-api-logo.gif "restful api logo"){: .pull-center }

## 簡介

REST 這個詞是 [Roy Thomas Fielding](https://en.wikipedia.org/wiki/Roy_Fielding) 在 2000 年的 [博士論文](http://www.ics.uci.edu/~fielding/pubs/dissertation/top.htm) 中提出，也就是他對互聯網軟件的架構原則，全稱為 Representational State Transfer 。

他在論文中提到："我這篇文章的寫作目的，就是想在符合架構原理的前提下，理解和評估以網絡為基礎的應用軟件的架構設計，得到一個功能強、性能好、適宜通信的架構。REST 指的是一組架構約束條件和原則。" 如果一個架構符合 REST 的約束條件和原則，我們就稱它為 RESTful 架構。

要理解 RESTful 架構，最好的方法就是去理解 `Representational State Transfer` 這個詞組所代表的具體含義，進而可以體會 REST 是一種什麼樣的設計。

### 資源, Resources

REST 中其實省略了主語 "資源"，這也就是網絡上的一個事物，可以是實體，如手機號、文本、圖片；也可以只是一個抽象概念，如服務。

可以用一個 URI（統一資源定位符）指向它，每種資源對應一個特定的 URI，URI 是每個資源的地址的標示符。

### 表現層, Representation

"資源" 是一種信息實體，它可以有多種外在表現形式，把 "資源" 具體呈現出來的形式，叫做它的 "表現層"。

比如，文本可以用 txt 格式、HTML 格式、XML 格式、JSON 格式表現，甚至可以採用二進制格式；圖片可以用 JPG 格式表現，也可以用 PNG 格式表現。

URI 只代表資源的實體，不代表它的形式，嚴格地說，有些網址最後的 `.html` 後綴名是不必要的，因為這個後綴名錶示格式，屬於表現層範疇，而 URI 應該只代表資源的位置；它的具體表現形式，應該在 HTTP 請求的頭信息中用 `Accept` 和 `Content-Type` 字段指定。

### 狀態轉化, State Transfer

訪問一個網站，就代表了客戶端和服務器的一個互動過程，在這個過程中，就會涉及到數據和狀態的變化。

HTTP 協議是一個無狀態協議，這也就意味著，所有的狀態都保存在服務器端；因此，如果客戶端想要操作服務器，必須通過某種手段，讓服務器端發生"狀態轉化"，而這種轉化是建立在表現層之上的，所以就是"表現層狀態轉化"。

具體來說，就是通過 HTTP 協議裡面的四個表示操作方式的動詞：GET (獲取資源)，POST (新建資源或者更新資源)，PUT (更新資源)，DELETE (刪除資源)。





<!--
安全性與冪等性

關於HTTP請求採用的這些個方法，具有兩個基本的特性，即“安全性”和“冪等性”。對於上述7種HTTP方法，GET、HEAD和OPTIONS均被認為是安全的方法，因為它們旨在實現對數據的獲取，並不具有“邊界效應（Side Effect[1]）”。至於其它4個HTTP方法，由於它們會導致服務端資源的變化，所以被認為是不安全的方法。

冪等性（Idempotent）是一個數學上的概念，在這裡表示發送一次和多次請求引起的邊界效應是一致的。在網速不夠快的情況下，客戶端發送一個請求後不能立即得到響應，由於不能確定是否請求是否被成功提交，所以它有可能會再次發送另一個相同的請求，冪等性決定了第二個請求是否有效。

上述3種安全的HTTP方法（GET、HEAD和OPTIONS）均是冪等方法。由於DELETE和PATCH請求操作的是現有的某個資源，所以它們是冪等方法。對於PUT請求，只有在對應資源不存在的情況下服務器才會進行添加操作，否則只作修改操作，所以它也是冪等方法。至於最後一種POST，由於它總是進行添加操作，如果服務器接收到兩次相同的POST操作，將導致兩個相同的資源被創建，所以這是一個非冪等的方法。

當我們在設計Web API的時候，應該儘量根據請求HTTP方法的冪等型來決定處理的邏輯。由於PUT是一個冪等方法，所以攜帶相同資源的PUT請求不應該引起資源的狀態變化，如果我們在資源上附加一個自增長的計數器表示被修改的次數，這實際上就破壞了冪等型。

不過就我個人的觀點來說，在有的場合下針對冪等型要求可以不需要那麼嚴格。舉個例子，我對於我們開發的發部分應用來說，數據表基本上都有一個名為LastUpdatedTime的字段表示記錄最後一次被修改的時間，因為這是為了數據安全審核（Auditing）的需要。在這種情況下，如果接收到一個基於數據修改的PUT請求，我們總是會用提交數據去覆蓋現有的數據，並將當前服務端時間（客戶端時間不可靠）作為字段LastUpdatedTime的值，這實際上也破壞了冪等性。

可能有人說我們可以在真正修改數據之前檢查提交的數據是否與現有數據一致，但是在涉及多個錶鏈接的時候這個“預檢”操作會帶來性能損失，而且針對每個字段的逐一比較也是一個很繁瑣的事情，所以我們一般不作這樣的預檢操作。

七、無狀態性

RESTful只要維護資源的狀態，而不需要維護客戶端的狀態。對於它來說，每次請求都是全新的，它只需要針對本次請求作相應的操作，不需要將本次請求的相關信息記錄下來以便用於後續來自相同客戶端請求的處理。

對於上面我們介紹的RESTful的這些個特性，它們都是要求我們為了滿足這些特徵做點什麼，唯有這個無狀態卻是要求我們不要做什麼，因為HTTP本身就是無狀態的。舉個例子，一個網頁通過調用Web API分頁獲取符合查詢條件的記錄。一般情況下，頁面導航均具有“上一頁”和“下一頁”鏈接用於呈現當前頁的前一頁和後一頁的記錄。那麼現在有兩種實現方式返回上下頁的記錄。

    Web API不僅僅會定義根據具體頁碼的數據查詢定義相關的操作，還會針對“上一頁”和“下一頁”這樣的請求定義單獨的操作。它自身會根據客戶端的Session ID對每次數據返回的頁面在本地進行保存，以便能夠知道上一頁和下一頁具體是哪一頁。
    Web API只會定義根據具體頁碼的數據查詢定義相關的操作，當前返回數據的頁碼由客戶端來維護。

第一種貌似很“智能”，其實就是一種畫蛇添足的作法，因為它破壞了Web API的無狀態性。設計無狀態的Web API不僅僅使Web API自身顯得簡單而精煉，還因減除了針對客戶端的“親和度（Affinty）”使我們可以有效地實施負載均衡，因為只有這樣集群中的每一臺服務器對於每個客戶端才是等效的。

[1] 大部分計算機書籍都將Side Effect翻譯成“副作用”，而我們一般將“副（負）作用”理解為負面的作用，其實計算機領域Side Effect表示的作用無所謂正負，所以我們覺得還是還原其字面的含義“邊界效用”。除此之外，對於GET、HEAD和OPTIONS請求來說，如果服務端需要對它們作日誌、緩存甚至計數操作，嚴格來說這也算是一種Side Effect，但是請求的發送者不對此負責。

[2] 這裡的“兼容”不是指支持由瀏覽器發送的請求，因為通過執行JavaScript腳本可以讓作為宿主的瀏覽器發送任何我們希望的請求，這裡的兼容體現在儘可能地支持瀏覽器訪問我們在地址欄中輸入的URI默認發送的HTTP-GET請求。
-->










## 最佳實踐

### 1. 資源命名

資源 (也就是 URI) 應該使用名詞而不是動詞，因為 "資源" 表示一種實體，所以應該是名詞，URI 不應該有動詞，動詞應該放在 HTTP 協議中，而且資源一般使用複數。

例如，某個 URI 是 `/posts/show/cars`，其中 `show` 是動詞，這個 URI 就設計錯了，正確的寫法應該是 `/posts/cars` ，然後用 GET 方法表示 `show` 。

<!--
使用子資源表達關係
如果一個資源與另外一個資源有關係，使用子資源：
GET /cars/711/drivers/ 返回 car 711的所有司機
GET /cars/711/drivers/4 返回 car 711的4號司機
-->

### 2. 修改狀態

GET 方法和查詢參數不應該涉及狀態改變，如果要修改狀態，應該使用 PUT、POST、DELETE 方法，而不是 GET 方法來改變狀態。

GET /users/711?activate
GET /users/711/activate

### 3. HTTP 頭

使用 HTTP 頭聲明序列化格式，在客戶端和服務端，雙方都要知道通訊的格式，格式在 HTTP 頭部中指定。

{% highlight text %}
Content-Type    請求格式
Accept          系列可接受的響應格式
{% endhighlight %}

### 4. 集合操作

可以為集合提供過濾、排序、選擇和分頁等功能。

{% highlight text %}
----- filtering 過濾，使用唯一的查詢參數進行過濾
GET /cars?color=red                    返回紅色的cars
GET /cars?seats<=2                     返回小於兩座位的cars集合

----- Sorting 排序，允許針對多個字段排序
GET /cars?sort=-manufactorer,+model    根據生產者降序和模型升序排列的car集合

----- Paging 分頁，通過limit和offset實現分頁，可以提供默認值
GET /cars?offset=10&limit=5            為了將總數發給客戶端，使用訂製的HTTP頭 X-Total-Count

----- Field Selection 段選擇，只顯示資源的部分所需要字段，降低網絡流量，提高API可用性
GET /cars?fields=manufacturer,model,id,color
{% endhighlight %}

### 5. 返回碼

使用 HTTP 狀態碼處理錯誤，HTTP 狀態碼提供了 70 個出錯，實際上可以只使用其中的 10 個左右。

{% highlight text %}
200 – OK – 一切正常
201 – OK – 新的資源已經成功創建
204 – OK – 資源已經成功擅長

304 – Not Modified – 客戶端使用緩存數據

400 – Bad Request – 請求無效，需要附加細節解釋如 "JSON無效"
401 – Unauthorized – 請求需要用戶驗證
403 – Forbidden – 服務器已經理解了請求，但是拒絕服務或這種請求的訪問是不允許的。
404 – Not found – 沒有發現該資源
422 – Unprocessable Entity – 只有服務器不能處理實體時使用，比如圖像不能被格式化，或者重要字段丟失。

500 – Internal Server Error – API開發者應該避免這種錯誤。

使用詳細的錯誤包裝錯誤：

{
  "errors": [
   {
    "userMessage": "Sorry, the requested resource does not exist",
    "internalMessage": "No car found in the database",
    "code": 34,
    "more info": "http://dev.mwaysolutions.com/blog/api/v1/errors/12345"
   }
  ]
}
{% endhighlight %}



### 6. 版本管理

可以在 URI 中加入版本號，也可以在 HTTP 請求頭或者消息體中添加。







<!--
RESTful API 設計最佳實踐
http://blog.jobbole.com/41233/


http://www.runoob.com/w3cnote/restful-architecture.html
-->

{% highlight text %}
{% endhighlight %}
