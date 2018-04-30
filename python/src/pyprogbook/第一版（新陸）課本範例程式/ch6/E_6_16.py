#E_6_16.py 功能: 印出變數所屬區域變數範圍或全域變數範圍
def func4():
#func4: 在函數內印出全域變數  
     print(global_var, 'print inside func4')
global_var='global variable'    
func4()
print(global_var, 'print outside func4')