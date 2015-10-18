# Useful Linux Commands


蒐集一些好用卻又常忘記的linux指令


##linux command
<table border="1">
<tbody><tr><td>df -h</td><td>看硬碟所剩容量</td></tr>
<tr><td>du -h</td><td>看目錄所佔硬碟空間大小</td></tr>
<tr><td>ls -lA | grep ^d | awk '{print $8}' | xargs du -s | sort -n</td><td>看看第一層的目錄所佔大小(單位為K)並由小到大sort,注意ls的欄位,此處dir_name在第8,所以取$8,grep ^d是指取list後開頭為d的~也就是目錄</td></tr>
<tr><td>find ./ -type f | wc -l</td><td>看目錄中有多少檔案(含子目錄)</td></tr>
<tr><td>find ./ -type f -name *.jpg -size 0 | wc -l</td><td>同上,加上檔名條件,size條件</td></tr>
<tr><td>find ./ -name "*.txt" -exec grep -H "main" {} ;</td><td>同上,加上-exec執行grep,找有"main"的檔案. 分號前面有反斜線,斜線前面要有空格. find找到的檔名會帶入{}處,給grep去用,-H是印出檔名的意思. 給grep的參數到";"處為止.</td></tr>
<tr><td>find . -name '*.php' -not -path "./*/*"| xargs wc -l</td><td>計算檔案有多少行,path後面參數可以排除一些不算,不然會包含子資料夾</td></tr>
<tr><td>find -type f -size 0 -delete</td><td>加上-delete可把特定條件檔案刪掉!</td></tr>
<tr><td>grep -H "main" file.txt</td><td>找檔案內容含有main的地方</td></tr>
<tr><td>ls -R */*.mat | wc -l</td><td>可以看幾層資料以下的檔案,有幾個,缺點是,必須知道是"幾層"資料夾</td></tr>
<tr><td>rm -r */*.mat</td><td>可以刪除幾層資料夾以下的特定檔案</td></tr>
<tr><td>screen -d -r</td><td>叫回之前在別的登入用的視窗</td></tr>
<tr><td>screen -ls或screen -list</td><td>列出目前screen session</td></tr>
<tr><td>screen -S session_name -X kill或quit</td><td>關掉指定的session, 或是直接到上面list出的session放置處刪除session file</td></tr>
<tr><td>ln -s 被連結檔名 捷徑檔名</td><td>生成一個symbolic link</td></tr>
<tr><td>cat file1 file2 &gt; file3</td><td>把file1跟file2的內容接起來,存到file3</td></tr>
<tr><td>scp source destination</td><td>用ssh copy檔案 source and dest could be user@host:~/xxx/ or user@host:/tmp2/xxx/</td></tr>
<tr><td>sftp user@host</td><td>secure ftp連線, 連上後put,上傳get下載,lls,local端ls</td></tr>
<tr><td>wget -O local_path remote_http_path</td><td>從remote_http_path下載檔案存到local_path</td></tr>
<tr><td>wget -c path</td><td>支援續傳(第一次下載跟續傳下載都要加-c)</td></tr>
<tr><td>nautilus .</td><td>檔案總管(搭配Xming使用)</td></tr>
<tr><td>dpkg-deb -x [filename.deb] [target folder]</td><td>得到compile好的deb package</td></tr>
<tr><td>dpkg -l</td><td>列出機器上安裝的package</td></tr>
<tr><td>chsh</td><td>改變預設shell,可以是/bin/bash or /bin/tcsh</td></tr>
<tr><td>bind '"e[A":history-search-backward' <br> bind '"e[B":history-search-forward'</td><td>在.bashrc中加入, 可用上下鍵搜尋開頭字相同的歷史指令</td></tr>
<tr><td>bindkey -k up history-search-backward <br> bindkey -k down history-search-forward</td><td>在.cshrc中加入,可以上下鍵搜尋開頭字相同的歷史指令</td></tr>
<tr><td>vim -X</td><td>不要搜尋X server,啟動vim速度較快</td></tr>
<tr><td></td><td></td></tr>
</tbody></table>


##vim command

<table border="1">
<tbody><tr><td>Ctrl+W+W</td><td>切換視窗</td></tr>
<tr><td>:split 檔案名</td><td>開新視窗</td></tr>
<tr><td>/</td><td>搜尋</td></tr>
<tr><td>G</td><td>遊標移到檔案結尾</td></tr>
<tr><td>gg</td><td>遊標移到檔案開頭</td></tr>
<tr><td>:diffsplit</td><td>把視窗切成上下 並比較兩個檔案 遊標會移動 用紅色標出不一樣之處</td></tr>
<tr><td>Ctrl+V</td><td>Column Mode選取</td></tr>
<tr><td>Shift+i</td><td>Column Mode Insert,此時只會顯示改一行,但是按下esc,就會全部都改了!</td></tr>
<tr><td>:%s/abc/def/g</td><td>把abc置換為def</td></tr>
<tr><td>u</td><td>Undo</td></tr>
<tr><td>Ctrl+r</td><td>Redo</td></tr>
<tr><td>set pastetoggle=<f2></f2></td><td>在vimrc中寫這行,之後貼上時,進入insert mode,按下f2便可切換是否要auto-indent</td></tr>
<tr><td>:e ++enc=utf-8 myfile.txt</td><td>更換編碼方式</td></tr>
<tr><td></td><td></td></tr>
</tbody></table>

## screen command

<table border="0">
<tbody><tr><td>Ctrl+a+數字</td><td>切換視窗</td></tr>
<tr><td>Ctrl+a+c</td><td>開新視窗</td></tr>
<tr><td>Ctrl+a+[</td><td>進入copy mode,可往上看log, 按q可離開copy mode</td></tr>
<tr><td>Ctrl+a＋a+command</td><td>兩層screen的視窗的操作</td></tr>
<tr><td></td><td></td></tr>
</tbody></table>


## git command

<table border="1">
<tbody><tr><td>git status</td><td>目前狀況</td></tr>
<tr><td>git add</td><td>加入檔案</td></tr>
<tr><td>git commit</td><td>commit file</td></tr>
<tr><td>git push</td><td>push到遠端</td></tr>
<tr><td>git pull</td><td>從遠端pull</td></tr>
<tr><td>git remote add origin ssh://user@host:22/~/xxx.git</td><td>加入遠端host</td></tr>
<tr><td>git init</td><td>建立local端空repository</td></tr>
<tr><td>git init --bare</td><td>建立遠端空repository</td></tr>
<tr><td>git clone ssh://user@host:22/~/git/xxx.git</td><td>git clone with ssh</td></tr>
<tr><td>git mv a.txt b.txt</td><td>rename file(git will also do mv in fs)</td></tr>
<tr><td>git mergetool</td><td>開啟內建mergetool</td></tr>
<tr><td>git rm --cached filename</td><td>remove file from git without delete it</td></tr>
<tr><td>git submodule init (enter) git submodule update</td><td>clone含有submodule的project時使用這兩個command, submodule相關可參考<a href="http://blog.wu-boy.com/2011/09/introduction-to-git-submodule/">這裡</a></td></tr>
<tr><td>git config core.sharedRepository group</td><td>在remote設定這項,之後push可確保group writable</td></tr>
<tr><td><pre>~/project $ git init
~/project $ git add .
~/project $ git commit -m "first commit"
~/project $ cd ~/Dropbox/git

~/Dropbox/git $ git init --bare project.git
~/Dropbox/git $ cd ~/project

~/project $ git remote add origin ~/Dropbox/git/project.git
~/project $ git push -u origin master</pre>
</td><td>git local tips</td></tr>
<tr><td></td><td></td></tr>
</tbody></table>

## ssh免密碼方法

<table border="0"><tbody><tr><td><pre>在工作站執行一次下面兩行指令（$代表提示字元, [ENTER]代表按enter鍵）
$ ssh-keygen -t rsa [ENTER]
Enter file in which to save the key (/home/ta/.ssh/id_rsa): [ENTER]
Enter passphrase (empty for no passphrase): [ENTER]
Enter same passphrase again: [ENTER]
$ cat .ssh/id_rsa.pub &gt; .ssh/authorized_keys
</pre></td></tr></tbody></table>


## 壓縮 解壓縮
<table border="1">
<tbody><tr><td>解壓縮 tar.gz</td><td>tar -zxvf filename.tar.gz</td></tr>
<tr><td>解壓縮 tar.gz裡的檔案xxx/abc.jpg 放到指定資料夾cde</td><td>tar zxvf filename.tar.gz -C cde/ xxx/abc.jpg --strip-components=1 (意思是去掉"xxx/abc.jpg"之前"1"個資料夾的)</td></tr>
<tr><td>壓縮 tar.gz</td><td>tar -zcvf filename.tar.gz /folder</td></tr>
<tr><td>解壓縮 tar.bz2</td><td>tar -jxvf filename.tar.bz2</td></tr>
<tr><td>壓縮 tar.bz2</td><td>tar -jcvf filename.tar.bz2 /folder</td></tr>
<tr><td>解開 .tar </td><td>tar -xvf filename.tar</td></tr>
<tr><td>打包 .tar </td><td>tar -cvf filename.tar /folder</td></tr>
<tr><td>解壓縮 gz</td><td>gunzip filename.gz (解完gz檔會消失)</td></tr>
<tr><td>壓縮 gz</td><td>gzip filename.xxx (只能壓單檔,壓完原始檔會消失)</td></tr>
<tr><td>解壓縮 zip</td><td>unzip filename.zip</td></tr>
<tr><td>壓縮 zip</td><td>zip filename.xxx /folder</td></tr>
<tr><td>解壓縮 rar</td><td>unrar x filename.rar</td></tr>
<tr><td>壓縮 7z</td><td>7za a filename.7z /folder/</td></tr>
<tr><td>解壓縮 7z</td><td>7za x filename.7z </td></tr>
<tr><td>壓縮 rar</td><td>rar a filename.rar folder/</td></tr>
<tr><td>解壓縮 rar,並放到target_folder</td><td>rar x filename.rar target_folder/</td></tr>
<tr><td>解壓縮 rar,不管裡面路徑,並放到target_folder</td><td>rar e filename.rar target_folder/</td></tr>
<tr><td>解壓縮 tar.xz</td><td>xz -d filename.tar.xz </td></tr>
<tr><td></td><td></td></tr>
</tbody></table>


## 自動更新ip位置

<table border="0">
<tbody><tr><td>dhclient -r</td><td>釋放IP</td></tr>
<tr><td>dhclient </td><td>更新IP</td></tr>
<tr><td>noip2</td><td>no-ip的update程式</td></tr>
</tbody></table>

## Dropbox使用注意事項

<table border="0">
<tbody><tr><td>~/.dropbox-dist/dropboxd</td><td>dropbox服務程式</td></tr>
<tr><td>~/bin/dropbox</td><td>dropbox控制interface,可看dropbox help有指令說明</td></tr>
<tr><td>目前裝在cml10</td><td>一定要dropbox執行的機器改檔案才會sync</td></tr>
<tr><td>dropbox_ch_LANG</td><td>要把語言設好 不然執行會error</td></tr>
<tr><td>dropbox exclude add</td><td>加ignore目錄,似乎要給絕對路徑才會成功</td></tr>
<tr><td>chmod o+rx ~/Dropbox</td><td>Dropbox預設權限是700,若要link到apache,要開權限</td></tr>
</tbody></table>

## google drive sync (use grive)

<table border="0">
<tbody><tr><td>grive -a</td><td>在要sync的資料夾中執行此行, 加入帳號</td></tr>
<tr><td>grive</td><td>在要sync的資料夾中執行此行,就開始sync,會跑很久</td></tr>
</tbody></table>
