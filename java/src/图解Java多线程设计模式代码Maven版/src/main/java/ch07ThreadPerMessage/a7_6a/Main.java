package ch07ThreadPerMessage.a7_6a;

import java.io.IOException;

public class Main {
    public static void main(String args[]) {
        try {
            new MiniServer(8888).execute();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
