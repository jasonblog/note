# ijmtdp
圖解Java多線程設計模式代碼Maven版

《圖解Java多線程設計模式》結城浩著
北京圖靈文化發展有限公司
http://www.hyuki.com/dp/dp2.html
hyuki@hyuki.com

■前言

本壓縮文件中保存的是《圖解Java多線程設計模式》一書的示例程序。

■示例程序的目錄結構

示例程序的目錄結構如下所示：

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

各個目錄的意義如下。

    src/模式名/Sample      示例程序
    src/模式名/jucSample   示例程序(主要是使用了java.util.concurrent的示例程序)
    src/模式名/Q☆...      習題中的代碼清單（☆表示習題編號）
    src/模式名/A☆...      習題解答中的代碼清單（☆表示習題編號）


■著作權和許可協議
使用本壓縮文件中的文件時
請遵守以下的The zlib/libpng License許可協議


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

（以下是中文參考譯文）
本軟件以“即-是”方式提供，不作任何明確或隱含的保證。
軟件作者不會對軟件使用所引起的損害承擔任何責任。

在遵循以下限制條件的情況下，您可以基於任何目的【包括商用、修改及重分發】使用本軟件。

1. 您不可以篡改軟件的原始信息，不可以聲明您編寫最初的軟件。
   如果您在產品中使用本軟件，可以在產品文檔裡鳴謝，但是不是必需的。
   
2. 修改源代碼版本，必需明確的標識,並且不要作為自己的原創軟件。

3. 本聲明在任何源碼的發佈中，均不可以移除或修改。

