# E_8_4: 封裝
class Encap:
    def __init__(self):
        self.i = 99    
        self.__i = 99  
    def hello(self):
        return ('我愛她'+ str(self.i))  
    def __hello(self):
        return ('私有方法')    
Love=Encap()   
print(Love.i)
#print(Love.__i)
print(Love.hello())
#print(Love.__hello())
