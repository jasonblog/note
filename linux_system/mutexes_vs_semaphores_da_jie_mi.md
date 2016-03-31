# Mutexes VS Semaphores 大揭祕


Mutex 與 Semaphore 都是用在保護 critical section，確保多個 process 平行運作並存取資源時，執行結果不會因為執行程序的時間先後的影響而導致錯誤。
Mutex(Mutual Exclusion) 與 Semaphore 的差別在於:

- process 使用 mutex 時，process 的運作是持有 Mutex，執行critical section 來存取資源，然後釋放 Mutex. Mutex 就像是資源的一把鎖。

- process 使用 semaphore 時，process 總是發出信號(signal)，或者總是接收信號(wait)，同一個 process 不會先後進行 signal 與 wait。也就是說，process 要嘛當 producer，要嘛當 consumer，不能兩者都是。Semaphore 是為了保護 process 的執行同步性。

Mutex 與 Semaphore 要解決的是不同的問題。瞭解這個部份後，就可以來區分 mutex 與 binary semaphore。mutex 確保數個 process 在一個時間點上，只能有一個 process 存取單項資源，而 semaphore 則是讓數個 producer 與 數個 consumer 在訊號上進行合作。

另一個 mutex 與 binary semaphore 的差異在於，使用 mutex 會導致 priority inversion。也因此 mutex 中多半採用了一些機制來防止 Priority Inversion。Priority Inversion 是基於 process 持有 mutex 的概念，使得數個不同 priority 的 process ，在等待資源時透過 mutex 傳遞 priority，避免 priority inversion 發生。

註：Mutex與Semaphore在很多書、文章中都有很多不同的解釋。我認為這篇文章談的是最嚴格定義的 Semaphore (Dijkstra 所提出)，因此兩者有相當不同的區分。而後衍生出的 semaphore ，則包含了 counting semaphore 與 binary semaphore，也才能夠用於保護資源，或是處理 multiple identical resources 的問題。

Other Link:

- Difference between binary semaphore and mutex
- Wikipedia: Example: Producer/consumer problem