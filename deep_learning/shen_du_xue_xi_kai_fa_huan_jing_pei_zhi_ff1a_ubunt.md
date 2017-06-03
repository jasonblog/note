# 深度學習開發環境配置：Ubuntu1 6.04+Nvidia GTX 1080+CUDA 8.0


懶人版方法：


```sh
apt-key adv --fetch-keys http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1604/x86_64/7fa2af80.pub
echo "deb http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1604/x86_64 /" | sudo tee /etc/apt/sources.list.d/cuda.list
sudo apt-get update
sudo apt-get -y install cuda-drivers cuda
```

這個方法會安裝穩定版的驅動和CUDA，可能不那麼新。

然後開始安裝 cuDNN, 先下載 cuDNN 6.0,


```sh
wget http://developer.download.nvidia.com/compute/redist/cudnn/v6.0/cudnn-8.0-linux-x64-v6.0.tgz
```

然後解壓到 /usr/local

```sh
sudo tar -zxf cudnn-8.0-linux-x64-v6.0.tgz -P /usr/local
```

至此，驅動， CUDA 和 cuDNN都安裝完了。

如果你想安裝最新版的驅動和最新版的CUDA，那麼接著讀下去吧。


### 1. 安裝 Nvidia 驅動

```sh
sudo add-apt-repository -qy ppa:graphics-drivers/ppa
sudo apt-get -qy update
sudo apt-get -qy install nvidia-370
sudo apt-get -qy install mesa-common-dev
sudo apt-get -qy install freeglut3-dev
sudo reboot
```
注意，一般比較新的主板，默認是UEFI BIOS，默認啟用了 Secure Boot，否則開機後登陸不進去。老主板沒有這個問題。

### 2. 安裝 CUDA 8.x

去 CUDA 8.x 下載頁面，一定要下載 runfile 安裝方式的安裝包，參考資料裡的好幾篇都是選擇這種方式，貌似 deb包有坑？

```sh
chmod u+x ./cuda_8.0.27_linux.run
sudo ./cuda_8.0.27_linux.run --tmpdir=/tmp
```

執行後會有一系列提示讓你確認，第一個就是問你是否安裝顯卡驅動，由於前一步已經安裝了顯卡驅動，所以這裡就不需要了，況且 runfile 自帶的驅動版本不是最新的。 因此 Install NVIDIA Accelerated Graphics Driver for Linux-x86_64 361.77? 這裡選擇 no。


```sh
Do you accept the previously read EULA?
accept/decline/quit: accept
Install NVIDIA Accelerated Graphics Driver for Linux-x86_64 361.77?
(y)es/(n)o/(q)uit: n
Install the CUDA 8.0 Toolkit?
(y)es/(n)o/(q)uit: y
Enter Toolkit Location
 [ default is /usr/local/cuda-8.0 ]: 
Do you want to install a symbolic link at /usr/local/cuda?
(y)es/(n)o/(q)uit: y
Install the CUDA 8.0 Samples?
(y)es/(n)o/(q)uit: y
Enter CUDA Samples Location
 [ default is /home/programmer ]:
 ```
 
 你以為你會成功安裝嗎？並不是，你一定會碰到一個錯誤，Installation Failed. Using unsupported Compiler. ，這是因為 Ubuntu 16.04 默認的 GCC 5.4 對於 CUDA 8.x來說過於新了，CUDA 安裝腳本還不能識別新版本的 GCC。

看了一下安裝日誌，解決方案也很簡單，加一個 --override 選項，


```sh
sudo ./cuda_8.0.27_linux.run --tmpdir=/tmp --override
```
這次可以成功了。


```sh
===========
= Summary =
===========
Driver:   Not Selected
Toolkit:  Installed in /usr/local/cuda-8.0
Samples:  Installed in /home/programmer, but missing recommended libraries
Please make sure that
 -   PATH includes /usr/local/cuda-8.0/bin
 -   LD_LIBRARY_PATH includes /usr/local/cuda-8.0/lib64, or, add /usr/local/cuda-8.0/lib64 to /etc/ld.so.conf and run ldconfig as root
To uninstall the CUDA Toolkit, run the uninstall script in /usr/local/cuda-8.0/bin
Please see CUDA_Installation_Guide_Linux.pdf in /usr/local/cuda-8.0/doc/pdf for detailed information on setting up CUDA.
***WARNING: Incomplete installation! This installation did not install the CUDA Driver. A driver of version at least 361.00 is required for CUDA 8.0 functionality to work.
To install the driver using this installer, run the following command, replacing <CudaInstaller> with the name of this run file:
    sudo <CudaInstaller>.run -silent -driver
Logfile is /tmp/cuda_install_6794.log
Signal caught, cleaning up
```

把以下兩行加入到 .bashrc

```sh
export PATH=/usr/local/cuda-8.0/bin${PATH:+:${PATH}}
export LD_LIBRARY_PATH=/usr/local/cuda-8.0/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
```

#### 安裝補丁

```sh
chmod u+x ./cuda_8.0.27.1_linux.run
sudo ./cuda_8.0.27.1_linux.run
```

測試是否安裝成功

最後再來測試一下CUDA，運行：


```sh
nvidia-smi
```

### 結果如下所示：


```sh
Sat Jun  3 13:36:13 2017       
+-----------------------------------------------------------------------------+
| NVIDIA-SMI 375.39                 Driver Version: 375.39                    |
|-------------------------------+----------------------+----------------------+
| GPU  Name        Persistence-M| Bus-Id        Disp.A | Volatile Uncorr. ECC |
| Fan  Temp  Perf  Pwr:Usage/Cap|         Memory-Usage | GPU-Util  Compute M. |
|===============================+======================+======================|
|   0  GeForce GTX 660     Off  | 0000:01:00.0     N/A |                  N/A |
| 31%   43C    P8    N/A /  N/A |    646MiB /  1991MiB |     N/A      Default |
+-------------------------------+----------------------+----------------------+
                                                                               
+-----------------------------------------------------------------------------+
| Processes:                                                       GPU Memory |
|  GPU       PID  Type  Process name                               Usage      |
|=============================================================================|
|    0                  Not Supported                                         |
+-----------------------------------------------------------------------------+
```

再來試幾個CUDA例子：

```sh
cd ~/NVIDIA_CUDA-8.0_Samples/1_Utilities/deviceQuery
make
```

執行 ./deviceQuery，得到：


```sh
CUDA Device Query (Runtime API) version (CUDART static linking)
Detected 1 CUDA Capable device(s)
Device 0: "GeForce GTX 1080"
  CUDA Driver Version / Runtime Version          8.0 / 8.0
  CUDA Capability Major/Minor version number:    6.1
  Total amount of global memory:                 8110 MBytes (8504279040 bytes)
  (20) Multiprocessors, (128) CUDA Cores/MP:     2560 CUDA Cores
  GPU Max Clock rate:                            1734 MHz (1.73 GHz)
  Memory Clock rate:                             5005 Mhz
  Memory Bus Width:                              256-bit
  L2 Cache Size:                                 2097152 bytes
  Maximum Texture Dimension Size (x,y,z)         1D=(131072), 2D=(131072, 65536), 3D=(16384, 16384, 16384)
  Maximum Layered 1D Texture Size, (num) layers  1D=(32768), 2048 layers
  Maximum Layered 2D Texture Size, (num) layers  2D=(32768, 32768), 2048 layers
  Total amount of constant memory:               65536 bytes
  Total amount of shared memory per block:       49152 bytes
  Total number of registers available per block: 65536
  Warp size:                                     32
  Maximum number of threads per multiprocessor:  2048
  Maximum number of threads per block:           1024
  Max dimension size of a thread block (x,y,z): (1024, 1024, 64)
  Max dimension size of a grid size    (x,y,z): (2147483647, 65535, 65535)
  Maximum memory pitch:                          2147483647 bytes
  Texture alignment:                             512 bytes
  Concurrent copy and kernel execution:          Yes with 2 copy engine(s)
  Run time limit on kernels:                     Yes
  Integrated GPU sharing Host Memory:            No
  Support host page-locked memory mapping:       Yes
  Alignment requirement for Surfaces:            Yes
  Device has ECC support:                        Disabled
  Device supports Unified Addressing (UVA):      Yes
  Device PCI Domain ID / Bus ID / location ID:   0 / 5 / 0
  Compute Mode:
     < Default (multiple host threads can use ::cudaSetDevice() with device simultaneously) >
deviceQuery, CUDA Driver = CUDART, CUDA Driver Version = 8.0, CUDA Runtime Version = 8.0, NumDevs = 1, Device0 = GeForce GTX 1080
Result = PASS
````

再測試試一下nobody：


```sh
cd ~/NVIDIA_CUDA-8.0_Samples/5_Simulations/nbody/
make
```


執行：

```sh
./nbody -benchmark -numbodies=256000 -device=0
```

得到：


```sh
> Windowed mode
> Simulation data stored in video memory
> Single precision floating point simulation
> 1 Devices used for simulation
gpuDeviceInit() CUDA Device [0]: "GeForce GTX 1080
> Compute 6.1 CUDA device: [GeForce GTX 1080]
number of bodies = 256000
256000 bodies, total time for 10 iterations: 2364.286 ms
= 277.192 billion interactions per second
= 5543.830 single-precision GFLOP/s at 20 flops per interaction
```

至此，說明 CUDA 8.x 安裝成功了。

## 參考資料

- 深度學習主機環境配置: Ubuntu16.04+Nvidia GTX 1080+CUDA8.0
- Nvidia GTX 1080 on Ubuntu 16.04 for Deep Learning - Changjiang
- Build Personal Deep Learning Rig: GTX 1080 + Ubuntu 16.04 + CUDA 8.0RC + CuDnn 7 + Tensorflow/Mxnet/Caffe/Darknet - by Guanghan Ning
- GeForce GTX 1080, CUDA 8.0, Ubuntu 16.04, Caffe
 







 