---
title: MySQL 通訊協議
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,通訊協議
description: Client/Server 通訊協議用於客戶端鏈接、代理、主備複製等，支持 SSL、壓縮，在鏈接階段進行認證，在執行命令時可以支持 Prepared Statements 以及 Stored Procedures 。當打算編寫數據庫代理、數據庫中間件、對 MySQL 數據包進行審核時，都需要了解底層的通信協議。在本文中，主要介紹 MySQL 通訊協議相關的內容。
---

Client/Server 通訊協議用於客戶端鏈接、代理、主備複製等，支持 SSL、壓縮，在鏈接階段進行認證，在執行命令時可以支持 Prepared Statements 以及 Stored Procedures 。

當打算編寫數據庫代理、中間件、對 MySQL 數據包進行審核時，都需要了解底層的通信協議。在本文中，主要介紹 MySQL 通訊協議相關的內容。

<!-- more -->

## 簡介

服務器啟動後，會使用 TCP 監聽一個本地端口，當客戶端的連接請求到達時，就會執行三段握手以及 MySQL 的權限驗證；驗證成功後，客戶端開始發送請求，服務器會以響應的報文格式返回數據；當客戶端發送完成後，會發送一個特殊的報文，告知服務器已結束會話。

MySQL 定義了幾種包類型，A) 客戶端->服務器，登錄時的 auth 包、執行 SQL 時的 CMD 包；B) 服務器->客戶端，登錄時的握手包、數據包、數據流結束包、成功包(OK Packet)、錯誤信息包。

協議定義了基本的數據類型，如 int、string 等；數據的傳送格式等。

### 協議

MySQL 的客戶端與服務器之間支持多種通訊方式，最廣泛使用的是 TCP 通訊；另外，還支持命名管道和共享內存，而 TCP 就是最通用的一種方式，在此僅介紹 TCP 方式。

在 C/S 之間，實際採用的是一種類似半雙工式的模式收發數據，即在一個 TCP 鏈路上，客戶端發出請求數據後，只有在接收完所有的服務端響應數據以後才能發下一次請求，中間不能發其它數據，需要有很強的順序性要求。

MySQL 客戶端與服務器的交互主要分為兩個階段，分別為握手認證階段和命令執行階段，詳細來說一次正常的過程如下：

{% highlight text %}
1. 三次握手建立 TCP 連接。

2. 建立 MySQL 連接，也就是認證階段。
    服務端 -> 客戶端：發送握手初始化包 (Handshake Initialization Packet)。
    客戶端 -> 服務端：發送驗證包 (Client Authentication Packet)。
    服務端 -> 客戶端：認證結果消息。

3. 認證通過之後，客戶端開始與服務端之間交互，也就是命令執行階段。
    客戶端 -> 服務端：發送命令包 (Command Packet)。
    服務端 -> 客戶端：發送迴應包 (OK Packet, or Error Packet, or Result Set Packet)。

4. 斷開 MySQL 連接。
    客戶端 -> 服務器：發送退出命令包。

5. 四次握手斷開 TCP 連接。
{% endhighlight %}

從服務器發往客戶端的數據包有四種：數據包、數據結束包、成功報告包以及錯誤消息包。Result Set Packet 首先發送包頭+列包+EOF包+行包+EOF包。

<!--
TCP 連接時採用三次握手連接，主動斷開時同樣採用的是三次握手，而非四次握手；數據的發送過程總會有摻雜著一些 TCP 包，不知道做什麼用的。
-->

### 報文格式

所有的包有統一的格式，並通過函數 my_net_write()@sql/net_serv.cc 寫入 buffer 等待發送。

{% highlight text %}
+-------------------+--------------+---------------------------------------------------+
|      3 Bytes      |    1 Byte    |                   N Bytes                         |
+-------------------+--------------+---------------------------------------------------+
|<= length of msg =>|<= sequence =>|<==================== data =======================>|
|<============= header ===========>|<==================== body =======================>|
{% endhighlight %}

MySQL 報文格式如上，消息頭包含了 A) 報文長度，標記當前請求的實際數據長度，以字節為單位；B) 序號，為了保證交互時報文的順序，每次客戶端發起請求時，序號值都會從 0 開始計算。

消息體用於存放報文的具體數據，長度由消息頭中的長度值決定。

> 單個報文的最大長度為 (2^24-1)Bytes ，也即 (16M-1)Bytes，對於包長為 (2^24-1)Bytes 也會拆為兩個包發送。這是因為最初沒有考慮 16M 的限制，從而沒有預留任何字段來標誌這個包的數據不完整，所以只好把長度為 (2^24-1) 的包視做不完整的包，直到後面收到一個長度小於 (2^24-1) 的包，然後拼起來。
>
> 這也意味著最後一個包的長度有可能是 0。

### 基本類型

接下來介紹一下報文中的數據類型，也就是不同的數據類型在報文中的表現形式。

#### 整型值

MySQL 報文中整型值分別有 1、2、3、4、8 字節長度，使用小字節序傳輸。

#### 二進制數據

也就是 Length Coded Binary，其數據長度不固定，長度值由數據前的 1-9 個字節決定，其中長度值所佔的字節數不定，字節數由第 1 個字節決定，如下：

{% highlight text %}
第一個字節值    後續字節數  長度值說明
    0-250            0   第一個字節值即為數據的真實長度
      251            0   空數據，數據的真實長度為零
      252            2   後續額外2個字節標識了數據的真實長度
      253            3   後續額外3個字節標識了數據的真實長度
      254            8   後續額外8個字節標識了數據的真實長度
{% endhighlight %}

#### 字符串

根據是否以 NULL 結尾，分為了有兩種形式：

* 以 NULL 結尾，Null-Terminated String<br>字符串長度不固定，當遇到 'NULL'(0x00) 字符時結束。

* 長度編碼，Length Coded String<br>字符串長度不固定，無 'NULL'(0x00) 結束符，編碼方式與上面的二進制數據相同。

### 客戶端請求報文

也就是從客戶端發送到服務端的請求命令。

{% highlight text %}
+-------------------+------------------------------------------------------------------+
|      1 Bytes      |                             N Bytes                              |
+-------------------+------------------------------------------------------------------+
|<==== command ====>|<============================ arguments =========================>|
{% endhighlight %}

客戶端向服務端發送的請求，其中第一個字節用於標識當前請求消息的類型，這也就定義了請求的種類，其中包括了：切換數據庫 COM_INIT_DB(0x02)、查詢命令 COM_QUERY(0x03) 等。

命令的宏定義在 include/mysql_com.h 文件中，該命令會在 dispatch_command() 中根據不同的命令進入不同代碼處理邏輯。

報文中的參數內容是用戶在 MySQL 客戶端輸入的命令，不包括每行命令結尾的 ';' 分號，採用的是非 NULL 結尾的字符串表示方法。

例如：當在 MySQL 客戶端中執行 use mysql; 命令時，發送的請求報文數據會是下面的樣子：
{% highlight text %}
0x02 0x6d 0x79 0x73 0x71 0x6c
{% endhighlight %}

其中，0x02 為請求類型值 COM_INIT_DB，後面的 0x6d 0x79 0x73 0x71 0x6c 為 ASCII 字符 mysql 。

### 錯誤碼

也就是當發生了錯誤之後，服務端發送給客戶端的報文。

MySQL 的錯誤包含了三部分：A) MySQL 特定的錯誤碼，數字類型，不通用；B) SQLSTATE，為 5 個字符的字符串，採用 ANSI SQL 和 ODBC 的標準；C) 錯誤信息。

對於錯誤報文的格式可以參照參考文件，其中第二字節表示由 MySQL 定義的錯誤編碼，服務器狀態實際是 ANSI SQL 對應的編碼，兩者並非一一對應。

在 MySQL 中可以通過 perror ERROR 查看；詳細的文檔，可以參考官方文檔 [Appendix B Errors, Error Codes, and Common Problems](http://dev.mysql.com/doc/refman/en/error-handling.html) 。

### 抓包分析

可以通過 tcpdump 捕獲包並保存在文件中，然後通過 Wireshark 打開文件，查看網絡包的內容，相對來說比較方便。可以通過 tcpdump -D 查看支持的網卡接口，通過 -i 指定接口，在此使用 lo。

注意，tcpdump 不能捕獲 unix socket，鏈接時不能使用 -S /tmp/mysql.sock 或者 -h localhost 參數，應當使用 -h 127.1 。

可以將 tcpdump 的包輸出到 stdout 或者通過 -w 保存到文件，然後用 Wireshark 分析。

{% highlight text %}
----- 將抓包的數據保存到文件
# tcpdump -i lo port 3306 -w filename

----- 當然，也可以打印到終端，然後處理數據
# tcpdump -i lo port 3306 -nn -X -q
# tcpdump -i any -s 0 -l -w - dst port 3306 | strings | grep -iE 'select|update'
{% endhighlight %}

<!--tcpflow -c -p -i eth0 dst port 3306 | grep -i -E "select|insert|update|delete|replace" | sed 's%\(.*\)\([.]\{4\}\)\(.*\)%\3%'-->

## 認證協議

認證稍微有點複雜，單獨拉出來。

MySQL 的用戶管理模塊信息存儲在系統表 mysql.user 中，其中包括了授權用戶的基本信息以及一些權限信息。在登陸時，只會用到 host、user、passwd 三個字段，也就是說登陸認證需要 host+user 關聯，當然可以使用 * 通配符。

服務器在收到新的連接請求時，會調用 login_connection() 作身份驗證，先根據 IP 做 ACL 檢查，然後才進入用戶名密碼驗證階段。

其中報文的格式如下。

![auth protocol]({{ site.url }}/images/databases/mysql/auth_protocol_handshake.png "auth protocol"){: .pull-center }

MySQL 認證採用經典的 CHAP 協議，即挑戰握手認證協議，在 native_password_authenticate() 函數的註釋中簡單介紹了該協議的執行過程：

{% highlight text %}
1. the server sends the random scramble to the client.
2. client sends the encrypted password back to the server.
3. the server checks the password.
{% endhighlight %}

random scramble 在 4.1 之前的版本中是 8 字節整數，在 4.1 以及後續版本是 20 字節整數，該值是通過 create_random_string() 函數生成。

根據版本不同，分為了兩類。

<!--
The Server Initialization Packet and the Client Authentication Packet both have an 8-byte field, scramble_buff. The value in this field is used for password authentication.

Relevant MySQL Source Code: libmysql/password.c, see also comments at start of file. It works thus:


[edit] 4.0 and before
    The server sends a random string to the client, in scramble_buff.
    The client encrypts the scramble_buff value using the hash of a password that the user has entered. This happens in sql/password.c:scramble() function.
    The client sends the encrypted scramble_buff value to the server.
    The server encrypts the original random string using a value in the mysql database, mysql.user.Password.
    The server compares its encrypted random string to what the client sent in scramble_buff.
    If they are the same, the password is okay.

In this protocol, snooping on the wire doesn't reveal the password. But note the problem - if the client doesn't know the password, but knows a hash of it (as stored in mysql.user.Password) it can connect to the server. In other words, the hash of a password is the real password; if one can get the value of mysql.user.Password - he can connect to the server.

[edit] 4.1 and later
Remember that mysql.user.Password stores SHA1(SHA1(password))
    The server sends a random string (scramble) to the client
    the client calculates:
        stage1_hash = SHA1(password), using the password that the user has entered.
        token = SHA1(scramble + SHA1(stage1_hash)) XOR stage1_hash
    the client sends the token to the server
    the server calculates
        stage1_hash' = token XOR SHA1(scramble + mysql.user.Password)
    the server compares SHA1(stage1_hash') and mysql.user.Password
    If they are the same, the password is okay.

(Note SHA1(A+B) is the SHA1 of the concatenation of A with B.)

This protocol fixes the flaw of the old one, neither snooping on the wire nor mysql.user.Password are sufficient for a successful connection. But when one has both mysql.user.Password and the intercepted data on the wire, he has enough information to connect. `
-->

### 4.0版本之前

基本流程如下：

1. 服務器發送隨機字符串 (scramble) 給客戶端。可以參考 create_random_string() 的生成方法。

2. 客戶端把用戶明文密碼加密一下，然後再將其與服務器發送的隨機字符串加密一下，然後變成了新的 scramble_buff 發送給服務端。可以參考 scramble() 函數的實現。

3. 服務端將 mysql.user.password 中的值加上原始隨機字符串進行加密，如果加密後的值和客戶端發送過來的內容一樣，則驗證成功。

需要注意的是：真正意義上的密碼是明文密碼的加密 hash 值; 如果有人知道了這個用戶的 password 哈希值，而不用知道原始明文密碼，實際上他就能直接登錄服務器。

### 4.1 以後版本

數據庫中保存的密碼是用 SHA1(SHA1(password)) 加密的，其流程為：

1. 服務器發送隨機字符串 (scramble) 給客戶端。

2. 客戶端作如下計算，然後客戶端將 token 發送給服務端。

    stage1_hash = SHA1(明文密碼)

    token = SHA1(scramble + SHA1(stage1_hash)) XOR stage1_hash

3. 服務端作如下計算，比對 SHA1(stage1_hash) 和 mysql.user.password 是否相同。

    stage1_hash = token XOR SHA1(scramble + mysql.user.password)

這裡實際用到了異或的自反性： A XOR B XOR B = A ，對於給定的數 A，用同樣的運算因子 B 作兩次異或運算後仍得到 A 本身。對於當前情況的話，實際的計算過程如下。

{% highlight text %}
token = SHA1(scramble + SHA1(SHA1(password))) XOR SHA1(password)         // 客戶端返回的值
      = PASSWORD XOR SHA1(password)

stage1_hash = token XOR SHA1(scramble + mysql.user.password) = token XOR PASSWORD
            = [PASSWORD XOR SHA1(password)] XOR PASSWORD
            = SHA1(password)
{% endhighlight %}

因此，校驗時，只需要 SHA1(stage1_hash) 與 mysql.user.password 比較一下即可。

這次沒上一個版本的缺陷了. 有了 mysql.user.password 和 scramble 也不能獲得 token，因為沒法獲得 stage1_hash。

但是如果用戶的 mysql.user.password 洩露，並且可以在網絡上截取的一次完整驗證數據，從而可以反解出 stage1_hash 的值。而該值是不變的，因此下次連接獲取了新的 scramble 後，自己加密一下 token 仍然可以鏈接到服務器。


## 源碼分析

接下來分別介紹客戶端、服務端的程序。

### 客戶端

對於 mysql 客戶端，源碼保存在 client/mysql.cc 文件中，下面是 main() 函數的主要執行流程。

{% highlight text %}
main()
 |-sql_connect()
 | |-sql_real_connect()
 |   |-mysql_init()                             # 調用MySQL初始化
 |   |-mysql_options()                          # 設置鏈接選項
 |   |-mysql_real_connect()                     # sql-common/client.c
 |     |-connect_sync_or_async()                # 通過該函數嘗試鏈接
 |     | |-my_connect()                         # 實際通過該函數建立鏈接
 |     |-cli_safe_read()                        # 等待第一個handshake包
 |     |-run_plugin_auth()                      # 通過插件實現認證
 |
 |-put_info()                                   # 打印客戶端的歡迎信息
 |-read_and_execute()                           # 開始等待輸入、執行SQL
{% endhighlight %}

客戶端最終會調用 mysql_real_connect()，實際調用的是 cli_mysql_real_connect()，通過該函數建立鏈接，其中認證方式可以通過 run_plugin_auth() 時用插件實現。

然後，會輸出一系列的歡迎信息，並通過 read_and_execute() 執行 SQL 命令。

在 MySQL 客戶端執行時，並非所有的命令都是需要發送到服務端的，其中有一個數組定義了常見的命令。

{% highlight c %}
static COMMANDS commands[] = {
  { "?",      '?', com_help,   1, "Synonym for `help'." },
  { "clear",  'c', com_clear,  0, "Clear the current input statement."},
  ... ...
};
{% endhighlight %}

每次讀取一行都會通過 find_command() 函數進行檢測，如果滿足對應的命令，且對應的函數變量非空，則直接執行，如 clear，此時不需要輸入分號即可；如果沒有找到，則必須要等待輸入分號。

{% highlight c %}
int read_and_execute(bool interactive)
{
    while (!aborted) {
        if (!interactive) {                               // 是否為交互模式
            ... ...   // 非交互模式，直接執行
        } else {                                          // 交互模式
            char *prompt = ...;                           // 首先會設置提示符
            line = readline(prompt);                      // 從命令行讀取
            if ( ... && (com= find_command(line))) {      // 從commands[]中查找
                (*com->func)(&glob_buffer,line);          // 如果是help、edit等指令，則直接執行
            }
            add_line(...);                                // 常見的SQL，最終在此執行
        }
    }
}

int com_go(String *buffer,char *line)
{
    timer=start_timer();                                                // 設置時間
    error= mysql_real_query_for_lazy(buffer->ptr(),buffer->length());   // 執行查詢SQL
    do {
        // 獲取結果
    } while(!(err= mysql_next_result(&mysql)));
}
{% endhighlight %}

在 add_line() 函數中，最終會調用 com_go() 函數，該函數是執行的主要函數，會最終調用 MySQL API 執行相應的 SQL、返回結果、輸出時間等統計信息。

### 服務端

服務端通過 network_init() 執行一系列初始化之後，會阻塞在 handle_connections_sockets() 函數的 select()/poll() 函數處。

對於 one_thread_per_connection 這種方式，會新建一個線程執行 handle_one_connection() 。

{% highlight text %}
handle_one_connection()
 |-thd_prepare_connection()
   |-login_connection()
     |-check_connection()
       |-acl_authenticate()
{% endhighlight %}

源碼內容如下。

<!-- send_server_handshake_packet(...);  // sql/sql_acl.cc -->

{% highlight c %}
/* sql/sql_connect.cc */
int check_connection(THD *thd)
{
    if (!thd->main_security_ctx.host) {  // 通過TCP/IP連接，或者本地用-h 127.1
         if (acl_check_host(...))        // 檢查hostname
    } else {                             // 使用unix sock連接，不會進行檢測
        ... ...
    }
    return acl_authenticate(thd, connect_errors, 0)
}

/* sql/sql_acl.cc */
bool acl_authenticate(THD *thd, uint connect_errors, uint com_change_user_pkt_len)
{
    if (command == COM_CHANGE_USER) {

    } else {
        do_auth_once()                      // 執行認證模式

    }
}
{% endhighlight %}

在 acl_check_host() 會檢查兩個對象，一個是 hash 表 acl_check_hosts；另一個是動態數組 acl_wild_hosts 。這2個對象是在啟動的時候，通過 init_check_host() 從 mysq.users 表裡讀出並加載的，其中 acl_wild_hosts 用於存放有統配符的主機，acl_check_hosts 存放具體的主機。

最終會調用 acl_authenticate() 這是主要的認證函數。

### 插件實現

MySQL 的認證授權採用插件實現。

默認採用 mysql_native_password 插件，也就是使用 native_password_auth_client() 作用戶端的認證，實際有效的函數是 scramble()。

上述的函數通過用戶輸入的 password、服務器返回的 scramble 生成 reply，返回給服務端；可以通過 ```password('string')``` 查看加密後的密文。

<!--MySQL 對於登陸認證採用插件方式，默認會採用 mysql_native_password 插件，也即新的協議，-->

以 plugin/auth/ 目錄下的插件為例，在啟動服務器時，可添加 \-\-plugin-load=auth_test_plugin.so 參數自動加載相應的授權插件。

{% highlight text %}
----- 獲得foobar的加密格式
mysql> select password('foobar');
----- 舊的加密格式
mysql> select old_password('foobar');
----- 默認方式
mysql> create user 'foobar2'@'localhost' identified via mysql_native_password using 'xxx';

----- 也可以動態加載
mysql> install plugin test_plugin_server soname 'auth_test_plugin.so';
----- 查看當前支持的插件
mysql> select * from information_schema.plugins where plugin_type='authentication';

mysql> create user 'foobar'@'localhost' identified with test_plugin_server;
mysql> SET PASSWORD FOR 'foobar'@'localhost'=PASSWORD('new_password');
mysql> DROP USER 'foobar'@'localhost';
mysql> FLUSH PRIVILEGES;
mysql> SELECT host, user, password, plugin FROM mysql.user;
{% endhighlight %}

在 plugin 目錄下有很多 auth 插件可供參考，詳細可參考官網 [Writing Authentication Plugins](http://dev.mysql.com/doc/refman/en/writing-authentication-plugins.html) 。

## 總結

在如下列舉客戶端與服務端的詳細交互過程，其中客戶端代碼在 client 目錄下。

{% highlight text %}
### Client(mysql)  ###                       ### Server(mysqld) ###
----------------------------------------     --------------------------------------------------
main()                                       mysqld_main()
 |-sql_connect()                              |-init_ssl()
 | |-sql_real_connect() {for(;;)}             |-network_init()
 |   |-mysql_init()                           |-handle_connections_sockets()
 |   |-init_connection_options()                |-select()/poll()
 |   |-mysql_real_connect()                     |
 |     |-cli_mysql_real_connect()               |
 |       |-socket()                             |
 |       |-vio_new()                            |
 |       |-vio_socket_connect()                 |
 |       | |-mysql_socket_connect()             |
 |       |   |-connect()                        |
 |       |   |                                  |
 |       |   |        [Socket Connect]          |
 |       |   |>>==========>>==========>>======>>|
 |       |                                      |-accept()
 |       |-vio_keepalive()                      |-vio_new()
 |       |-my_net_set_read_timeout()            |-my_net_init()
 |       |-my_net_set_write_timeout()           |-create_new_thread()
 |       |-vio_io_wait()                          |-handle_one_connection()    {新線程}
 |       |                                          |-thd_prepare_connection() {for(;;)}
 |       |                                          | |-login_connection()
 |       |                                          |   |-check_connection()
 |       |                                          |     |-acl_check_host()
 |       |                                          |     |-vio_keepalive()
 |       |                                          |     |-acl_authenticate()
 |       |                                          |       |-do_auth_once()
 |       |                                          |       | |-native_password_authenticate()  {插件實現}
 |       |                                          |       |   |-create_random_string()
 |       |                                          |       |   |-send_server_handshake_packet()
 |       |                                          |       |   |
 |       |              [Handshake Initialization]  |       |   |
 |       |<<==<<==========<<==========<<==========<<==========<<|
 |       |-cli_safe_read()                          |       |   |-my_net_read()
 |       |-run_plugin_auth()                        |       |   |
 |       | |-native_password_auth_client()          |       |   |
 |       |   |-scramble()                           |       |   |
 |       |     |-my_net_write()                     |       |   |
 |       |     |                                    |       |   |
 |       |     |            [Client Authentication] |       |   |
 |       |     |>>==========>>==========>>==========>>========>>|
 |       |                                          |       |   |-check_scramble()
 |       |                                          |       |-mysql_change_db()
 |       |                                          |       |-my_ok()
 |       |                      [OK]                |       |
 |       |<<==========<<==========<<==========<<==========<<|
 |       |-cli_safe_read()                          |
 |                                                  |
 |                                                  |
 |                                                  |
 |                                                  |
 |-put_info() {Welcome Info}                        |
 |-read_and_execute() [for(;;)]                     |
                                                    |-thd_is_connection_alive()  [while()]
                                                    |-do_command()
{% endhighlight %}




<!--

最終會調用 native_password_authenticate() 函數實現認證，<br><br>

mysql -h a.b.c.d -u root -pxxxx

最終都會調用到mysql_real_connect(sql/client.c的1856行的宏CLI_MYSQL_REAL_CONNECT)，我們從這個函數出發。

<ol><li>
客戶端發起 socket 連接，等待三次握手的通過。</li><br><li>

三次握手通過之後，客戶端進入 client_safe_read() 阻塞，同時服務端從 handle_connection_sockets() 函數的 select()/poll() 處返回，獲知有新的連接。</li><br><li>

服務端創建線程，然後開始對這個連接進行校驗，其中 check_connection() 為的主校驗函數，會檢測該用戶是否可以鏈接，然後產生一個長度為 20 的 scramble 發送給客戶端。</li><br><li>






密碼驗證的plugin初始化比較隱晦，只能估摸著摸索一下：
前面提到了那個重要的函數native_password_authenticate， 這個是密碼驗證插件的定義函數。也是密碼驗證算法入口。mysql 通過以下方式將其註冊成plugin.
1. 初始化一個插件結構，該結構定義了一個插件所對應的handler，對應的安全認證函數handler為native_password_handler或者old_password_handler
mysql_declare_plugin(mysql_password)
{
  MYSQL_AUTHENTICATION_PLUGIN,
  &native_password_handler,
  native_password_plugin_name.str,
  "R.J.Silk, Sergei Golubchik",
  "Native MySQL authentication",
  PLUGIN_LICENSE_GPL,
  NULL,
  NULL,
  0x0100,
  NULL,
  NULL,
  NULL
},
{
  MYSQL_AUTHENTICATION_PLUGIN,
  &old_password_handler,
  old_password_plugin_name.str,
  "R.J.Silk, Sergei Golubchik",
  "Old MySQL-4.0 authentication",
  PLUGIN_LICENSE_GPL,
  NULL,
  NULL,
  0x0100,
  NULL,
  NULL,
  NULL
}
mysql_declare_plugin_end;
native_password_handler被定義為：
static struct st_mysql_auth native_password_handler=
{
  MYSQL_AUTHENTICATION_INTERFACE_VERSION,
  native_password_plugin_name.str,
  native_password_authenticate //那個重要的函數
};
2. 下面就要看mysql_declare_plugin是怎麼定義的。它是一個很曲折的宏定義：
#ifndef MYSQL_DYNAMIC_PLUGIN
#define __MYSQL_DECLARE_PLUGIN(NAME, VERSION, PSIZE, DECLS)                   \
MYSQL_PLUGIN_EXPORT int VERSION= MYSQL_PLUGIN_INTERFACE_VERSION;                                  \
MYSQL_PLUGIN_EXPORT int PSIZE= sizeof(struct st_mysql_plugin);                                    \
MYSQL_PLUGIN_EXPORT struct st_mysql_plugin DECLS[]= {
#else
#define __MYSQL_DECLARE_PLUGIN(NAME, VERSION, PSIZE, DECLS)                   \
MYSQL_PLUGIN_EXPORT int _mysql_plugin_interface_version_= MYSQL_PLUGIN_INTERFACE_VERSION;         \
MYSQL_PLUGIN_EXPORT int _mysql_sizeof_struct_st_plugin_= sizeof(struct st_mysql_plugin);          \
MYSQL_PLUGIN_EXPORT struct st_mysql_plugin _mysql_plugin_declarations_[]= {
#endif

#define mysql_declare_plugin(NAME) \
__MYSQL_DECLARE_PLUGIN(NAME, \
                 builtin_ ## NAME ## _plugin_interface_version, \
                 builtin_ ## NAME ## _sizeof_struct_st_plugin, \
                 builtin_ ## NAME ## _plugin)
該定義在c++中做了一個c的export聲明，聲明瞭名為 builtin_## NAME ## _plugin， 類型為st_mysql_plugin的插件數組。在前面第一步中，使用了 mysql_password作為NAME, 因此，這個插件數組名字為builtin_mysql_password_plugin,裡面放了兩個安全驗證插件，一個為native_password_handler 另一個為old_password_handler。
3. 初始化插件
在plugin.cc的plugin_init方法中，有一段初始化插件的for循環，循環中的mysql_mandatory_plugins是如何定義的呢？

  for (builtins= mysql_mandatory_plugins; *builtins || mandatory; builtins++)
  {
    if (!*builtins)
    {
      builtins= mysql_optional_plugins;
      mandatory= false;
      if (!*builtins)
        break;
    }
mysql_mandatory_plugins定義在了sql_builtin.cc.in
struct st_mysql_plugin *mysql_mandatory_plugins[]=
{
  builtin_binlog_plugin, builtin_mysql_password_plugin, @mysql_mandatory_plugins@ 0
};
在cmakefile中，
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/sql/sql_builtin.cc.in ${CMAKE_BINARY_DIR}/sql/sql_builtin.cc)將builtin.cc.in 轉為源文件進行編譯。於是安全驗證插件被包含在了mysql_mandatory_plugins中，並在上面介紹的方法中得到了初始化。



下面以 plugin/auth_examples 目錄下的插件為例，在啟動 server 時，添加 --plugin-load=auth_test_plugin.so 參數自動加載。現在還不太清楚？？？？？？？？
<pre style="font-size:0.8em; face:arial;">
mysql> select password('foobar');                                                           // 獲得foobar的加密格式
mysql> select old_password('foobar');                                                       // 舊的加密格式
mysql> create user 'foobar2'@'localhost' identified via mysql_native_password using 'xxx';  // 默認方式

mysql> install plugin test_plugin_server soname 'auth_test_plugin.so';                      // 也可以動態加載
mysql> select * from information_schema.plugins where plugin_type='authentication';         // 查看當前支持的插件



mysql> create user 'foobar'@'localhost' identified with test_plugin_server;
mysql> SET PASSWORD FOR 'foobar'@'localhost'=PASSWORD('new_password');
mysql> DROP USER 'foobar'@'localhost';
mysql> FLUSH PRIVILEGES;
select host, user, password, plugin from mysql.user;
</pre>




<pre style="font-size:0.8em; face:arial;">
typedef struct st_net {
    unsigned int pkt_nr;                // 發送包的順序，SVR+CLI都從0開始
    my_bool compress;                    // 是否壓縮
} NET;

vio_read();                 // 最終讀取的函數，會調用read()



struct st_vio {   /* include/violite.h */

};
typedef struct st_vio Vio;


typedef struct st_mysql {
    NET net;               // 網絡參數

} MYSQL;
</pre>
在 Vio 中，包含了大量的接口函數，用於屏蔽底層的不同協議。

<br><br>
客戶端和服務端的 NET.pkt_nr 都會從 0 開始，在接受包時比較 packet number 和 NET.pkt_nr 是否相等，否則報 packet number 亂序，連接報錯，也就是說必須要保證數據的順序性。<br><br>


<br><br><h1>網絡實現</h1><p>
MySQL 中通訊相關的代碼保存在 vio 目錄下，主要完成對底層通訊協議以及跨平臺的封裝，現在支持的協議包括 TCP/IP、共享內存、SSL、命名管道等。這樣只需要關心相關的算法、處理邏輯即可。<br><br>

與網絡協議相關的主要有兩個數據結構：struct st_net@include/mysql_com.h(NET)、struct st_vio@include/violite.h(Vio)。其中 NET 是 MySQL 網絡通信數據結構，Vio 是對網絡通信底層的進行封裝結構。通過 Vio 屏蔽了跨平臺的差異性、統一了不同讀寫策略的接口等。<br><br>

常見的接口有。
<ul><li>
net_write_command()@sql/net_serv.cc<br>
所有的 SQL 最終調用這個命令發送出去。</li><br><li>

my_net_write()@sql/net_serv.cc<br>
連接階段的 socket write 操作調用這個函數。</li><br><li>

my_net_read()@sql/net_serv.ccc<br>
讀取包,會判斷包大小,是否是分包
<!--
網絡協議關鍵函數
my_real_read解析MySQL packet,第一次讀取4字節,根據packet length再讀取餘下來的長度
cli_safe_read客戶端解包函數,包含了my_net_read
</li></ul>

<ul><li>
net_send_ok()@src/protocol.cc<br>
發送 OK 包，通常為命令、insert、update、delete 的返回結果
● 包體首字節為0.
● insert_id, affect_rows也是一併發過來.
</li></ul>

<ul><li>
NET 緩衝<br>
每次 socket 操作都會先將數據讀寫到 NET.buf 緩衝區中，從而減少系統調用調用的次數。當寫入的數據 + buff 內的數據超過 buff 大小才會發出一次 write() 操作，寫入不會導致 buff 區域擴展，可以參考 net_write_buff@sql/net_serv.cc。<br><br>
net->buff大小初始net->max_packet, 讀取會導致會導致 buff的realloc最大net->max_packet_size
注意，一次 SQL 命令結束時，會調用 net_flush() 把 buff 裡的數據都寫到 socket 裡。</li><br><li>


VIO 緩衝<br>
● 從my_read_read可以看出每次packet讀取都是按需讀取， 為了減少系統調用,vio層面加了一個read_buffer.
● 每次讀取前先判斷vio->read_buffer所需數據的長度是 否足夠.如果存在則直接copy. 如果不夠,則觸發一次 socket read 讀取2048個字(vio/viosocket.c: vio_read_buff)



ok packet
● ok包,命令和insert,update,delete的返回結果
● 包體首字節為0.
● insert_id, affect_rows也是一併發過來.
● src/protocol.cc:net_send_ok

error packet

● 錯誤的命令,非法的sql的返回包
● 包體首字節為255.
● error code就是CR_***,include/errmsg.h
● sqlstate marker是#
● sqlstate是錯誤狀態,include/sql_state.h
● message是錯誤的信息
● sql/protocol.cc:net_send_error_packet

resultset packet

● 結果集的數據包,由多個packet組合而成
● 例如查詢一個結構集,順序如下:
    ○ header
    ○ field1....fieldN
    ○ eof
    ○ row1...rowN
    ○ eof
● sql/client.c:cli_read_query_result
● 下面是一個sql "select * from d"查詢結果集的例子,結果 集是6行，3個字段
    ○ 公式：假設結果集有N行, M個字段.則包的個數為，header(1) + field (M) + eof(1) + row(N) + eof(1)
    ○ 所以這個例子的MySQL packet的個數是12個

resultset packet - header

● field packet number決定了接下來的field packet的個數.
● 一個返回6行記錄,3個字段的查詢語句

resultset packet - field

● 結果集中一個字段一個field packet.
● tables_alias是sql語句裡表的別名,org_table才是表的真 實名字.
● sql/protocol.cc:Protocol::send_fields
● sql/client.c:cli_read_query_result

resultset packet - eof

● eof包是用於分割field packet和row packet.
● 包體首字節為254
● sql/protocol.cc:net_send_eof

resultset packet - row

● row packet裡才是真正的數據包.一行數據一個packet.
● row裡的每個字段都是length coded binary
● 字段的個數在header packet裡
● sql/client.c:cli_read_rows

command packet

● 命令包,包括我們的sql語句還有一些常見的命令.
● 包體首字母表示命令的類型(include/mysql_com.h),大 部分命令都是COM_QUERY.

網絡協議關鍵函數

● net_write_command(sql/net_serv.cc)所有的sql最終調用這個命令發送出去.
● my_net_write(sql/net_serv.cc)連接階段的socket write操作調用這個函數.
● my_net_read讀取包,會判斷包大小,是否是分包
● my_real_read解析MySQL packet,第一次讀取4字節,根據packet length再讀取餘下來的長度
● cli_safe_read客戶端解包函數,包含了my_net_read

NET緩衝

● 每次socket操作都會先把數據寫,讀到net->buff,這是一 個緩衝區, 減少系統調用調用的次數.
● 當寫入的數據和buff內的數據超過buff大小才會發出一次 write操作,然後再把要寫入的buff裡插入數, 寫入不會 導致buff區區域擴展.(sql/net_serv.cc: net_write_buff).
● net->buff大小初始net->max_packet, 讀取會導致會導致 buff的realloc最大net->max_packet_size
● 一次sql命令的結束都會調用net_flush,把buff裡的數據 都寫到socket裡.

VIO緩衝

● 從my_read_read可以看出每次packet讀取都是按需讀取， 為了減少系統調用,vio層面加了一個read_buffer.
● 每次讀取前先判斷vio->read_buffer所需數據的長度是 否足夠.如果存在則直接copy. 如果不夠,則觸發一次 socket read 讀取2048個字(vio/viosocket.c: vio_read_buff)

MySQL API

● 數據從mysql_send_query處發送給服務端,實際調用的是 net_write_command.
● cli_read_query_result解析header packet, field packet,獲 得field_count的個數
● mysql_store_result解析了row packet,並存儲在result- >data裡
● myql_fetch_row其實遍歷result->data

________________________________

PACKET NUMBER

在做proxy的時候在這裡迷糊過,翻了幾遍代碼才搞明白，細節如下： 客戶端服務端的net->pkt_nr都從0開始.接受包時比較packet number 和net->pkt_nr是否相等,否則報packet number亂序,連接報錯;相等則pkt_nr自增.發送包時把net->pkt_nr作為packet number發送,然後對net->pkt_nr進行自增保持和對端的同步.

接收包

sql/net_serv.c:my_real_read

898     if (net->buff[net->where_b + 3] != (uchar) net->pkt_nr)

發送包

sql/net_serv.c:my_net_write

392   int3store(buff,len);
393   buff[3]= (uchar) net->pkt_nr++;

我們來幾個具體場景的packet number, net->pkt_nr的變化

連接

0 c ———–> s 0  connect

0 c <—-0——s 1  handshake

2 c —–1—–>s 1  auth

2 c <—–2——s 0  ok

開始兩方都為0,服務端發送handshake packet(pkt=0)之後自增為1,然後等待對端發送過來pkt=1的包

查詢

每次查詢,服務客戶端都會對net->pkt_nr進行清零

include/mysql_com.h
388 #define net_new_transaction(net) ((net)->pkt_nr=0)
sql/sql_parse.cc:do_command
805   net_new_transaction(net);

sql/client.c:cli_advanced_command

800   net_clear(&mysql->net, (command != COM_QUIT));

開始兩方net->pkt_nr皆為0, 命令發送後客戶端端為1,服務端開始發送分包,分包的pkt_nr的依次遞增,客戶端的net->pkt_nr也隨之增加.

1 c ——0—–> s 0  query

1 c <—-1——s 2  resultset

2 c <—-2——s 3  resultset

解包的細節

my_net_read負責解包，首先讀取4個字節，判斷packet number是否等於net->pkt_nr然後再次讀取packet_number長度的包體。

偽代碼如下：

remain=4
for(i = 0; i < 2; i++) {
    //數據是否讀完
    while (remain>0)  {
        length = read(fd, net->buff, remain)
        remain = remain - length
    }
    //第一次
    if (i=0) {
        remain = uint3korr(net->buff+net->where_b);
    }
}

網絡層優化

從ppt裡可以看到,一個resultset packet由多個包組成,如果每次讀寫包都導致系統調用那肯定是不合理,常規優化方法:寫大包加預讀

NET->BUFF

每個包發送到網絡或者從網絡讀包都會先把數據包保存在net->buff裡,待到net->buff滿了或者一次命令結束才會通過socket發出給對端.net->buff有個初始大小(net->max_packet),會隨讀取數據的增多而擴展.

VIO->READ_BUFFER

每次從網絡讀包,並不是按包的大小讀取,而是會盡量讀取2048個字節,這樣一個resultset包的讀取不會再引起多次的系統調用了.header packet讀取完畢後, 接下來的field,eof, row apcket讀取僅僅需要從vio-read_buffer拷貝指定字節的數據即可.

MYSQL API說明

api和MySQL客戶端都會使用sql/client.c這個文件,解包的過程都是使用sql/client.c:cli_read_query_result.

mysql_store_result來解析row packet,並把數據存儲到res->data裡,此時所有數據都存內存裡了.

mysql_fetch_row僅僅是使用內部的遊標,遍歷result->data裡的數據

3052     if (!res->data_cursor)
3053     {
3054       DBUG_PRINT("info",("end of data"));
3055       DBUG_RETURN(res->current_row=(MYSQL_ROW) NULL);
3056     }
3057     tmp = res->data_cursor->data;
3058     res->data_cursor = res->data_cursor->next;
3059     DBUG_RETURN(res->current_row=tmp);

mysql_free_result是把result->data指定的行數據釋放掉.


● handshake
● auth
● ok|error
● resultset
    ○ header
    ○ field
    ○ eof
    ○ row
● command packet



● 數據從mysql_send_query處發送給服務端,實際調用的是 net_write_command.
● cli_read_query_result解析header packet, field packet,獲 得field_count的個數
● mysql_store_result解析了row packet,並存儲在result- >data裡
● myql_fetch_row其實遍歷result->data


net->buff

每個包發送到網絡或者從網絡讀包都會先把數據包保存在net->buff裡,待到net->buff滿了或者一次命令結束才會通過socket發出給對端.net->buff有個初始大小(net->max_packet),會隨讀取數據的增多而擴展.
vio->read_buffer

每次從網絡讀包,並不是按包的大小讀取,而是會盡量讀取2048個字節,這樣一個resultset包的讀取不會再引起多次的系統調用了.header packet讀取完畢後, 接下來的field,eof, row apcket讀取僅僅需要從vio-read_buffer拷貝指定字節的數據即可.
MySQL api說明

api和MySQL客戶端都會使用sql/client.c這個文件,解包的過程都是使用sql/client.c:cli_read_query_result.

mysql_store_result來解析row packet,並把數據存儲到res->data裡,此時所有數據都存內存裡了.

mysql_fetch_row僅僅是使用內部的遊標,遍歷result->data裡的數據

3052     if (!res->data_cursor)
3053     {
    3054       DBUG_PRINT("info",("end of data"));
    3055       DBUG_RETURN(res->current_row=(MYSQL_ROW) NULL);
    3056     }
    3057     tmp = res->data_cursor->data;
    3058     res->data_cursor = res->data_cursor->next;
    3059     DBUG_RETURN(res->current_row=tmp);

    mysql_free_result是把result->data指定的行數據釋放掉.





 枚舉類型enum_vio_type定義了Vio的幾種不同的網絡連接類型，根據不同的類型，在網絡通信時有相應不同的處理邏輯。

       Vio數據結構的定義分為幾個部分：成員變量部分、接口部分、SSL部分、共享內存部分和windows特有成員變量。其中成員變量包括：socket描述符sd；管道描述符hPipe；localhost表示是否為本機；fcntl_mode表示socket文件描述符的模式，通過fcntl()函數（在windows下是fcntlsocket()函數）設置sd描述符的一些特性；local和remote分別表示本地和遠端的網絡地址，其中sockaddr_storage是通用網絡地址數據結構；addrLen遠端的網絡地址長度；type表示網絡連接類型；數組desc是Vio的描述信息；read_buffer、read_pos、read_end分別表示讀buffer緩衝的指針地址、讀取的當前位置以及結束地址，socket數據讀取採用緩衝機制，提高讀的性能。接口部分是socket的基本操作的函數指針，根據不同的平臺和不同的讀寫策略，分別指向不同的處理函數。SSL部分是指當定義了HAVE_OPENSSL宏時，定義SSL相關的成員變量ssl_arg。共享內存部分是指當定義了HAVE_SMEM宏時，定義共享內存相關的成員變量。如果定義了_WIN32宏時，那麼需要定義windows操作的成員變量。SSL部分、共享內存部分和windows特有成員變量都是根據編譯時定義的宏及平臺決定的，是不同讀寫策略時，使用的成員變量。

源碼實現

       MySQL數據結構Vio對網絡通信的封轉，在源碼的/vio/vio.c、/vio/viosocket.c、/vio/viossl.c、/vio/viosslfactories.c中實現。由於大多數的實現是根據不同的連接類型，對底層函數的調用，基本不涉及複雜的算法和處理邏輯。因此，以下內容中，僅對幾個比較核心的處理過程進行簡要的分析。

vio_init函數

       vio_init()函數是Vio的內部初始化函數，對外接口vio_new*()初始化函數，都調用vio_init()函數進行初始化。該函數根據不同的連接類型，初始化相應類型的處理函數指針。參考源碼/vio/vio.c。

vio_read_buffer函數

       vio_read_buffer()函數是網絡緩衝寫方法，該方法同IO_CACHE的io讀有相似之處。讀取策略分為三種：如果read_buffer中有數據，直接從read_buffer中讀取數據；如果讀取的數據長度小於16K，那麼調用vio_read()函數讀取16K數據到read_buffer中，然後再從read_buffer中讀取數據；如果數據長度大於16K，直接通過vio_read()讀取制定長度的數據，不需要寫入緩衝read_buffer。這種讀寫策略，可以有效的提高小數據量讀取的性能。例如readline這種方式，需要通過逐個字符判斷是否為換行符，如果直接調用vio_read()進行讀取，會每次產生一次尋址、讀取等物理操作。而採用緩衝讀策略，所有的操作從read_buffer中獲取數據，避免頻繁的物理操作。

vio_close函數

       vio_close()函數是關閉網絡連接的操作，在該過程中，有一個問題需要特別說明。在調用close()（windows下使用closesocket()函數）進行關閉socket連接之前，調用shutdown()函數將讀寫關閉。這是因為，在多個進程共享一個socket套接字，調用close()只是引用數減1，其他進程仍然可以通信，直到計數為0，才將socket套接字釋放。而調用shutdown(2)則使得其他進程也無法進行通信。具體close()[1]和shutdown()[2]的區別，可以參考Linux manual中相應的解釋。

socket_poll_read函數

       socket_poll_read()函數是實現IO複用的封裝函數，從該封裝函數中可以看出，MySQL在IO複用中，如果操作系統是windows，那麼採用select()函數（在linux下select()的最大文件描述數目為1024，windows下無限制），在Linux系統中，使用poll()函數。select()和poll()類似，性能比select()略高。然而，這兩種方式都存在一個問題，因為他們都需要遍歷所有的文件描述符，當監聽描述符個數增加時，監聽效率降低，並且select和poll每次都要在用戶態和內核態拷貝監聽的描述符參數。更為高效的解決方案是epoll()方法，可以解決select()和poll()存在的不足。具體詳細的分析和差異，參考相應的文檔。

       SSL相關的處理函數在源碼的/vio/viossl.c和/vio/viosslfactories.c，具體的實現主要是對SSL相關函數的封裝，不再贅述。

結論

       通過以上分析可知，MySQL數據結構Vio主要封裝了不同連接方式的網絡通信接口，從而使得網絡通信可以忽略平臺差異和連接方式的不同，並且可以支持共享內存、SSL安全連接方式等通信方式。

       然而在IO複用方面，MySQL數據結構Vio存在一定的不足，使用了poll()方式。對於數據庫這種高併發系統來說，會隨著連接數的增加，使得poll()的系統調用次數嚴重下降，處理能力也隨之降低。因此，對MySQL來說，一般的優化方案是限制連接數，而在應用層使用連接池的策略，來解決這個問題。然而當多個不同系統同時使用一個數據庫實例時，仍然會導致連接數增加，如果該值設置較小的話，會導致連接失敗。最佳的方式是使用epoll()方式代替poll()方式，從本質上提高處理能力。





用戶通過 mysql 客戶端登錄時，實際最終調用的是 mysql_real_connect() 函數。<br><br>


MySQL 的用戶管理模塊信息存儲在系統表 mysql.user 中，包括了授權用戶的基本信息以及一些權限信息。其中登陸時，只會用到 host、user、passwd，也就是說登陸認證需要 host+user 關聯，當然可以使用 * 通配符。<br><br>


信息系統中，訪問控制分為自主訪問控制(DAC)和強制訪問控制(MAC)。具體到DBMS，自主訪問控制就是我們所熟悉的GRANT，REVOKE，大多數數據庫都支持自助的訪問控制。強制訪問控制就是ORACLE中的LABEL，只有很少的一些系統支持MAC。
嚴格來說，登錄並不屬於訪問控制機制，而應該屬於用戶身份識別和認證。在Mysql中，將登錄和DAC的相關接口都實現在了
sql_acl.cc中(其實說登錄是用戶擁有的一種權限也未嘗不可，正如ORACLE中的CREATE SESSION，不過登錄並不僅僅是一種權
限，還包含很多其他的屬性)，從文件名大家可以看出來，ACL即ACCESS CONTROL LIST，訪問控制列表，這是實現訪問控制的
基本方法。下圖是Mysql的整個訪問控制的流程。

-->

## 參考

關於 MySQL的認證流程，包括客戶端和服務器端，可以參考本地 [MySQL認證協議](/reference/databases/mysql/MySQL_Auth_Procotol.mht)；詳細的協議介紹可以參考 [MySQL Client/Server Protocol](http://dev.mysql.com/doc/internals/en/client-server-protocol.html)，或者 [中文資料](http://blog.geekcome.com/archives/270)，或者保存的本地資料 [MySQL服務器和客戶端通信協議分析](/reference/databases/mysql/MySQL_Client_Server_Protocol.mht) 。

MySQL 的認證授權可以採用插件，在 plugin 目錄下有很多 auth 插件可供參考，具體可以參考官網的 [MySQL Reference - Writing Authentication Plugins](http://dev.mysql.com/doc/refman/en/writing-authentication-plugins.html) 。


{% highlight text %}
{% endhighlight %}
