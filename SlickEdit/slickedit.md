# SlickEdit


![SlickEdit](http://upload.wikimedia.org/wikipedia/en/2/2e/SlickEdit_logo.png)

## 换编辑器的原因
1. Source Insight 不进行大的更新了(今天看, 居然更新了一个小版本, 呵呵, 装了没任何变换啊) 经常无响应, 郁闷死我了
2. Source Insight不能进行定制, 闭源软件做到这个份上就应该开源啊, 没有插件机制, 别人给你写插件还要hack, 用外挂的方式, 想想真恶心啊, 你值得别人这么对你吗.

## 我目前对编辑器的需求
1. 源码工程与源码目录分离
2. 有静态数据库, 可以快速跳转
3. 持续更新有保障, 有活力

## Understand vs SlickEdit
* 简单的搜索后发现这么两个还算比较靠谱吧, 真是少啊, 开源软件怎么就没有做大的呢! 开源只有eclipse靠谱了, 可是我总不能用他来看c代码吧.
* 让我决定选择 SlickEdit的原因其实很简单, 官方社区比较好, 人比较多, 还有一些亮点, V18 beta版本中居然支持 golang赞, 果断用, 而且要精通.

## SlickEdit
* [破解文件(来自网络)](http://pan.baidu.com/s/1uxMTF#dir/path=%2Fakshare)
* Editor Emulation: 可以模拟各种编辑器, 如: VIM, Emacs等, 我先选了CUA, 就是跟office word一致的快捷键方式.
* Color Scheme: 我选择了一个墨绿色背景的 Chalkboard, google了一下发现 GitHub Gist上面有一些人自己的设置, 以后可以去参考别人的定制一下.
* Fonts(字体): 还是选择我习惯的Consolas, 顺便用MacType渲染了一下, 弄了下Source Code Pro, 感觉和Consolas差不多(Mac下面没有consolas头疼)

## Config
1. 工具栏图标设置为小(因为工具栏都是最常用的功能实在找不到不用快捷键而使用它们的理由)
* Menu: View -> Toolbars -> Customize... -> Options -> Options -> Toolbar button size and spacing -> Small, 勾选 Hide when application is inactive
(Tools -> Options -> Appearance -> Toolbars)

2. 字体: Consolas
* Menu: Tools -> Appearance -> Fonts
consolas, 12

3. 逗号后面自动填充空格
* Menu: Tools -> Options -> Languages -> All Languages -> Context Tagging -> Parameter information -> 勾选Insert space after comma

4. 高亮当前光标下的matching symbols
* Menu: Tools -> Options -> Languages -> All Languages -> Context Tagging -> Parameter information -> 勾选Highlight matching symbols under cursor

5. 显示行号
* Menu: Tools -> Options -> Languages -> All Languages -> View -> 勾选 Line numbers, Mininum width 1

6. 高亮符号
* Menu: Tools -> Options -> Languages -> All Languages -> View -> 勾选 Symbol coloring, 勾选 Use bold for symbol names in definitions and declarations 和 Highlight unidentified symbols, Symbol lookup: Use strict symbol lookups(full symbol analysis)

7. 高亮修改的行
* Menu: Tools -> Options -> Languages -> All Languages -> View -> Modified lines

8. 高亮引用
* Menu: Tools -> Options -> Editing -> Context Tagging -> References -> Highlight references in editor

9. 自动在保存时清楚修改标记
* Menu: Tools -> Options -> File Options -> Save -> Reset modified lines

10. Context Tagging 速度
* Menu: Tools -> Options -> Editing-> Context Tagging -> Maximums(tune for performance) -> Update after (ms) idle(0 implies no delay) -> 150

11. 自动语法提示最少输入字母数量:
* Menu: Tools -> Options -> Languages -> All Languages -> Auto-Complete -> Syntax expansion -> 勾选Use Syntax Expansion on space -> Minimum expandable keyword length -> 3

12. #include自动语法提示:
* Menu: Tools -> Options -> Languages -> All Languages -> Auto-Complete -> List symbols -> List include files after typing #include: List files after typing " or <

13. 多开SlickEdit
在开始菜单或者快捷方式 修改 "D:\Program Files\SlickEdit\win\vs.exe" +new

14. .h/.cpp文档切换
* Menu: Document -> Edit Associated FIle(Ctrl + `)
* Cmd: :edit-associated-file

15. 折叠代码
* 右击工具栏: Toolbars -> Selective Display
* View-->Toolbars-->Selective Display开启

16. 折叠单击打开(默认是双击)
* Tools -> Options -> Keyboard and Mouse -> Advanced -> Selective Display, Expand/collapse -> Expand on single click

17. tab缩进风格
* Menu: Tools -> Options -> Languages -> All Languages -> Formatting -> Indent -> 勾选 Indent with tabs, Syntax indet:4(不确定, 提示与smart indent冲突了, 数目不同?)

18. {}缩进风格
* Menu: Tools -> Options -> Languages -> All Languages -> Formatting -> Begin/end style -> Style 2

19. 退格键删tab
* Menu: Tools -> Options -> Languages -> C/C++ -> 勾选Backspace at beginning of line un-indents

20. 立即保存配置
* Menu: Tools -> Options -> Application Options -> Exit -> Save configuration -> Save configuration immediately

21. 配置备份
备份 C:\Users\akagi201\Documents\My SlickEdit Config\18.0.1 下面所有文件, 以后都放C盘就好了, 可以直接替换

22. 拷贝字符串后, 仍然保持选中状态
* Tools -> Options -> Editing -> Selections -> 取消勾选 Deselect after copy

23. 将Slickedit的Projects资源列表切换成文件夹模式(待确认)
* Slickedit的文件排列是按照Source，Head之类的分的，很不习惯，把它改成按照文件夹结构在Projects窗口中点击一个.vpj文件，按右键；选择Auto Folder→Directory View，会出来一
个警告框。再点OK就好了

24. Workspace分类管理
* Workspace多了以后分类管理会比较方便Project->Organize all workspace，可以用目录的形式对workspace进行管理，注意这里并不会改变workspace
文件的位置，只是在slickedit里面的分类而已.

25. GCC 参数配置
* 必须在建立了GNU C/C++项目后才能在Build里面找到GNU C Options 

26. mou_push_tag： 
* Bind到Ctrl+鼠标左键就能和SourceInsight一样可以按着Ctrl点符号就跳到定义处。不过SI在按下Ctrl的时候鼠标指针会变成小手看上去比较爽，SE的这个鼠标指针不会变化～～不过仅仅是视觉上稍有不爽，功能是一样的.

27.当前选中行出现缩进辅助框 
* Menu: Tools-->Options-->Appearance-->General 
Current line highlight 设置成 Syntax indent ruler

28. 修改跳转到Symbol定义处的速度 
Tools-->Options-->Editing-->Context Tagging 
Tagging Tool Windows项中的Updata Tool Windows after(ms) 设置成100

29. 修改符号着色的速度 
Tools-->Options-->Editing-->Context Tagging 
Symbol Coloring Performance项中将Update after(ms) idle的值设置成100

30. 在C/C++中代码自适应 
Tools-->Options-->languages-->Application Languages-->C/C++ -->Adaptive Formatting 
勾选 Use Adaptive Formatting(重启软件后生效)

31. 在C/C++中的代码注释格式 
Tools-->Options-->languages-->Application Languages-->C/C++ -->Comments 
Comment line 项中 Left 项设置成//外加1个空格即// ; 
Location 项设置成At left margin

32. 在C/C++中代码字符完成时,可以用TAB键切换匹配的字符 
Tools-->Options-->languages-->Application Languages-->C/C++ -->Auto-Complete 
Options标签页中 勾选Tab cycles through choices

33. 在C/C++中字符补全不区分大小写 
Tools-->Options-->languages-->Application Languages-->C/C++ -->Color Coding 
取消Tokens标签页中identifies框中Case-sensitive前面的勾选

34. 单个文件符号着色开启 
View-->Symbol Coloring 
勾选Enable Symbol Coloring 
勾选Highlight Unindentifed Symbols 
勾选All Symbols

## Refs
* <http://www.cnblogs.com/turingcat/archive/2013/01/09/2852372.html>
* <http://www.slickedit.com/support/faq/3-slickedit/6-running-slickeditr-from-a-usb-drive>