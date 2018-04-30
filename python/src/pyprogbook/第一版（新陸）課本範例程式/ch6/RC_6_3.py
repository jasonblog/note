#RC_6_3 功能: 以自訂函數撰寫直線法折舊費用
import numpy as np
def straightline(cost,salvage,n):
#straightline: 直線法公式
    return ((cost-salvage)/n)
def slinetable(exp_list,n,cost):
    #slinetable: 計算折舊費用分攤表    
    acm_exp=0
    sline_result=[]
    row=[]
    for i in range(0,n):
        acm_exp=acm_exp+exp_list[i]
        balance=cost-acm_exp
        row.append(i+1)
        row.append(exp_list[i])
        row.append(acm_exp)
        row.append(balance)
        sline_result.append(row)
        row=[]
    return (sline_result)
cost=100000
salvage=10000
n=10
sline_exp=straightline(cost,salvage,n)
sline_exp_list=list(np.linspace(sline_exp,sline_exp, n))
slt=slinetable(sline_exp_list,n,cost)
print('%3s%8s%8s%8s' %('年度','折舊費用','累計折舊','期末帳面價值'))
for i in range(0, n):
    print('%4d%10d%12d%14d' %(slt[i][0],slt[i][1],slt[i][2],slt[i][3]))

