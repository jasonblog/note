# 在Linux Ubuntu 安裝 Tensorflow 與Keras


###5.1 安裝Anaconda

Step2. 下載Anaconda3-4.2.0-Linux-x86_64.sh 
```sh
wget https://repo.continuum.io/archive/Anaconda3-4.2.0-Linux-x86_64.sh
```

Step3. 安裝Anaconda 

```sh
bash Anaconda3-4.2.0-Linux-x86_64.sh -b
```

Step4. 編輯~/.bashrc 加入模組路徑 

```sh
sudo gedit ~/.bashrc
```

輸入下列內容: 

```sh
export PATH="/home/user/anaconda3/bin:PATH"
```

Step4. 編輯~/.bashrc 加入模組路徑 

```sh
sudo gedit ~/.bashrc
```

## 4.4 在Anaconda虛擬環境安裝TensorFlow與Keras

Step5. 使~/.bashrc 修改生效 
```sh
source ~/.bashrc
```

Step6. 查看python 版本 

```sh
python --version
```

## 5.2 安裝TensorFlow與Keras
Step1. 安裝TensorFlow 

```sh
pip install tensorflow
```

Step2. 安裝Keras 

```sh
pip install keras
```

## 5.3 啟動Jupyter Notebook
啟動Jupyter Notebook 

```sh
mkdir -p ~/pywork
cd ~/pywork
jupyter notebook
```
