# （七）：統一目標輸出目錄


上一節我們把規則單獨提取出來，方便了Makefile的維護，每個模塊只需要給出關於自己的一些變量，然後再使用統一的規則Makefile。這一節我們繼續改進我們的Makefile，到目前為止我們的Makefile編譯鏈接輸出的目標都在源文件同目錄下或模塊Makefile同一目錄下，當一個項目大了之後，這樣會顯得很亂，尋找編譯輸出的文件也比較困難。既然Makefile本身就是按照我們的的規則來編譯鏈接程序，那麼我們就可以指定其編譯鏈接目標的目錄，這樣，我們可以清楚輸出文件的地方，並且在清除已編譯的目標時直接刪除指定目錄即可，不需要一層一層的進入源代碼目錄進行刪除，這樣又提高了效率。

既然要統一目標輸出目錄，那麼該目錄就需要存在，所以我們可以增加一條規則來創建這些目錄，包括創建可執行文件的目錄、鏈接庫文件的目錄以及.o文件的目錄。並且目錄還可以通過條件判斷根據是否產生調試信息來區分開相應的目標文件。一般一個工程的頂層目錄下都會有一個build目錄來存放編譯的目標文件結果，目前我的工程目錄下通過Makefile創建的目錄build的目錄樹如下：



```sh
build/            //build根目錄  
├── unix        //unix平臺項目下不帶調試信息輸出目錄  
│   ├── bin    //存放可執行文件目錄  
│   ├── lib    //存放可文件目錄  
│   └── obj    //存放.o文件目錄，該目錄下將每個模塊生成的.o文件各自的目錄下面  
│       ├── ipc  
│       ├── main  
│       └── tools  
└── unix_dbg   ////unix平臺項目下帶調試信息輸出目錄  
    ├── bin  
    ├── lib  
    └── obj  
        ├── ipc  
        ├── main  
        └── tools  
  
14 directories, 0 files  
```

以上目錄中bin和lib目錄在頂層Makefile中創建，obj及其下面模塊子目錄在各模塊的Makefile裡面創建。
頂層Makefile創建目錄如下：

```sh
ifeq ($(DEBUG_SYMBOLS), TRUE)  
>---BUILDDIR = ./build/$(PLATFORM)_dbg  
else  
>---BUILDDIR = ./build/$(PLATFORM)  
endif  
  
all : $(BUILDDIR) $(MODULES)  
  
$(BUILDDIR):  
>---@echo "    Create directory $@ ..."  
>---mkdir -p $(BUILDDIR)/bin $(BUILDDIR)/lib
```

我們在all目標裡面增加了其依賴目標BUILDDIR，該目標對應的規則為創建bin目錄和lib目錄。這樣每次編譯之前都會創建目錄。


各模塊內部Makefile創建生成.O文件的目錄，如上目錄樹所示。類似於頂層Makefile，各模塊內部Makefile需要根據平臺、編譯調試信息、以及模塊名稱來生成需要的目錄名稱，然後再增加創建該目錄的規則。因為每個模塊都會做這些處理，所以我們將這部分寫在規則Makefile(Makefile.rule)裡面，如下：


```sh
# define a root build directory base on the platform  
# if without a SRC_BASE defined, just use local src directory  
ifeq ($(SRC_BASE),)  
>---BUILDDIR = $(MOD_SRC_DIR)  
>---OBJDIR = $(MOD_SRC_DIR)  
>---LIBDIR = $(MOD_SRC_DIR)                                                                                                                                  
>---BINDIR = $(MOD_SRC_DIR)  
else  
>---ifeq ($(DEBUG_SYMBOLS), TRUE)  
>--->---BUILDDIR = $(SRC_BASE)/build/$(PLATFORM)_dbg  
>---else  
>--->---BUILDDIR = $(SRC_BASE)/build/$(PLATFORM)  
>---endif  
>---OBJDIR = $(BUILDDIR)/obj/$(MODULE)  
>---LIBDIR = $(BUILDDIR)/lib  
>---BINDIR = $(BUILDDIR)/bin  
endif  
……  
ifeq ($(MAKELEVEL), 0)  
all : msg  
else  
all : lib bin  
endif  
  
lib : $(OBJDIR) $(SRC_LIB)  
  
bin : $(OBJDIR) $(SRC_BIN)                                                                                                                         
  
$(OBJDIR) :  
>---@echo "   MKDIR $(notdir $@)..."  
>---@mkdir -p $@  
```
此時我們編譯一下後查看build目錄：

```sh
build/  
└── unix_dbg  
    ├── bin  
    ├── lib  
    └── obj  
        ├── ipc  
        ├── main  
        └── tools  
  
7 directories, 0 files   
```

由於我們是開啟了調試信息，所以創建了unix_dbg目錄，並且該目錄下創建了bin、lib、obj目錄及其模塊目錄，但我們沒有發現有文件存放在裡面。

到目前為止，這一節僅僅講述如何創建統一的目標文件存放目錄，但是要想將編譯生成的目標文件自動生成到這些目錄還沒有完成。其實我們只需要給目標加上路徑即可，但還是有一些詳細的地方需要處理，具體的我們會在下一節中講到，這一節暫不給出最後的Makefile。



