[讀書會] Linux環境編程：從應用到內核
===
###### tags: `tlkh`, `study-group`

# 書目及相關資源

* 書目：http://www.books.com.tw/products/CN11351825
	* [樣本電子書（含第一章）](https://docs.google.com/viewer?a=v&pid=forums&srcid=MTY5NzU4NjA5MDQ3NzkxOTM1MzIBMDUzMDc0Nzc1MzE3NDgyNjc3ODcBZ3ZBUHZWMjRCd0FKATAuMQEBdjI)
	* [電子書（中國亞馬遜）](https://www.amazon.cn/gp/product/B01H1RZTIM/ref=ox_sc_sfl_title_1?ie=UTF8&psc=1&smid=A1AJ19PSB66TGU)
	* [電子書（多看閱讀）](http://www.duokan.com/book/113882)
	* 書店狀況
		* 實體書店：大享書局/天瓏門市/若水堂。由於門市有時需要訂貨，建議在去門市前可以先電話詢問看看
		* 網路書店：淘寶、露天等
* 作者討論群組：[《linux環境編程》討論組 ](https://groups.google.com/forum/#!forum/linux_aple)

# 前言（引述自 Miller Lee）

最近有本 `Linux 環境編程` 我覺得那本不錯，就我的經驗 kernel 本身因為要支援上層 API，所以 code 會混雜很多上層的功能，從 POSIX  API 入門的話，我覺得應該會是比較好的方法！

可以把書中幾個沒有解釋清楚的點配合POSIX每個禮拜或每兩個禮拜大家可以好好見面討論(可能北車最方便？？)
像是第一章提到當 read 的 size 比 buffer size 大到時候
Read 可能會回傳或是 block。這樣可以對照 Source code 看是那些 socket 有這樣的行為還有 POSIX 是怎麼定義的

另外第一章可以跟Linux Kernel Development的VFS參照。

# 有興趣一起加入閱讀報告的勇者
[Miller Lee](https://www.facebook.com/win.lee.39566/about)
[Mike Shang](https://www.facebook.com/mshang816/about)
[Waylin Ch](https://www.facebook.com/waylin.ch/about)
[Viller Hsiao](https://www.facebook.com/villerhsiao/about) (villerhsiao at gmail.com)
[Wen Liao](https://www.facebook.com/satoshi.liao.5/about)
[Scott Tsai](https://www.facebook.com/scottt.tw/about)
[Clay Chang](https://www.facebook.com/clay.chang/about)
[SZ Lin](https://www.facebook.com/lock.lin711/about)
[Jian-Hong Pan(StarNight)](https://www.facebook.com/chienhung.pan/about)
[Winfred Lu](https://www.facebook.com/winfred.lu.9/about)
[Gavin Guo](https://www.facebook.com/profile.php?id=1782064975&sk=about&__mref=message)
[MingChia Chung (Quexint)](https://www.facebook.com/mingchia.chung)
[Yen-Kuan Wu](https://www.facebook.com/profile.php?id=100002466873353&sk=about)

# 分享形式

* 基本上預設大家事前都已經看了相關章節（？）。形式可以是
	* 一個人報告之後討論 或是
  	* 不只一人，主講人 + 有意願延伸分享的人 + 討論。(超過一個人大家可能要商量控制一下時間)
      * 有特定內容想分享，或是有問題想討論的人，事前將大綱在「類似會議記錄的文件」中寫下來。
* 大家自行認領(請在章節名單後面填上您的大名)
* 視訊會議，過程會有錄影或錄音之後會上傳 [社群 youtube 頻道](https://www.youtube.com/channel/UCzv1VG_o1MoQCA1G0uixMaA)。

# 版權
* 本次讀書會相關衍生資料（線上筆記與錄音錄影與討論內容），除特別聲明外，經當事人同意，皆採 [CC-BY](https://creativecommons.org/licenses/by/3.0/tw/)授權。


# 活動
## 第1章 文件I／O (9/8 7:00 pm Wen Liao 主講)
* [筆記與討論區](https://docs.google.com/document/d/1k1KZmkQtUebfNZ9-oo7flmQ7cc6DsHpRi5nMhMlA2wQ/edit?usp=sharing)  ([Chat log](https://docs.google.com/document/d/1LqxmygFcPv-MtU1yNqMpXG5uwyxW25aq8_riZOZnuvM/edit?usp=sharing), [後半錄音](https://www.youtube.com/watch?v=9VAQ_sxJcq4&feature=youtu.be), [錄音備份](https://drive.google.com/open?id=0B_FULIPcXRNDZnRnZ2ZfSXdoU3M))
## 第2章 標準I／O庫 (9/22 7:00 pm Quexint 主講) 
* [Quexint's Notes](https://docs.google.com/document/d/1fWUV9Ngk9aCZLsXf9gW-jTRq-x80y_bwmUrvPhrX38U/edit) ([Chat log](https://docs.google.com/document/d/1Bu3UJQY2BsjZHGUVsoA8t9iCM4mU8tomRLNxdkKLPSU/edit#heading=h.srhpj3tt1c71), [YouTube 錄影缺後半](https://www.youtube.com/watch?v=oYJUNRNmLxk&feature=youtu.be), [Youtube Quexint視角，中間缺一段](https://www.youtube.com/watch?v=V7vILjWjK7w))

## 第3章 進程環境 (10/27 7:00 pm Waylin 主講)
* 視訊 url: http://zoom.us/j/2109998888
* [筆記與討論區](https://docs.google.com/document/d/1lV_k8EvpIQzxpB5xU9e9jiZSvI-PP5DewcUk1JIFRCo/edit#heading=h.fod9jt9rbite)

## 安排中
* 第4章 進程控制：進程的一生
* 第5章 進程控制：狀態、調度和優先級
* 第6章 信號 (Scott Tsai, Viller)
* 第7章 理解Linux線程（1） (Ｖiller)
* 第8章 理解Linux線程（2） (Viller)
* 第9章 進程間通信：管道  (Yen-Kuan)
	* [筆記區](https://hackmd.io/KwIxwBgFmYFoIBMCMBTOVUA5Fy6AdjmRAE4BjYRAQ2gPyA==)
* 第10章 進程間通信：SystemV IPC (Yen-Kuan)
* 第11章 進程間通信：POSIX IPC (Yen-Kuan)
* 第12章 網絡通信：連接的建立 (Mike Shang, Winfred Lu)
* 第13章 網絡通信：數據報文的發送 (Mike Shang, Winfred Lu)
* 第14章 網絡通信：數據報文的接收 (Mike Shang, Winfred Lu)
* 第15章 編寫安全無錯代碼

# 如何參與?
* 如有興趣一起加入閱讀與報告者請更新勇者名單與主講章節。
* 如果有興趣瞭解並觀看我們報告的 video ，請登入 [twlinuxkernelhackers mailing list](https://groups.google.com/forum/#!forum/tw-linux-kernel-hacker)，我們會寄送相關活動邀請。 

# 作者回饋
	
	> From 李彬：   
	> 很榮幸收到你們的邀請，看到有臺灣讀者對我們的書感興趣非常高興。
	> 我看到了你們讀書會的時間是週四，時間上確實無法參加。
	> 我在一家小的公司工作，負責的工作挺多的，深表抱歉。
	> 我會關注你們的討論，也歡迎你們來google group討論組來討論。
	> 大家互相切磋，共同進步。
	> 代我向討論組其他同學問好。

	> From 高峰：
	> 很抱歉，這次不能參加你們的討論會。
	> 因為那個時間，一般我還是在工作中。
	> 很感謝臺灣的朋友們對本書的喜歡，從你們討論會的說明上看，臺灣似乎不好買到。
	> 不知道你們是否可以從京東訂購：http://item.jd.com/11962820.html。
	> 
	> 坦率講，由於時間和精力所限，這本書還沒有達到我和李彬的心中理想的質量。
	> 如果有機會再版的話，我們會修訂和補充一些內容。
	> 在group裡面，有一個置頂帖子，大家可以補充第一版沒有涉及到的內容。

	> 最近發現臺灣的朋友確實對本書有關注，所以跟出版社做了溝通。
	> 1. 出版社會聯繫臺灣的出版社，看看有在臺灣直接出版的可能；
		> Update from 高峰 2016/10/14
		> 現在臺灣的基峰出版社已經和大陸的出版社簽訂了出版合同。
		> 另外，我和李彬已經開始二版的計劃。
		除了修訂和改善第一版本的內容外，估計還會新增5~6個章節。
		估計一年內應該差不多可以在臺灣面世了
	> 2. 第二版計劃已經立項，我和李彬會開始著手第二版的工作：包括對第一版的內容修訂，和增加新的內容。

# Source 下載及編譯
* 環境: Ubuntu 14.04.5
*  kernel: 3.4.22
	*  git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
	*  cd linux-stable
	*  git checkout v3.4.22
	*  Build under Ubuntu 14.04 server
		*  `cp /boot/config-$(uname -r) .config`
		*  `yes '' | make oldconfig`
		*  `make clean`
		*  `make -j $(getconf _NPROCESSORS_ONLN) deb-pkg LOCALVERSION=-custom`
	* [Source code online reference](http://lxr.linux.no/linux+v3.4.22/)
	* [編譯參考資料](https://wiki.ubuntu.com/KernelTeam/GitKernelBuild)
*  glibc: 2.17
	```shell
	mkdir $HOME/src
	cd $HOME/src
	git clone git://sourceware.org/git/glibc.git
	cd glibc
	git checkout glibc-2.17
	mkdir -p $HOME/build/glibc/2.17
	cd $HOME/build/glibc
	$HOME/src/glibc/configure --prefix=$HOME/build/glibc/2.17
	make
	```