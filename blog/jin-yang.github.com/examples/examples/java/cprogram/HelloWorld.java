/* HelloWorld.java */
public class HelloWorld {
    native void helloFromC();        /* (1) */
    static {
        System.loadLibrary("ctest"); /* (2) */
    }
    static public void main(String argv[]) {
        HelloWorld helloWorld = new HelloWorld();
        helloWorld.helloFromC();     /* (3) */
    }
}
