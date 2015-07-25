# 磁盤管理


目錄

磁盤管理
查看磁盤空間
打包/ 壓縮
解包/解壓縮
總結
日程磁盤管理中，我們最常用的有查看當前磁盤使用情況，查看當前目錄所佔大小，以及打包壓縮與解壓縮；

4.1. 查看磁盤空間
查看磁盤空間利用大小:

```
df -h
```

-h: human縮寫，以易讀的方式顯示結果（即帶單位：比如M/G，如果不加這個參數，顯示的數字以B為單位）

```
$df -h
/opt/app/todeav/config#df -h
Filesystem            Size  Used Avail Use% Mounted on
/dev/mapper/VolGroup00-LogVol00
2.0G  711M  1.2G  38% /
/dev/mapper/vg1-lv2    20G  3.8G   15G  21% /opt/applog
/dev/mapper/vg1-lv1    20G   13G  5.6G  70% /opt/app
```

查看當前目錄所佔空間大小:

```
du -sh
```

-h 人性化顯示
-s 遞歸整個目錄的大小
```
$du -sh
653M
```

查看當前目錄下所有子文件夾排序後的大小:

```
for i in `ls`; do du -sh $i; done | sort
或者：
du -sh `ls` | sort
```

4.2. 打包/ 壓縮
在linux中打包和壓縮和分兩步來實現的；

打包

打包是將多個文件歸併到一個文件:

```
tar -cvf etc.tar /etc <==僅打包，不壓縮！
```

-c :打包選項
-v :顯示打包進度
-f :使用檔案文件
注：有的系統中指定參數時不需要在前面加上-，直接使用tar xvf

壓縮

```
$gzip demo.txt
```

生成 demo.txt.gz

4.3. 解包/解壓縮
解包

```
tar -xvf demo.tar
```

-x 解包選項

解壓後綴為 .tar.gz的文件 1. 先解壓縮，生成**.tar:

```
$gunzip    demo.tar.gz
```

解包:

```
$tar -xvf  demo.tar
$bzip2 -d demo.tar.bz2
```

bz2解壓:

```
tar jxvf demo.tar.bz2
```

如果tar 不支持j，則同樣需要分兩步來解包解壓縮，使用bzip2來解壓，再使用tar解包:

```
bzip2 -d  demo.tar.bz2
tar -xvf  demo.tar
```

-d decompose,解壓縮

tar解壓參數說明：

-z 解壓gz文件
-j 解壓bz2文件
-J 解壓xz文件
4.4. 總結
查看磁盤空間 df -h

查看目錄大小 du -sh

打包 tar -cvf

解包 tar -xvf

壓縮 gzip

解壓縮 gunzip bzip
