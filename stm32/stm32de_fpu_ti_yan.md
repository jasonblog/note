# STM32的FPU體驗


STM32-F4屬於Cortex-M4構架，與M0、M3的最大不同就是有硬件浮點運算FPU，數學計算速度相比普通cpu運算快上幾十倍。想要使用FPU首先包含#include 「arm_math.h」，還有在keil的target選項中勾選use single precision。
 
##1.1 簡單的FPU運算性能測試
    測試條件是開啟一個100ms定時器，定時串口打印計算次數，優化級別是0，main函數中運行的代碼如下：
float a=1.1,b=1.2,c=1.3,d=1.4; 

1、FPU運算474566次，CPU運算64688次，除法速度快了7.3倍多。
c = b / d;
 
2、FPU運算722169次，CPU運算244271次，乘法運算快了3倍。FPU的乘法運算比除法運算快1.5倍。
c = b * d;
 
3、FPU運算19398次，CPU運算19628次，FPU的雙精度除法運算沒有優勢，比單精度運算慢了24.5倍
c = b / 1.4;
 
4、FPU運算503321次，CPU運算65450次，單精度常數和變量的運算差不多，單精度常數的除法快6%左右，這根編譯器的關係比較大。
c = b / 1.4f;
 
5、FPU運算519073次，跟下面比較說明整形常數和單精度常數的除法運算速度幾乎一樣。
c = b / 3;
 
6、FPU運算519057次
c = b / 3.0f;
 
7、FPU運算263643次
c = arm_cos_f32(1.3f);
 
8、FPU運算3949次，說明IT給的DSP庫運算速度還是很給力的，速度快了67倍
c = cos(1.3f);



##1.2 代碼設置

舊版本的keil設置如下，但是發現我使用的keil5包含的新固件庫已經不需要這一步了。
如果沒有啟動FPU而使用數學函數運算時，CPU執行時認為遇到非法指令而跳轉到HardFault_Handler()中斷函數中死循環。因此，需要在系統初始化時開啟FPU。在system_stm32f4xx.c中的SystemInit()函數中添加如下代碼：

```c
 /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif
```

當__FPU_PRESENT=1且__FPU_USED=1時，編譯時就加入了啟動FPU的代碼，CPU也就能正確高效的使用FPU進行簡單的加減乘除。需要使用固件庫自帶的arm_math.h而非編譯器自帶的math.h，這個文件根據編譯控制項（__FPU_USED ==1）來決定是使用哪一種函數方法：如果沒有使用FPU，那就調用keil的標準math.h頭文件中定義的函數；如果使用了FPU，那就是用固件庫自帶的優化函數來解決問題。


##1.3編譯控制
在arm_math.h開頭部分有一些編譯控制信息：


```c
#ifndef _ARM_MATH_H
#define _ARM_MATH_H

#define __CMSIS_GENERIC              /* disable NVIC and Systick functions */

#if defined (ARM_MATH_CM4)
#include "core_cm4.h"
#elif defined (ARM_MATH_CM3)
#include "core_cm3.h"
#elif defined (ARM_MATH_CM0)
#include "core_cm0.h"
#else
 #include "ARMCM4.h"
#warning "Define either ARM_MATH_CM4 OR ARM_MATH_CM3...By Default building on ARM_MATH_CM4....."
#endif

#undef  __CMSIS_GENERIC              /* enable NVIC and Systick functions */

#include "string.h"
#include "math.h"
```

從中可以看出，為了使用STM32F4的arm_math.h，我們需要定義ARM_MATH_CM4；否則如果不使用CMSIS的庫，就會調用Keil自帶的math.h。另外，定義控制項__CC_ARM在某些數學函數中會使用VSQRT指令（浮點運算指令），運算速度比Q指令要快很多。
總結一下，需要添加宏定義ARM_MATH_CM4, __CC_ARM。


##1.4添加庫
根據使用的器件和運算模式，添加arm_cortexMxx_math.lib到工程文件中，位於\Libraries\CMSIS\Lib\ARM中。
   * The library installer contains prebuilt versions of the libraries in the <code>Lib</code> folder.
   * - arm_cortexM4lf_math.lib (Little endian and Floating Point Unit on Cortex-M4)
   * - arm_cortexM4bf_math.lib (Big endian and Floating Point Unit on Cortex-M4)
   * - arm_cortexM4l_math.lib (Little endian on Cortex-M4)
   * - arm_cortexM4b_math.lib (Big endian on Cortex-M4)
   * - arm_cortexM3l_math.lib (Little endian on Cortex-M3)
   * - arm_cortexM3b_math.lib (Big endian on Cortex-M3)
   * - arm_cortexM0l_math.lib (Little endian on Cortex-M0)
   * - arm_cortexM0b_math.lib (Big endian on Cortex-M3)
註：如果存儲空間不允許，也可以不添加庫，只添加\Libraries\CMSIS\DSP_Lib\Source中需要的源文件和arm_math.h。
其他DSP使用示例見\Libraries\CMSIS\DSP_Lib\Examples。


##1.5 DSP_Lib的文件結構
BasicMathFunctions：提供浮點數的各種基本運算函數，如加減乘除等運算。對於M0/M3只能用Q運算，即文件夾下以_q7、_q15和_q31結尾的文件；而M4F能直接硬件浮點計算，屬於文件夾下以_f32結尾的文件。

- CommonTables：arm_common_tables.c文件提供位翻轉或相關參數表。
- ComplexMathFunctions：複述數學功能，如向量處理，求模運算的。
- ControllerFunctions：控制功能，主要為PID控制函數。arm_sin_cos_f32/-q31.c函數提供360點正餘弦函數表和任意角度的正餘弦函數值計算功能。
- FastMathFunctions：快速數學功能函數，提供256點正餘弦函數表和任意任意角度的正餘弦函數值計算功能，和Q值開平方運算：
- FilteringFunctions：濾波函數功能，主要為FIR和LMS（最小均方根）濾波函數。
- MatrixFunctions：矩陣處理函數。
- StatisticsFunctions：統計功能函數，如求平均值、計算RMS、計算方差/標準差等。
- SupportFunctions：支持功能函數，如數據拷貝，Q格式和浮點格式相互轉換，Q任意格式相互轉換。
- TransformFunctions：變換功能。包括複數FFT（CFFT）/複數FFT逆運算（CIFFT）、實數FFT（RFFT）/實數FFT逆運算（RIFFT）、和DCT（離散餘弦變換）和配套的初始化函數。
 
##1.6常用庫函數
需要包含頭文件：`#include <arm_math.h>`
```c
float32_t arm_sin_f32(float32_t x);
float32_t arm_cos_f32(float32_t x);
static __INLINE arm_status  arm_sqrt_f32(float32_t in, float32_t *pOut)
```