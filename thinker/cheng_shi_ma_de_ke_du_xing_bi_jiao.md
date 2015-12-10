# 程式碼的可讀性比較


以下就程式碼的可讀性即其產生之 binary 進行分析。依其順序，逐漸改進其可讀性。透過比較，我們能較透澈的了解可讀性的性質。
下面的程式，是一個分析 HTTP request line 的 parser 。 Request line 的格式如下
```c
GET /path/to/resource HTTP/1.1
```
程式的目的是將 request line 裡的三個欄位，分別取出為 method 、 uri 和 protocol 。

程式一

最平舖直訴的方式，這大概是每個 programmer 都會經過的階段吧!!
```c
001	#include <stdio.h>
002	#include <string.h>
003	#include <stdlib.h>
004	
005	typedef struct {
006	    char *method;
007	    char *uri;
008	    char *proto;
009	} http_req;
010	
011	int http_req_parse(http_req *req, const char *buf, int sz) {
012	    int i, prev;
013	    
014	    for(i = 0; i < sz; i++) {
015	        if(buf[i] == ' ') break;
016	        if(buf[i] == '\n' || buf[i] == '\r')
017	            return -1;
018	    }
019	    if(i == sz || i == 0) return -1;
020	    req->method = (char *)malloc(i + 1);
021	    strncpy(req->method, buf, i);
022	    req->method[i] = 0;
023	    
024	    prev = ++i;
025	    for(; i < sz; i++) {
026	        if(buf[i] == ' ') break;
027	        if(buf[i] == '\n' || buf[i] == '\r') break;
028	    }
029	    if(i == sz || i == prev || buf[i] != ' ') {
030	        free(req->method);
031	        return -1;
032	    }
033	    req->uri = (char *)malloc(i - prev + 1);
034	    strncpy(req->uri, buf + prev, i - prev);
035	    req->uri[i - prev] = 0;
036	    
037	    prev = ++i;
038	    for(; i < sz; i++) {
039	        if(buf[i] == ' ') break;
040	        if(buf[i] == '\n' || buf[i] == '\r') break;
041	    }
042	    if(i != sz || i == prev) {
043	        free(req->method);
044	        free(req->uri);
045	        return -1;
046	    }
047	    req->proto = (char *)malloc(i - prev + 1);
048	    strncpy(req->proto, buf + prev, i - prev);
049	    req->proto[i - prev] = 0;
050	    
051	    return 0;
052	}
053	
054	int main(int argc, const char *argv[]) {
055	    const char *data = "GET /test.html HTTP/1.1";
056	    http_req req;
057	    
058	    if(http_req_parse(&req, data, strlen(data)) < 0) {
059	        fprintf(stderr, "error to parse request line!\n");
060	        return 1;
061	    }
062	    
063	    printf("request line: %s\n", data);
064	    printf("method: %s\n", req.method);
065	    printf("uri: %s\n", req.uri);
066	    printf("protocol: %s\n", req.proto);
067	    
068	    return 0;
069	}
```
##重複的動作

將不斷重複的動作取出，並定個有意義的名稱。如此程式不但變小，而且因為 function 名稱所帶來的意涵，加強了程式的可讀性。由此可知，將程式的部分流程變成 function ，並附于適當的名稱，能改善程式的可讀性。

strncspn() 其實就是 strcspn() 的變形，可看 Linux 或 FreeBSD 的 man page 。而 strndup() 就是 strdup() 的變形。透過這些有意義的名稱，程式碼的流程更有意義，提供更多線索，更適合大腦解讀。
```c
001	#include <stdio.h>
002	#include <string.h>
003	#include <stdlib.h>
004	
005	typedef struct {
006	    char *method;
007	    char *uri;
008	    char *proto;
009	} http_req;
010	
011	int strncspn(const char *s, int max, const char *charset) {
012	    int i, j, cs_sz;
013	    char c;
014	    
015	    cs_sz = strlen(charset);
016	    for(i = 0; i < max && s[i] != 0; i++) {
017	        c = s[i];
018	        for(j = 0; j < cs_sz; j++) {
019	            if(c == charset[j]) return i;
020	        }
021	    }
022	    return max;
023	}
024	
025	char *strndup(const char *s, int max) {
026	    int sz = strlen(s);
027	    char *buf;
028	    
029	    if(sz > max) sz = max;
030	    buf = (char *)malloc(sz + 1);
031	    memcpy(buf, s, sz);
032	    buf[sz] = 0;
033	    
034	    return buf;
035	}
036	
037	int http_req_parse(http_req *req, const char *buf, int sz) {
038	    const char *substr, *last, *next;
039	    int substr_sz;
040	    
041	    last = buf + sz;
042	    
043	    substr_sz = strncspn(buf, sz, " \r\n");
044	    if(substr_sz == sz || substr_sz == 0 || buf[substr_sz] != ' ')
045	        return -1;
046	    req->method = strndup(buf, substr_sz);
047	    
048	    substr = buf + substr_sz + 1;
049	    substr_sz = strncspn(substr, last - substr, " \r\n");
050	    next = substr + substr_sz;
051	    if(substr_sz == 0 || next == last || *next != ' ') {
052	        free(req->method);
053	        return -1;
054	    }
055	    req->uri = strndup(substr, substr_sz);
056	    
057	    substr = next + 1;
058	    substr_sz = strncspn(substr, last - substr, " \r\n");
059	    next = substr + substr_sz;
060	    if(next != last) {
061	        free(req->method);
062	        free(req->uri);
063	        return -1;
064	    }
065	    req->proto = strndup(substr, substr_sz);
066	    
067	    return 0;
068	}
069	
070	int main(int argc, const char *argv[]) {
071	    const char *data = "GET /test.html HTTP/1.1";
072	    http_req req;
073	    
074	    if(http_req_parse(&req, data, strlen(data)) < 0) {
075	        fprintf(stderr, "error to parse request line!\n");
076	        return 1;
077	    }
078	    
079	    printf("request line: %s\n", data);
080	    printf("method: %s\n", req.method);
081	    printf("uri: %s\n", req.uri);
082	    printf("protocol: %s\n", req.proto);
083	    
084	    return 0;
085	}
```
##邏輯拆離

前面的程式，將 parse 的過程中的數個邏輯交叉混合在一起。下面的程式將這些邏輯個別分離，變成獨立的區塊。將相關邏輯集中處理，而非交錯在一起，導至讀者必需不斷的在邏輯之間切換。

另一方面，將邏輯拆離，能減少透過 variable 保留和傳遞資訊的狀況。如第一和第二個程式，透過變數 i 和 substr 保留和傳遞目前處理的狀態，以在 function 前後傳遞資訊。這迫使讀者必需追蹤 variable 的內容，才能理解每一段程式碼的作用和正確性。

而邏輯拆離後，條件判斷也減少了。使用條件式，經常是一種邏輯上的修補行為，對意外狀況的處置。然而大部分情況並非不可避免的，只需適當的安排，將邏輯拆離，既可避免這種修補的動作。

而邏輯拆離後，每一個程式碼區塊的功能也單純化。讀者更易理解，程式撰寫時，也更能確定程式的正確性。

下例，一開始先把確定換行符號是否在字串裡，以排除換行的狀況。接著取得空白符號的位置。最後複製字串，並成 method 、 uri 和 proto 的內容。
```c
001	#include <stdio.h>
002	#include <string.h>
003	#include <stdlib.h>
004	
005	typedef struct {
006	    char *method;
007	    char *uri;
008	    char *proto;
009	} http_req;
010	
011	int strncspn(const char *s, int max, const char *charset) {
012	    int i, j, cs_sz;
013	    char c;
014	    
015	    cs_sz = strlen(charset);
016	    for(i = 0; i < max && s[i] != 0; i++) {
017	        c = s[i];
018	        for(j = 0; j < cs_sz; j++) {
019	            if(c == charset[j]) return i;
020	        }
021	    }
022	    return max;
023	}
024	
025	char *strndup(const char *s, int max) {
026	    int sz = strlen(s);
027	    char *buf;
028	    
029	    if(sz > max) sz = max;
030	    buf = (char *)malloc(sz + 1);
031	    memcpy(buf, s, sz);
032	    buf[sz] = 0;
033	    
034	    return buf;
035	}
036	
037	int http_req_parse(http_req *req, const char *buf, int sz) {
038	    const char *substr, *last;
039	    int i;
040	    const char *fss[4];
041	    
042	    sz = strncspn(buf, sz, "\r\n");
043	    last = buf + sz;
044	    
045	    substr = buf;
046	    for(i = 1; i < 4; i++) {
047	        fss[i] = substr + strncspn(substr, last - substr, " ");
048	        if(fss[i] == last) break;
049	        substr = fss[i] + 1;
050	    }
051	    if(i != 3)
052	        return -1;
053	    
054	    fss[0] = buf;
055	    fss[3] = last;
056	    for(i = 0; i < 3; i++) {
057	        if(i > 0) fss[i]++;
058	        if((fss[i + 1] - fss[i]) < 1)
059	            return -1;
060	    }
061	    
062	    req->method = strndup(fss[0], fss[1] - fss[0]);
063	    req->uri = strndup(fss[1], fss[2] - fss[1]);
064	    req->proto = strndup(fss[2], fss[3] - fss[2]);
065	    
066	    return 0;
067	}
068	
069	int main(int argc, const char *argv[]) {
070	    const char *data = "GET /test.html HTTP/1.1";
071	    http_req req;
072	    
073	    if(http_req_parse(&req, data, strlen(data)) < 0) {
074	        fprintf(stderr, "error to parse request line!\n");
075	        return 1;
076	    }
077	    
078	    printf("request line: %s\n", data);
079	    printf("method: %s\n", req.method);
080	    printf("uri: %s\n", req.uri);
081	    printf("protocol: %s\n", req.proto);
082	    
083	    return 0;
084	}
```
##再次簡化

前一個程式將邏輯分離，這個程式將分離後的程式再改進。例如將重用性高的部分再取出，如 strnchrs() 其實就是 strchr 的變形，這樣的 function 功能單純，可重用性高。再加上賦于一個有意義的名稱，增加了程式的可讀性。

另外，把幾個 loop 替換成直接的 statement ，直接的陳述往往比 loop 和 condition 來的易讀。但，如果重複的次數太多時，當然使用 loop 才是合理的狀況。
```c
001	#include <stdio.h>
002	#include <string.h>
003	#include <stdlib.h>
004	
005	typedef struct {
006	    char *method;
007	    char *uri;
008	    char *proto;
009	} http_req;
010	
011	int strncspn(const char *s, int max, const char *charset) {
012	    int i, j, cs_sz;
013	    char c;
014	    
015	    cs_sz = strlen(charset);
016	    for(i = 0; i < max && s[i] != 0; i++) {
017	        c = s[i];
018	        for(j = 0; j < cs_sz; j++) {
019	            if(c == charset[j]) return i;
020	        }
021	    }
022	    return max;
023	}
024	
025	char *strndup(const char *s, int max) {
026	    int sz = strlen(s);
027	    char *buf;
028	    
029	    if(sz > max) sz = max;
030	    buf = (char *)malloc(sz + 1);
031	    memcpy(buf, s, sz);
032	    buf[sz] = 0;
033	    
034	    return buf;
035	}
036	
037	int strnchrs(const char *s, int max, int c, const char *chrs[], int chrs_max) {
038	    int i, j = 0;
039	    
040	    for(i = 0; i < chrs_max; i++, j++) {
041	        for(; j < max; j++)
042	            if(s[j] == c) break;
043	        if(j == max) break;
044	        chrs[i] = s + j;
045	    }
046	    
047	    return i;
048	}
049	
050	int http_req_parse(http_req *req, const char *buf, int sz) {
051	    int i;
052	    const char *last;
053	    const char *fss[3], *starts[3];
054	    
055	    sz = strncspn(buf, sz, "\r\n");
056	    last = buf + sz;
057	    
058	    if(strnchrs(buf, sz, ' ', fss, 3) != 2)
059	        return -1;
060	    
061	    starts[0] = buf;
062	    starts[1] = fss[0] + 1;
063	    starts[2] = fss[1] + 2;
064	    fss[2] = last;
065	    
066	    for(i = 0; i < 3; i++)
067	        if(starts[i] == fss[i]) return -1;
068	    
069	    req->method = strndup(starts[0], fss[0] - starts[0]);
070	    req->uri = strndup(starts[1], fss[1] - starts[1]);
071	    req->proto = strndup(starts[2], fss[2] - starts[2]);
072	    
073	    return 0;
074	}
075	
076	int main(int argc, const char *argv[]) {
077	    const char *data = "GET /test.html HTTP/1.1";
078	    http_req req;
079	    
080	    if(http_req_parse(&req, data, strlen(data)) < 0) {
081	        fprintf(stderr, "error to parse request line!\n");
082	        return 1;
083	    }
084	    
085	    printf("request line: %s\n", data);
086	    printf("method: %s\n", req.method);
087	    printf("uri: %s\n", req.uri);
088	    printf("protocol: %s\n", req.proto);
089	    
090	    return 0;
091	}
```

##行數

表面上看來，改進可讀性之後，程式碼的行數突然大增。但是如果仔細算一下有意義的 statement 數目，其實是減少的。之所以會有大增的表象，是來自於 function 、 變數的宣告 、 呼叫和留白。但這些是否該例入程式的複雜度裡？本人採取否定的態度。我的計算方法，是將每一個 assign statement 算一行，每一個 for 、 if 、 break 、 return 、 continue 算一行。如此
```c
if(...) break;
```
會算兩行。而 variable 和 function 的宣告不列入計算。

仔細算下來，最後一個程式比第一個程式還少上數行。

##執行檔的大小
```c
-rwxr-xr-x  1 thinker  users  6117 May  6 00:06 readability0
-rwxr-xr-x  1 thinker  users  6161 May  5 20:58 readability1
-rwxr-xr-x  1 thinker  users  6064 May  5 21:30 readability2
-rwxr-xr-x  1 thinker  users  6125 May  6 00:06 readability3
```

依序為前面四個程式的大小。第四個程式的成本是區區數個 bytes ，但可讀性卻大幅的改善。另一方面，本程式規模較小，大程式的的重複性更高，增加可讀性可能會使程式比原來更小。程式愈大，其效能愈大。

##結論

程式的可讀性，來自於有意義的名稱、邏輯的分割。而可讀性卻不會增加程式的大小，反而能使程式更精簡、更小、甚至更快。本文並沒有討論到模組的計設和資訊封裝等議題，單就程式流程的可讀性進行討論。