package ch05;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;

public class Ch05_3_3ContrastUseBitwiseNot
{
    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        Mat img = Highgui.imread("C://opencv3.1//samples//lena.jpg");
        Core.bitwise_not(img, img);
        Highgui.imwrite("C://opencv3.1//samples//lena-contrast3.jpg", img);
    }
}
