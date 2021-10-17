## 簡介 Kotlin: run, let, with, also 和 apply



Kotlin 的 Standard Library 提供了幾種 Function ，有些可以處理之前提到的[可為空變數](https://medium.com/@louis383/kotlin-這些符號到底什麼意思-4274d3ae32ab)。老實說如果使用到最後，會發現其實它們大多數是可以互相替換的。它們並沒有什麼特殊的特性，目的只是提升程式的語意，增加閱讀性而生。

本文先假設讀者已經知道 lambda 表達式是怎麼一回事，範例之中就不再另外解釋。[參考 Kotlin 文件的 [Higher-Order Functions and Lambdas](https://kotlinlang.org/docs/reference/lambdas.html#passing-a-lambda-to-the-last-parameter)]

# run

可以將 `run` 想像成一個獨立出來的 Scope ， `run` 會把最後一行的東西回傳或是帶到下一個 chain。
第一個例子像是：

```kotlin
val whatsMyName = "Francis"
run {
    val whatsMyName = "Ajax"
    println("Call my name! $whatsMyName")
}
println("What's my name? $whatsMyName")
```



雖然上面有兩個 `whatsMyName` (其實第二個同名參數可以叫同樣名稱，是因為 Kotlin 的特性 [Name shadowed](https://discuss.kotlinlang.org/t/variables-with-same-name/3564)) ，我們在 `run` 這個 scope 內又重新定義了 `whatsMyName` 變數，不過這不影響 scope 外面的變數。所以印出來是：

```
Call my name! Ajax
What's my name? Francis
```

`run` 還能將**最後一行的東西回傳**，或**傳給下一個 chain** ，也就是說能這麼寫：

```kotlin
run {
    val telephone = Telephone()
    telephone.whoCallMe = "English"
    telephone    // <--  telephone 被帶到下一個 Chain
}.callMe("Softest part of heart")    // <-- 這裡可以執行 `Telephone` Class 的方法

// callMe function in Telephone class
fun callMe(myName: String) {
    println("$whoCallMe ! Call me $myName !!");
}
```

```
/* Result */
English! Call me Softest part of heart !!
```

也可以這麼做：

```kotlin
val wowCall = run {
        val telephone = Telephone()
        telephone.fromWhere = "Sagittarius"
        telephone.whoCallMe = "Still Unknown"
        telephone  // <-- telephone 回傳，wowCall 型態成為 Telephone
    }
println("WOW, This signal is from ${wowCall.fromWhere}")
```

```
/* Result */
WOW, This signal is from Sagittarius
```

# with

`with` 一般常常作為初始化時使用， `with(T)` 之中的傳入值可以以 `this` (稱作 identifier) 在 scope 中取用，不用打出 `this`也沒關係。雖然， `with` 也會**將最後一行回傳**，但目前看起來大部分還是只用它來做初始化。透過 `with()`很明確知道是為了括弧中的變數進行設定。

```kotlin
val greatSmartphone = GoodSmartPhone()
with(greatSmartphone) {
    this.setCleanSystemInterface(true)
    
    // `this` is not necessary
    setGreatBatteryLife(true)
    setGreatBuildQuality(true)
    setNouch(ture)
}
```



但很多使用狀況變數可能是可為空的變數，如此一來 `with`的 scope 中就必須要宣告 「?」或「!!」來取用該物件的方法 (Method)。

```kotlin
private fun buildGreatSmartphone(goodSmartPhone: GoodSmartPhone?) {
    with(goodSmartPhone) {
        this?.setCleanSystemInterface(true)
        this?.setGreatBatteryLife(true)
        this?.setGreatBuildQuality(true)
        this?.setNouch(ture)
    }
}
```

# T.run

什⋯ 什麼嘛，多了 T 是怎麼一回事！
這些 function 的使用方式，需要接在一個變數後面才行。像是 `someVariable.run { /* do something */ }`，包含 `T.run` 下面四個 `let also` `apply` 都屬於這種 extension function。因為 `run`有兩種用法，這裡為了避免混淆而將 T 寫出來。

`T.run` 也能像 `with` 一樣來做初始化，而且 extension function 有個好處是可以在使用時就進行 「?」 或 「!!」 的宣告。另外，T 能夠以 `this` 的形式在 scope 內取用。像是上面的範例，如果用 `T.run`來 做初始化，就會是：

```kotlin
private fun buildGreatSmartphone(goodSmartPhone: GoodSmartPhone?) {
    goodSmartPhone?.run {
        this.setCleanSystemInterface(true)
        // `this` is not necessary
        setGreatBatteryLife(true)
        setGreatBuildQuality(true)
        setNouch(ture)
    }
}
```

當然如果傳進來的變數是空值， `T.run{}` 內的程式碼就根本不會執行了。
除此之外， `T.run` 和 `run` 的特性完全一樣。可以將**最後一行的東西回傳**，或是**傳給下一個 chain**。參考以下範例，要根據筆電系統版本印出 Windows 的開發代號：

```kotlin
// data class Laptop(maker, model, system)
val laptopA = Laptop("Dell", "XPS 13 9343c", "Windows 8.1")
val laptopB = Laptop("Lenovo", "T420s", "Windows 7")
val laptopC = Laptop("MSI", "GS65 Stealth", "Windows 10")
printWindowsCodeName(laptopA)
printWindowsCodeName(laptopB)
printWindowsCodeName(laptopC)

fun printWindowsCodeName(laptop: Laptop?) {
    val codename = laptop?.run {
            // `this` is Laptop.
            // `this` can ignore when use fields and methods
            system.split(" ")    // <-- pass to next chain
        }?.run {
            // `this` is the split strings. a List<String>
            val result = when (this.last()) {
                "7" -> "Blackcomb"
                "8" -> "Milestone"
                "8.1" -> "Blue"
                "10" -> "Threshold"
                else -> "Windows 9"
            }
            result    //  <-- pass value back
        }
    
    println("${laptop?.system} codename is $codename")
}
```

結果會是

```
Windows 8.1 codename is Blue
Windows 7 codename is Blackcomb
Windows 10 codename is Threshold
```

但實作上 `T.run` 有可能需要取用外層變數或方法，但 `this` 已經被變數 `T` 佔用。例如，取得 Activity 則要這樣處理：

```kotlin
presenter?.run {
    attachView(this@MainActivity)
    addLifecycleOwner(this@MainActivity)
}
```

有沒有其他的做法，可以讓 identifier 不是 `this`呢？
這就需要來介紹下一位： `let`

# let

又或者可以寫成 `T.let`，也是一個 extension function。T 在 scope 內則是用 `it` 來存取而不是 `this`。也**可以依照需求改成其他的名字**，增加可讀性。
與 `run` 相同，**會將最後一行帶到下一個 chain 或是回傳**。

```kotlin
class TreasureBox {
    private val password = "password"
    private val treasure = "You've got a Windows install USB"
    fun open(key: String?): String {
        val result = key?.let {
            // `it` is the key String.
            // `this` is TreasureBox.
            
            var treasure = "error"
            if (it == password) {
                treasure = this.treasure
            }
            treasure     // <-- pass value back
        } ?: "error"
      
        return result
    }
}

val treasureBox = TreasureBox()
println("Open the box , and ${treasureBox.open(null)}")
println("Open the box , and ${treasureBox.open("admin")}")
println("Open the box , and ${treasureBox.open("password")}")
```

結果會是

```
Open the box , and error
Open the box , and error
Open the box , and You've got a Windows install USB
```

`let` 的操作方式基本上與上述的 `run`或 `T.run`大至相同。依據自己的需求，也可以互相串來串去使用。需求像是希望能自訂 identifier 時，或是希望 `this` 可以存取到上層內容時，建議使用 `let` 。

```kotlin
key?.let { topSecretPassword ->
    var treasure = "error"
    if (topSecretPassword == password) {
        treasure = this.treasure
    }
    treasure
}
```

自定義 identifier

# also

也可以寫作 `T.also`
剩下的 `also`和 `apply` 決大部分也是使用於初始化物件。前文提到：這幾種 Standard Library Function 其實可以互相替換，選擇合適的場景使用即可。

而它們與上面的 `run` 與 `let`的不同之處在於： `run`與 `let` 會將最後一行傳給下個 Chain 或是回傳，物件類型依最後一行而定； `also`和 `apply` 則是**將「自己 (this)」回傳或傳入下個 chain**。

有點像是 [builder pattern](https://sourcemaking.com/design_patterns/builder) ，做完一次設定後又將自己回傳回去。另外， `also`在 scope 內可以透過 `it` 來存取 `T`本身。

```kotlin
// 半糖少冰
val drink = FiftyLan().also {
    it.setSugarLevel(FiftyLan.SugarLevel.Half)
}.also {
    it.setIceLevel(FiftyLan.IceLevel.Few)
}.also {
    it.要多帶我們一杯紅茶拿鐵嗎好喝喔 = false
}.also {
    it.plasticBag = true
}

drink.printResult()
```



結果會是

```
Your drink details:
sugar level is 50
ice level is 70
Customer needs plastic bag = true
```

話又說回來凪(Nagi) 的[點餐單](https://4.bp.blogspot.com/-9c_rmUxrM2w/VjeDxrGl8AI/AAAAAAAAGOY/_M7aXqGEe4o/s1600/NAGI_%E4%B8%AD%E6%96%87%E9%BB%9E%E9%BA%B5%E5%96%AE%E6%9B%B4%E6%96%B0-01.jpg)跟 Builder Pattern 也有異曲同工之妙呢。

# apply

也可以寫作 `T.apply` 。
`apply`與 `also` 有 87 分像，不同的地方是 `apply` 在 scope 內 `T`的存取方式是 `this` ，其他都與 `also` 一樣。

這裡的範例以 Fragment 生成時，需要時做的 `newInstance()`方法。利用 `apply` 和 `also` 在 Kotlin 之中如何改寫：

```kotlin
companion object {
    private const val COFFEE_SHOP_LIST_KEY = "coffee-list-key"
    fun newInstance(coffeeShops: List<CoffeeShop>): ListFragment {
        return ListFragment().apply {
            // `this` is `ListFragment` in apply scope  
            arguments = Bundle().also {
                // `it` is `Bundle` in also scope
                // `this` is `ListFragment`        
                it.putParcelableArrayList(COFFEE_SHOP_LIST_KEY, coffeeShops as ArrayList<out Parcelable>)
            }
        }
    }
}
```

在 return 時，透過 `apply` 對 Fragment 進行加工後再回傳。

# 結論

這些 Standard Library 提供的 Function 其實大同小異。用的時候除了語意以外，還有什麼選擇方式？

1. 要傳遞最後一行，還是傳遞自己？

2. 是否需要 extension function 先判斷可為空的變數？

3. Scope 內想透過 `this`或 `it` 存取 `T`？
    考量可以是：是否需要存取外層的變數，identifier 是否可以依需求自由命名

先判斷 1. ，假設情境需要傳遞自己，即有 `apply` 或 `also` 可以選擇。再用 3. 判斷目前情境適合哪一個： `apply` 透過`this` 而 `also`透過 `it` 來存取傳入變數。

如果需要傳遞最後一行，有四個選項： `run`、 `T.run`、 `with` 和 `let` 。先用 2. 判斷是否需要預先判斷可為空變數。

- 不需要先判斷，那麼就剩： `run`和 `with` 。 `with`可以在 scope 內透過 `this` 存取傳入變數； `run` 沒有任何傳入變數，但可以將最後一行傳遞出去。
- 需要先判斷，即為 `T.run`和 `let` 。再用 3. 判斷哪個適合目前的情境： `T.run` 透過 `this` ，`let` 則是透過 `it` 來存取傳入的變數。

Elye Project 寫的一篇：[Mastering Kotlin standard functions: run, with, let, also and apply ](https://medium.com/@elye.project/mastering-kotlin-standard-functions-run-with-let-also-and-apply-9cd334b0ef84)。([簡體中文翻譯](http://liangfei.me/2018/03/31/kotlin-mastering-standard-functions/)) 下方有提到明確的判斷方式，建議大家可以看看這篇。

另外，也大力推薦 [朱立 Ju1ian](https://twitter.com/walkingice) 的 《[Kotlin 的 scope function: apply, let, run..等等](https://julianchu.net/2018/05/05-kotlin.html)》。寫個更為詳細，清楚。

希望這篇文章有幫助到正在學習 Kotlin 的 Developer 。



## 出處

https://louis383.medium.com/%E7%B0%A1%E4%BB%8B-kotlin-run-let-with-also-%E5%92%8C-apply-f83860207a0c