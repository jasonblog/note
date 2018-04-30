# E_8_3: 類別中不同方法的呼叫方式
class CallM(object):
    You='U'
    @classmethod
    def cls_method(cls, name):
        print('你好! %s, 這是類別方法. ' % name)
        print(' U =', cls.You)
    @staticmethod    
    def stc_method(name):
        print('你好! %s, 這是靜態方法. ' % name)  
        print(' U =', CallM.You)        
    def nor_method(self):
        print('你好! %s, 這是實體方法. ')  
        print(' U =', self.You)  
CallM.cls_method('hello')
CallM.stc_method('hello')
CallM.nor_method(CallM)        
classtest = classmethod(CallM.nor_method)
CallM.classtest('hi')
#print(classmethod(CallM.nor_method))