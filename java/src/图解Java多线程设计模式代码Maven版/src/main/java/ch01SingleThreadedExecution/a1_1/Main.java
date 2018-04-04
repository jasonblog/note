package ch01SingleThreadedExecution.a1_1;

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
Chris BEGIN
Bobby BEGIN
Alice BEGIN
***** BROKEN ***** No.3: Alice, Canada
***** BROKEN ***** No.4: Chris, Brazil
***** BROKEN ***** No.4: Chris, Brazil
***** BROKEN ***** No.6: Alice, Canada
***** BROKEN ***** No.7: Chris, Brazil
***** BROKEN ***** No.7: Chris, Brazil
***** BROKEN ***** No.9: Alice, Canada
***** BROKEN ***** No.10: Chris, Brazil
***** BROKEN ***** No.10: Chris, Brazil
***** BROKEN ***** No.12: Alice, Canada
***** BROKEN ***** No.13: Chris, Brazil
***** BROKEN ***** No.13: Chris, Brazil
***** BROKEN ***** No.15: Alice, Canada
***** BROKEN ***** No.16: Chris, Alaska
***** BROKEN ***** No.16: Alice, Brazil
 */