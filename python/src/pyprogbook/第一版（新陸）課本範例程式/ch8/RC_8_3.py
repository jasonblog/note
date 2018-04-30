#RC_8_3: 將貨幣時間價值相關的方法建立成套件
import PCLin_Finance.TimeValueClass.TimeValue as tv
import PCLin_Finance.InterestsClass.Interest as rt
#設定初始值        
pv=100
fv=115.92740743
i=0.03
n=5    
rn=0.06
m=int(input('請輸入年複利次數: 1, 2, 4, 12, 52, 365 = '))
#呼叫TimeValue類別，建構物實體
tv1=tv.TimeValue()
#呼叫物件實體的方法
print('%d年後的終值 = %10.4f' %(n, tv1.fvfix(pv, i, n)))
print('%d年後的現值 = %10.4f' %(n, tv1.pvfix(fv, i, n)))
#呼叫TimeValue類別，建構物實體
rt1=rt.Interest()
#呼叫物件實體的方法
print('有效利率 = %8.4f%%' %(rt1.earfix(i, m)*100))
print('連續複利 = %8.4f%%' %(rt1.ccrfix(i, m)*100))