# 並行計算工具的選擇


並行計算是未來的發展趨勢，如何選擇並行的架構是個重要的問題。

首先要發揮集群計算的能力，用MPI是個不二的選擇。

其次為了充分利用cpu和gpu的多核計算能力，有一系列的庫可以選擇，openMP openACC CUDA OpenCL ，openMP openACC是同一層次的接口，是`share memory並行`，openACC比openMP有所擴展，主要是在GPU的方面，這一層的實現比較簡單，如果這一層已經足夠了，不要刻意把問題複雜化使用更復雜的調用接口，CUDA OpenCL差不多是一層，openCL的優勢在於擴展行強，他只是一個通用的標準，和MPI一樣有很多實現，所以對於不同的多核硬件都可以很好的支持，但是Cuda是Nvidia的特別實現，效率上會比openCL略勝一籌，然後還有一個參考是基於前面架構有沒有一些現成的庫可以使用，比如fft blas等等。

<br><ul><li><b>CUDA</b>: 貌似上面的前輩們都已經總結了優缺點了，再敘述一遍吧。N家的多線程計算平臺和API，支持GPU。</li><ul><li>關鍵詞： 既有平臺又是API</li><li>語言支持： C/C++, Fortran</li><li>優點：版本更行快，社區活躍, 支持GPU</li><li>缺點：只支持N卡</li></ul><li><b>OpenCL</b>: 不是很瞭解，搬運下wiki的內容吧。看這wiki的內容，感覺不明覺厲啊。異構平臺，支持CPU，GPU， DSPs， FPGAs 等。在多進程方面使用 task-based 和 data-based parallelism。</li><ul><li>關鍵詞： 平臺 + API</li><li>語言支持： C/C++</li><li>優點： 跨好多平臺啊</li><li>缺點：版本更新慢</li></ul><li><b>OpenMP</b>: 這是directive-based parallelism。在寫完一般的代碼後，轉成多線程需要改動的地方不是很大，但只能支持shared memory（共享存儲？），是一個管理線程級別的庫。</li><ul><li>關鍵詞： API</li><li>語言支持： C/C++, Fortran</li><li>優點：移植改動少，支持普遍（普通的gcc都支持）</li><li>缺點：只支持shared memory</li></ul><li><b>OpenACC</b>:  支持CPU/GPU工作，從名字Accelerator就可以推斷出，主要用於CPU代碼向GPU的移植（？有待考證，我在課程中主要是這麼用的。。。），更新方面其實也不快，需要注意編程技巧，否則代碼會變慢。</li><ul><li>關鍵詞： API</li><li>語言支持： C/C++, Fortran</li><li>優點： 移植簡單</li><li>缺點： 編譯器啊，支持PGI，看計劃有支持GCC，但還不知道什麼時候才能發佈</li></ul><li><b>OpenMPI</b>:  消息傳遞接口庫。支持分佈式存儲，適合大規模集群服務器。另外，需要手動管理數據的分佈和消息的傳遞，需要重新修改代碼。</li><ul><li>關鍵詞： API</li><li>語言支持： C/C++</li><li>優點： 支持共享存儲和分佈式存儲</li><li>缺點： 模型複雜，需要重構代碼。</li>