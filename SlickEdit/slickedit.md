# SlickEdit


![SlickEdit](http://upload.wikimedia.org/wikipedia/en/2/2e/SlickEdit_logo.png)

## 換編輯器的原因
1. Source Insight 不進行大的更新了(今天看, 居然更新了一個小版本, 呵呵, 裝了沒任何變換啊) 經常無響應, 鬱悶死我了
2. Source Insight不能進行定製, 閉源軟件做到這個份上就應該開源啊, 沒有插件機制, 別人給你寫插件還要hack, 用外掛的方式, 想想真噁心啊, 你值得別人這麼對你嗎.

## 我目前對編輯器的需求
1. 源碼工程與源碼目錄分離
2. 有靜態數據庫, 可以快速跳轉
3. 持續更新有保障, 有活力

## Understand vs SlickEdit
* 簡單的搜索後發現這麼兩個還算比較靠譜吧, 真是少啊, 開源軟件怎麼就沒有做大的呢! 開源只有eclipse靠譜了, 可是我總不能用他來看c代碼吧.
* 讓我決定選擇 SlickEdit的原因其實很簡單, 官方社區比較好, 人比較多, 還有一些亮點, V18 beta版本中居然支持 golang贊, 果斷用, 而且要精通.

## SlickEdit
* [破解文件(來自網絡)](http://pan.baidu.com/s/1uxMTF#dir/path=%2Fakshare)
* Editor Emulation: 可以模擬各種編輯器, 如: VIM, Emacs等, 我先選了CUA, 就是跟office word一致的快捷鍵方式.
* Color Scheme: 我選擇了一個墨綠色背景的 Chalkboard, google了一下發現 GitHub Gist上面有一些人自己的設置, 以後可以去參考別人的定製一下.
* Fonts(字體): 還是選擇我習慣的Consolas, 順便用MacType渲染了一下, 弄了下Source Code Pro, 感覺和Consolas差不多(Mac下面沒有consolas頭疼)

## Config
1. 工具欄圖標設置為小(因為工具欄都是最常用的功能實在找不到不用快捷鍵而使用它們的理由)
* Menu: View -> Toolbars -> Customize... -> Options -> Options -> Toolbar button size and spacing -> Small, 勾選 Hide when application is inactive
(Tools -> Options -> Appearance -> Toolbars)

2. 字體: Consolas
* Menu: Tools -> Appearance -> Fonts
consolas, 12

3. 逗號後面自動填充空格
* Menu: Tools -> Options -> Languages -> All Languages -> Context Tagging -> Parameter information -> 勾選Insert space after comma

4. 高亮當前光標下的matching symbols
* Menu: Tools -> Options -> Languages -> All Languages -> Context Tagging -> Parameter information -> 勾選Highlight matching symbols under cursor

5. 顯示行號
* Menu: Tools -> Options -> Languages -> All Languages -> View -> 勾選 Line numbers, Mininum width 1

6. 高亮符號
* Menu: Tools -> Options -> Languages -> All Languages -> View -> 勾選 Symbol coloring, 勾選 Use bold for symbol names in definitions and declarations 和 Highlight unidentified symbols, Symbol lookup: Use strict symbol lookups(full symbol analysis)

7. 高亮修改的行
* Menu: Tools -> Options -> Languages -> All Languages -> View -> Modified lines

8. 高亮引用
* Menu: Tools -> Options -> Editing -> Context Tagging -> References -> Highlight references in editor

9. 自動在保存時清楚修改標記
* Menu: Tools -> Options -> File Options -> Save -> Reset modified lines

10. Context Tagging 速度
* Menu: Tools -> Options -> Editing-> Context Tagging -> Maximums(tune for performance) -> Update after (ms) idle(0 implies no delay) -> 150

11. 自動語法提示最少輸入字母數量:
* Menu: Tools -> Options -> Languages -> All Languages -> Auto-Complete -> Syntax expansion -> 勾選Use Syntax Expansion on space -> Minimum expandable keyword length -> 3

12. #include自動語法提示:
* Menu: Tools -> Options -> Languages -> All Languages -> Auto-Complete -> List symbols -> List include files after typing #include: List files after typing " or <

13. 多開SlickEdit
在開始菜單或者快捷方式 修改 "D:\Program Files\SlickEdit\win\vs.exe" +new

14. .h/.cpp文檔切換
* Menu: Document -> Edit Associated FIle(Ctrl + `)
* Cmd: :edit-associated-file

15. 摺疊代碼
* 右擊工具欄: Toolbars -> Selective Display
* View-->Toolbars-->Selective Display開啟

16. 摺疊單擊打開(默認是雙擊)
* Tools -> Options -> Keyboard and Mouse -> Advanced -> Selective Display, Expand/collapse -> Expand on single click

17. tab縮進風格
* Menu: Tools -> Options -> Languages -> All Languages -> Formatting -> Indent -> 勾選 Indent with tabs, Syntax indet:4(不確定, 提示與smart indent衝突了, 數目不同?)

18. {}縮進風格
* Menu: Tools -> Options -> Languages -> All Languages -> Formatting -> Begin/end style -> Style 2

19. 退格鍵刪tab
* Menu: Tools -> Options -> Languages -> C/C++ -> 勾選Backspace at beginning of line un-indents

20. 立即保存配置
* Menu: Tools -> Options -> Application Options -> Exit -> Save configuration -> Save configuration immediately

21. 配置備份
備份 C:\Users\akagi201\Documents\My SlickEdit Config\18.0.1 下面所有文件, 以後都放C盤就好了, 可以直接替換

22. 拷貝字符串後, 仍然保持選中狀態
* Tools -> Options -> Editing -> Selections -> 取消勾選 Deselect after copy

23. 將Slickedit的Projects資源列表切換成文件夾模式(待確認)
* Slickedit的文件排列是按照Source，Head之類的分的，很不習慣，把它改成按照文件夾結構在Projects窗口中點擊一個.vpj文件，按右鍵；選擇Auto Folder→Directory View，會出來一
個警告框。再點OK就好了

24. Workspace分類管理
* Workspace多了以後分類管理會比較方便Project->Organize all workspace，可以用目錄的形式對workspace進行管理，注意這裡並不會改變workspace
文件的位置，只是在slickedit裡面的分類而已.

25. GCC 參數配置
* 必須在建立了GNU C/C++項目後才能在Build裡面找到GNU C Options 

26. mou_push_tag： 
* Bind到Ctrl+鼠標左鍵就能和SourceInsight一樣可以按著Ctrl點符號就跳到定義處。不過SI在按下Ctrl的時候鼠標指針會變成小手看上去比較爽，SE的這個鼠標指針不會變化～～不過僅僅是視覺上稍有不爽，功能是一樣的.

27.當前選中行出現縮進輔助框 
* Menu: Tools-->Options-->Appearance-->General 
Current line highlight 設置成 Syntax indent ruler

28. 修改跳轉到Symbol定義處的速度 
Tools-->Options-->Editing-->Context Tagging 
Tagging Tool Windows項中的Updata Tool Windows after(ms) 設置成100

29. 修改符號著色的速度 
Tools-->Options-->Editing-->Context Tagging 
Symbol Coloring Performance項中將Update after(ms) idle的值設置成100

30. 在C/C++中代碼自適應 
Tools-->Options-->languages-->Application Languages-->C/C++ -->Adaptive Formatting 
勾選 Use Adaptive Formatting(重啟軟件後生效)

31. 在C/C++中的代碼註釋格式 
Tools-->Options-->languages-->Application Languages-->C/C++ -->Comments 
Comment line 項中 Left 項設置成//外加1個空格即// ; 
Location 項設置成At left margin

32. 在C/C++中代碼字符完成時,可以用TAB鍵切換匹配的字符 
Tools-->Options-->languages-->Application Languages-->C/C++ -->Auto-Complete 
Options標籤頁中 勾選Tab cycles through choices

33. 在C/C++中字符補全不區分大小寫 
Tools-->Options-->languages-->Application Languages-->C/C++ -->Color Coding 
取消Tokens標籤頁中identifies框中Case-sensitive前面的勾選

34. 單個文件符號著色開啟 
View-->Symbol Coloring 
勾選Enable Symbol Coloring 
勾選Highlight Unindentifed Symbols 
勾選All Symbols

## Refs
* <http://www.cnblogs.com/turingcat/archive/2013/01/09/2852372.html>
* <http://www.slickedit.com/support/faq/3-slickedit/6-running-slickeditr-from-a-usb-drive>