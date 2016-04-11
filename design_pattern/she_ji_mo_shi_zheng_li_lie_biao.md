# 設計模式整理列表


以下第一部分是物件導向程式設計基本原則，後面三個部分是設計模式(design pattern)。 

- 基本原則 (1~5 又稱 SOLID)
    - 單一職責原則 (SRP：Single Responsibility Principle)
一個類別，應該只有一個引起它變化的原因
    - 開放、封閉原則 (OCP：Open Closed Principle)
對於擴展是開放的 (open for extension)
對於修改是封閉的 (closed for modification)
    - 里氏(Liskov)代換原則 (LSP：Liskov Substitution Principle)
子類別必須能替換父類別。
    - 介面隔離原則 (ISP：Interface Segregation Principle)
    - 依賴倒轉原則 (DIP：Dependency Inversion Principle)
抽象不應該依賴細節，細節應該依賴抽像。因為抽像相對較穩定。
高層模組不應該依賴低層模組，兩個都應該依賴抽像。
針對接口編寫程式，不要對具體實現的東西編寫程式。
    - 迪米特法則 (LoD：Law of Demeter)
最少知識原則 Principle of Least Knowledge
只和自己眼前的朋友交談 Only talk to your immediate friends
低耦合

    [例如]
郵差送來掛號信，須要蓋收件人印章。
一般人不會叫郵差自己進屋找印章，既浪費時間也不安全。
正常都是自己進屋拿，或是請其他家人幫忙拿。
因為不應該給郵差進屋找東西的權限、郵差也不須要知道印章放在屋內何處。
    - 合成/聚合重覆使用原則 (CARP)(Composite/Aggregate Reuse Principle)
多用合成/聚合，少用繼承。
在兩個物件有 has-a (has-parts、is-part-of)關係時 => 合成/聚合 (A has a B)
當兩個物件有 is-a (is-a-kind-of)關係時 => 繼承 (Superman is a kind of Person)
        - 合成 (Composite)：A、B兩物件有合成關係時，表示其中一個物件消失(ex:書本)，另一個物件也會消失(ex:章節)。
        - 聚合 (Aggregate)：A、B兩物件有聚合關係時，表示其中一個物件消失(ex:球隊)，另一個物件不會消失(ex:球員)。

###[參考]<br>
http://zh.wikipedia.org/wiki/%E9%A1%9E%E5%88%A5%E5%9C%96
http://antrash.pixnet.net/blog/post/80808873-%E8%AB%96%E7%89%A9%E4%BB%B6%E5%B0%8E%E5%90%91part-9%EF%BC%9A%E8%AA%8D%E8%AD%98is-a%E8%B7%9Fhas-a%E7%9A%84%E5%B7%AE%E7%95%B0%E6%80%A7
http://fengyu0318.sg1003.myweb.hinet.net/ood/classRelationShip.pdf

---

- 創建型模式 (Creational Pattern)
    - 簡單工廠模式 (Simple Factory pattern)
使用 class 的靜態方法，依不同條件，取得不同物件，並用取得的物件，做類似的事情。
缺點是要新增不同條件時，須修改到類別的靜態方法。

    - 工廠方法模式 (Factory Method Pattern)
避免了簡單工廠模式，要新增條件時，對工廠類靜態方法的修改(對修改應該是封閉的)。

    - 抽象工廠模式 (Abstract Factory Pattern)
抽象工廠類別，可返回同類型的工廠。這些返回的工廠，有多個相同的方法，做類似的事。 

    - 建造者模式(生成器模式) (Builder Pattern)
將某種類的產品建造過程，將生產步驟整理出來，
所有要生產這類產品的 class，均要實現這些標準化步驟。
另外，為避免實際生產時，遺漏某步驟，統一由一個指揮者 class，執行一系列的生產步驟。

    - 原型模式 (Prototype Pattern)
複製一個已存在的物件，來產生新的物件。
淺複製(shallow clone)：僅複製舊物件內的屬性，舊物件內的物件不複製。所以新、舊物件會共用這些其他物件。
深複製(deep clone)：舊物件內的屬性、參考到的其他物件，都會複製一份。
    - 單例模式 (Singleton Pattern)
讓一個類別只能有一個實例(Instance)的方法。
產生單一實例的方式：
懶漢方式(Lazy initialization)：第一次使用時，才產生實例。
餓漢方式(Eager initialization)：class 載入時就產生實例，不管後面會不會用到。


- 結構型模式 (Structural Pattern)
    - 適配器模式 (Adapter Pattern)
某個已存在的類，介面不是用戶所期待的。適配器，則是當做中間的轉接口，以提供用戶期待的介面。
可分成兩種實作方法
Object Adapter Pattern (對象適配器模式)：將已存在的類的實例，包裹在適配器類別中。
Class Adapter Pattern (類適配器模式)：使用多重繼承。
    - 橋接模式 (Bridge Pattern)
將一個物件的具體行為(實作)抽出來，成為一個獨立的物件。
也就是原本的一個物件，變成兩個物件：「抽像物件」+「實作物件」。
優點是抽像物件與實作物件可以解耦合，各自獨立變化。
    - 組合模式 (Composite Pattern)
數個物件之間，呈現一種樹狀結構。
    - 裝飾模式 (Decorator Pattern)
在某一物件動態加上功能。
一層一層的將功能套上去，每一層執行的是不同的物件。
    - 外觀模式 (Facade Pattern)
將原有的大系統包裝起來，用另一個較簡易的介面開放給使用者。
使用者只須了解介面的的使用方式。而不須了解大系統內，各個小系統的操作方式。
    - 享元模式 (Flyweight Pattern)
物件之間，若有共同的部分可以共享，則將可共用的部分獨立為共享物件，
不能共享的部份外部化，使用時再將外部化的部分傳給共享物件。
這樣做的優點是減少記憶體使用量。缺點是程式邏輯可能變得較複雜。
    - 代理模式 (Proxy Pattern)
有兩個物件，代理物件、真實物件，
系統使用代理物件操作，代理物件內部則再去操作真實物件。
應用：遠程代理、虛擬代理、安全代理

###[參考]<br>
http://zh.wikipedia.org/wiki/%E4%BB%A3%E7%90%86%E6%A8%A1%E5%BC%8F

---

- 行為型模式 (Behavioral pattern)
    - 責任鏈模式 (Chain-of-responsibility Pattern)
有幾個物件都能處理某種請求，但處理的能範圍(權限)不同，
當這個物件沒有處理的權限時，能夠將這個請求，傳遞給下一個物件繼續處理。
    - 命令模式 (Command Pattern)
一般命令包含發出命令和執行命令。
命令模式，則是將這個過程拆成三個物件，發出命令的物件(Invoker)、命令的物件(command)、執行命令的物件(receiver)
由 Invoker 物件，來建造要執行的命令。如此要擴充功能時，例如增加命令重覆執行、取消命令...等，也變得較單純。
    - 解釋器模式 (Interpreter Pattern)
用來解釋、直譯一種語言的方法。<br>
    [參考]<br>
http://en.wikipedia.org/wiki/Interpreter_pattern
http://www.dofactory.com/Patterns/PatternInterpreter.aspx
    - 迭代器模式 (Iterator Pattern)
遍歷容器裡面元素的一種方法。
    - 中介者模式 (Mediator Pattern)
當物件和物件之間或有錯綜複雜的交互作用，可將這些關係交由另一物件(中介者)來處理，
以減少這些物件間的耦合。
    - 備忘錄模式 (Memento Pattern)
讓物件回復到之前狀態的方法。
    - 觀察者模式 (發佈/訂閱模式) (Observer Pattern)
兩種類型的物件，「通知者」和「觀察者」。
        - 訂閱：「通知者」可增減訂閱列表中的「觀察者」
        - 發佈：當有監聽的事件發生時，「通知者」可從訂閱列表中，將事件通知「觀察者」，「觀察者」則會對此事件做相對應的動作。
        - 功用：解除耦合，讓耦合的雙方依賴抽像(接口)，而不依賴具體。
    - 狀態模式 (State Pattern)
一物件有多個狀態，在不同狀態下有不同的行為。
一般可能會用多個 if else 來處理這些分支行為。
若使用狀態模式，則是將這些狀態處理，提取出來到另外的 class 處理這些分支。
也就是將 if else 改寫成 class。
    - 策略模式 (Strategy Pattern)
將不同的演算法，定義成一個家族，
這些演算法實現同樣的接口，且寫成個別的類別，所以彼此之間能夠互相替換。
優點是以後要增加新的演算法，只須額外新增一個類別，不須動到原本的類別。
    - 模板方法模式 (Template Method Pattern)
將不變的部分移到父類別，去除子類別重覆的程式碼
    - 訪問者模式 (Visitor Pattern)
當一個「物件結構」中的「元素」幾乎不會異動，但這些「元素的行為」常會增減，則適合用訪問者模式。
訪問者模式是將「元素的行為」，提取出來，每一種行為做成一個 「Visitor(訪問者) 物件」，
每一個 「Visitor(訪問者) 物件」，都能根據原本「物件結構」中不同的「元素」，產生不同行為。<br>

### 參考：
http://en.wikipedia.org/wiki/SOLID_%28object-oriented_design%29 (SOLID)<br>
http://en.wikipedia.org/wiki/Software_design_pattern<br>
http://www.dofactory.com/Patterns/Patterns.aspx<br>