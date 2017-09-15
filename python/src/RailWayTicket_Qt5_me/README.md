台鐵訂票規則
(一)針對每分鐘訂票超過 60 次身分證字號進行阻擋。
(二)異常訂票 IP 封鎖機制。
(三)驗證碼改為浮動碼（4~6 碼）。

要爬台鐵網站進行訂票一定要取得 session cookie，
直接爬訂票頁面會發生錯誤
爬時刻表網站需要 zLib 解壓縮

目錄架構
-----------------------------
BatchRenameCaptcha  批次命名驗證碼 用來產生模型訓練資料集
BuyTicket.py        買票爬蟲
Image.py            處理驗證碼
kerasInitModel      初始化keras model
MainWindow.py       顯示GUI視窗
station.json        存放台鐵車站代碼
TrainningCNN.py     訓練CNN模型
VPN.py              執行VPN



如何安裝
-----------------------------
step 1.
````
    下載安裝OpenVPN 選擇Installer, Windows Vista and later
    https://openvpn.net/index.php/open-source/downloads.html

    並且到OpenVPN的安裝目錄   舉例 E:\OpenVPN\bin
    右鍵點擊openvpn.exe ->內容 ->相容性頁籤 ->將"以系統管理員的身份執行此程式"勾選

Step2.

    安裝Anaconda2  python2.7版本
    安裝Keras 1.22版本

Step3.

    安裝以下package
    conda install -c https://conda.binstar.org/menpo opencv
    PyQt4
    下載phantomjs.exe 並將目錄設定到環境變數
    pip install retrying

Step4.

    設置keras運行環境
    Keras的相關設定請參考這篇
    https://keras-cn.readthedocs.io/en/latest/getting_started/keras_windows/

    記得在環境變數加入這個(GPU加速)
    C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin

    .theanorc.txt設定檔 最後兩行的註解拿掉
    device=gpu  <-設為這個才會使用GPU

    P.s. 這份中文教學中的範例代碼已經無法使用 請至下面網址觀看最新範例
    https://github.com/fchollet/keras/tree/master/examples


-------------------------------------------------------
