# Anaconda使用總結


##序
Python易用，但用好卻不易，其中比較頭疼的就是包管理和Python不同版本的問題，特別是當你使用Windows的時候。為瞭解決這些問題，有不少發行版的Python，比如WinPython、Anaconda等，這些發行版將python和許多常用的package打包，方便pythoners直接使用，此外，還有virtualenv、pyenv等工具管理虛擬環境。

個人嘗試了很多類似的發行版，最終選擇了Anaconda，因為其強大而方便的包管理與環境管理的功能。該文主要介紹下Anaconda，對Anaconda的理解，並簡要總結下相關的操作。

##Anaconda概述




Anaconda是一個用於科學計算的Python發行版，支持 Linux, Mac, Windows系統，提供了包管理與環境管理的功能，可以很方便地解決多版本python並存、切換以及各種第三方包安裝問題。Anaconda利用工具/命令conda來進行package和environment的管理，並且已經包含了Python和相關的配套工具。

這裡先解釋下conda、anaconda這些概念的差別。conda可以理解為一個工具，也是一個可執行命令，其核心功能是`包管理與環境管理`。包管理與pip的使用類似，環境管理則允許用戶方便地安裝不同版本的python並可以快速切換。Anaconda則是一個打包的集合，裡面預裝好了conda、某個版本的python、眾多packages、科學計算工具等等，所以也稱為Python的一種發行版。其實還有Miniconda，顧名思義，它只包含最基本的內容——python與conda，以及相關的必須依賴項，對於空間要求嚴格的用戶，Miniconda是一種選擇。

進入下文之前，說明一下conda的設計理念——`conda將幾乎所有的工具、第三方包都當做package對待，甚至包括python和conda自身`！因此，conda打破了包管理與環境管理的約束，能非常方便地安裝各種版本python、各種package並方便地切換。

##Anaconda的安裝
https://www.continuum.io/downloads#linux

Anaconda的下載頁參見官網下載，Linux、Mac、Windows均支持。

安裝時，會發現有兩個不同版本的Anaconda，分別對應Python 2.7和Python 3.5，兩個版本其實除了這點區別外其他都一樣。後面我們會看到，安裝哪個版本並不本質，因為通過環境管理，我們可以很方便地切換運行時的Python版本。（由於我常用的Python是2.7和3.4，因此傾向於直接安裝Python 2.7對應的Anaconda）

下載後直接按照說明安裝即可。這裡想提醒一點：儘量按照Anaconda默認的行為安裝——不使用root權限，僅為個人安裝，安裝目錄設置在個人主目錄下（Windows就無所謂了）。這樣的好處是，同一臺機器上的不同用戶完全可以安裝、配置自己的Anaconda，不會互相影響。

對於Mac、Linux系統，Anaconda安裝好後，實際上就是在主目錄下多了個文件夾（~/anaconda）而已，Windows會寫入註冊表。安裝時，安裝程序會把bin目錄加入PATH（Linux/Mac寫入~/.bashrc，Windows添加到系統變量PATH），這些操作也完全可以自己完成。以Linux/Mac為例，安裝完成後設置PATH的操作是

```sh
# 將anaconda的bin目錄加入PATH，根據版本不同，也可能是~/anaconda3/bin
echo 'export PATH="~/anaconda2/bin:$PATH"' >> ~/.bashrc
# 更新bashrc以立即生效
source ~/.bashrc
```

配置好PATH後，可以通過which conda或conda --version命令檢查是否正確。假如安裝的是Python 2.7對應的版本，運行python --version或python -V可以得到Python 2.7.12 :: Anaconda 4.1.1 (64-bit)，也說明該發行版默認的環境是Python 2.7。

##Conda的環境管理
Conda的環境管理功能允許我們同時安裝若干不同版本的Python，並能自由切換。對於上述安裝過程，假設我們採用的是Python 2.7對應的安裝包，那麼Python 2.7就是默認的環境（默認名字是root，注意這個root不是超級管理員的意思）。

假設我們需要安裝Python 3.4，此時，我們需要做的操作如下：

```sh
# 創建一個名為python34的環境，指定Python版本是3.4（不用管是3.4.x，conda會為我們自動尋找3.4.x中的最新版本）
conda create --name python34 python=3.4

# 安裝好後，使用activate激活某個環境
activate python34 # for Windows
source activate python34 # for Linux & Mac
# 激活後，會發現terminal輸入的地方多了python34的字樣，實際上，此時系統做的事情就是把默認2.7環境從PATH中去除，再把3.4對應的命令加入PATH

# 此時，再次輸入
python --version
# 可以得到`Python 3.4.5 :: Anaconda 4.1.1 (64-bit)`，即系統已經切換到了3.4的環境

# 如果想返回默認的python 2.7環境，運行
deactivate python34 # for Windows
source deactivate python34 # for Linux & Mac

# 刪除一個已有的環境
conda remove --name python34 --all
```

用戶安裝的不同python環境都會被放在目錄~/anaconda/envs下，可以在命令中運行`conda info -e`查看已安裝的環境，當前被激活的環境會顯示有一個星號或者括號。

說明：有些用戶可能經常使用python 3.4環境，因此直接把~/anaconda/envs/python34下面的bin或者Scripts加入PATH，去除anaconda對應的那個bin目錄。這個辦法，怎麼說呢，也是可以的，但總覺得不是那麼elegant……

如果直接按上面說的這麼改PATH，你會發現conda命令又找不到了（當然找不到啦，因為conda在~/anaconda/bin裡呢），這時候怎麼辦呢？方法有二：1. 顯式地給出conda的絕對地址 2. 在python34環境中也安裝conda工具（推薦）。

##Conda的包管理
Conda的包管理就比較好理解了，這部分功能與pip類似。

例如，如果需要安裝scipy：
```sh
# 安裝scipy
conda install scipy
# conda會從從遠程搜索scipy的相關信息和依賴項目，對於python 3.4，conda會同時安裝numpy和mkl（運算加速的庫）

# 查看已經安裝的packages
conda list
# 最新版的conda是從site-packages文件夾中搜索已經安裝的包，不依賴於pip，因此可以顯示出通過各種方式安裝的包

```


conda的一些常用操作如下：

```sh
# 查看當前環境下已安裝的包
conda list

# 查看某個指定環境的已安裝包
conda list -n python34

# 查找package信息
conda search numpy

# 安裝package
conda install -n python34 numpy
# 如果不用-n指定環境名稱，則被安裝在當前活躍環境
# 也可以通過-c指定通過某個channel安裝

# 更新package
conda update -n python34 numpy

# 刪除package
conda remove -n python34 numpy
```

前面已經提到，conda將conda、python等都視為package，因此，完全可以使用conda來管理conda和python的版本，例如

```sh
# 更新conda，保持conda最新
conda update conda

# 更新anaconda
conda update anaconda

# 更新python
conda update python
# 假設當前環境是python 3.4, conda會將python升級為3.4.x系列的當前最新版本
```

補充：如果創建新的python環境，比如3.4，運行`conda create -n python34 python=3.4`之後，conda僅安裝python 3.4相關的必須項，如python, pip等，如果希望該環境像默認環境那樣，安裝anaconda集合包，只需要：

```sg
# 在當前環境下安裝anaconda包集合
conda install anaconda

# 結合創建環境的命令，以上操作可以合併為
conda create -n python34 python=3.4 anaconda
# 也可以不用全部安裝，根據需求安裝自己需要的package即可
```

##設置國內鏡像
如果需要安裝很多packages，你會發現conda下載的速度經常很慢，因為Anaconda.org的服務器在國外。所幸的是，清華TUNA鏡像源有Anaconda倉庫的鏡像，我們將其加入conda的配置即可：

```sh
# 添加Anaconda的TUNA鏡像
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
# TUNA的help中鏡像地址加有引號，需要去掉

# 設置搜索時顯示通道地址
conda config --set show_channel_urls yes
```

執行完上述命令後，會生成~/.condarc(Linux/Mac)或C:\Users\USER_NAME\.condarc文件，記錄著我們對conda的配置，直接手動創建、編輯該文件是相同的效果。


Anaconda具有跨平臺、包管理、環境管理的特點，因此很適合快速在新的機器上部署Python環境。總結而言，整套安裝、配置流程如下：

下載Anaconda、安裝
配置PATH（bashrc或環境變量），更改TUNA鏡像源
創建所需的不用版本的python環境
Just Try!
cheat-sheet 下載：
Conda cheat sheet

##參考資料

- Anaconda Homepage
- Anaconda Documentation
- Conda Docs

文／PeterYuan（簡書作者）
原文鏈接：http://www.jianshu.com/p/2f3be7781451
著作權歸作者所有，轉載請聯繫作者獲得授權，並標註“簡書作者”。