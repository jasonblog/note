#E_5_7 功能: 找出1～100能被7整除的因數、個數與總和。
count=0  #累加1到100能被7整除的個數
lis0=[]  #記錄1到100能被7整除的因子
sumi=0   #累加1到100能被7整除的因子總合為
for i in range(1,101):
    [quotient, remainder]=divmod(i,7) 
    if remainder==0:
        count=count+1
        sumi=sumi+i
        lis0.append(i)
print('1到100能被7整數的因子=',lis0)        
print('1到100能被7整數的個數=',count)
print('1到100能被7整數的因子總合為=',sumi)
