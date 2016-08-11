# C 語言的潛規則型態轉換


同樣是看過[C Programming: A Modern Approach](http://knking.com/books/c2/index.html)的筆記整理。一樣，寫的時候手上沒書，請自行斟酌，盡信書不如無書，更何況是組裝工亂寫的東西呢。

一般來說，變數型態可以強制轉換，這學過C 語言應該都知道。不過看了書上才發現不只強制轉換，compiler也會幫你的程式碼加料、做型態轉換。更恐怖的是，沒注意到的話，會發生慘劇。

本文開始之前，先定義專有名詞：

* promotion
  * 當然不是升官還是促銷。而是把變數的型態升級，如short int -> int。為什麼要這樣幹呢？因為expression裡面變數不只一個，做運算的時候需要把某些變數升級，以免計算結果超過預期。諷刺的是，因為這樣的方式，也可能讓你的程式行為和你預期的不同。
  
書中提到潛規則型態轉換發生的時機如下

* expression中變數型態不同
* assign的left value和right value型態不同
* 函數中的參數變數型態和函數內部使用該變數時的型態不同
* 函數的回傳值和宣告的不同

後面兩種大概就是下面這樣的情況吧

```c 
long test(int i)
{
    i = 0.5f;
    
    return i;
}
```

書中用下面的方式分類C 語言的潛規則型態轉換規則，分別整理如下，偷懶不講C99了。


## assignment
這個最簡單，等號左邊用啥型態右邊就得轉成這樣的型態。這也是為何

```c
    int i = 11.039;
    printf("%d", i);
```
結果會是11的原因。

## expression
一樣這是compiler的術語。此這邊就是一堆的歡樂的型態排列組合，C99還有`_Complex`和`_Bool`參戰。

一樣先分類

* expression中的變數只有小數點那種型態，依下面順序promote: `float` -> `double` -> `long double`
* expression中的變數只有整數，就變成
  * promote時要判斷使用`unsigned` 還是`signed`整數依照下面的順序promote: 
    * ... `int` -> `unsigned int` -> `long int` -> `unsigned long int` ...
  * 這種情況沒注意的話，可能就會出現下面的悲劇

- implicit.c

```c 
#include <stdio.h>
void implicit_conv()
{
    int i = -1;
    unsigned int j = 100;

    if (i < j) {
        printf("expected\n"); 
    }
    else {
        printf("GG\n");     
    }
}

int main(int argc, char **argv)
{
    implicit_conv();
    
    return 0;
}
```

執行結果如下
```sh
$ make implicit
cc     implicit.c   -o implicit

$ ./implicit 
GG

```
為何會GG呢，因為 `i < j`是一個**expression**，照上面的規矩，expression同時有`int`和`unsigned int`的話，`int`會被promote的`unsigned int`，`-1`的二進位不知道的人，可能要先搞懂再來學C吧？

如果你擔心程式有類似的問題，可以把`gcc`最囉唆的檢查打開，就會噴出錯誤如下

```sh
$ make implicit CFLAGS="-Wall -Wextra -Werror"
cc -Wall -Wextra -Werror   -c -o implicit.o implicit.c
implicit.c: In function ‘implicit_conv’:
implicit.c:7:11: error: comparison between signed and unsigned integer expressions [-Werror=sign-compare]
     if (i < j) {
           ^
implicit.c: In function ‘main’:
implicit.c:15:14: error: unused parameter ‘argc’ [-Werror=unused-parameter]
 int main(int argc, char **argv)
              ^
implicit.c:15:27: error: unused parameter ‘argv’ [-Werror=unused-parameter]
 int main(int argc, char **argv)
                           ^
cc1: all warnings being treated as errors
make: *** [implicit.o] Error 1
```

你可能會問，那有小數點的型態和整數型態亂戰會怎樣呢？自己估狗或看書吧。

## 題外話 
### 題外話一
由於這些潛規則，在轉換型態的時候，可以看到這樣的statement。

```c
double whatsoever;
whatsoever = (double) 10 / 3;
```
為什麼`3`不用型態轉換呢？你必須要比對C語言的[Operator precedence](http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B#Operator_precedence)，也就是運算元處理順序。可以知道C 語法會處理順序如下

* `10` cast成`double`型態
* 因為潛規則，`3`也會被promote成`doule`
* 計算`10 / 3`
* 將結果assign給 `=` 左邊的變數

### 題外話二
在C語言中有小數點的常數預設型態可是`double`唷。所以你如果有`float`最好使用下面的方式轉換型態。至於為什麼會規定是`double`呢？書中有講八卦，就不破梗了。

```c
float whatsoever = 3.14f;
```
