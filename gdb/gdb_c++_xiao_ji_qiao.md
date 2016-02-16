# gdb C++ 小技巧


以前自己用 gdb 都是拿來 debug C 語言的程式，這次拿來對 C++ 除錯發現有些小地方要注意的。

第一個是 C++ 字串。平常印出 C 語言的字串只需要用 print str_var 就可以了，不過要印出 C++ 的字串則是要用 print str_var.c_str()。

另外一個要注意的是 breakpoint 的設定方式。breakpoint 的設定可以用行號或者是 function name，不過如果在 C++ 除錯時如果有 namespace 必須要打出`完整的 name + class + method name`。比如說 ibus 的 PYBopomofoEditor.cc 裡面有 BopomofoEditor::processBopomofo method，要設定中斷點就必須打


b PY::BopomofoEditor::processBopomofo(unsigned int, unsigned int, unsigned int)


我想你也會跟我一樣覺得打這串也太長了吧，不過 gdb 支援自動補齊，只要打 PY::Bo 之後按下 Tab 鍵就可以用自動補齊的方式快速的打完整串了。

