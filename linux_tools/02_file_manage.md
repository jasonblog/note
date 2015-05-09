# 文件及目錄管理


目錄

文件及目錄管理
創建和刪除
目錄切換
列出目錄項
查找目錄及文件 find/locate
查看文件內容
查找文件內容
文件與目錄權限修改
給文件增加別名
管道和重定向
設置環境變量
Bash快捷輸入或刪除
綜合應用
總結
文件管理不外乎文件或目錄的創建、刪除、查詢、移動，有mkdir/rm/mv

文件查詢是重點，用find來進行查詢；find的參數豐富，也非常強大；

查看文件內容是個大的話題，文本的處理有太多的工具供我們使用，在本章中只是點到即止，後面會有專門的一章來介紹文本的處理工具；

有時候，需要給文件創建一個別名，我們需要用到ln，使用這個別名和使用原文件是相同的效果；

2.1. 創建和刪除
創建：mkdir
刪除：rm
刪除非空目錄：rm -rf file目錄
刪除日誌 rm *log (等價: $find ./ -name “*log” -exec rm {} ;)
移動：mv
複製：cp (複製目錄：cp -r )
查看當前目錄下文件個數:

```
$find ./ | wc -l
```

複製目錄:
```
$cp -r source_dir  dest_dir
```

2.2. 目錄切換
找到文件/目錄位置：cd
切換到上一個工作目錄： cd -
切換到home目錄： cd or cd ~
顯示當前路徑: pwd
更改當前工作路徑爲path: $cd path
2.3. 列出目錄項
顯示當前目錄下的文件 ls
按時間排序，以列表的方式顯示目錄項 ls -lrt
以上這個命令用到的頻率如此之高，以至於我們需要爲它建立一個快捷命令方式:

在.bashrc 中設置命令別名:
```
alias lsl='ls -lrt'
alias lm='ls -al|more'
```

這樣，使用lsl，就可以顯示目錄中的文件按照修改時間排序；以列表方式顯示；

注：.bashrc 在/home/你的用戶名/ 文件夾下，以隱藏文件的方式存儲；可使用 ls -a 查看；

2.4. 查找目錄及文件 find/locate
搜尋文件或目錄:
```
$find ./ -name "core*" | xargs file
```

查找目標文件夾中是否有obj文件:
```
$find ./ -name '*.o'
```

遞歸當前目錄及子目錄刪除所有.o文件:
```
$find ./ -name "*.o" -exec rm {} \;
```

find是實時查找，如果需要更快的查詢，可試試locate；locate會爲文件系統建立索引數據庫，如果有文件更新，需要定期執行更新命令來更新索引庫:

```
$locate string
```

尋找包含有string的路徑:

```
$updatedb
```

與find不同，locate並不是實時查找。你需要更新數據庫，以獲得最新的文件索引信息。

2.5. 查看文件內容
查看文件：cat vi head tail more

顯示時同時顯示行號:

```
$cat -n
```

按頁顯示列表內容:

```
$ls -al | more
```

只看前10行:

```
$head - 10 **
```
顯示文件第一行:
```
$head -1 filename
```

顯示文件倒數第五行:
```
$tail -5 filename
```

查看兩個文件間的差別:
```
$diff file1 file2
```

動態顯示文本最新信息:
```
$tail -f crawler.log
```

2.6. 查找文件內容
使用egrep查詢文件內容:
```
egrep '03.1\/CO\/AE' TSF_STAT_111130.log.012
egrep 'A_LMCA777:C' TSF_STAT_111130.log.035 > co.out2
```

2.7. 文件與目錄權限修改
改變文件的擁有者 chown
改變文件讀、寫、執行等屬性 chmod
遞歸子目錄修改： chown -R tuxapp source/
增加腳本可執行權限： chmod a+x myscript
2.8. 給文件增加別名
創建符號鏈接/硬鏈接:
```
ln cc ccAgain :硬連接；刪除一個，將仍能找到；
ln -s cc ccTo :符號鏈接(軟鏈接)；刪除源，另一個無法使用；（後面一個ccTo 爲新建的文件）
```

2.9. 管道和重定向
批處理命令連接執行，使用 |
串聯: 使用分號 ;
前面成功，則執行後面一條，否則，不執行:&&
前面失敗，則後一條執行: ||

```
ls /proc && echo  suss! || echo failed.
```

能夠提示命名是否執行成功or失敗；

與上述相同效果的是:

```
if ls /proc; then echo suss; else echo fail; fi
```

重定向:
```
ls  proc/*.c > list 2> &l 將標準輸出和標準錯誤重定向到同一文件；
```

等價的是:
```
ls  proc/*.c &> list
```

清空文件:
```
:> a.txt
```

重定向:
```
echo aa >> a.txt
```

2.10. 設置環境變量
啓動帳號後自動執行的是 文件爲 .profile，然後通過這個文件可設置自己的環境變量；

安裝的軟件路徑一般需要加入到path中:
```
PATH=$APPDIR:/opt/app/soft/bin:$PATH:/usr/local/bin:$TUXDIR/bin:$ORACLE_HOME/bin;export PATH
```

2.11. Bash快捷輸入或刪除
快捷鍵:
```
Ctl-U   刪除光標到行首的所有字符,在某些設置下,刪除全行
Ctl-W   刪除當前光標到前邊的最近一個空格之間的字符
Ctl-H   backspace,刪除光標前邊的字符
Ctl-R   匹配最相近的一個文件，然後輸出
```

2.12. 綜合應用
查找record.log中包含AAA，但不包含BBB的記錄的總數:
```
cat -v record.log | grep AAA | grep -v BBB | wc -l
```

2.13. 總結
文件管理，目錄的創建、刪除、查詢、管理: mkdir rm mv

文件的查詢和檢索: find locate

查看文件內容：cat vi tail more

管道和重定向: ; | && >
