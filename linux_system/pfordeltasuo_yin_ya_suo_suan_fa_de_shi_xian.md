# PForDelta索引压缩算法的实现


前日一个朋友给我发来了一个索引压缩算法，写得非常漂亮而且简洁，压缩比和解压性能方面大大超过目前已知的一些字节对齐的算法和Pfordelta这样的非字节对齐的算法，让人叹为观止，这是我看到的最好的压缩算法，他将会以论文的形式发表，相信必将震惊世界，我之前也写了很多Pfordelta的博客，大家对这个算法的具体实现很好奇，有几个难点，一个是bit pack和unpack，一个是关于exception的占位符在做链接的时候如果间隔超过了2的b次方如何处理等等，在以前的博客中曾应用了一个日本开发者开发的算法，但目前已经不开源了，我昨天又读了一遍pfordelta实现方面的论文【Super-Scalar RAM-CPU Cache compression】，今天一天写了个大概，bit pack和unpack在1，2，4，8，16...这种2的幂是比较容易处理的，其余的比较困难，我这里只实现了pack3和unpack3，论文【Balancing Vectorized Query Execution with Bandwidth-optimized Storage】中给出了unpack12的参考代码，理论上应该实现从pack3到pack16的全部方法，限于博客篇幅不全写了，有兴趣读者朋友依葫芦画瓢可以继续完成。

值得注意的是：（1）PFORDelta可以对0值进行压缩，这是非常有利的。（2）pack和unpack的函数指针数组也比较有特色，PACK[bitwidth](code,data,MAX);通过bitwidth值来取相应的pack和unpack函数，提高CPU流水线的通畅性。

算法本博客不做过多解释，有兴趣的朋友可以参见我此前的博客：

http://blog.csdn.net/pennyliang/archive/2010/09/25/5905691.aspx

另外，我这位朋友想让我做一个最快的Pfordelta算法来PK一下，会输多少，我知道肯定会输，而且会很大，只是想尽可能做个最合格的对手，如果有读者朋友有更好的实现，也请发给我参考，非常感谢。


```c
#include<stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
const int N = 64;
const int MAX = 64;
uint32_t MAXCODE[] = {0,
                      1,//1U<<0 - 1
                      3,//1u<<1 - 1
                      (1U << 3) - 1,
                      (1U << 4) - 1,
                      (1U << 5) - 1,
                      (1U << 6) - 1,
                      (1U << 7) - 1,
                      (1U << 8) - 1,
                      (1U << 9) - 1,
                      (1U << 10) - 1,
                      (1U << 11) - 1,
                      (1U << 12) - 1,
                      (1U << 13) - 1,
                      (1U << 14) - 1,
                      (1U << 15) - 1,
                      (1U << 16) - 1,
                      (1U << 17) - 1,
                      (1U << 18) - 1,
                      (1U << 19) - 1,
                      (1U << 20) - 1,
                      (1U << 21) - 1,
                      (1U << 22) - 1,
                      (1U << 23) - 1,
                      (1U << 24) - 1,
                      (1U << 25) - 1,
                      (1U << 26) - 1,
                      (1U << 27) - 1,
                      (1U << 28) - 1,
                      (1U << 29) - 1,
                      (1U << 30) - 1,
                      0x7FFFFFFF,
                      0xFFFFFFFF,
                     };
typedef void(*pattern_fun)(uint32_t* code, uint32_t* data, size_t n);



void PACK3(uint32_t* code, uint32_t* data, size_t n)
{
    for (int i = 0, j = 0; j < n; i += 3, j += 32) {
        code[i] = 0;
        code[i + 1] = 0;
        code[i + 2] = 0;
        code[i] |= data[j + 0] << 0;
        code[i] |= data[j + 1] << 3;
        code[i] |= data[j + 2] << 6;
        code[i] |= data[j + 3] << 9;
        code[i] |= data[j + 4] << 12;
        code[i] |= data[j + 5] << 15;
        code[i] |= data[j + 6] << 18;
        code[i] |= data[j + 7] << 21;
        code[i] |= data[j + 8] << 24;
        code[i] |= data[j + 9] << 27;
        code[i] |= (data[j + 10] & 0x00000003) << 30;
        code[i + 1] |= (data[j + 10] & 0x00000004) >> 2;
        code[i + 1] |= data[j + 11] << 1;
        code[i + 1] |= data[j + 12] << 4;
        code[i + 1] |= data[j + 13] << 7;
        code[i + 1] |= data[j + 14] << 10;
        code[i + 1] |= data[j + 15] << 13;
        code[i + 1] |= data[j + 16] << 16;
        code[i + 1] |= data[j + 17] << 19;
        code[i + 1] |= data[j + 18] << 22;
        code[i + 1] |= data[j + 19] << 25;
        code[i + 1] |= data[j + 20] << 28;
        code[i + 1] |= (data[j + 21] & 0x00000001) << 31;
        code[i + 2] |= (data[j + 21] & 0x00000006) >> 1;
        code[i + 2] |= data[j + 22] << 2;
        code[i + 2] |= data[j + 23] << 5;
        code[i + 2] |= data[j + 24] << 8;
        code[i + 2] |= data[j + 25] << 11;
        code[i + 2] |= data[j + 26] << 14;
        code[i + 2] |= data[j + 27] << 17;
        code[i + 2] |= data[j + 28] << 20;
        code[i + 2] |= data[j + 29] << 23;
        code[i + 2] |= data[j + 30] << 26;
        code[i + 2] |= data[j + 31] << 29;
    }
}



void UNPACK3(uint32_t* data, uint32_t* code, size_t n)
{
    for (int i = 0, j = 0; i < n; i += 32, j += 3) {
        data[i + 0] = ((code[j + 0] & 0x00000007) >> 0); // 000......111
        data[i + 1] = ((code[j + 0] & 0x00000038) >> 3); // 000...111000
        data[i + 2] = ((code[j + 0] & 0x000001C0) >> 6);
        data[i + 3] = ((code[j + 0] & 0x00000E00) >> 9);
        data[i + 4] = ((code[j + 0] & 0x00007000) >> 12); //
        data[i + 5] = ((code[j + 0] & 0x00038000) >> 15); //
        data[i + 6] = ((code[j + 0] & 0x001C0000) >> 18);
        data[i + 7] = ((code[j + 0] & 0x00E00000) >> 21);
        data[i + 8] = ((code[j + 0] & 0x07000000) >> 24); //
        data[i + 9] = ((code[j + 0] & 0x38000000) >> 27); //

        data[i + 10] = ((code[j + 0] & 0xC0000000) >> 30) | (code[j + 1] & 0x00000001)
                       << 2;

        data[i + 11] = ((code[j + 1] & 0x0000000E) >> 1);
        data[i + 12] = ((code[j + 1] & 0x00000070) >> 4);
        data[i + 13] = ((code[j + 1] & 0x00000380) >> 7);
        data[i + 14] = ((code[j + 1] & 0x00001C00) >> 10);
        data[i + 15] = ((code[j + 1] & 0x0000E000) >> 13);
        data[i + 16] = ((code[j + 1] & 0x00070000) >> 16);
        data[i + 17] = ((code[j + 1] & 0x00380000) >> 19);
        data[i + 18] = ((code[j + 1] & 0x01C00000) >> 22);
        data[i + 19] = ((code[j + 1] & 0x0E000000) >> 25);
        data[i + 20] = ((code[j + 1] & 0x70000000) >> 28);

        data[i + 21] = ((code[j + 1] & 0x80000000) >> 31) | (code[j + 2] & 0x00000003)
                       << 1;

        data[i + 22] = ((code[j + 2] & 0x0000001C) >> 2);
        data[i + 23] = ((code[j + 2] & 0x000000E0) >> 5);
        data[i + 24] = ((code[j + 2] & 0x00000700) >> 8);
        data[i + 25] = ((code[j + 2] & 0x00003800) >> 11);
        data[i + 26] = ((code[j + 2] & 0x0001C000) >> 14);
        data[i + 27] = ((code[j + 2] & 0x000E0000) >> 17);
        data[i + 28] = ((code[j + 2] & 0x00700000) >> 20);
        data[i + 29] = ((code[j + 2] & 0x03800000) >> 23);
        data[i + 30] = ((code[j + 2] & 0x1C000000) >> 26);
        data[i + 31] = ((code[j + 2] & 0xE0000000) >> 29);
    }
}
pattern_fun PACK[] = {NULL, NULL, NULL, PACK3};
pattern_fun UNPACK[] = {NULL, NULL, NULL, UNPACK3};



int compress(size_t n, int bitwidth, uint32_t* input, uint32_t* code,
             uint32_t* exception, uint32_t* f_e)
{
    uint32_t miss[N], data[N], prev = 0;
    int j = 0;
    int last_e = -1;

    for (int i = 0; i < MAX; i++) {
        int val = input[i];
        data[i] = val;
        miss[j] = i;

        if ((val > MAXCODE[bitwidth])) {
            j++;
            last_e = i;
        } else if (last_e >= 0 && ((i - last_e)) > MAXCODE[bitwidth]) {
            j++;
            last_e = i;
        } else {}

    }

    if (j > 0) {
        *f_e = miss[0];
        prev = miss[0];
        exception[0] = input[prev];

        for (int i = 1; i < j; i++) {
            int cur = miss[i];
            exception[i] = input[cur];
            data[prev] = (cur - prev) - 1;
            prev = cur;
        }
    }

    PACK[bitwidth](code, data, MAX);
    return j;
};

int decompress(size_t exception_cnt, int bitwidth, uint32_t* output,
               uint32_t* input, uint32_t* exception, uint32_t* f_e)
{
    uint32_t next, code[N], cur = *f_e;
    UNPACK[bitwidth](code, input, MAX);

    for (int i = 0; i < MAX; ++i) {
        output[i] = code[i];
    }

    if (cur != MAX && exception_cnt > 0)

    {
        cur = *f_e;
        next = output[cur];
        output[cur] = exception[0];
        cur += next + 1;

        for (int i = 1, j = 1; cur < MAX && j < exception_cnt; i++, j++) {
            next = cur + code[cur] + 1 ;
            output[cur] = exception[i];
            cur = next;
        }
    }

    return MAX;
}

int main(void)
{
    uint32_t input[] = {10, 13, 14, 16, 20, 22, 25, 30, 37, 40, 44, 47, 48, 50, 54, 56, 58, 63, 70, 73, 74, 77, 78, 80, 84, 86, 89, 94, 101, 104, 106, 109, 110, 112, 115, 117, 120, 121, 123, 133, 141, 151, 152, 157, 158, 166, 168, 178, 186, 195, 196, 202, 203, 209, 299, 301, 304, 329, 336, 339, 352, 354, 357, 359};
    uint32_t* code = (uint32_t*)malloc(1024 * sizeof(uint32_t));
    uint32_t* output = (uint32_t*)malloc(1024 * sizeof(uint32_t));
    uint32_t* exception = code + 6;
    uint32_t  first_exception = MAX;
    uint32_t compressed_temp_arr[MAX] = {0};
    compressed_temp_arr[0] = input[0];

    for (int i = 1; i < MAX; ++i) {
        compressed_temp_arr[i] = input[i] - input[i - 1] - 1;
    }

    int j = compress(MAX, 3, compressed_temp_arr, code, exception,
                     &first_exception);

    decompress(j, 3, output, code, exception, &first_exception);

    for (int i = 1; i < MAX; ++i) {
        output[i] = output[i] + output[i - 1] + 1;
    }

    for (int i = 0; i < MAX; ++i) {
        printf("%d,", output[i]);
    }

    printf("/n");

    for (int i = 0; i < MAX; ++i) {
        printf("%d,", input[i]);
    }

    printf("/n");
    int ori_size = sizeof(input) / sizeof(uint32_t) * 32;
    int com_size = j + MAX * 3;
    float ratio = ori_size / com_size;
    printf("original size:%d bit,compressed size:%d bit,compressed ratio:%f/n,bit per docid:%f",
           ori_size, com_size, ratio, com_size * 1.0f / 64);
    return 0;
}
```