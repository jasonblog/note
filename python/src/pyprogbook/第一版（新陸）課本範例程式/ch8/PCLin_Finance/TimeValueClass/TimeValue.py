#TimeValue: 定義貨幤相關方法的類別 
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