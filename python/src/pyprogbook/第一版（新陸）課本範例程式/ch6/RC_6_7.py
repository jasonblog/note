#RC_6_7 功能: 教育基金
def PVOA(PMT, i, n):
#PVOA: 計算普通年金現值公式    
    PVIFA=0
    #計算年金現值複利因子
    for t in range(1, n+1):
        PVIFA=PVIFA+((1+i)**(-t))
    PVA=PMT*PVIFA
    return(PVA)
cash=int(input('原始投資金額 = '))    
PMT=int(input('輸入每年領回金額 = '))    
n=int(input('輸入可以領回總年數 = '))
i=float(input('年利率 = '))
investment=round(PVOA(PMT, i, n),2)
print('本教育基金之投資方案現值 = ', investment)
payoff=round(investment-cash,2)
print('投資方案現值-原始投資金額= ', payoff)
if payoff<0:
    print('負報酬, 不建議購買')
else: 
    print('正報酬, 建議購買')  
    
    
