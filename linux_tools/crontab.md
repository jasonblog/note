# crontab 定時任務
通過crontab 命令，我們可以在固定的間隔時間執行指定的系統指令或 shell script腳本。時間間隔的單位可以是分鐘、小時、日、月、周及以上的任意組合。這個命令非常適合週期性的日誌分析或數據備份等工作。

### 19.1. 命令格式
crontab [-u user] [ -e | -l | -r ]

### 19.2. 命令參數
- -u user：用來設定某個用戶的crontab服務；
- file：file是命令文件的名字,表示將file做為crontab的任務列表文件並載入crontab。如果在命令行中沒有指定這個文件，crontab命令將接受標準輸入（鍵盤）上鍵入的命令，並將它們載入crontab。
- -e：編輯某個用戶的crontab文件內容。如果不指定用戶，則表示編輯當前用戶的crontab文件。
- -l：顯示某個用戶的crontab文件內容，如果不指定用戶，則表示顯示當前用戶的crontab文件內容。
- -r：從/var/spool/cron目錄中刪除某個用戶的crontab文件，如果不指定用戶，則默認刪除當前用戶的crontab文件。
- -i：在刪除用戶的crontab文件時給確認提示。

### 19.3. crontab的文件格式
分 時 日 月 星期 要運行的命令

- 第1列分鐘1～59
- 第2列小時1～23（0表示子夜）
- 第3列日1～31
- 第4列月1～12
- 第5列星期0～6（0表示星期天）
- 第6列要運行的命令

### 19.4. 常用方法
### 創建一個新的crontab文件
向cron進程提交一個crontab文件之前，首先要設置環境變量EDITOR。cron進程根據它來確定使用哪個編輯器編輯crontab文件。9 9 %的UNIX和LINUX用戶都使用vi，如果你也是這樣，那麼你就編輯$HOME目錄下的. profile文件，在其中加入這樣一行:
```
EDITOR=vi; export EDITOR
```
然後保存並退出。不妨創建一個名為<user> cron的文件，其中<user>是用戶名，例如， davecron。在該文件中加入如下的內容。
```
# (put your own initials here)echo the date to the console every
# 15minutes between 6pm and 6am
0,15,30,45 18-06 * * * /bin/echo 'date' > /dev/console
```
保存並退出。注意前面5個域用空格分隔。

在上面的例子中，系統將每隔1 5分鐘向控制檯輸出一次當前時間。如果系統崩潰或掛起，從最後所顯示的時間就可以一眼看出系統是什麼時間停止工作的。在有些系統中，用tty1來表示控制檯，可以根據實際情況對上面的例子進行相應的修改。為了提交你剛剛創建的crontab文件，可以把這個新創建的文件作為cron命令的參數:
```
$ crontab davecron
```
現在該文件已經提交給cron進程，它將每隔1 5分鐘運行一次。同時，新創建文件的一個副本已經被放在/var/spool/cron目錄中，文件名就是用戶名(即dave)。

### 列出crontab文件
使用-l參數列出crontab文件:
```
$ crontab -l
0,15,30,45,18-06 * * * /bin/echo `date` > dev/tty1
```
可以使用這種方法在$HOME目錄中對crontab文件做一備份:
```
$ crontab -l > $HOME/mycron
```
這樣，一旦不小心誤刪了crontab文件，可以用上一節所講述的方法迅速恢復。

### 編輯crontab文件
如果希望添加、刪除或編輯crontab文件中的條目，而EDITOR環境變量又設置為vi，那麼就可以用vi來編輯crontab文件:
```
$ crontab -e
```

可以像使用vi編輯其他任何文件那樣修改crontab文件並退出。如果修改了某些條目或添加了新的條目，那麼在保存該文件時， cron會對其進行必要的完整性檢查。如果其中的某個域出現了超出允許範圍的值，它會提示你。 我們在編輯crontab文件時，沒準會加入新的條目。例如，加入下面的一條：
```
# DT:delete core files,at 3.30am on 1,7,14,21,26,26 days of each month
30 3 1,7,14,21,26 * * /bin/find -name 'core' -exec rm {} \;
```
保存並退出。

註解

最好在crontab文件的每一個條目之上加入一條註釋，這樣就可以知道它的功能、運行時間，更為重要的是，知道這是哪位用戶的定時作業。
### 刪除crontab文件
```
$crontab -r
```

### 19.5. 使用實例
### 實例1：每1分鐘執行一次myCommand
```
* * * * * myCommand
```

### 實例2：每小時的第3和第15分鐘執行
```
3,15 * * * * myCommand
```
### 實例3：在上午8點到11點的第3和第15分鐘執行
```
3,15 8-11 * * * myCommand
```
### 實例4：每隔兩天的上午8點到11點的第3和第15分鐘執行
```
3,15 8-11 */2  *  * myCommand
```
### 實例5：每週一上午8點到11點的第3和第15分鐘執行
```
3,15 8-11 * * 1 myCommand
```
### 實例 6：每晚的21:30重啟smb
```
30 21 * * * /etc/init.d/smb restart
```
### 實例7：每月1、10、22日的4 : 45重啟smb
```
45 4 1,10,22 * * /etc/init.d/smb restart
```
### 實例8：每週六、週日的1 : 10重啟smb
```
10 1 * * 6,0 /etc/init.d/smb restart
```
### 實例9：每天18 : 00至23 : 00之間每隔30分鐘重啟smb
```
0,30 18-23 * * * /etc/init.d/smb restart
```
### 實例10：每星期六的晚上11 : 00 pm重啟smb
```
0 23 * * 6 /etc/init.d/smb restart
```
### 實例11：每一小時重啟smb
```
* */1 * * * /etc/init.d/smb restart
```
### 實例12：晚上11點到早上7點之間，每隔一小時重啟smb
```
* 23-7/1 * * * /etc/init.d/smb restart
```
### 19.6. 使用注意事項
### 注意環境變量問題
有時我們創建了一個crontab，但是這個任務卻無法自動執行，而手動執行這個任務卻沒有問題，這種情況一般是由於在crontab文件中沒有配置環境變量引起的。

在crontab文件中定義多個調度任務時，需要特別注環境變量的設置，因為我們手動執行某個任務時，是在當前shell環境下進行的，程序當然能找到環境變量，而系統自動執行任務調度時，是不會加載任何環境變量的，因此，就需要在crontab文件中指定任務運行所需的所有環境變量，這樣，系統執行任務調度時就沒有問題了。

不要假定cron知道所需要的特殊環境，它其實並不知道。所以你要保證在shelll腳本中提供所有必要的路徑和環境變量，除了一些自動設置的全局變量。所以注意如下3點：

1. 腳本中涉及文件路徑時寫全局路徑；
2. 腳本執行要用到java或其他環境變量時，通過source命令引入環境變量，如:
```
cat start_cbp.sh
!/bin/sh
source /etc/profile
export RUN_CONF=/home/d139/conf/platform/cbp/cbp_jboss.conf
/usr/local/jboss-4.0.5/bin/run.sh -c mev &
```
3. 當手動執行腳本OK，但是crontab死活不執行時,很可能是環境變量惹的禍，可嘗試在crontab中直接引入環境變量解決問題。如:
```
0 * * * * . /etc/profile;/bin/sh /var/www/java/audit_no_count/bin/restart_audit.sh
```
### 注意清理系統用戶的郵件日誌
每條任務調度執行完畢，系統都會將任務輸出信息通過電子郵件的形式發送給當前系統用戶，這樣日積月累，日誌信息會非常大，可能會影響系統的正常運行，因此，將每條任務進行重定向處理非常重要。 例如，可以在crontab文件中設置如下形式，忽略日誌輸出:
```
0 */3 * * * /usr/local/apache2/apachectl restart >/dev/null 2>&1
```
“/dev/null 2>&1”表示先將標準輸出重定向到/dev/null，然後將標準錯誤重定向到標準輸出，由於標準輸出已經重定向到了/dev/null，因此標準錯誤也會重定向到/dev/null，這樣日誌輸出問題就解決了。

### 系統級任務調度與用戶級任務調度
系統級任務調度主要完成系統的一些維護操作，用戶級任務調度主要完成用戶自定義的一些任務，可以將用戶級任務調度放到系統級任務調度來完成（不建議這麼做），但是反過來卻不行，root用戶的任務調度操作可以通過”crontab –uroot –e”來設置，也可以將調度任務直接寫入/etc/crontab文件，需要注意的是，如果要定義一個定時重啟系統的任務，就必須將任務放到/etc/crontab文件，即使在root用戶下創建一個定時重啟系統的任務也是無效的。

### 其他注意事項
新創建的cron job，不會馬上執行，至少要過2分鐘才執行。如果重啟cron則馬上執行。

當crontab失效時，可以嘗試/etc/init.d/crond restart解決問題。或者查看日誌看某個job有沒有執行/報錯tail -f /var/log/cron。

千萬別亂運行crontab -r。它從Crontab目錄（/var/spool/cron）中刪除用戶的Crontab文件。刪除了該用戶的所有crontab都沒了。

在crontab中%是有特殊含義的，表示換行的意思。如果要用的話必須進行轉義%，如經常用的date ‘+%Y%m%d’在crontab裡是不會執行的，應該換成date ‘+%Y%m%d’。

更新系統時間時區後需要重啟cron,在ubuntu中服務名為cron:
```
$service cron restart
```
ubuntu下啟動、停止與重啟cron:
```
$sudo /etc/init.d/cron start
$sudo /etc/init.d/cron stop
$sudo /etc/init.d/cron restart
```
