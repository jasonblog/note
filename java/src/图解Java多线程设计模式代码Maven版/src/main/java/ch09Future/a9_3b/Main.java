package ch09Future.a9_3b;

import ch09Future.a9_3b.content.Content;
import ch09Future.a9_3b.content.Retriever;

import java.io.FileOutputStream;
import java.io.IOException;

public class Main {
    public static void main(String args[]) {
        long start = System.currentTimeMillis();

        Content content1 = Retriever.retrieve("http://www.yahoo.com/");
        Content content2 = Retriever.retrieve("http://www.google.com/");
        Content content3 = Retriever.retrieve("http://www.hyuki.com/");

        saveToFile("yahoo.html", content1);
        saveToFile("google.html", content2);
        saveToFile("hyuki.html", content3);

        long end = System.currentTimeMillis();

        System.out.println("Elapsed time = " + (end - start) + "msec.");
    }

    // 将content中的内容写入名为filename的文件中
    private static void saveToFile(String filename, Content content) {
        byte[] bytes = content.getBytes();
        try {
            System.out.println(Thread.currentThread().getName() + ": Saving to " + filename);
            FileOutputStream out = new FileOutputStream(filename);
            for (int i = 0; i < bytes.length; i++) {
                out.write(bytes[i]);
            }
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
