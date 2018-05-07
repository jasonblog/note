#迭代器模式(Iterator Pattern)
定義：Convert the interface of a class into another interface clients expect. Adapter lets classes work together that couldn't otherwise because of incompatible interfaces.（將一個類的接口變換成客戶端所期待的另一種接口，從而是原本因接口不匹配而無法在一起工作的兩個類能夠在一起工作。）  


迭代器模式通用類圖如圖所示。  
![Alt text](iterator.jpg "迭代器模式類圖")


 我們先來看看迭代器模式中的各個角色：

- Iterator抽象迭代器：抽象迭代器負責定義訪問和遍歷元素的接口。
- ConcreteIterator具體迭代器：具體迭代器角色要實現迭代器接口，完成容器元素的遍歷。
- Aggregate抽象容器：容器角色負責提供創建具體迭代器角色的接口，必然提供一個類似createIterator()這樣的方法，在Java中一般是iterator()方法。
- Concrete Aggregate具體容器：具體容器實現容器接口定義的方法，創建出容納迭代器的對象。