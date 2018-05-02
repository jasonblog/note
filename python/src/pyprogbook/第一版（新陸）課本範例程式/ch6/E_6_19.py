#E_6_19.py 功能: 自訂函數名稱與Python內建函數相同
def callfunc(recchar):
#callfunc: 印出標題以及呼叫封閉函數
    print('my callfunc()')
    num=len(recchar)
    print('The length of char variable = ', num)
def len(recchar):
#len: 計算字串長度    
    sumi=0
    for i in recchar:
        sumi+=1
    return(sumi)        
callfunc('I love Python')  