#E_5_4 功能: 猜數字遊戲
num=int(input('請輸入0-30的數值: '))
sex=input('請輸入性別: 1. 男  2. 女: ')
nmax=30   #設定最大值
nmin=0   #設定最小值
bingo=18  #答案
diff=0   #計算差額
if num==bingo:  #條件1
    if sex=='1':
        print('帥哥，你猜對了，真棒!')
    else:
        print('美女，妳猜對了，了不起')
elif (num <= bingo) & (num>=nmin):  #條件2
    diff=abs(num-bingo)
    if diff<3:
        print('你猜的數字比較小，已經接近了')
    else:
        print('你猜的數字比較小，還差很多')
elif (num> bingo) & (num<=nmax):  #條件3
    diff=num-bingo
    if diff<3:
        print('你猜的數字比較大，已經接近了')
    else:
        print('你猜的數字比較大，還差很多')
else:  #條件4
    if num<0 or num>30:
        print('輸入超過數值範圍')
    else:
        print('輸入錯誤')  