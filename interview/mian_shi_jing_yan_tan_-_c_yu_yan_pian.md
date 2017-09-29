# 面試經驗談 - C 語言篇


年紀一把還要被人確認 C 語言程度實在蠻冏的，不過兩個月沒 coding，程度真的有點下降(拜某幸福企業所賜)，只好再進牛棚熱身一下

這邊揀選的題目，是這1、2個月碰到出現率偏高的題目。這些題目相信很多人都看過了，但這邊不但列出所有的解法，而且還附上大量參考資料，夠有誠意吧？

##問題 1: 無號整數裡位元 == 1 的個數

###解法 1
最基本的解法，沒什麼可以多說的，除非你對 C 語言位移運算很生疏

```c
int bitcount(unsigned int x)
{
    int count, i;
     
    for(i = 0, count = 0; i < 32; ++i)
        if((1<<i) & x)
            ++count;
         
    return count;
}
```

### 解法 2

```c
int bitcount(unsigned int x)
{
    int count;
 
    for(count = 0; x != 0; x &= x - 1)
        ++count;
 
    return count;
}
```

解法 2 出自 The C Abswer Book 2nd，也就是 K&R C 2nd 習題 2-9 的解答。這個解法的原理利用最右邊的 bit - 1 時產生的位元變化。首先我們知道：

x = (x - 1) + 1

這邊假設 x 是個只有 4bits 長度的無號整數，x = 1010b：

![](./images/1.png)

![](./images/2.png)

![](./images/3.png)

![](./images/4.png)

![](./images/5.png)

![](./images/6.png)

![](./images/pic0.png.png)