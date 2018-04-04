目錄(詳細)
=============
* 序章1　Java線程　　1 
* I1.1　Java線程　　2 
* I1.2　何謂線程　　2 
* 明為跟蹤處理流程，實為跟蹤線程　　2 
* 單線程程序　　3 
* 多線程程序　　4 
* Thread類的run方法和start方法　　5 
* I1.3　線程的啟動　　9 
* 線程的啟動（1）——利用Thread類的子類　　9 
* 線程的啟動（2）——利用Runnable接口　　10 
* I1.4　線程的暫停　　12 
* I1.5　線程的互斥處理　　13 
* synchronized方法　　14 
* synchronized代碼塊　　17 
* I1.6　線程的協作　　18 
* 等待隊列——線程休息室　　19 
* wait方法——將線程放入等待隊列　　19 
* notify方法——從等待隊列中取出線程　　21 
* notifyAll方法——從等待隊列中取出所有線程　　23 
* wait、notify、notifyAll是Object類的方法　　24 
* I1.7　線程的狀態遷移　　24 
* I1.8　線程相關的其他話題　　26 
* I1.9　本章所學知識　　26 
* I1.10　　練習題　　26 
* 序章2　多線程程序的評價標準　　31 
* I2.1　多線程程序的評價標準　　32 
* 安全性——不損壞對象　　32 
* 生存性——必要的處理能夠被執行　　32 
* 可複用性——類可重複利用　　33 
* 性能——能快速、大批量地執行處理　　33 
* 評價標準總結　　33 
* I2.2　本章所學知識　　34 
* I2.3　練習題　　34 
* 第1章　　Single Threaded Execution模式——能通過這座橋的只有一個人　　35 
* 1.1　Single Threaded Execution模式　　36 
* 1.2　示例程序1：不使用Single Threaded Execution模式的程序　　36 
* Main類　　37 
* 非線程安全的Gate類　　37 
* UserThread類　　38 
* 執行起來看看……出錯了　　39 
* 為什麼會出錯呢　　40 
* 1.3　示例程序2：使用Single Threaded Execution模式的程序　　41 
* 線程安全的Gate類　　41 
* synchronized的作用　　42 
* 1.4　Single Threaded Execution模式中的登場角色　　43 
* 1.5　拓展思路的要點　　44 
* 何時使用（可使用Single Threaded Execution模式的情況）　　44 
* 生存性與死鎖　　45 
* 可複用性和繼承反常　　46 
* 臨界區的大小和性能　　46 
* 1.6　相關的設計模式　　47 
* Guarded Suspension模式　　47 
* Read-Write Lock模式　　47 
* Immutable模式　　47 
* Thread-Specific Storage模式　　48 
* 1.7　延伸閱讀1：關於synchronized　　48 
* synchronized語法與Before/After模式　　48 
* synchronized在保護著什麼　　49 
* 該以什麼單位來保護呢　　50 
* 使用哪個鎖保護　　50 
* 原子操作　　51 
* long與double的操作不是原子的　　51 
* 1.8　延伸閱讀2：java.util.concurrent包和計數信號量　　52 
* 計數信號量和Semaphore類　　52 
* 使用Semaphore類的示例程序　　52 
* 1.9　本章所學知識　　55 
* 1.10　　練習題　　55 
* 第2章　　Immutable模式——想破壞也破壞不了　　61 
* 2.1　Immutable模式　　62 
* 2.2　示例程序　　62 
* 使用Immutable模式的Person類　　62 
* Main類　　63 
* PrintPersonThread類　　63 
* 2.3　Immutable模式中的登場角色　　65 
* 2.4　拓展思路的要點　　66 
* 何時使用（可使用Immutable模式的情況）　　66 
* 考慮成對的mutable類和immutable類 [性能]　　66 
* 為了確保不可變性 [可複用性]　　67 
* 標準類庫中用到的Immutable模式　　67 
* 2.5　相關的設計模式　　69 
* Single Threaded Execution模式　　69 
* Read-Write Lock模式　　69 
* Flyweight模式　　69 
* 2.6　延伸閱讀1：final　　69 
* final的含義　　69 
* 2.7　延伸閱讀2：集合類與多線程　　71 
* 示例1：非線程安全的java.util.ArrayList類　　71 
* 示例2：利用Collections.synchronizedList方法所進行的同步　　74 
* 示例3：使用copy-on-write的java.util.concurrent.CopyOnWriteArrayList類　　75 
* 2.8　本章所學知識　　76 
* 2.9　練習題　　77 
* 第3章　　Guarded Suspension模式——等我準備好哦　　81 
* 3.1　Guarded Suspension模式　　82 
* 3.2　示例程序　　82 
* Request類　　83 
* RequestQueue類　　84 
* ClientThread類　　85 
* ServerThread類　　85 
* Main類　　86 
* java.util.Queue與java.util.LinkedList的操作　　87 
* getRequest詳解　　87 
* putRequest詳解　　89 
* synchronized的含義　　89 
* wait與鎖　　89 
* 3.3　Guarded Suspension模式中的登場角色　　90 
* 3.4　拓展思路的要點　　91 
* 附加條件的synchronized　　91 
* 多線程版本的if　　91 
* 忘記改變狀態與生存性　　91 
* wait與notify/notifyAll的責任 [可複用性]　　91 
* 各種稱呼　　91 
* 使用java.util.concurrent.LinkedBlockingQueue的示例程序　　93 
* 3.5　相關的設計模式　　94 
* Single Threaded Execution模式　　94 
* Balking模式　　94 
* Producer-Consumer模式　　94 
* Future模式　　94 
* 3.6　本章所學知識　　95 
* 3.7　練習題　　95 
* 第4章　　Balking模式——不需要就算了　　99 
* 4.1　Balking模式　　100 
* 4.2　示例程序　　100 
* Data類　　100 
* SaverThread類　　102 
* ChangerThread類　　102 
* Main類　　103 
* 4.3　Balking模式中的登場角色　　105 
* 4.4　拓展思路的要點　　106 
* 何時使用（可使用Balking模式的情況）　　106 
* balk結果的表示方式　　107 
* 4.5　相關的設計模式　　107 
* Guarded Suspension模式　　107 
* Observer模式　　107 
* 4.6　延伸閱讀：超時　　108 
* Balking模式和Guarded Suspension模式之間　　108 
* wait何時終止呢　　108 
* guarded timed的實現（使用wait）　　109 
* synchronized中沒有超時，也不能中斷　　110 
* java.util.concurrent中的超時　　111 
* 4.7　本章所學知識　　111 
* 4.8　練習題　　112 
* 第5章　　Producer-Consumer模式——我來做，你來用　　115 
* 5.1　Producer-Consumer模式　　116 
* 5.2　示例程序　　116 
* Main類　　116 
* MakerThread類　　117 
* EaterThread類　　118 
* Table類　　118 
* 解讀put方法　　120 
* 解讀take方法　　121 
* 5.3　Producer-Consumer模式中的登場角色　　122 
* 5.4　拓展思路的要點　　123 
* 守護安全性的Channel角色（可複用性）　　123 
* 不可以直接傳遞嗎　　124 
* Channel角色的剩餘空間所導致的問題　　124 
* 以什麼順序傳遞Data角色呢　　125 
* “存在中間角色”的意義　　125 
* Consumer角色只有一個時會怎麼樣呢　　126 
* 5.5　相關的設計模式　　126 
* Mediator模式　　126 
* Worker Thread模式　　126 
* Command模式　　126 
* Strategy模式　　127 
* 5.6　延伸閱讀1：理解InterruptedException異常　　127 
* 可能會花費時間，但可以取消　　127 
* 加了throws InterruptedException的方法　　127 
* sleep方法和interrupt方法　　128 
* wait方法和interrupt方法　　128 
* join方法和interrupt方法　　129 
* interrupt方法只是改變中斷狀態　　129 
* isInterrupted方法——檢查中斷狀態　　130 
* Thread.interrupted方法——檢查並清除中斷狀態　　130 
* 不可以使用Thread類的stop方法　　130 
* 5.7　延伸閱讀2：java.util.concurrent包和Producer-Consumer模式　　131 
* java.util.concurrent包中的隊列　　131 
* 使用java.util.concurrent.ArrayBlockingQueue的示例程序　　132 
* 使用java.util.concurrent.Exchanger類交換緩衝區　　133 
* 5.8　本章所學知識　　136 
* 5.9　練習題　　137 
* 第6章　　Read-Write Lock模式——大家一起讀沒問題，但讀的時候不要寫哦　　141 
* 6.1　Read-Write Lock模式　　142 
* 6.2　示例程序　　142 
* Main類　　143 
* Data類　　143 
* WriterThread類　　146 
* ReaderThread類　　146 
* ReadWriteLock類　　147 
* 執行起來看看　　149 
* 守護條件的確認　　150 
* 6.3　Read-Write Lock模式中的登場角色　　151 
* 6.4　拓展思路的要點　　153 
* 利用“讀取”操作的線程之間不會衝突的特性來提高程序性能　　153 
* 適合讀取操作繁重時　　153 
* 適合讀取頻率比寫入頻率高時　　153 
* 鎖的含義　　153 
* 6.5　相關的設計模式　　154 
* Immutable模式　　154 
* Single Threaded Execution模式　　154 
* Guarded Suspension模式　　154 
* Before/After模式　　154 
* Strategized Locking模式　　154 
* 6.6　延伸閱讀：java.util.concurrent.locks包和Read-Write Lock模式　　154 
* java.util.concurrent.locks包　　154 
* 使用java.util.concurrent.locks的示例程序　　155 
* 6.7　本章所學知識　　156 
* 6.8　練習題　　157 
* 第7章　　Thread-Per-Message模式——這項工作就交給你了　　163 
* 7.1　Thread-Per-Message模式　　164 
* 7.2　示例程序　　164 
* Main類　　164 
* Host類　　165 
* Helper類　　166 
* 7.3　Thread-Per-Message模式中的登場角色　　168 
* 7.4　拓展思路的要點　　169 
* 提高響應性，縮短延遲時間　　169 
* 適用於操作順序沒有要求時　　169 
* 適用於不需要返回值時　　169 
* 應用於服務器　　169 
* 調用方法＋啟動線程→發送消息　　170 
* 7.5　相關的設計模式　　170 
* Future模式　　170 
* Worker Thread模式　　170 
* 7.6　延伸閱讀1：進程與線程　　171 
* 7.7　延伸閱讀2：java.util.concurrent包和Thread-Per-Message模式　　171 
* java.lang.Thread類　　171 
* java.lang.Runnable接口　　172 
* java.util.concurrent.ThreadFactory接口　　173 
* java.util.concurrent.Executors類獲取的ThreadFactory　　174 
* java.util.concurrent.Executor接口　　175 
* java.util.concurrent.ExecutorService接口　　176 
* java.util.concurrent.ScheduledExecutorService類　　177 
* 總結　　178 
* 7.8　本章所學知識　　180 
* 7.9　練習題　　180 
* 第8章　　Worker Thread模式——工作沒來就一直等，工作來了就幹活　　187 
* 8.1　Worker Thread模式　　188 
* 8.2　示例程序　　188 
* Main類　　189 
* ClientThread類　　190 
* Request類　　190 
* Channel類　　191 
* WorkerThread類　　192 
* 8.3　Worker Thread模式中的登場角色　　193 
* 8.4　拓展思路的要點　　195 
* 提高吞吐量　　195 
* 容量控制　　195 
* 調用與執行的分離　　196 
* Runnable接口的意義　　197 
* 多態的Request角色　　198 
* 獨自一人的Worker角色　　199 
* 8.5　相關的設計模式　　199 
* Producer-Consumer模式　　199 
* Thread-Per-Message模式　　199 
* Command模式　　199 
* Future模式　　199 
* Flyweight模式　　199 
* Thread-Specific Storage模式　　200 
* Active Ojbect模式　　200 
* 8.6　延伸閱讀1：Swing事件分發線程　　200 
* 什麼是事件分發線程　　200 
* 事件分發線程只有一個　　200 
* 事件分發線程調用監聽器　　201 
* 註冊監聽器的意義　　201 
* 事件分發線程也負責繪製界面　　201 
* javax.swing.SwingUtilities類　　202 
* Swing的單線程規則　　203 
* 8.7　延伸閱讀2：java.util.concurrent包和Worker Thread模式　　204 
* ThreadPoolExecutor類　　204 
* 通過java.util.concurrent包創建線程池　　205 
* 8.8　本章所學知識　　207 
* 8.9　練習題　　208 
* 第9章　　Future模式——先給您提貨單　　211 
* 9.1　Future模式　　212 
* 9.2　示例程序　　212 
* Main類　　214 
* Host類　　214 
* Data接口　　215 
* FutureData類　　216 
* RealData類　　217 
* 9.3　Future模式中的登場角色　　218 
* 9.4　拓展思路的要點　　219 
* 吞吐量會提高嗎　　219 
* 異步方法調用的“返回值”　　220 
* “準備返回值”和“使用返回值”的分離　　220 
* 變種——不讓主線程久等的Future角色　　220 
* 變種——會發生變化的Future角色　　221 
* 誰會在意多線程呢？“可複用性”　　221 
* 回調與Future模式　　221 
* 9.5　相關的設計模式　　222 
* Thread-Per-Message模式　　222 
* Builder模式　　222 
* Proxy模式　　222 
* Guarded Suspension模式　　222 
* Balking模式　　222 
* 9.6　延伸閱讀：java.util.concurrent包與Future模式　　222 
* java.util.concurrent包　　222 
* 使用了java.util.concurrent包的示例程序　　223 
* 9.7　本章所學知識　　226 
* 9.8　練習題　　226 
* 第10章　　Two-Phase Termination模式——先收拾房間再睡覺　　231 
* 10.1　Two-Phase Termination模式　　232 
* 10.2　示例程序　　233 
* CountupThread類　　234 
* Main類　　236 
* 10.3　Two-Phase Termination模式中的登場角色　　237 
* 10.4　拓展思路的要點　　238 
* 不能使用Thread類的stop方法　　238 
* 僅僅檢查標誌是不夠的　　239 
* 僅僅檢查中斷狀態是不夠的　　239 
* 在長時間處理前檢查終止請求　　239 
* join方法和isAlive方法　　240 
* java.util.concurrent.ExecutorService接口與Two-Phase Termination模式　　240 
* 要捕獲程序整體的終止時　　241 
* 優雅地終止線程　　243 
* 10.5　相關的設計模式　　243 
* Before/After模式　　243 
* Multiphase Cancellation模式　　243 
* Multi-Phase Startup模式　　244 
* Balking模式　　244 
* 10.6　延伸閱讀1：中斷狀態與InterruptedException異常的相互轉換　　244 
* 中斷狀態→InterruptedException異常的轉換　　244 
* InterruptedException異常→中斷狀態的轉換　　245 
* InterruptedException異常→InterruptedException異常的轉換　　245 
* 10.7　延伸閱讀2：java.util.concurrent包與線程同步　　246 
* java.util.concurrent.CountDownLatch類　　246 
* java.util.concurrent.CyclicBarrier類　　249 
* 10.8　本章所學知識　　253 
* 10.9　練習題　　253 
* 第11章　　Thread-Specific Storage模式——一個線程一個儲物櫃　　263 
* 11.1　Thread-Specific Storage模式　　264 
* 11.2　關於java.lang.ThreadLocal類　　264 
* java.lang.ThreadLocal就是儲物間　　264 
* java.lang.ThreadLocal與泛型　　265 
* 11.3　示例程序1：不使用Thread-Specific Storage模式的示例　　265 
* Log類　　266 
* Main類　　266 
* 11.4　示例程序2：使用了Thread-Specific Storage模式的示例　　267 
* 線程特有的TSLog類　　268 
* Log類　　269 
* ClientThread類　　270 
* Main類　　271 
* 11.5　Thread-Specific Storage模式中的登場角色　　272 
* 11.6　拓展思路的要點　　274 
* 局部變量與java.lang.ThreadLocal類　　274 
* 保存線程特有的信息的位置　　275 
* 不必擔心其他線程訪問　　275 
* 吞吐量的提高很大程序上取決於實現方式　　276 
* 上下文的危險性　　276 
* 11.7　相關的設計模式　　277 
* Singleton模式　　277 
* Worker Thread模式　　277 
* Single Threaded Execution模式　　277 
* Proxy模式　　277 
* 11.8　延伸閱讀：基於角色與基於任務　　277 
* 主體與客體　　277 
* 基於角色的考慮方式　　278 
* 基於任務的考慮方式　　278 
* 實際上兩種方式是綜合在一起的　　279 
* 11.9　本章所學知識　　279 
* 11.10　　練習題　　280 
* 第12章　　Active Object模式——接收異步消息的主動對象　　283 
* 12.1　Active Object模式　　284 
* 12.2　示例程序1　284 
* 調用方：Main類　　287 
* 調用方：MakerClientThread類　　288 
* 調用方：DisplayClientThread類　　289 
* 主動對象方：ActiveObject接口　　289 
* 主動對象方：ActiveObjectFactory類　　290 
* 主動對象方：Proxy類　　290 
* 主動對象方：SchedulerThread類　　291 
* 主動對象方：ActivationQueue類　　292 
* 主動對象方：MethodRequest類　　293 
* 主動對象方：MakeStringRequest類　　294 
* 主動對象方：DisplayStringRequest類　　295 
* 主動對象方：Result類　　295 
* 主動對象方：FutureResult類　　296 
* 主動對象方：RealResult類　　296 
* 主動對象方：Servant類　　297 
* 示例程序1的運行　　297 
* 12.3　ActiveObject模式中的登場角色　　298 
* 12.4　拓展思路的要點　　304 
* 到底做了些什麼事情　　304 
* 運用模式時需要考慮問題的粒度　　304 
* 關於併發性　　304 
* 增加方法　　305 
* Scheduler角色的作用　　305 
* 主動對象之間的交互　　306 
* 通往分佈式——從跨越線程界線變為跨越計算機界線　　306 
* 12.5　相關的設計模式　　306 
* Producer-Consumer模式　　306 
* Future模式　　307 
* Worker Thread模式　　307 
* Thread-Specific Storage模式　　307 
* 12.6　延伸閱讀：java.util.concurrent包與Active Object模式　　307 
* 類與接口　　307 
* 調用方：Main類　　309 
* 調用方：MakerClientThread類　　309 
* 調用方：DisplayClientThread類　　310 
* 主動對象方：ActiveObject接口　　311 
* 主動對象方：ActiveObjectFactory類　　311 
* 主動對象：ActiveObjectImpl類　　312 
* 示例程序2的運行　　313 
* 12.7　本章所學知識　　314 
* 12.8　練習題　　315 
* 第13章　　總結——多線程編程的模式語言　　321 
* 13.1　多線程編程的模式語言　　322 
* 模式與模式語言　　322 
* 13.2　Single Threaded Execution模式 
* ——能通過這座橋的只有一個人　　323 
* 13.3　Immutable模式 
* ——想破壞也破壞不了　　324 
* 13.4　Guarded Suspension模式 
* ——等我準備好哦　　325 
* 13.5　Balking模式 
* ——不需要就算了　　326 
* 13.6　Producer-Consumer模式 
* ——我來做，你來用　　327 
* 13.7　Read-Write Lock模式 
* ——大家一起讀沒問題，但讀的時候不要寫哦　　328 
* 13.8　Thread-Per-Message模式 
* ——這項工作就交給你了　　329 
* 13.9　Worker Thread模式 
* ——工作沒來就一直等，工作來了就幹活　　330 
* 13.10　　Future模式 
* ——先給您提貨單　　330 
* 13.11　Two-Phase Termination模式 
* ——先收拾房間再睡覺　　331 
* 13.12　Thread-Specific Storage模式 
* ——一個線程一個儲物櫃　　332 
* 13.13　Active Object模式 
* ——接收異步消息的主動對象　　333 
* 13.14　寫在最後　　335 
* 附錄　　337 
* 附錄A　習題解答　　338 
* 附錄B　Java內存模型　　447 
* 附錄C　Java線程的優先級　　467 
* 附錄D　線程相關的主要API　　469 
* 附錄E　java.util.concurrent包　　475 
* 附錄F　示例程序的運行步驟　　483 
* 附錄G　參考文獻　　485