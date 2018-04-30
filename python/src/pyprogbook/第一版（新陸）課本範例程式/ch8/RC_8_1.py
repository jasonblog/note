#RC_8_1: 定義貨幣時間價值的類別
class TimeValue:
    #終值    
    def fvfix(self, pv, i, n):
        #fvfix: 計算終值公式    
        fv=pv*(1+i)**n
        return(fv)
    #現值         
    def pvfix(self, fv, i, n):
        #pvfix: 計算現值公式    
        pv=fv/((1+i)**n)
        return(pv)      
#設定初始值        
pv=100
fv=115.92740743
i=0.03
n=5    
#呼叫TimeValue類別，建構物實體
tv1=TimeValue()
#呼叫物件實體的方法
print('%d年後的終值 = %10.4f' %(n, tv1.fvfix(pv, i, n)))
print('%d年後的現值 = %10.4f' %(n, tv1.pvfix(fv, i, n)))
