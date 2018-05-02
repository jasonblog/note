#descrip_stat.py功能: 以自訂函數寫法計算敍述統計並回傳
import math
def depsta(sample):
#depsta功能: 計算樣本的敍述統計    
    maxv=max(sample)
    minv=min(sample)
    meanv=sum(sample)/len(sample)
    sumi=0
    for i in range(0, len(sample)):
        sumi=sumi+((sample[i]-meanv)**2)
    varv=sumi/(len(sample)-1)    
    stdv=math.sqrt(varv)
    return(maxv, minv, meanv, varv, stdv)
sample=[5,8,9,6,4,1,5,3,6,2]
print('max=%2.1f,min=%2.1f, mean=%2.1f,var=%2.1f,std=%2.1f'%(depsta(sample)))
