# CUDA ---- Hello World From GPU

- hello.cu


限定符__global__告訴編譯器這個function將由CPU調用在GPU上執行，其調用形式為：

helloFromGPU<<<1,10>>>();

一個kernel是由一組線程執行，所有線程執行相同的代碼。上面一行三對尖括號中的1和10 表明了該function將有10個線程，具體含義之後博文中會詳述。下面是完整代碼：



```c
#include <stdio.h>

__global__ void helloFromGPU(void)
{
    printf("Hello World from GPU!\n");
}

int main(void)
{
    // hello from cpu
    printf("Hello World from CPU!\n");

    helloFromGPU <<< 1, 10>>>();

    cudaDeviceReset();

    return 0;
}
```

```sh
nvcc hello.cu -o hello
```

```sh
./hello
```

這裡順便提及下，我們將CPU端稱為host，GPU端稱為device。

cudaDeviceReset()用來顯式的摧毀清理CUDA程序佔用的資源。現在用下面的命令編譯：

```sh
$nvcc –arch sm_20 hello.cu –o hello
```

-arch sm_20是用來指定編譯器使用Fermi架構產生device代碼。編譯成功後執行$ ./hello:

```sh
Hello World from CPU!

Hello World from GPU!

Hello World from GPU!

Hello World from GPU!

Hello World from GPU!

Hello World from GPU!

Hello World from GPU!

Hello World from GPU!

Hello World from GPU!

Hello World from GPU!

Hello World from GPU!
```
 

一個典型的CUDA程序結構包含五個主要步驟：

- 分配GPU空間。
- 將數據從CPU端複製到GPU端。
- 調用CUDA kernel來執行計算。
- 計算完成後將數據從GPU拷貝回CPU。
- 清理GPU內存空間。