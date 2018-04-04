# 如何使用篇


- 為什麼要使用Service?
    - 將元件的生命週期跟Thread的生命週期分開(避免前述Thread參考到元件, 在Thread結束前無法釋放物件導致Memory leak)
    - 當一個Process內只剩下Thread在執行, 避免Process被系統意外回收, 導致Thread被提前結束
    - 跨行程演出


Service分成兩種: startService和bindService。

startService
由第一個元件啟動, 由第一個元件結束, 這個是最常見的, 也是最簡單的啟動方式。
一開始先宣告一個Service的類別