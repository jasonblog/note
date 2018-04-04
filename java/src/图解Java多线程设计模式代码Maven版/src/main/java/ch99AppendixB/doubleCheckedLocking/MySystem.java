package ch99AppendixB.doubleCheckedLocking;// ×无法确保能够正确地运行

import java.util.Date;

public class MySystem {
    private static MySystem instance = null;
    private Date date = new Date();

    private MySystem() {
    }

    public static MySystem getInstance() {
        if (instance == null) {                 // (a) 第一次test
            synchronized (MySystem.class) {     // (b) 进入synchronized代码块
                if (instance == null) {         // (c) 第二次test
                    instance = new MySystem();  // (d) set
                }
            }                                   // (e) 从synchronized代码块中退出
        }
        return instance;                        // (f)
    }

    public Date getDate() {
        return date;
    }
}
