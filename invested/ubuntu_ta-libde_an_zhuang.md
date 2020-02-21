# ubuntu TA-lib的安裝


安裝TA-lib用下面這句指令就會自動解決環境衝突問題並安裝好。

conda install TA-lib

除了這個方法之外，還有第二種方法安裝了。就是下面這種方法：

```sh
sudo apt-get update
sudo apt-get install python3.6-dev
# 裝talib前要先裝numpy
python3.6 -m pip install numpy -i https://pypi.doubanio.com/simple
wget http://prdownloads.sourceforge.net/ta-lib/ta-lib-0.4.0-src.tar.gz
tar -xzvf ta-lib-0.4.0-src.tar.gz
cd ta-lib
./configure --prefix=/usr
make
sudo make install
cd ..
pip install TA-Lib
```

