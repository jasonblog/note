## Day 1：教練！我想學 Linux Kernel 啊！



這系列文章希望使用 eBPF 去理解運作在 Raspberry Pi 上的 Linux 核心之運作，並且觀察 Linux 核心如何與嵌入式系統的周邊互動，進而學習 Linux 核心。

這個挑戰的寫作動機背後有諸多要素。第一個是開發板越來越容易取得。2,000 元不到就能夠擁有介於個人電腦與嵌入式系統的開發環境，初學者 (比如說筆者本人) 可以在不用費心處理工具鏈、 交叉編譯、bootloader 的設置等複雜議題下，直接以類似個人電腦的方式進行嵌入式系統的開發。

另外一個因素則與 Linux 的發展有關。這方面啟發自 Steven Rostedt 在 2017 年的 Kernel Recipe 中，名為「Understanding the Linux Kernel via Ftrace」的演講。Steven Rostedt 在該演講中，示範如何以 ftrace 為工具來理解 Linux 核心的運作。而隨著 eBPF 漸漸成熟，這類方法勢必有能夠有更多元的使用。因此，除了閱讀核心原始碼或撰寫核心模組，eBPF 可以作為驗證自己理解是否正確的實證方法。

這些小心的特色最近逐漸開始在主要的發行版本中可以使用。以常見的 Raspberry Pi OS 為例，最新版本已經使用了 5.4 版的 Linux 核心。這意味著 eBPF 在 Raspberry Pi 上的發展逐漸成熟。加上 Raspberry Pi 兼具個人電腦與嵌入式系統的特徵，可以方便的加上週邊裝置。這兩者匯集在一起，就提供了一個認識 Linux 核心的可能方法：Maker 風格的快速 Prototype，配上 eBPF 近乎透視的功能，在作業系統與硬體周邊進行互動的同時，同時可以觀測到作業系統核心做了什麼。

在以往，編寫核心模組往往被視為理解 Linux 核心的灘頭堡。現在有了 eBPF，這就如同斥侯一般，可以先行探察核心如何運作，以減少不必要的錯誤，並且做到一些撰寫核心模組較難做到的觀測。

這 30 天的內容，希望以 Raspberry Model B 作為一個 Linux 的活體，搭配 eBPF、perf、ftrace 等工具，觀察 Raspberry Pi 在與 I2C、SPI、UART 等周邊裝置互動時，作業系統核心的行為。並且搭配核心的文件與近幾年 Linux Foundation 的研討會內容，嘗試撰寫實驗性的核心模組或相關程式碼，以達到學習 Linux 核心的效果。