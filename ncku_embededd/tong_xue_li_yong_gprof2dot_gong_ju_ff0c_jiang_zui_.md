# 利用 gprof2dot 工具，將最耗時的函式與呼叫的路徑視覺化

﻿# 2016q3 Homework01 (raytracing)
contributed by <`hugikun999`>

## Gprof
在make時記得要加上 `CFLAGS=-pg` 和 `LDFLAGS=-pg` 兩個選項，第一個是設定編譯選項，第二個是鏈接選項。

### commond
```shell
$ gprof -p ./raytracing
```
只輸出function的時間開銷表。

```shell
$ gprof -q ./raytracing
```
只輸出call graph

## [graphviz](http://www.openfoundry.org/tw/foss-programs/8820-graphviz-)
將gprof產生的結果數據透過graphviz產生出容易觀看的圖表。graphviz的檔案需要特定格式(ex:dot、neato、fdp)，會需要用到[gprof2dot](https://github.com/jrfonseca/gprof2dot)將gprof產生的檔案轉成dot的形式。
![](https://i.imgur.com/skxYvPO.png)
### install gprof2dot
安裝的時後會用到[pip](https://pip.pypa.io/en/stable/installing/),所以要先安裝python。
要注意這裡不可以下```$ sudo apt install pip```，會找不到東西安裝。
```
$sudo apt install python python-pip
$sudo pip install --upgrade pip
$sudo pip install gprof2dot
```
### command
在[這個網站](https://github.com/jrfonseca/gprof2dot)中有提供不同情況下使用的command，但是其中gprof給的command執行後會出現command not found，因此我改用下面的command。
```
$ gprof ./raytracing | gprof2dot | dot -T png -o output.png
```

## Raytracing
### file detail
```C
typedef double point3[3];
```
用```point3```來宣告的東西是一個具有三個element的指標。
```C
struct object_fill{}
```
紀錄RGB、反光率之類的object。
```
models.inc
```
這個檔案是一些物體和光的資料。


### 可能的優化方向
* Loop unrolling
		藉由展開function而不使用呼叫的方式，可以減少花費在呼叫的時間，但是會造成程式本身的膨脹，有點類似空間換取時間的做法。
* Force inline
		如果定義時加上"inline"通常compiler就會自動inline，但在這次的MAKEFILE中特別加了關閉編譯器最佳化選項 -O0，因此會須要force inline，就算關閉最佳化依然會inline。
* Pthread
		
* SIMD

* AVX

* openMP
### function
* inline function
	效果：在定義時將前面加上inline，藉由inline可以將函式直接在main中展開，免除函式呼叫所造成的成本，從而提升效能。

	注意：須要在定意時加，若在宣告時加沒有作用，且可能會造成在complier時main過度龐大。另外如果函式內含有其它複雜度高的函式，則效益不大。
```C
sqrt(double a);
```
說明：回傳a的[方均根](https://zh.wikipedia.org/wiki/%E5%B9%B3%E6%96%B9%E5%B9%B3%E5%9D%87%E6%95%B0)。

回傳值：a的方均根。



### 未優化	
將原本下載的原始碼做編譯而得到的結果，可以看到這個program花最多的時間是dot_product()，藉由這張數據圖可以看到花費時間比較多的程式，可以從這些function去尋找優化方法。另外可以看到由call graph所繪出的圖，思考如何從中減少時間的方向。
```
Execution time od raytracing() : 5.309807 sec
```

![](https://i.imgur.com/qCabbdv.png)

![](https://i.imgur.com/ztFT2no.png)

![](https://i.imgur.com/a9O7pJ7.png)

### 優化
*   establish record（失敗）
	我發現normalize()和length()兩個函式其實很相近，在normalize()中已經算出length()要求的東西，所以我就把它的位置記錄起來，這樣只要比對位置一樣就不必重算。


![](https://i.imgur.com/TWWCOMn.png)

![](https://i.imgur.com/ZuwFNOH.png)

![](https://i.imgur.com/NKhtFtb.png)

從數據看來normalize()的％數有上升，但是花費的時間反而更多。

*	Loop unrolling
(一)
我更改了add_vector()、subtract_vector()、multiply_vectors()、multiply_vector()、dot_product()這些fuction，把能不用for迴圈做的事給拆開來寫。從perf stat可以看到instructions有減少但是cache-misses卻有所上升。

![](https://i.imgur.com/DZ3enp2.png)

![](https://i.imgur.com/Fm9gdvW.png)

![](https://i.imgur.com/K5HnA20.png)

![](https://i.imgur.com/64bLwBS.png)

（二）
在```raytracing.c```中的raytracing()裡面有三個for迴圈，第三個for是以SAMPLE這個變數下去跑，其實它的真實值是4，所以這裡也可以拆開來跑。但是由於這次for迴圈內的行數較多拆開來會有點亂。
> 參考自heathcliffYang

force inline+loop unrolling
```
Execution time of raytracing() : 2.184140 sec
```

*	Force inline
(一)	
將```math-toolkih.h```中所有有用到```inline```的地方全部替換成``` __attribute__((always_inline))```，特別住意前面是兩個```_```而不是一個。可以看到branch-misses下降很多，cache-misses卻上升了，總體時間下降了很多。另外在```make PROFILE=1```時會出現```always_inline function might not be inlinable```是指compiler不一定會做inline這個指令

![](https://i.imgur.com/e4NTELQ.png)


![](https://i.imgur.com/xhPyP87.png)

![](https://i.imgur.com/87btcpS.png)

![](https://i.imgur.com/DZlr9q6.png)

（二）
在```idx_stack.h```中也有inline的部份，依照之前的改法改成```__attribute__((always_inline))```去強制inline。時間大概可以再減少0.02秒。


*  OpenMP
（一）
在```raytracing.c```中加上```#inclde <omp.h>```並在更改MAKEFILE中的參數。目前結果是變更慢且做出來的圖是錯的，應該是parallel的地方錯了。
(1)MAKEFILE
```C
CC ?= gcc
CFLAGS = \
	-std=gnu99 -Wall -O0 -g -fopenmp
LDFLAGS = \
	-lm -fopenmp
```
(2)raytracing.c
```C
void raytracing(uint8_t *pixels, color background_color,
                rectangular_node rectangulars, sphere_node spheres,
                light_node lights, const viewpoint *view,
                int width, int height)
{
    point3 u, v, w, d;
    color object_color = { 0.0, 0.0, 0.0 };

    /* calculate u, v, w */
    calculateBasisVectors(u, v, w, view);

    idx_stack stk;

    int factor = sqrt(SAMPLES);
    #pragma omp parallel for
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            double r = 0, g = 0, b = 0;
            /* MSAA */
＃pragma omp parallel for
            for (int s = 0; s < SAMPLES; s++) {
                idx_stack_init(&stk);
                rayConstruction(d, u, v, w,
                                i * factor + s / factor,
                                j * factor + s % factor,
                                view,
                                width * factor, height * factor);
                if (ray_color(view->vrp, 0.0, d, &stk, rectangulars, spheres,
                              lights, object_color,
                              MAX_REFLECTION_BOUNCES)) {
                    r += object_color[0];
                    g += object_color[1];
                    b += object_color[2];
                } else {
                    r += background_color[0];
                    g += background_color[1];
                    b += background_color[2];
                }
                pixels[((i + (j * width)) * 3) + 0] = r * 255 / SAMPLES;
                pixels[((i + (j * width)) * 3) + 1] = g * 255 / SAMPLES;
                pixels[((i + (j * width)) * 3) + 2] = b * 255 / SAMPLES;
            }
        }
    }
}
```
![](https://i.imgur.com/sTCcVp5.png)

發現在上面的code中有r、g、b，在其中會分別歸0，而若分下去做可能會造成存取上的問題。

（二）
嘗試將最下方的pixels分開做改成如下。發現圖是正確的，執行時間卻變超久，應該是每個thread做的事情太少反而浪費分出去的時間。應該以更大的部份下去分。
```
# Rendering scene
Done!
Execution time of raytracing() : 13.653941 sec
```
(1)raytarcing.c
```C
#pragma omp section
{
pixels[((i + (j * width)) * 3) + 0] = r * 255 / SAMPLES;
}
#pragma omp section
{                
pixels[((i + (j * width)) * 3) + 1] = g * 255 / SAMPLES;
}
#pragma omp section
{
pixels[((i + (j * width)) * 3) + 2] = b * 255 / SAMPLES;
}
```

(三)（失敗）
嘗試對```raytracing.c```的rayConstruction()作平行化，原本我是想連add_vector()的地方一起放入section，但是我發現這邊的加法有相依性，我把順序改動之後產生的圖就是錯的，跟一般的加法不太一樣。總結就是會花更多的時間。
（1）raytracing.c
```C
static void rayConstruction(point3 d, const point3 u, const point3 v,
                            const point3 w, unsigned int i, unsigned int j,
                            const viewpoint *view, unsigned int width,
                            unsigned int height)
{
    double xmin = -0.0175;
    double ymin = -0.0175;
    double xmax =  0.0175;
    double ymax =  0.0175;
    double focal = 0.05;

    point3 u_tmp, v_tmp, w_tmp, s;
#pragma omp parallel sections
{
   #pragma omp section
   {
    double w_s = focal;
    multiply_vector(w, w_s, w_tmp);
   }
   #pragma omp section
   {
    double u_s = xmin + ((xmax - xmin) * (float) i / (width - 1));
    multiply_vector(u, u_s, u_tmp);
   }
   #pragma omp section
   {
    double v_s = ymax + ((ymin - ymax) * (float) j / (height - 1));
    multiply_vector(v, v_s, v_tmp);
    /* s = e + u_s * u + v_s * v + w_s * w  */
   }
}
```

(四)
透過分析raytracing()這個函式，可以發現到d、stk、object_color這三個東西在三個for迴圈內是會被更改到的（大家共用），所以在平行化的時候會有正確性的問題，利用```privete```這個標簽可以將變數複製一份，存取的同時就不會更動到其它thread的數據。結果大幅降低所花費的時間。
（1）Loop unrolling + force inline +openMP
```
Execution time of raytracing() : 1.059777 sec
```
(2)raytracing.c
```C
void raytracing(uint8_t *pixels, color background_color,
                rectangular_node rectangulars, sphere_node spheres,
                light_node lights, const viewpoint *view,
                int width, int height)
{
    point3 u, v, w, d;
    color object_color = { 0.0, 0.0, 0.0 };

    /* calculate u, v, w */
    calculateBasisVectors(u, v, w, view);

    idx_stack stk;

    int factor = sqrt(SAMPLES);
#pragma omp parallel for private( d, stk, object_color) num_threads()
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            double r = 0, g = 0, b = 0;
            /* MSAA */
                idx_stack_init(&stk);
                rayConstruction(d, u, v, w,
                                i * factor + 0 / factor,
                                j * factor + 0 % factor,
                                view,
                                width * factor, height * factor);
                if (ray_color(view->vrp, 0.0, d, &stk, rectangulars, spheres,
                              lights, object_color,
                              MAX_REFLECTION_BOUNCES)) {
                    r += object_color[0];
                    g += object_color[1];
                    b += object_color[2];
                } else {
                    r += background_color[0];
                    g += background_color[1];
                    b += background_color[2];
                }
                pixels[((i + (j * width)) * 3) + 0] = r * 255 / SAMPLES;
                pixels[((i + (j * width)) * 3) + 1] = g * 255 / SAMPLES;
                pixels[((i + (j * width)) * 3) + 2] = b * 255 / SAMPLES;



                idx_stack_init(&stk);
                rayConstruction(d, u, v, w,
                                i * factor + 1 / factor,
                                j * factor + 1 % factor,
                                view,
                                width * factor, height * factor);
                if (ray_color(view->vrp, 0.0, d, &stk, rectangulars, spheres,
                              lights, object_color,
                              MAX_REFLECTION_BOUNCES)) {
                    r += object_color[0];
                    g += object_color[1];
                    b += object_color[2];
                } else {
                    r += background_color[0];
                    g += background_color[1];
                    b += background_color[2];
                }
                pixels[((i + (j * width)) * 3) + 0] = r * 255 / SAMPLES;
                pixels[((i + (j * width)) * 3) + 1] = g * 255 / SAMPLES;
                pixels[((i + (j * width)) * 3) + 2] = b * 255 / SAMPLES;



                idx_stack_init(&stk);
                rayConstruction(d, u, v, w,
                                i * factor + 2 / factor,
                                j * factor + 2 % factor,
                                view,
                                width * factor, height * factor);
                if (ray_color(view->vrp, 0.0, d, &stk, rectangulars, spheres,
                              lights, object_color,
                              MAX_REFLECTION_BOUNCES)) {
                    r += object_color[0];
                    g += object_color[1];
                    b += object_color[2];
                } else {
                    r += background_color[0];
                    g += background_color[1];
                    b += background_color[2];
                }
                pixels[((i + (j * width)) * 3) + 0] = r * 255 / SAMPLES;
                pixels[((i + (j * width)) * 3) + 1] = g * 255 / SAMPLES;
                pixels[((i + (j * width)) * 3) + 2] = b * 255 / SAMPLES;



                idx_stack_init(&stk);
                rayConstruction(d, u, v, w,
                                i * factor + 3 / factor,
                                j * factor + 3 % factor,
                                view,
                                width * factor, height * factor);
                if (ray_color(view->vrp, 0.0, d, &stk, rectangulars, spheres,
                              lights, object_color,
                              MAX_REFLECTION_BOUNCES)) {
                    r += object_color[0];
                    g += object_color[1];
                    b += object_color[2];
                } else {
                    r += background_color[0];
                    g += background_color[1];
                    b += background_color[2];
                }
                pixels[((i + (j * width)) * 3) + 0] = r * 255 / SAMPLES;
                pixels[((i + (j * width)) * 3) + 1] = g * 255 / SAMPLES;
                pixels[((i + (j * width)) * 3) + 2] = b * 255 / SAMPLES;
        }
    }
}
```