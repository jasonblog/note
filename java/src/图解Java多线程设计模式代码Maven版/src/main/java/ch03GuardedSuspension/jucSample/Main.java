package ch03GuardedSuspension.jucSample;

public class Main {
    public static void main(String[] args) {
        RequestQueue requestQueue = new RequestQueue();
        new ClientThread(requestQueue, "Alice", 3141592L).start();
        new ServerThread(requestQueue, "Bobby", 6535897L).start();
    }
}
/*
Alice requests [ Request No.0 ]
Bobby handles  [ Request No.0 ]
Alice requests [ Request No.1 ]
Alice requests [ Request No.2 ]
Bobby handles  [ Request No.1 ]
Bobby handles  [ Request No.2 ]
Alice requests [ Request No.3 ]
Bobby handles  [ Request No.3 ]
Alice requests [ Request No.4 ]
Bobby handles  [ Request No.4 ]
Alice requests [ Request No.5 ]
Alice requests [ Request No.6 ]
Bobby handles  [ Request No.5 ]
Bobby handles  [ Request No.6 ]
Alice requests [ Request No.7 ]
...
 */