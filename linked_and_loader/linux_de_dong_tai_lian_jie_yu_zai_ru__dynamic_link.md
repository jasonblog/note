# Linux 的動態連結與載入 (Dynamic Linking)

##簡介

傳統的連結器會將所有程式連結成單一的執行檔，在執行時只需要該執行檔就能順利執行。但是，使用動態連結機制時，函式庫可以先不需要被連結進來，而是在需要的時候才透過動態連結器 (Dynamic Linker) 尋找並連結函式庫，這種方式可以不用載入全部的程式，因此可以節省記憶體。當很多執行中的程式共用到函式庫時，動態連結所節省的記憶體數量就相當可觀。

除了節省記憶體之外，動態連結技術還可以節省編譯、組譯、連結所花費的時間。這是因為動態連結函式庫 (Dynamic Linking Libraries: DLLs) 可以單獨被編譯、組譯與連結，程式設計師不需要在改了某個函式庫後就重新連結所有程式。因此，對程式開發人員而言，動態連結技術可以節省程式開發的時間，因為程式設計人員使用編譯、組譯與連結器的次數往往非常頻繁，有些人甚至不到一分鐘就會編譯一次。

除此之外，動態連結函式庫由於可以單獨重新編譯，因此，一但編譯完新版本後，就可以直接取代舊版本。這讓舊程式得以不需重新編譯就可以連結到新函式庫，因此，只要我們將動態連結函式庫換掉，即使功能不完全相同，只要函式庫名稱一樣，舊程式仍可順利執行該新版的函數，這讓動態函式庫變成可任意抽換的。這種可抽換性對程式開發人員而言，同時具有優點與缺點。

動態連結器的任務，就是在需要的時候才載入動態函式庫，並且進行連結 (linking) 與重新定位 (relocation) 的動作。然後再執行該函式庫中的函數。

當程式第一次執行到動態函數時，動態連結器會搜尋看看該函數是否已經在記憶體中，如果有則會跳到該函數執行，如果沒有則會呼叫載入器，動態的將該函式庫載入到記憶體，然後才執行該函數。這種函式庫被稱為動態連結函式庫 (Dynamic Linking Library)，在 MS. Windows 中，這種函式庫的附檔名為 .dll ，而在 Linux 中，這種函式庫的附檔名通常為 .so (Shared Object)。

使用動態連結機制呼叫函數時，通常會利用間接跳轉的方式，先跳入一個稱為 Stub 的程式中，然後在第一次呼叫時，該Stub 會呼叫動態載入器載入該函數，而在第二次以後，則會直接跳入該函數。


![](./images/LinuxDLLstub.jpg)

圖 1 動態連結機制的實作方式

圖 1 所顯示的動態跳轉機制，其關鍵是利用動態連函數區 (Stub) 作為跳轉點。在主程式中，呼叫動態函數是透過 Stub 區中的 f1, f2, f3 等函數標記，但是，這些標記區域包含了一段段的Stub小程式，這些小程式會決定是要直接跳轉，或者是呼叫動態載入器。

在程式剛載入之時，Ptr_f1,Ptr_f2,Ptr_f3 等用來儲存動態函數位址的變數，會被填入 DL_f1, DL_f2, DL_f3 等位址。當主程式執行 CALL f2@PLT 指令時，會跳到Stub區的 f2 標記，此時，會執行 LD PC, Ptr_f2@GOT 這個指令，但是由於 Ptr_f2 當中儲存的是 DL_f2 的位址，因此，該LD跳轉指令相當於沒有作用，於是會繼續呼叫動態連結器 (Dlinker) 去載入 f2 對應的函數到記憶體中，f2_in_memory 顯示了載入完成後的動態函數。

一但 f2 的動態函數f2_in_memory被載入後，Dlinker 會將 f2_in_memory 填入到 Ptr_f2 當中。於是，當下一次主程式再呼叫 CALL f2@PLT 時，Stub 區中的 LD PC, Ptr_f2@GOT 就會直接跳入動態函數 f2_in_memory 中，而不會再透過載入器了。

##動態連結的優缺點

動態連結函式庫通常是與位置無關的程式碼 (Position Independent Code)，使用相對定址的方式。否則，如果在動態連結時利用修改記錄修正函式庫的記憶體內容，會造成每個程式都在修正函式庫，就可能造成不一致的窘境。

動態函式庫的優點是可任意抽換，不需刻意更新舊程式，系統隨時保持最新狀態。但是，這也可能造成『動態連結地獄』 (DLL hell) 的困境，因為，如果新的函式庫有錯，或者與舊的主程式不相容，那麼，原本執行正常的程式會突然出現錯誤，甚至無法使用。

一但有了『動態連結技術』，就能很容易的實作出『動態載入技術』。所位的動態載入技術，是在程式中再決定要載入哪些函數的方法。舉例而言，我們可以讓使用者在程式中輸入某個參數，然後立刻用『動態載入技術』載入該函式庫執行。這會使得程式具有較大的彈性，因為，我們可以在必要的時候呼叫動態載入器，讓使用者決定要載入哪些函式庫。

必須提醒讀者的是，雖然動態連結 已經是相當常見的功能，但是在UNIX/Linux 與 Windows中卻有不同的稱呼，在 Windows 中直接稱為 DLLs (Dynamic Linking Libraries)，其附檔名通常為 .dll，而在UNIX/Linux 中的動態連結函式庫則被稱為 Share Objects，其附檔名通常是 .so。

##動態載入技術

動態連結雖然不需要事先載入函式庫，但是在編譯時就已經知道動態函數的名稱與參數類型，因此，編譯器可以事先檢查函數型態的相容性。但是，有一種稱為動態載入的技術，允許程式設計人員在程式執行的過程中，動態決定要載入哪個函式庫，要執行哪個函數，這種技術比動態連結更具有彈性，靈活度也更高。其方法是讓程式可以呼叫載入器，以便動態的載入程式，因此才被稱為動態載入技術 。

舉例而言，Linux 當中的系統呼叫 execve()，就是動態載入技術的一個簡單範例。當我們呼叫 execve() 以載入程式時，就是利用了載入器將某個程式載入記憶體當中執行。範例 1 就顯示了一個使用 execve() 載入 ls 檔案的程式，該程式會顯示etc資料夾中passwd的檔案屬性。

範例 1. 使用 execve 呼叫載入器的範例

```c
#include <unistd.h>
int main()
{
    char* argv[] = {"ls", "-al", "/etc/passwd", (char*)0};
    char* envp[] = {"PATH=/bin", 0};
    execve("/bin/ls", argv, envp);
}
```

然而，範例 1 只是一個簡單的動態載入功能，execve 函數能做到的功能相當的有限，如果我們使用 UNIX/Linux 中的 libdl.so 這的函式庫，那可以做到較為先進的動態載入功能，像是連續呼叫同一個函式庫，或取得函式庫中的變數值等等。

表格 1 顯示了這兩個平台的動態載入函式庫對照表。在 UNIX/Linux 當中，『dl』 函式庫可支援動態載入功能，其引用檔為 dlfcn.h，函式庫的目的檔為 libdl.so，可以用 dlopen() 載入動態函式庫，然後用 dlsym() 取得函數指標，最後用 dlclose() 函數關閉函式庫。

而在 MS. Windows 當中，動態函式庫直接內建在核心當中，其引用檔為 windows.h，動態連結的目的檔為 Kernel32.dll，可以使用 LoadLibrary() 與 LoadLibraryEx() 等函數載入動態函式庫，然後用 GetProcAddress 取得函數位址，最後用 FreeLibrary() 關閉動態函式庫。

表格 1 Linux 與 MS. Windows 中動態載入函式庫的對照比較表

<table class="wiki-content-table">
<tbody><tr>
<td>使用方法</td>
<td>~ UNIX/Linux</td>
<td>~Windows</td>
</tr>
<tr>
<td>引入檔</td>
<td>#include &lt;dlfcn.h&gt;</td>
<td>#include &lt;windows.h&gt;</td>
</tr>
<tr>
<td>函式庫檔</td>
<td>libdl.so</td>
<td>Kernel32.dll</td>
</tr>
<tr>
<td>載入功能</td>
<td>dlopen</td>
<td>LoadLibrary, LoadLibraryEx</td>
</tr>
<tr>
<td>取得函數</td>
<td>dlsym</td>
<td>GetProcAddress</td>
</tr>
<tr>
<td>關閉功能</td>
<td>dlclose</td>
<td>FreeLibrary</td>
</tr>
</tbody></table>

範例 2. Linux 動態載入函式庫的使用範例

```c
// 程式：dlcall.c , 編譯指令：gcc -o dl_call dl_call.c –ldl
#include <dlfcn.h>  // 引用dlfcn.h動態函式庫

int main(void)
{
    void* handle = dlopen("libm.so", RTLD_LAZY);        // 開啟 shared library 'libm'
    double (*cosine)( double);                           // 宣告cos()函數的變數
    cosine = dlsym(handle, "cos");                      // 找出cos()函數的記憶體位址
    printf("%f\n", (*cosine)(2.0));                     // 呼叫cos()函數
    dlclose(handle);                                    // 關閉函式庫

    return 0;
}
```

為了支援『動態連結』與『動態載入』的功能，動態函式庫的目的檔當中，通常不會使用絕對定址等方式，而會使用與位置無關的編碼方式(Position-independent code)，對於支援動態重定位 (具有虛擬位址) 的機器而言，可以使用基底暫存器 (base register) 作為定址基底，然後利用基底定址法 (base addressing mode)達成與位置無關的編碼方式。透過適當的分段，以及虛擬位址技術，還可以保護這些區段不被竄改，由於虛擬位址的主題與作業系統的設計密切相關，有興趣者請進一步閱讀作業系統的主題。