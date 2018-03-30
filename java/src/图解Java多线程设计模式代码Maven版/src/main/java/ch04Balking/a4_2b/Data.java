package ch04Balking.a4_2b;

import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;

public class Data {
    private final String filename;  // 保存的文件名称
    private String content;         // 数据内容
    private boolean changed;        // 修改后的内容若未保存，则为true

    public Data(String filename, String content) {
        this.filename = filename;
        this.content = content;
        this.changed = true;
    }

    // 修改数据内容
    public synchronized void change(String newContent) {
        content = newContent;
        changed = true;
    }

    // 若数据内容修改过，则保存到文件中
    public void save() throws IOException {   // not synchronized
        if (!changed) {
            System.out.println(Thread.currentThread().getName() + " balks");
            return;
        }
        doSave();
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
        }
        changed = false;
    }

    // 将数据内容实际保存到文件中
    private void doSave() throws IOException {
        System.out.println(Thread.currentThread().getName() + " calls doSave, content = " + content);
        Writer writer = new FileWriter(filename);
        writer.write(content);
        writer.close();
    }
}
