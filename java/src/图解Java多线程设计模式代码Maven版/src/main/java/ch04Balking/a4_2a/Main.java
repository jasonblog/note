package ch04Balking.a4_2a;

public class Main {
    public static void main(String[] args) {
        Data data = new Data("data.txt", "(empty)");
        new ChangerThread("ChangerThread", data).start();
        new SaverThread("SaverThread", data).start();
    }
}
/*
SaverThread calls doSave, content = No.0
ChangerThread balks
SaverThread calls doSave, content = No.1
ChangerThread balks
ChangerThread calls doSave, content = No.2
SaverThread calls doSave, content = No.3
ChangerThread balks
SaverThread calls doSave, content = No.4
ChangerThread balks
ChangerThread calls doSave, content = No.5
ChangerThread calls doSave, content = No.6
SaverThread calls doSave, content = No.7
ChangerThread balks
...
 */