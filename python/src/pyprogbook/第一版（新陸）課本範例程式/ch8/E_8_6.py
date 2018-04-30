# E_8_6: 多重繼承
class ParentClass1():
    def Parent_method1(self):
        print("呼叫父類別1方法(Parent_method1)")
class ParentClass2():
    def Parent_method2(self):
        print("呼叫父類別2方法(Parent_method2)")
class ChildClass(ParentClass1, ParentClass2):
    def Child_method(self):
        print("子類別方法(child method)")
baby = ChildClass()
baby.Parent_method1()
baby.Parent_method2()
baby.Child_method() 