# I2C protocol 時間計算


I2C protocol 原理及應用有提供了一個範例程式碼，裏面的 i2c_wait 在當時是用試誤法來測出需要幾個 nop 指令。最近正好在做新的案子，使用不同的 crystal，就想用時脈來計算出真正需要多少個 nop 指令。
`以下推導過程，使用 22.1184MHz 的振盪器，I²C匯流排速度為標準模式（100 Kbit/s）`。

- 1 clock = 1/22.1184 us
- 1 machine cycle = 12 clock = 12/22.1184 us
- 1 nop = 1 machine cycle = 12/22.1184 us = 542.534722 ns
- 100 kbit/s = 1bit per 1/100000s = 10 us
- 1 bit cycle = 10/0.542 = 18.432 machine cycle
- 2 * i2c_wait = 1 bit cycle ==> i2c_wait = 9.2 machine cycle = 9 nop

實際上使用的 nop 有 8 個，可以正常 read/write 256 bytes eeprom。但是由於還會呼叫 
lcall，ret 等指令，理論上應該少更多才對。不知是 eeprom 不穩，無法達到 100Kbit/s，還是其它原因？
`以下推導過程，使用 40MHz 的振盪器，I²C匯流排速度為標準模式（100 Kbit/s）`。

- 1 clock = 1/40 us
- 1 machine cycle = 12 clock = 12/40 us
- 1 nop = 1 machine cycle = 12/40 us = 0.3 us
- 100 kbit/s = 1bit per 1/100000s = 10 us
- 1 bit cycle = 10/0.3 = 33.33 machine cycle
- 2 * i2c_wait = 1 bit cycle ==> i2c_wait = 16.67 machine cycle = 16 nop

實際上使用的 nop 有 10 個，只有測試讀寫 1 byte。由於還會呼叫 lcall，ret 等指令，所以少了 6 個 nop 是可以理解的。但是實際上測試需要做讀寫大量的資料，不然像上例的 eeprom，單獨讀寫一個沒問題，大量讀寫時，卻出現很多錯誤。

