# ARM CM4 Pratice (3): USART 初探



##致謝

感謝網友Zack，Villar，學弟Joe Ho，還有其他大大的幫忙，不然這次應該是撞牆撞到死吧。

##前言

這次實驗有卡關，不然其實不算難。卡關的點如下：

- 一開始使用USART1，可是USART1接到STLink 接腳，最後用USART6代替。(STM32F4 Discovry Disco 開發版手冊，p19, SB11那段)

- SPL的HSE 設定和版子不合，造成Baud rate計算錯誤。

這次的實驗是一個ECHO程式，透過版子上的USART6和電腦連線，電腦送出什麼字元，版子就傳回什麼字元。

##目錄

- 事前準備
- 測試環境
- USART 控制
- 程式碼
    - 完整程式碼
    - Makefile
- 功能驗證
- 參考資料

##事前準備

- Saleae 邏輯分析儀 (一千新台幣有找）
    - 需要自行到Saleae官方網站下載安裝Linux版軟體

