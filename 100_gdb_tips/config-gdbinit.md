# 配置gdb init文件 

## 技巧

當gdb啟動時，會讀取HOME目錄和當前目錄下的的配置文件，執行裡面的命令。這個文件通常為“.gdbinit”。

這裡給出了本文檔中介紹過的，可以放在“.gdbinit”中的一些配置：

	# 打印STL容器中的內容
	python
	import sys
	sys.path.insert(0, "/home/xmj/project/gcc-trunk/libstdc++-v3/python")
	from libstdcxx.v6.printers import register_libstdcxx_printers
	register_libstdcxx_printers (None)
	end
	
	# 保存歷史命令
	set history filename ~/.gdb_history
	set history save on
	
	# 退出時不顯示提示信息
	set confirm off
	
	# 按照派生類型打印對象
	set print object on
	
	# 打印數組的索引下標
	set print array-indexes on
	
	# 每行打印一個結構體成員
	set print pretty on

歡迎補充。

## 貢獻者

xmj

