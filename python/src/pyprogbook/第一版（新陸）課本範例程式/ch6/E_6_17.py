#E_6_17.py 功能: 在自訂函數內將某變數宣告全域變數
def func5():
#func5: 在函數內印出全域變數 
    global num_var
    num_var=num_var+10
    print('inside func5 num_var = ', num_var)
num_var=5    
print('before call func5 num_var = ', num_var)
func5()
print('after call func5 num_var = ', num_var)  