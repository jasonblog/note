/*
用法：echo 參數1 參數2 ...
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/*argc，代表在呼叫這個執行檔的時候，總共有幾個參數*/
/*請注意，第一個參數一定是執行檔的檔名，這樣的設計，有助於除錯*/
/*例如在pipe中（後面張婕蕙教授），可以將檔名當成輸出資料的一部份*/
/*argv則是字串陣列，最後一個字串是NULL*/

int main(int argc, char**argv) {
    int i=0;
    
    while(argv[i]!=NULL) {
        printf("%s\n", argv[i]);
        i++;
        
    }
    
    /*依照程式的屬性，設定適當的return value*/
    return 1;
}

