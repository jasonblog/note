---
title: Nginx 入門
layout: post
comments: true
language: chinese
category: [linux,webserver]
keywords: nginx,簡介
description: Nginx (發音 "Engine X") 是一款輕量級且高性能的 Web 服務器、反向代理服務器，同時也是一個 IMAP POP3 SMTP 服務器，其代碼完全通過 C 語言編寫，支持多平臺，並且在 BSD-like 協議下發行。是由俄羅斯的 Igor Sysoev 開發，其特點是佔有內存少、併發能力強、豐富的功能集和示例配置文件。在此僅簡單介紹其安裝使用方法。
---

Nginx (發音 "Engine X") 是一款輕量級且高性能的 Web 服務器、反向代理服務器，同時也是一個 IMAP POP3 SMTP 服務器，完全通過 C 語言編寫，支持多平臺，並且在 BSD-like 協議下發行。

是由俄羅斯的 Igor Sysoev 開發，其特點是佔有內存少、併發能力強、豐富的功能集和示例配置文件。在此僅簡單介紹其安裝使用方法。

<!-- more -->

![nginx logo](/images/linux/nginx-logo.png "nginx logo"){: .pull-center width='70%'}

## 簡介

很多操作可以直接參考官方網站的文檔 [www.nginx.org](http://nginx.org/en/docs/) ，在此僅列舉其中比較常用的一些操作，使用的操作系統是 CentOS 。

{% highlight text %}
----- 在CentOS中直接通過yum安裝
# yum install nginx --enablerepo=epel

----- 啟動，然後可以通過127.1直接訪問，也可以通過enable設置開機啟動
# systemctl start nginx.service
{% endhighlight %}

僅介紹一下安裝 Nginx 之後的默認配置選項：默認的服務器根目錄是 ```/usr/share/nginx/html```，配置文件在 ```/etc/nginx``` 目錄下，默認是 ```/etc/nginx/nginx.conf```，日誌默認在 ```/var/log/nginx``` 目錄下。

### 常用操作

如下是經常會使用的命令。

{% highlight text %}
----- 查看相關進程的資源
# ps axw -o pid,ppid,user,%cpu,vsz,wchan,command | egrep 'nginx:'

----- 其它的一些常見的命令
# kill -QUIT `cat /run/nginx.pid`        ← 從容停止
# kill -TERM|INT `cat /run/nginx.pid`    ← 快速停止
# kill -USR1 `cat /run/nginx.pid`        ← 重新打開一個日誌文件
# kill -USR2 `cat /run/nginx.pid`        ← 升級可執行文件
# pkill -9 nginx                         ← 強制停止

# nginx -t -c /etc/nginx/nginx.conf      ← 檢查配置文件的語法
# kill -HUP `cat /run/nginx.pid`         ← 重新加載配置文件
# nginx -s reload                        ← 同上
{% endhighlight %}

另外，為了防止由於安全配置導致報錯，可通過 ```setenforce 0``` 關閉 SeLinux，之前被坑過 ^_^

現在可以直接通過瀏覽器打開 [http://localhost/](http://localhost/) 即可。當然，如果發現不符合期望的，可以直接查看默認的錯誤日誌 ```/var/log/nginx/error.log``` 。


### 源碼安裝

如下是，如何通過源碼安裝、配置 nginx 。

#### 1. 安裝頭文件

安裝時依賴 pcre、openssl、gzip 三個庫，在 CentOS 中，可以通過如下命令查看以及安裝。

{% highlight text %}
# rpm -qa | grep -E '(zlib|pcre|openssl)'
# yum install zlib zlib-devel pcre pcre-devel openssl openssl-devel
# yum install libxslt libxslt-devel
# yum install perl-ExtUtils-Embed
{% endhighlight %}

其中各個庫的作用如下：

* pcre: 用來作地址重寫的功能；
* zlib：用於傳輸數據打包，省流量，但消耗資源；
* openssl：提供ssl加密協議；
* xslt：用於過濾轉換XML請求；
* gd: 用於JPEG/GIF/PNG圖片的一個過濾器

#### 2. 添加用戶

一般啟動時會使用 nginx 用戶，所以通過如下方式添加相應的用戶、用戶組。

{% highlight text %}
# groupadd nginx
# useradd -r -g nginx -s /sbin/nologin -M nginx
{% endhighlight %}

其中 -r 指定系統用戶；-g 指定用戶組；-s 指定 shell；-M 不創建 home 。

#### 3. 下載編譯

直接從官網 [www.nginx.org](http://nginx.org/) 下載源碼，然後通過如下方式編譯、安裝。

<!--
內容有些多，一眼看來難免頭昏腦脹，但堅持看完，相信你一定會有所收穫。
nginx參數：
--prefix= 指向安裝目錄
--sbin-path 指向（執行）程序文件（nginx）
--conf-path= 指向配置文件（nginx.conf）
--error-log-path= 指向錯誤日誌目錄
--pid-path= 指向pid文件（nginx.pid）
--lock-path= 指向lock文件（nginx.lock）（安裝文件鎖定，防止安裝文件被別人利用，或自己誤操作。）
--user= 指定程序運行時的非特權用戶
--group= 指定程序運行時的非特權用戶組
--builddir= 指向編譯目錄
--with-rtsig_module 啟用rtsig模塊支持（實時信號）
--with-select_module 啟用select模塊支持（一種輪詢模式,不推薦在高載環境下使用）禁用：--without-select_module
--with-poll_module 啟用poll模塊支持（功能與select相同，與select特性相同，為一種輪詢模式,不推薦在高載環境下使用）
--with-file-aio 啟用file aio支持（一種APL文件傳輸格式）
--with-ipv6 啟用ipv6支持
--with-http_ssl_module 啟用ngx_http_ssl_module支持（使支持https請求，需已安裝openssl）
--with-http_realip_module 啟用ngx_http_realip_module支持（這個模塊允許從請求標頭更改客戶端的IP地址值，默認為關）
--with-http_addition_module 啟用ngx_http_addition_module支持（作為一個輸出過濾器，支持不完全緩衝，分部分響應請求）
--with-http_geoip_module 啟用ngx_http_geoip_module支持（該模塊創建基於與MaxMind GeoIP二進制文件相配的客戶端IP地址的ngx_http_geoip_module變量）
--with-http_sub_module 啟用ngx_http_sub_module支持（允許用一些其他文本替換nginx響應中的一些文本）
--with-http_dav_module 啟用ngx_http_dav_module支持（增加PUT,DELETE,MKCOL：創建集合,COPY和MOVE方法）默認情況下為關閉，需編譯開啟
--with-http_flv_module 啟用ngx_http_flv_module支持（提供尋求內存使用基於時間的偏移量文件）
--with-http_gzip_static_module 啟用ngx_http_gzip_static_module支持（在線實時壓縮輸出數據流）
--with-http_random_index_module 啟用ngx_http_random_index_module支持（從目錄中隨機挑選一個目錄索引）
--with-http_secure_link_module 啟用ngx_http_secure_link_module支持（計算和檢查要求所需的安全鏈接網址）
--with-http_degradation_module  啟用ngx_http_degradation_module支持（允許在內存不足的情況下返回204或444碼）
--with-http_stub_status_module 啟用ngx_http_stub_status_module支持（獲取nginx自上次啟動以來的工作狀態）
--without-http_charset_module 禁用ngx_http_charset_module支持（重新編碼web頁面，但只能是一個方向--服務器端到客戶端，並且只有一個字節的編碼可以被重新編碼）
--without-http_gzip_module 禁用ngx_http_gzip_module支持（該模塊同-with-http_gzip_static_module功能一樣）
--without-http_ssi_module 禁用ngx_http_ssi_module支持（該模塊提供了一個在輸入端處理處理服務器包含文件（SSI）的過濾器，目前支持SSI命令的列表是不完整的）
--without-http_userid_module 禁用ngx_http_userid_module支持（該模塊用來處理用來確定客戶端後續請求的cookies）
--without-http_access_module 禁用ngx_http_access_module支持（該模塊提供了一個簡單的基於主機的訪問控制。允許/拒絕基於ip地址）
--without-http_auth_basic_module禁用ngx_http_auth_basic_module（該模塊是可以使用用戶名和密碼基於http基本認證方法來保護你的站點或其部分內容）
--without-http_autoindex_module 禁用disable ngx_http_autoindex_module支持（該模塊用於自動生成目錄列表，只在ngx_http_index_module模塊未找到索引文件時發出請求。）
--without-http_geo_module 禁用ngx_http_geo_module支持（創建一些變量，其值依賴於客戶端的IP地址）
--without-http_map_module 禁用ngx_http_map_module支持（使用任意的鍵/值對設置配置變量）
--without-http_split_clients_module 禁用ngx_http_split_clients_module支持（該模塊用來基於某些條件劃分用戶。條件如：ip地址、報頭、cookies等等）
--without-http_referer_module 禁用disable ngx_http_referer_module支持（該模塊用來過濾請求，拒絕報頭中Referer值不正確的請求）
--without-http_rewrite_module 禁用ngx_http_rewrite_module支持（該模塊允許使用正則表達式改變URI，並且根據變量來轉向以及選擇配置。如果在server級別設置該選項，那麼他們將在 location之前生效。如果在location還有更進一步的重寫規則，location部分的規則依然會被執行。如果這個URI重寫是因為location部分的規則造成的，那麼 location部分會再次被執行作為新的URI。 這個循環會執行10次，然後Nginx會返回一個500錯誤。）
--without-http_proxy_module 禁用ngx_http_proxy_module支持（有關代理服務器）
--without-http_fastcgi_module 禁用ngx_http_fastcgi_module支持（該模塊允許Nginx 與FastCGI 進程交互，並通過傳遞參數來控制FastCGI 進程工作。 ）FastCGI一個常駐型的公共網關接口。
--without-http_uwsgi_module 禁用ngx_http_uwsgi_module支持（該模塊用來醫用uwsgi協議，uWSGI服務器相關）
--without-http_scgi_module 禁用ngx_http_scgi_module支持（該模塊用來啟用SCGI協議支持，SCGI協議是CGI協議的替代。它是一種應用程序與HTTP服務接口標準。它有些像FastCGI但他的設計 更容易實現。）
--without-http_memcached_module 禁用ngx_http_memcached_module支持（該模塊用來提供簡單的緩存，以提高系統效率）
-without-http_limit_zone_module 禁用ngx_http_limit_zone_module支持（該模塊可以針對條件，進行會話的併發連接數控制）
--without-http_limit_req_module 禁用ngx_http_limit_req_module支持（該模塊允許你對於一個地址進行請求數量的限制用一個給定的session或一個特定的事件）
--without-http_empty_gif_module 禁用ngx_http_empty_gif_module支持（該模塊在內存中常駐了一個1*1的透明GIF圖像，可以被非常快速的調用）
--without-http_browser_module 禁用ngx_http_browser_module支持（該模塊用來創建依賴於請求報頭的值。如果瀏覽器為modern ，則$modern_browser等於modern_browser_value指令分配的值；如 果瀏覽器為old，則$ancient_browser等於 ancient_browser_value指令分配的值；如果瀏覽器為 MSIE中的任意版本，則 $msie等於1）
--without-http_upstream_ip_hash_module 禁用ngx_http_upstream_ip_hash_module支持（該模塊用於簡單的負載均衡）
--with-http_perl_module 啟用ngx_http_perl_module支持（該模塊使nginx可以直接使用perl或通過ssi調用perl）
--with-perl_modules_path= 設定perl模塊路徑
--with-perl= 設定perl庫文件路徑
--http-log-path= 設定access log路徑
--http-client-body-temp-path= 設定http客戶端請求臨時文件路徑
--http-proxy-temp-path= 設定http代理臨時文件路徑
--http-fastcgi-temp-path= 設定http fastcgi臨時文件路徑
--http-uwsgi-temp-path= 設定http uwsgi臨時文件路徑
--http-scgi-temp-path= 設定http scgi臨時文件路徑
-without-http 禁用http server功能
--without-http-cache 禁用http cache功能
--with-mail 啟用POP3/IMAP4/SMTP代理模塊支持
--with-mail_ssl_module 啟用ngx_mail_ssl_module支持
--without-mail_pop3_module 禁用pop3協議（POP3即郵局協議的第3個版本,它是規定個人計算機如何連接到互聯網上的郵件服務器進行收發郵件的協議。是因特網電子郵件的第一個離線協議標 準,POP3協議允許用戶從服務器上把郵件存儲到本地主機上,同時根據客戶端的操作刪除或保存在郵件服務器上的郵件。POP3協議是TCP/IP協議族中的一員，主要用於 支持使用客戶端遠程管理在服務器上的電子郵件）
--without-mail_imap_module 禁用imap協議（一種郵件獲取協議。它的主要作用是郵件客戶端可以通過這種協議從郵件服務器上獲取郵件的信息，下載郵件等。IMAP協議運行在TCP/IP協議之上， 使用的端口是143。它與POP3協議的主要區別是用戶可以不用把所有的郵件全部下載，可以通過客戶端直接對服務器上的郵件進行操作。）
--without-mail_smtp_module 禁用smtp協議（SMTP即簡單郵件傳輸協議,它是一組用於由源地址到目的地址傳送郵件的規則，由它來控制信件的中轉方式。SMTP協議屬於TCP/IP協議族，它幫助每臺計算機在發送或中轉信件時找到下一個目的地。）
--with-google_perftools_module 啟用ngx_google_perftools_module支持（調試用，剖析程序性能瓶頸）
--with-cpp_test_module 啟用ngx_cpp_test_module支持
--add-module= 啟用外部模塊支持
--with-cc= 指向C編譯器路徑
--with-cpp= 指向C預處理路徑
--with-cc-opt= 設置C編譯器參數（PCRE庫，需要指定–with-cc-opt=”-I /usr/local/include”，如果使用select()函數則需要同時增加文件描述符數量，可以通過–with-cc- opt=”-D FD_SETSIZE=2048”指定。）
--with-ld-opt= 設置連接文件參數。（PCRE庫，需要指定–with-ld-opt=”-L /usr/local/lib”。）
--with-cpu-opt= 指定編譯的CPU，可用的值為: pentium, pentiumpro, pentium3, pentium4, athlon, opteron, amd64, sparc32, sparc64, ppc64
--without-pcre 禁用pcre庫
--with-pcre 啟用pcre庫
--with-pcre= 指向pcre庫文件目錄
--with-pcre-opt= 在編譯時為pcre庫設置附加參數
--with-md5= 指向md5庫文件目錄（消息摘要算法第五版，用以提供消息的完整性保護）
--with-md5-opt= 在編譯時為md5庫設置附加參數
--with-md5-asm 使用md5彙編源
--with-sha1= 指向sha1庫目錄（數字簽名算法，主要用於數字簽名）
--with-sha1-opt= 在編譯時為sha1庫設置附加參數
--with-sha1-asm 使用sha1彙編源
--with-zlib= 指向zlib庫目錄
--with-zlib-opt= 在編譯時為zlib設置附加參數
--with-zlib-asm= 為指定的CPU使用zlib彙編源進行優化，CPU類型為pentium, pentiumpro
--with-libatomic 為原子內存的更新操作的實現提供一個架構
--with-libatomic= 指向libatomic_ops安裝目錄
--with-openssl= 指向openssl安裝目錄
--with-openssl-opt 在編譯時為openssl設置附加參數
--with-debug 啟用debug日誌







make是用來編譯的，它從Makefile中讀取指令，然後編譯。

make install是用來安裝的，它也從Makefile中讀取指令，安裝到指定的位置。

configure命令是用來檢測你的安裝平臺的目標特徵的。它定義了系統的各個方面，包括nginx的被允許使用的連接處理的方法，比如它會檢測你是不是有CC或GCC，並不是需要CC或GCC，它是個shell腳本，執行結束時，它會創建一個Makefile文件。nginx的configure命令支持以下參數：

--with-select_module --without-select_module 啟用或禁用構建一個模塊來允許服務器使用select()方法。該模塊將自動建立，如果平臺不支持的kqueue，epoll，rtsig或/dev/poll。
--with-poll_module --without-poll_module 啟用或禁用構建一個模塊來允許服務器使用poll()方法。該模塊將自動建立，如果平臺不支持的kqueue，epoll，rtsig或/dev/poll。
--without-http_gzip_module — 不編譯壓縮的HTTP服務器的響應模塊。編譯並運行此模塊需要zlib庫。
--without-http_rewrite_module 不編譯重寫模塊。編譯並運行此模塊需要PCRE庫支持。
--without-http_proxy_module — 不編譯http_proxy模塊。
--with-pcre=path — 設置PCRE庫的源碼路徑。PCRE庫的源碼（版本4.4 - 8.30）需要從PCRE網站下載並解壓。其餘的工作是Nginx的./ configure和make來完成。正則表達式使用在location指令和 ngx_http_rewrite_module 模塊中。
--with-pcre-jit —編譯PCRE包含“just-in-time compilation”（1.1.12中， pcre_jit指令）。
--with-zlib=path —設置的zlib庫的源碼路徑。要下載從 zlib（版本1.1.3 - 1.2.5）的並解壓。其餘的工作是Nginx的./ configure和make完成。ngx_http_gzip_module模塊需要使用zlib 。
--with-cc-opt=parameters — 設置額外的參數將被添加到CFLAGS變量。例如,當你在FreeBSD上使用PCRE庫時需要使用:--with-cc-opt="-I /usr/local/include。.如需要需要增加 select()支持的文件數量:--with-cc-opt="-D FD_SETSIZE=2048".
--with-ld-opt=parameters —設置附加的參數，將用於在鏈接期間。例如，當在FreeBSD下使用該系統的PCRE庫,應指定:--with-ld-opt="-L /usr/local/lib".

典型實例(下面為了展示需要寫在多行，執行時內容需要在同一行)

./configure
--sbin-path=/usr/local/nginx/nginx
--conf-path=/usr/local/nginx/nginx.conf
--pid-path=/usr/local/nginx/nginx.pid
--with-http_ssl_module
--with-pcre=../pcre-4.4
--with-zlib=../zlib-1.1.3

$ ./configure --prefix=/usr --sbin-path=/usr/sbin/nginx --conf-path=/etc/nginx/nginx.conf \
    --error-log-path=/var/log/nginx/error.log --pid-path=/var/run/nginx.pid \
    --user=nginx --group=nginx \
    --with-http_ssl_module \
    --with-http_flv_module \
    --with-http_gzip_static_module \
    --http-log-path=/var/log/nginx/access.log \
    --http-client-body-temp-path=/var/tmp/nginx/client \
    --http-proxy-temp-path=/var/tmp/nginx/proxy \
    --http-fastcgi-temp-path=/var/tmp/nginx/fcgi \
    --with-http_stub_status_module
-->

{% highlight text %}
$ ./configure      \
    --prefix=/usr/share/nginx \                   ← 安裝目錄，存放服務器文件
    --sbin-path=/usr/sbin/nginx \                 ← 可執行文件路徑
    --conf-path=/etc/nginx/nginx.conf \           ← 默認配置文件
    --error-log-path=/var/log/nginx/error.log \   ← 報錯文件，通過error_log修改
    --http-log-path=/var/log/nginx/access.log \   ← 日誌文件，通過access_log修改
    --pid-path=/run/nginx.pid \                   ← 存儲主進程號，通過pid修改
    --user=nginx \                                ← 進程工作用戶，通過user指定
    --group=nginx \                               ← 用戶組，通過user指定

    --lock-path=/run/lock/subsys/nginx  \
    --http-client-body-temp-path=/var/lib/nginx/tmp/client_body \
    --http-proxy-temp-path=/var/lib/nginx/tmp/proxy \
    --http-fastcgi-temp-path=/var/lib/nginx/tmp/fastcgi \
    --http-uwsgi-temp-path=/var/lib/nginx/tmp/uwsgi \
    --http-scgi-temp-path=/var/lib/nginx/tmp/scgi  \

    --with-file-aio  \                            ← 依賴libaio庫
    --with-ipv6  \                                ← 支持IPV6
    --with-http_ssl_module  \                     ← 使用https協議模塊
    --with-http_spdy_module  \                    ← Google的SPDY協議支持

    --with-pcre \                                 ← 支持地址重寫
    --with-pcre-jit \
    --with-google_perftools_module \              ← 依賴Google Perf工具，沒有研究過
    --with-debug \                                ← 添加調試日誌

    --add-module=/path/to/echo-nginx-module       ← 編譯添加三方模塊

    --with-ld-opt='-Wl,-E'
{% endhighlight %}

<!--
    --with-http_realip_module \
    --with-http_addition_module \
    --with-http_xslt_module \                     # 過濾轉換XML請求，需要libxslt
    --with-http_image_filter_module \             # 傳輸jpeg/gif/png圖片的過濾器，依賴gd庫
    --with-http_geoip_module  \                   # 依賴GeoIP，沒有使用過
    --with-http_sub_module  \
    --with-http_dav_module  \
    --with-http_flv_module  \
    --with-http_mp4_module  \
    --with-http_gunzip_module  \
    --with-http_gzip_static_module \
    --with-http_random_index_module \
    --with-http_secure_link_module \
    --with-http_degradation_module \
    --with-http_stub_status_module \
    --with-http_perl_module \
    --with-mail \
    --with-mail_ssl_module \
    --with-pcre \
    --with-pcre-jit \
    --with-google_perftools_module \     # 依賴Google Perf工具，沒有研究過
    --with-debug \

    --with-cc-opt='-O2 -g -pipe -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4 -m64 -mtune=generic' \
-->

<!--
CentOS7 編譯選項
nginx version: nginx/1.8.0
built by gcc 4.8.5 20150623 (Red Hat 4.8.5-4) (GCC)
built with OpenSSL 1.0.1e-fips 11 Feb 2013
TLS SNI support enabled
configure arguments: --prefix=/usr/share/nginx --sbin-path=/usr/sbin/nginx --conf-path=/etc/nginx/nginx.conf --error-log-path=/var/log/nginx/error.log --http-log-path=/var/log/nginx/access.log --pid-path=/run/nginx.pid --user=nginx --group=nginx --lock-path=/run/lock/subsys/nginx --http-client-body-temp-path=/var/lib/nginx/tmp/client_body --http-proxy-temp-path=/var/lib/nginx/tmp/proxy --http-fastcgi-temp-path=/var/lib/nginx/tmp/fastcgi --http-uwsgi-temp-path=/var/lib/nginx/tmp/uwsgi --http-scgi-temp-path=/var/lib/nginx/tmp/scgi --with-file-aio --with-ipv6 --with-http_ssl_module --with-http_spdy_module --with-pcre --with-pcre-jit --with-debug --add-module=/home/andy/Workspace/webserver/nginx/modules/echo-nginx-module-0.60 --with-cc-opt='-O2 -g -pipe -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4 -m64 -mtune=generic' --with-ld-opt=-Wl,-E
-->

nginx 沒有通過 autotools 進行編譯，而是通過自己的 shell 腳本實現，其中相應的腳本都在 auto 目錄下，其中編譯生成的文件都保存在 objs 目錄下。

編譯完成之後，可以通過如下命令安裝即可。

{% highlight text %}
# make install
{% endhighlight %}


#### 4. 啟動測試

源碼編譯完成之後，可以直接通過如下方式啟動。

{% highlight text %}
# /usr/sbin/nginx
{% endhighlight %}


#### 5. systemd 支持

對於新一代的 init 支持，需要添加如下文件。

{% highlight text %}
$ cat /usr/lib/systemd/system/nginx.service
[Unit]
Description=The nginx HTTP and reverse proxy server
After=network.target remote-fs.target nss-lookup.target

[Service]
Type=forking
PIDFile=/run/nginx.pid
# Nginx will fail to start if /run/nginx.pid already exists but has the wrong
# SELinux context. This might happen when running `nginx -t` from the cmdline.
# https://bugzilla.redhat.com/show_bug.cgi?id=1268621
ExecStartPre=/usr/bin/rm -f /run/nginx.pid
ExecStartPre=/usr/sbin/nginx -t
ExecStart=/usr/sbin/nginx
ExecReload=/bin/kill -s HUP $MAINPID
KillSignal=SIGQUIT
TimeoutStopSec=5
KillMode=process
PrivateTmp=true

[Install]
WantedBy=multi-user.target
{% endhighlight %}

然後就可以通過如下方式進行控制了。

{% highlight text %}
$ systemctl [start|reload|restart|stop] nginx.service
$ journalctl -f -u nginx.service                            ← 查看日誌
{% endhighlight %}



### 環境

配置完 Nginx 服務之後，實際上會在設置的 root 目錄下添加符號 接，從而可以針對不同的運行環境，如 Bootstrap、echarts、php、DVWA，可以直接通過如下方式設置：

{% highlight text %}
basic:     只包括了基本的測試。
    ln -s basic html

bootstrap: 一個不錯的前端框架。
    ln -s ../bootstrap/bootstrap-3.3.5-dist html

echarts:
    cd echarts-roots
    ln -s ../../echarts/echarts.min.js echarts.js
    cd ..
    ln -s echarts-root html

php-root:  PHP的安裝環境
    ln -s php-root html

DVWA:      一個安全測試環境
    ln -s /home/andy/Workspace/security/dvwa/DVWA-1.9 html
{% endhighlight %}

## 變量

Nginx 配置文件的使用就像是一門微型的編程語言，既然如此，肯定不會少 “變量”，其支持內建變量，可以直接查看源碼 ```http/ngx_http_variables.c``` 中 ```ngx_http_core_variables[]``` 的實現。

如下，使用了 ```ngx_rewrite``` 標準模塊的 ```set``` 配置指令對變量 ```$foobar``` 進行了賦值操作，也即把字符串 hello world 賦給了它；如下需要依賴 echo 模塊，詳細安裝方法參考下面的內容。

{% highlight text %}
location /hello {
    set $foobar "hello world";
    echo "foobar: $foobar";
}
{% endhighlight %}

然後使用類似如下的 curl 命令測試即可。

{% highlight text %}
$ curl 'http://localhost:80/hello'
{% endhighlight %}

### 內置變量

與 Apache 類似，在 core module 中支持一些內置的變量，常見的如下：

* $arg_PARAMETER<br>GET 請求的 PARAMETER 值。
* $args<br>請求行中的參數。
* $query_string<br>與 $args 相同。
* $binary_remote_addr<br>二進制碼形式的客戶端地址。
* $body_bytes_sent<br>發送頁面的字節數。
* $content_length<br>請求頭中的 Content-length 字段。
* $content_type<br>請求頭中的 Content-Type 字段。
* $cookie_COOKIE<br> COOKIE 的值。
* $document_root<br>當前請求在 root 指令中指定的值。
* $document_uri<br>與 $uri 相同。
* $uri<br>請求中的當前URI，不含請求參數，參數位於$args，如 "/foo/bar.html" 。
* $request_uri<br>包含客戶端請求參數的原始 URI，無法修改，可以查看 $uri 改寫。
* $is_args<br>如果 $args 設置，值為 "?"，否則為 ""。
* $limit_rate<br>可以限制連接速率。
* $nginx_version<br>當前運行的 nginx 版本號。
* $remote_addr<br>客戶端的 IP 地址。
* $remote_port<br>客戶端的端口。
* $remote_user<br>已經經過 Auth Basic Module 驗證的用戶名。
* $request_filename<br>當前連接請求的文件路徑，由 root 或 alias 指令與 URI 請求生成。
* $request_body<br>包含請求的主要信息，在 proxy_pass、fastcgi_pass 中比較有意義。
* $request_method<br>客戶端請求的動作，通常為 GET 或 POST。
* $server_addr<br>服務器地址。
* $server_name<br>服務器名稱。
* $server_port<br>請求到達服務器的端口號。
* $server_protocol<br>請求使用的協議，通常是 HTTP/1.0 或 HTTP/1.1。

## 配置文件

### server_name

根據 HTTP 請求的 header Host 選擇 nginx 配置文件裡符合條件的 server_name 的 server 配置，也就是說一個配置文件裡可以配置多個不同域名的服務。

其匹配的順序如下： 1) 完全匹配 (www.example.com)；2) 後綴匹配 (\*.example.com)；3) 前綴匹配 (www.example.\*)；4) 正則匹配。

若前面四項都沒匹配上，則根據以下順序：1) listen 指令裡配置了 default 或 default_server 的 server；2) 第一個匹配上 listen 的 server。



{% highlight text %}
server {
    listen 8000;
    server_name www;
    location / {
        echo "first";
    }
}

server {
    listen  8000;
    server_name www.example.com;
    location / {
        echo "second";
    }
}

server {
    listen 8000;
    server_name www.example.*;
    location / {
        echo "third";
    }
}

server {
    listen 8000;
    server_name ~\w+.com;
    location / {
        echo "forth";
    }
}

server {
    listen 8000;
    server_name ~.*example.com;
    location / {
        echo "fifth";
    }
}
{% endhighlight %}

對於如上的示例，可以通過如下命令進行測試，當然為了測試需要在 /etc/hosts 配置文件中添加如下內容。

{% highlight text %}
127.1 www www.example.com www.example.org www.foobar.com example.com example.org
{% endhighlight %}

注意 fifth 實際與 forth 衝突，不會出現 fifth 的。

{% highlight text %}
$ curl http://www:8000/                 ← 全匹配
first
$ curl http://www.example.com:8000/     ← 全匹配
second
$ curl http://www.example.org:8000/     ← 前綴匹配
third
$ curl http://example.com:8000/         ← 第一個正則匹配
forth
$ curl http://www.foobar.com:8000/      ← 同上
forth
$ curl http://example.org/              ← 無匹配，返回第一個符合的listen的
first
{% endhighlight %}

如果在配置文件裡再加入一個配置。

{% highlight text %}
server {
    listen 8000 default;
    server_name _;

    location / {
        echo "sixth";
    }
}
{% endhighlight %}

則訪問 http://example.org/ 返回 sixth 。

### accept_mutex

簡單來說，這個參數主要用來處理驚群問題，可以通過如下方式配置。

{% highlight text %}
events { 
	accept_mutex off; 
} 
{% endhighlight %}

當新連接到達時，如果激活了 `accept_mutex`，那麼多個 worker 將以串行方式來處理，只有一個會被喚醒，其它的 worker 繼續保持休眠狀態；如果沒有激活 `accept_mutex`，那麼所有的 worker 都被喚醒，不過只有一個 worker 能獲取新連接，其它的 worker 會重新進入休眠狀態。

不過不像 Apache 會啟動成百上千的進程，如果發生驚群問題的話，影響相對較大；但是對 Nginx 而言，一般來說，`worker_processes` 會設置成 CPU 個數，所以最多也就幾十個，即便發生驚群問題的話，影響相對也較小。

另外，高版本的 Linux 中，accept 不存在驚群問題，但是 epoll_wait 等操作還有。

如果關閉了它，可能會引起一定程度的驚群問題，表現為上下文切換增多 (`sar -w`) 或者負載上升，但是如果網站訪問量比較大，為了系統的吞吐量，建議關閉。

關閉之後一般各個工作進程的負載會更均衡，可以通過 [github ngx-req-distr](https://github.com/openresty/openresty-systemtap-toolkit#ngx-req-distr) 測試。

## 常用模塊

安裝完 Nginx 之後，可以通過 -V 參數，查看編譯時使用的參數；很多三方的模塊可以參考 [NGINX 3<sup>rd</sup> Party Modules](https://www.nginx.com/resources/wiki/modules/) ，保存了一個比較詳細的列表。

對於三方模塊，可以通過如下方法添加到二進制文件中。

{% highlight text %}
$ ./configure \
   --add-module=/path/to/echo-nginx-module
{% endhighlight %}

### echo

用來顯示一些常見的變量，不過這個模塊不包含在 Nginx 源碼中，可以通過如下方法進行安裝；首先從 [github openresty/echo-nginx-module](https://github.com/openresty/echo-nginx-module) 。

可以下載之後通過如下方式編譯。

{% highlight text %}
$ ./configure --add-module=/path/to/echo-nginx-module
{% endhighlight %}

簡單的可以通過如下方式使用，也可以從官網查看詳細的使用方法。

{% highlight text %}
location /hello {
  echo "hello, world!";
}
{% endhighlight %}

然後執行如下命令測試即可。

{% highlight text %}
$ curl 'http://localhost:80/hello'
{% endhighlight %}


### ngx_http_map_module

該模塊可以用來創建變量，將這些變量的值與其它變量相關聯，允許分類或者同時映射多個值到多個不同值並儲存到一個變量中，而且僅在變量被接受的時候執行視圖映射操作，對於處理沒有引用變量的請求時，這個模塊並沒有性能上的缺失。

詳細可以參考 Nginx 官方文檔 [Module ngx_http_map_module](http://nginx.org/en/docs/http/ngx_http_map_module.html) 。

### auth_basic_module

關於該模塊的內容詳細可以查看 [Nginx 基本認證模塊](http://nginx.org/en/docs/http/ngx_http_auth_basic_module.html) ，使用簡單介紹如下。

需要保證已經安裝了 ```ngx_http_auth_basic_module``` 模塊，同樣可以通過 ```nginx -V``` 查看，如果不需要可以使用 ```--without-http_auth_basic_module``` 。

{% highlight text %}
location / {
    auth_basic           "closed site";
    auth_basic_user_file conf/htpasswd;
}
{% endhighlight %}

其中 ```auth_basic``` 的字符串內容可以任意，例如 ```Restricted```、```NginxStatus``` 等；另外，需要注意的是 ```auth_basic_user_file``` 的路徑，否則會出現 403 錯誤。

其中密碼文件的格式內容如下，其中密碼通過 ```crypt()``` 函數加密，以及 Apache 基於 MD5 的變種 Hash 算法 (apr1) 生成，可以通過 Apache 中的 ```httpd-tools``` 中的 ```htpasswd``` 加密，或者使用 ```openssl passwd``` 獲取。

{% highlight text %}
# comment
name1:password1
name2:password2:comment
name3:password3
{% endhighlight %}

可以通過如下命令生成密碼。

{% highlight text %}
$ sh -c "openssl passwd -apr1 >> /etc/nginx/.htpasswd"
$ printf "ttlsa:$(openssl passwd -crypt 123456)\n" >>conf/htpasswd
$ htpasswd -c conf/htpasswd username
{% endhighlight %}

另外，可以查看 Python 版本 [github htpasswd.py]( {{ site.example_repository }}/python/misc/htpasswd.py )，使用方法如下。

{% highlight text %}
$ htpasswd.py -b -c pass.txt admin 123456
{% endhighlight %}

<!--
http://coderschool.cn/2207.html
http://trac.edgewall.org/export/10890/trunk/contrib/htpasswd.py
-->




## 其它

記錄一些常見的問題，以及解決方法。

### 管理腳本

一個 Bash 管理腳本。

{% highlight bash %}
#!/bin/bash
# chkconfig: - 30 21
# description: http service.
# Source Function Library
. /etc/init.d/functions
# Nginx Settings
#the shell is used as a tool that start, stop, restart the servie nginx

NGINX_SBIN="/usr/local/nginx/sbin/nginx"
NGINX_CONF="/usr/local/nginx/conf/nginx.conf"
NGINX_PID="/usr/local/nginx/logs/nginx.pid"
RETVAL=0
prog="Nginx"

start() {
        echo -n $"Starting $prog: "
        mkdir -p /dev/shm/nginx_temp
        daemon $NGINX_SBIN -c $NGINX_CONF
        RETVAL=$?
        echo
        return $RETVAL
}

stop() {
        echo -n $"Stopping $prog: "
        killproc -p $NGINX_PID $NGINX_SBIN -TERM
        rm -rf /dev/shm/nginx_temp
        RETVAL=$?
        echo
        return $RETVAL
}

reload(){
        echo -n $"Reloading $prog: "
        killproc -p $NGINX_PID $NGINX_SBIN -HUP
        RETVAL=$?
        echo
        return $RETVAL
}

restart(){
        stop
        start
}

configtest(){
    $NGINX_SBIN -c $NGINX_CONF -t
    return 0
}

case "$1" in
  start)
        start
        ;;
  stop)
        stop
        ;;
  reload)
        reload
        ;;
  restart)
        restart
        ;;
  configtest)
        configtest
        ;;
  *)
        echo $"Usage: $0 {start|stop|reload|restart|configtest}"
        RETVAL=1
esac

exit $RETVAL
{% endhighlight %}

### 監控 nginx 的工作狀態

監控需要在編譯時添加 ```--with-http_stub_status_module``` 選項，默認不會包含在內；然後在配置文件中添加如下內容。

{% highlight text %}
location /status {
    stub_status on;
    access_log off;
    allow 127.0.0.1;
    deny all;
}
{% endhighlight %}

修改完之後重新加載，然後可以通過 http://localhost/status 查看，此時會顯示如下信息。

{% highlight text %}
Active connections: 557             # 對後端發起的活動連接數
server accepts handled requests     # 總共處理的鏈接數；成功握手次數；總共處理的請求
35 35 65
# 讀取到客戶端Header信息數；返回給客戶端的Header信息數；等待下次請求的駐留連接
Reading: 3 Writing: 16 Waiting: 8
{% endhighlight %}

注意，對於最後一行的 Waiting，當開啟 keep-alive 的情況下，這個值等於 active - (reading + writing)，意思就是說已經處理完正在等候下一次請求指令的駐留連接。

### 啟動報錯

記錄下遇到的一些常見問題。

#### Can't load XXX

例如報錯為 ```Can't load '/usr/local/lib64/perl5/auto/nginx/nginx.so'```，該報錯是在使用 perl 模塊時可能引入的報錯，也就是編譯時使用 ```--with-http_perl_module``` 參數。

{% highlight text %}
# /usr/sbin/nginx
Can't load '/usr/local/lib64/perl5/auto/nginx/nginx.so' for module nginx:
    /usr/local/lib64/perl5/auto/nginx/nginx.so: undefined symbol:
    ngx_http_perl_handle_request at /usr/share/perl5/XSLoader.pm line 68.
at /usr/local/lib64/perl5/nginx.pm line 56.
Compilation failed in require.
BEGIN failed–compilation aborted.
nginx: [alert] perl_parse() failed: 255
{% endhighlight %}

此時，需要在編譯的時候添加一個連接選項。

{% highlight text %}
$ ./configure --with-http_perl_module --with-ld-opt="-Wl,-E"
{% endhighlight %}

然後，重新安裝即可。


### 403 Forbidden

可以通過查看 error 日誌，通常是由於權限不足導致，最簡單的方式可以嘗試在配置文件中設置 user root;，也就是 nginx 的子進程採用 root 用戶。

當然，通常也可以使用 ```chown nginx:nginx -R /var/www``` 解決。

<!--
如果 root 在用戶目錄下，可以通過如下命令允許 nginx 訪問用戶文件，否則會報 403 Forbidden 錯誤。

{% highlight text %}
# setsebool -P httpd_read_user_content 1
# setsebool -P httpd_enable_homedirs 1
{% endhighlight %}
-->



<!--
## SSL 支持

### SPDY

普通的 https 慢而且又吃資源，Google 發明的 SPDY 協議。

{% highlight text %}
listen [::]:443 ssl;
listen 443 ssl;

listen [::]:443 ssl spdy;
listen 443 ssl spdy;
{% endhighlight %}
-->



## 參考

可以參考官方文檔 [nginx.org docs](http://nginx.org/en/docs/) 。

<!--
NGINX引入線程池 性能提升9倍
http://www.infoq.com/cn/articles/thread-pools-boost-performance-9x
-->




{% highlight text %}
{% endhighlight %}
