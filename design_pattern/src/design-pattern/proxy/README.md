#代理模式

代理模式-为其他对象提供一种代理，来动态控制这个对象的访问
代理对象启到中介作用，可去掉功能服务或增加额外的服务

几种代理方式：
1.远程代理--为不同地理的对象，提供局域网代表对象---通过远程代理来模拟各个店铺的监控
2.虚拟代理--根据需要将资源消耗很大的对象进行延迟-真正需要的时候进行创建--比如说先加载文字-图片是虚拟的-再加载
3.保护代理--控制一个对象的访问权限--权限控制
4.智能引用代理--火车票代售处

智能引用代理实现
静态代理和动态代理
静态代理--代理和被代理对象在代理在代理之前是确定的，他们都是实现的相同的接口或者继承相同的抽象类
模拟场景--模拟一辆车


动态代理
Java动态代理类位于Java.lang.reflect包下，一般主要涉及到以下几个类：
1.Interface InvocationHandler:该接口中仅定义一个方法
public Object invoke(Object obj, Method method, Object[] args)
在实际应用时，第一个参数obj一般指代理类，method是指被代理的方法，args为该方法的参数数组，这个方法在代理类中动态实现

2.Proxy:该类即为动态代理类
static Object newProxyInstance(ClassLoader loader, Class[] interfaces, InvocationHandler h)
返回代理类的一个实例，返回后的代理类可以当作代理类使用（可使用被代理类的在接口中声明过的方法）

所谓Dynamic Proxy是这样一种class:
他是在运行时生成的class
该class需要实现一组interface
使用动态代理类时，必须实现InvocationHandler接口

动态代理实现步骤
1.创建一个实现接口InvocationHandler的类，它必须实现invoke方法
2.创建被代理的类以及接口
3.调用Proxy的静态方法，创建一个代理类
newProxyInstance(ClassLoader loader, Class[] interfaces, InvocationHandler h)
4.通过代理调用方法

JDK动态代理和CGLIB动态代理区别
JDK动态代理：
    1.只能代理实现了接口的类
    2.没有实现接口的类不能实现JDK动态代理
CGLIB动态代理：
    1.针对类来实现代理
    2.对指定目标类产生一个子类，通过方法拦截技术拦截所有父类方法的调用
    3.不能对final的类代理
