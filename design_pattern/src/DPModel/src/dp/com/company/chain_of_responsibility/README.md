#責任鏈模式 (Command Pattern) 
定義：Avoid coupling the sender of a request to its receiver by giving more than one object a chance to handle the request. Chain the receiving objects and pass the request along the chain until an object handles it.（使多個對象都有機會處理請求，從而避免了請求的發送者接受者之間的耦合關係。將這些對象連成一條鏈，並沿著這條鏈傳遞請求，直到有對象處理它為止。）  


責任鏈模式的通用類圖如下圖：  
![Alt text](chainofresponsibility.jpg "責任鏈模式類圖")

#責任鏈模式的應用
##1.責任鏈模式的優點
責任鏈模式非常顯著的優點是將請求和處理分開。請求者可以不用知道誰處理的，處理者可以不用知道請求的全貌，兩者解耦，提高系統的靈活性。  


##2.責任鏈模式的缺點 
責任鏈模式有兩個非常顯著的缺點：一是性能問題，每個請求都是從鏈頭到鏈尾，特別是在鏈比較長的時候，性能是一個非常大的問題。而是調試不很方便，特別是鏈條比較長，環節比較多的時候，由於採用了類似遞歸的方式，調試的時候邏輯可能比較複雜。  


##3.責任鏈模式的注意事項
鏈中節點數量需要控制，避免出現超長鏈的情況，一般的做法是在Handler中設置一個最大節點數量，在SetNext方法中判斷是否是否是超過其閥值，超過則不允許該鏈建立，避免無意識地破壞系統性能。