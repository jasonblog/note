目录(详细)
=============
* 序章1　Java线程　　1 
* I1.1　Java线程　　2 
* I1.2　何谓线程　　2 
* 明为跟踪处理流程，实为跟踪线程　　2 
* 单线程程序　　3 
* 多线程程序　　4 
* Thread类的run方法和start方法　　5 
* I1.3　线程的启动　　9 
* 线程的启动（1）——利用Thread类的子类　　9 
* 线程的启动（2）——利用Runnable接口　　10 
* I1.4　线程的暂停　　12 
* I1.5　线程的互斥处理　　13 
* synchronized方法　　14 
* synchronized代码块　　17 
* I1.6　线程的协作　　18 
* 等待队列——线程休息室　　19 
* wait方法——将线程放入等待队列　　19 
* notify方法——从等待队列中取出线程　　21 
* notifyAll方法——从等待队列中取出所有线程　　23 
* wait、notify、notifyAll是Object类的方法　　24 
* I1.7　线程的状态迁移　　24 
* I1.8　线程相关的其他话题　　26 
* I1.9　本章所学知识　　26 
* I1.10　　练习题　　26 
* 序章2　多线程程序的评价标准　　31 
* I2.1　多线程程序的评价标准　　32 
* 安全性——不损坏对象　　32 
* 生存性——必要的处理能够被执行　　32 
* 可复用性——类可重复利用　　33 
* 性能——能快速、大批量地执行处理　　33 
* 评价标准总结　　33 
* I2.2　本章所学知识　　34 
* I2.3　练习题　　34 
* 第1章　　Single Threaded Execution模式——能通过这座桥的只有一个人　　35 
* 1.1　Single Threaded Execution模式　　36 
* 1.2　示例程序1：不使用Single Threaded Execution模式的程序　　36 
* Main类　　37 
* 非线程安全的Gate类　　37 
* UserThread类　　38 
* 执行起来看看……出错了　　39 
* 为什么会出错呢　　40 
* 1.3　示例程序2：使用Single Threaded Execution模式的程序　　41 
* 线程安全的Gate类　　41 
* synchronized的作用　　42 
* 1.4　Single Threaded Execution模式中的登场角色　　43 
* 1.5　拓展思路的要点　　44 
* 何时使用（可使用Single Threaded Execution模式的情况）　　44 
* 生存性与死锁　　45 
* 可复用性和继承反常　　46 
* 临界区的大小和性能　　46 
* 1.6　相关的设计模式　　47 
* Guarded Suspension模式　　47 
* Read-Write Lock模式　　47 
* Immutable模式　　47 
* Thread-Specific Storage模式　　48 
* 1.7　延伸阅读1：关于synchronized　　48 
* synchronized语法与Before/After模式　　48 
* synchronized在保护着什么　　49 
* 该以什么单位来保护呢　　50 
* 使用哪个锁保护　　50 
* 原子操作　　51 
* long与double的操作不是原子的　　51 
* 1.8　延伸阅读2：java.util.concurrent包和计数信号量　　52 
* 计数信号量和Semaphore类　　52 
* 使用Semaphore类的示例程序　　52 
* 1.9　本章所学知识　　55 
* 1.10　　练习题　　55 
* 第2章　　Immutable模式——想破坏也破坏不了　　61 
* 2.1　Immutable模式　　62 
* 2.2　示例程序　　62 
* 使用Immutable模式的Person类　　62 
* Main类　　63 
* PrintPersonThread类　　63 
* 2.3　Immutable模式中的登场角色　　65 
* 2.4　拓展思路的要点　　66 
* 何时使用（可使用Immutable模式的情况）　　66 
* 考虑成对的mutable类和immutable类 [性能]　　66 
* 为了确保不可变性 [可复用性]　　67 
* 标准类库中用到的Immutable模式　　67 
* 2.5　相关的设计模式　　69 
* Single Threaded Execution模式　　69 
* Read-Write Lock模式　　69 
* Flyweight模式　　69 
* 2.6　延伸阅读1：final　　69 
* final的含义　　69 
* 2.7　延伸阅读2：集合类与多线程　　71 
* 示例1：非线程安全的java.util.ArrayList类　　71 
* 示例2：利用Collections.synchronizedList方法所进行的同步　　74 
* 示例3：使用copy-on-write的java.util.concurrent.CopyOnWriteArrayList类　　75 
* 2.8　本章所学知识　　76 
* 2.9　练习题　　77 
* 第3章　　Guarded Suspension模式——等我准备好哦　　81 
* 3.1　Guarded Suspension模式　　82 
* 3.2　示例程序　　82 
* Request类　　83 
* RequestQueue类　　84 
* ClientThread类　　85 
* ServerThread类　　85 
* Main类　　86 
* java.util.Queue与java.util.LinkedList的操作　　87 
* getRequest详解　　87 
* putRequest详解　　89 
* synchronized的含义　　89 
* wait与锁　　89 
* 3.3　Guarded Suspension模式中的登场角色　　90 
* 3.4　拓展思路的要点　　91 
* 附加条件的synchronized　　91 
* 多线程版本的if　　91 
* 忘记改变状态与生存性　　91 
* wait与notify/notifyAll的责任 [可复用性]　　91 
* 各种称呼　　91 
* 使用java.util.concurrent.LinkedBlockingQueue的示例程序　　93 
* 3.5　相关的设计模式　　94 
* Single Threaded Execution模式　　94 
* Balking模式　　94 
* Producer-Consumer模式　　94 
* Future模式　　94 
* 3.6　本章所学知识　　95 
* 3.7　练习题　　95 
* 第4章　　Balking模式——不需要就算了　　99 
* 4.1　Balking模式　　100 
* 4.2　示例程序　　100 
* Data类　　100 
* SaverThread类　　102 
* ChangerThread类　　102 
* Main类　　103 
* 4.3　Balking模式中的登场角色　　105 
* 4.4　拓展思路的要点　　106 
* 何时使用（可使用Balking模式的情况）　　106 
* balk结果的表示方式　　107 
* 4.5　相关的设计模式　　107 
* Guarded Suspension模式　　107 
* Observer模式　　107 
* 4.6　延伸阅读：超时　　108 
* Balking模式和Guarded Suspension模式之间　　108 
* wait何时终止呢　　108 
* guarded timed的实现（使用wait）　　109 
* synchronized中没有超时，也不能中断　　110 
* java.util.concurrent中的超时　　111 
* 4.7　本章所学知识　　111 
* 4.8　练习题　　112 
* 第5章　　Producer-Consumer模式——我来做，你来用　　115 
* 5.1　Producer-Consumer模式　　116 
* 5.2　示例程序　　116 
* Main类　　116 
* MakerThread类　　117 
* EaterThread类　　118 
* Table类　　118 
* 解读put方法　　120 
* 解读take方法　　121 
* 5.3　Producer-Consumer模式中的登场角色　　122 
* 5.4　拓展思路的要点　　123 
* 守护安全性的Channel角色（可复用性）　　123 
* 不可以直接传递吗　　124 
* Channel角色的剩余空间所导致的问题　　124 
* 以什么顺序传递Data角色呢　　125 
* “存在中间角色”的意义　　125 
* Consumer角色只有一个时会怎么样呢　　126 
* 5.5　相关的设计模式　　126 
* Mediator模式　　126 
* Worker Thread模式　　126 
* Command模式　　126 
* Strategy模式　　127 
* 5.6　延伸阅读1：理解InterruptedException异常　　127 
* 可能会花费时间，但可以取消　　127 
* 加了throws InterruptedException的方法　　127 
* sleep方法和interrupt方法　　128 
* wait方法和interrupt方法　　128 
* join方法和interrupt方法　　129 
* interrupt方法只是改变中断状态　　129 
* isInterrupted方法——检查中断状态　　130 
* Thread.interrupted方法——检查并清除中断状态　　130 
* 不可以使用Thread类的stop方法　　130 
* 5.7　延伸阅读2：java.util.concurrent包和Producer-Consumer模式　　131 
* java.util.concurrent包中的队列　　131 
* 使用java.util.concurrent.ArrayBlockingQueue的示例程序　　132 
* 使用java.util.concurrent.Exchanger类交换缓冲区　　133 
* 5.8　本章所学知识　　136 
* 5.9　练习题　　137 
* 第6章　　Read-Write Lock模式——大家一起读没问题，但读的时候不要写哦　　141 
* 6.1　Read-Write Lock模式　　142 
* 6.2　示例程序　　142 
* Main类　　143 
* Data类　　143 
* WriterThread类　　146 
* ReaderThread类　　146 
* ReadWriteLock类　　147 
* 执行起来看看　　149 
* 守护条件的确认　　150 
* 6.3　Read-Write Lock模式中的登场角色　　151 
* 6.4　拓展思路的要点　　153 
* 利用“读取”操作的线程之间不会冲突的特性来提高程序性能　　153 
* 适合读取操作繁重时　　153 
* 适合读取频率比写入频率高时　　153 
* 锁的含义　　153 
* 6.5　相关的设计模式　　154 
* Immutable模式　　154 
* Single Threaded Execution模式　　154 
* Guarded Suspension模式　　154 
* Before/After模式　　154 
* Strategized Locking模式　　154 
* 6.6　延伸阅读：java.util.concurrent.locks包和Read-Write Lock模式　　154 
* java.util.concurrent.locks包　　154 
* 使用java.util.concurrent.locks的示例程序　　155 
* 6.7　本章所学知识　　156 
* 6.8　练习题　　157 
* 第7章　　Thread-Per-Message模式——这项工作就交给你了　　163 
* 7.1　Thread-Per-Message模式　　164 
* 7.2　示例程序　　164 
* Main类　　164 
* Host类　　165 
* Helper类　　166 
* 7.3　Thread-Per-Message模式中的登场角色　　168 
* 7.4　拓展思路的要点　　169 
* 提高响应性，缩短延迟时间　　169 
* 适用于操作顺序没有要求时　　169 
* 适用于不需要返回值时　　169 
* 应用于服务器　　169 
* 调用方法＋启动线程→发送消息　　170 
* 7.5　相关的设计模式　　170 
* Future模式　　170 
* Worker Thread模式　　170 
* 7.6　延伸阅读1：进程与线程　　171 
* 7.7　延伸阅读2：java.util.concurrent包和Thread-Per-Message模式　　171 
* java.lang.Thread类　　171 
* java.lang.Runnable接口　　172 
* java.util.concurrent.ThreadFactory接口　　173 
* java.util.concurrent.Executors类获取的ThreadFactory　　174 
* java.util.concurrent.Executor接口　　175 
* java.util.concurrent.ExecutorService接口　　176 
* java.util.concurrent.ScheduledExecutorService类　　177 
* 总结　　178 
* 7.8　本章所学知识　　180 
* 7.9　练习题　　180 
* 第8章　　Worker Thread模式——工作没来就一直等，工作来了就干活　　187 
* 8.1　Worker Thread模式　　188 
* 8.2　示例程序　　188 
* Main类　　189 
* ClientThread类　　190 
* Request类　　190 
* Channel类　　191 
* WorkerThread类　　192 
* 8.3　Worker Thread模式中的登场角色　　193 
* 8.4　拓展思路的要点　　195 
* 提高吞吐量　　195 
* 容量控制　　195 
* 调用与执行的分离　　196 
* Runnable接口的意义　　197 
* 多态的Request角色　　198 
* 独自一人的Worker角色　　199 
* 8.5　相关的设计模式　　199 
* Producer-Consumer模式　　199 
* Thread-Per-Message模式　　199 
* Command模式　　199 
* Future模式　　199 
* Flyweight模式　　199 
* Thread-Specific Storage模式　　200 
* Active Ojbect模式　　200 
* 8.6　延伸阅读1：Swing事件分发线程　　200 
* 什么是事件分发线程　　200 
* 事件分发线程只有一个　　200 
* 事件分发线程调用监听器　　201 
* 注册监听器的意义　　201 
* 事件分发线程也负责绘制界面　　201 
* javax.swing.SwingUtilities类　　202 
* Swing的单线程规则　　203 
* 8.7　延伸阅读2：java.util.concurrent包和Worker Thread模式　　204 
* ThreadPoolExecutor类　　204 
* 通过java.util.concurrent包创建线程池　　205 
* 8.8　本章所学知识　　207 
* 8.9　练习题　　208 
* 第9章　　Future模式——先给您提货单　　211 
* 9.1　Future模式　　212 
* 9.2　示例程序　　212 
* Main类　　214 
* Host类　　214 
* Data接口　　215 
* FutureData类　　216 
* RealData类　　217 
* 9.3　Future模式中的登场角色　　218 
* 9.4　拓展思路的要点　　219 
* 吞吐量会提高吗　　219 
* 异步方法调用的“返回值”　　220 
* “准备返回值”和“使用返回值”的分离　　220 
* 变种——不让主线程久等的Future角色　　220 
* 变种——会发生变化的Future角色　　221 
* 谁会在意多线程呢？“可复用性”　　221 
* 回调与Future模式　　221 
* 9.5　相关的设计模式　　222 
* Thread-Per-Message模式　　222 
* Builder模式　　222 
* Proxy模式　　222 
* Guarded Suspension模式　　222 
* Balking模式　　222 
* 9.6　延伸阅读：java.util.concurrent包与Future模式　　222 
* java.util.concurrent包　　222 
* 使用了java.util.concurrent包的示例程序　　223 
* 9.7　本章所学知识　　226 
* 9.8　练习题　　226 
* 第10章　　Two-Phase Termination模式——先收拾房间再睡觉　　231 
* 10.1　Two-Phase Termination模式　　232 
* 10.2　示例程序　　233 
* CountupThread类　　234 
* Main类　　236 
* 10.3　Two-Phase Termination模式中的登场角色　　237 
* 10.4　拓展思路的要点　　238 
* 不能使用Thread类的stop方法　　238 
* 仅仅检查标志是不够的　　239 
* 仅仅检查中断状态是不够的　　239 
* 在长时间处理前检查终止请求　　239 
* join方法和isAlive方法　　240 
* java.util.concurrent.ExecutorService接口与Two-Phase Termination模式　　240 
* 要捕获程序整体的终止时　　241 
* 优雅地终止线程　　243 
* 10.5　相关的设计模式　　243 
* Before/After模式　　243 
* Multiphase Cancellation模式　　243 
* Multi-Phase Startup模式　　244 
* Balking模式　　244 
* 10.6　延伸阅读1：中断状态与InterruptedException异常的相互转换　　244 
* 中断状态→InterruptedException异常的转换　　244 
* InterruptedException异常→中断状态的转换　　245 
* InterruptedException异常→InterruptedException异常的转换　　245 
* 10.7　延伸阅读2：java.util.concurrent包与线程同步　　246 
* java.util.concurrent.CountDownLatch类　　246 
* java.util.concurrent.CyclicBarrier类　　249 
* 10.8　本章所学知识　　253 
* 10.9　练习题　　253 
* 第11章　　Thread-Specific Storage模式——一个线程一个储物柜　　263 
* 11.1　Thread-Specific Storage模式　　264 
* 11.2　关于java.lang.ThreadLocal类　　264 
* java.lang.ThreadLocal就是储物间　　264 
* java.lang.ThreadLocal与泛型　　265 
* 11.3　示例程序1：不使用Thread-Specific Storage模式的示例　　265 
* Log类　　266 
* Main类　　266 
* 11.4　示例程序2：使用了Thread-Specific Storage模式的示例　　267 
* 线程特有的TSLog类　　268 
* Log类　　269 
* ClientThread类　　270 
* Main类　　271 
* 11.5　Thread-Specific Storage模式中的登场角色　　272 
* 11.6　拓展思路的要点　　274 
* 局部变量与java.lang.ThreadLocal类　　274 
* 保存线程特有的信息的位置　　275 
* 不必担心其他线程访问　　275 
* 吞吐量的提高很大程序上取决于实现方式　　276 
* 上下文的危险性　　276 
* 11.7　相关的设计模式　　277 
* Singleton模式　　277 
* Worker Thread模式　　277 
* Single Threaded Execution模式　　277 
* Proxy模式　　277 
* 11.8　延伸阅读：基于角色与基于任务　　277 
* 主体与客体　　277 
* 基于角色的考虑方式　　278 
* 基于任务的考虑方式　　278 
* 实际上两种方式是综合在一起的　　279 
* 11.9　本章所学知识　　279 
* 11.10　　练习题　　280 
* 第12章　　Active Object模式——接收异步消息的主动对象　　283 
* 12.1　Active Object模式　　284 
* 12.2　示例程序1　284 
* 调用方：Main类　　287 
* 调用方：MakerClientThread类　　288 
* 调用方：DisplayClientThread类　　289 
* 主动对象方：ActiveObject接口　　289 
* 主动对象方：ActiveObjectFactory类　　290 
* 主动对象方：Proxy类　　290 
* 主动对象方：SchedulerThread类　　291 
* 主动对象方：ActivationQueue类　　292 
* 主动对象方：MethodRequest类　　293 
* 主动对象方：MakeStringRequest类　　294 
* 主动对象方：DisplayStringRequest类　　295 
* 主动对象方：Result类　　295 
* 主动对象方：FutureResult类　　296 
* 主动对象方：RealResult类　　296 
* 主动对象方：Servant类　　297 
* 示例程序1的运行　　297 
* 12.3　ActiveObject模式中的登场角色　　298 
* 12.4　拓展思路的要点　　304 
* 到底做了些什么事情　　304 
* 运用模式时需要考虑问题的粒度　　304 
* 关于并发性　　304 
* 增加方法　　305 
* Scheduler角色的作用　　305 
* 主动对象之间的交互　　306 
* 通往分布式——从跨越线程界线变为跨越计算机界线　　306 
* 12.5　相关的设计模式　　306 
* Producer-Consumer模式　　306 
* Future模式　　307 
* Worker Thread模式　　307 
* Thread-Specific Storage模式　　307 
* 12.6　延伸阅读：java.util.concurrent包与Active Object模式　　307 
* 类与接口　　307 
* 调用方：Main类　　309 
* 调用方：MakerClientThread类　　309 
* 调用方：DisplayClientThread类　　310 
* 主动对象方：ActiveObject接口　　311 
* 主动对象方：ActiveObjectFactory类　　311 
* 主动对象：ActiveObjectImpl类　　312 
* 示例程序2的运行　　313 
* 12.7　本章所学知识　　314 
* 12.8　练习题　　315 
* 第13章　　总结——多线程编程的模式语言　　321 
* 13.1　多线程编程的模式语言　　322 
* 模式与模式语言　　322 
* 13.2　Single Threaded Execution模式 
* ——能通过这座桥的只有一个人　　323 
* 13.3　Immutable模式 
* ——想破坏也破坏不了　　324 
* 13.4　Guarded Suspension模式 
* ——等我准备好哦　　325 
* 13.5　Balking模式 
* ——不需要就算了　　326 
* 13.6　Producer-Consumer模式 
* ——我来做，你来用　　327 
* 13.7　Read-Write Lock模式 
* ——大家一起读没问题，但读的时候不要写哦　　328 
* 13.8　Thread-Per-Message模式 
* ——这项工作就交给你了　　329 
* 13.9　Worker Thread模式 
* ——工作没来就一直等，工作来了就干活　　330 
* 13.10　　Future模式 
* ——先给您提货单　　330 
* 13.11　Two-Phase Termination模式 
* ——先收拾房间再睡觉　　331 
* 13.12　Thread-Specific Storage模式 
* ——一个线程一个储物柜　　332 
* 13.13　Active Object模式 
* ——接收异步消息的主动对象　　333 
* 13.14　写在最后　　335 
* 附录　　337 
* 附录A　习题解答　　338 
* 附录B　Java内存模型　　447 
* 附录C　Java线程的优先级　　467 
* 附录D　线程相关的主要API　　469 
* 附录E　java.util.concurrent包　　475 
* 附录F　示例程序的运行步骤　　483 
* 附录G　参考文献　　485