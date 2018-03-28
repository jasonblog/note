# 動態代理
所謂Dynamic Proxy是這樣一種class:
他是在運行時生成的class
該class需要實現一組interface
使用動態代理類時，必須實現InvocationHandler接口

動態代理實現步驟
1.創建一個實現接口InvocationHandler的類，它必須實現invoke方法
2.創建被代理的類以及接口
3.調用Proxy的靜態方法，創建一個代理類
newProxyInstance(ClassLoader loader, Class[] interfaces, InvocationHandler h)
4.通過代理調用方法