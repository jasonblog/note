#裝飾模式 (Decorate Pattern) 
定義：Attach additional responsibilities to an object dynamically keeping the same interface. Decorators provide a flexible alternative to subclassing for extending functionality.（動態地給一個對象添加一些額外的職責。就增加功能來說，裝飾模式相比生產子類更為靈活。）  


裝飾模式的通用類圖如下圖：  
![Alt text](decorator.jpg "裝飾模式類圖")

在類圖中，有四個角色需要說明：

- Component抽象構件：Component是一個接口或者是抽象類，就是定義我們最核心的對象，也就是最原始的對象。
- ConcreteComponent具體構件：ConcreteComponent是最核心、最原始、最基本的接口或抽象類的實現，你要裝飾的就是它。
- Decorator裝飾角色：一般是一個抽象類，做什麼用呢？實現接口或者抽象方法，它裡面可不一定有抽象的方法呀，在它的屬性裡必然有一個private變量指向Component抽象構件。
- 具體裝飾對象：ConcreteDecorator1和ConcreteDecorator2是兩個具體的裝飾類，你要把最核心的、最原始的、最基本的東西裝飾城其他東西。

#裝飾模式的應用
##1.裝飾模式的優點
 * 裝飾類與被裝飾類可以獨立發展，而不會耦合。換句話說，Component類無須知道Decorator類，Decorator類是從外部來擴展Component類的功能，而Decorator類也不用知道具體的構件。
 * 裝飾模式時繼承關係得一個替代方案。我們看裝飾類Decorator，不管裝飾多少層，返回的對象還是Component，實現的還是is-a的關係。
 * 裝飾模式可以動態地擴展一個實現類得功能。

##2.裝飾模式的缺點 
對於裝飾模式記住一點就足夠了：多層的裝飾是很複雜的。因此，儘量減少裝飾類的數量，以便降低系統的複雜度。  


##3.裝飾模式的使用場景
 * 需要擴展一個類的功能，或給一個類增加附加功能。
 * 需要動態地給一個對象增加功能，這些功能可以再動態地撤銷。
 * 需要為一批的兄弟類進行改裝或加裝功能，當然是首選裝飾模式。