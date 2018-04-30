# E_8_1: 銀行存提款功能採用程序導向程式設計模式
#新增帳戶
def create_account():
    return {'balance': 0}
#計算存款後餘額    
def deposit(account, amount):
    account['balance'] += amount
    return account['balance']
#計算提款後餘額
def withdraw(account, amount):
    account['balance'] -= amount
    return account['balance']
customer_a = create_account()
customer_b = create_account()
print('a客戶存款後餘額: ',deposit(customer_a, 100))
print('b客戶存款後餘額: ',deposit(customer_b, 50))
print('b客戶提款後餘額: ',withdraw(customer_b, 10))
print('a客戶提款後餘額: ',withdraw(customer_a, 10))
