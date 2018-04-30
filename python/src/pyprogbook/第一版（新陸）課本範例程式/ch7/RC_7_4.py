#RC_7_4: 房價指數等樣本資料做常態分配檢定
import scipy.stats as stats
import pandas as pd
#匯入excel資料並計算敘述統計後指派給data
df1=pd.read_excel('./file/houseindex.xlsx', sheetname='rowdata')
df1.columns = ['y', 'x1', 'x2', 'x3', 'x4','x5', 'x6','x7','x8','x9', 'x10']
data=df1.describe()
#計算三個統計量: 常態分配檢定、偏態(Skewness)、峰態(Kurtosis)
s1=[]
s2=[]
s3=[]
for i in range(0, len(data.columns)):
    s=df1[df1.columns[i]]
    [st, pv1]=stats.normaltest(s)
    pv2=stats.skew(s)
    pv3=stats.kurtosis(s)    
    s1.append(pv1)
    s2.append(pv2) 
    s3.append(pv3)  
#將三個新統計量加入到df2,再與data合併     
df2 = pd.DataFrame([list(s1), list(s2), list(s3)], index=['norm','skew', 'kurt'], columns = ['y', 'x1', 'x2', 'x3', 'x4','x5', 'x6','x7','x8','x9', 'x10'])
df3=pd.concat([data,df2])
print(df3.round(2))

