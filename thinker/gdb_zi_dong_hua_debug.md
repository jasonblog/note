# GDB 自動化 debug

近來，有很多人都習慣寫 unittest ，確保程式的正確性。我也是 unittest 的愛用者。然而， unittest 主要針對模組進行單元性的測試，但有些整合性問題，若要透過一般 unittest 的工具，勢必需要在程式裡埋一些 run time 用不到的 code，進行一些記錄。例如，你想知道程式執行時，是否按某種規則、協定呼叫模組。按一般做法，大概必需在模組的進入點，埋下一些程式碼，檢查呼叫的次序和流程是否依照協定。這類的 code ，通常很雜亂，因為檢查的 function 數量可能不少，進而邏輯很分散。如果能有一種方法，能將這些邏輯集中，那不是很美滿嗎? 這時 GDB 加上 scripting 就能幫上忙。
舉例，我在刻 MadButterfly 時，想要知道畫面上某個物件移動時，是否只重畫周圍相關的圖形，而不是整個畫面。這時，我可以透過 gdb ，埋一些 break point ，並加上 scripting 的指令，記錄每一次物件移動時，有多少相關的圖形被重畫。於是我在 gdb prompt 下指令
```c
set pagination off

break object_move

commands
silent
set $draw_cnt=0
continue
end

break object_draw

commands
silent
set $draw_cnt = $draw_cnt + 1
continue
end

break where_redraw_completed

commands
silent
print $draw_cnt
continue
end

run
```
commands 指令，會 attach 到前一個 break 指令設定的 break point 。當 break point 被觸發時，commands 和 end 之間的指令就會被執行，就像你在 prompt 下指令一樣。於是這些指令會在 break point 被觸發時，自動被執行，包括 continue 指令。由於最後一個指令是 continue ，於是每當那些 break point 被觸發，程式並不會在執行這些指令後停止，而是繼續執行 (continue)。 然而，每個 break point 所 attach 的指令，也包括了 silent 這個指令，這能避免 gdb 在 break point 觸發之後，自動秀出一些狀態、訊息。而 set pagination off 則能避免 gdb 將 print 出來的資料，進行像 more 指令一樣的分頁。若不關掉 pagination ， gdb 會因為等待使用者按下一頁，而使程式暫停。就這樣，程式能不斷的執行，而我們又能監看 update 的數量。

相同的技術，也可用於執行整合性的 test case 。 gdb 的 command 也包括一些 if-else 、 while 等指令。(請見 ) 而這些指令也能寫成一個文字檔，當成 script 執行。透過這樣的技巧，希望也能對你的開發工作有所幫助。