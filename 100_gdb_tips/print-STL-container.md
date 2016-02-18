# 打印STL容器中的內容 

## 例子

	#include <iostream>
	#include <vector>
	
	using namespace std;
	
	int main ()
	{
	  vector<int> vec(10); // 10 zero-initialized elements
	
	  for (int i = 0; i < vec.size(); i++)
	    vec[i] = i;
	
	  cout << "vec contains:";
	  for (int i = 0; i < vec.size(); i++)
	    cout << ' ' << vec[i];
	  cout << '\n';
	
	  return 0;
	}

## 技巧一

在gdb中，如果要打印C++ STL容器的內容，缺省的顯示結果可讀性很差：

	(gdb) p vec
	$1 = {<std::_Vector_base<int, std::allocator<int> >> = {
	    _M_impl = {<std::allocator<int>> = {<__gnu_cxx::new_allocator<int>> = {<No data fields>}, <No data fields>}, _M_start = 0x404010, _M_finish = 0x404038, 
              _M_end_of_storage = 0x404038}}, <No data fields>}

gdb 7.0之後，可以使用gcc提供的python腳本，來改善顯示結果：

	(gdb) p vec
	$1 = std::vector of length 10, capacity 10 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}

某些發行版(Fedora 11+)，不需要額外的設置工作。可在gdb命令行下驗證（若沒有顯示，可按下文的方法進行設置）。

		(gdb) info pretty-printer

方法如下:

1. 獲得python腳本，建議使用gcc默認安裝的

		sudo find / -name "*libstdcxx*"
2. 若本機查找不到python腳本，建議下載gcc對應版本源碼包，相對目錄如下

		gcc-4.8.1/libstdc++-v3/python
3. 也可直接下載最新版本

		svn co svn://gcc.gnu.org/svn/gcc/trunk/libstdc++-v3/python

4. 將如下代碼添加到.gdbinit文件中（假設python腳本位於 /home/maude/gdb_printers/ 下）

		python
		import sys
		sys.path.insert(0, '/home/maude/gdb_printers/python')
		from libstdcxx.v6.printers import register_libstdcxx_printers
		register_libstdcxx_printers (None)
		end

（源自https://sourceware.org/gdb/wiki/STLSupport）

## 技巧二

`p vec`的輸出無法閱讀，但能給我們提示，從而得到無需腳本支持的技巧：

	(gdb) p *(vec._M_impl._M_start)@vec.size()
	$2 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
	
## 技巧三

將 [dbinit_stl_views](http://www.yolinux.com/TUTORIALS/src/dbinit_stl_views-1.03.txt ) 下載下來,，執行命令
```shell
cat dbinit_stl_views-1.03.txt >> ~/.gdbinit
```
即可
一些常用的容器及其對應的命令關係
```shell
std::vector<T>  pvector stl_variable 
std::list<T>  plist stl_variable T 
std::map<T,T>  pmap stl_variable 
std::multimap<T,T>  pmap stl_variable 
std::set<T>  pset stl_variable T 
std::multiset<T>  pset stl_variable 
std::deque<T>  pdequeue stl_variable 
std::stack<T>  pstack stl_variable 
std::queue<T>  pqueue stl_variable 
std::priority_queue<T>  ppqueue stl_variable 
std::bitset<n><td>  pbitset stl_variable 
std::string  pstring stl_variable 
std::widestring  pwstring stl_variable  
```
更多詳情，參考配置中的幫助


## 貢獻者

xmj

xanpeng

enjolras
