package ch01SingleThreadedExecution.sample1;

public class Main {
    public static void main(String[] args) {
        System.out.println("Testing Gate, hit CTRL+C to exit.");
        Gate gate = new Gate();
        new UserThread(gate, "Alice", "Alaska").start();
        new UserThread(gate, "Bobby", "Brazil").start();
        new UserThread(gate, "Chris", "Canada").start();
    }
}
/*
Testing Gate, hit CTRL+C to exit.
Alice BEGIN
Bobby BEGIN
Chris BEGIN
***** BROKEN ***** No.13343: Bobby, Brazil
***** BROKEN ***** No.18343: Chris, Canada
***** BROKEN ***** No.88136: Bobby, Brazil
***** BROKEN ***** No.88547: Chris, Canada
***** BROKEN ***** No.89925: Alice, Alaska
***** BROKEN ***** No.90401: Chris, Canada
***** BROKEN ***** No.90908: Chris, Alaska
***** BROKEN ***** No.90908: Chris, Alaska
***** BROKEN ***** No.94588: Chris, Canada
***** BROKEN ***** No.95215: Chris, Canada
***** BROKEN ***** No.99224: Chris, Canada
***** BROKEN ***** No.101935: Bobby, Brazil
***** BROKEN ***** No.102115: Chris, Canada
***** BROKEN ***** No.103128: Chris, Brazil
***** BROKEN ***** No.109751: Alice, Alaska
***** BROKEN ***** No.111395: Alice, Alaska
...
 */