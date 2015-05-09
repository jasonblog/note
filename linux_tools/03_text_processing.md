# 文本處理

目錄

文本處理
文件查找
定製搜索
找到後的後續動作
-print的定界符
grep 文本搜索
xargs 命令行參數轉換
sort 排序
uniq 消除重複行
用tr進行轉換
cut 按列切分文本
paste 按列拼接文本
wc 統計行和字符的工具
sed 文本替換利器
awk 數據流處理工具
print 打印當前行
特殊變量： NR NF $0 $1 $2
傳遞外部變量
用樣式對awk處理的行進行過濾
設置定界符
讀取命令輸出
在awk中使用循環
awk實現head、tail命令
打印指定列
打印指定文本區域
awk常用內建函數
迭代文件中的行、單詞和字符
1. 迭代文件中的每一行
2.迭代一行中的每一個單詞
3. 迭代每一個字符
本節將介紹Linux下使用Shell處理文本時最常用的工具： find、grep、xargs、sort、uniq、tr、cut、paste、wc、sed、awk； 提供的例子和參數都是常用的； 我對shell腳本使用的原則是命令單行書寫，儘量不要超過2行； 如果有更爲複雜的任務需求，還是考慮python吧；

3.1. 文件查找
查找txt和pdf文件:

```
find . \( -name "*.txt" -o -name "*.pdf" \) -print
```

正則方式查找.txt和pdf:

```
find . -regex  ".*\(\.txt|\.pdf\)$"
```

-iregex： 忽略大小寫的正則

否定參數 ,查找所有非txt文本:

```
find . ! -name "*.txt" -print
```

指定搜索深度,打印出當前目錄的文件（深度爲1）:
```
find . -maxdepth 1 -type f
```

定製搜索
按類型搜索
```
find . -type d -print  //只列出所有目錄
```

-type f 文件 / l 符號鏈接

按時間搜索
-atime 訪問時間 (單位是天，分鐘單位則是-amin，以下類似）
-mtime 修改時間 （內容被修改）
-ctime 變化時間 （元數據或權限變化）
最近第7天被訪問過的所有文件:

```
find . -atime 7 -type f -print
```

最近7天內被訪問過的所有文件:

```
find . -atime -7 -type f -print
```

查詢7天前被訪問過的所有文件:

```
find . -atime +7 type f -print
```

按大小搜索：
w字 k M G 尋找大於2k的文件:

```
find . -type f -size +2k
```

按權限查找:

```
find . -type f -perm 644 -print //找具有可執行權限的所有文件
```

按用戶查找:

```
find . -type f -user weber -print// 找用戶weber所擁有的文件
```

找到後的後續動作
刪除
刪除當前目錄下所有的swp文件:

```
find . -type f -name "*.swp" -delete
```

另一種語法:

```
find . type f -name "*.swp" | xargs rm
```

執行動作（強大的exec）
將當前目錄下的所有權變更爲weber:

```
find . -type f -user root -exec chown weber {} \;
```

注：{}是一個特殊的字符串，對於每一個匹配的文件，{}會被替換成相應的文件名；

將找到的文件全都copy到另一個目錄:

```
find . -type f -mtime +10 -name "*.txt" -exec cp {} OLD \;
```

結合多個命令
如果需要後續執行多個命令，可以將多個命令寫成一個腳本。然後 -exec 調用時執行腳本即可:

-exec ./commands.sh {} \;
-print的定界符
默認使用’\n’作爲文件的定界符；

-print0 使用’\0’作爲文件的定界符，這樣就可以搜索包含空格的文件；

3.2. grep 文本搜索
```
grep match_patten file // 默認訪問匹配行
```

常用參數

-o 只輸出匹配的文本行 VS -v 只輸出沒有匹配的文本行
-c 統計文件中包含文本的次數
grep -c “text” filename

-n 打印匹配的行號
-i 搜索時忽略大小寫
-l 只打印文件名
在多級目錄中對文本遞歸搜索(程序員搜代碼的最愛）:

```
grep "class" . -R -n
```

匹配多個模式:

```
grep -e "class" -e "vitural" file
```

grep輸出以0作爲結尾符的文件名（-z）:

```
grep "test" file* -lZ| xargs -0 rm
```

綜合應用：將日誌中的所有帶where條件的sql查找查找出來:

```
cat LOG.* | tr a-z A-Z | grep "FROM " | grep "WHERE" > b
```

3.3. xargs 命令行參數轉換
xargs 能夠將輸入數據轉化爲特定命令的命令行參數；這樣，可以配合很多命令來組合使用。比如grep，比如find； - 將多行輸出轉化爲單行輸出

```
cat file.txt| xargs
```

n 是多行文本間的定界符

將單行轉化爲多行輸出
```
cat single.txt | xargs -n 3
```

-n：指定每行顯示的字段數

xargs參數說明

-d 定義定界符 （默認爲空格 多行的定界符爲 n）
-n 指定輸出爲多行
-I {} 指定替換字符串，這個字符串在xargs擴展時會被替換掉,用於待執行的命令需要多個參數時
-0：指定0爲輸入定界符
示例:

```
cat file.txt | xargs -I {} ./command.sh -p {} -1

#統計程序行數
find source_dir/ -type f -name "*.cpp" -print0 |xargs -0 wc -l
```

3.4. sort 排序
字段說明

-n 按數字進行排序 VS -d 按字典序進行排序
-r 逆序排序
-k N 指定按第N列排序
示例:

```
sort -nrk 1 data.txt
sort -bd data // 忽略像空格之類的前導空白字符
```

3.5. uniq 消除重複行
消除重複行
```
sort unsort.txt | uniq
```

統計各行在文件中出現的次數
```
sort unsort.txt | uniq -c
```

找出重複行
```
sort unsort.txt | uniq -d
```

可指定每行中需要比較的重複內容：-s 開始位置 -w 比較字符數

3.6. 用tr進行轉換
通用用法
```
echo 12345 | tr '0-9' '9876543210' //加解密轉換，替換對應字符
cat text| tr '\t' ' '  //製表符轉空格
```

tr刪除字符
```
cat file | tr -d '0-9' // 刪除所有數字
```

-c 求補集

```
cat file | tr -c '0-9' //獲取文件中所有數字
cat file | tr -d -c '0-9 \n'  //刪除非數字數據
```

tr壓縮字符
tr -s 壓縮文本中出現的重複字符；最常用於壓縮多餘的空格:

```
cat file | tr -s ' '
```

字符類
tr中可用各種字符類：
alnum：字母和數字
alpha：字母
digit：數字
space：空白字符
lower：小寫
upper：大寫
cntrl：控制（非可打印）字符
print：可打印字符
使用方法：tr [:class:] [:class:]

```
tr '[:lower:]' '[:upper:]'
```

3.7. cut 按列切分文本
截取文件的第2列和第4列
```
cut -f2,4 filename
```

去文件除第3列的所有列
```
cut -f3 --complement filename
```

-d 指定定界符
```
cat -f2 -d";" filename
```

cut 取的範圍
N- 第N個字段到結尾
-M 第1個字段爲M
N-M N到M個字段
cut 取的單位
-b 以字節爲單位
-c 以字符爲單位
-f 以字段爲單位（使用定界符）
示例:

```
cut -c1-5 file //打印第一到5個字符
cut -c-2 file  //打印前2個字符
```

截取文本的第5到第7列

```
$echo string | cut -c5-7
```

3.8. paste 按列拼接文本
將兩個文本按列拼接到一起;

```
cat file1
1
2

cat file2
colin
book

paste file1 file2
1 colin
2 book
```

默認的定界符是製表符，可以用-d指明定界符:

```
paste file1 file2 -d ","
1,colin
2,book
```

3.9. wc 統計行和字符的工具
```
$wc -l file // 統計行數

$wc -w file // 統計單詞數

$wc -c file // 統計字符數
```

3.10. sed 文本替換利器
首處替換
```
sed 's/text/replace_text/' file   //替換每一行的第一處匹配的text
```

全局替換
```
sed 's/text/replace_text/g' file
```

默認替換後，輸出替換後的內容，如果需要直接替換原文件,使用-i:

```
sed -i 's/text/repalce_text/g' file
```

移除空白行
```
sed '/^$/d' file
```

變量轉換
已匹配的字符串通過標記&來引用.

```
echo this is en example | sed 's/\w+/[&]/g'
$>[this]  [is] [en] [example]
```

子串匹配標記
第一個匹配的括號內容使用標記 1 來引用

```
sed 's/hello\([0-9]\)/\1/'
```

雙引號求值
sed通常用單引號來引用；也可使用雙引號，使用雙引號後，雙引號會對表達式求值:

```
sed 's/$var/HLLOE/'
```

當使用雙引號時，我們可以在sed樣式和替換字符串中指定變量；

```
eg:
p=patten
r=replaced
echo "line con a patten" | sed "s/$p/$r/g"
$>line con a replaced
```

其它示例
字符串插入字符：將文本中每行內容（ABCDEF） 轉換爲 ABC/DEF:

```
sed 's/^.\{3\}/&\//g' file
```

3.11. awk 數據流處理工具
awk腳本結構
```
awk ' BEGIN{ statements } statements2 END{ statements } '
```

工作方式
1.執行begin中語句塊；

2.從文件或stdin中讀入一行，然後執行statements2，重複這個過程，直到文件全部被讀取完畢；

3.執行end語句塊；

print 打印當前行
使用不帶參數的print時，會打印當前行
```
echo -e "line1\nline2" | awk 'BEGIN{print "start"} {print } END{ print "End" }'
```

print 以逗號分割時，參數以空格定界;
```
echo | awk ' {var1 = "v1" ; var2 = "V2"; var3="v3"; \
print var1, var2 , var3; }'
$>v1 V2 v3
```

使用-拼接符的方式（”“作爲拼接符）;
```
echo | awk ' {var1 = "v1" ; var2 = "V2"; var3="v3"; \
print var1"-"var2"-"var3; }'
$>v1-V2-v3
```

特殊變量： NR NF $0 $1 $2
NR:表示記錄數量，在執行過程中對應當前行號；

NF:表示字段數量，在執行過程總對應當前行的字段數；

$0:這個變量包含執行過程中當前行的文本內容；

$1:第一個字段的文本內容；

$2:第二個字段的文本內容；

```
echo -e "line1 f2 f3\n line2 \n line 3" | awk '{print NR":"$0"-"$1"-"$2}'
```

打印每一行的第二和第三個字段
```
awk '{print $2, $3}' file
```

統計文件的行數
```
awk ' END {print NR}' file
```

累加每一行的第一個字段
```
echo -e "1\n 2\n 3\n 4\n" | awk 'BEGIN{num = 0 ;
print "begin";} {sum += $1;} END {print "=="; print sum }'
```

傳遞外部變量
```
var=1000
echo | awk '{print vara}' vara=$var #  輸入來自stdin
awk '{print vara}' vara=$var file # 輸入來自文件
```

用樣式對awk處理的行進行過濾
```
awk 'NR < 5' #行號小於5
awk 'NR==1,NR==4 {print}' file #行號等於1和4的打印出來
awk '/linux/' #包含linux文本的行（可以用正則表達式來指定，超級強大）
awk '!/linux/' #不包含linux文本的行
```


設置定界符
使用-F來設置定界符（默認爲空格）:

```
awk -F: '{print $NF}' /etc/passwd
```

讀取命令輸出
使用getline，將外部shell命令的輸出讀入到變量cmdout中:

```
echo | awk '{"grep root /etc/passwd" | getline cmdout; print cmdout }'
```

在awk中使用循環
```
for(i=0;i<10;i++){print $i;}
for(i in array){print array[i];}
```

以逆序的形式打印行：(tac命令的實現）:

```
seq 9| \
awk '{lifo[NR] = $0; lno=NR} \
END{ for(;lno>-1;lno--){print lifo[lno];}
} '
```

awk實現head、tail命令
head
```
awk 'NR<=10{print}' filename
```

tail
```
awk '{buffer[NR%10] = $0;} END{for(i=0;i<11;i++){ \
print buffer[i %10]} } ' filename
```

打印指定列
awk方式實現
```
ls -lrt | awk '{print $6}'
```

cut方式實現
```
ls -lrt | cut -f6
```

打印指定文本區域
確定行號
```
seq 100| awk 'NR==4,NR==6{print}'
```

確定文本
打印處於start_pattern 和end_pattern之間的文本:

```
awk '/start_pattern/, /end_pattern/' filename
```

示例:

```
seq 100 | awk '/13/,/15/'
cat /etc/passwd| awk '/mai.*mail/,/news.*news/'
```

awk常用內建函數
index(string,search_string):返回search_string在string中出現的位置

sub(regex,replacement_str,string):將正則匹配到的第一處內容替換爲replacement_str;

match(regex,string):檢查正則表達式是否能夠匹配字符串；

length(string)：返回字符串長度

```
echo | awk '{"grep root /etc/passwd" | getline cmdout; print length(cmdout) }'
```

printf 類似c語言中的printf，對輸出進行格式化:

```
seq 10 | awk '{printf "->%4s\n", $1}'
```

3.12. 迭代文件中的行、單詞和字符
1. 迭代文件中的每一行
```
while 循環法
while read line;
do
echo $line;
done < file.txt
```


改成子shell:
```
cat file.txt | (while read line;do echo $line;done)
```

awk法
```
cat file.txt| awk '{print}'
```

2.迭代一行中的每一個單詞
```
for word in $line;
do
echo $word;
done
```

3. 迭代每一個字符
${string:start_pos:num_of_chars}：從字符串中提取一個字符；(bash文本切片）

${#word}:返回變量word的長度

```
for((i=0;i<${#word};i++))
do
echo ${word:i:1);
done
```

以ASCII字符顯示文件:

```
$od -c filename```

