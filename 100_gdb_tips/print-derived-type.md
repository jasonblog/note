# 按照派生類型打印對象

## 例子

	#include <iostream>
	using namespace std;
	
	class Shape {
	 public:
	  virtual void draw () {}
	};
	
	class Circle : public Shape {
	 int radius;
	 public:
	  Circle () { radius = 1; }
	  void draw () { cout << "drawing a circle...\n"; }
	};
	
	class Square : public Shape {
	 int height;
	 public:
	  Square () { height = 2; }
	  void draw () { cout << "drawing a square...\n"; }
	};
	
	void drawShape (class Shape &p)
	{
	  p.draw ();
	}
	
	int main (void)
	{
	  Circle a;
	  Square b;
	  drawShape (a);
	  drawShape (b);
	  return 0;
	}

## 技巧

在gdb中，當打印一個對象時，缺省是按照聲明的類型進行打印：

	(gdb) frame
	#0  drawShape (p=...) at object.cxx:25
	25	  p.draw ();
	(gdb) p p
	$1 = (Shape &) @0x7fffffffde90: {_vptr.Shape = 0x400a80 <vtable for Circle+16>}

在這個例子中，p雖然聲明為class Shape，但它實際的派生類型可能為class Circle和Square。如果要缺省按照派生類型進行打印，則可以通過如下命令進行設置：

	(gdb) set print object on

	(gdb) p p
	$2 = (Circle &) @0x7fffffffde90: {<Shape> = {_vptr.Shape = 0x400a80 <vtable for Circle+16>}, radius = 1}

當打印對象類型信息時，該設置也會起作用：

	(gdb) whatis p
	type = Shape &
	(gdb) ptype p
	type = class Shape {
	  public:
	    virtual void draw(void);
	} &

	(gdb) set print object on
	(gdb) whatis p
	type = /* real type = Circle & */
	Shape &
	(gdb) ptype p
	type = /* real type = Circle & */
	class Shape {
	  public:
	    virtual void draw(void);
	} &

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Print-Settings.html#index-set-print)

## 貢獻者

xmj

xanpeng

