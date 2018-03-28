#代理模式

代理模式-為其他對象提供一種代理，來動態控制這個對象的訪問
代理對象啟到中介作用，可去掉功能服務或增加額外的服務

幾種代理方式：
1.遠程代理--為不同地理的對象，提供局域網代表對象---通過遠程代理來模擬各個店鋪的監控
2.虛擬代理--根據需要將資源消耗很大的對象進行延遲-真正需要的時候進行創建--比如說先加載文字-圖片是虛擬的-再加載
3.保護代理--控制一個對象的訪問權限--權限控制
4.智能引用代理--火車票代售處

智能引用代理實現
靜態代理和動態代理
靜態代理--代理和被代理對象在代理在代理之前是確定的，他們都是實現的相同的接口或者繼承相同的抽象類
模擬場景--模擬一輛車


動態代理
Java動態代理類位於Java.lang.reflect包下，一般主要涉及到以下幾個類：
1.Interface InvocationHandler:該接口中僅定義一個方法
public Object invoke(Object obj, Method method, Object[] args)
在實際應用時，第一個參數obj一般指代理類，method是指被代理的方法，args為該方法的參數數組，這個方法在代理類中動態實現

2.Proxy:該類即為動態代理類
static Object newProxyInstance(ClassLoader loader, Class[] interfaces, InvocationHandler h)
返回代理類的一個實例，返回後的代理類可以當作代理類使用（可使用被代理類的在接口中聲明過的方法）

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

JDK動態代理和CGLIB動態代理區別
JDK動態代理：
    1.只能代理實現了接口的類
    2.沒有實現接口的類不能實現JDK動態代理
CGLIB動態代理：
    1.針對類來實現代理
    2.對指定目標類產生一個子類，通過方法攔截技術攔截所有父類方法的調用
    3.不能對final的類代理
