package c0.singleton;

import c0.singleton.Singleton;

/**
 * 單例模式-測試
 */
public class SingletonTest extends Thread {
    String myId;
    public SingletonTest(String id) {
        myId = id;
    }
    
    // 執行序執行的時候就去呼叫Singleton.getInstance()，多執行幾次會出現兩個不同的Singleton，這是多執行序造成的問題
    public void run() {
    	Singleton singleton = Singleton.getInstance();
    	if(singleton != null){

    		// 用hashCode判斷前後兩次取到的Singleton物件是否為同一個
            System.out.println(myId+"產生 Singleton:" + singleton.hashCode());       		
    	}
    }
    
    public static void main(String[] argv) {    	
        /*
        // 單執行緒的時候，s1與s2確實為同一個物件
        Singleton s1  =  Singleton.getInstance();
        Singleton s2  =  Singleton.getInstance();
        System.out.println("s1:"+s1.hashCode() + " s2:" + s2.hashCode());
        System.out.println();
        */
    	
    	// 兩個執行緒同時執行
        Thread t1 = new SingletonTest("執行序T1"); // 產生Thread物件
        Thread t2 = new SingletonTest("執行序T2"); // 產生Thread物件
        t1.start(); // 開始執行t1.run()
        t2.start();
    }
}
