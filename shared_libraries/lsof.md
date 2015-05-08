# lsof

這 10 條 Linux 命令依次是：

pgrep：比如，你可以使用 pgrep -u root 來代替 ps -ef | egrep 『^root 『 | awk 『{print $2}』，以便抓取屬於 root 的 PID。

pstree：我覺得這個命令很酷，它可以直接列出進程樹，或者換句話說是按照樹狀結構來列出進程。

bc：這個命令在我的系統中沒有找到，可能需要安裝。這是用來執行計算的一個命令，如使用它來開平方根。

split：這是一個很有用的命令，它可以將一個大文件分割成幾個小的部分。比如：split -b 2m largefile LF_ 會將 largefile 分割成帶有 LF 文件名前綴且大小為 2 MB 的小文件。

nl：能夠顯示行號的命令。在閱讀腳本或代碼時，這個命令應該非常有用。如：nl wireless.h | head。
mkfifo：作者說這是他最喜歡的命令。該命令使得其他命令能夠通過一個命名的管道進行通信。嗯，聽起來有點空洞。舉例說明，先創建一個管道並寫入內容： mkfifo ive-been-piped ls -al split/* | head > ive-been-piped

然後就可以讀取了：head ive-been-piped。

ldd：其作用是輸出指定文件依賴的動態鏈接庫。比如，通過 ldd /usr/java/jre1.5.0_11/bin/java 可以瞭解哪些線程庫鏈接到了 java 依賴（動態鏈接）了哪些庫。（感謝 NetSnail 的指正。）

col：可以將 man 手冊頁保存為無格式的文本文件。如： PAGER=cat man less | col -b > less.txt

xmlwf：能夠檢測 XML 文檔是否良好。比如： curl -s 『http://bashcurescancer.com' > bcc.html xmlwf bcc.html perl -i -pe 『s@
@
@g' bcc.html xmlwf bcc.html bcc.html:104:2: mismatched tag

lsof：列出打開的文件。如：通過 lsof | grep TCP 可以找到打開的端口。




關於lsof，網管員應該瞭解以下這個用法：
# 顯示當前SSH的連接用戶和源IP地址

$ sudo lsof -n | grep sshd | grep TCP | cut -c18-28,70-
root  TCP *:22 (LISTEN)
root  TCP *:22 (LISTEN)
root  TCP 1.2.3.4:22->6.7.8.9:2544 (ESTABLISHED)
huangwei  TCP 1.2.3.4:22->6.7.8.9:2544 (ESTABLISHED)
root  TCP 1.2.3.4:22->6.7.8.9:29340 (ESTABLISHED)
huangwei  TCP 1.2.3.4:22->6.7.8.9:29340 (ESTABLISHED)
root  TCP 1.2.3.4:22->6.7.8.9:33223 (ESTABLISHED)
huangwei  TCP 1.2.3.4:22->6.7.8.9:33223 (ESTABLISHED)
huangwei  TCP [::1]:cisco-sccp (LISTEN)
huangwei  TCP 127.0.0.1:cisco-sccp (LISTEN)
huangwei  TCP 1.2.3.4:40183->74.125.227.8:https (ESTABLISHED)
root  TCP 1.2.3.4:22->6.7.8.9:43698 (ESTABLISHED)
huangwei  TCP 1.2.3.4:22->6.7.8.9:43698 (ESTABLISHED)
root  TCP 1.2.3.4:22->6.7.8.9:44943 (ESTABLISHED)
huangwei  TCP 1.2.3.4:22->6.7.8.9:44943 (ESTABLISHED)
huangwei  TCP 1.2.3.4:38038->74.125.227.20:www (ESTABLISHED)
看看是誰在大量並發連接呢？

DDoS？網站管理員的噩夢！一條」簡單」命令就能找出script kids？看過來：

$ sudo netstat -anp |grep 'tcp\|udp' | awk '{print $5}' | cut -d: -f1 | sort | uniq -c | sort -nr
      8 192.168.0.218
      7 192.168.0.38
      6 192.168.0.14
      6 0.0.0.0
      2 192.168.0.166
      2 192.168.0.110
      2 192.168.0.10
      1 192.168.232.223
      1 192.168.0.70
      1 192.168.0.6
      1 192.168.0.50
      1 192.168.0.22
      1 192.168.0.210
      1 192.168.0.194
看看現在服務器打開了多少端口？看過來：

$ sudo lsof -i
COMMAND    PID     USER   FD   TYPE  DEVICE SIZE/OFF NODE NAME
mysqld    1190    mysql   10u  IPv4    5022      0t0  TCP localhost:mysql (LISTEN)
apache2   1347     root    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
svnserve  1759      svn    3u  IPv4    6612      0t0  TCP ooxx-vpn:svn (LISTEN)
sshd      2583     root    3r  IPv4 1194924      0t0  TCP 192.168.1.176:8822->192.168.2.223:40876 (ESTABLISHED)
sshd      2651 huangwei    3u  IPv4 1194924      0t0  TCP 192.168.1.176:8822->192.168.2.223:40876 (ESTABLISHED)
apache2   2714 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2715 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2722 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2722 www-data   11u  IPv4 1198941      0t0  TCP ooxx-vpn:www->192.168.0.50:4068 (ESTABLISHED)
apache2   2723 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2725 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2725 www-data   11u  IPv4 1198939      0t0  TCP ooxx-vpn:www->192.168.0.194:15397 (ESTABLISHED)
apache2   2734 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2809 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2809 www-data   11u  IPv4 1198940      0t0  TCP ooxx-vpn:www->192.168.0.218:1521 (ESTABLISHED)
apache2   2810 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2811 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2818 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2819 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2845 www-data    3u  IPv4    5327      0t0  TCP *:www (LISTEN)
apache2   2845 www-data   11u  IPv4 1198938      0t0  TCP ooxx-vpn:www->192.168.0.14:36802 (ESTABLISHED)
proftpd   7191      ftp    0u  IPv4   29954      0t0  TCP ooxx-vpn:ftp (LISTEN)
sshd      9720     root    3u  IPv4   47070      0t0  TCP *:8822 (LISTEN)
sshd      9720     root    4u  IPv6   47072      0t0  TCP *:8822 (LISTEN)
svnserve 11217      svn    4u  IPv4 1019658      0t0  TCP ooxx-vpn:svn->192.168.0.166:6211 (ESTABLISHED)
svnserve 11350      svn    4u  IPv4 1020389      0t0  TCP ooxx-vpn:svn->192.168.0.166:6286 (ESTABLISHED)
svnserve 12706      svn    4u  IPv4  627093      0t0  TCP ooxx-vpn:svn->192.168.0.22:1084 (ESTABLISHED)
看看本機上所有佔用 TCP 80端口的應用程序

$ sudo lsof -i tcp:80
樣例輸出如下：

COMMAND   PID     USER   FD   TYPE  DEVICE SIZE/OFF NODE NAME
apache2  2827 www-data    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
apache2  2827 www-data   11u  IPv4 2026780      0t0  TCP ooxx-vpn:www->192.168.0.22:14949 (ESTABLISHED)
apache2  2875 www-data    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
apache2  2919 www-data    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
apache2  2920 www-data    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
apache2  2921 www-data    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
apache2  2924 www-data    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
apache2  2926 www-data    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
apache2  2928 www-data    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
apache2  2930 www-data    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
apache2  2932 www-data    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
apache2  2933 www-data    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
apache2 26081     root    3u  IPv4 1609898      0t0  TCP *:www (LISTEN)
