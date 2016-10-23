# Matrix Multiplication using SIMD

contributed by <`0140454`>, <`linachiu`>
GitHub: [matrix-multiplication](https://github.com/0140454/matrix-multiplication)

## 組員
- [ ] 吳勃興
- [ ] 邱婉菱


## 作業要求

* 參照 [software-pipeline](https://hackmd.io/s/ry7eqDEC)，以 SSE/AVX 實作矩陣乘法。

## 實作

### Naive version

**程式碼**

```clike=
void naive_multiply(int *src1, int *src2, int *dst, int src1_w, int src1_h,
                    int src2_w, int src2_h)
{
    for (int i = 0; i < src1_h; ++i) {
        for (int j = 0; j < src2_w; ++j) {
            dst[i * src2_w + j] = 0;
            for (int k = 0; k < src2_h; ++k) {
                dst[i * src2_w + j] += src1[i * src1_w + k] * src2[k * src2_w + j];
            }
        }
    }
}
```

從程式碼中可以看出，下面兩張圖分別為 `src1` 與 `src2` 的 access pattern。

![](https://i.imgur.com/JChvCUm.png)

我們都知道 C 是 row-major 的語言，所以 `src1` 在 cache 的部份是有發揮效益的。

但在 `src2` 的部份，則會因為一直以 column order 的方式去讀取，導致 cache 沒辦法有效地被利用。

因此，接下來的工作除了要使用 SSE 和 AVX 來實作外，也要注意 cache 相關的問題。

**結果**

執行兩個 1024x1024 矩陣相乘的時間。

```shell=
$ ./naive
naive: 		 17820600 us
```

### Sub-matrix version

在實作 SIMD 前，先實作另外一個版本。

相較於 naive 而言，這個版本會將整個矩陣分為 4x4 的小矩陣進行運算。

因為拆成小矩陣運算的關係，`src2` 的 access pattern 變化如下，這樣會有比較大的機會讓 cache 發揮效用。

![](https://i.imgur.com/KD8uENc.png)

實際執行結果如下，$Speedup = \frac{17820600}{8369778} = 2.13$

```shell=
$ ./submatrix
submatrix:   8369778 us
```

### SSE version
**程式碼**
 因為太長了 可以去github看
 這裡講想法跟解釋一些用到的function
 
 想法是先把兩個矩陣切成很多4x4的小矩陣，把對應的位置先做好矩陣相乘後再相加
 
 ![](https://i.imgur.com/0XJ6usb.gif)

 
 * matrix1 一次橫向讀入128bit = 四個int 存進I0 ，只要給要讀入陣列的起始位置，他會往後抓127bit, I1,I2,I3同理
 ```
  __m128i I0 = _mm_loadu_si128((__m128i *)(src1 + (x + 0) * src1_w + k));
 ```
 ![](https://i.imgur.com/qs3XLYI.png)


 * matrix2 一次縱向讀入128bit = 四個int 存進I4 ，因為陣列縱向的記憶體位置不連續，所以要給他四個int， I5,I6,I7同理
 ```
  __m128i I4 = _mm_set_epi32 (src2[(k+3) * src2_w + y], 
  			src2[(k+2) * src2_w + y],
				src2[(k+1) * src2_w + y],
				src2[k * src2_w + y]);
```
![](https://i.imgur.com/KT5Yig6.png)


* 將I0與I4,I5,I6,I7相乘  , I1,I2,I3同理
```
 __m128i T0 = _mm_mullo_epi32(I0, I4);
 __m128i T1 = _mm_mullo_epi32(I0, I5);
 __m128i T2 = _mm_mullo_epi32(I0, I6);
 __m128i T3 = _mm_mullo_epi32(I0, I7);
```
先講解_mm_mullo_epi32(m128i, m128i)
他是 SSE4.1 的function 所以要 #include \<smmintrin.h\>
原理:
```
_mm_mullo_epi32(m128i, m128i)

 FOR j := 0 to 1
	i := j*64
	dst[i+63:i] := a[i+31:i] * b[i+31:i]
 ENDFOR
```

I0*I4 這時候我們得到T0的值會是 T0(M1 00 * M2 00 , M1 01 * M2 10 , M1 02 * M2 20 , M1 03 * M2 30)  
而我們最終要放在 matrix 00位置 的值應該要是(M1 00 * M2 00   + M1 01 * M2 10 + M1 02 * M2 20 +...+ M1 0n * M2 n0)

* 所以在這裡將他轉置
```
 __m128i T16 = _mm_unpacklo_epi32(T0, T1);
 __m128i T17 = _mm_unpacklo_epi32(T2, T3);
 __m128i T18 = _mm_unpackhi_epi32(T0, T1);
 __m128i T19 = _mm_unpackhi_epi32(T2, T3);

 __m128i T20 = _mm_unpacklo_epi64(T16, T17);
 __m128i T21 = _mm_unpackhi_epi64(T16, T17);
 __m128i T22 = _mm_unpacklo_epi64(T18, T19);
 __m128i T23 = _mm_unpackhi_epi64(T18, T19);
```

* 就可以把他們相加起來

```
T20 = _mm_add_epi32(T20,T21);
T20 = _mm_add_epi32(T20,T22);
T20 = _mm_add_epi32(T20,T23);

des0 = _mm_add_epi32(T20,des0);
```
![](https://i.imgur.com/uw2w9rP.png)


* 我們來比較一下效能

```shell=
$ ./sse
sse: 	 4070998 us
```
* 在加上 prefetch 的版本

```shell=
$ ./sse_prefetch 
sse_prefetch: 	 4120991 us
```

這邊prefetch反而比較慢，在猜可能是因為在第二個矩陣取值的時候是用縱向的，所以就算先載入，第二個矩陣取值的時候記憶體不連續還是會覆蓋掉原先載入的值。

>> 改 [prefetch 第二個矩陣](https://github.com/0140454/matrix-multiplication/commit/df7d9e8f7051bfca5f4e57e5428638f59f7c9ea5) 後，效能有所提升。
>> 
>> ```shell=
>> sse: 		 2864652 us
>> 
>> sse_prefetch: 	 1989581 us
>> ```
>> 
>> 另外，如果用 AVX 中提到的向量方式，直接運算而不轉置。可以發現其執行時間又會更短一些。
>> 
>> 程式碼可以到 [branch sse_v2](https://github.com/0140454/matrix-multiplication/tree/sse_v2) 觀看。
>> 
>> ```shell=
>> sse: 		 2310845 us
>> 
>> sse_prefetch: 	 1358132 us
>> ```
>> [name=吳勃興]

### AVX version

因為 AVX 沒有提供完整的 integer 操作函數，又電腦剛好支援 AVX2，所以這一部份就改用 AVX2 來實作。

概念和上面提及的方式差不多，都是將大矩陣分割成小矩陣來運算，因為 AVX 可以同時對 256-bit 的資料做操作，也就是 8 個 integer。因此，在這邊小矩陣的大小為 8x8。

另外，小矩陣相乘的時候並不會先進行轉置的動作，而是透過[向量的方式](https://zh.wikipedia.org/wiki/%E7%9F%A9%E9%99%A3%E4%B9%98%E6%B3%95#.EF.BC.8D.E5.90.91.E9.87.8F.E6.96.B9.E6.B3.95.3D)直接運算。

主要用到的函數有

* _mm256_setzero_si256
    將 `__m256i` 內的元素全部填零

* _mm256_loadu_si256
    從指定記憶體位置中讀取 256-bit 的資料到 `__m256i`

* _mm256_set1_epi32
    將 `__m256i` 中以某個數字填充

* _mm256_mullo_epi32
    這是 AVX2 才引入的函數，對 `__m256i` 做乘法

* _mm256_add_epi32
    也是 AVX2 才引入的函數，對 `__m256i` 做加法

* _mm256_storeu_si256
    將 `__m256i` 中的資料複製到指定的記憶體位置

而在效能方面

```shell=
$ ./avx
avx: 		 873441 us

$ ./avx_prefetch
avx_prefetch: 	 807615 us
```

### 各版本比較

**初步比較**

| 版本 | naive | sub-matrix | sse | sse_prefetch | sse_prefetch_modify |
| --- | ----- | ---------- | --- | ------------ | ------------------- |
| 時間 (us) | 17376861 | 8030999 | 2779563 | 2902576 | 1989581 |
| Speedup | 1 | 2.16 | 6.25 | 5.99 | 8.73 |

| 版本 | sse v2 | sse_prefetch v2 | avx | avx_prefetch |
| --- | ------ | --------------- | --- | ------------ |
| 時間 (us) | 2310845 | 1358132 | 873441 | 807615 |
| Speedup | 7.52 | 12.79 | 19.89 | 21.52 |

## 參考資料
* [Intel Intrinsics Guide](https://software.intel.com/sites/landingpage/IntrinsicsGuide/)
* [SSE 引入的標頭檔](http://www.aiuxian.com/article/p-1028293.html)
* [Memory part 5: What programmers can do](https://lwn.net/Articles/255364/)
* [AVX matrix-multiplication, or something like it](http://www.mindfruit.co.uk/2012/02/avx-matrix-multiplication-or-something.html)