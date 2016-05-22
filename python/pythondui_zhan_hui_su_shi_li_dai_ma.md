# python堆棧回溯示例代碼

```py
import sys, traceback
def dump_stack():
    try:
        raise Exception("dump stack")
    except:
        tb = sys.exc_info()[2]
        while tb.tb_next:
            tb = tb.tb_next
        f = tb.tb_frame
        while f:
            print("Frame %s in %s at line %d" % (f.f_code.co_name, f.f_code.co_filename, f.f_lineno))
            f = f.f_back
            
def my_1():
	dump_stack()
def my_2():
	my_1()
def my_3():
	my_2()

my_3()
```