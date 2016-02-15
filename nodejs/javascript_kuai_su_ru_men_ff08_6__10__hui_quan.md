# JavaScript 快速入門（6/10）－ 迴圈


迴圈是一種重複執行的運算，一個典型的迴圈，包含兩個主要的部份－分別是是判斷式以及重複執行的程式區塊，JavaScript 有兩種主要的迴圈－ for 與 while ，首先來看 for 迴圈，語法如下：

```js
for (init;cond;increment){

     // for 迴圈執行程式

}
```

for關鍵字定義迴圈重複執行的次數以及何時結束：
- 小括弧的內容定義迴圈計數器，控制區塊中程式敘述的執行次數，其中的init初始化計數器的起始值，這個運算式只會執行一次。
- cond 是一個判斷式，定義計數器何時停止，如果它的結果是true，表示迴圈將繼續重複執行，否則跳出迴圈。
- increment 負責改變計數器的值，通常是將計數器的值進行遞增或是遞減的操作。
每一次for區塊裡面的程式敘述執行完畢時，它會跳回for重新執行小括弧中的運算式，一直到cond的結果為false，則跳出迴圈。考慮以下的程式片段：

```js
<script>
    for (var i = 0; i < 10 ; i++) {
        console.log(i);
    }
</script>
```

for 關鍵字後方的第一個條件式定義了 i 計數器變數，第二個條件式要求當 i 小於 10 執行其中的內容，否則跳出迴圈，最後一個條件式則於每一次迴圈開始執行時將 i 加 1 ，逐一在控制檯輸出 0~ 9 的數字。

##》迴圈 while 與 do/while

while 是另外一種類型的迴圈，當執行的迴圈沒有固定次數，無法透過for迴圈來達到重複執行的目的，while迴圈很適合用來處理這樣的情況。

```js
while(expression){ 
     // expression 是 true 則執行這裡的語法　…　
}
```

expression運算式傳回布林運算結果，如果這個結果值是true，其中while區塊內的程式碼便會被執行，完成之後程式會再回到 while 重新執行一次exprssion，一直到它的值為false為止。考慮以下的程式碼：

```js
<script>
    var i = 500;
    var j = 1000;
    while (i < j) {
        i += 3;
        j += 1;
    }
    console.log("i=" + i + ";j=" + j);
</script>
```


變數i與j的初始值設定為500與1000，接下來的while迴圈，每一次為變數i加上3，變數j加上1，一直到i的值不再小於j　便停止迴圈，輸出i與j的內容。

```js
i=1250;j=1250
```

以上為最後的輸出結果，其中 i 與 j 的值已經相等，再一次迴圈則 i 將大於 j，因此會跳出迴圈。while迴圈另外還有一個do-while版本，語法如下：

```js
do{
     // expression 是 true 則執行這裡的語法　…　
　  
} while(expression)
```


其中關鍵字do宣告while迴圈開始，而while運算式則在迴圈結束的時候進行判斷，如果結果值是true，這個迴圈會再一次執行，否則跳出迴圈。與上述while迴圈的差異，在於這個迴圈本身無論如何，會先執行區塊中的程式碼一次，既使while的expression執行結果為false。while與do/while在大部份場合的執行結果並沒有差異，但是它會影響第一次的執行，考慮以下的程式碼：

```js
<script>
    var i = 1;
    var number1 = 2;
    while (number1 % 2 != 0) {
        console.log("number1：" + number1);
        number1 = 3 * i;
        i++;
    }
    var j = 1;
    var number2 = 2;
    do {
        console.log("number2：" + number2);
        number2 = 3 * j;
        j++;
    } while (number2 % 2 != 0);

</script>
```

第一段迴圈先檢視number1是否除以2不等於零，也就是如果它是奇數的話，就進入迴圈，輸出其變數值，然後將變數i乘以3設定給它，最後將i加上1。另外一段while迴圈先輸出number2這個變數，然後將變數j乘上3，最後則將j加1，而最後的while檢視number2是否為奇數，是的話則繼續下一個迴圈。

```js
number2：2
number2：3
```

範例中的兩段迴圈，每一段均針對一個指定的變數進行奇偶數的判斷，然後這個變數會被調整乘上3的倍數，再進行下一次的判斷，由於一開始它的初始值是一個偶數，因此第一段迴圈無法通過while檢視，沒有任何輸出。相反的，第二段迴圈由於無論如何必須先執行一次，因此它會直接輸出其中的number2，但是一進入迴圈之後，number2被調整成為3的倍數，因此迴圈結束的while判斷還是會通過，直到它變成6，才結束迴圈的執行。

##》break與continue

在預設的情形下，迴圈只有到達指定的條件時才會結束執行，我們可以透過break敘述強制中斷迴圈，將其配置於迴圈區塊內指定的位置，就能達到中斷的效果。以稍早討論的 while 為例，break 語法如下：


```js
while(true){
     …　
     break ; 　// 程式在這裡中斷跳出迴圈
     …
}
```

當程式執行到break這一行敘述的時候，迴圈便會中斷，跳出整個大括弧的執行範圍區塊。break會無條件中斷迴圈，因此通常搭配if敘述判斷是否要執行，只有在某些條件符合的情形下，才會執行 break 敘述。考慮以下的程式碼：

```js
<script>
    var count = 0;
    while (true) {
        count++;
        if (count > 100)
            break;
    }
    console.log(count);
</script>
```

while敘述以常數值true表示建立一個無窮迴圈，每一次迴圈執行時，將變數count加1，if判斷式檢視count是否已經大於100，是的話則引用break，中斷迴圈的執行。最後這個範例輸出的變數值為101。

瞭解break之後，現在來看continue，它會在執行時跳出迴圈，但不會停止迴圈的執行，而是繼續下一次的迴圈，我們可以利用這種特性要求迴圈執行部份的內容。例如以下的程式片段：

```js
<script>
    var i = 0;
    var result = 0;
    for (i = 1; i <= 100; i++) {
        if (i % 2 == 0)
            continue;
        result += i;
    }
    console.log(result);
</script>
```

coutinue會從目前的迴圈中跳開直接進入下一次的迴圈，這一點是與break最大的差異，如果引用break，這個迴圈將永遠終止跳出，而不會有下一次的執行機會。