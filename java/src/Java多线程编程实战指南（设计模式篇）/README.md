# 《Java多線程編程實戰指南（設計模式篇）》源碼

這是國內首部Java多線程設計模式原創作品《Java多線程編程實戰指南（設計模式篇）》一書的源碼。

新書《Java多線程編程實戰指南（設計模式篇）》，張龍老師作推薦序。本書從理論到實戰，用生活化的實例和通俗易懂的語言全面介紹Java多線程編程的“三十六計”多線程設計模式。亞馬遜、噹噹、京東、互動出版網、淘寶等各大書店有售。
 
##【試讀樣章下載】
http://download.csdn.net/detail/broadview2006/9254525

##【購買鏈接】

亞馬遜購買鏈接：
http://www.amazon.cn/dp/B016IW624G/ref=viscent-douban

京東購買鏈接：
http://item.jd.com/11785190.html

噹噹購買鏈接：
http://product.dangdang.com/23794043.html

互動出版網購買鏈接：
http://product.china-pub.com/4879802

文軒網購買鏈接：
http://www.winxuan.com/product/1201179948

##【前言】
隨著現代CPU的生產工藝從提升CPU主頻頻率轉向多核化，即在一塊芯片上集成多個CPU內核（Core），以往那種靠CPU自身處理能力的提升所帶來的軟件計算性能提升的那種“免費午餐”不復存在。在此背景下，多線程編程在充分利用計算資源、提高軟件服務質量方面扮演了越來越重要的角色。然而，多線程編程並非一個簡單地使用多個線程進行編程的數量問題，其又有自身的問題。好比俗話說“一個和尚打水喝，兩個和尚挑水喝，三個和尚沒水喝”，簡單地使用多個線程進行編程可能導致更加糟糕的計算效率。
設計模式相當於軟件開發領域的“三十六計”，它為特定背景下反覆出現的問題提供了一般性解決方案。多線程相關的設計模式為我們恰當地使用多線程進行編程並達到提升軟件服務質量這一目的提供了指引和參考。當然，設計模式不是菜譜，即便是菜譜我們也不能指望照著菜譜做就能做出一道美味可口的菜餚，但我們又不能因此而否認菜譜存在的價值。
可惜的是，國外與多線程編程相關的設計模式書籍多數採用C++作為描述語言，且書中所舉的例子又多與應用開發人員的實際工作經歷相去甚遠。本書作為國內第一本多線程編程相關設計模式的原創書籍，希望能夠為Java開發者普及多線程相關的設計模式開一個頭。

本書採用Java（JDK 1.6） 語言和UML（Unified Modeling Language）為描述語言，並結合作者多年工作經歷的相關實戰案例，介紹了多線程環境下常用設計模式的來龍去脈：各個設計模式是什麼樣的及其典型的實際應用場景、實際應用時需要注意的相關事項以及各個模式的可複用代碼實現。

本書第1章對多線程編程基礎進行了回顧，雖然該章講的是基礎但重點仍然是強調“實戰”。所謂“溫故而知新”，有一定多線程編程基礎、經驗的讀者也不妨快速閱讀一下本章，說不定有新的收穫。

本書第3章到第14章逐一詳細講解了多線程編程相關的12個常用設計模式。針對每個設計模式，相應章節會從以下幾個方面進行詳細講解。

模式簡介。這部分簡要介紹了相應設計模式的由來及其核心思想，以便讀者能夠快速地對其有個初步認識。

模式的架構。這部分會從靜態（類及類與類之間的結構關係）和動態（類與類之間的交互）兩個角度對相應設計模式進行詳細講解。模式架構分別使用UML類圖（Class Diagram）和序列圖（Sequence Diagram）對模式的靜態和動態兩個方面進行描述。

實戰案例解析。在相應設計模式架構的基礎上，本部分會給出相關的實戰案例並對其進行解析。不同於教科書式的範例，實戰案例強調的是“實戰”這一背景。因此實戰案例解析中，我們會先提出實際案例中我們面臨的實際問題，並在此基礎上結合相應設計模式講解相應設計模式是如何解決這些問題的。實戰案例解析中我們會給出相關的Java代碼，並講解這些代碼與相應設計模式的架構間的對應關係，以便讀者進一步理解相應設計模式。為了便於讀者進行實驗，本書給出的實戰案例代碼都力求做到可運行。實戰案例解析有助於讀者進一步理解相應的設計模式，並體驗相應設計模式的應用場景。建議讀者在閱讀這部分時先關注重點，即實戰案例中我們要解決哪些問題，相應設計模式又是如何解決這些問題的，實戰案例的代碼與相應設計模式的架構間的對應關係。而代碼中其與設計模式非強相關的細節則可以稍後關注。

模式的評價與實現考量。這部分會對相應設計模式在實現和應用過程中需要注意的一些事項、問題進行講解，並討論應用相應設計模式所帶來的好處及缺點。該節也會討論相應設計模式的典型應用場景。

可複用實現代碼。這部分給出相應設計模式的可複用實現代碼。編寫設計模式的可複用代碼有助於讀者進一步理解相應設計模式及其在實現和應用過程中需要注意的相關事項和問題，也便於讀者在實際工作中應用相應設計模式。

Java標準庫實例。考慮到Java標準庫的API設計過程中已經應用了許多設計模式，本書儘可能地給出相應設計模式在Java API中的應用情況。

相關模式。設計模式不是孤立存在的，一個具體的設計模式往往和其它設計模式之間存在某些聯繫。這部分會描述相應設計模式與其它設計模式之間存在的關係。這當中可能涉及GOF的設計模式，這類設計模式並不在本書的討論範圍之內。有需要的讀者，請自行參考相關書籍。

本書的源碼可以從

http://github.com/Viscent/javamtp

下載或博文視點官網

http://www.broadview.com.cn

相關圖書頁面。

##【目錄】

第1章  Java多線程編程實戰基礎 1

1.1  無處不在的線程 1

1.2  線程的創建與運行 2

1.3  線程的狀態與上下文切換 5

1.4  線程的監視 7

1.5  原子性、內存可見性和重排序——重新認識synchronized和volatile 10

1.6  線程的優勢和風險 11

1.7  多線程編程常用術語 13

第2章  設計模式簡介 17

2.1  設計模式及其作用 17

2.2  多線程設計模式簡介 20

2.3  設計模式的描述 21

第3章  Immutable Object（不可變對象）模式 23

3.1  Immutable Object模式簡介 23

3.2  Immutable Object模式的架構 25

3.3  Immutable Object模式實戰案例 27

3.4  Immutable Object模式的評價與實現考量 31

3.5  Immutable Object模式的可複用實現代碼 32

3.6  Java標準庫實例 32

3.7  相關模式 34

3.7.1  Thread Specific Storage模式（第10章） 34

3.7.2  Serial Thread Confinement模式（第11章） 34

3.8  參考資源 34

第4章  Guarded Suspension（保護性暫掛）模式 35

4.1  Guarded Suspension模式簡介 35

4.2  Guarded Suspension模式的架構 35

4.3  Guarded Suspension模式實戰案例解析 39

4.4  Guarded Suspension模式的評價與實現考量 45

4.4.1  內存可見性和鎖洩漏（Lock Leak） 46

4.4.2  線程過早被喚醒 46

4.4.3  嵌套監視器鎖死 47

4.5  Guarded Suspension模式的可複用實現代碼 50

4.6  Java標準庫實例 50

4.7  相關模式 51

4.7.1  Promise模式（第6章） 51

4.7.2  Producer-Consumer模式（第7章） 51

4.8  參考資源 51

第5章  Two-phase Termination（兩階段終止）模式 52

5.1  Two-phase Termination模式簡介 52

5.2  Two-phase Termination模式的架構 53

5.3  Two-phase Termination模式實戰案例 56

5.4  Two-phase Termination模式的評價與實現考量 63

5.4.1  線程停止標誌 63

5.4.2  生產者-消費者問題中的線程停止 64

5.4.3  隱藏而非暴露可停止的線程 65

5.5  Two-phase Termination模式的可複用實現代碼 65

5.6  Java標準庫實例 66

5.7  相關模式 66

5.7.1  Producer-Consumer模式（第7章） 66

5.7.2  Master-Slave模式（第12章） 66

5.8  參考資源 66

第6章  Promise（承諾）模式 67

6.1  Promise模式簡介 67

6.2  Promise模式的架構 68

6.3  Promise模式實戰案例解析 70

6.4  Promise模式的評價與實現考量 74

6.4.1  異步方法的異常處理 75

6.4.2  輪詢（Polling） 75

6.4.3  異步任務的執行 75

6.5  Promise模式的可複用實現代碼 77

6.6  Java標準庫實例 77

6.7  相關模式 78

6.7.1  Guarded Suspension模式（第4章） 78

6.7.2  Active Object模式（第8章） 78

6.7.3  Master-Slave模式（第12章） 78

6.7.4  Factory Method模式 78

6.8  參考資源 79

第7章  Producer-Consumer（生產者/消費者）模式 80

7.1  Producer-Consumer模式簡介 80

7.2  Producer-Consumer模式的架構 80

7.3  Producer-Consumer模式實戰案例解析 83

7.4  Producer-Consumer模式的評價與實現考量 87

7.4.1  通道積壓 87

7.4.2  工作竊取算法 88

7.4.3  線程的停止 92

7.4.4  高性能高可靠性的Producer-Consumer模式實現 92

7.5  Producer-Consumer模式的可複用實現代碼 92

7.6  Java標準庫實例 93

7.7  相關模式 93

7.7.1  Guarded Suspension模式（第4章） 93

7.7.2  Thread Pool模式（第9章） 93

7.8  參考資源 93

第8章  Active Object（主動對象）模式 94

8.1  Active Object模式簡介 94

8.2  Active Object模式的架構 95

8.3  Active Object模式實戰案例解析 98

8.4  Active Object模式的評價與實現考量 105

8.4.1  錯誤隔離 107

8.4.2  緩衝區監控 108

8.4.3  緩衝區飽和處理策略 108

8.4.4  Scheduler空閒工作者線程清理 109

8.5  Active Object模式的可複用實現代碼 109

8.6  Java標準庫實例 111

8.7  相關模式 112

8.7.1  Promise模式（第6章） 112

8.7.2  Producer-Consumer模式（第7章） 112

8.8  參考資源 112

第9章  Thread Pool（線程池）模式 113

9.1  Thread Pool模式簡介 113

9.2  Thread Pool模式的架構 114

9.3  Thread Pool模式實戰案例解析 116

9.4  Thread Pool模式的評價與實現考量 117

9.4.1  工作隊列的選擇 118

9.4.2  線程池大小調校 119

9.4.3  線程池監控 121

9.4.4  線程洩漏 122

9.4.5  可靠性與線程池飽和處理策略 122

9.4.6  死鎖 125

9.4.7  線程池空閒線程清理 126

9.5  Thread Pool模式的可複用實現代碼 127

9.6  Java標準庫實例 127

9.7  相關模式 127

9.7.1  Two-phase Termination模式（第5章） 127

9.7.2  Promise模式（第6章） 127

9.7.3  Producer-Consumer模式（第7章） 127

9.8  參考資源 128

第10章  Thread Specific Storage（線程特有存儲）模式 129

10.1  Thread Specific Storage模式簡介 129

10.2  Thread Specific Storage模式的架構 131

10.3  Thread Specific Storage模式實戰案例解析 133

10.4  Thread Specific Storage模式的評價與實現考量 135

10.4.1  線程池環境下使用Thread Specific Storage模式 138

10.4.2  內存洩漏與偽內存洩漏 139

10.5  Thread Specific Storage模式的可複用實現代碼 145

10.6  Java標準庫實例 146

10.7  相關模式 146

10.7.1  Immutable Object模式（第3章） 146

10.7.2  Proxy（代理）模式 146

10.7.3  Singleton（單例）模式 146

10.8  參考資源 147

第11章  Serial Thread Confinement（串行線程封閉）模式 148

11.1  Serial Thread Confinement模式簡介 148

11.2  Serial Thread Confinement模式的架構 148

11.3  Serial Thread Confinement模式實戰案例解析 151

11.4  Serial Thread Confinement模式的評價與實現考量 155

11.4.1  任務的處理結果 155

11.5  Serial Thread Confinement模式的可複用實現代碼 156

11.6  Java標準庫實例 160

11.7  相關模式 160

11.7.1  Immutable Object模式（第3章） 160

11.7.2  Promise模式（第6章） 160

11.7.3  Producer-Consumer模式（第7章） 160

11.7.4  Thread Specific Storage（線程特有存儲）模式 （第10章） 161

11.8  參考資源 161

 

第12章  Master-Slave（主僕）模式 162

12.1  Master-Slave模式簡介 162

12.2  Master-Slave模式的架構 162

12.3  Master-Slave模式實戰案例解析 164

12.4  Master-Slave模式的評價與實現考量 171

12.4.1  子任務的處理結果的收集 172

12.4.2  Slave參與者實例的負載均衡與工作竊取 173

12.4.3  可靠性與異常處理 173

12.4.4  Slave線程的停止 174

12.5  Master-Slave模式的可複用實現代碼 174

12.6  Java標準庫實例 186

12.7  相關模式 186

12.7.1  Two-phase Termination模式（第5章） 186

12.7.2  Promise模式（第6章） 186

12.7.3  Strategy（策略）模式 186

12.7.4  Template（模板）模式 186

12.7.5  Factory Method（工廠方法）模式 186

12.8  參考資源 187

第13章  Pipeline（流水線）模式 188

13.1  Pipeline模式簡介 188

13.2  Pipeline模式的架構 189

13.3  Pipeline模式實戰案例解析 194

13.4  Pipeline模式的評價與實現考量 208

13.4.1  Pipeline的深度 209

13.4.2  基於線程池的Pipe 209

13.4.3  錯誤處理 212

13.4.4  可配置的Pipeline 212

13.5  Pipeline模式的可複用實現代碼 212

13.6  Java標準庫實例 222

13.7  相關模式 222

13.7.1  Serial Thread Confinement模式（第11章） 222

13.7.2  Master-Slave模式（第12章） 222

13.7.3  Composite模式 223

13.8  參考資源 223

第14章  Half-sync/Half-async（半同步/半異步）模式 224

14.1  Half-sync/Half-async模式簡介 224

14.2  Half-sync/Half-async模式的架構 224

14.3  Half-sync/Half-async模式實戰案例解析 226

14.4  Half-sync/Half-async模式的評價與實現考量 234

14.4.1  隊列積壓 235

14.4.2  避免同步層處理過慢 235

14.5  Half-sync/Half-async模式的可複用實現代碼 236

14.6  Java標準庫實例 240

14.7  相關模式 240

14.7.1  Two-phase Termination模式（第5章） 240

14.7.2  Producer-Consumer模式（第7章） 241

14.7.3  Active Object模式（第8章） 241

14.7.4  Thread Pool模式（第9章） 241

14.8  參考資源 241

第15章  模式語言 242

15.1  模式與模式間的聯繫 242

15.2  mmutable Object（不可變對象）模式 244

15.3  Guarded Suspension（保護性暫掛）模式 244

15.4  Two-phase Termination（兩階段終止）模式 245

15.5  Promise（承諾）模式 246

15.6  Producer-Consumer（生產者/消費者）模式 247

15.7  Active Object（主動對象）模式 248

15.8  Thread Pool（線程池）模式 249

15.9  Thread Specific Storage（線程特有存儲）模式 250

15.10  Serial Thread Confinement（串行線程封閉）模式 251

15.11  Master-Slave（主僕）模式 252

15.12  Pipeline（流水線）模式 253

15.13  Half-sync/Half-async（半同步/半異步）模式 254

附錄  本書常用UML圖指南 255

A.1  UML簡介 255

A.2  類圖（Class Diagram） 256

A.1.1  類的屬性、方法和立體型（Stereotype） 256

A.1.2  類與類之間的關係 258

A.3  序列圖（Sequence Diagram） 261

參考文獻 263
