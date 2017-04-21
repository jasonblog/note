class StructPass
{
    public native void toNativeStruct();
    public native void fromNativeStruct();
    public native void initNative();

    private String name;
    private int age;
    private float height;

    static {
        System.loadLibrary("jni");
    }

    public StructPass(String n, int a, float h)
    {
        name = n;
        age = a;
        height = h;
    }

    public static void main(String[] args)
    {
        StructPass hello = new StructPass(new String("kerker"), 10, 180);
        hello.initNative();
        hello.toNativeStruct();
        hello.fromNativeStruct();
        System.out.println("In Java:");
        System.out.println("  hello.name = \"" + hello.name + "\"");
        System.out.println("  hello.age = "  + hello.age);
        System.out.println("  hello.height = " + hello.height);
    }
}
