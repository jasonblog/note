#E_6_15.py 功能: 區域變數與全域變數在變數命名空間的查詢
def func3():
#func3: 在函數內查詢並印出y是否在區域變數在變數命名空間  
    y=6
    print('y in func3 = ', 'y' in locals())
x=3    
func3()
print('y in global = ', 'y' in globals())
print('x in global = ', 'x' in globals())
print('func3 in global = ', 'func3' in globals())
  