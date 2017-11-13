# 用 Anaconda 完美解決 Python2 和 python3 共存問題


Python3 被越來越多的開發者所接受，同時讓人尷尬的是很多遺留的老系統依舊運行在 Python2 的環境中，因此有時你不得不同時在兩個版本中進行開發，調試。

如何在系統中同時共存 Python2 和 Python3 是開發者不得不面對的問題，一個利好的消息是，Anaconda 能完美解決Python2 和 Python3 的共存問題，而且在 Windows 平臺經常出現安裝依賴包（比如 MySQL-python）失敗的情況也得以解決。

##Anaconda 是什麼？

Anaconda 是 Python 的一個發行版，如果把 Python 比作 Linux，那麼 Anancoda 就是 CentOS 或者 Ubuntu。它解決了Python開發者的兩大痛點。

- 第一：提供包管理，功能類似於 pip，Windows 平臺安裝第三方包經常失敗的場景得以解決。-
- 第二：提供虛擬環境管理，功能類似於 virtualenv，解決了多版本Python並存問題。

##下載 Anaconda

直接在官網下載最新版本的 https://www.continuum.io/downloads 安裝包， 選擇 Python3.6 版本的安裝包，下載完成後直接安裝，安裝過程選擇默認配置即可，大約需要1.8G的磁盤空間。

`conda` 是 Anaconda 下用於包管理和環境管理的命令行工具，是 pip 和 vitualenv 的組合。安裝成功後 conda 會默認加入到環境變量中，因此可直接在命令行窗口運行 conda 命令

如果你熟悉 virtualenv，那麼上手 conda 非常容易，不熟悉 virtulenv 的也沒關係，它提供的命令就幾個，非常簡單。我們可以利用 conda 的虛擬環境管理功能在 Python2 和 Python3 之間自由切換。

##多版本切換


```sh
# 基於 python3.6 創建一個名為test_py3 的環境
conda create --name test_py3 python=3.6 

# 基於 python2.7 創建一個名為test_py2 的環境
conda create --name test_py2 python=2.7

# 激活 test 環境
activate test_py2  # windows
source activate test_py2 # linux/mac

# 切換到python3
activate test_py3
```

更多命令，可查看幫助 conda -h



## 包管理工具

```sh 
# 安裝 matplotlib 
conda install matplotlib
# 查看已安裝的包
conda list 
# 包更新
conda update matplotlib
# 刪除包
conda remove matplotlib
```

對於那些用 pip 無法安裝成功的模塊你都可以嘗試用 conda 來安裝，如果用 conda 找不到相應的包，當然你繼續選擇 pip 來安裝包也是沒問題的。

##提高下載速度

Anaconda 的鏡像地址默認在國外，用 conda 安裝包的時候會很慢，目前可用的國內鏡像源地址是清華大學提供的。修改 ~/.condarc (Linux/Mac) 或 C:\Users\當前用戶名.condarc (Windows) 配置


```sh
channels:
 - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
 - defaults
show_channel_urls: true
```

除此之外，你也可以把 pip 的鏡像源地址也換成國內的，豆瓣源速度比較快。修改 ~/.pip/pip.conf (Linux/Mac) 或 C:\Users\當前用戶名\pip\pip.ini (Windows) 配置：

```sh
[global]
trusted-host =  pypi.douban.com
index-url = http://pypi.douban.com/simple
```