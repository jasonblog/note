# 在匿名空間設置斷點

## 例子
	namespace Foo
	{
	  void foo()
	  {
	  }
	}

	namespace
	{
	  void bar()
	  {
	  }
	}

## 技巧

在gdb中，如果要對namespace Foo中的foo函數設置斷點，可以使用如下命令：

	(gdb) b Foo::foo

如果要對匿名空間中的bar函數設置斷點，可以使用如下命令：

	(gdb) b (anonymous namespace)::bar

## 貢獻者

xmj

