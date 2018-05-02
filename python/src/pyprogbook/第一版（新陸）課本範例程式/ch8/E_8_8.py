# E_8_8: Raise 引發異常
try: 
    a=100
    b='50'
    num=a+b
    raise TypeError 
except TypeError as e: 
    print('字串不能算術運算: ' + str(e))  
    num=int(a)+int(b)
    print('num =' , num)