# E_8_5: 支票存款繼承的應用
#定義新增支票帳戶繼承自CreateBankAccount
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
    def __deposit(self, amount):
        self.balance += amount
        return self.balance
    def __str__(self):
        return('帳號: '+self.id+ '\n姓名: ' + self.name + '\n期初餘額: ' + str(self.balance))    
class CheckingAccount(CreateBankAccount):
    def __init__(self, id, name):
        super().__init__(id, name) # 呼叫父類別__init__()
        self.overdraftlimit = 30000 #透支額度
    #考慮透支額度的提款方法         
    def withdraw(self, amount):
        if amount <= self.balance + self.overdraftlimit:
            self.balance -= amount 
        else:
            raise ValueError('超出信用額度')
        return self.balance
    def __str__(self):
         return (super().__str__() + 
                '\n透支額度\t' + str(self.overdraftlimit));

customer_c = CheckingAccount('k001', 'Angela Merkel')
print(customer_c.__str__())
#print('存款後餘額: ',customer_c.deposit(150))
print('存款後餘額: ',customer_c._CreateBankAccount__deposit(150))
print('提款後餘額: ',customer_c.withdraw(50))