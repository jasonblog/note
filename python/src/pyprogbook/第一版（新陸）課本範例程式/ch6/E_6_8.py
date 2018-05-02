#E_6_8.py 功能: 預設參數的使用範例
import math
def defaultscore(score=40):
#defaultscore: 判斷若小於50則開根號乘以10 
    if score<50:
        return math.sqrt(score)*10
    else:
        return score
name='Jack'
ss=[16,'',36,55,'']
for ii in  ss:
    if type(ii) is int:
        result=defaultscore(ii)
    else:
        result=defaultscore()
    print('%s 的成績 = %4.2f ' % (name, result))
