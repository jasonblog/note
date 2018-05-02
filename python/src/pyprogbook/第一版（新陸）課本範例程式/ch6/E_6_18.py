#E_6_18.py 功能: 印出函數內含函數的封閉變數
def out_func():
#out_func: 印出區域變數並呼叫子函數in_func    
    var='local variable'
    print('In out_func variable =', var)
    def in_func():
    #in_func: 印出封閉變數       
        var='enclosed variable'
        print('In in_func variable =', var)
    in_func()
    print('After in_func variable =', var)
var='global variable'
print('Before out_func variable =', var)
out_func()  
print('After out_func variable =', var)