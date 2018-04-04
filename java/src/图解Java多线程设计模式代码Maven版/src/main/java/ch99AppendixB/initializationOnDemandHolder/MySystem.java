package ch99AppendixB.initializationOnDemandHolder;

import java.util.Date;

public class MySystem {
    private Date date = new Date();

    private MySystem() {
    }

    public static MySystem getInstance() {
        return Holder.instance;
    }

    public Date getDate() {
        return date;
    }

    private static class Holder {
        public static MySystem instance = new MySystem();
    }
}
