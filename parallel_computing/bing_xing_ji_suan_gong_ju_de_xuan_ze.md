# 并行计算工具的选择


并行计算是未来的发展趋势，如何选择并行的架构是个重要的问题。

首先要发挥集群计算的能力，用MPI是个不二的选择。

其次为了充分利用cpu和gpu的多核计算能力，有一系列的库可以选择，openMP openACC CUDA OpenCL ，openMP openACC是同一层次的接口，是`share memory并行`，openACC比openMP有所扩展，主要是在GPU的方面，这一层的实现比较简单，如果这一层已经足够了，不要刻意把问题复杂化使用更复杂的调用接口，CUDA OpenCL差不多是一层，openCL的优势在于扩展行强，他只是一个通用的标准，和MPI一样有很多实现，所以对于不同的多核硬件都可以很好的支持，但是Cuda是Nvidia的特别实现，效率上会比openCL略胜一筹，然后还有一个参考是基于前面架构有没有一些现成的库可以使用，比如fft blas等等。

<br><ul><li><b>CUDA</b>: 貌似上面的前辈们都已经总结了优缺点了，再叙述一遍吧。N家的多线程计算平台和API，支持GPU。</li><ul><li>关键词： 既有平台又是API</li><li>语言支持： C/C++, Fortran</li><li>优点：版本更行快，社区活跃, 支持GPU</li><li>缺点：只支持N卡</li></ul><li><b>OpenCL</b>: 不是很了解，搬运下wiki的内容吧。看这wiki的内容，感觉不明觉厉啊。异构平台，支持CPU，GPU， DSPs， FPGAs 等。在多进程方面使用 task-based 和 data-based parallelism。</li><ul><li>关键词： 平台 + API</li><li>语言支持： C/C++</li><li>优点： 跨好多平台啊</li><li>缺点：版本更新慢</li></ul><li><b>OpenMP</b>: 这是directive-based parallelism。在写完一般的代码后，转成多线程需要改动的地方不是很大，但只能支持shared memory（共享存储？），是一个管理线程级别的库。</li><ul><li>关键词： API</li><li>语言支持： C/C++, Fortran</li><li>优点：移植改动少，支持普遍（普通的gcc都支持）</li><li>缺点：只支持shared memory</li></ul><li><b>OpenACC</b>:  支持CPU/GPU工作，从名字Accelerator就可以推断出，主要用于CPU代码向GPU的移植（？有待考证，我在课程中主要是这么用的。。。），更新方面其实也不快，需要注意编程技巧，否则代码会变慢。</li><ul><li>关键词： API</li><li>语言支持： C/C++, Fortran</li><li>优点： 移植简单</li><li>缺点： 编译器啊，支持PGI，看计划有支持GCC，但还不知道什么时候才能发布</li></ul><li><b>OpenMPI</b>:  消息传递接口库。支持分布式存储，适合大规模集群服务器。另外，需要手动管理数据的分布和消息的传递，需要重新修改代码。</li><ul><li>关键词： API</li><li>语言支持： C/C++</li><li>优点： 支持共享存储和分布式存储</li><li>缺点： 模型复杂，需要重构代码。</li>