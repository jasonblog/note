#E_6_14.py 功能: 變數命名空間的範圍
def func2():
#func2: 在自訂函數內印出x    
    x=5
    print('x = %d %s'  %(x, 'in func2'))
x=10    
print('x = %d %s'  %(x, 'in global'))
func2()    