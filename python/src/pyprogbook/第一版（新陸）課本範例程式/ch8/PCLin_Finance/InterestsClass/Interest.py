#Interests: 定義利率相關方法的類別 
class Interest:
    #有效利率
    def earfix(self, rn, m):
        #earfix: 計算有效利率公式 
        ear=((1+(rn/m))**m)-1
        return(ear)  
    #連續複利
    def ccrfix(self, rn, m):
        import math         
        #ccrfix: 計算連續複利公式 
        ccr=m*math.log(1+(rn/m))
        return(ccr)