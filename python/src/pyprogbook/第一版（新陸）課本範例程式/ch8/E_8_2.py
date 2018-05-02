# E_8_2: 銀行存提款功能採用物件導向程式設計模式
#定義新增銀行帳戶類別
class CreateBankAccount:
    #定義新增帳戶方法，額餘設定為0    
    def __init__(self, id, name):
        self.id = id
        self.name = name
        self.balance = 0
    #定義提款方法          
    def withdraw(self, amount):
        self.balance -= amount
        return self.balance
    #定義存款方法 
    def deposit(self, amount):
        self.balance += amount
        return self.balance
    def __str__(self):
        return('帳號: '+self.id+ '\n姓名: ' + self.name + '\n初期餘額: ' + str(self.balance))        
customer_a = CreateBankAccount('c001', 'Jemery Lin')
customer_b = CreateBankAccount('c002', 'Barack Obama')
print(customer_a.__str__())
print('存款後餘額: ',customer_a.deposit(100))
print(customer_b.__str__())
print('存款後餘額: ',customer_b.deposit(50))
print(customer_a.__str__())
print('提款後餘額: ',customer_a.withdraw(10))
print(customer_b.__str__())
print('提款後餘額: ',customer_b.withdraw(10))