# ijmtdp
图解Java多线程设计模式代码Maven版

《图解Java多线程设计模式》结城浩著
北京图灵文化发展有限公司
http://www.hyuki.com/dp/dp2.html
hyuki@hyuki.com

■前言

本压缩文件中保存的是《图解Java多线程设计模式》一书的示例程序。

■示例程序的目录结构

示例程序的目录结构如下所示：

    src
    +------ Future
    |       +------ Sample
    |       |       +       .
    |       |       |       .
    |       |       +------ Main.java
    |       |
    |       +------ jucSample
    |       |       +       .
    |       |       |       .
    |       |       +------ Main.java
    |       |
    |       +------ Q9-3
    |       |       +------ Main.java
    |       |
    |       +------ A9-3a
    |       |       +       .
    |       |       |       .
    |       |       +------ Main.java
    |       |
    |       +------ A9-3b
    |       |       +       .
    |       |       |       .
    |       |       +------ Main.java
    |
    +------ Balking
    |       +------ Sample
    |       |       .
    |       .       .
    .       .
    .

各个目录的意义如下。

    src/模式名/Sample      示例程序
    src/模式名/jucSample   示例程序(主要是使用了java.util.concurrent的示例程序)
    src/模式名/Q☆...      习题中的代码清单（☆表示习题编号）
    src/模式名/A☆...      习题解答中的代码清单（☆表示习题编号）


■著作权和许可协议
使用本压缩文件中的文件时
请遵守以下的The zlib/libpng License许可协议


Copyright (C) 2002,2006 Hiroshi Yuki.
http://www.hyuki.com/dp/dp2.html
hyuki@hyuki.com

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim
that you wrote the original software. If you use this software in a product,
an acknowledgment in the product documentation would be appreciated but is not
required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

（以下是中文参考译文）
本软件以“即-是”方式提供，不作任何明确或隐含的保证。
软件作者不会对软件使用所引起的损害承担任何责任。

在遵循以下限制条件的情况下，您可以基于任何目的【包括商用、修改及重分发】使用本软件。

1. 您不可以篡改软件的原始信息，不可以声明您编写最初的软件。
   如果您在产品中使用本软件，可以在产品文档里鸣谢，但是不是必需的。
   
2. 修改源代码版本，必需明确的标识,并且不要作为自己的原创软件。

3. 本声明在任何源码的发布中，均不可以移除或修改。

