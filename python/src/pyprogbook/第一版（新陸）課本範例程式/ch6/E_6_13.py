#E_6_13.py 功能: 形式參數與「**」的用法，代入空的dict範例
def sales(n,**d):
#sales 功能: 1. 判斷性別以及2. 加總字典的值
    if n==1:
        sex='先生'
    else:
        sex='小姐'
    return (sex, sum(d.values()))
[sex,total]=sales(1,A=5000, B=4000, C=3000)
print('%s您好, 您負責的銷售區域之銷售金額 = %d' %(sex, total))

