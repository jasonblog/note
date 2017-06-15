# CUDA ---- Hello World From GPU

- hello.cu


限定符__global__告诉编译器这个function将由CPU调用在GPU上执行，其调用形式为：

helloFromGPU<<<1,10>>>();

一个kernel是由一组线程执行，所有线程执行相同的代码。上面一行三对尖括号中的1和10 表明了该function将有10个线程，具体含义之后博文中会详述。下面是完整代码：



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

这里顺便提及下，我们将CPU端称为host，GPU端称为device。

cudaDeviceReset()用来显式的摧毁清理CUDA程序占用的资源。现在用下面的命令编译：

```sh
$nvcc –arch sm_20 hello.cu –o hello
```

-arch sm_20是用来指定编译器使用Fermi架构产生device代码。编译成功后执行$ ./hello:

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
 

一个典型的CUDA程序结构包含五个主要步骤：

- 分配GPU空间。
- 将数据从CPU端复制到GPU端。
- 调用CUDA kernel来执行计算。
- 计算完成后将数据从GPU拷贝回CPU。
- 清理GPU内存空间。