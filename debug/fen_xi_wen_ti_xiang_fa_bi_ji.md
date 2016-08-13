# 分析問題想法筆記

## 分析log 技巧
- 找一份正常 log 比對異常 log 差異 (如果有 log level 針對Error or Warning看)
- crash 的 那支程式 pid 看 log 最後出現的 pid 在程式碼哪邊分析crash 點
- 平常針對負責的程式部份把從log 流程知道有哪些 thread 有合作關鍵

##流水號 - 可比對兩端數據 ＆ sample rate 是否一致 ?

加上流水號比對兩端數據差異 (發送版端跟接手手機的數據是否一樣)

##發現兩個部份程式碼相同, 但結果不一致 ?
仔細從頭到尾整個流程哪有差異 (一定存在差異)
ex: PC & 板子 cardboard algo 差異在 sensor raw data 是兩各不同 chip
板子透過 usb 到手機 , 結果 usb 程式部份有 bug
但是 PC 吃 vive 的 usb 是出貨產品一定沒問題

想法找出我們自己加上的部份可能會有 bug  (usb 是我們自己開發)

##timer 問題打不到 bug
問題點用累加 111  222 333 只累加不打印(打印會影響效能進而影響測試結果) , 另外一條 threads 打印累加結果


##板端 跟 手機端哪邊數據異常？

板端數據不 smooth 就把 quat wxyz 都填上 0 , 代表板子


## 利用git比較差異

利用git在不同參數編譯後產生差異東西


## 一樣程式在不同平台出現差異問題
- 不同 input (raw data)
- 仔細比對之間程式碼是否真的相同

