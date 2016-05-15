# API 設計— 命名之術


相信很多人都知道，寫程式的時候，多數的時間，我們都是在想變數名稱、函數名稱、類別名稱。

由於軟體正式開始使用後，不管是內部使用或是開放專案，都有外部依賴，一旦用了不好的名稱，往後修改，是相當不便，而且會破壞外部軟件的穩定度。

2010 年的 jQuery，主張了 Less is more 的概念，從 jQuery 的 API 設計，多少可以學到一些如何設計簡單易用 API 的概念。然而，不同的程式語言、編譯器都有各自適用的設計模式，不見得會通用，需要對其效能、語法限制作斟酌。 所以，Sometimes, less is more, but sometimes, less leads to confusion.

筆者雖不算相當資深，但覺得多年的設計經驗應該找個時間，統合組織一下，寫出來 (所以這篇其實藏在草稿裡很久了 ….. XD)

##資料與狀態

最常見的糟糕變數命名，很顯然的就是 a1, a2, i1, i2, j1, j2, r1, r2, 這類毫無任何含意的變數名稱（拜託，我們不是在寫 LLVM bitcode 啊，你又不是編譯器）

好吧，再進步一點，變成 product1, product2, book1, book2，好像好多了？

但事實上，有誰能從字面上分得出 product1 跟 product2 的差別？這類的變數名稱只有在一個情境下有意義，譬如：從一個 product list 中，其中符合條件第一項與第二項的 product。

其實，程式設計就是在處理資料與狀態，用來儲存資料的變數，很明顯的，因此有狀態、範圍之區分，只要掌握這個概念，基本上就不會取出令人匪夷所思的變數名稱了。

譬如，一份產品清單內，有已經上架的產品以及尚未上架的產品，其變數名稱很明顯就可命名為: $publishedProducts 或為 $unpublishedProducts，由其情境下的狀態、數量去構成。

語意是變數名稱中最重要的一部份，”published”, “unpublished” 意味著產品的狀態，所以最簡單最簡單的 pattern，就是 Adj. + N. 就可以是一個清楚的變數名稱。

`單數複數表示也很重要，當我們採用複數命名，其意味著該變數很可能是一個 array 類型、collection 類型、traversable 類型的資料。`

不過，如果你在同一個函數下，不需要同時處理兩種狀態的產品清單，那麼其實狀態描述多半可以省略（除非其情境有可能造成混淆才需要區分），否則多半可以寫成如下代碼:

```php
function processProductStats() {
    $products = getPublishedProducts();
}
```


如上，函數名稱已經很明顯的告訴讀者，這邊取得的產品，是已經 published 的產品。

而同時處理兩種狀態的產品清單，寫成如下代碼，就很容易閱讀:

```php
function processProductStats() {
    $publishedProducts = getPublishedProducts();
    $unpublishedProducts = getUnpublishedProducts();
}
```

會比下方代碼，好得多:

```php
function processProductStats() {
    $p1 = getProducts(true, 1, 10);
    $p2 = getProducts(false, 2 , 10);
}
```

不蓋你，真的有人這樣寫，真心不騙。

再複雜一點，當你的資料有不同關係時，也可以加上 prep. of 作為區分，譬如:

$publishedProductsOfUser, $publishedProductsOfAdmin

如上，意味著 published product 區分為 user product 或 admin product。

如果同一個 Context 內，要標示不同的資料來源，可以用 “from”

$userFromFacebook, $userFromTwitter

當然，也可以直接簡化為 $twitterUser 或 $facebookUser，”from” 只是最為一個當你不知道如何命名時，最安全的一個手法。

而匈牙利命名法也不是全然的有病，在某些場合下，其實相當有用，譬如現代框架常用的 ORM 會提供 Collection 這類的物件，而 Collection 可以轉成Model 的陣列資料，如下:

```sh
$products = new ProductCollection;
$items = $products->items(); // 實際上是 Product[] 而非 array of array
```

如上場合使用 $items 其實沒有什麼大問題，但多數時候，我們會處理多個 Collection:

```sh
$products = new ProductCollection;
$categories = new CategoryCollection;
```

全部轉成 array of model record 呢?

```sh
$products = new ProductCollection;
$categories = new CategoryCollection;
$productItems = $products->items();
$categoryItems = $categories->items();
```

好，目前都沒有什麼大問題，不過我們現在遇到一個狀況，就是需要將資料組合後，以 JSON 格式送出，而 encode_json 只吃 array 型別的資料，所以我們需要將 Array of model record 轉成 array of array，怎麼辦？程式碼很可能變成這樣:

```js
$products = new ProductCollection;
$categories = new CategoryCollection;

$productItems = [];
foreach ($products as $product) {
    $productItem = $product->toArray();
    $productItem['types'] = $product->types->toArray();
    $productItems[] = $productItem;
}
$output = [
    'products' => $productItems,
    ......
];
```

上面這段程式碼，看起來沒有什麼大問題，不過他違反了一個規則 — Consistency 一致性

上上段程式碼，我們使用 suffix -items 作為 array of model records 表示，但這邊，我們卻使用 suffix -item 以及 suffix -items 作為 array of array 的表示，如果先看第一段代碼，再看這段代碼，就會知道容易造成混淆的地方為何了。

整理一下，加上型別名稱，是否比較清楚?


```js
$products = new ProductCollection;
$categories = new CategoryCollection;

$arrayProducts = [];
foreach ($products as $product) {
    $arrayProduct = $product->toArray();
    $arrayProduct['types'] = $product->types->toArray();
    $arrayProducts[] = $arrayProduct;
}
$output = [
    'products' => $arrayProducts,
    ......
];
```

這邊，取名為 $arrayProduct 以及 $arrayProducts 的主要原因，是為了強調其型別為 array，因此作為 prefix 放在 N 的前面，同樣的 $productArray 或是 $productsArray 也是可行，或甚至更清楚的名稱也是可行，如 $arrayOfProductArray 以及 $productArray… 等，同樣都達到了型別標示的目的。
##常見縮寫

而常見變數名稱如 $i 其實是 $index 的縮寫，所以也不用大驚小怪，而 $j 則是常見用來作為第二層迴圈的索引值，這幾個用法，都是相當通用，所以也不用避諱。

$a, $b ??

隨著 functional 寫法越來越普遍，多數陣列處理也會用上 comparator，譬如 sort(), 其中 $a , $b 多半用來表示兩邊不同的變數，這個也蠻常用的，算是很簡潔的寫法，畢竟 comparator 函數內容通常很精簡，不太需要過度清楚的區分不同變數。

##操作資料

目前為止以上概念都很簡單，我們現在懂了變數命名，更進一步的，就是撰寫操作這些資料的代碼，而操作這些資料，我們需要函數，因此函數名稱是比變數名稱更重要的，因為它不僅僅是操作資料，也是對外公開表示：「Hey I can do this」

如果你取錯名稱，在下一個版本把它改了名字，套件升級的使用者只會覺得憤怒，因為你讓他們的軟體增加了不穩定性… XD

你能想像當你走進一家政府機構辦事，該路上的指示牌到告訴你 XXX 事務請往右走到底，第一次很順利，隔年你又去辦事，跟著指示牌走到底，結果居然換位置了？當然很怒啊。 所以這個指示牌有沒有更新，就很重要，在軟體裡面我們可以把它叫做 Changelog 或 Migration，通常會以 CHANGELOG 或用 MIGRATION, UPGRADE 命名…之所以全部用大寫，是因為小寫的話沒人看，最後作者怒了，全部都用大寫，其意義就是「拜託你給我看這個文件」

當然，Migration, Upgrade 越少越好，雖然我們沒辦法一次就看到好的設計，但我們至少可以在一開始的時候，把初步的設計做好。
函數命名

在講函數命名之前，我想可以先來簡單介紹一下，幾個通用性的動詞，基本上理解這幾種動詞背後所隱含的意味，函數命名就不會錯太多。

以下幾個 prefix- 不是絕對的限制，只是一個通用、常用的準則，用以參考：
```sh
get-
```
通常類別方法以 get- 開頭，意味著我們要取得某項屬性或數值，很顯然的，我們沒辦法透過屬性直接存取，所以才需要 get- ，而 get- 最重要的一點，就是他「只做簡單回傳，`不作修改`」，也因此，一個 get method 不應該修改物件的狀態、修改物件內容、也不該調用複雜資料庫查詢

除非整個類別是專為遠端 API 設計，其 get- 才比較 make sense，不過也須注意，類別本身屬性的取得和遠端資料取得都使用 get- ，很可能會造成混淆。譬如:

```js
$client = new MyAppAPI;
$client->getHost();    // 好像沒有 API query?
$client->getUserId();  // 到底有沒有 API query 呢?
$client->getStories(); // 到底有沒有 API query 呀!!???!
```

如上範例，使用 fetch- prefix 改為如下代碼，可能清楚很多:

```js
$client = new MyAppAPI;
$client->getHost();
$client->fetchUserId();
$client->fetchStories();
```

get- prefix 意味著，輕巧的、快速的拿到某個資料… 以此類推，set- 也是一樣。

不過，如果是單純的函數，而非類別方法，則 get- 就無此限制，命名方式較為彈性。

```js
set-
```

通常被用在設定屬性，與 get- 一樣，不該用在資料庫操作。我們在另外一篇文章「建構子設計之道」有討論 set- 正確的使用時機。

```js
query-
```

上面說的 get- 不該調用遠端 API 或資料庫操作，讀者肯定心中充滿了疑問，「什麼？不用 get? 我看你才不懂吧」

事實上，還有一個 prefix 很好用，就是 query-，query 意味著查詢，而且是帶有條件的，所以使用者可能會期望 query- method 有條件參數可以傳遞。查詢有很多種含義，不管是 database query, api query 都算是 query，當開發者查看 API 規格，甚至只看得到 caller 怎麼呼叫，”query-” 很明顯的就可以讓開發者了解 — 喔，這個可能得花一點時間，而且可能會有一些 ”溝通” 要做

```js
do-
```

do- 列在這邊，實際是常見的錯用 prefix- ，很多時候會看到有人寫 doUpdate, doQuery, doKill 這類包含 do- prefix 的函數名稱，但其實 do- 很沒意義。 如果你只是為了區分 doUpdate 以及 update 這兩個函數名稱，不如就用底線開頭來隱藏另外一個實作吧 _update, update 表面上看起來，很明顯的 update 才是真正的開放 API，而 _update 的 scope 建議應該為 private 或 protected。

```sh
fetch-/store-
```

fetch- 與 store- 通常是對應的，比 query 更模糊一點的是，fetch- 通常不帶有複雜條件，譬如 fetchProduct, fetchBooks, fetchUserStories 等等，對象通常是資料庫，當然場景需求不同，沒有一定要如此限制。

而當 “fetch”, “store” 同時出現在同一個類別，就可能需要考慮到一致性，因為使用者可能預期 “fetch”, “store” 是對同一個 storage 操作，不該 fetch from A, 但 store to B.

```sh
build-
```

通常用在 — 給予某些參數，建置某個結構或物件。 筆者常用的是 — 給予某些簡單的參數，建置某一用途的 Config 物件或 Map，譬如 buildReactProducctAppConfig。 build- 也可能意味著，建置資料庫內某種用途的資料，譬如 buildBaseData、buildUserData，但其意義與包含此函數的類別名稱有關。

```sh
create-
```

通常被用在 ORM Record 建置，如 ActiveRecord Pattern 常使用這個 create() method 來建置新的 Record。 create- 與 build- 很像，但就筆者看來，這兩者不同的地方在於，create- 有明顯目標跟範圍，譬如 Product::create().. Book::create(), createOrder() … 等等，而 build- 如 buildBaseData 等，不是這麼明確。（但也有可能有筆者沒有想到的案例，若有的話，之後補上 XD）

```sh
new-
```

new- 其實與 create- 不同，new- 很明顯的只意味著建構一個類別的實體化，取名以 new- 開頭，那麼隱含的操作應該不包含資料庫操作。

```sh
remove-, delete-
```

remove- 與 delete- 其實差異也不是太明顯，但就筆者來看，remove- 意味著從某個資料集，移除掉這個項目，這個 “移除” 很可能只是移除關聯性，並不會刪除實體資料，而 delete- ，很明確的意味著是實體資料的刪除。

```sh
traverse-
```

在處理樹狀資料時，traverse- 很好用，traverse- 意味著會遍歷一整個樹，常用在處理 AST 的部分。

下集待續

(也不知道有沒有下集….先寫到這邊，不然等到寫完也不知道什麼時候了。)