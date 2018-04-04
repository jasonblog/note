package ch04Balking.sample;

public class Main {
    public static void main(String[] args) {
        Data data = new Data("data.txt", "(empty)");
        new ChangerThread("ChangerThread", data).start();
        new SaverThread("SaverThread", data).start();
    }
}
/*
SaverThread calls doSave, content = No.0
SaverThread calls doSave, content = No.1
SaverThread calls doSave, content = No.2
ChangerThread calls doSave, content = No.3
...
 */