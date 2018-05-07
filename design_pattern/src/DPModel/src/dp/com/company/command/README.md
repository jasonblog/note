#命令模式 (Command Pattern) 
定義：Encapsulate a request as an object, thereby letting you parameterize clients with different requests, queue or log request , and support undoable operations.（將一個請求封裝成一個對象，從而讓你使用不同的請求把客戶端參數化，對請求排隊或者記錄請求日誌，可以提供命令的撤銷和恢復功能。）    


命令模式 的通用類圖如下圖：  
![Alt text](command.jpg "命令模式 類圖")

從類圖中看，我們看到三個角色：

- Receiver接收者角色：該角色就是幹活的角色，命令傳遞到這裡是應該被執行的。
- Command命令角色：需要執行的所有命令都在這裡聲明。
- Invoker調用者角色：接收到命令，並執行命令。


#命令模式的應用
##1.命令模式的優點
 * 類間解耦：調用者角色與接收者角色之間沒有任何依賴關係，調用者實現功能時只須調用Command抽象類的execute方法就可以，不需要了解到底是哪個接收者執行。
 * 可擴展性：Command的子類可以非常容易擴展，而調用者Invoker和高層次的模塊Client不產生嚴重的代碼耦合。
 * 命令模式結合其它模式會更優秀：命令模式可以結合責任鏈模式，實現命令族解析任務；結合模板方法模式，則可以減少Command子類的膨脹問題。  


##2.命令模式的缺點 
如果命令很多，Command的子類會膨脹得非常大，這個需要在項目中慎重考慮。  


##3.命令模式的使用場景
只要你認為是命令的地方就可以採用命令模式，例如，在GUI開發中，一個按鈕的點擊是一個命令，就可以採用命令模式；模擬DOS命令的時候，當然也要採用命令模式；觸發-反饋機制的處理等。