# 使用call命令在GDB中重複調用某函數


在白盒測試中經常使用GDB進行函數的分支覆蓋測試，但在測試對象函數觸發很困難，測試效率就很低下。
假設測試函數fun1有10條分支。每次進入fun1需設置10個變量。
那麼一般情況下要在GDB中操作10 * 10 = 100次才能將該分支覆蓋完畢。

經過查找，GDB中存在一種方法，重複調用該函數，使用10+10 =20次即可覆蓋分支。

GDB使用步驟：
###1）首先對該函數打斷點
有以下函數：

```c
int webprc_cmmenu_lchk(WEB_CMMENU_LAN_PATH_CHK_STATUS *result, int filetype,CM_COMMON_INFO *cominf, WEB_FLAG_TYPE type, char *errmsg)
```

###2)第一次，手動進入該函數
GDB將顯示以下msg：

```sh
Breakpoint 2, webprc_cmmenu_lchk (result=0x1006cc8c, filetype=60, cominf=0x48197008, type=0 '\000',
    errmsg=0x10087f30 "\302\302\302\302\302\302"...) at webcmmenu.c:212
212         webprc_lock(WEB_SEM_LOCK);
(gdb) n
213         result->status.status.size = sizeof(WEB_CMMENU_LAN_PATH_CHK_STATUS);   
(gdb) n
214         result->run_status = WEBSTS_CMMENU1_LANPATCH_CHK;                    
```

在前兩行中顯示了在進入webprc_cmmenu_lchk時各個參數的值：

```sh
result = 0x1006cc8c
filetype = 60
cominf=0x48197008
type=0
errmsg = 0x10087f30
```

### 3）繼續執行，執行完函數時，`即到達函數的結尾「}」時，不要退出該函數！`

```c

call webprc_cmmenu_lchk(result=0x1006cc8c, filetype=60, cominf=0x48197008, type=0,errmsg=0x10087f30)
```

GDB將輸出以下log：


```sh
The program being debugged stopped while in a function called from GDB.
Evaluation of the expression containing the function
(webprc_cmmenu_lchk) will be abandoned.
When the function is done executing, GDB will silently stop.
```

此時即意味著webprc_cmmenu_lchk函數又重新執行，進入參數與上一次執行相同。

原理：
經常用到GDB中call命令對字符串進行賦值操作，大多數都是調用系統庫函數。
而在調用項目中的函數時，只要確定進入函數時的寄存器狀態，自然可以重複調用該函數了。

