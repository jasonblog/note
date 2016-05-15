# API 設計 — 建構之道: 類別建構子設計


好的 API 設計，缺不了好的建構子 (constructor) 設計，建構子設計得好，基本上這個類別就不會髒到哪裡去。一個骯髒的物件，很可能是:

- 物件屬性全部開放，無法確定 Caller 從外部修改了哪些東西，增加重構時的不確定性。(除非物件本身是 Data Object)

- 暫用變數存成物件屬性，甚至跨方法存取這個暫時用變數。

- 物件行為過度依賴屬性狀態，以及過度複雜的狀態，以至於最後有些 method 必須在某個狀態下使用，若在其他狀態下呼叫，則會導致無法預的狀況發生。

- 無法很清楚的快速分辨這個物件所需的必要參數。

- 外部開放的 helper method。

類別方法基本上是圍繞著物件屬性在打轉的，如果沒辦法控制得好哪些屬性該放哪些屬性不該放，最後就會導致整個類別方法走火入魔。

##必要參數

基本上一個類別的建構子的參數，一定要是必要參數，讓這個物件被初始化之後，就可以直接呼叫方法使用（不是 Setter)，必要參數不可以在物件初始化之後才去設定。

換句話說，只要傳了必要參數給建構子，這個類別就可以直接有操作的行為，藉此就可以簡化物件的狀態。

我們常會看到初學者這樣寫:
```sh
$car = new Car;
$car->setWheel(4);
$car->setColor('red');
$car->setDriver($john);
$car->setKey("key of the car");
$car->drive(); // drive requires wheel property
```

其中 Wheel 跟 Color 很明顯的是必要參數，因為 drive() 方法需要 wheel，原因是，現實生活中一臺車子是不允許臨時改輪子數或顏色的 :p。

思考一下，倘若開發者忘記呼叫 setWheel 就直接呼叫 drive() ，不就會造成錯誤嗎?

key 是後取得參數，不見得可以在物件建構時取得，又或是我們可以換 key，也因此 Wheel 跟 Color 應該變成 constructor 的一部份，而 key 應保留為 setter，讓 drive() 方法內直接檢查是否有 key 來做錯誤處理，又或是直接傳遞給 drive() 方法做檢查。

而如果只有一個方法需要那個參數，執行完就會拋棄，則不應該存取參數為類別屬性，取而代之，應該是變成該方法的參數

重新整理過，變成:
```sh
$car = new Car('red', 4);
$car->setKey("key of the car");
$car->drive($driver); // drive requires driver to drive
```

又或是將 key 包裝在 Driver 內，一來也比較符合真實世界的邏輯，如此一來，就簡化成:

```sh
$car = new Car('red', 4);
$car->drive($driver);
```

那麼 drive($driver) 的原型以及實作，便可寫成下列方式:

```sh
public function drive(Driver $driver) {
   $key = $driver->getKey();
   // validate the key
   // ... more stuff goes here
}
```

建構子的參數，如果是不可改的，則存到類別屬性後，不可提供 Setter 給予使用者操作。

相反的，如果是可改的，就可以安心的加上 Setter。

那究竟什麼是可改什麼是不可改？你只需要問自己，在物件操作時，突然改掉這個屬性的值，有沒有可能造成其他方法出錯？如果是，那就是不可改。

那麼該物件屬性的 Scope 該設為 protected 或 private 呢？

當你是在設計 final class 時，使用 private，並在 class 上加上 final keyword；若你允許其他 user 透過延展 (extend) 你的類別，那麼就使用 protected ；若這個屬性被 end user class 修改後會產生不確定性，那麼就使用 private。

又譬如，一個 ArrayMapper 或 ArrayFilter，很明顯的 Array 就是他的必要參數，必須要設計在建構子裡，而不是建構後才去 Set Array。譬如:

```java
class ArrayFilter
{
    public function __construct(array $array, $opt1 = 'default')
    {
        ...
    }
}
```

透過必要參數來設計建構子跟傳遞初始化屬性有幾個好處，在新增類別方法時，你可以非常確定有哪些屬性是一定會有 Value，而不用害怕去存取尚未初始化的屬性而造成錯誤。
簡化屬性狀態

避免類別方法直接依賴屬性的不同狀態，如果你真的有很多屬性狀態要操作，建議直接把方法包裝在狀態物件上，透過 Chain of Responsibility pattern 來處理。譬如:

```java
class Account
{
    protected $state;
    protected $objectRequiredByState1;
    protected $objectRequiredByState2;

    public function __construct()
    {
        $this->state = 0;
    }

    public function proceed()
    {
        switch ($this->state++) {
        case 1:
            $this->objectRequiredByState1 = new Foo;
            break;

        case 2:
            $this->objectRequiredByState2 = new Foo;
        }
    }

    public function doSomething()
    {
        $this->objectRequiredByState1->run();
    }

    public function doSomething2()
    {
        $this->objectRequiredByState2->run();
    }
}

```

看得出來問題點嗎？doSomething 隱含相依於狀態 1，而 doSomething2 隱行相依於狀態 2，這樣就違反了物件設計的包裝原則 — 你必須很清楚知道現在這個物件是在哪個狀態才能做操作，否則就會出錯。
跨方法存取暫用變數

你可能見過像這樣的寫法:

```java
class Foo
{
    public function foo()
    {
        $this->tmpVar = new Service;
    }

    public function bar()
    {
        $ret = $this->tmpVar->create(....);
        // do more stuff on $ret
    }
}
```

看得出來問題所在嗎？bar 很明顯的依賴於 foo，但這個依賴是隱含的，也就是說，外部操作必須很明顯呼叫 foo() 後才能呼叫 bar()，否則將會出錯。當這樣的 use case 在同一個類別中越來越多時，這個類別就髒掉了。

你可能會說，那我就設計個例外好了？事實上設計成以下這樣，也不是太好，因為對這個類別使用者來說，要記住的規則還是太多：

```sh
class Foo
{
    public function foo()
    {
        $this->tmpVar = new Service;
    }
    public function bar()
    {
        if (!$this->tmpVar) {
            throw new LogicException(...);
        }

        $ret = $this->tmpVar->create(....);
        // do more stuff on $ret
    }
}
```


其實整個概念很簡單，就是物件的設計，要盡量避免「一定要看範例才寫得出來正確的程式」，或減少在寫程式時，思考「這樣寫會不會出問題」的疑慮，因為疑慮一多，去查找程式碼或寫測試驗證，耗費的時間就越多。

###How About Dependency Injection?

讀到這邊，讀者一定會想，所以才要用 Dependency Injection 呀！其實 DI 也不是銀彈，The downsides are:

- 很難只看 constructor prototype 就清楚 required argument，最後會變成 required argument 一定要寫在文件裡，且要建構好足夠用的 DI object 才可以使用。

- 因為改用 DI 的關係，Type Checking 也都要自己寫檢查，相反的，若寫成 function argument 我們可以透過 programming language 本身自帶的 type checking 幫你做參數檢查。

- 跨元件或跨框架開發時，DI 也必須要支援到同一套 prototype，否則會變成各用一套 DI library 最後弄巧成拙。

- 難以透過靜態分析工具分析正確性。

但也不是這樣就不要用 DI 了，個人認為 DI 最適用的場景是 Framework 之類的角色，由於 Web Framework 是一整個 Application 的主控者，本身需要分享各種不同的 Service 來跨元件使用，而這些元件本身外部性不高，而外部性高的，可以寫成 ServiceProvider 多包一層，透過 DI 建構外部物件所需要的參數。 也就是說， It depends.

###什麼時候需要 Factory?

我們常會看到許多遭到濫用的 Factory pattern，因為初學者學 Design Pattern 第一個學的就是 Factory pattern。所以很多時候會看到 Factory pattern 裡面的建構方法只有一行…..

事實上，只有在建構子參數複雜，需要花上很多行數準備參數並建構一個物件時，我們才需要 Factory pattern，讓外部呼叫邏輯清晰。

譬如說:

```php
$foo = Foo::complexLogics1($request);
$bar = Bar::complexLogics2($request);
$environment = new Environment($foo, $bar);
$environment->setTimeout(…);
$environment->setOtherRequirement();
```

變成:

```php
$environment = EnvironmentFactory::createFromRequest($request);
```

這樣在 Application code 中，就不會因為交互穿插的參數準備打亂了整個閱讀順暢度，一方面在其他地方要建構類似的物件時，也可減少 Copy Paste 的數量。

簡而言之，當一個系統中類別數量超過上百上千，你很難清楚記住每個類別中隱含的操作邏輯或行為。

也因此，在設計時，一定要盡可能減少類別最小參數需求，或減少基本參數建構的複雜度，就算你的測試沒有面面俱到，也可以保有最基本的品質。