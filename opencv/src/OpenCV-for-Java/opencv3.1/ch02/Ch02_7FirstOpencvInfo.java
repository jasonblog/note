package ch02;

import org.opencv.core.Core;

public class Ch02_7FirstOpencvInfo
{

    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        System.out.println("Version:" + Core.VERSION);
        System.out.println("NATIVE_LIBRARY_NAME:" + Core.NATIVE_LIBRARY_NAME);
        //編譯資訊
        //System.out.println("cmake info:" + Core.getBuildInformation());

    }

}
