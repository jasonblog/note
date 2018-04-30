# E_8_7: 多型
class Animal():
    def __init__(self, name): 
        self.name = name
    def __str__(self):
       return('動物名稱: '+self.name + '\t叫聲: ')  
class Cat(Animal):
    def Call(self):
        self.calling='喵 喵!'
        return(super().__str__() + self.calling)
class Dog(Animal):
    def Call(self):
        self.calling='旺 旺!'
        return(super().__str__() + self.calling)
class Sheep(Animal):
    def Call(self):
        self.calling='咩 咩!'
        return(super().__str__() + self.calling)        
Animals_List = [Cat('猫咪'), Dog('狗狗'), Sheep('綿羊')]
for i in Animals_List:
    print(i.Call())   