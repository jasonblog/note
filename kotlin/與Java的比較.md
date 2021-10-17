### Kotlin 學習筆記 暨 Java 的比較

只要犧牲的效能不要太大，但可以換到更簡潔直覺的程式邏輯，這是覺得(期許) Kotlin 未來能取代 Java ，成為 Android 主要開發語言的理由。

Java 對我來說，邏輯太拗口，很多 compiler 能簡化邏輯的 "High Level Programming Language"，Java 卻打死不做 ... 這是多年以來(20年)來不想碰 Java 的原因。當一個語言不能直覺處理邏輯，會反複繞著圈子，用間接方式才能達到目地，另外在觀念上，越簡潔的程式邏輯，往往能更快速找出問題。

但 Kotlin 現況就是 sample code 太少，運用 example 還不多，之前開發的專案都是 Java，要很快地捨棄傳統不是太容易，所以目前學習時，只好開發底層 toolkit 時，用 Java 轉 Kotlin，上層 API 直接寫 Kotlin 來開發專案。

1. ------

以下開始記錄 Kotlin 的優點 (與 Java 比較) :
https://www.androidauthority.com/kotlin-vs-java-783187/

1. 比較簡潔(concise)。
      如 UI 的 findViewById，Kotlin 可直接存取 ui compoent)

2. default null safe。
      Kotlin 變數預設是不允許為 null (Java 是)，如果要直接設成 null，要使用 "?=" 設給 l-value。

3. Extension functions。

4. Coroutines (Kotlin 導入 suspend 概念，此詞意同 non-blocking 機制，相仿 thread sleep )
      Kotlin 支援 async/await (同 C# async/await)

```kotlin
// non-blocking coroutine
val job = launch {
    delay(1000L) // delay 是 suspend 的 sleep
}

// blocking coroutine
runBlocking {
    delay(1000L)
}
```



   \* life cycle
     GlobalScope.lauch 的 GlobalScope 是指整個 app 存在時，此 coroutine 就會存在。也可自     訂 coroutine 的 life cycle。
   \* 在 coroutine block 中呼叫 func 時，要使用 suspend 修飾字放在 func 前，才能被 corouine     使用。
   \* coroutine 的 func : join、cancel、cancelAndJoin
   \* timeout

```kotlin
withTimeout(1300L) {  //withTimeoutOrNull() 則是可以回傳 null
    repeat(1000) {
        i ->
        println("I'm sleeping $i ...")
        delay(500L)
    }
}
```



5. 支援 default function parameter (Java 不支援，需要以 function overload 達成)
      支援 properties (Java 沒有，C# 有)
6. 


========================================================
Kotlin 語言本身語法學習筆記 :
========================================================

1. statement 無分號結尾 (python style)

2. 基本型別 (跟 Java 幾乎差不多)
      Double - 64 bits
      Long - 64 bits
      Float - 32 bits
      Int - 32 bits
      Short - 16 bits
      Byte - 8 bits

3. 支援 string template (C# 也有)
    var a = 1
    val s1 = "a equals $a"
    val s2 = "${s1.replace("equals", "=")} : test in string template."

4. function :
      \* sum() 有 a, b Int 參數；回傳值也是 Int，也可以是 null

    ```kotlin
    fun sum(a: Int, b: Int) : Int ? {
        return a + b
    }
    ```

    

  \* Kotlin 支援 function default parameter，但 Java 不支援。
    ex : fun sum(a :Int = 0, b :Int = 1) :Int? {} ...

  \* Kotlin 沒有 C# 的 parameter out/ref 的關鍵字，但可用下列 dirty 方式達成 :
      ex : fun getVal(oRet :Int) {
           var o = oRet
           ++o
         }

5. Kotlin 跟 Java 一樣，不支援 goto，但支援 continue@xxx、break@xxx、return@xxx
      @xxx 是 label name，可放在某行 line 的任意處 xxx@
      但只支援跳回 continue/break/return 之前的 label，在關鍵字後的 label 就跳不到了。

  *所以就我自己的寫作習慣，只能使用 try catch finally 來達成了，有點不理解，即使原理一樣，但偏偏就是要限制住 =_______= ...........。

6. Class
      \* kotlin 有三層式的 constructor 設計

    ```kotlin
    class Test(a : Int) {
        init {}
        constructor(a : Int) {}
    }
    ```

​      Test t = Test(a)

​      a. 第一層先 call Test(a :Int) 本身就是一個 constructor，但只能傳變數，不能加code
​      b. 第二層再 call init block，可以加初始化程式碼
​      c. 第三層最後 call constructor(a :Int)，可以丟不同參數，做 class overload.

  \* class 不能衍生時，保留字使用 sealed (跟 C# 相同，Java 使用 final 關鍵字)
  \* object 保留字用在 宣告 anonymous class，可跳過宣告 explicit class 使用的繁瑣性。
     val a :ClassA = object : ClassA(), ClassB { val v = 1 }
     (變數 a 是 Class，初始化一個 anonymous class 繼承 ClassA and Class B)

​     也可以直接拿來當 explicit class 的宣告 :
​       object TestClass { ... }

​     還有如果在一個 class 內的 object class，func 前面加上 companion 修飾字，就可以直接 call 這個 func，意思相同於 static func of class。

7. Android Studio 3.2.1 建立有 kotlin class *.jar 的方式
    (跟舊版的 AS 不同，..... 這也是個麻煩的地方)
      \* 編輯 module 目錄的 build.gradle，

```
apply plugin: 'com.android.library'
apply plugin: 'kotlin-android'   <-- 一定要新增這行
```


   \* 再增加下面這些 script 到 build.gradle 的最後面。

```
def mod_name = 'rutil.db.mysql.jar'
task deleteJar(type: Delete) {
    delete '../libs/' + mod_name
}

task createJar(type: Copy) {
    from('build/intermediates/intermediate-jars/release/')
    from('build/tmp/kotlin-classes/release/')
    into('../libs/')
    include('classes.jar')
    rename('classes.jar', '../libs/' + mod_name)
}

createJar.dependsOn(deleteJar, build)
```


output 目錄可以修改，選擇 debug/release 目前還沒有找到方式，不過上面的 script，在 3.2.1 是可以建立有 kotlin 的 *.jar。

## Kotlin 的改進空間 :

1. 變數宣告
    相對於其他語言的變數宣告邏輯，Kotlin 蠻不直覺的，如
    Java => static final int a;
    C# => const int a;     (const int value，名稱為 a)
    K  => val a :int  (const value，名稱為 a，類型為 int)

變數宣告是最常用的敘述，硬是要多一步邏輯來達成，如果只是為了 "有語言自己的特性"，其實有點多餘，如果能改善就好了 ........，算一個大缺點，在開發時，經常會回來補 variable type 的窘況。